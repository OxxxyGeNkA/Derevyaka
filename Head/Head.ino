/*
Matrix 8x8:
DIN   11
CS    9
CLK   13
Вначале подключать левый глаз, затем правый
*/

byte  playing_state      = 0; //0 - ничего не играет, 1..n играет трек с номером 1..n
byte  ticks_on_track[25] = {0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100}; //Длительность в тиках (1tick = 40мс) каждого трека
byte  now_tick_track     = 0; //

byte  eyes_state         = 0; //0 - просто моргает, 1 - смотрит влево (если сработал левый ИК датчик), 2 - смотрит вправо, 3 - закрывает глаза (гадает по руке)
byte  ticks_on_eyes[4]   = {134, 55, 55, 100}; //Длительность в тиках (40мс) каждой анимации
byte  now_tick_eyes      = 0;
byte  wait_for_eyes      = 0; //Чтобы не мотал глазами слишком часто  

//I2C
#include <Wire.h>
#include <iarduino_I2C_connect.h> 

iarduino_I2C_connect SLAVE;

byte  REG_MAS[7]; //0 - Левый ИК, 1 - Правый ИК, 2 - Расстояние с УЗ дальномера, 3..5 - Кнопки, 6 - RFID

//Matrix 8x8
#include <GyverMAX7219.h>

#define CS  9
#define HM  2 //Количество матриц по горизонтали
#define VM  1 //Количество матриц по вертикали

MAX7219 < HM, VM, CS > eyes;

//MP3 module
#include <SoftwareSerial.h>

#define RX  2
#define TX  3

SoftwareSerial myMP3(RX, TX);

byte sendBuffer[6]; //buffer that will be used to store commands before sending

//Manual says the code for this is 7E 03 35 01 EF so load into buffer array
void selectTFCard(){
  sendBuffer[0] = 0x7E;
  sendBuffer[1] = 0x03;
  sendBuffer[2] = 0x35;
  sendBuffer[3] = 0x01;
  sendBuffer[4] = 0xEF;
  sendUARTCommand();
}

void playSound(byte songNumber){
  sendBuffer[0] = 0x7E;
  sendBuffer[1] = 0x04;
  sendBuffer[2] = 0x41;
  sendBuffer[3] = 0x00;
  sendBuffer[4] = songNumber;
  sendBuffer[5] = 0xEF;
  sendUARTCommand();
}

//set the playback volume 7E 03 31 0F EF = set volume to 0x0F = 15
void setplayVolume(byte volume){
  sendBuffer[0] = 0x7E;
  sendBuffer[1] = 0x03;
  sendBuffer[2] = 0x31;
  sendBuffer[3] = volume;
  sendBuffer[4] = 0xEF;
  sendUARTCommand();
}

//stop the current track playing
void stopSound(){
  sendBuffer[0] = 0x7E;
  sendBuffer[1] = 0x02;
  sendBuffer[2] = 0x0E;
  sendBuffer[3] = 0xEF;
  sendUARTCommand();
}

void sendUARTCommand(){
  int q;
  for(q=0;q < sendBuffer[1] + 2;q++){
    myMP3.write(sendBuffer[q]);  
  }
  Serial.println("Commands Sent");
  for(q=0;q < sendBuffer[1] + 2;q++){
    Serial.println(sendBuffer[q],HEX);  
  }
  delay(25);//stops odd commands being missed
}

void setup() {
  Wire.begin(0x01);
  SLAVE.begin(REG_MAS);  

  eyes.begin();
  eyes.setBright(10);  
  eyes.fastLineH(0, 3, 7);
  eyes.fastLineV(3, 0, 7);
  eyes.fastLineH(7, 11, 15);
  eyes.fastLineH(4, 11, 15);
  eyes.fastLineV(11, 0, 7);
  eyes.fastLineV(15, 4, 7);
  eyes.line(15, 0, 11, 4);
  eyes.update();
  delay(5000);
}

