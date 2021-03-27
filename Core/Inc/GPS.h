/*
 * GPS.h
 *
 *  Created on: Mar 26, 2021
 *      Author: adamf
 */
#include "main.h"
#include <stdint.h>

#ifndef INC_GPS_H_
#define INC_GPS_H_

int gps_msg_config(UART_HandleTypeDef *huart, const char *nema, int rus1);
int gps_rate_config(UART_HandleTypeDef *huart, uint16_t measRate, uint16_t navRate, uint16_t timeRef);
int get_date_time(UART_HandleTypeDef *huart);

#endif /* INC_GPS_H_ */
