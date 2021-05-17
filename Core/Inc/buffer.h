/*
 * buffer.h
 *
 *  Created on: Apr 18, 2021
 *      Author: Lewis Russell
 */

#ifndef INC_BUFFER_H_
#define INC_BUFFER_H_

//includes
#include "string.h"
#include <myfatfs.h>

//defines
#define Bufflength 131072
//#define Bufflength 1024



//structs and unions
typedef union {
  uint64_t value;
  struct {
    uint32_t low;
    uint32_t high;
  };
  struct {
    uint16_t s0;
    uint16_t s1;
    uint16_t s2;
    uint16_t s3;
  };
  uint8_t bytes[8];
} BytesUnion8;

typedef struct{
        uint32_t id;        // EID if ide set, SID otherwise
        uint8_t extended;   // Extended ID flag
        uint8_t length;     // Number of data bytes
        uint32_t time;		// time since system start in millis
        BytesUnion8 data;   // 64 bits - lots of ways to access it.
} CAN_FRAME;

typedef union {
  uint64_t longs[8];
  uint32_t ints[16];
  uint16_t shorts[32];
  uint8_t bytes[64];
} BytesUnion64;

typedef struct{
        uint32_t id;        // EID if ide set, SID otherwise
        uint8_t extended;   // Extended ID flag
        uint32_t length;    // Number of data bytes
        uint32_t time;		// time since system start in millis
        BytesUnion64 data;  // 64 bits - lots of ways to access it.
} CAN_FD_FRAME;

typedef union{
        char DataBuff[2*Bufflength];
        struct{
          char DataBuff1[Bufflength];
          char DataBuff2[Bufflength];
        };
}Buff;

typedef struct{
  Buff Data;
  UINT counter;
}DataBuff;

//functions
int WriteToBuff(char Data[], int len);

//variables
extern DataBuff DataBuffer;

#endif /* INC_BUFFER_H_ */
