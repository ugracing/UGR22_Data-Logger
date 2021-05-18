/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <GPS.h>
#include <CANFD.h>
#include <myprintf.h>
#include <buffer.h>
#include "MY_NRF24.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sDate = {0};


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

FDCAN_HandleTypeDef hfdcan1;
FDCAN_HandleTypeDef hfdcan2;

RTC_HandleTypeDef hrtc;

SD_HandleTypeDef hsd1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart8;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */

extern CAN_FRAME CanFrame;
extern CAN_FD_FRAME CanFDFrame;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SDMMC1_SD_Init(void);
static void MX_FDCAN1_Init(void);
static void MX_FDCAN2_Init(void);
static void MX_UART8_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//Telemetry stuff
uint64_t TxpipeAddrs = 0x11223344AA;
char myTxData[32];

int end_flag = 1;
char close_msg[] = "\n\r emergency shutdown";
int fileNum = 1;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SDMMC1_SD_Init();
  MX_FATFS_Init();
  MX_USB_DEVICE_Init();
  MX_FDCAN1_Init();
  MX_FDCAN2_Init();
  MX_UART8_Init();
  MX_USART3_UART_Init();
  MX_SPI1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  //HAL_Delay(1000);//NEEDED FOR USB MASS STORAGE TO WORK
  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
  //Telemetry
  NRF24_begin(TELE_CE_GPIO_Port, TELE_CS_Pin, TELE_CE_Pin, hspi1);
    //nrf24_DebugUART_Init(huart3);



    // TRANSMIT NO ACK //
    NRF24_stopListening();
    NRF24_openWritingPipe(TxpipeAddrs);
    NRF24_setAutoAck(false);
    NRF24_setChannel(42);
    NRF24_setPayloadSize(32);

    printRadioSettings();
  //END Telemetry
  printf("Starting\n");
  FDCAN_Config(&hfdcan1);
  //GPS DMA
  HAL_UART_Receive_DMA(&huart3, (uint8_t *)rxBuf, BUFFLENGTH);
  HAL_Delay(1000);//GPS required this in testing, maybe remove?
  //variables required for 10Hz GPS
  uint16_t measRate = 100;
  uint16_t navRate = 1;
  uint16_t timeRef = 0;
  gps_rate_config(&huart3, measRate, navRate, timeRef);

  gps_msg_config(&huart3, "DTM", 0);//ERROR
  gps_msg_config(&huart3, "GAQ", 0);
  gps_msg_config(&huart3, "GBQ", 0);
  gps_msg_config(&huart3, "GBS", 0);//ERROR
  gps_msg_config(&huart3, "GGA", 0);//ERROR
  gps_msg_config(&huart3, "GLL", 0);//ERROR
  gps_msg_config(&huart3, "GLQ", 0);
  gps_msg_config(&huart3, "GNQ", 0);
  gps_msg_config(&huart3, "GNS", 0);//ERROR
  gps_msg_config(&huart3, "GPQ", 0);
  gps_msg_config(&huart3, "GRS", 0);//ERROR
  gps_msg_config(&huart3, "GSA", 0);//ERROR
  gps_msg_config(&huart3, "GST", 0);//ERROR
  gps_msg_config(&huart3, "GSV", 0);//ERROR
  gps_msg_config(&huart3, "RLM", 0);
  gps_msg_config(&huart3, "RMC", 1);//ERROR
  gps_msg_config(&huart3, "TXT", 0);
  gps_msg_config(&huart3, "VLW", 0);//ERROR
  gps_msg_config(&huart3, "VTG", 0);//ERROR
  gps_msg_config(&huart3, "ZDA", 0);//ERROR

  if(f_mount(&myFATAFS, SDPath, 1) == FR_OK){
  	  //HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
  	  char Filename[20] = "test";
      char FilePath[150];
      char ConfigPath[] ="Config.csv\0";
      char ConfigParams[1000];

      //Tries to open config File
      if(f_open(&Config, ConfigPath, FA_READ) == FR_NO_FILE){
        //If file doesnt exist creates a file using hard coded defualts and passes those to internal config array
        f_open(&Config, ConfigPath, FA_WRITE | FA_CREATE_ALWAYS);

        sprintf(ConfigParams, "ID,Device,Bytes,Rate(HZ),Type,Description\n");
        sprintf(ConfigParams + strlen(ConfigParams),"0x50,Datalogger,8,0.20,uint32_t,HIGH:FileNO LOW:millis,\n");

        f_write(&Config, ConfigParams, strlen(ConfigParams), &ConfByteW);
      }else{
        //If file does exist reads in config parameters to internal config array
        f_read(&Config, ConfigParams, strlen(ConfigParams), &ConfByteR);
      }
      f_close(&Config);
      //MAKE NEW FILE INCREMENTED BY 1
      
      strcpy(FilePath,Filename);
      int FilePathLen = strlen(Filename);
      sprintf(FilePath + FilePathLen, "%i.csv",fileNum);
      while(f_open(&myFILE, FilePath, FA_READ) == FR_OK){
    	  f_close(&myFILE);
        fileNum++;
        strcpy(FilePath,Filename);
        sprintf(FilePath + FilePathLen, "%i.csv",fileNum);
      }
      f_open(&myFILE, FilePath, FA_WRITE | FA_CREATE_ALWAYS);

  }

  	  //write speed test
  	  	/*for(int i = 0; i<131072; i++){
  		    DataBuffer.Data.DataBuff1[i] = 'A';
  	    }
  	    int start = HAL_GetTick();
  	    for(int i = 0; i<2048; i++){
  		f_write(&myFILE, DataBuffer.Data.DataBuff1, sizeof(DataBuffer.Data.DataBuff1), &testByte);
        }
  	    int end = HAL_GetTick();
  	    int duration = end - start;
  	    char myTime[200];
  	    sprintf(myTime, "\r%i", duration);
  	    f_write(&myFILE, myTime, strlen(myTime), &testByte);
        f_close(&myFILE);*/

  extern int GPS_flag;
  extern int FDCAN_Flag;
  extern int CAN_Flag;


  CAN_FD_FRAME FDBuffer[50] = {0};
  CAN_FRAME Buffer[50] = {0};
  uint32_t Tele_IDs[50] = {0};
  Tele_IDs[0] = 0x321;

  sTime.SecondFraction = 999;
  uint32_t LocalTime = HAL_GetTick();
  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

  sTime.SubSeconds -= LocalTime % 1000;
  LocalTime = LocalTime/1000;
  sTime.Seconds -= LocalTime % 60;
  LocalTime = LocalTime/60;
  sTime.Minutes -= LocalTime % 60;
  LocalTime = LocalTime/60;
  sTime.Hours -= LocalTime % 60;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  char CanWrite[400];
  int CW = 0;
  char CanFDWrite[400];
  int CFDW = 0;
  int AllowedTele = 1;
  int BuffIndex = 0;
  int Txcnt = 0;
  int TxDataSpace = 0;
  uint32_t LoopTime = 10; //time a loop should take (this is aguess should test avg time)
  uint32_t StartTime = 0;
  uint32_t EndTime = 0;
  uint32_t radiodelay = 1000;
  uint32_t r_start = 0;
  uint32_t r_end = 0;
  r_start = HAL_GetTick();

  while (end_flag){
    StartTime = HAL_GetTick();
	  if(GPS_flag){
		  WriteToBuff(rxBuf, sizeof(rxBuf));
		  GPS_flag=0;
	  }
	  
    r_end = HAL_GetTick();
    if(r_start - r_end > radiodelay){
      r_start = HAL_GetTick();
      for(int i = 0; i < AllowedTele; i++){
        //make packet (Time ID data)
        Txcnt = sprintf(myTxData,"%u %x ",FDBuffer[BuffIndex].time, FDBuffer[BuffIndex].id);
        TxDataSpace = 32 - Txcnt;
        if(FDBuffer[BuffIndex].length > TxDataSpace){
          for(int i = 0; i < TxDataSpace; i++){
            Txcnt += sprintf(myTxData + Txcnt, "%c", FDBuffer[BuffIndex].data.bytes[i]);
          }
          NRF24_write(myTxData, 32);
          Txcnt = 0;
          if(FDBuffer[BuffIndex].length - TxDataSpace > 32){
            for(int i = 0; i < 32; i++){
              Txcnt += sprintf(myTxData + Txcnt, "%c", FDBuffer[BuffIndex].data.bytes[i + TxDataSpace]);
            }
            Txcnt = 0;
            i++;
            NRF24_write(myTxData, 32);
            for(int i = 0; i < FDBuffer[BuffIndex].length - TxDataSpace+32; i++){
              Txcnt += sprintf(myTxData + Txcnt, "%c", FDBuffer[BuffIndex].data.bytes[i + TxDataSpace + 32]);
            }
            i++;
            NRF24_write(myTxData, FDBuffer[BuffIndex].length - (TxDataSpace + 32));
          }else{
            Txcnt = 0;
            for(int i = 0; i < FDBuffer[BuffIndex].length - TxDataSpace; i++){
              Txcnt += sprintf(myTxData + Txcnt, "%c", FDBuffer[BuffIndex].data.bytes[i + TxDataSpace]);
            }
            i++;
            NRF24_write(myTxData, FDBuffer[BuffIndex].length - TxDataSpace);
          }
        }
        else{
          for(int i = 0; i < FDBuffer[BuffIndex].length; i++){
            Txcnt += sprintf(myTxData + Txcnt, "%c", FDBuffer[BuffIndex].data.bytes[i]);
          }
          NRF24_write(myTxData, FDBuffer[BuffIndex].length + (32 - TxDataSpace));
        }
        BuffIndex++;
        if(FDBuffer[BuffIndex].id == 0){
          BuffIndex = 0;
        }
      }
    }
    // if(NRF24_write(myTxData, 32)){
		//   	  printf("TeleSending\n\r");
	  // 		  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
	  // 		  HAL_Delay(1000);
	  // 	  }
	  /*HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);


	  printf("Date:%u Month:%u Year:%u\n", sDate.Date, sDate.Month, sDate.Year);
	  printf("Hours:%u Minutes:%u Seconds:%u\n", sTime.Hours, sTime.Minutes, sTime.Seconds);*/

	  if(FDCAN_Flag){

		  RTC_TimeTypeDef lTime = sTime;
		  LocalTime = CanFDFrame.time;
		  lTime.SubSeconds += LocalTime % 1000;
		  LocalTime = LocalTime/1000;
		  lTime.Seconds += LocalTime % 60;
		  LocalTime = LocalTime/60;
		  lTime.Minutes += LocalTime % 60;
		  LocalTime = LocalTime/60;
		  lTime.Hours += LocalTime % 60;
      //Write to SD Card
		  //date/time, CANID, Data
		  CFDW = sprintf(CanFDWrite, "%u.%u.%u %u:%u:%u.%u,0x%X,",
				  sDate.Date,sDate.Month,sDate.Year, lTime.Hours,lTime.Minutes,lTime.Seconds,lTime.SubSeconds,
				  CanFDFrame.id);
		  for(int i = 0; i < CanFDFrame.length; i++){
			  CFDW += sprintf(CanFDWrite + CFDW, "%c", CanFDFrame.data.bytes[i]);
		  }
		  CFDW += sprintf(CanFDWrite + CFDW, "\n\r");
		  WriteToBuff(CanFDWrite, CFDW);

      //Write to telemetry buffer
      
      for(int i = 0; i <50; i++){
        if(CanFDFrame.id == Tele_IDs[i]){ //check if frame should be checked
          for(int j = 0; j < 50; j++){
            if(FDBuffer[j].id == CanFDFrame.id){
              FDBuffer[j] = CanFDFrame;
              goto TeleDoneFD;
            }
            if(FDBuffer[j].id == 0){
              FDBuffer[j] = CanFDFrame;
              goto TeleDoneFD;
            }
          }
        }
      }
TeleDoneFD:
		  FDCAN_Flag = 0;
	  }
	  if(CAN_Flag){

		  RTC_TimeTypeDef lTime = sTime;
		  LocalTime = CanFrame.time;
		  lTime.SubSeconds += LocalTime % 1000;
		  LocalTime = LocalTime/1000;
		  lTime.Seconds += LocalTime % 60;
		  LocalTime = LocalTime/60;
		  lTime.Minutes += LocalTime % 60;
		  LocalTime = LocalTime/60;
		  lTime.Hours += LocalTime % 60;

		  //date/time, CANID, Data
		  CW = sprintf(CanWrite, "%u.%u.%u %u:%u:%u.%u,0x%X,",
				  sDate.Date,sDate.Month,sDate.Year, lTime.Hours,lTime.Minutes,lTime.Seconds,lTime.SubSeconds,
				  CanFrame.id);
		  for(int i = 0; i < CanFDFrame.length; i++){
			  CW += sprintf(CanWrite + CW, "%c", CanFrame.data.bytes[i]);
		  }
		  CW += sprintf(CanWrite + CW, "\n\r");
		  WriteToBuff(CanWrite, CW);

      for(int i = 0; i <50; i++){
        if(CanFrame.id == Tele_IDs[i]){ //check if frame should be checked
          for(int j = 0; j < 50; j++){
            if(Buffer[j].id == CanFrame.id){
              Buffer[j] = CanFrame;     //if its already in the buffer replace it
              goto TeleDone;
            }
            if(Buffer[j].id == 0){
              Buffer[j] = CanFrame;     //if you get to the end of the defined packets withoout finding anything add the packet on the end
              goto TeleDone;
            }
          }
        }
      }
      printf("couldnt track, id buffer too full :(");
TeleDone:
		  CAN_Flag = 0;
	  }
	  //WriteToBuff(A, 2);
	  //HAL_Delay(1000);
	  //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    /* USER CODE END WHILE */
    EndTime = HAL_GetTick();
    if(StartTime - EndTime > LoopTime + 5){
      AllowedTele--;
    }else if(StartTime - EndTime < LoopTime){
      AllowedTele++;
    }if (AllowedTele<1){
    	AllowedTele = 1;
    }
    /* USER CODE BEGIN 3 */
}

    //f_write(&myFILE, close_msg, strlen(close_msg), &testByte);
    //f_close(&myFILE);
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
    //printf("File closed\n");

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_HIGH);
  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 10;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART3
                              |RCC_PERIPHCLK_FDCAN|RCC_PERIPHCLK_UART8
                              |RCC_PERIPHCLK_SPI1|RCC_PERIPHCLK_SDMMC
                              |RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.PLL2.PLL2M = 2;
  PeriphClkInitStruct.PLL2.PLL2N = 12;
  PeriphClkInitStruct.PLL2.PLL2P = 3;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 3;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL2;
  PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL2;
  PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
  /** Enable USB Voltage detector
  */
  HAL_PWREx_EnableUSBVoltageDetector();
}

