/*
 * File:   TemplateEventChecker.c
 * Author: Gabriel Hugh Elkaim
 *
 * Template file to set up typical EventCheckers for the  Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the
 * names will have to be changed to match your code.
 *
 * This EventCheckers file will work with both FSM's and HSM's.
 *
 * Remember that EventCheckers should only return TRUE when an event has occured,
 * and that the event is a TRANSITION between two detectable differences. They
 * should also be atomic and run as fast as possible for good results.
 *
 * This file includes a test harness that will run the event detectors listed in the
 * ES_Configure file in the project, and will conditionally compile main if the macro
 * EVENTCHECKER_TEST is defined (either in the project or in the file). This will allow
 * you to check you event detectors in their own project, and then leave them untouched
 * for your project unless you need to alter their post functions.
 *
 * Created on September 27, 2013, 8:37 AM
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "EventChecker.h"
#include "ES_Events.h"
#include "serial.h"
#include "AD.h"
#include "robot.h"
#include "TopHSM.h"
#include "timers.h"
#include <xc.h>
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define BATTERY_DISCONNECT_THRESHOLD 175

/*******************************************************************************
 * EVENTCHECKER_TEST SPECIFIC CODE                                                             *
 ******************************************************************************/

//#define EVENTCHECKER_TEST
#ifdef EVENTCHECKER_TEST
#include <stdio.h>
#define SaveEvent(x) do {eventName=__func__; storedEvent=x;} while (0)

static const char *eventName;
static ES_Event storedEvent;
#endif

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this EventChecker. They should be functions
   relevant to the behavior of this particular event checker */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/

/* Any private module level variable that you might need for keeping track of
   events would be placed here. Private variables should be STATIC so that they
   are limited in scope to this module. */

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

static enum {
    DOWN, UP
} lastBumperState;

static unsigned char lastBumperLevel = 0;

uint8_t Check_Bumper(void) {
//    printf("\nin check bumper");
    static ES_EventTyp_t lastEvent = BUMPER_UP;
    ES_EventTyp_t curEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    unsigned char currentBumperLevel = Robot_ReadBumpers();

    enum {
        DOWN, UP
    } currentBumperState;

    if (currentBumperLevel)
        currentBumperState = DOWN;
    else
        currentBumperState = UP;
    
//    printf("\n\t%d", currentBumperLevel);

    if (currentBumperState != lastBumperState) { //event detected
        if (currentBumperState == DOWN) {
            printf("\n\tdown");
            thisEvent.EventType = BUMPER_DOWN;
        }
        if (currentBumperState == UP) {
            printf("\n\tup");
            thisEvent.EventType = BUMPER_UP;
        }
        thisEvent.EventParam = currentBumperLevel;
        returnVal = TRUE;
        printf("\n\tEvent %d with param %d", thisEvent.EventType, thisEvent.EventParam);
        PostTopHSM(thisEvent); 
    }
    
    lastBumperState = currentBumperState;
    return (returnVal);
}

static enum {
    TAPE, NO_TAPE
} lastTapeState;

#define HIGH_THRESHOLD 10
#define LOW_THRESHOLD 1

int Check_TapeSensor(unsigned int sensorPort) {
    static ES_EventTyp_t lastEvent = TAPE_NOT_DETECTED;
    ES_EventTyp_t curEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    unsigned int currentTapeReading = AD_ReadADPin(sensorPort);

    enum {
        TAPE, NO_TAPE
    } currentTapeState;

    if (currentTapeReading > HIGH_THRESHOLD)
        currentTapeState = TAPE;
    else
        currentTapeState = NO_TAPE;

    if (currentTapeState != lastTapeState) { //event detected
        if (currentTapeState == TAPE)
            thisEvent.EventType = TAPE_DETECTED;
        if (currentTapeState == NO_TAPE)
            thisEvent.EventType = TAPE_NOT_DETECTED;
        thisEvent.EventParam = (uint16_t) currentTapeReading;
        returnVal = TRUE;
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
        PostTopHSM(thisEvent);
#else
        SaveEvent(ThisEvent);
#endif   
    }

    return (returnVal);
}

