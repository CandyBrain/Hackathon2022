#include <ArduinoBLE.h>



//Mux control pins
int S0 = 20;//D2
int S1 = 21;//D3
int S2 = 22;//D4
int S3 = 23;//D5
int TX = 16;
int RX = 17;
const int NUM_OF_CH = 16;
int controlPin[] = {S0, S1, S2, S3}; 
int sensor_data[NUM_OF_CH];


int stop1 = 0;
int ant = 0;

//tx 보내는 핀 rx 받는 핀
//Mux in “SIG” pin
int SIG_pin = A0;//모든 아날로그 인풋이 sig_pin 통해 옴. ADC PIN DECODER OUTPUT A7 은 디지털 핀으로 사용가능하다.

 // BLE AR_data Service
BLEService AR1_Service("180F");

// BLE AR_Level Characteristic
BLEUnsignedCharCharacteristic AR1_LevelChar("2A19",  // standard 16-bit characteristic UUID
    BLERead | BLENotify); // remote clients will be able to get notifications if this characteristic changes

int old_AR_Level = 0;  // last AR_Level reading from analog input
long previousMillis = 0;  // last time the AR_Level was checked, in ms

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  Serial.begin(115200);    // initialize serial communication
  while (!Serial);

 // pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  /* Set a local name for the BLE device
     This name will appear in advertising packets
     and can be used by remote devices to identify this BLE device
     The name can be changed but maybe be truncated based on space left in advertisement packet
  */
  BLE.setLocalName("AR1");
  BLE.setAdvertisedService(AR1_Service); // add the service UUID
  AR1_Service.addCharacteristic(AR1_LevelChar); // add the AR_Level characteristic
  BLE.addService(AR1_Service); // Add the AR_data service
  AR1_LevelChar.writeValue(old_AR_Level); // set initial value for this characteristic

  /* Start advertising BLE.  It will start continuously transmitting BLE
     advertising packets and will be visible to remote BLE central devices
     until it receives a new connection */

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

//  **************************************************************

void loop_ar(){ 
   int AR_dataLevel;
 
    for(int i = 0; i < NUM_OF_CH; i ++){
      sensor_data[i] = readMux(i);
    }//16은 채널 개수 
   
    for(int i = 0; i < NUM_OF_CH ; i ++){
     AR_dataLevel=sensor_data[i];
     Serial.print(' ');
     Serial.print(AR_dataLevel); //comma is used as a delimiter.
     Serial.print(","); //comma is used as a delimiter.
     AR1_LevelChar.writeValue(AR_dataLevel);  // and update the AR_Level characteristic
     if(i==15){
        Serial.println("2");
        }
     }  
      Serial.println(" ");//end of line, 초기화;
      delay(15); // 1000 milli seconds delay 1초 지연
    }

    
int readMux(int channel){
            int muxChannel[NUM_OF_CH][4]={ //4는 디지털 컨트롤 핀 개수 의미 (MUX을 제어하는)  16은 채널개수
            {0,0,0,0},
            {1,0,0,0}, //channel 1 
            {0,1,0,0}, //channel 2 
            {1,1,0,0}, //channel 3 
            {0,0,1,0}, //channel 4 
            {1,0,1,0}, //channel 5 
            {0,1,1,0}, //channel 6 
            {1,1,1,0}, //channel 7 
            {0,0,0,1}, //channel 8 
            {1,0,0,1}, //channel 9 
            {0,1,0,1}, //channel 10 
            {1,1,0,1}, //channel 11 
            {0,0,1,1}, //channel 12 
            {1,0,1,1}, //channel 13 
            {0,1,1,1}, //channel 14 
            {1,1,1,1} //channel 15 
            }; //loop through the 4 sig 
            for(int i = 0; i < 4; i ++){ //config 4 digital out pins of mux IC.
              digitalWrite(controlPin[i], muxChannel[channel][i]);
            } //read the value at the SIG pin 
            int val = analogRead(SIG_pin); // 센서값 읽어오기 
            return val;
    }
             



//  ***************************************************************
void loop() {
  // wait for a BLE central
  BLEDevice central = BLE.central();
 

  // if a central is connected to the peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);
    
    // check the AR_Level every 200ms
    // while the central is connected:
    while (central.connected()) { // 연결시마다 
      long currentMillis = millis();
      // if 200ms have passed, check the AR_Level:
      if (currentMillis - previousMillis >= 300) { // 200ms 마다 AR_level 체크 및 loop_ar 함수 돌림(값 받아오는) 
        previousMillis = currentMillis;
        loop_ar();
      // updateAR_dataLevel();
      }
    }
    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}


void updateAR_dataLevel() {
  /* Read the current voltage level on the A0 analog input pin.
     This is used here to simulate the charge level of a AR_data.
  */
  int AR_data = analogRead(A0);
  int AR_dataLevel = map(AR_data, 0, 1023, 0, 100); //map(AR_data, 0, 1023, 0, 100);

  if (AR_dataLevel != old_AR_Level) {      // if the AR_Level has changed
    Serial.print("AR_Level % is now: "); // print it
    Serial.println(AR_dataLevel);
    AR1_LevelChar.writeValue(AR_dataLevel);  // and update the AR_Level characteristic
    old_AR_Level = AR_dataLevel;           // save the level for next comparison
  }
}

 
