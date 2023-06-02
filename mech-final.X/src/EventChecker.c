/* 
 * File:   EventChecker.c
 * Author: Gabriel Hugh Elkaim, ericdvet
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include <xc.h>
#include <stdio.h>

#include "ES_Configure.h"
#include "EventChecker.h"
#include "ES_Events.h"
#include "serial.h"
#include "AD.h"
#include "robot.h"
#include "TopHSM.h"
#include "timers.h"
#include "LED.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

#define BUFFER_SIZE 1

#define HYSTERSIS_BOUND_TWO 625
#define HYSTERSIS_BOUND_ONE_FIVE 0

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

/**
 * @Function filterPeak2KHz(unsigned int peakReading)
 * @param peakReading - Output of the 2 KHz Beacon Detector's Peak Detector 
 * @return Buffered value of input
 * @brief Returns the buffered average of the input
 * @author ericdvet,
 */
unsigned int filterPeak2KHz(unsigned int peakReading);

/**
 * @Function filterPeak15KHz(unsigned int peakReading)
 * @param peakReading - Output of the 1.5 KHz Beacon Detector's Peak Detector 
 * @return Buffered value of input
 * @brief Returns the buffered average of the input
 * @author ericdvet,
 */
unsigned int filterPeak15KHz(unsigned int peakReading);

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/

static int lastBumperLevel;

static int lastTapeReading;

typedef struct CircularBuffer {
    int head;
    unsigned int buffer [BUFFER_SIZE];
} CircularBuffer;

CircularBuffer peakBuffer2KHz;
CircularBuffer peakBuffer15KHz;

static enum {
    BEACON_DETECTED_2KHZ, BEACON_NOT_DETECTED_2KHZ
} last2KHzBeaconState;

static enum {
    BEACON_DETECTED_15KHZ, BEACON_NOT_DETECTED_15KHZ
} last15KHzBeaconState;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function Check_Bumper(void)
 * @param none
 * @return TRUE or FALSE
 * @brief Post an event of either BUMPER_DOWN or BUMPER_UP if a bumper is down
 *        or up. Returns TRUE if there was an event, FALSE otherwise.
 * @author ericdvet,
 */
uint8_t Check_Bumper(void) {
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

    if (lastBumperLevel != currentBumperLevel) { //event detected
        if (currentBumperState == DOWN) {
            thisEvent.EventType = BUMPER_DOWN;
        }
        if (currentBumperState == UP) {
            thisEvent.EventType = BUMPER_UP;
        }
        thisEvent.EventParam = currentBumperLevel;
        returnVal = TRUE;
        PostTopHSM(thisEvent);
    }

    lastBumperLevel = currentBumperLevel;
    return (returnVal);
}

/**
 * @Function Check_TapeSensor(void)
 * @param none
 * @return TRUE or FALSE
 * @brief Post an event of either TAPE_DETECTED or TAPE_NOT_DETECTED if a tape
 * is detected or not. Returns TRUE if there was an event, FALSE otherwise.
 * @author ericdvet,
 */
uint8_t Check_TapeSensor(void) {
    static ES_EventTyp_t lastEvent = TAPE_NOT_DETECTED;
    ES_EventTyp_t curEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    int currentTapeReading = Robot_ReadTapeSensor();
//    printf("\n\t%d", currentTapeReading);

    enum {
        TAPE, NO_TAPE
    } currentTapeState;

    if (currentTapeReading)
        currentTapeState = TAPE;
    else
        currentTapeState = NO_TAPE;

    //    printf("\n\tCurrent Tape Reading: %d", currentTapeReading);

    if (currentTapeReading != lastTapeReading) { //event detected
        if (currentTapeState == TAPE)
            thisEvent.EventType = TAPE_DETECTED;
        if (currentTapeState == NO_TAPE)
            thisEvent.EventType = TAPE_NOT_DETECTED;
        thisEvent.EventParam = currentTapeReading;
        returnVal = TRUE;
        PostTopHSM(thisEvent);
    }

    lastTapeReading = currentTapeReading;

    return (returnVal);
}

/**
 * @Function Check_PeakDetector2KHz(void)
 * @param none
 * @return TRUE or FALSE
 * @brief Post an event of either TWO_KHZ_BEACON_DETECTED or 
 * TWO_KHZ_BEACON_NOT_DETECTED if a 2KHz beacon is detected or not. Returns TRUE
 * if there was an event, FALSE otherwise.
 * @author ericdvet,
 */
