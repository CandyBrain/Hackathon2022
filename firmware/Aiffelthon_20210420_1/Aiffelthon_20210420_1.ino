#include <Arduino.h>
#include <ArduinoBLE.h>

void setup() {
  // 시리얼 통신 초기화 
  
  Serial.begin(115200);
    while (!Serial);
    pinMode(LED_BUILTIN, OUTPUT);
    
    // begin initialization
    if (!BLE.begin()) {
      Serial.println("starting BLE failed!");
      while(1);
    }
}

void loop() {
  // put your main code here, to run repeatedly:

}
