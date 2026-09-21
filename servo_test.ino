#include <ESP32Servo.h>
// 180 degree positional Servo Motors 

Servo servo1;  // create Servo object to control a servo

int pos = 0;    // variable to store the servo position

void setup() {
  servo1.attach(1);  // attaches the servo on pin GPIO1/A0/D0 of the XIAO ESP32-S3
  servo1.write(pos);
  delay(5000);

}

void loop() {
  // Test slowly increasing pulse widths to see where it stops responding
  for (int pos = 0; pos <= 180; pos += 1) {
    servo1.write(pos);
    delay(5);
  }

  for (int pos = 180; pos >= 0; pos -= 1) {
    servo1.write(pos);
    delay(5);
  }
}
