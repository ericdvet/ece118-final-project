#include "robot.h"
#include "BOARD.h"
#include <xc.h>
#include "IO_Ports.h"


#include "pwm.h"
#include "serial.h"
#include "AD.h"

/*******************************************************************************
 * PRIVATE #DEFINES                                                            *
 ******************************************************************************/

#define LEFT_DIR LATBbits.LATB3
#define LEFT_DIR_INV LATBbits.LATB2
#define RIGHT_DIR LATEbits.LATE5
#define RIGHT_DIR_INV LATEbits.LATE6

#define LEFT_DIR_TRIS _TRISB3
#define LEFT_DIR_INV_TRIS _TRISB2
#define RIGHT_DIR_TRIS _TRISE5
#define RIGHT_DIR_INV_TRIS _TRISE6

#define BUMPER_PORT PORTZ

#define BUMPER_FRONT_LEFT PIN3
#define BUMPER_FRONT_RIGHT PIN4
#define BUMPER_REAR_LEFT PIN5
#define BUMPER_REAR_RIGHT PIN6

#define LEFT_PWM PWM_PORTY10
#define RIGHT_PWM PWM_PORTY12

#define ROACH_BAT_VOLTAGE BAT_VOLTAGE

#define NUMLEDS 12


void Robot_Init(void) {

    PWM_Init();
    PWM_SetFrequency(1000);
    PWM_AddPins(LEFT_PWM | RIGHT_PWM);

    LEFT_DIR_TRIS = 0;
    LEFT_DIR_INV_TRIS = 0;
    RIGHT_DIR_TRIS = 0;
    RIGHT_DIR_INV_TRIS = 0;
    LEFT_DIR = 0;
    LEFT_DIR_INV = ~LEFT_DIR;
    RIGHT_DIR = 0;
    RIGHT_DIR_INV = ~RIGHT_DIR;

    IO_PortsSetPortInputs(BUMPER_PORT, BUMPER_FRONT_LEFT | BUMPER_FRONT_RIGHT | BUMPER_REAR_RIGHT | BUMPER_REAR_LEFT);
}

void Robot_LeftMotor(int speed) {
    int newSpeed;

    if (speed <= 0) {
        LEFT_DIR = 0;
        newSpeed = speed * -1;
    } else {
        LEFT_DIR = 1;
        newSpeed = speed;
    }

    PWM_SetDutyCycle(LEFT_PWM, speed);
}

void Robot_RightMotor(int speed) {
    int newSpeed;

    if (speed <= 0) {
        RIGHT_DIR = 0;
        newSpeed = speed * -1;
    } else {
        RIGHT_DIR = 1;
        newSpeed = speed;
    }

    PWM_SetDutyCycle(RIGHT_PWM, speed);
}

unsigned char Robot_FrontLeftBumper(void) {
    return (IO_PortsReadPort(BUMPER_PORT) & BUMPER_FRONT_LEFT);
}

unsigned char Robot_FrontRightBumper(void) {
    return (IO_PortsReadPort(BUMPER_PORT) & BUMPER_FRONT_RIGHT);
}

unsigned char Robot_RearLeftBumper(void) {
    return (IO_PortsReadPort(BUMPER_PORT) & BUMPER_REAR_LEFT);
}

unsigned char Robot_RearRightBumper(void) {
    return (IO_PortsReadPort(BUMPER_PORT) & BUMPER_REAR_LEFT);
}

unsigned char Robot_ReadBumpers(void) {
    unsigned char bumperMask = 0;

    if (Robot_FrontLeftBumper())
        bumperMask = bumperMask | 0b1000;

    if (Robot_FrontLeftBumper())
        bumperMask = bumperMask | 0b0100;

    if (Robot_FrontLeftBumper())
        bumperMask = bumperMask | 0b0010;

    if (Robot_FrontLeftBumper())
        bumperMask = bumperMask | 0b0001;

    return bumperMask;
}

#define ROBOT_TEST
#ifdef ROBOT_TEST

#include <stdio.h>


#define DELAY(x) for (int wait = 0; wait <= x; wait++) {asm("nop");}
#define DELAY_TIME 18300

int main(void) {
    BOARD_Init();
    Robot_Init();

    printf("\r\ntesting... testing... 1 2 3...");

    Robot_LeftMotor(1000);
    DELAY(DELAY_TIME);
    Robot_LeftMotor(0);
    DELAY(DELAY_TIME);
    Robot_RightMotor(1000);
    DELAY(DELAY_TIME);
    Robot_RightMotor(0);
    DELAY(DELAY_TIME);

    while (1) {
        switch (Robot_ReadBumpers()) {
            case 0b1000:
                printf("\r\nFront Left Bumper");
                break;
            case 0b0100:
                printf("\r\nFront Right Bumper");
                break;
            case 0b0010:
                printf("\r\nRear Left Bumper");
                break;
            case 0b0001:
                printf("\r\nRear Right Bumper");
                break;
            default:
                break;
        }
    }
    
    while(1);
}

#endif