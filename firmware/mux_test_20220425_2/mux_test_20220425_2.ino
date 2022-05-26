#include <ArduinoBLE.h>

// ###############################
// ### variable initialization ###
// ###############################

int result[]={};
int count=0;


// switch
int val=1;

// Mux1 control pins
int s0_1 = 2;
int s1_1 = 3;
int s2_1 = 4;
int s3_1 = 5;
int en_1 = 6;

// Mux_2 control pins
int s0_2 = 7;
int s1_2 = 8;
int s2_2 = 9;
int s3_2 = 10;
int en_2 = 11;

// Mux_3 control pins
int s0_3 = 2;
int s1_3 = 3;
int s2_3 = 4;
int s3_3 = 5;
int en_3 = 12;

// Mux_4 control pins
int s0_4 = 7;
int s1_4 = 8;
int s2_4 = 9;
int s3_4 = 10;
int en_4 = 13;


//Mux in “SIG” pin (Mux1, Mux2 둘다 공통)
int SIG_1_pin = 14; //  (A0)
//int SIG_2_pin = 15; // (A1)

const int LED_PIN = LED_BUILTIN;

void setup() {
  // pin initialization
  pinMode(LED_PIN, OUTPUT);  // builtin LED

  // Mux1, Mux3 control pin init
  pinMode(s0_1, OUTPUT);
  pinMode(s1_1, OUTPUT);
  pinMode(s2_1, OUTPUT);
  pinMode(s3_1, OUTPUT);
  pinMode(en_1, OUTPUT);

  // Mux2, Mux4 control pin init
  pinMode(s0_2, OUTPUT);
  pinMode(s1_2, OUTPUT);
  pinMode(s2_2, OUTPUT);
  pinMode(s3_2, OUTPUT);
  pinMode(en_2, OUTPUT); 

  
  // enable pin init
  digitalWrite(en_1, HIGH);  // Mux1 disable
  //digitalWrite(en_2, HIGH);  // Mux2 disable
  //digitalWrite(en_3, HIGH);  // Mux3 disable
  //digitalWrite(en_4, HIGH);  // Mux4 disable    


  // Serial init
  Serial.begin(115200);
  while (!Serial);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.println("Body pressure distribution");
  Serial.println("Label, ch0, ch1, ch2, ch3, ch4"); 
}


void loop(){
  // wait switch on
  while(val == 1)
  {
    val = digitalRead(21);
    delay(10);
    //Serial.println("시작버튼을 눌러주세요");
  }
  digitalWrite(en_1, LOW); // Mux1 enable
  //digitalWrite(en_2, LOW); // Mux2 enable

  Serial.print("DATA");
  for(int i = 0; i < 5; i ++){ 
    //Serial.print("Value at channel "); 
    //Serial.print(i); Serial.print(": "); 
    Serial.print(",");
    Serial.print(readMux_1(i));
    //result[count] = readMux_2(i);
    //count++;
    delay(10);
    }
    Serial.println("");

    //count++;

    //int tmp1 = sizeof(result);
    //Serial.println(tmp1);
    
  //for(int j=0;j<2;j++){
  //  Serial.println(result[j]);    
  //} 
}  

  // 함수선언
  int readMux_1(int channel)  { 
    int controlPin[] = {s0_1, s1_1, s2_1, s3_1}; 
    int muxChannel_1[16][4]={ 
    {0,0,0,0}, //channel 0  / s1
    {1,0,0,0}, //channel 1  / s2
    {0,1,0,0}, //channel 2  / s3
    {1,1,0,0}, //channel 3  / s4
    {0,0,1,0}, //channel 4  / s5
    {1,0,1,0}, //channel 5  / s6
    {0,1,1,0}, //channel 6  / s7
    {1,1,1,0}, //channel 7  / s8
    {0,0,0,1}, //channel 8  / s9
    {1,0,0,1}, //channel 9  / s10
    {0,1,0,1}, //channel 10 / s11
    {1,1,0,1}, //channel 11 / s12
    {0,0,1,1}, //channel 12 / s13
    {1,0,1,1}, //channel 13 / s14
    {0,1,1,1}, //channel 14 / s15
    {1,1,1,1}  //channel 15 / s16
  }; 
  
  //loop through the 4 sig 
  for(int i = 0; i < 4; i ++){ 
    digitalWrite(controlPin[i], muxChannel_1[channel][i]); 
    } 
    //read the value at the SIG pin 
    int val = analogRead(SIG_1_pin);
    return val; 
    }


  int readMux_2(int channel)  { 
    int controlPin[] = {s0_2, s1_2, s2_2, s3_2}; 
    int muxChannel_2[16][4]={ 
    {0,0,0,0}, //channel 0  / s17  
    {1,0,0,0}, //channel 1  / s18 
    {0,1,0,0}, //channel 2  / s19
    {1,1,0,0}, //channel 3  / s20 
    {0,0,1,0}, //channel 4  / s21 
    {1,0,1,0}, //channel 5  / s22 
    {0,1,1,0}, //channel 6  / s23 
    {1,1,1,0}, //channel 7  / s24 
    {0,0,0,1}, //channel 8  / s25 
    {1,0,0,1}, //channel 9  / s26 
    {0,1,0,1}, //channel 10 / s27
    {1,1,0,1}, //channel 11 / s28  
    {0,0,1,1}, //channel 12 / s29  
    {1,0,1,1}, //channel 13 / s30  
    {0,1,1,1}, //channel 14 / s31  
    {1,1,1,1}  //channel 15 / s32  
  }; 
  
  //loop through the 4 sig 
  for(int i = 0; i < 4; i ++){ 
    digitalWrite(controlPin[i], muxChannel_2[channel][i]); 
    } 
    //read the value at the SIG pin 
    int val = analogRead(SIG_1_pin);
    return val; 
    }
