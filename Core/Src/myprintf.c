/*
 * myprintf.c
 *
 *  Created on: Apr 18, 2021
 *      Author: adamf
 */

#include <myprintf.h>
#include "main.h"

int _write(int file, char *ptr, int len)
{
  int i=0;
  for(i=0 ; i<len ; i++)
    ITM_SendChar((*ptr++));
  return len;
}
