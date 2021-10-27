/**
******************************************************************************
* File Name          : main.c
* Description        : Main program body
******************************************************************************
*
* COPYRIGHT(c) 2020 STMicroelectronics
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

/* USER CODE BEGIN Includes */
#include "vl53lx_api.h"
#include "vl53lx_platform.h"
/* USER CODE END Includes */
/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
VL53LX_Dev_t                   dev;
VL53LX_DEV                     Dev = &dev;
int status;
volatile int IntCount;
#define isInterrupt 1 /* If isInterrupt = 1 then device working in hardware interrupt mode, else device working in polling mode */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void RangingLoop(void); /*  */
/* USER CODE END PFP */

int main(void)
{
  /* USER CODE BEGIN 2 */
  
  printf("VL53L1X Examples...\n");

  Dev->I2cDevAddr = 0x52;
  Dev->fd = VL53LX_i2c_init("/dev/i2c-1", Dev->I2cDevAddr); //choose between i2c-0 and i2c-1; On the raspberry pi zero, i2c-1 are pins 2 and 3
  if (Dev->fd<0) {
      printf ("Failed to init\n");
      return 1;
  }
  
  uint8_t byteData;
  uint16_t wordData;
  VL53LX_RdByte(Dev, 0x010F, &byteData);
  printf("VL53LX Model_ID: %02X\n\r", byteData);
  VL53LX_RdByte(Dev, 0x0110, &byteData);
  printf("VL53LX Module_Type: %02X\n\r", byteData);
  VL53LX_RdWord(Dev, 0x010F, &wordData);
  printf("VL53LX: %02X\n\r", wordData);
  RangingLoop();
  
  /* USER CODE END 2 */
  
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    
    /* USER CODE BEGIN 3 */
    
  }
  /* USER CODE END 3 */
  
}

/* USER CODE BEGIN 4 */

/* ranging and display loop */
void RangingLoop(void)
{
  VL53LX_MultiRangingData_t MultiRangingData;
  VL53LX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
  uint8_t NewDataReady=0;
  int no_of_object_found=0,j;
  printf("Ranging loop starts\n");
  
  status = VL53LX_WaitDeviceBooted(Dev);
  status = VL53LX_DataInit(Dev);
  status = VL53LX_StartMeasurement(Dev);
  
  if(status){
    printf("VL53LX_StartMeasurement failed: error = %d \n", status);
    while(1);
  }
  
  do{ // polling mode
    status = VL53LX_GetMeasurementDataReady(Dev, &NewDataReady);                        
    usleep(1000); // 1 ms polling period, could be longer.
    if((!status)&&(NewDataReady!=0)){
      status = VL53LX_GetMultiRangingData(Dev, pMultiRangingData);
      no_of_object_found=pMultiRangingData->NumberOfObjectsFound;
      printf("Count=%5d, ", pMultiRangingData->StreamCount);
      printf("#Objs=%1d ", no_of_object_found);
      for(j=0;j<no_of_object_found;j++){
        if(j!=0)printf("\n                     ");
        printf("status=%d, D=%5dmm, S=%5dmm, Signal=%2.2f Mcps, Ambient=%2.2f Mcps",
               pMultiRangingData->RangeData[j].RangeStatus,
               pMultiRangingData->RangeData[j].RangeMilliMeter,
               pMultiRangingData->RangeData[j].SigmaMilliMeter,
               pMultiRangingData->RangeData[j].SignalRateRtnMegaCps/65536.0,
               pMultiRangingData->RangeData[j].AmbientRateRtnMegaCps/65536.0);
      }
      printf ("\n");
      if (status==0){
        status = VL53LX_ClearInterruptAndStartMeasurement(Dev);
      }
    }
  }
  while (1);
}

/* USER CODE END 4 */

/**
* @brief  This function is executed in case of error occurrence.
* @param  None
* @retval None
*/
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
* @brief Reports the name of the source file and the source line number
* where the assert_param error has occurred.
* @param file: pointer to the source file name
* @param line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
  
}

#endif

/**
* @}
*/ 

/**
* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
