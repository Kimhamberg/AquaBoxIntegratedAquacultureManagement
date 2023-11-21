
#include <Servo.h>
Servo myservo;
int pos = 0; 
const int E1 = 3; ///<Motor1 fart
const int E2 = 11;///<Motor2 fart 
const int E3 = 5; ///<Motor3 fart 
const int E4 = 6; ///<Motor4 fart 
const int M1 = 4; ///<Motor1 Retning 
const int M2 = 12;///<Motor2 Retning 
const int M3 = 8; ///<Motor3 Retning 
const int M4 = 7; ///<Motor4 Retning 

void M1_back(char Speed) // Definerer Motor1 kjør bak 
{ 
digitalWrite(M1,LOW); 
analogWrite(E1,Speed); 
} 
void M2_back(char Speed) // Definerer Motor2 kjør bak 
{ 
digitalWrite(M2,HIGH); 
analogWrite(E2,Speed); 
} 
void M3_back(char Speed) // Definerer Motor3 kjør bak 
{ 
digitalWrite(M3,LOW); 
analogWrite(E3,Speed); 
} 
void M4_back(char Speed) // Definerer Motor4 kjør bak
{ 
digitalWrite(M4,HIGH); 
analogWrite(E4,Speed); 
} 

void M1_advance(char Speed) // Definerer Motor1 kjør frem 
{ 
digitalWrite(M1,HIGH); 
analogWrite(E1,Speed); 
} 
void M2_advance(char Speed) // Definerer Motor2 kjør frem 
{ 
digitalWrite(M2,LOW); 
analogWrite(E2,Speed); 
}  
void M3_advance(char Speed) // DefinererMotor3 kjør frem 
{ 
digitalWrite(M3,HIGH); 
analogWrite(E3,Speed); 
} 
void M4_advance(char Speed) // Definerer Motor4 kjør frem
{ 
digitalWrite(M4,LOW); 
analogWrite(E4,Speed); 
}

void pauseaction(char Speed) //Definerer at motorene skal stoppe
{
analogWrite(E1,0);
analogWrite(E2,0);
analogWrite(E3,0);
analogWrite(E4,0);
}

void setup() { 
for(int i=3;i<9;i++) 
pinMode(i,OUTPUT); 
for(int i=11;i<13;i++) 
pinMode(i,OUTPUT);


Serial.begin(9600);
myservo.attach(10); // Pin hvor motoren er koblet til
myservo.write(90);
} 
 
void loop() { 
  unsigned long endTime= millis() + 2000; //Bilen kjører frem i 2s
  while (true) {
    if (millis() >=endTime){
      break;
    }
    Serial.print("Skal kjore frem.");
    M1_back(100); 
    M2_back(100); 
    M3_back(100);   
    M4_back(100);
  }
    
  unsigned long endTime1 = millis()+5000; //Bilen stopper i 5 sek 
  while (true) {
    if (millis() >= endTime1){
      break;
    }
    Serial.println("Should be pausing.");
    pauseaction(100);
    unsigned long endTimev = millis()+2000; //Vinsjen heises ned i 2 sek
    while (true){
      if (millis()>= endTimev){
        break;
      }
      Serial.println("Heis ned");
      myservo.write(0);
    }
    unsigned long endTimes = millis()+2000; //Vinsjen står stille i bunnpunktet i 2 sek
    while (true){
      if (millis()>= endTimes){
        break;
      }
      Serial.println("Pause");
      myservo.write(90);
    }
    unsigned long endTimev1 = millis()+2000; //Vinsjen heises opp igjen 
    while (true){
      if (millis()>= endTimev1){
        break;
      }
      Serial.println("Heis opp");
      myservo.write(180);
    }
  } 

  unsigned long endtime_f_panytt = millis() + 1000; //Venter til at vinjen er heist opp igjen før bilen kjører
  while (true) {
    if (millis() >= endtime_f_panytt){
      break;
    }
    Serial.println("Stop for kjoring");
    myservo.write(90);
  }

  
  unsigned long endTime2 = millis() + 2200; //Bilen kjører frem igjen
  while (true) {
    if (millis() >= endTime2 ) {
      break;
    }
      Serial.println("Skal kjore frem pa nytt.");
      M1_back(100); 
      M2_back(100); 
      M3_back(100); 
      M4_back(100); 
    }


    unsigned long endTime3 = millis() + 4000; //Bilen kjører bak
    while (true) {
      if (millis() >= endTime3) {
        break;
    }
      Serial.println("Skal kjore bak.");
      M1_advance(100); 
      M2_advance(100); 
      M3_advance(100); 
      M4_advance(100);  
    }
  
} 