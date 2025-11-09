import cv2 as cv
import numpy as np
import serial, time

STREAM_URL = "http://192.168.0.58:81/stream"

PORT = "COM4"
BAUDE_RATE = 9600

TOP_CROP = 160
BOTTOM_CROP = 280
SIDE_CROP = 420

cap = cv.VideoCapture(STREAM_URL)
ser = serial.Serial(PORT, BAUDE_RATE, timeout=1)
time.sleep(2)

found = False

def get_object(mask, frame, colour):
    contours, _ = cv.findContours(mask, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)
    for contour in contours:
        area = cv.contourArea(contour)

        if area > 1000:
            x, y, w, h = cv.boundingRect(contour)
            frame = cv.rectangle(frame, (x, y), (x + w, y + h),
                                 (0, 0, 255), 2)
            
            cv.putText(frame, f"{colour} Colour", (x, y), 
                       cv.FONT_HERSHEY_SIMPLEX, 1.0, 
                        (0, 0, 255))
            
            return True
    
    return False
            

while True:
    ret, frame = cap.read()
    if not ret:
        print("error: failed to capture video")
        break

    h, w, _ = frame.shape
    cropped = frame[TOP_CROP:h - BOTTOM_CROP, SIDE_CROP:w - SIDE_CROP]

    hsv_frame = cv.cvtColor(cropped, cv.COLOR_BGR2HSV)

    red_lower = np.array([136, 87, 50])
    red_upper = np.array([180, 255, 255])
    red_mask = cv.inRange(hsv_frame, red_lower, red_upper)

    blue_lower = np.array([25, 120, 140])
    blue_upper = np.array([102, 255, 255])
    blue_mask = cv.inRange(hsv_frame, blue_lower, blue_upper)

    white_lower = np.array([0, 0, 150], np.uint8)
    white_upper = np.array([135, 25, 255], np.uint8)
    white_mask = cv.inRange(hsv_frame, white_lower, white_upper)

    red_found = get_object(red_mask, cropped, "Red")
    blue_found = get_object(blue_mask, cropped, "Blue")
    white_found = get_object(white_mask, cropped, "White")

    colour_to_send = None 

    if not found: 
        if red_found:
            colour_to_send = b'red\n'
        elif blue_found:
            colour_to_send = b'blue\n'
        elif white_found:
            colour_to_send = b'white\n'

        if colour_to_send:
            print(f"Sending command: {colour_to_send.decode().strip()}")
            ser.write(colour_to_send)
            print("Waiting for Arduino to finish...")
            found = True         
    
    elif found:
        response = ser.readline().decode(errors="ignore").strip()
        if "READY" in response:
            print("Arduino is READY. Detecting next object.")
            ser.reset_input_buffer()
            found = False
      

    cv.imshow("Object Detector", cropped)
    if cv.waitKey(1) == 27: 
        break

cap.release()
cv.destroyAllWindows()