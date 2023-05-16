/* 
 * File:   TemplateEventChecker.h
 * Author: Gabriel Hugh Elkaim
 *
 * Template file to set up typical EventCheckers for the  Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the
 * names will have to be changed to match your code.
 *
 * This EventCheckers file will work with simple services, FSM's and HSM's.
 *
 * Remember that EventCheckers should only return TRUE when an event has occured,
 * and that the event is a TRANSITION between two detectable differences. They
 * should also be atomic and run as fast as possible for good results.
 *
 * This is provided as an example and a good place to start.
 *
 * Created on September 27, 2013, 8:37 AM
 * Modified on September 12, 2016, 7:59 PM
 */

#ifndef EVENT_CHECKER_H
#define	EVENT_CHECKER_H

/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "ES_Configure.h"   // defines ES_Event, INIT_EVENT, ENTRY_EVENT, and EXIT_EVENT
#include "BOARD.h"

/*******************************************************************************
 * PUBLIC #DEFINES                                                             *
 ******************************************************************************/


/*******************************************************************************
 * PUBLIC TYPEDEFS                                                             *
 ******************************************************************************/


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

uint8_t Check_Bumper(void);
int Check_TapeSensor(unsigned int sensorPin);
int Check_PingSensor(unsigned int EWpin);
void InitBuffer2KHz();
void InitBuffer15KHz();
uint8_t Check_PeakDetector2KHz(void);
uint8_t Check_PeakDetector15KHz(void);



#endif	/* TEMPLATEEVENTCHECKER_H */

