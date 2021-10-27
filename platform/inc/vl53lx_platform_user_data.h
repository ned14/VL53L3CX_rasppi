
/* SPDX-License-Identifier: GPL-2.0+ OR BSD-3-Clause */
/******************************************************************************
 * Copyright (c) 2020, STMicroelectronics - All Rights Reserved

 This file is part of VL53LX and is dual licensed,
 either GPL-2.0+
 or 'BSD 3-clause "New" or "Revised" License' , at your option.
 ******************************************************************************
 */



#ifndef _VL53LX_PLATFORM_USER_DATA_H_
#define _VL53LX_PLATFORM_USER_DATA_H_

#ifndef __KERNEL__
#include <stdlib.h>
#endif

#include "vl53lx_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

  int VL53LX_i2c_init(char* devPath, int devAddr);
  int32_t VL53LX_i2c_close(void);

  /**
   * @file vl53l0x_platform.h
   *
   * @brief All end user OS/platform/application porting
   */

   /**
    * @defgroup VL53LX_platform_group VL53LX Platform Functions
    * @brief    VL53LX Platform Functions
    *  @{
    */

    /**
     * @struct  VL53LX_Dev_t
     * @brief    Generic PAL device type that does link between API and platform abstraction layer
     *
     */
  typedef struct {
    VL53LX_DevData_t Data;               /*!< embed ST Ewok Dev  data as "Data"*/

    /*!< user specific field */
    uint8_t   I2cDevAddr;                /*!< i2c device address user specific field */
    int   fd;
  } VL53LX_Dev_t;


  /**
   * @brief   Declare the device Handle as a pointer of the structure @a VL53LX_Dev_t.
   *
   */
  typedef VL53LX_Dev_t* VL53LX_DEV;



#define VL53LXDevDataGet(Dev, field) (Dev->Data.field)



#define VL53LXDevDataSet(Dev, field, VL53LX_p_003) ((Dev->Data.field) = (VL53LX_p_003))



#define VL53LXDevStructGetLLDriverHandle(Dev) (&Dev->Data.LLData)



#define VL53LXDevStructGetLLResultsHandle(Dev) (&Dev->Data.llresults)


#ifdef __cplusplus
}
#endif

#endif


