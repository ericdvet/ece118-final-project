/*
 * File:   robot.h
 * Author: evetha
 */

#ifndef ROACH_H    /* Guard against multiple inclusion */
#define ROACH_H

#include "BOARD.h"

/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/


void Robot_Init(void);


void Robot_LeftMotor(int speed);


void Robot_RightMotor(int speed);


void Robot_Servo(int position);


unsigned int Robot_BatteryVoltage(void);


unsigned char Robot_FrontLeftBumper(void);


unsigned char Robot_FrontRightBumper(void);


unsigned char Robot_RearLeftBumper(void);


unsigned char Robot_RearRightBumper(void);


unsigned char Robot_ReadBumpers(void);


unsigned int Robot_Read2KHzPeakDetector(void);


unsigned int Robot_Read15KHzPeakDetector(void);

unsigned int Robot_ReadTapeSensor(void);


#endif 

/* *****************************************************************************
 End of File
 */