void loop() {
  eyes.clear();

  if (REG_MAS[0] == 1 && REG_MAS[1] == 0 && wait_for_eyes == 0) {
    eyes_state = 1;
    now_tick_eyes = 0;
    wait_for_eyes = 5;    
  }

  if (REG_MAS[1] == 1 && REG_MAS[0] == 0 && wait_for_eyes == 0) {
    eyes_state = 2;
    now_tick_eyes = 0;
    wait_for_eyes = 5;    
  }

  if (eyes_state == 0) {
    if (now_tick_eyes < 125) {
      eyes.rect(3, 2, 4, 5, GFX_FILL);
      eyes.rect(2, 3, 5, 4, GFX_FILL);  
      eyes.rect(11, 2, 12, 5, GFX_FILL);
      eyes.rect(10, 3, 13, 4, GFX_FILL);          
    }
    else if (now_tick_eyes == 125 || now_tick_eyes == 133) {
      eyes.rect(3, 2, 4, 4, GFX_FILL);
      eyes.rect(2, 3, 5, 3, GFX_FILL);
      eyes.rect(11, 2, 12, 4, GFX_FILL);
      eyes.rect(10, 3, 13, 3, GFX_FILL);      
    }
    else if (now_tick_eyes == 126 || now_tick_eyes == 132) {
      eyes.rect(3, 2, 4, 3, GFX_FILL);
      eyes.rect(2, 3, 5, 3, GFX_FILL);  
      eyes.rect(11, 2, 12, 3, GFX_FILL);
      eyes.rect(10, 3, 13, 3, GFX_FILL);     
    }
    else {
      eyes.rect(3, 2, 4, 2, GFX_FILL);
      eyes.dot(2, 3);
      eyes.dot(5, 3);
      eyes.rect(11, 2, 12, 2, GFX_FILL);
      eyes.dot(10, 3);
      eyes.dot(13, 3);
    }
  }

  if (eyes_state == 1) {
    if (now_tick_eyes == 0) {
      eyes.rect(3, 2, 4, 5, GFX_FILL);
      eyes.rect(2, 3, 5, 4, GFX_FILL);  
      eyes.rect(11, 2, 12, 5, GFX_FILL);
      eyes.rect(10, 3, 13, 4, GFX_FILL);       
    }
    else if (now_tick_eyes == 1 || now_tick_eyes == 54) {
      eyes.rect(4, 2, 5, 5, GFX_FILL);
      eyes.rect(3, 3, 6, 4, GFX_FILL);
      eyes.rect(12, 2, 13, 5, GFX_FILL);
      eyes.rect(11, 3, 14, 4, GFX_FILL);      
    }
    else if (now_tick_eyes == 2 || now_tick_eyes == 53) {
      eyes.rect(5, 2, 6, 5, GFX_FILL);
      eyes.rect(4, 3, 7, 4, GFX_FILL);  
      eyes.rect(13, 2, 14, 5, GFX_FILL);
      eyes.rect(12, 3, 15, 4, GFX_FILL);       
    }
    else {
      eyes.rect(6, 2, 7, 5, GFX_FILL);
      eyes.rect(5, 3, 7, 4, GFX_FILL);     
      eyes.rect(14, 2, 15, 5, GFX_FILL);
      eyes.rect(13, 3, 15, 4, GFX_FILL);
    }
  }
  
  if (eyes_state == 2) {
    if (now_tick_eyes == 0) {
      eyes.rect(3, 2, 4, 5, GFX_FILL);
      eyes.rect(2, 3, 5, 4, GFX_FILL);  
      eyes.rect(11, 2, 12, 5, GFX_FILL);
      eyes.rect(10, 3, 13, 4, GFX_FILL);       
    }
    else if (now_tick_eyes == 1 || now_tick_eyes == 54) {
      eyes.rect(2, 2, 3, 5, GFX_FILL);
      eyes.rect(1, 3, 4, 4, GFX_FILL);
      eyes.rect(10, 2, 11, 5, GFX_FILL);
      eyes.rect(9, 3, 12, 4, GFX_FILL);      
    }
    else if (now_tick_eyes == 2 || now_tick_eyes == 53) {
      eyes.rect(1, 2, 2, 5, GFX_FILL);
      eyes.rect(0, 3, 3, 4, GFX_FILL);  
      eyes.rect(9, 2, 10, 5, GFX_FILL);
      eyes.rect(8, 3, 11, 4, GFX_FILL);       
    }
    else {
      eyes.rect(0, 2, 1, 5, GFX_FILL);
      eyes.rect(0, 3, 2, 4, GFX_FILL);     
      eyes.rect(8, 2, 9, 5, GFX_FILL);
      eyes.rect(8, 3, 10, 4, GFX_FILL);
    }
  }


  ++now_tick_eyes;
  if (now_tick_eyes == ticks_on_eyes[eyes_state]) {
    if (eyes_state == 0 && wait_for_eyes > 0) {
      --wait_for_eyes;
    }
    if (eyes_state == 1 || eyes_state == 2) {
      wait_for_eyes = 5;
    }
    now_tick_eyes = 0;
    eyes_state = 0;   
  }
  eyes.update();  
  delay(40);
}