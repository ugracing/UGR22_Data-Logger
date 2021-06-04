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
#include <buffer.h>

//typedefs
typedef struct{
	uint32_t id;
	uint32_t Bytes;
	uint32_t Distribution;
	char Intsructions[128];
}ReadInstruction;

FDCAN_RxHeaderTypeDef RxHeader;
//variables

ReadInstruction Configs[100]; //Arbitrarily set known packet limit to 100
CAN_FRAME CanFrame;
CAN_FD_FRAME CanFDFrame;
uint8_t RxData[64];

//functions
void FDCAN_Config(FDCAN_HandleTypeDef *hfdcan);
int CANFD_Data_Process(char WriteArray[], int StrIndex);
int CAN_Data_Process(char WriteArray[], int StrIndex);
int numPlaces (int n);

#endif /* INC_CANFD_H_ */
