#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "LoopApp.h"



/***** Global sign for capturing wrong status *******/
volatile unsigned int g_status = 0;
//int fake_flag = 0;    // Delceared in protect.h
LoopApp_t* dev = NULL;
int SW_MODE = 0;
UDPIP_t* udp_ipPtr = NULL;
/***** Parameters for SNMP Trap ********************/
TRAPIP_t*      trapip = NULL;
uint8_t        trap_status = 0;
char*          trap_string = NULL;

/****** Pointers for Constellation ****************/
int constindex[MAX_CHANNEL_NUM] = { 0 };



/*
RETSIGTYPE stop_server(int __attribute__((unused)) a) {  
        keep_running = 0;  
} 
*/


void *thread_cpu();
void *thread_conste0();
void *thread_regular();



int main(){
    
/************** Main veriables definations ************************************/
    int status = MXL_SUCCESS;
    unsigned char devId = 0, i = 0;
    pthread_t thread[6] = { 0 };
    struct timeval t_start = { 0 }, t_end = { 0 };
    

    trap_string = (char*)calloc(256, sizeof(char));
    if (trap_string == NULL) {
        printf("Malloc trap_string fail, program stop\n");
        return 0;
    }    
    
/******* Data Structure Initalization & Config Files Load *******************/
    dev = (LoopApp_t*)malloc(sizeof(LoopApp_t));
    if (dev == NULL){
        printf("Malloc LoopApp Top Device fail\n");
        return 1;
    }
    status = loopAppDev_init(dev, devId, (uint8_t*)&g_status);
    if (status != MXL_SUCCESS)    return status;
    trapip = (TRAPIP_t*)calloc(MAX_TRAP_NUM, sizeof(TRAPIP_t));
    udp_ipPtr = (UDPIP_t*)calloc(MAX_UDP_ADDR, sizeof(UDPIP_t));
    /**  Load related configuration files  **/
    confileth_load(dev->sd_thPtr);
    remote_ip_load(trapip);
    udpTarget_load(udp_ipPtr,&SW_MODE);

    if (-1 == wiringPiSetup()) {
        puts("Setup wiringPi failed!");
        return 1; 
    }
    get_i2cdelay();
    i2c_init(devId);
    

    
/*-----------------  Net SNMP Initalization -----------------------------------------*/
#if  DEBUG_BYPASS_SNMP == 0
    const char *app_name = "Satesingalmon";
    snmp_server_init(app_name);   
    send_start_trap("Start");
#endif // DEBUG_BYPASS_SNMP

/**************** Chip Initalize Configurations *********************************/
#if DEBUG_BYPASS_MXL582 == 0
    gettimeofday(&t_start, NULL); 
    if ((status = MxL_582_Init(devId, &(dev->satDevice->oemdata))) != 0)    puts("Initalization failuer\n");
    gettimeofday(&t_end, NULL);    printf("Total Time cost is %lds\n", t_end.tv_sec - t_start.tv_sec);
  
//    int num_get = 0;
//    int point[FFT_FREAME_LEN][2] = { 0 };
//    status = getspecturm(devId, 0, MXL_HYDRA_TUNER_ID_0, 300000,  256, 0, point);
    
    
    
    /* Restore data structure from configuration file, and control chip to lock*/
    status = confile_loadV2(dev->sdPtr);
    if (status != 0)    return 0;   
    uint32_t lockedchannels = 0;
    status = lockAllDemodsV2(dev, MXL_HYDRA_TUNER_ID_0, &lockedchannels);
    MXL_HYDRA_DEBUG("Active channel Msk: 0x%x\tLocked Msk: 0x%x\t, Unlocked Number: 0x%x\n", lockedchannels >> 16, lockedchannels & 0xFF, status);

    if ((pthread_create(&thread[0], NULL, thread_conste0, NULL)) != 0)    printf("Thread Conste create fail ...\n");
    else    ;
    if ((pthread_create(&thread[4], NULL, thread_regular, NULL)) != 0)    printf("Thread Regular create fail ...\n");       
    else    ;
#endif // DEBUG_BYPASS_MXL582


    /* Last Thread is for monitor Program, thread[5] = cpu*/
    if ((pthread_create(&thread[5], NULL, thread_cpu, NULL)) != 0)    printf("Thread cpu create fail ...\n"); 
    else    ;     
    
    while(1){  
        #if  DEBUG_BYPASS_SNMP == 0        
            agent_check_and_process(0);/* block every 1 second */
        #endif
        if(!(*dev->satDevice->i2c_status))    LED_ON;
        else             LED_OFF;
    }  

    #if  DEBUG_BYPASS_SNMP == 0     
        /* at shutdown time */  
        snmp_server_close(app_name);
    #endif
    return 0;
}


void *thread_regular(){
    int i = 0;
    printf("Thread Regular create success O(∩_∩)O~~\n");
    fflush(stdout);
    while(1){
        if ((*dev->satDevice->i2c_status & 1) == 0) {        
            for(i=0;i<MAX_CHANNEL_NUM;i++){
                if(dev->sdPtr[i]->frequencyInKHz!=0){
                    regular_checkPerChannel(dev->satDevice->devId, i, dev->sdPtr[i], dev->sd_thPtr[i]);  
                }
            }    
        }
        MxLWare_HYDRA_OEM_SleepInMs(300);
    }
}


