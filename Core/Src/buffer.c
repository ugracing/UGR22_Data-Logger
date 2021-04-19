/*
 * buffer.c
 *
 *  Created on: Apr 18, 2021
 *      Author: Lewis Russell
 */
#include <stdio.h>
#include <string.h>
#include <buffer.h>
#include <myprintf.h>


DataBuff DataBuffer = {.Data.DataBuff = 0, .counter = 0};

int WriteToBuff(char Data[], int len){
  if(len <= 1 || len >= Bufflength){
    return 3; //Dude dont try and break it
  }

  if(DataBuffer.counter < Bufflength && DataBuffer.counter + len >= Bufflength){
    DataBuffer.counter = Bufflength;
    memcpy((DataBuffer.Data.DataBuff + DataBuffer.counter), Data, len);
    DataBuffer.counter += len;
    if(f_write(&myFILE, DataBuffer.Data.DataBuff1, sizeof(DataBuffer.Data.DataBuff1), &testByte) == 0){

      //memset(DataBuffer.Data.DataBuff1, 0, sizeof(DataBuffer.Data.DataBuff1)); //Clear buffer after writing
    	f_sync(&myFILE);
      printf("Buffer 1 has been written\n");
      return 1; //buffer 1 has been written
    }
    return 4; //could not write :(
  }

  if(DataBuffer.counter + len >= 2*Bufflength){
    DataBuffer.counter = 0;
    memcpy((DataBuffer.Data.DataBuff + DataBuffer.counter), Data, len);
    DataBuffer.counter += len;
    if(f_write(&myFILE, DataBuffer.Data.DataBuff2, sizeof(DataBuffer.Data.DataBuff2), &testByte) == 0){
      //memset(DataBuffer.Data.DataBuff2, 0, sizeof(DataBuffer.Data.DataBuff2)); //Clear buffer after writing
    	f_sync(&myFILE);
      printf("Buffer 2 has been written\n");
      return 2; //buffer 2 has been written
    }
    return 4; //could not write :(
  }
/*if(DataBuffer.counter + len >= 2*Bufflength){
	f_write(&myFILE, DataBuffer.Data.DataBuff, sizeof(DataBuffer.Data.DataBuff), &testByte);
	DataBuffer.counter = 0;
}*/
  memcpy((DataBuffer.Data.DataBuff + DataBuffer.counter), Data, len);
  DataBuffer.counter += len;
  return 0; //moved data to buffer
}
