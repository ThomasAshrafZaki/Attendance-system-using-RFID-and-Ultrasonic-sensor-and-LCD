

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#define SS_PIN 10
#define RST_PIN 9
#define Buzzer 5
const int trigPin1 = 14; // Trigger pin of 1st ultrasonic sensor
const int echoPin1 = 15; // Echo pin of 1st ultrasonic sensor
const int trigPin2 = 16; // Trigger pin of 2nd ultrasonic sensor
const int echoPin2 = 17; // Echo pin of 2nd ultrasonic sensor
int entering = 0;
int exiting = 0;
int totalInRoom = 0;
const int rs = 6, en = 7, d4 = 8, d5 = 2, d6 = 3, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
bool Flag=0;
int i =8;
int doctor = 0;

void setup() 
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  SPI.begin();      // Initiate  SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  pinMode(Buzzer,OUTPUT);

}
void loop() 
{
 while (Flag)
 {
    int distance1 = getDistance(trigPin1, echoPin1);
  int distance2 = getDistance(trigPin2, echoPin2);

  if (distance1 < 5)
   { // Adjust this value based on your setup
    entering++;
    totalInRoom++;
     if (doctor == 1)
    {
      entering = 0;
      totalInRoom = 0;
      doctor = 0;


    }
    else
    {
      digitalWrite(Buzzer,HIGH);
      delay(30);
      digitalWrite(Buzzer,LOW);

    }
    lcdUpdate();
    delay(1200); // Adjust this delay as needed to avoid multiple counts for a single entry
  }

  if (distance2 < 5) { // Adjust this value based on your setup
    exiting++;
    totalInRoom--;
    digitalWrite(Buzzer,HIGH);
    delay(30);
    digitalWrite(Buzzer,LOW);

    lcdUpdate();
    delay(1200); // Adjust this delay as needed to avoid multiple counts for a single exit
  }
  rfid1();

 }
 rfid1();
  
} 

void rfid1()
{
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
  String UID="";
  for (byte i=0;i<rfid.uid.size;i++) 
  {
     Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");// E8 07 C7 D2
     Serial.print(rfid.uid.uidByte[i], HEX);
     UID.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ")); // 0E c2 12 
     UID.concat(String(rfid.uid.uidByte[i], HEX));
  }
  UID.toUpperCase();
  // E8 07 C7 D2
  if (UID.substring(1) == "90 31 A6 20" && Flag == 0) //change here the UID of the card/cards that you want to give access
  {
    lcd.clear();
    lcd.print("Welcome ");
    delay(300);
      lcd.print("Dr/");
      lcd.setCursor(0, 1);
      lcd.print("Mohammed Khamis");
      lcd.setCursor(3, 1);
      UID.substring(1) = "0";
      Flag ^= 1;
      doctor = 1;
      digitalWrite(Buzzer,HIGH);
      delay(100);
      digitalWrite(Buzzer,LOW);
      delay(50);
      digitalWrite(Buzzer,HIGH);
      delay(100);
      digitalWrite(Buzzer,LOW);
      
  }
  else if (UID.substring(1) == "90 31 A6 20" && Flag == 1)
  {
    lcd.clear();
      UID.substring(1) = "0";
      Flag ^= 1;
      totalInRoom = 0;
      exiting = 0;
      entering = 0;
  }
  else
  {
    lcd.clear();
    lcd.print("Access denied");
    digitalWrite(Buzzer,HIGH);
    delay(1500);
    digitalWrite(Buzzer,LOW);
  }
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}
}
int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH) / 58.0;
}
void lcdUpdate() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter:");
  lcd.print(entering);
  lcd.print(" Exit:");
  lcd.print(exiting);
  lcd.setCursor(0, 1);
  lcd.print("Total: ");
  lcd.print(totalInRoom);
}
