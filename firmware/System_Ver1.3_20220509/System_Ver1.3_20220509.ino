#include <ArduinoBLE.h>

// ###############################
// ### variable initialization ###
// ###############################

int result[1000];
int count=0;
unsigned long time;
unsigned long previous_time;
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

//int fsrReading, fsrVoltage;

void setup() {
  // pin initialization
  //pinMode(LED_PIN, OUTPUT);  // builtin LED

  // Mux1, Mux3 control pin init
  pinMode(s0_1, OUTPUT);
  pinMode(s1_1, OUTPUT);
  pinMode(s2_1, OUTPUT);
  pinMode(s3_1, OUTPUT);

  // Mux2, Mux4 control pin init
  pinMode(s0_2, OUTPUT);
  pinMode(s1_2, OUTPUT);
  pinMode(s2_2, OUTPUT);
  pinMode(s3_2, OUTPUT); 

  // Mux enable pin init
  pinMode(en_1, OUTPUT);
  pinMode(en_2, OUTPUT);
  pinMode(en_3, OUTPUT);
  pinMode(en_4, OUTPUT);  
 
  digitalWrite(en_1, HIGH);  // Mux1 disable
  digitalWrite(en_2, HIGH);  // Mux2 disable
  digitalWrite(en_3, HIGH);  // Mux3 disable
  digitalWrite(en_4, HIGH);  // Mux4 disable    

  //analogReference(3.3);

  // Serial init
  Serial.begin(115200);
  while (!Serial);
}


void loop(){
  // wait switch on
  while(val == 1)
  {
    val = digitalRead(21);
    delay(10);
    //Serial.println("시작버튼을 눌러주세요");
  }

  // 지정시간 체크
  time = millis();
  //Serial.print(time);
  //Serial.print(",");
  Serial.print("DATA");
  Serial.print(",");

  //Serial.print("Mux1");
  //Serial.print(",");
  if ( millis() - previous_time > 180000){
  previous_time = millis();  
  Serial.println (previous_time);
  Serial.println("측정시간 완료"); 
  val = 1; // 지정시간 이후 대기상태로
  }
  
  else{

  // Mux1
  selMux1();  
  
  for(int i = 0; i < 16; i ++){
    int fsrReading = readMux_1_3(i);
    int fsrVoltage = map(fsrReading, 0, 1023, 0, 3300);

    // cal FSR resistance
    unsigned long fsrResistance = cal_fsrResistance(fsrVoltage);

    if ( fsrReading < 16) {
      fsrReading = 0;
    }
    
    //Serial.print(fsrResistance);
    Serial.print(fsrReading);
    //Serial.print(fsrVoltage);
    
    Serial.print(",");
    }
  
  // Mux2
  selMux2();

  for(int i = 0; i < 16; i ++){
    int fsrReading = readMux_2_4(i);
    int fsrVoltage = map(fsrReading, 0, 1023, 0, 3300);

    // cal FSR resistance
    unsigned long fsrResistance = cal_fsrResistance(fsrVoltage);

    if ( fsrReading < 16) {
      fsrReading = 0;
    }
    
    //Serial.print(fsrResistance);
    Serial.print(fsrReading);
    //Serial.print(fsrVoltage);

    Serial.print(",");
    }


  // Mux3
  selMux3();

  for(int i = 0; i < 16; i ++){
    int fsrReading = readMux_1_3(i);
    int fsrVoltage = map(fsrReading, 0, 1023, 0, 3300);

    // cal FSR resistance
    unsigned long fsrResistance = cal_fsrResistance(fsrVoltage);

    if ( fsrReading < 16) {
      fsrReading = 0;
    }
    
    //Serial.print(fsrResistance);
    Serial.print(fsrReading);
    //Serial.print(fsrVoltage);

    Serial.print(",");
    }


  // Mux4
  selMux4();

  for(int i = 0; i < 16; i ++){
    //int fsrReading = readMux_2_4(i); 
    int fsrReading = 0; // 현재 mux4의 센서가 플로팅된 상태이다. 일단 모두 0으로 입력 추후 변경할 것 
    int fsrVoltage = map(fsrReading, 0, 1023, 0, 3300);

    // cal FSR resistance
    unsigned long fsrResistance = cal_fsrResistance(fsrVoltage);

    if ( fsrReading < 16) {
      fsrReading = 0;
    }
    
    //Serial.print(fsrResistance);
    Serial.print(fsrReading);
    //Serial.print(fsrVoltage);

    Serial.print(","); // 줄바꿈
    }
    Serial.println(""); // 데이터 다 받고 전체 줄바꿈
    
    //Serial.println(previous_time); // 시간 디스플레이
  }
}  
  
  // only Mux1 enable
  void selMux1(){
    // Mux1
    digitalWrite(en_1, LOW);   // Mux1 enable
    digitalWrite(en_2, HIGH);  // Mux2 disable
    digitalWrite(en_3, HIGH);  // Mux3 disable
    digitalWrite(en_4, HIGH);  // Mux4 disable
  }

  // only Mux2 enable
  void selMux2(){
    // Mux1
    digitalWrite(en_1, HIGH);  // Mux1 disable
    digitalWrite(en_2, LOW);   // Mux2 enable
    digitalWrite(en_3, HIGH);  // Mux3 disable
    digitalWrite(en_4, HIGH);  // Mux4 disable
  }

  // only Mux3 enable
  void selMux3(){
    // Mux1
    digitalWrite(en_1, HIGH);  // Mux1 disable
    digitalWrite(en_2, HIGH);  // Mux2 disable
    digitalWrite(en_3, LOW);   // Mux3 enable
    digitalWrite(en_4, HIGH);  // Mux4 disable
  }

  // only Mux4 enable
  void selMux4(){
    // Mux1
    digitalWrite(en_1, HIGH);  // Mux1 disable
    digitalWrite(en_2, HIGH);  // Mux2 disable
    digitalWrite(en_3, HIGH);  // Mux3 disable
    digitalWrite(en_4, LOW);   // Mux4 enable
  }    


  // FSR resistance
  unsigned long cal_fsrResistance(int tmp_fsrVoltage) {
      unsigned long tmp_fsrResistance = 3300 - tmp_fsrVoltage;
      tmp_fsrResistance *= 3000;
      tmp_fsrResistance /= tmp_fsrVoltage;

      return tmp_fsrResistance;
  };



  // 함수선언
  int readMux_1_3(int channel)  { 
    int controlPin[] = {s0_1, s1_1, s2_1, s3_1}; 
    int muxChannel_1_3[16][4]={ 
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
    digitalWrite(controlPin[i], muxChannel_1_3[channel][i]); 
    } 
    //read the value at the SIG pin 
    int val2 = analogRead(SIG_1_pin);
    delay(12);
    return val2;
    }


  int readMux_2_4(int channel)  { 
    int controlPin[] = {s0_2, s1_2, s2_2, s3_2}; 
    int muxChannel_2_4[16][4]={ 
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
    digitalWrite(controlPin[i], muxChannel_2_4[channel][i]); 
    } 
    //read the value at the SIG pin 
    int val2 = analogRead(SIG_1_pin);
    return val2; 
    }
