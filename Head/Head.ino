/*
Matrix 8x8:
DIN   11
CS    9
CLK   13
Вначале подключать левый глаз, затем правый
*/

//Matrix 8x8
#include <GyverMAX7219.h>

#define CS  9
#define HM  2 //Количество матриц по горизонтали
#define VM  1 //Количество матриц по вертикали

MAX7219 < HM, VM, CS > eyes; 

void setup() {
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
}

void loop() {
}