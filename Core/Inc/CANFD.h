/*
 * CANFD.h
 *
 *  Created on: Apr 18, 2021
 *      Author: adamf
 */

#ifndef INC_CANFD_H_
#define INC_CANFD_H_

//includes
#include "main.h"

//typdefs
FDCAN_RxHeaderTypeDef RxHeader;

//variables

uint8_t RxData[64];

//functions
void FDCAN_Config(FDCAN_HandleTypeDef *hfdcan);

#endif /* INC_CANFD_H_ */
