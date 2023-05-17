#include "robot.h"
#include "BOARD.h"
#include <xc.h>
#include "IO_Ports.h"


#include "pwm.h"
#include "serial.h"
#include "AD.h"
#include "RC_Servo.h"
#include "LED.h"

/*******************************************************************************
 * PRIVATE #DEFINES                                                            *
 ******************************************************************************/

// Bumper Ports

#define BUMPER_PORT PORTZ

#define BUMPER_FRONT_LEFT PIN3
#define BUMPER_FRONT_RIGHT PIN4
#define BUMPER_REAR_LEFT PIN5
#define BUMPER_REAR_RIGHT PIN6

// Driving Wheel Ports

#define LEFT_IN_1 PWM_PORTY10
#define LEFT_IN_2 PWM_PORTY12
#define RIGHT_IN_1 PWM_PORTY04
#define RIGHT_IN_2 PWM_PORTX11

// Servo Port
#define SERVO_PIN RC_PORTY06

// Peak Detector Input Ports
#define PEAK_2KHZ_PIN AD_PORTV3
#define PEAK_15KHZ_PIN AD_PORTV4

// Tape Detector 
#define TAPE_DETECTOR_PIN AD_PORTW8

void Robot_Init(void) {

    // Driving Motors
    PWM_Init();
    PWM_SetFrequency(1000);
    PWM_AddPins(LEFT_IN_1);
    PWM_AddPins(LEFT_IN_2);
    PWM_AddPins(RIGHT_IN_1);
    PWM_AddPins(RIGHT_IN_2);
    
    // Peak Detectors
    AD_Init();
    AD_AddPins(PEAK_2KHZ_PIN);
    AD_AddPins(PEAK_15KHZ_PIN);

    // Servo
    RC_Init();
    RC_AddPins(SERVO_PIN);
    
    // LEDs on UNO32
    LED_Init();
    LED_AddBanks(LED_BANK1 | LED_BANK2 | LED_BANK3);
    LED_OffBank(LED_BANK1, 7);
    LED_OffBank(LED_BANK2, 7);
    LED_OffBank(LED_BANK3, 7);
    
    // Tape Detector
    AD_AddPins(TAPE_DETECTOR_PIN);

    // Bumpers
    IO_PortsSetPortInputs(BUMPER_PORT, BUMPER_FRONT_LEFT | BUMPER_FRONT_RIGHT | BUMPER_REAR_RIGHT | BUMPER_REAR_LEFT);
}

void Robot_LeftMotor(int speed) {
    int in1, in2;

    if (speed < 0) {
        in1 = speed * -1;
        in2 = 0;
    } else {
        in1 = 0;
        in2 = speed;
    }

    PWM_SetDutyCycle(LEFT_IN_1, in1);
    PWM_SetDutyCycle(LEFT_IN_2, in2);
}

void Robot_RightMotor(int speed) {
    int in1, in2;

    if (speed < 0) {
        in1 = speed * -1;
        in2 = 0;
    } else {
        in1 = 0;
        in2 = speed;
    }

    PWM_SetDutyCycle(RIGHT_IN_1, in2);
    PWM_SetDutyCycle(RIGHT_IN_2, in1);
}

void Robot_Servo(int position) {
    RC_SetPulseTime(SERVO_PIN, position);
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
    return (IO_PortsReadPort(BUMPER_PORT) & BUMPER_REAR_RIGHT);
}

unsigned char Robot_ReadBumpers(void) {
    unsigned char bumperMask = 0;

    if (Robot_FrontLeftBumper())
        bumperMask = bumperMask | 0b1000;

    if (Robot_FrontRightBumper())
        bumperMask = bumperMask | 0b0100;

    if (Robot_RearLeftBumper())
        bumperMask = bumperMask | 0b0010;

    if (Robot_RearRightBumper())
        bumperMask = bumperMask | 0b0001;

    return bumperMask;
}

unsigned int Robot_Read2KHzPeakDetector(void) {
    return AD_ReadADPin(PEAK_2KHZ_PIN);
}

unsigned int Robot_Read15KHzPeakDetector(void) {
    return AD_ReadADPin(PEAK_15KHZ_PIN);
}

unsigned int Robot_ReadTapeSensor(void) {
    return (AD_ReadADPin(TAPE_DETECTOR_PIN) > 500);
}

//#define ROBOT_TEST
#ifdef ROBOT_TEST

#include <stdio.h>


#define DELAY(x) for (int wait = 0; wait <= x; wait++) {asm("nop");}
#define DELAY_TIME 18300

int main(void) {
    BOARD_Init();
    Robot_Init();

    printf("\nWelcome to evetha's robot.h test harness.  Compiled on %s %s.\n", __TIME__, __DATE__);
    
    while(1) {
        printf("\n\t%d", Robot_ReadTapeSensor());
    }
//    AD_AddPins(AD_PORTV3);
//    while(1) {
//        printf("\n\t%d", AD_ReadADPin(AD_PORTV3));
//    }

    // IO library output tests
    //    while (1) {
    //        IO_PortsSetPortOutputs(PORTX, PIN3);
    //        IO_PortsSetPortOutputs(PORTX, PIN4);
    //        DELAY(100000);
    //        IO_PortsTogglePortBits(PORTX, PIN3);
    //        IO_PortsClearPortBits(PORTX, PIN4);
    //    }

    while (1) {
        Robot_RightMotor(1000);
        DELAY(1000000);
        Robot_RightMotor(800);
        DELAY(1000000);
        Robot_RightMotor(600);
        DELAY(1000000);
        Robot_RightMotor(400);
        DELAY(1000000);
        Robot_RightMotor(200);
        DELAY(1000000);
        Robot_RightMotor(0);
        DELAY(1000000);
        Robot_RightMotor(-200);
        DELAY(1000000);
        Robot_RightMotor(-400);
        DELAY(1000000);
        Robot_RightMotor(-600);
        DELAY(1000000);
        Robot_RightMotor(-800);
        DELAY(1000000);
        Robot_RightMotor(-1000);
        DELAY(1000000);
    }

    while (1) {
        printf("\n\t%d", Robot_ReadBumpers());
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

    while (1);
}

#endif