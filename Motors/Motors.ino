/*
Motors:
LEFT_STEP    10
RIGHT_STEP   11
LEFT_DIR     12
RIGHT_DIR    13

PS2 Controller:
PS2_DAT   8 
PS2_CMD   7
PS2_SEL   6
PS2_CLK   4
*/

//I2C
#include <Wire.h>
#include <iarduino_I2C_connect.h> 

iarduino_I2C_connect SLAVE;

byte  REG_MAS[1]; //0 - Расстояние с УЗ дальномера

//Step motor
#include <AccelStepper.h>

#define LEFT_STEP       10
#define RIGHT_STEP      11
#define LEFT_DIR        12
#define RIGHT_DIR       13

#define MAX_SPEED       200
#define ROTATE_SPEED    80
#define MOVE_SPEED      100
#define START_SPEED     20
#define STOP_DIST       30

int SPEED_LEFT = 0;
int SPEED_RIGHT = 0;

AccelStepper LeftMotor(1, LEFT_STEP, LEFT_DIR);
AccelStepper RightMotor(1, RIGHT_STEP, RIGHT_DIR);

//PS2 Controller
#include <PS2X_lib.h>
PS2X ps2x;   

#define PS2_DAT   8 
#define PS2_CMD   7
#define PS2_SEL   6
#define PS2_CLK   4

int error = 0;

void setup() {
  Wire.begin(0x02);
  SLAVE.begin(REG_MAS); 

  LeftMotor.setMaxSpeed(MAX_SPEED);
  RightMotor.setMaxSpeed(MAX_SPEED);

  LeftMotor.setSpeed(SPEED_LEFT);
  RightMotor.setSpeed(SPEED_RIGHT);

  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, true, false);  
}

void loop() {
  ps2x.read_gamepad();

  byte RX = ps2x.Analog(PSS_RX);
  byte LY = ps2x.Analog(PSS_LY);

  if (RX < 128) {
    SPEED_LEFT = -map(RX, 0, 128, ROTATE_SPEED, 0);
    SPEED_RIGHT = map(RX, 0, 128, ROTATE_SPEED, 0);    
  }
  else if (RX > 128) {
    SPEED_LEFT = -map(RX, 128, 255, 0, -ROTATE_SPEED);
    SPEED_RIGHT = map(RX, 128, 255, 0, -ROTATE_SPEED);    
  }
  else {
    SPEED_LEFT = 0;
    SPEED_RIGHT = 0;     
  }

  if (LY < 127 && REG_MAS[0] > STOP_DIST) {
    SPEED_LEFT += map(LY, 0, 127, MOVE_SPEED, START_SPEED);
    SPEED_RIGHT += map(LY, 0, 127, MOVE_SPEED, START_SPEED);    
  }
  else if (LY > 127) {
    SPEED_LEFT += map(LY, 127, 255, START_SPEED, MOVE_SPEED);
    SPEED_RIGHT += map(LY, 127, 255, START_SPEED, MOVE_SPEED);
    SPEED_LEFT = -SPEED_LEFT;
    SPEED_RIGHT = -SPEED_RIGHT;    
  }
  else {
    SPEED_LEFT += 0;
    SPEED_RIGHT += 0;     
  }  

  LeftMotor.setSpeed(SPEED_LEFT);
  RightMotor.setSpeed(SPEED_RIGHT);

  delay(20);
}
