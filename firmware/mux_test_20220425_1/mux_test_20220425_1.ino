#include <ArduinoBLE.h>

// ###############################
// ### variable initialization ###
// ###############################

// switch
int val=1;

// Mux_1 control pins
int s0_1 = 2;
int s1_1 = 3;
int s2_1 = 4;
int s3_1 = 5;
int en_1 = 6;

//Mux_1 in “SIG” pin
int SIG_1_pin = 14;


// Mux_2 control pins
int s0_2 = 7;
int s1_2 = 8;
int s2_2 = 9;
int s3_2 = 10;
int en_2 = 11;



// the loop function runs over and over again forever
void setup() {
  pinMode(s0_1, OUTPUT);
  pinMode(s1_1, OUTPUT);
  pinMode(s2_1, OUTPUT);
  pinMode(s3_1, OUTPUT);
  pinMode(en_1, OUTPUT);
  digitalWrite(en_1, HIGH);  //Mux disable
  
  //pinMode(12, OUTPUT);
  //pinMode(21, INPUT);

  Serial.begin(115200);
}
void loop(){
  // wait switch on
  
  Serial.print("시작버튼을 눌러주세요");
  while(val == 1)
  {
    val = digitalRead(21);
    //Serial.println(val);
  }


  // Mux_1 control
  digitalWrite(s0_1, LOW);
  digitalWrite(s1_1, LOW);
  digitalWrite(s2_1, LOW);
  digitalWrite(s3_1, LOW);
  digitalWrite(en_1, LOW);

  while(1){
     int AR_data = analogRead(SIG_1_pin);
     delay(100);
     Serial.println(AR_data);
  }
} 
