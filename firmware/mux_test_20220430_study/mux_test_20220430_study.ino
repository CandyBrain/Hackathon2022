// ###############################
// ### variable initialization ###
// ###############################


// Mux1 control pins
int s0_1 = 2;
int s1_1 = 3;
int s2_1 = 4;
int s3_1 = 5;

//Mux in “SIG” pin (Mux1, Mux2 둘다 공통)
int SIG_1_pin = 14; //  (A0)

void setup() {
  // pin initialization
  // Mux1 control pin init
  pinMode(s0_1, OUTPUT);
  pinMode(s1_1, OUTPUT);   
  pinMode(s2_1, OUTPUT);
  pinMode(s3_1, OUTPUT);
 
  // Serial init
  Serial.begin(115200);
  while (!Serial);

  // channel select
  digitalWrite(s0_1, HIGH);  // Mux1 disable
  digitalWrite(s1_1, LOW);  // Mux2 disable
  digitalWrite(s2_1, HIGH);  // Mux3 disable
  digitalWrite(s3_1, HIGH);  // Mux4 disable
}


void loop(){

    int val2 = analogRead(SIG_1_pin);    
    Serial.print(val2);
    Serial.print(",");
    delay(10);
    
    Serial.println("");
}
