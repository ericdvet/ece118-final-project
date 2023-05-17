/* 
 * File:   EventChecker.h
 * Author: Gabriel Hugh Elkaim, ericdvet
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

/**
 * @Function Check_Bumper(void)
 * @param none
 * @return TRUE or FALSE
 * @brief Post an event of either BUMPER_DOWN or BUMPER_UP if a bumper is down
 *        or up. Returns TRUE if there was an event, FALSE otherwise.
 * @author ericdvet,
 */
uint8_t Check_Bumper(void);

/**
 * @Function Check_TapeSensor(void)
 * @param none
 * @return TRUE or FALSE
 * @brief Post an event of either TAPE_DETECTED or TAPE_NOT_DETECTED if a tape
 * is detected or not. Returns TRUE if there was an event, FALSE otherwise.
 * @author ericdvet,
 */
uint8_t Check_TapeSensor(void);


uint8_t Check_PingSensor(unsigned int EWpin);

/**
 * @Function InitBuffer2KHz(void)
 * @param None 
 * @return None
 * @brief Initializes the 2 KHz circular buffer
 * @author ericdvet,
 */
void InitBuffer2KHz();

/**
 * @Function InitBuffer15KHz(void)
 * @param None 
 * @return None
 * @brief Initializes the 1.5 KHz circular buffer
 * @author ericdvet,
 */
void InitBuffer15KHz();

/**
 * @Function Check_PeakDetector2KHz(void)
 * @param none
 * @return TRUE or FALSE
 * @brief Post an event of either TWO_KHZ_BEACON_DETECTED or 
 * TWO_KHZ_BEACON_NOT_DETECTED if a 2KHz beacon is detected or not. Returns TRUE
 * if there was an event, FALSE otherwise.
 * @author ericdvet,
 */
uint8_t Check_PeakDetector2KHz(void);

/**
 * @Function Check_PeakDetector15KHz(void)
 * @param none
 * @return TRUE or FALSE
 * @brief Post an event of either TWO_KHZ_BEACON_DETECTED or 
 * TWO_KHZ_BEACON_NOT_DETECTED if a 1.5KHz beacon is detected or not. Returns 
 * TRUE if there was an event, FALSE otherwise.
 * @author ericdvet,
 */
uint8_t Check_PeakDetector15KHz(void);


#endif	

