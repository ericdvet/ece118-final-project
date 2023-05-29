/*
 * File: Service_Bumper.c
 * Author: J. Edward Carryer, Gabriel H Elkaim, ericdvet,
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "BOARD.h"
#include <stdio.h>

#include "AD.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "Service_Bumper.h"
#include "EventChecker.h"
#include "TopHSM.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

#define DEBOUNCE_TICKS 50

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

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitBumperService(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunBumperService function.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, ericdvet, */
uint8_t InitBumperService(uint8_t Priority) {
    ES_Event ThisEvent;

    MyPriority = Priority;

    ES_Timer_InitTimer(BUTTON_DEBOUNCE_TIMER, DEBOUNCE_TICKS);
    
    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostBumperService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 * will be used inside ES_Configure to point to which queue events should
 * be posted to. Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, ericdvet, */
uint8_t PostBumperService(ES_Event ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunBumperService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the service,
 * as this is called any time a new event is passed to the event queue. Returns 
 * ES_NO_EVENT if the event have been "consumed."
 * @author J. Edward Carryer, ericdvet, */
ES_Event RunBumperService(ES_Event ThisEvent) {
    ES_Event ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
    

    static ES_EventTyp_t lastEvent = ES_NO_EVENT;
    ES_EventTyp_t curEvent;

    switch (ThisEvent.EventType) {
        case ES_INIT:
            break;
            
        case ES_TIMERACTIVE:
        case ES_TIMERSTOPPED:
            break;

        case ES_TIMEOUT:
            ES_Timer_InitTimer(BUTTON_DEBOUNCE_TIMER, DEBOUNCE_TICKS);
            Check_Bumper();
            break;
    }

    return ReturnEvent;
}

/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/

