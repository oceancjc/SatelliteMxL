#ifndef SATE_FUNC_H
#define SATE_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/route.h> 
#include <unistd.h>
#include "MxLWare_HYDRA_DeviceApi.h"
#include "MxLWare_HYDRA_CommonApi.h"
#include "MxLWare_HYDRA_DemodTunerApi.h"
#include "MaxLinearDataTypes.h"
#include "MxLWare_HYDRA_OEM_Drv.h"

#include "globals.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    
#define MAX_CHANNEL_NUM    (8)
#define MAX_TRAP_NUM       (4)
#define MAX_UDP_ADDR       (4)
#define FFT_FREAME_LEN    (1024)
#define CONST_LEN         (256)
#define INTERVAL_MS       (100)
    
    
typedef struct{
    int standardMask;
    int frequencyInKHz;
    int freqSearchRangeKHz; 
    int symbolRateKSps; 
    int spectrumInfo; 
    int fecFormat;
    int modulation;
    int rollOff;
    int pilots;
    int fecLock;
    int agcLock;
    int pwr_100dbm;
    int snr;
    int mer;
    int evm;
    int ebn0;
    int cfo_Hz;
    int fo_symbol;
    int ber_1e_7;
    int corrRsErrors;
    int rsErrors;
    int berWindow;
    int berCount;
    int corrRsErrors1;
    int rsErrors1;
    int berWindow_Iter1;
    int berCount_Iter1;
    int crcErrors;
    int packetErrorCount;
    int totalPackets;
    int16_t consti[256];
    int16_t constq[256];
    volatile uint8_t parachange;
}SATDATA_t;

typedef struct{
    int rxpwrLow;
    int rxpwrHigh;
    int snrLow;
    int berMax;
    int merLow;
    int evmLow;
    int cfoMax_Hz;
    int sfo_symbol; 
    int lockSwitch;    
}SATDATATH_t;


    
typedef struct {
    uint8_t devId;    
    uint8_t* i2c_status;
    oem_data_t oemdata;
}SATDEVICE_t;   
    
    


typedef struct{// ip address for snmptrap
    char ip[16];
    int port;
}TRAPIP_t;
#define UDPIP_t    TRAPIP_t




extern TRAPIP_t*      trapip;
extern uint8_t        trap_status;
extern char*          trap_string;


extern int            SW_MODE;
extern UDPIP_t*       udp_ipPtr;

extern MXL_STATUS_E MxL_582_Init(unsigned char devId, oem_data_t* oemdata);
extern MXL_STATUS_E mxl_lockDemod(uint32_t gstatus, uint8_t update, uint8_t devId,MXL_HYDRA_TUNER_ID_E tunerId,MXL_HYDRA_DEMOD_ID_E demodId, SATDATA_t* sdPtr );
extern int lock_Demod(SATDEVICE_t* dev, uint8_t tunerId, uint8_t demodId, SATDATA_t* data);
extern MXL_STATUS_E getfreqparameters(uint8_t devId, int demodId, SATDATA_t* data);
extern MXL_STATUS_E getlockstatus(uint8_t devId, int demodId, int* fecLock, int* agcLock);
extern MXL_STATUS_E waitdemodlock(uint8_t devId, int demodId, int* fecLock, int* agcLock, uint32_t timeout_ms);
extern MXL_STATUS_E get_pwr_snr(uint8_t devId,int demodId, int* pwr_100dbm, int* snr);
extern MXL_STATUS_E getcfo(uint8_t devId, int demodId, int* cfo_Hz, int* sfo_symbol);
extern MXL_STATUS_E getscaledber(uint8_t devId, int demodId, uint32_t* scaledBerIter1, uint32_t* scaledBerIter2);
extern MXL_STATUS_E getberErrcounters(uint8_t devId, int demodId, SATDATA_t* sdPtr);
extern int constInitV2(char** vi, char** vq, int channelPerDev, int numPerLine);
extern void constClear(char**vi, char**vq, int numOfdev, int channelPerDev);
extern MXL_STATUS_E getconst(uint8_t devId, int demodId, char ***vi, char ***vq,  int *index, char len, uint8_t fake_flag);
extern MXL_STATUS_E getconstV2(uint8_t devId, int demodId, char **vi, char **vq, SATDATA_t* sdPtr, int *index, char len, uint8_t fake_flag);
extern MXL_STATUS_E getspecturm(uint8_t devId, int demodId, int tunerId, int fs, uint32_t numofFreqSteps, int index_s, int point[FFT_FREAME_LEN][2]);
extern void regular_check(int i, SATDATA_t* sd, SATDATATH_t* sdth);
extern MXL_STATUS_E getRegularParamters(uint8_t devId, uint8_t demodId, SATDATA_t* sdPtr);
extern void remote_ip_load(TRAPIP_t* trapip);
extern void remote_ip_change(char index, TRAPIP_t* trapip);

extern void confile_update(char index, SATDATA_t* sdPtr);
extern void confile_load (SATDATA_t* sdPtr[MAX_CHANNEL_NUM], unsigned int gstatus );
extern int confile_loadV2(SATDATA_t* sdPtr[MAX_CHANNEL_NUM]);

extern void confileth_load (SATDATATH_t* sate_thPtr[MAX_CHANNEL_NUM] );
extern void confileth_update(char index, SATDATATH_t* sate_thPtr);

extern void udpTarget_load(UDPIP_t* udpPtr, int* mode);
extern void udpTarget_Modeupdate(int mode);
extern void udpTarget_IPupdate(int index, UDPIP_t* udpPtr);

    
    
#ifdef __cplusplus
}
#endif


#endif

