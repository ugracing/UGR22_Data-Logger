/*
 * CANFD.c
 *
 *  Created on: Apr 18, 2021
 *      Author: adamf
 */

#include <CANFD.h>
#include <myprintf.h>

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
					HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
					printf("Packet Acquired!\n");

					/*
				  	  for(int i=0;i<64;i++){
				  			  printf("%c",RxData[i]);
				  	  }
				  	  printf("\n");
				  	*/
			      }
}
