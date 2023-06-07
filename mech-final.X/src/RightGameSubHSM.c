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
#include "RightGameSubHSM.h"
#include "robot.h"
#include "LED.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    SubLoadingZoneState,
    SubLoadingTo23BufferState,
    Sub23ZoneState,
    Sub23BeaconLost1State,
    Sub23BeaconLost2State,
    Sub1Zone0State,
    Sub1Zone1State,
    Sub1Zone2State,
    SubAiming1State,
    SubAiming2State,
    SubPoweringUpState,
    SubReloadBall1State,
    SubShootBall1State,
    SubReloadBall2State,
    SubShootBall2State,
    SubReloadBall3State,
    SubShootBall3State,
    SubReorientState,
    SubReturn1BeaconLost1State,
    SubReturn1BeaconLost2State,
    SubReturn1State,
    SubReturn1Reset1State,
    SubReturn1Reset2State,
    SubReturn23BeaconLost1State,
    SubReturn23BeaconLost2State,
    SubReturn23State,
    SubReturn23Reset1State,
    SubReturn23Reset2State,
    SubReturnLoadingState,
    SubExitState,
} RightGameSubHSMState_t;

static const char *StateNames[] = {
    "InitPSubState",
    "SubLoadingZoneState",
    "SubLoadingTo23BufferState",
    "Sub23ZoneState",
    "Sub1ZoneState",
    "SubAimingState",
    "SubPoweringUpState",
    "SubReloadBall1State",
    "SubShootBall1State",
    "SubReloadBall2State",
    "SubShootBall2State",
    "SubReloadBall3State",
    "SubShootBall3State",
    "SubReorientState",
    "SubReturnState",
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

static RightGameSubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
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
uint8_t InitRightGameSubHSM(void) {
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunRightGameSubHSM(INIT_EVENT);
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
ES_Event RunRightGameSubHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    RightGameSubHSMState_t nextState; // <- change type to correct enum

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
                //                ES_Timer_InitTimer(FUCK_UP_TIMER, 5000);
                nextState = SubLoadingZoneState;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case SubLoadingZoneState:
            Robot_LeftMotor(500);
            Robot_RightMotor(500);
            switch (ThisEvent.EventType) {
                case TAPE_DETECTED:
                    if (ThisEvent.EventParam & 0b1100) {
                        nextState = SubLoadingTo23BufferState;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                        ES_Timer_InitTimer(TIMER_TWO, 400);
                    }
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == FUCK_UP_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 250);
                        nextState = Sub1Zone1State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case SubLoadingTo23BufferState:
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == TIMER_TWO) {
                        nextState = Sub23ZoneState;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                default:
                    break;
            }
            break;

        case Sub23ZoneState:
            Robot_LeftMotor(800);
            Robot_RightMotor(700);
            switch (ThisEvent.EventType) {
                case TAPE_DETECTED:
                    if (ThisEvent.EventParam & 0b1100) {
                        ES_Timer_InitTimer(START_TIMER, 600);
                        nextState = Sub1Zone0State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case TWO_KHZ_BEACON_NOT_DETECTED:
                    ES_Timer_InitTimer(START_TIMER, 250);
                    nextState = Sub23BeaconLost2State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == FUCK_UP_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 250);
                        nextState = Sub1Zone2State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case Sub23BeaconLost1State:
            //            Robot_LeftMotor(500);
            //            Robot_RightMotor(-500);
            Robot_LeftMotor(800);
            Robot_RightMotor(500);
            LED_OnBank(LED_BANK2, 7);
            switch (ThisEvent.EventType) {
                case TAPE_DETECTED:
                    if (ThisEvent.EventParam & 0b1100) {
                        ES_Timer_InitTimer(START_TIMER, 600);
                        nextState = Sub1Zone0State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case TWO_KHZ_BEACON_DETECTED:
                    nextState = Sub23ZoneState;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 500);
                        nextState = Sub23BeaconLost2State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    if (ThisEvent.EventParam == FUCK_UP_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 250);
                        nextState = Sub1Zone2State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case Sub23BeaconLost2State:
            //            Robot_LeftMotor(-500);
            //            Robot_RightMotor(500);
            Robot_LeftMotor(500);
            Robot_RightMotor(800);
            LED_OffBank(LED_BANK2, 7);
            switch (ThisEvent.EventType) {
                case TAPE_DETECTED:
                    if (ThisEvent.EventParam & 0b1100) {
                        ES_Timer_InitTimer(START_TIMER, 600);
                        nextState = Sub1Zone0State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case TWO_KHZ_BEACON_DETECTED:
                    nextState = Sub23ZoneState;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 500);
                        nextState = Sub23BeaconLost2State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    if (ThisEvent.EventParam == FUCK_UP_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 250);
                        nextState = Sub1Zone2State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case Sub1Zone0State:
            Robot_LeftMotor(800);
            Robot_RightMotor(800);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 250);
                        nextState = Sub1Zone2State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case Sub1Zone1State:
            Robot_LeftMotor(500);
            Robot_RightMotor(-500);
            Robot_Servo(1000, 1000);
            switch (ThisEvent.EventType) {
                case TWO_KHZ_BEACON_DETECTED:
                    nextState = SubAiming1State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 500);
                        nextState = Sub1Zone2State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                default:
                    break;
            }
            break;

        case Sub1Zone2State:
            Robot_LeftMotor(-500);
            Robot_RightMotor(500);
            Robot_Servo(1000, 1000);
            switch (ThisEvent.EventType) {
                case TWO_KHZ_BEACON_DETECTED:
                    nextState = SubAiming1State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 500);
                        nextState = Sub1Zone1State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                default:
                    break;
            }
            break;

        case SubAiming1State:
            Robot_LeftMotor(500);
            Robot_RightMotor(-500);
            switch (ThisEvent.EventType) {
                case TWO_KHZ_BEACON_NOT_DETECTED:
                    //                    ES_Timer_InitTimer(START_TIMER, 100);
                    ES_Timer_InitTimer(START_TIMER, 125);
                    nextState = SubAiming2State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                default:
                    break;
            }
            break;

        case SubAiming2State:
            Robot_LeftMotor(-500);
            Robot_RightMotor(500);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 2000);
                        nextState = SubPoweringUpState;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                default:
                    break;
            }
            break;

        case SubPoweringUpState:
            Robot_LeftMotor(0);
            Robot_RightMotor(0);
            Robot_FlyWheel(700);
            Robot_Servo(1000, 1000);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 400);
                        nextState = SubShootBall1State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case ONE_FIVE_KHZ_BEACON_DETECTED:
                    nextState = SubReloadBall1State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                default:
                    break;
            }
            break;

        case SubReloadBall1State:
            Robot_FlyWheel(700);
            Robot_Servo(1000, 1000);
            switch (ThisEvent.EventType) {
                    //                case ES_TIMEOUT:
                    //                    if (ThisEvent.EventParam == START_TIMER) {
                    //                        ES_Timer_InitTimer(START_TIMER, 400);
                    //                        nextState = SubShootBall1State;
                    //                        makeTransition = TRUE;
                    //                        ThisEvent.EventType = ES_NO_EVENT;
                    //                    }
                    //                    break;
                case ONE_FIVE_KHZ_BEACON_NOT_DETECTED:
                    ES_Timer_InitTimer(START_TIMER, 500);
                    nextState = SubShootBall1State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                default:
                    break;
            }
            break;

        case SubShootBall1State:
            Robot_FlyWheel(700);
            Robot_Servo(2000, 1000);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 500);
                        nextState = SubReloadBall2State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                default:
                    break;
            }
            break;

        case SubReloadBall2State:
            Robot_FlyWheel(700);
            Robot_Servo(1000, 1000);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 500);
                        nextState = SubShootBall2State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                default:
                    break;
            }
            break;

        case SubShootBall2State:
            Robot_FlyWheel(700);
            Robot_Servo(2000, 1000);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 500);
                        nextState = SubReloadBall3State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                default:
                    break;
            }
            break;

        case SubReloadBall3State:
            Robot_FlyWheel(700);
            Robot_Servo(1000, 1000);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 500);
                        nextState = SubShootBall3State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                default:
                    break;
            }
            break;

        case SubShootBall3State:
            Robot_FlyWheel(700);
            Robot_Servo(2000, 1000);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 250); // this is to reorient not reload
                        nextState = SubReorientState;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                default:
                    break;
            }
            break;

        case SubReorientState:
            Robot_LeftMotor(-500);
            Robot_RightMotor(500);
            Robot_FlyWheel(0);
            Robot_Servo(1000, 1000);
            switch (ThisEvent.EventType) {
                case TWO_KHZ_BEACON_DETECTED:
                    nextState = SubReturn23State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                default:
                    break;
            }
            break;

        case SubReturn23State:
            Robot_LeftMotor(-800);
            Robot_RightMotor(-700);
