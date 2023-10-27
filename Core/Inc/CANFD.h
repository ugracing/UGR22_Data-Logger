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
	uint64_t Distribution;
	char Intsructions[128];
}ReadInstruction;

extern FDCAN_RxHeaderTypeDef RxHeader;
//variables

extern ReadInstruction Configs[100]; //Arbitrarily set known packet limit to 100
extern CAN_FRAME CanFrame;
extern CAN_FD_FRAME CanFDFrame;


//functions
void FDCAN_Config(FDCAN_HandleTypeDef *hfdcan);
int CANFD_Data_Process(char WriteArray[], int StrIndex);
int CAN_Data_Process(char WriteArray[], int StrIndex);
int numPlaces (uint64_t n);

#endif /* INC_CANFD_H_ */
