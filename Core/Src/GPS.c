/*
 * GPS.c
 *
 *  Created on: Mar 26, 2021
 *      Author: Charlie McCarthy
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <GPS.h>
#include "main.h"
#include <myprintf.h>
#include <stdbool.h>
#include <buffer.h>

int gps_msg_config(UART_HandleTypeDef *huart, const char *nema, int rus1){
	if (strlen(nema) != 3) return 1;


	char msg[32];
	snprintf(msg, sizeof(msg)-1,"PUBX,40,%s,0,%i,0,0,0,0", nema, rus1);

	int checksum_int = 0;
	for (int i = 0; msg[i] && i < 32; i++)
		checksum_int ^= (unsigned char)msg[i];

	char checksum[8];
	snprintf(checksum, sizeof(checksum)-1, "*%.2X", checksum_int);

	char messagetosend[42];
	snprintf(messagetosend, sizeof(messagetosend)-1, "$%s%s\r\n", msg, checksum);
	printf("\n\rSending: ");
	printf("%s", messagetosend);
	if (HAL_UART_Transmit(huart, messagetosend, strlen(messagetosend), 100) != HAL_OK){
		Error_Handler();
	}
	return 0;
}

int gps_rate_config(UART_HandleTypeDef *huart, uint16_t measRate, uint16_t navRate, uint16_t timeRef){
		uint16_t length = 6;
		uint8_t msg[14];
		msg[0] = 0xb5;
		msg[1] = 0x62;
		msg[2] = 0x06;
		msg[3] = 0x08;
		msg[4] = length;
		msg[5] = 0;
		msg[6] = measRate;
		msg[7] = 0;
		msg[8] = navRate;
		msg[9] = 0;
		msg[10] = timeRef;
		msg[11] = 0;

		uint8_t CK_A = 0;
		uint8_t CK_B = 0;
		for(int i=2; i<12; i++){
	        CK_A = CK_A + msg[i];
	        CK_A &= 0xFF;
	        CK_B = CK_B + CK_A;
	        CK_B &= 0xFF;
	    }
		msg[12] = CK_A;
		msg[13] = CK_B;

		if (HAL_UART_Transmit(huart, msg, sizeof(msg), 100) != HAL_OK){
				Error_Handler();
				return 1;
			}
		return 0;
}

int get_date_time(UART_HandleTypeDef *huart){

    char get_time_msg[9];
    snprintf(get_time_msg, sizeof(get_time_msg)-1, "PUBX,04");

    int checksum_int = 0;
    for (int i = 0; get_time_msg[i] && i < 32; i++)
        checksum_int ^= (unsigned char)get_time_msg[i];
    char checksum[8];
    snprintf(checksum, sizeof(checksum)-1, "*%.2X", checksum_int);

    char get_time_msg_to_send[20];
    snprintf(get_time_msg_to_send, sizeof(get_time_msg_to_send)-1, "$%s%s\r\n", get_time_msg, checksum);

    //SOME UART BS GOES HERE TO SEND MESSAGE
    HAL_UART_Transmit(huart, get_time_msg_to_send, strlen(get_time_msg_to_send), 100);
    printf("\n\rSending: ");
    printf("%s", get_time_msg_to_send);
    char pubx_date_time_buffer[71];
    HAL_UART_Receive(huart, (uint8_t *)pubx_date_time_buffer, sizeof(pubx_date_time_buffer), 1000);
    printf("%s", pubx_date_time_buffer);

    return 0;

}

int GPS_flag = 0;

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_RxCpltCallback can be implemented in the user file.
   */
 /* if(count%2){
	  for(int i=0; i<BUFFLENGTH/2; i++){
		  printf("%c", rxBuf[i]);
	  }
  }
  else{
	  for(int i=BUFFLENGTH/2; i<BUFFLENGTH; i++){
	  		  printf("%c", rxBuf[i]);
	  }
  }*/
  //for(int i=0; i<BUFFLENGTH; i++){
  	  		  //printf("%c", rxBuf[i]);
  	  //}
  GPS_flag = 1;
  //WriteToBuff(rxBuf+70, BUFFLENGTH/2);
  //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);*/
  //printf("GPS Recieved\n");

}