//            Robot_FlyWheel(0);
//            Robot_Servo(1000, 1000);
            switch (ThisEvent.EventType) {
                case TWO_KHZ_BEACON_NOT_DETECTED:
                    ES_Timer_InitTimer(START_TIMER, 250);
                    nextState = SubReturn23BeaconLost1State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case TAPE_DETECTED:
                    if (ThisEvent.EventParam & 0b0010) {
                        nextState = SubReturn1State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case BUMPER_DOWN:
                    ES_Timer_InitTimer(START_TIMER, 500);
                    nextState = SubReturn23Reset1State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case SubReturn23BeaconLost1State:
            //            Robot_LeftMotor(500);
            //            Robot_RightMotor(-500);
            Robot_LeftMotor(-500);
            Robot_RightMotor(-800);
            switch (ThisEvent.EventType) {
                case TWO_KHZ_BEACON_DETECTED:
                    nextState = SubReturn23State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 500);
                        nextState = SubReturn23BeaconLost2State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case TAPE_DETECTED:
                    if (ThisEvent.EventParam & 0b0010) {
                        nextState = SubReturn1State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case BUMPER_DOWN:
                    ES_Timer_InitTimer(START_TIMER, 500);
                    nextState = SubReturn23Reset1State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case SubReturn23BeaconLost2State:
            //            Robot_LeftMotor(-500);
            //            Robot_RightMotor(500);
            Robot_LeftMotor(-800);
            Robot_RightMotor(-500);
            switch (ThisEvent.EventType) {
                case TWO_KHZ_BEACON_DETECTED:
                    nextState = SubReturn23State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 500);
                        nextState = SubReturn23BeaconLost1State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case TAPE_DETECTED:
                    if (ThisEvent.EventParam & 0b0010) {
                        nextState = SubReturn1State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case BUMPER_DOWN:
                    ES_Timer_InitTimer(START_TIMER, 1000);
                    nextState = SubReturn23Reset1State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case SubReturn23Reset1State:
            Robot_LeftMotor(800);
            Robot_RightMotor(500);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 500);
                        nextState = SubReturn23State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case TAPE_DETECTED:
                    //                    if (ThisEvent.EventParam & 0b0011) {
                    //                        nextState = SubReturn1State;
                    //                        makeTransition = TRUE;
                    //                        ThisEvent.EventType = ES_NO_EVENT;
                    //                    }
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case SubReturn1State:
            Robot_LeftMotor(-800);
            Robot_RightMotor(-700);
            switch (ThisEvent.EventType) {
                case TWO_KHZ_BEACON_NOT_DETECTED:
                    ES_Timer_InitTimer(START_TIMER, 250);
                    nextState = SubReturn1BeaconLost1State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case TAPE_DETECTED:
                    if (ThisEvent.EventParam & 0b0010) {
                        nextState = SubReturnLoadingState;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case BUMPER_DOWN:
                    ES_Timer_InitTimer(START_TIMER, 500);
                    nextState = SubReturn1Reset1State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case SubReturn1BeaconLost1State:
            //            Robot_LeftMotor(500);
            //            Robot_RightMotor(-500);
            Robot_LeftMotor(-500);
            Robot_RightMotor(-800);
            switch (ThisEvent.EventType) {
                case TWO_KHZ_BEACON_DETECTED:
                    nextState = SubReturn1State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 500);
                        nextState = SubReturn1BeaconLost2State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case TAPE_DETECTED:
                    if (ThisEvent.EventParam & 0b0010) {
                        nextState = SubReturnLoadingState;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case BUMPER_DOWN:
                    ES_Timer_InitTimer(START_TIMER, 500);
                    nextState = SubReturn1Reset1State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case SubReturn1BeaconLost2State:
            //            Robot_LeftMotor(-500);
            //            Robot_RightMotor(500);
            Robot_LeftMotor(-800);
            Robot_RightMotor(-500);
            switch (ThisEvent.EventType) {
                case TWO_KHZ_BEACON_DETECTED:
                    nextState = SubReturn1State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 500);
                        nextState = SubReturn1BeaconLost1State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case TAPE_DETECTED:
                    if (ThisEvent.EventParam & 0b0010) {
                        nextState = SubReturnLoadingState;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case BUMPER_DOWN:
                    ES_Timer_InitTimer(START_TIMER, 500);
                    nextState = SubReturn1Reset1State;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case SubReturn1Reset1State:
            Robot_LeftMotor(800);
            Robot_RightMotor(500);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == START_TIMER) {
                        ES_Timer_InitTimer(START_TIMER, 500);
                        nextState = SubReturn1State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case TAPE_DETECTED:
                    if (ThisEvent.EventParam & 0b0010) {
                        nextState = SubReturn23State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case SubReturnLoadingState:
            Robot_LeftMotor(-800);
            Robot_RightMotor(-800);
            switch (ThisEvent.EventType) {
                case BUMPER_DOWN:
                    ES_Timer_InitTimer(EXIT_TIMER, 500);
                    nextState = SubExitState;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case SubExitState:
            Robot_LeftMotor(0);
            Robot_RightMotor(0);
            break;

        default: // all unhandled states fall into here
            break;

    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunRightGameSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunRightGameSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

