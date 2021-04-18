/*
 * myfatfs.h
 *
 *  Created on: Apr 18, 2021
 *      Author: adamf
 */

#ifndef INC_MYFATFS_H_
#define INC_MYFATFS_H_

#include "fatfs.h"

FATFS myFATAFS;
FIL myFILE;
FIL Config;
UINT ConfByteR;
UINT ConfByteW;
UINT testByte;

#endif /* INC_MYFATFS_H_ */
