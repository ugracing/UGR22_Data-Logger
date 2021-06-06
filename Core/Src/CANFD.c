/*
 * CANFD.c
 *
 *  Created on: Apr 18, 2021
 *      Author: adamf
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
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
	  sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
	  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	  sFilterConfig.FilterID1 = 0x0;
	  sFilterConfig.FilterID2 = 0x7FF;

	  FDCAN_FilterTypeDef eFilterConfig;
	  eFilterConfig.IdType = FDCAN_EXTENDED_ID;
	  eFilterConfig.FilterIndex = 8;
	  eFilterConfig.FilterType = FDCAN_FILTER_RANGE;
	  eFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	  eFilterConfig.FilterID1 = 0x0;
	  eFilterConfig.FilterID2 = 0x1FFFFFFF;
	  if (HAL_FDCAN_ConfigFilter(hfdcan, &sFilterConfig) == HAL_OK)
	  {
	    //printf("Filter configured\n");
	  }
	  if (HAL_FDCAN_ConfigFilter(hfdcan, &eFilterConfig) == HAL_OK)
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

int numPlaces (uint64_t n) {
	if (n < 10) return 1;
	if (n < 100) return 2;
	if (n < 1000) return 3;
	if (n < 10000) return 4;
	if (n < 100000) return 5;
	if (n < 1000000) return 6;
	if (n < 10000000) return 7;
	if (n < 100000000) return 8;
	if (n < 1000000000) return 9;
	if (n < 10000000000) return 10;
	if (n < 100000000000) return 11;
	if (n < 1000000000000) return 12;
	if (n < 10000000000000) return 13;
	if (n < 100000000000000) return 14;
	if (n < 1000000000000000) return 15;
	return 16;
}

int CANFD_Data_Process(char WriteArray[], int StrIndex){
	int found = 0, i = 0, size = 0;
	char inst[128];

	for(i = 0; i <(sizeof(*Configs)/sizeof(ReadInstruction)); i++){
		if(CanFDFrame.id == Configs[i].id){
			found = 1;
			break;
		}
	}

	if(found == 0){
		for(int j = 0; j < CanFDFrame.length; j++){
			StrIndex += sprintf(WriteArray + StrIndex, "%X", CanFDFrame.data.bytes[j]);
		}
	}
	if(found == 1){
		char delim[] = " ";
		sprintf(inst,"%s",Configs[i].Intsructions);
		uint32_t k = 0, c = 0;
	    char *ptr = strtok(inst, delim);

	    int digits = numPlaces(Configs[i].Distribution);

	    while(ptr != NULL){

			size = (int)(Configs[i].Distribution/pow(10,digits - c - 1))%10;

			switch(size){
				case 1:
					StrIndex += sprintf(WriteArray + StrIndex, ptr, CanFDFrame.data.bytes[k]);
					k++;
					break;
				case 2:
					StrIndex += sprintf(WriteArray + StrIndex, ptr, CanFDFrame.data.shorts[k/2]);
					k+=2;
					break;
				case 4:
					StrIndex += sprintf(WriteArray + StrIndex, ptr, CanFDFrame.data.ints[k/4]);
					k+=4;
					break;
				case 8:
					StrIndex += sprintf(WriteArray + StrIndex, ptr, CanFDFrame.data.longs[k/8]);
					k+=8;
					break;
			}
			c++;
	    	if(c == digits){
	    		break;
	    	}
	    	ptr = strtok(NULL, delim);
	    }
	}
	return StrIndex;
}

int CAN_Data_Process(char WriteArray[], int StrIndex){
	int found = 0, i = 0, size = 0;
	char inst[128];

	for(i = 0; i <(sizeof(*Configs)/sizeof(ReadInstruction)); i++){
		if(CanFrame.id == Configs[i].id){
			found = 1;
			break;
		}
	}

	if(found == 0){
		for(int j = 0; j < CanFrame.length; j++){
			StrIndex += sprintf(WriteArray + StrIndex, "%X", CanFrame.data.bytes[j]);
		  }
	}
	if(found == 1){
		char delim[] = " ";
		sprintf(inst,"%s",Configs[i].Intsructions);
		uint32_t k = 0, c = 0;
	    char *ptr = strtok(inst, delim);

	    int digits = numPlaces(Configs[i].Distribution);

	    while(ptr != NULL){

			size = (int)(Configs[i].Distribution/pow(10,digits - c - 1))%10;

			switch(size){
				case 1:
					StrIndex += sprintf(WriteArray + StrIndex, ptr, CanFrame.data.bytes[k]);
					k++;
					break;
				case 2:
					StrIndex += sprintf(WriteArray + StrIndex, ptr, CanFrame.data.shorts[k/2]);
					k+=2;
					break;
				case 4:
					StrIndex += sprintf(WriteArray + StrIndex, ptr, CanFrame.data.ints[k/4]);
					k+=4;
					break;
				case 8:
					StrIndex += sprintf(WriteArray + StrIndex, ptr, CanFrame.data.longs[k/8]);
					k+=8;
					break;
			}
			c++;
	    	if(c == digits){
	    		break;
	    	}
	    	ptr = strtok(NULL, delim);
	    }
	}
	return StrIndex;
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
						CanFrame.length = RxHeader.DataLength/65536;
						CanFrame.time = time;
						CAN_Flag = 1;
						memcpy(CanFrame.data.bytes, RxData, CanFrame.length);
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
						memcpy(CanFDFrame.data.bytes, RxData, CanFDFrame.length);
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
