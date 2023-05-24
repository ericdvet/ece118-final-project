/*
 * File:   ping.c
 * Author: ericdvet,
 */

#include <xc.h>
#include <BOARD.h>
#include <sys/attribs.h>
#include "ping.h"
#include <stdio.h>
#include "AD.h"
#include "robot.h"

#define FREE_RUNNING_TIMER 1000

static int freeRunningTimer;

static enum {
    PING, ECHO1, ECHO2, WAIT
} pingSensorState;

static int echoRecorded;

void __ISR(_CHANGE_NOTICE_VECTOR) ChangeNotice_Handler(void) {
    static char readPort = 0;
    readPort = PORTD; // this read is required to make the interrupt work
    IFS1bits.CNIF = 0;

    printf("\n\tVECTOR");

}

void __ISR(_TIMER_4_VECTOR) Timer4IntHandler(void) {
    IFS0bits.T4IF = 0;
    //Anything else that needs to occur goes here

    //    printf("\n\t%d\t\n", Robot_ReadPingSensor());

    freeRunningTimer++;

    switch (pingSensorState) {

        case PING:
            Robot_SendPing(1);
            //            if (Robot_ReadPingSensor() > 800) {
            //                Robot_SendPing(0);
            //                pingSensorState = ECHO;
            //                freeRunningTimer = 0;
            //            }
//            if (freeRunningTimer % (1000 / FREE_RUNNING_TIMER) == 0)
//                printf("\n\t%d", Robot_ReadPingSensor());
            if (freeRunningTimer > 10000 / FREE_RUNNING_TIMER) {
                Robot_SendPing(0);
                pingSensorState = ECHO1;
                freeRunningTimer = 0;
            }
//            printf("PING");
            break;

        case ECHO1:
//            printf("\n\t%d", Robot_ReadPingSensor());
            if (Robot_ReadPingSensor() > 800) {
                pingSensorState = ECHO2;
                freeRunningTimer = 0;
            }
            if (freeRunningTimer > 60000 / FREE_RUNNING_TIMER) {
                pingSensorState = PING;
                freeRunningTimer = 0;
            }
//            printf("ECHO1");
            break;

        case ECHO2:
            if (Robot_ReadPingSensor() < 800) {
                pingSensorState = WAIT;
                echoRecorded = freeRunningTimer;
                freeRunningTimer = 0;
//                printf("\n\t%dcm\t\n", echoRecorded);
            }
            if (freeRunningTimer > 23200 / FREE_RUNNING_TIMER) {
                pingSensorState = PING;
                freeRunningTimer = 0;
                echoRecorded = -1;
            }
//            printf("ECHO2");
            break;

        case WAIT:
//            if (freeRunningTimer % (1000 / FREE_RUNNING_TIMER) == 0)
//                printf("\n\t%d", Robot_ReadPingSensor());
            if (freeRunningTimer > 60000 / FREE_RUNNING_TIMER) {
                pingSensorState = PING;
                freeRunningTimer = 0;
            }
//            printf("WAIT");
            break;

        default:
            break;
    }

    //    if (freeRunningTimer > 1000000 / FREE_RUNNING_TIMER) {
    //        printf("\n\t%d", freeRunningTimer);
    //        freeRunningTimer = 0;
    //    }
}

/**
 * @function    PING_Init(void)
 * @brief       Sets up both the timer and Change notify peripherals along with their
 *              respective interrupts.  Also handles any other tasks needed such as pin 
 *              I/O directions, and any other things you need to initialize the sensor.
 *              TIMERS library must be inited before this library.
 * @return      SUCCESS or ERROR (as defined in BOARD.h)
 */
char PING_Init(void) {
    // following block inits the timer
    T4CON = 0;
    T4CONbits.TCKPS = 0b100;
    //PR4 = BOARD_GetPBClock() / 64 * 100 / 1000000; // this is not the timer value wanted
    PR4 = 2500;
    T4CONbits.ON = 1;
    IFS0bits.T4IF = 0;
    IPC4bits.T4IP = 3;
    IEC0bits.T4IE = 1;

    // following block inits change notify
    //    CNCONbits.ON = 1; // Change Notify On
    //    CNENbits.CNEN14 = 1;
    //    int temp = PORTD; // this is intentional to ensure a interrupt occur immediately upon enabling
    //    IFS1bits.CNIF = 0; // clear interrupt flag
    //    IPC6bits.CNIP = 1; //set priority
    //    IPC6bits.CNIS = 3; // and sub priority
    //    IEC1bits.CNIE = 1; // enable change notify

    //Anything else that needs to occur goes here
    freeRunningTimer = 0;
    pingSensorState = PING;
}

/**
 * @function    PING_GetDistance(void)
 * @brief       Returns the calculated distance in mm using the sensor model determined
 *              experimentally. 
 *              No I/O should be done in this function
 * @return      distance in mm
 */
unsigned int PING_GetDistance(void) {
    return PING_GetTimeofFlight() / 58.0;
}

/**
 * @function    PING_GetTimeofFlight(void)
 * @brief       Returns the raw microsecond duration of the echo from the sensor.
 *              NO I/O should be done in this function.
 * @return      time of flight in uSec
 */
unsigned int PING_GetTimeofFlight(void) {
    return echoRecorded * FREE_RUNNING_TIMER;
} 