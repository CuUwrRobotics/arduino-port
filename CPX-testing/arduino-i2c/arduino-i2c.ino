// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup()
{
  Wire.begin(0x3C);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(115200);           // start serial for output
}

void loop() {
  //  while (0 > Wire.available()) {
  delay(1000);
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  //  }
  //  delay(50);
  //  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  //  delay(50);
  //  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));



}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while (0 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    if (c == '\n') Serial.println("\\n");
    else Serial.print(c);         // print the character
    if (Wire.available() == 0)
      Serial.println(); // That was the last one.
  }
  Wire.write("hello back. ");
}
