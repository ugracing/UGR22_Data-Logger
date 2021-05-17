/*
 * CANFD.c
 *
 *  Created on: Apr 18, 2021
 *      Author: adamf
 */
#include <stdio.h>
#include <string.h>
#include <CANFD.h>
#include <buffer.h>
#include <myprintf.h>

int FDCAN_Flag = 0;
int CAN_Flag = 0;

void FDCAN_Config(FDCAN_HandleTypeDef *hfdcan){
	FDCAN_FilterTypeDef sFilterConfig;

	  /* Configure Rx filter */
	  sFilterConfig.IdType = FDCAN_STANDARD_ID;
	  sFilterConfig.FilterIndex = 0;
	  sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	  sFilterConfig.FilterID1 = 0x321;
	  sFilterConfig.FilterID2 = 0x7FF;
	  if (HAL_FDCAN_ConfigFilter(hfdcan, &sFilterConfig) == HAL_OK)
	  {
	    //printf("Filter configured\n");
	  }

	  /* Configure global filter:
	     Filter all remote frames with STD and EXT ID
	     Reject non matching frames with STD ID and EXT ID */
	  if (HAL_FDCAN_ConfigGlobalFilter(hfdcan, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
	  {
	    Error_Handler();
	  }
	if (HAL_FDCAN_Start(hfdcan) != HAL_OK)
	  {
	    Error_Handler();
	  }
	if (HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
	  {
	    /* Notification Error */
	    Error_Handler();
	  }
	HAL_FDCAN_EnableTxDelayCompensation(hfdcan);

}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan1, uint32_t RxFifo0ITs)
{
	if (HAL_FDCAN_GetRxMessage(hfdcan1, FDCAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
			      {
					uint32_t time = HAL_GetTick();
					HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
					printf("Packet Acquired!\n");
					if(RxHeader.FDFormat == FDCAN_CLASSIC_CAN){
						CanFrame.id = RxHeader.Identifier;
						switch(RxHeader.DataLength/65536){
							case 1:
								CanFrame.length = 1;
								break;
							case 2:
								CanFrame.length = 2;
								break;
							case 3:
								CanFrame.length = 3;
								break;
							case 4:
								CanFrame.length = 4;
								break;
							case 5:
								CanFrame.length = 5;
								break;
							case 6:
								CanFrame.length = 6;
								break;
							case 7:
								CanFrame.length = 7;
								break;
							case 8:
								CanFrame.length = 8;
								break;
							case 9:
								CanFrame.length = 12;
								break;
							case 10:
								CanFrame.length = 16;
								break;
							case 11:
								CanFrame.length = 20;
								break;
							case 12:
								CanFrame.length = 24;
								break;
							case 13:
								CanFrame.length = 32;
								break;
							case 14:
								CanFrame.length = 48;
								break;
							case 15:
								CanFrame.length = 64;
								break;
						}
						CanFrame.time = time;
						CAN_Flag = 1;
						memcpy(&CanFrame.data.bytes, RxData, CanFrame.length);
						if(RxHeader.IdType == FDCAN_EXTENDED_ID){
							CanFrame.extended = 1;
						}
						else{
							CanFrame.extended = 0;
						}
					}
					if(RxHeader.FDFormat == FDCAN_FD_CAN){
						CanFDFrame.id = RxHeader.Identifier;

						switch(RxHeader.DataLength/65536){
							case 1:
								CanFDFrame.length = 1;
								break;
							case 2:
								CanFDFrame.length = 2;
								break;
							case 3:
								CanFDFrame.length = 3;
								break;
							case 4:
								CanFDFrame.length = 4;
								break;
							case 5:
								CanFDFrame.length = 5;
								break;
							case 6:
								CanFDFrame.length = 6;
								break;
							case 7:
								CanFDFrame.length = 7;
								break;
							case 8:
								CanFDFrame.length = 8;
								break;
							case 9:
								CanFDFrame.length = 12;
								break;
							case 10:
								CanFDFrame.length = 16;
								break;
							case 11:
								CanFDFrame.length = 20;
								break;
							case 12:
								CanFDFrame.length = 24;
								break;
							case 13:
								CanFDFrame.length = 32;
								break;
							case 14:
								CanFDFrame.length = 48;
								break;
							case 15:
								CanFDFrame.length = 64;
								break;
						}

						CanFDFrame.time = time;
						FDCAN_Flag = 1;
						memcpy(&CanFDFrame.data.bytes, RxData, CanFDFrame.length);
						if(RxHeader.IdType == FDCAN_EXTENDED_ID){
							CanFDFrame.extended = 1;
						}
						else{
							CanFDFrame.extended = 0;
						}
					}

					/*
				  	  for(int i=0;i<64;i++){
				  			  printf("%c",RxData[i]);
				  	  }
				  	  printf("\n");
				  	*/
			      }
}
