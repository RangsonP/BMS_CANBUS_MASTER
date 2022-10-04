#include <Arduino.h>
#include <math.h>
#include "Function.h"
#include "CANID.h"
#include "Variables.h"
#include "CANStatus.h"
#include "Pins.h"
#include "DataConv.h"
#include <SerialMessage.h>
#include <Preferences.h>

#include "driver/gpio.h" // only use in board ESP32
#include "driver/can.h" // only use in board ESP32


void setup() {
  Serial.begin(115200);
  setup_can_driver();
  setIDx();  // Set canbus id to default
  getIDx();
}

void loop() {
  readCANmessageSerial();
  // Wait for set mode request
  getCommand();
}