uint8_t Check_Bumper(void) {
//    printf("\nin check bumper");
    static ES_EventTyp_t lastEvent = BUMPER_UP;
    ES_EventTyp_t curEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    unsigned char currentBumperLevel = Robot_ReadBumpers();

    enum {
        DOWN, UP
    } currentBumperState;

    if (currentBumperLevel)
        currentBumperState = DOWN;
    else
        currentBumperState = UP;
    
//    printf("\n\t%d", currentBumperLevel);

    if (currentBumperState != lastBumperState) { //event detected
        if (currentBumperState == DOWN) {
            printf("\n\tdown");
            thisEvent.EventType = BUMPER_DOWN;
        }
        if (currentBumperState == UP) {
            printf("\n\tup");
            thisEvent.EventType = BUMPER_UP;
        }
        thisEvent.EventParam = currentBumperLevel;
        returnVal = TRUE;
        printf("\n\tEvent %d with param %d", thisEvent.EventType, thisEvent.EventParam);
        PostTopHSM(thisEvent); 
    }
    
    lastBumperState = currentBumperState;
    return (returnVal);
}

static enum {
    TAPE, NO_TAPE
} lastTapeState;

#define HIGH_THRESHOLD 10
#define LOW_THRESHOLD 1

int Check_TapeSensor(unsigned int sensorPort) {
    static ES_EventTyp_t lastEvent = TAPE_NOT_DETECTED;
    ES_EventTyp_t curEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    unsigned int currentTapeReading = AD_ReadADPin(sensorPort);

    enum {
        TAPE, NO_TAPE
    } currentTapeState;

    if (currentTapeReading > HIGH_THRESHOLD)
        currentTapeState = TAPE;
    else
        currentTapeState = NO_TAPE;

    if (currentTapeState != lastTapeState) { //event detected
        if (currentTapeState == TAPE)
            thisEvent.EventType = TAPE_DETECTED;
        if (currentTapeState == NO_TAPE)
            thisEvent.EventType = TAPE_NOT_DETECTED;
        thisEvent.EventParam = (uint16_t) currentTapeReading;
        returnVal = TRUE;
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
        PostTopHSM(thisEvent);
#else
        SaveEvent(ThisEvent);
#endif   
    }

    return (returnVal);
}

//#define TIMER1_US_PER_TICK 25
//#define ECHO_THRESHOLD 0
//
//typedef enum {
//    ECHO_NONE,
//            ECHO_HIGH,
//            ECHO_LOW,
//} EchoState_t;
//
//static EchoState_t lastEchoState = ECHO_NONE;
//static int timerStart;
//static int microSecCount;
//
//int Check_PingSensor(unsigned int EWpin) {
//    EchoState_t currentEchoState;
//    if (AD_ReadADPin(EWPin) > ECHO_THRESHOLD)
//        currentEchoState = ECHO_HIGH;
//    else
//        currentEchoState = ECHO_LOW;
//    
//    if (lastEchoState == ECHO_LOW && currentEchoState == ECHO_HIGH) {
//        OpenTimer(TI_ON, 0xFFFF);
//        timerStart = ReadTimer();
//        T1CON
//        
//    }
//}

/* 
 * The Test Harness for the event checkers is conditionally compiled using
 * the EVENTCHECKER_TEST macro (defined either in the file or at the project level).
 * No other main() can exist within the project.
 * 
 * It requires a valid ES_Configure.h file in the project with the correct events in 
 * the enum, and the correct list of event checkers in the EVENT_CHECK_LIST.
 * 
 * The test harness will run each of your event detectors identically to the way the
 * ES_Framework will call them, and if an event is detected it will print out the function
 * name, event, and event parameter. Use this to test your event checking code and
 * ensure that it is fully functional.
 * 
 * If you are locking up the output, most likely you are generating too many events.
 * Remember that events are detectable changes, not a state in itself.
 * 
 * Once you have fully tested your event checking code, you can leave it in its own
 * project and point to it from your other projects. If the EVENTCHECKER_TEST marco is
 * defined in the project, no changes are necessary for your event checkers to work
 * with your other projects.
 */
#ifdef EVENTCHECKER_TEST
#include <stdio.h>
static uint8_t(*EventList[])(void) = {EVENT_CHECK_LIST};

void PrintEvent(void);

void main(void) {
    BOARD_Init();
    /* user initialization code goes here */

    // Do not alter anything below this line
    int i;

    printf("\r\nEvent checking test harness for %s", __FILE__);

    while (1) {
        if (IsTransmitEmpty()) {
            for (i = 0; i< sizeof (EventList) >> 2; i++) {
                if (EventList[i]() == TRUE) {
                    PrintEvent();
                    break;
                }

            }
        }
    }
}

void PrintEvent(void) {
    printf("\r\nFunc: %s\tEvent: %s\tParam: 0x%X", eventName,
            EventNames[storedEvent.EventType], storedEvent.EventParam);
}
#endif