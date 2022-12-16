/*
RFID:
SDA   10
SCK   13
MOSI  11
MISO  12
RST   4

LCD 128x64:
SCK   3
CS    8
SID   9

Ultrasonic:
Echo  7
Trig  6

IR:
IR_Left   5
IR_Right  2

Buttons:
BUT1  14 (A0)
BUT2  15 (A1)
BUT3  16 (A2)
*/

//I2C
#include <Wire.h>
#include <iarduino_I2C_connect.h> 

iarduino_I2C_connect MASTER;

bool LEFT_IR_VAR  = 0;
bool RIGHT_IR_VAR = 0;
byte US_VAR       = 0;
bool BUT1_VAR     = 0;
bool BUT2_VAR     = 0;
bool BUT3_VAR     = 0;
byte RFID_VAR     = 0;

//RFID
#include <SPI.h>
#include <MFRC522.h>

#define SS    10
#define RST   4

MFRC522 rfid(SS, RST);

//LCD 128x64
#include <LCD12864RSPI.h>
#define AR_SIZE(a) sizeof(a) / sizeof(a[0])

unsigned char test_en[]   = "TEST";
unsigned char test_ru[]   = "ТЕСТ";
unsigned char test_rfid[] = "RFID:";

//Ultrasonic
#include <Ultrasonic.h>

#define TRIG  6
#define ECHO  7

Ultrasonic US(TRIG, ECHO);

//IR
#define LEFT_IR   5
#define RIGHT_IR  2

//Buttons
#define BUT1  14
#define BUT2  15
#define BUT3  16

void setup() {
  Wire.begin();

  SPI.begin();
  rfid.PCD_Init();

  LCDA.Initialise();
  
  pinMode(LEFT_IR, INPUT);
  pinMode(RIGHT_IR, INPUT);

  pinMode(BUT1, INPUT);
  pinMode(BUT2, INPUT);
  pinMode(BUT3, INPUT);

  delay(100);
}

void loop() {
  LEFT_IR_VAR = digitalRead(LEFT_IR);
  RIGHT_IR_VAR = digitalRead(RIGHT_IR);
  US_VAR = US.Ranging(CM);
  BUT1_VAR = digitalRead(BUT1);
  BUT2_VAR = digitalRead(BUT2);
  BUT3_VAR = digitalRead(BUT3);
  
  LCDA.CLEAR();
  delay(100);
  LCDA.DisplayString(0, 0, test_en, 16);  //Тест английских символов
  delay(10);
  LCDA.DisplayString(1, 0, test_ru, 16);  //Тест русских символов
  delay(10);
  LCDA.DisplayString(2, 0, test_rfid, 16);
  delay(10);
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    // запишем метку в 4 байта
    unsigned char rfid_id[] = "";
    uint32_t ID32;
    uint64_t ID64;
    for (byte i = 0; i < 4; i++) {
      ID32 <<= 8;
      ID32 |= rfid.uid.uidByte[i];
      ID64 <<= 8;
      ID64 |= rfid.uid.uidByte[i];
    }
    itoa (ID64,rfid_id,16);
    LCDA.DisplayString(3, 0, rfid_id, 16);  //Вывод на экран информации с RFID
    delay(10);
    Serial.println(ID32, HEX);    
  }

  MASTER.writeByte(0x01, 0, LEFT_IR_VAR);
  MASTER.writeByte(0x01, 1, RIGHT_IR_VAR);
  MASTER.writeByte(0x01, 2, US_VAR);
  MASTER.writeByte(0x01, 3, BUT1_VAR);
  MASTER.writeByte(0x01, 4, BUT2_VAR);
  MASTER.writeByte(0x01, 5, BUT3_VAR);
  MASTER.writeByte(0x01, 6, RFID_VAR);
  
  MASTER.writeByte(0x02, 0, US_VAR);

  delay(500);
}