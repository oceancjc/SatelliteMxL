/*
* Copyright (c) 2011-2013 MaxLinear, Inc. All rights reserved
*
* License type: GPLv2
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*
* This program may alternatively be licensed under a proprietary license from
* MaxLinear, Inc.
*
* See terms and conditions defined in file 'LICENSE.txt', which is part of this
* source code package.
*/

#include "MxLWare_HYDRA_OEM_Defines.h"
#include "MxLWare_HYDRA_OEM_Drv.h"
#include <sys/time.h>
#include <pthread.h>
#include "gpio_i2c.h"
#ifdef __MXL_OEM_DRIVER__
#include "MxL_HYDRA_I2cWrappers.h"
#endif


#define I2CBase		0x60
#define	I2C_Line_Single
pthread_mutex_t mut[4]; 

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_DeviceReset
 *
 * @param[in]   devId           Device ID
 *
 * @author Mahee
 *
 * @date 06/12/2012 Initial release
 *
 * This API performs a hardware reset on Hydra SOC identified by devId.
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
MXL_STATUS_E MxLWare_HYDRA_OEM_DeviceReset(UINT8 devId)
{
    MXL_STATUS_E mxlStatus = MXL_SUCCESS;

    reset(devId);
    return mxlStatus;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_SleepInMs
 *
 * @param[in]   delayTimeInMs
 *
 * @author Mahee
 *
 * @date 06/12/2012 Initial release
 *
 * his API will implement delay in milliseconds specified by delayTimeInMs.
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
void MxLWare_HYDRA_OEM_SleepInMs(UINT16 delayTimeInMs)
{
    usleep(1000*delayTimeInMs);

    #ifdef __MXL_OEM_DRIVER__
    Sleep(delayTimeInMs);
    #endif

}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_GetCurrTimeInMs
 *
 * @param[out]   msecsPtr
 *
 * @author Mahee
 *
 * @date 06/12/2012 Initial release
 *
 * This API should return system time milliseconds.
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
void MxLWare_HYDRA_OEM_GetCurrTimeInMs(UINT64 *msecsPtr)
{
    struct timeval currTime;
    #if __WORDSIZE == 32  
    static unsigned long long sec_count = 0;
    unsigned long long mask, incrementValue;
    #endif  
    unsigned long long timeInUsec;

    MXL_HYDRA_DEBUG(" MxL_HRCLS_Ctrl_GetTimeTickInUsec ");
    gettimeofday(&currTime, NULL);

    #if __WORDSIZE == 32  
    incrementValue = ((unsigned long long) 1 << (sizeof(currTime.tv_sec) * 8));
    mask = incrementValue-1;  

    if ((unsigned long long) currTime.tv_sec < (sec_count & mask))	sec_count += incrementValue;
    sec_count &= ~(mask);
    sec_count |= (currTime.tv_sec & mask);
    timeInUsec = (unsigned long long)(sec_count * 1000);
    #else
    timeInUsec = (unsigned long long)((unsigned long long) currTime.tv_sec * 1000;
    #endif
    *msecsPtr = (UINT64) timeInUsec;

}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_I2cWrite
 *
 * @param[in]   devId           Device ID
 * @param[in]   dataLen
 * @param[in]   buffPtr
 *
 * @author Mahee
 *
 * @date 06/12/2012 Initial release
 *
 * This API performs an I2C block write by writing data payload to Hydra device.
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/

MXL_STATUS_E MxLWare_HYDRA_OEM_I2cWrite(UINT8 devId, UINT16 dataLen, UINT8 *buffPtr)
{
    MXL_STATUS_E status = MXL_SUCCESS;
    i2c_start(devId);
    UINT8 i2cId =I2CBase | (devId & 0x07);
    
    if(i2c_sendbyte(devId,i2cId<<1) != 0){
        i2c_stop(devId);
        MXL_HYDRA_ERROR("[ERR -1] Func:%s, Line:%d, i2c no ACK Response in address, devId = %d\n",
                        __func__, __LINE__, devId);
        g_status |= 1<< (4 + devId);	
        return MXL_FAILURE;
    }
    
    UINT16 i = 0;
    for (i = 0;i<dataLen;i++){
        if(i2c_sendbyte( devId,buffPtr[i] ) != 0){
            i2c_stop(devId);
            MXL_HYDRA_ERROR("[ERR -1] Func:%s, Line:%d, i2c no ACK Response in data, devId = %d\n", 
                            __func__, __LINE__, devId);
            g_status |= 1<< (4 + devId);	
            return MXL_FAILURE;		
        }		
        
    }
    i2c_stop(devId);
    return status;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_I2cRead
 *
 * @param[in]   devId           Device ID
 * @param[in]   dataLen
 * @param[out]  buffPtr
 *
 * @author Mahee
 *
 * @date 06/12/2012 Initial release
 *
 * This API shall be used to perform I2C block read transaction.
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
MXL_STATUS_E MxLWare_HYDRA_OEM_I2cRead(UINT8 devId, UINT16 dataLen, UINT8 *buffPtr)
{
    MXL_STATUS_E status = MXL_SUCCESS;
    i2c_start(devId);
    UINT8 i2cId =I2CBase | (devId & 0x03);
    if(i2c_sendbyte(devId, (i2cId<<1) | 0x01 ) != 0){
        i2c_stop(devId);
        MXL_HYDRA_ERROR("[ERR Read Block] Func:%s, Line:%d, i2c no ACK Response read block I2C_add, devId = %d\n",
                        __func__, __LINE__, devId);
        g_status |= 1<< (4 + devId);
        return MXL_FAILURE;
    }
    UINT16 i;
    for(i=0;i!=dataLen-1;i++){
        if(0 != i2c_receivebyte(devId, &buffPtr[i], 0) ){
            MXL_HYDRA_ERROR("[ERR Read Block] Func:%s, Line:%d, Read Register read block data, devId = %d\n", 
                            __func__, __LINE__, devId);
            g_status |= 1<< (4 + devId);	
            return MXL_FAILURE;
        }    
    }
    if(0 != i2c_receivebyte(devId, &buffPtr[i], 1) ){
        MXL_HYDRA_ERROR("[ERR Read Block] Func:%s, Line:%d, Read Register Step NonACK Failed, devId = %d\n",
                        __func__, __LINE__, devId);
        g_status |= 1<< (4 + devId);
        return MXL_FAILURE;
    }  
    i2c_stop(devId);

    return status;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_InitI2cAccessLock
 *
 * @param[in]   devId           Device ID
 *
 * @author Mahee
 *
 * @date 04/11/2013 Initial release
 *
 * This API will initilize locking mechanism used to serialize access for
 * I2C operations.
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
MXL_STATUS_E MxLWare_HYDRA_OEM_InitI2cAccessLock(UINT8 devId)
{
    if(0!=pthread_mutex_init(&mut[devId], NULL)){
        MXL_HYDRA_ERROR("Mutex Init fail... mutex num = %d\n", devId);
        return MXL_FAILURE;
    }	

    pthread_mutex_init(&mut[devId], NULL);
    return MXL_SUCCESS;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_DeleteI2cAccessLock
 *
 * @param[in]   devId           Device ID
 *
 * @author Mahee
 *
 * @date 04/11/2013 Initial release
 *
 * This API will release locking mechanism and associated resources.
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
MXL_STATUS_E MxLWare_HYDRA_OEM_DeleteI2cAccessLock(UINT8 devId)
{
    if(0!=pthread_mutex_destroy (&mut[devId]))	return MXL_FAILURE;
    return MXL_SUCCESS;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_Lock
 *
 * @param[in]   devId           Device ID
 *
 * @author Mahee
 *
 * @date 04/11/2013 Initial release
 *
 * This API will should be used to lock access to device i2c access
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
MXL_STATUS_E MxLWare_HYDRA_OEM_Lock(UINT8 devId)
{
    if(0!=pthread_mutex_lock(&mut[devId]))	return MXL_FAILURE;
    return MXL_SUCCESS;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_Unlock
 *
 * @param[in]   devId           Device ID
 *
 * @author Mahee
 *
 * @date 04/11/2013 Initial release
 *
 * This API will should be used to unlock access to device i2c access
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
MXL_STATUS_E MxLWare_HYDRA_OEM_Unlock(UINT8 devId)
{
    if(0!=pthread_mutex_unlock(&mut[devId]))	return MXL_FAILURE;
    return MXL_SUCCESS;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_MemAlloc
 *
 * @param[in]   sizeInBytes
 *
 * @author Sateesh
 *
 * @date 04/23/2015 Initial release
 *
 * This API shall be used to allocate memory.
 *
 * @retval memPtr                 - Memory Pointer
 *
 ************************************************************************/
void* MxLWare_HYDRA_OEM_MemAlloc(UINT32 sizeInBytes)
{
  void *memPtr = NULL;
  sizeInBytes = sizeInBytes;

  memPtr = malloc(sizeInBytes * sizeof(UINT8));

  return memPtr;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_MemFree
 *
 * @param[in]   memPtr
 *
 * @author Sateesh
 *
 * @date 04/23/2015 Initial release
 *
 * This API shall be used to free memory.
 *
 *
 ************************************************************************/
void MxLWare_HYDRA_OEM_MemFree(void *memPtr)
{
  free(memPtr);
  memPtr = NULL;
  

}
