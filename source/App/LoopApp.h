#ifndef __LOOPAPP_H__
#define __LOOPAPP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "MxLWare_HYDRA_DeviceApi.h"
#include "gpio_i2c.h"
#include "Net_Conf.h"
#include "Sate_func.h"
#include "protect.h"
#include "MxLWare_HYDRA_CommonApi.h"
#include "MaxLinearDataTypes.h"
#include "MxLWare_HYDRA_OEM_Drv.h"
#include "devip.h"
#include "sateTable.h"
#include "satethTable.h"
#include "consTable.h"
#include "sysdescr.h"
#include "trap.h"
#include "udpTable.h"
#include "globals.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UDP_RECEIVEPORT  13898
#define UDP_TRANSMITPORT 13899

    
typedef struct LoopApp_t LoopApp_t;
struct LoopApp_t {
    SATDEVICE_t*  satDevice;
    SATDATA_t*   sdPtr[MAX_CHANNEL_NUM];
    SATDATATH_t* sd_thPtr[MAX_CHANNEL_NUM];
    char** vi; char** vq;
    void(*LoopAppDevice_Init)(const LoopApp_t* loopdev);
    void(*LoopAppDevice_Clear)(const LoopApp_t* loopdev);
};

    
extern LoopApp_t* dev;
    
void loopAppDev_clear(LoopApp_t* dev);
int loopAppDev_init(LoopApp_t* dev, uint8_t devId, uint8_t* i2c_status);
MXL_STATUS_E mxl_lockDemodV2(LoopApp_t* dev, uint8_t update, MXL_HYDRA_TUNER_ID_E tunerId, MXL_HYDRA_DEMOD_ID_E demodId);
int lockAllDemodsV2(LoopApp_t* device, uint8_t tunerId, uint32_t* lockedchannels);
void regular_checkPerChannel(uint8_t devId, uint8_t demodId, SATDATA_t* sdPtr, SATDATATH_t* sdth);
void snmp_server_init(const char* app_name);
void snmp_server_loop(int blockinterval_second);
void snmp_server_close(const char* app_name);
    
int framerParam(LoopApp_t* dev, char* frame); 
int cmdAnalysis(char* cmd, char* opcode, int* opdata);

    
#ifdef __cplusplus
}
#endif

#endif // !__LOOPAPP_H__

