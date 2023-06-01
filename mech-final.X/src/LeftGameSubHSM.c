/*
 * File: TemplateSubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is for a substate machine. Make sure it has a unique name
 *
 * This is provided as an example and a good place to start.
 *
 * History
 * When           Who     What/Why
 * -------------- ---     --------
 * 09/13/13 15:17 ghe      added tattletail functionality and recursive calls
 * 01/15/12 11:12 jec      revisions for Gen2 framework
 * 11/07/11 11:26 jec      made the queue static
 * 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 * 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
 */


/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "BOARD.h"
#include "TopHSM.h"
#include "LeftGameSubHSM.h"
#include "robot.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    SubLoadingFindGoalState,
    SubLoadingGoalFoundState,
    ZoneLoadingTo23State,
    Sub23FindGoalState,
    Sub23GoalFoundState,
    Zone23To1State,
    Sub1FindGoalState,
    Sub1GoalFoundState,
    SubShootState,
    SubReturnState,
} LeftGameSubHSMState_t;

static const char *StateNames[] = {
    "InitPSubState",
    "SubLoadingFindGoalState",
    "SubLoadingGoalFoundState",
    "ZoneLoadingTo23State",
    "Sub23FindGoalState",
    "Sub23GoalFoundState",
    "Zone23To1State",
    "Sub1FindGoalState",
    "Sub1GoalFoundState",
};



/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static LeftGameSubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitLeftGameSubHSM(void) {
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunLeftGameSubHSM(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @Function RunTemplateSubHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the heirarchical state
 *        machine, as this is called any time a new event is passed to the event
 *        queue. This function will be called recursively to implement the correct
 *        order for a state transition to be: exit current state -> enter next state
 *        using the ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       The lower level state machines are run first, to see if the event is dealt
 *       with there rather than at the current level. ES_EXIT and ES_ENTRY events are
 *       not consumed as these need to pass pack to the higher level state machine.
 * @author J. Edward Carryer, 2011.10.23 19:25
 * @author Gabriel H Elkaim, 2011.10.23 19:25 */
ES_Event RunLeftGameSubHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    LeftGameSubHSMState_t nextState; // <- change type to correct enum

    ES_Tattle(); // trace call stack

    switch (CurrentState) {
        case InitPSubState: // If current state is initial Psedudo State
            if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state

                // now put the machine into the actual initial state
                //                Robot_LeftMotor(-500);
                //                Robot_RightMotor(500);
                nextState = SubLoadingGoalFoundState;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case SubLoadingGoalFoundState:
            Robot_LeftMotor(500);
            Robot_RightMotor(500);
            switch (ThisEvent.EventType) {
                    //                case TWO_KHZ_BEACON_NOT_DETECTED:
                    //                    nextState = SubLoadingFindGoalState;
                    //                    makeTransition = TRUE;
                    //                    ThisEvent.EventType = ES_NO_EVENT;
                    //                    //                    ES_Timer_InitTimer(START_TIMER, 1000);
                    //                    break;
                case TAPE_DETECTED:
                    if (ThisEvent.EventParam & 0b1100) {
                        nextState = Sub23GoalFoundState;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case ZoneLoadingTo23State:
            Robot_LeftMotor(500);
            Robot_RightMotor(500);
            switch (ThisEvent.EventType) {
                case TAPE_DETECTED:
                case TAPE_NOT_DETECTED:
                    if (!(ThisEvent.EventParam | 0b1000)) {
                        nextState = Sub23GoalFoundState;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
            }
            break;

        case Sub23FindGoalState: // in the first state, replace this with correct names
            Robot_LeftMotor(-500);
            Robot_RightMotor(500);
            switch (ThisEvent.EventType) {
                case TWO_KHZ_BEACON_DETECTED:
                    nextState = Sub23GoalFoundState;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case Sub23GoalFoundState:
            Robot_LeftMotor(800);
            Robot_RightMotor(800);
            switch (ThisEvent.EventType) {
                    //                case TWO_KHZ_BEACON_NOT_DETECTED:
                    //                    nextState = Sub23FindGoalState;
                    //                    makeTransition = TRUE;
                    //                    ThisEvent.EventType = ES_NO_EVENT;
                    //                    //                    ES_Timer_InitTimer(START_TIMER, 1000);
                    //                    break;
                case TAPE_DETECTED:
                    if (ThisEvent.EventParam & 0b1100) {
                        nextState = Sub1GoalFoundState;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                        ES_Timer_InitTimer(START_TIMER, 2000);
                    }
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case Zone23To1State:
            Robot_LeftMotor(0);
            Robot_RightMotor(0);
            //            switch (ThisEvent.EventType) {
            //                case TAPE_DETECTED:
            //                case TAPE_NOT_DETECTED:
            //                    if (!(ThisEvent.EventParam | 0b1000)) {
            //                        nextState = Sub23FindGoalState;
            //                        makeTransition = TRUE;
            //                        ThisEvent.EventType = ES_NO_EVENT;
            //                    }
            //            }
            break;

        case Sub1GoalFoundState:
            Robot_LeftMotor(500);
            Robot_RightMotor(500);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    nextState = SubShootState;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                default:
                    break;
            }
            break;

        case SubShootState:
            Robot_LeftMotor(-1000);
            Robot_RightMotor(-1000);
            break;

        default: // all unhandled states fall into here
            break;

    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunLeftGameSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunLeftGameSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/
