/*
 * File: TemplateService.h
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a simple service to work with the Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that this file
 * will need to be modified to fit your exact needs, and most of the names will have
 * to be changed to match your code.
 *
 * This is provided as an example and a good place to start.
 *
 * Created on 23/Oct/2011
 * Updated on 13/Nov/2013
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "BOARD.h"
#include "AD.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "Service_PingSensor.h"
#include "EventChecker.h"
#include <stdio.h>
#include "TopHSM.h"
#include "IO_Ports.h"
#include "timers.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

#define DEBOUNCE_TICKS 200

#define TRIGGER_PORT PORTV
#define TRIGGER_PIN PIN3
#define ECHO_PIN AD_PORTV4
#define THRESHOLD_PING_SENSOR 0

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/
/* You will need MyPriority and maybe a state variable; you may need others
 * as well. */

static uint8_t MyPriority;

typedef enum {
    SENSOR_NONE,
    SENSOR_PING,
    SENSOR_BURST,
    SENSOR_OUTPUT,
    SENSOR_DONE,
} SensorStates_t;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateService(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateService function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitPingSensorService(uint8_t Priority) {
    ES_Event ThisEvent;

    MyPriority = Priority;

    // in here you write your initialization code
    // this includes all hardware and software initialization
    // that needs to occur.

    // post the initial transition event
    ES_Timer_InitTimer(BUTTON_DEBOUNCE_TIMER, DEBOUNCE_TICKS);
    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostPingSensorService(ES_Event ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the service,
 *        as this is called any time a new event is passed to the event queue. 
 * @note Remember to rename to something appropriate.
 *       Returns ES_NO_EVENT if the event have been "consumed." 
 * @author J. Edward Carryer, 2011.10.23 19:25 */
ES_Event RunPingSensorService(ES_Event ThisEvent) {
    ES_Event ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    /********************************************
     in here you write your service code
     *******************************************/
    static ES_EventTyp_t lastEvent = ES_NO_EVENT;
    static SensorStates_t sensorState = SENSOR_NONE;
    static int timeLogger;
    int sensorReading;

    switch (ThisEvent.EventType) {
        case ES_INIT:
            sensorState = SENSOR_NONE;
            break;
        case ES_TIMERACTIVE:

        case ES_TIMERSTOPPED:

            break;


        case ES_TIMEOUT:
            ES_Timer_InitTimer(BUTTON_DEBOUNCE_TIMER, DEBOUNCE_TICKS);
            switch (sensorState) {
                case SENSOR_NONE:
                    sensorReading = TIMERS_GetTime();
                    sensorState = SENSOR_PING;
                    break;

                case SENSOR_PING:
                    IO_PortsSetPortBits(TRIGGER_PORT, TRIGGER_PIN);
                    if (TIMERS_GetTime() - sensorReading >= 11)
                        sensorState = SENSOR_BURST;
                    break;

                case SENSOR_BURST:
                    IO_PortsClearPortBits(TRIGGER_PORT, TRIGGER_PIN);
                    if (AD_ReadADPin(ECHO_PIN) > THRESHOLD_PING_SENSOR) {
                        sensorState = SENSOR_OUTPUT;
                        sensorReading = TIMERS_GetTime();
                    }
                    break;

                case SENSOR_OUTPUT:
                    if (AD_ReadADPin(ECHO_PIN) <= THRESHOLD_PING_SENSOR) {
                        sensorState = SENSOR_DONE;
                        ReturnEvent.EventType = SENSOR_READING;
                        ReturnEvent.EventParam = (1000 * (TIMERS_GetTime() - sensorReading)) / 148;
                        PostTopHSM(ReturnEvent);
                        sensorReading = TIMERS_GetTime();

                    }
                    break;

                case SENSOR_DONE:
                    if (TIMERS_GetTime() - sensorReading >= 21)
                        sensorState = SENSOR_NONE;
                    break;

                default:
                    break;

                    //            if (curEvent != lastEvent) { // check for change from last time
                    //                ReturnEvent.EventType = curEvent;
                    //                ReturnEvent.EventParam = batVoltage;
                    //                lastEvent = curEvent; // update history
                    //#ifndef SIMPLESERVICE_TEST           // keep this as is for test harness
                    //                PostTopHSM(ReturnEvent);
                    //#else
                    //                PostTemplateService(ReturnEvent);
                    //#endif   
                    //            }
                    break;
#ifdef SIMPLESERVICE_TEST     // keep this as is for test harness      
                default:
                    printf("\r\nEvent: %s\tParam: 0x%X",
                            EventNames[ThisEvent.EventType], ThisEvent.EventParam);
                    break;
#endif
            }
    }

    return ReturnEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/


/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/

