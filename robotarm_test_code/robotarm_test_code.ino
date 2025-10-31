#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  500
#define SERVOMAX  2500

#define SERVO_COUNT 6
int servoChannels[SERVO_COUNT] = {0, 1, 2, 3, 4, 5}; // 6 servos: base–gripper

// Convert angle (0–180°) → PCA9685 pulse value
uint16_t angleToPulse(int angle) {
  int pulse_us = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  return (uint16_t)((float)pulse_us / 1000000 * 50 * 4096);
}

void setup() {
  Serial.begin(38400);
  pwm.begin();
  pwm.setPWMFreq(50);
  delay(500);
  Serial.println("READY");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.startsWith("MOVE")) {
      // Format: MOVE a1,a2,a3,a4,a5,a6 T=xxxx
      int T_index = cmd.indexOf("T=");
      String angles_str = cmd.substring(5, T_index);
      angles_str.trim();

      int angles[SERVO_COUNT];
      for (int i = 0; i < SERVO_COUNT; i++) {
        int comma = angles_str.indexOf(',');
        if (comma == -1) comma = angles_str.length();
        angles[i] = angles_str.substring(0, comma).toInt();
        angles_str.remove(0, comma + 1);
      }

      int duration_ms = cmd.substring(T_index + 2).toInt();
      moveServos(angles, duration_ms);
      Serial.println("OK");
    }

    else if (cmd.startsWith("GRIP")) {
      if (cmd.indexOf("OPEN") != -1) {
        pwm.setPWM(servoChannels[5], 0, angleToPulse(70)); // open
      } else {
        pwm.setPWM(servoChannels[5], 0, angleToPulse(120)); // close
      }
      delay(500);
      Serial.println("OK");
    }

    else if (cmd == "HOME") {
      int homeAngles[SERVO_COUNT] = {90, 90, 90, 90, 90, 90};
      moveServos(homeAngles, 1000);
      Serial.println("OK");
    }
  }
}

void moveServos(int targetAngles[], int duration_ms) {
  // Simple direct move (no interpolation for test)
  for (int i = 0; i < SERVO_COUNT; i++) {
    pwm.setPWM(servoChannels[i], 0, angleToPulse(targetAngles[i]));
  }
  delay(duration_ms);
}