uint8_t Check_PeakDetector2KHz(void) {
    static ES_EventTyp_t lastEvent;
    ES_EventTyp_t curEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    int current2KHzPeak = filterPeak2KHz(Robot_Read2KHzPeakDetector());

    enum {
        BEACON_DETECTED_2KHZ, BEACON_NOT_DETECTED_2KHZ
    } current2KHzBeaconState;

    if (current2KHzPeak > HYSTERSIS_BOUND_TWO)
        current2KHzBeaconState = BEACON_DETECTED_2KHZ;
    else
        current2KHzBeaconState = BEACON_NOT_DETECTED_2KHZ;

//    printf("\n\t%d ", current2KHzPeak);

    if (current2KHzBeaconState != last2KHzBeaconState) { //event detected
        if (current2KHzBeaconState == BEACON_DETECTED_2KHZ) {
            thisEvent.EventType = TWO_KHZ_BEACON_DETECTED;
            LED_OnBank(LED_BANK2, 7);
        } else {
            thisEvent.EventType = TWO_KHZ_BEACON_NOT_DETECTED;
            LED_OffBank(LED_BANK2, 7);
        }
        thisEvent.EventParam = current2KHzPeak;
        returnVal = TRUE;
        PostTopHSM(thisEvent);
    }

    last2KHzBeaconState = current2KHzBeaconState;

    return (returnVal);
}

/**
 * @Function InitBuffer2KHz(void)
 * @param None 
 * @return None
 * @brief Initializes the 2 KHz circular buffer
 * @author ericdvet,
 */
void InitBuffer2KHz(void) {
    peakBuffer2KHz.head = 0;
    for (int i = 0; i < BUFFER_SIZE; i++)
        peakBuffer2KHz.buffer[i] = 0;
}

/**
 * @Function filterPeak2KHz(unsigned int peakReading)
 * @param peakReading - Output of the 2 KHz Beacon Detector's Peak Detector 
 * @return Buffered value of input
 * @brief Returns the buffered average of the input
 * @author ericdvet,
 */
unsigned int filterPeak2KHz(unsigned int peakReading) {
    int newHead, sum;
    newHead = peakBuffer2KHz.head + 1;
    sum = 0;

    if (newHead >= BUFFER_SIZE) {
        newHead = 0;
    }

    peakBuffer2KHz.buffer[peakBuffer2KHz.head] = peakReading;
    peakBuffer2KHz.head = newHead;

    for (int i = 0; i < BUFFER_SIZE; i++) {
        sum += peakBuffer2KHz.buffer[i];
    }

    return (sum / BUFFER_SIZE);
}

/**
 * @Function Check_PeakDetector15KHz(void)
 * @param none
 * @return TRUE or FALSE
 * @brief Post an event of either TWO_KHZ_BEACON_DETECTED or 
 * TWO_KHZ_BEACON_NOT_DETECTED if a 1.5KHz beacon is detected or not. Returns 
 * TRUE if there was an event, FALSE otherwise.
 * @author ericdvet,
 */
uint8_t Check_PeakDetector15KHz(void) {
    static ES_EventTyp_t lastEvent;
    ES_EventTyp_t curEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    int current15KHzPeak = filterPeak15KHz(Robot_Read15KHzPeakDetector());
//    printf("\n\t%d", current15KHzPeak);

    enum {
        BEACON_DETECTED_15KHZ, BEACON_NOT_DETECTED_15KHZ
    } current15KHzBeaconState;

    if (current15KHzPeak > HYSTERSIS_BOUND_ONE_FIVE)
        current15KHzBeaconState = BEACON_DETECTED_15KHZ;
    else
        current15KHzBeaconState = BEACON_NOT_DETECTED_15KHZ;


    if (current15KHzBeaconState != last15KHzBeaconState) { //event detected
        if (current15KHzBeaconState == BEACON_DETECTED_15KHZ) {
            thisEvent.EventType = ONE_FIVE_KHZ_BEACON_DETECTED;
            LED_OnBank(LED_BANK3, 7);
        } else {
            thisEvent.EventType = ONE_FIVE_KHZ_BEACON_NOT_DETECTED;
            LED_OffBank(LED_BANK3, 7);
        }
        thisEvent.EventParam = current15KHzPeak;
        returnVal = TRUE;
        PostTopHSM(thisEvent);
    }

    last15KHzBeaconState = current15KHzBeaconState;

    return (returnVal);
}

/**
 * @Function InitBuffer15KHz(void)
 * @param None 
 * @return None
 * @brief Initializes the 1.5 KHz circular buffer
 * @author ericdvet,
 */
void InitBuffer15KHz(void) {
    peakBuffer15KHz.head = 0;
    for (int i = 0; i < BUFFER_SIZE; i++)
        peakBuffer15KHz.buffer[i] = 0;
}

/**
 * @Function filterPeak15KHz(unsigned int peakReading)
 * @param peakReading - Output of the 1.5 KHz Beacon Detector's Peak Detector 
 * @return Buffered value of input
 * @brief Returns the buffered average of the input
 * @author ericdvet,
 */
unsigned int filterPeak15KHz(unsigned int peakReading) {
    int newHead, sum;
    newHead = peakBuffer15KHz.head + 1;
    sum = 0;

    if (newHead >= BUFFER_SIZE) {
        newHead = 0;
    }

    peakBuffer15KHz.buffer[peakBuffer15KHz.head] = peakReading;
    peakBuffer15KHz.head = newHead;

    for (int i = 0; i < BUFFER_SIZE; i++) {
        sum += peakBuffer15KHz.buffer[i];
    }

    return (sum / BUFFER_SIZE);
}

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