/**
  * @brief FDCAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = DISABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 1;
  hfdcan1.Init.NominalSyncJumpWidth = 23;
  hfdcan1.Init.NominalTimeSeg1 = 56;
  hfdcan1.Init.NominalTimeSeg2 = 23;
  hfdcan1.Init.DataPrescaler = 1;
  hfdcan1.Init.DataSyncJumpWidth = 5;
  hfdcan1.Init.DataTimeSeg1 = 10;
  hfdcan1.Init.DataTimeSeg2 = 5;
  hfdcan1.Init.MessageRAMOffset = 0;
  hfdcan1.Init.StdFiltersNbr = 2;
  hfdcan1.Init.ExtFiltersNbr = 2;
  hfdcan1.Init.RxFifo0ElmtsNbr = 64;
  hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_64;
  hfdcan1.Init.RxFifo1ElmtsNbr = 0;
  hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxBuffersNbr = 0;
  hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.TxEventsNbr = 0;
  hfdcan1.Init.TxBuffersNbr = 0;
  hfdcan1.Init.TxFifoQueueElmtsNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  /* USER CODE END FDCAN1_Init 2 */

}

/**
  * @brief FDCAN2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN2_Init(void)
{

  /* USER CODE BEGIN FDCAN2_Init 0 */

  /* USER CODE END FDCAN2_Init 0 */

  /* USER CODE BEGIN FDCAN2_Init 1 */

  /* USER CODE END FDCAN2_Init 1 */
  hfdcan2.Instance = FDCAN2;
  hfdcan2.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan2.Init.AutoRetransmission = DISABLE;
  hfdcan2.Init.TransmitPause = DISABLE;
  hfdcan2.Init.ProtocolException = DISABLE;
  hfdcan2.Init.NominalPrescaler = 1;
  hfdcan2.Init.NominalSyncJumpWidth = 1;
  hfdcan2.Init.NominalTimeSeg1 = 2;
  hfdcan2.Init.NominalTimeSeg2 = 2;
  hfdcan2.Init.DataPrescaler = 1;
  hfdcan2.Init.DataSyncJumpWidth = 1;
  hfdcan2.Init.DataTimeSeg1 = 1;
  hfdcan2.Init.DataTimeSeg2 = 1;
  hfdcan2.Init.MessageRAMOffset = 0;
  hfdcan2.Init.StdFiltersNbr = 0;
  hfdcan2.Init.ExtFiltersNbr = 0;
  hfdcan2.Init.RxFifo0ElmtsNbr = 0;
  hfdcan2.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan2.Init.RxFifo1ElmtsNbr = 0;
  hfdcan2.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan2.Init.RxBuffersNbr = 0;
  hfdcan2.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
  hfdcan2.Init.TxEventsNbr = 0;
  hfdcan2.Init.TxBuffersNbr = 0;
  hfdcan2.Init.TxFifoQueueElmtsNbr = 0;
  hfdcan2.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan2.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  if (HAL_FDCAN_Init(&hfdcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN2_Init 2 */

  /* USER CODE END FDCAN2_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 1;
  hrtc.Init.SynchPrediv = 16388;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 1;
  sDate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC1_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */

  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
  hsd1.Init.ClockDiv = 0;
  hsd1.Init.TranceiverPresent = SDMMC_TRANSCEIVER_NOT_PRESENT;
  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief UART8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART8_Init(void)
{

  /* USER CODE BEGIN UART8_Init 0 */

  /* USER CODE END UART8_Init 0 */

  /* USER CODE BEGIN UART8_Init 1 */

  /* USER CODE END UART8_Init 1 */
  huart8.Instance = UART8;
  huart8.Init.BaudRate = 115200;
  huart8.Init.WordLength = UART_WORDLENGTH_8B;
  huart8.Init.StopBits = UART_STOPBITS_1;
  huart8.Init.Parity = UART_PARITY_NONE;
  huart8.Init.Mode = UART_MODE_TX_RX;
  huart8.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart8.Init.OverSampling = UART_OVERSAMPLING_16;
  huart8.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart8.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart8.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart8, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart8, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart8) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART8_Init 2 */

  /* USER CODE END UART8_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 38400;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, RS_485_TX_EN_Pin|RS_485_RX_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, TELE_CE_Pin|TELE_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPS_RST_GPIO_Port, GPS_RST_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : RS_485_TX_EN_Pin RS_485_RX_EN_Pin */
  GPIO_InitStruct.Pin = RS_485_TX_EN_Pin|RS_485_RX_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : TELE_CE_Pin TELE_CS_Pin */
  GPIO_InitStruct.Pin = TELE_CE_Pin|TELE_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : TELE_IRQ_Pin */
  GPIO_InitStruct.Pin = TELE_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TELE_IRQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_INT_Pin */
  GPIO_InitStruct.Pin = BTN_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : GPS_RST_Pin */
  GPIO_InitStruct.Pin = GPS_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPS_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CARD_DET_Pin */
  GPIO_InitStruct.Pin = CARD_DET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(CARD_DET_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */

  end_flag = 0;

}
void HAL_PWR_PVDCallback (void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PWR_PVDCallback can be implemented in the user file
  */
	//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
	end_flag = 0;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
	  printf("Error Handler\n");
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
