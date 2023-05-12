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
#define BUMPER_DOWN 1
#define BUMPER_UP 0


void Robot_Init(void);


void Robot_LeftMotor(int speed);


void Robot_RightMotor(int speed);


unsigned int Robot_BatteryVoltage(void);


unsigned char Robot_FrontLeftBumper(void);


unsigned char Robot_FrontRightBumper(void);


unsigned char Robot_RearLeftBumper(void);


unsigned char Robot_RearRightBumper(void);


unsigned char Robot_ReadBumpers(void);


char Robot_LEDSSet(uint16_t pattern);


#endif 

/* *****************************************************************************
 End of File
 */
