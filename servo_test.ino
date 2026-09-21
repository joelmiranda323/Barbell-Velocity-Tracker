#include <ESP32Servo.h>

Servo servo1;  // create Servo object to control a servo
// twelve Servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  servo1.attach(1);  // attaches the servo on pin 9 to the Servo object
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