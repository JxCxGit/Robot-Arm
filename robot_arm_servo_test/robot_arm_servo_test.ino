#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Create PCA9685 driver object
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Pulse width range in microseconds
#define SERVOMIN  500    // 0 degrees
#define SERVOMAX  2500   // 180 degrees

// Servo channels (as requested)
#define SERVO_10 0  // Base
#define SERVO_11 1  // Shoulder
#define SERVO_12 2  // Elbow
#define SERVO_13 3  // Wrist
#define SERVO_14 4  // Wrist rotate
#define SERVO_15 5  // Gripper

// Converts angle (0–180) to 12-bit PWM value
uint16_t angleToPulse(int angle) {
  int pulse_us = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  return (uint16_t)((float)pulse_us / 1000000 * 50 * 4096); // 50Hz cycle
}

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(50);  // Standard servo frequency
  delay(10);
}

void loop() {
  // Sweep servos 10 to 14 (0–180°)
  for (int angle = 0; angle <= 180; angle += 10) {
    for (int ch = SERVO_10; ch <= SERVO_14; ch++) {
      pwm.setPWM(ch, 0, angleToPulse(angle));
    }
    delay(200);
  }

  for (int angle = 180; angle >= 0; angle -= 10) {
    for (int ch = SERVO_10; ch <= SERVO_14; ch++) {
      pwm.setPWM(ch, 0, angleToPulse(angle));
    }
    delay(200);
  }

  // Gripper sweep (65–120° only)
  for (int angle = 65; angle <= 120; angle += 5) {
    pwm.setPWM(SERVO_15, 0, angleToPulse(angle));
    delay(150);
  }

  for (int angle = 120; angle >= 65; angle -= 5) {
    pwm.setPWM(SERVO_15, 0, angleToPulse(angle));
    delay(150);
  }
}