void *thread_conste0(){
    int i = 0,status = 0;
    printf("Thread Conste create success O(∩_∩)O~~\n");
    while (1){
        for (i = 0; i < MAX_CHANNEL_NUM; ++i){
            if (dev->sdPtr[i]->fecLock == 1) {
                status = getconstV2(dev->satDevice->devId, i, dev->vi, dev->vq, dev->sdPtr[i], &constindex[i], 1, fake_flag); 
                if (status != 0)    printf("GetConste fail in channel %d\n",i);
            }               
        }
        
    }
}



void *thread_cpu(){
    printf("Thread CPU create success O(∩_∩)O~~\n");
    while(1){
#if DEBUG_BYPASS_MONITOR == 0
        if( !check_running("cpu") ){
            puts("CPU IS NOT RUNNING!");
            system(RUN_CPU_PROGRAM);
            sleep(2); 
        }
#endif
        sleep(10);
#if DEBUG_BPYASS_DOG == 0
        if (check_dog(DOG_KEY,DOG_POSI,DOG_TXT) )     fake_flag = 1;
#endif
    }

}


//    /*Try to lock one channel for testing*/
//    dev->sdPtr[0]->frequencyInKHz = 994000;
//    dev->sdPtr[0]->standardMask = MXL_HYDRA_DVBS2;
//    dev->sdPtr[0]->symbolRateKSps = 45000;
//    dev->sdPtr[0]->freqSearchRangeKHz = 10000;
//    dev->sdPtr[1]->frequencyInKHz = 994000;
//    dev->sdPtr[1]->standardMask = MXL_HYDRA_DVBS2;
//    dev->sdPtr[1]->symbolRateKSps = 45000;
//    dev->sdPtr[1]->freqSearchRangeKHz = 10000;
//    dev->sdPtr[2]->frequencyInKHz = 994000;
//    dev->sdPtr[2]->standardMask = MXL_HYDRA_DVBS2;
//    dev->sdPtr[2]->symbolRateKSps = 45000;
//    dev->sdPtr[2]->freqSearchRangeKHz = 10000;
//    if ((status = mxl_lockDemodV2(dev, 1, MXL_HYDRA_TUNER_ID_0, MXL_HYDRA_DEMOD_ID_0)) != 0) {
//        printf("Lock Demod failuer with status = %d\n", status);
//        return -1;
//    } 
//    if ((status = mxl_lockDemodV2(dev, 1, MXL_HYDRA_TUNER_ID_0, MXL_HYDRA_DEMOD_ID_1)) != 0) {
//        printf("Lock Demod failuer with status = %d\n", status);
//        return -1;
//    } 
//    if ((status = mxl_lockDemodV2(dev, 1, MXL_HYDRA_TUNER_ID_0, MXL_HYDRA_DEMOD_ID_2)) != 0) {
//        printf("Lock Demod failuer with status = %d\n", status);
//        return -1;
//    } 
//    int cnt = 0;
//    do{
//        MxLWare_HYDRA_OEM_SleepInMs(100);
//        getlockstatus(dev->satDevice->devId, MXL_HYDRA_DEMOD_ID_0, &(dev->sdPtr[MXL_HYDRA_DEMOD_ID_0]->fecLock)); 
//    } while (!dev->sdPtr[MXL_HYDRA_DEMOD_ID_0]->fecLock && ++cnt < 10);
//    if (cnt > 10 && !dev->sdPtr[MXL_HYDRA_DEMOD_ID_0]->fecLock)    printf("Lock Demod %d Time out\n", MXL_HYDRA_DEMOD_ID_0);
//    else                                                            printf("Lock Demod %d Success\n", MXL_HYDRA_DEMOD_ID_0);
//    cnt = 0;
//    do {
//        MxLWare_HYDRA_OEM_SleepInMs(100);
//        getlockstatus(dev->satDevice->devId, MXL_HYDRA_DEMOD_ID_1, &(dev->sdPtr[MXL_HYDRA_DEMOD_ID_1]->fecLock)); 
//    } while (!dev->sdPtr[MXL_HYDRA_DEMOD_ID_0]->fecLock && ++cnt < 10);
//    if (cnt == 10 && !dev->sdPtr[MXL_HYDRA_DEMOD_ID_0]->fecLock)    printf("Lock Demod %d Time out\n", MXL_HYDRA_DEMOD_ID_1);
//    else                                                            printf("Lock Demod %d Success\n", MXL_HYDRA_DEMOD_ID_1);
//    cnt = 0;
//    do {
//        MxLWare_HYDRA_OEM_SleepInMs(100);
//        getlockstatus(dev->satDevice->devId, MXL_HYDRA_DEMOD_ID_2, &(dev->sdPtr[MXL_HYDRA_DEMOD_ID_2]->fecLock)); 
//    } while (!dev->sdPtr[MXL_HYDRA_DEMOD_ID_0]->fecLock && ++cnt < 10);
//    if (cnt == 10 && !dev->sdPtr[MXL_HYDRA_DEMOD_ID_0]->fecLock)    printf("Lock Demod %d Time out\n", MXL_HYDRA_DEMOD_ID_2);
//    else                                                            printf("Lock Demod %d Success\n", MXL_HYDRA_DEMOD_ID_2);
//
//    
//    status = getRegularParamters(dev->satDevice->devId, MXL_HYDRA_DEMOD_ID_0, dev->sdPtr[0]);


