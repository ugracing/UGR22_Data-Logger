/*
 * myfatfs.h
 *
 *  Created on: Apr 18, 2021
 *      Author: adamf
 */

#ifndef INC_MYFATFS_H_
#define INC_MYFATFS_H_

#include "fatfs.h"

extern FATFS myFATAFS;
extern FIL myFILE;
extern FIL Config;
extern UINT ConfByteR;
extern UINT ConfByteW;
extern UINT testByte;

#endif /* INC_MYFATFS_H_ */
