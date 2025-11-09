#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// PCA9685 default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  100     // pulse count for 0 degrees
#define SERVOMAX  600     // pulse count for 180 degrees

void moveServoSmooth(uint8_t servo, int fromPos, int toPos, int step = 3) {
  if (fromPos < toPos) {
    for (int pos = fromPos; pos <= toPos; pos += step) {
      pwm.setPWM(servo, 0, pos);
      delay(35);
    }
  } else {
    for (int pos = fromPos; pos >= toPos; pos -= step) {
      pwm.setPWM(servo, 0, pos);
      delay(35);
    }
  }
}

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~50–60 Hz

  homePose();
  delay(1500);
}

void homePose() {
  moveServoSmooth(0, 440, 440);   // Base
  moveServoSmooth(1, 350, 350);   // Shoulder
  moveServoSmooth(2, 490, 490);
  moveServoSmooth(3, 190, 190); 
  moveServoSmooth(5, 100, 100);   // Gripper
}

void pickObject() {
  moveServoSmooth(0, 440, 350);   // align base
  moveServoSmooth(2, 490, 550);   // move down
  moveServoSmooth(3, 190, 100);   // wrist down
  moveServoSmooth(5, 100, 100);   // open gripper
  delay(800);
  moveServoSmooth(5, 100, 230);
  delay(800);
  moveServoSmooth(3, 100, 350);   // lift wrist back
}

void dropRed() {
  moveServoSmooth(0, 350, 600);   // rotate to red bin
  moveServoSmooth(3, 350, 100);   // drop wrist
  moveServoSmooth(5, 230, 100);   // open gripper
}

void dropWhite() {
  moveServoSmooth(0, 350, 570);   // rotate to white bin
  moveServoSmooth(3, 350, 250);
  moveServoSmooth(5, 230, 100);
  moveServoSmooth(3, 250, 350);
}

void dropBlue() {
  moveServoSmooth(0, 350, 175);   // rotate to blue bin
  moveServoSmooth(3, 350, 100);
  moveServoSmooth(5, 230, 100);
}

void loop() {
  if (Serial.available()) {
    String colour = Serial.readStringUntil('\n');
    colour.trim();

    Serial.print("Received: ");
    Serial.println(colour);

    pickObject();
    delay(1000);

    if (colour == "red") {
      dropRed();
    } else if (colour == "blue") {
      dropBlue();
    } else if (colour == "white") {
      dropWhite();
    }

    homePose();
    delay(800);

    Serial.println("READY");
  }
}
