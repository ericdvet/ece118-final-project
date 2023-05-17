/*
 * File:   robot.h
 * Author: ericdvet, 
 */

#ifndef ROBOT_H    /* Guard against multiple inclusion */
#define ROBOT_H

#include "BOARD.h"

/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

/**
 * @Function Robot_Init(void)
 * @param None.
 * @return None.
 * @brief Performs all the initialization necessary for the robot.
 * @note  None.
 * @author ericdvet,  */
void Robot_Init(void);

/**
 * @Function Robot_LeftMotor(int speed)
 * @param speed - A value between -1000 and 1000 which is the new speed. A 
 * 0 stops the motor. A negative value is reverse.
 * @return None.
 * @brief Sets the speed and direction of the left 
 * motor.
 * @note  None.
 * @author ericdvet,  */
void Robot_LeftMotor(int speed);

/**
 * @Function Robot_RightMotor(int speed)
 * @param speed - A value between -1000 and 1000 which is the new speed. A 
 * 0 stops the motor. A negative value is reverse.
 * @return None.
 * @brief Sets the speed and direction of the right 
 * motor.
 * @note  None.
 * @author ericdvet,  */
void Robot_RightMotor(int speed);

/**
 * @Function Robot_Servo(int position)
 * @param speed - A value between 1000 and 2000 which is the new position. 
 * @return None.
 * @brief Sets the position of the servo. 
 * @note  None.
 * @author ericdvet,  */
void Robot_Servo(int position1, int position2);

/**
 * @Function Robot_FrontLeftBumper(void)
 * @param None
 * @return Number > 0 if front left bumper is tripped
 * @brief Checks whether the front left bumper has been tripped. 
 * @note  None.
 * @author ericdvet,  */
unsigned char Robot_FrontLeftBumper(void);

/**
 * @Function Robot_FrontRightBumper(void)
 * @param None
 * @return Number > 0 if front right bumper is tripped
 * @brief Checks whether the front right bumper has been tripped. 
 * @note  None.
 * @author ericdvet,  */
unsigned char Robot_FrontRightBumper(void);

/**
 * @Function Robot_RearLeftBumper(void)
 * @param None
 * @return Number > 0 if rear left bumper is tripped
 * @brief Checks whether the rear left bumper has been tripped. 
 * @note  None.
 * @author ericdvet,  */
unsigned char Robot_RearLeftBumper(void);

/**
 * @Function Robot_RearRightBumper(void)
 * @param None
 * @return Number > 0 if rear right bumper is tripped
 * @brief Checks whether the rear right bumper has been tripped. 
 * @note  None.
 * @author ericdvet,  */
unsigned char Robot_RearRightBumper(void);

/**
 * @Function Robot_ReadBumpers(void)
 * @param None.
 * @return 4-bit value representing all four bumpers in following order: 
 * front left, front right, rear left, rear right
 * @brief  Returns the state of all 4 bumpers
 * @author ericdvet, */
unsigned char Robot_ReadBumpers(void);

/**
 * @Function Robot_Read2KHzPeakDetector(void)
 * @param None.
 * @return Current output of 2KHz Beacon Detector's Peak Detector
 * @brief  Returns the output of the 2KHz Beacon Detector's Peak Detector
 * @author ericdvet, */
unsigned int Robot_Read2KHzPeakDetector(void);

/**
 * @Function Robot_Read15KHzPeakDetector(void)
 * @param None.
 * @return Current output of 1.5KHz Beacon Detector's Peak Detector
 * @brief  Returns the output of the 1.5KHz Beacon Detector's Peak Detector
 * @author ericdvet, */
unsigned int Robot_Read15KHzPeakDetector(void);

/**
 * @Function Robot_ReadTapeSensor(void)
 * @param None.
 * @return Current output of tape sensor
 * @brief  Returns the output of the tape sensor
 * @author ericdvet, */
unsigned int Robot_ReadTapeSensor(void);

unsigned int Robot_ReadPingSensor(void);


#endif 

/* *****************************************************************************
 End of File
 */
