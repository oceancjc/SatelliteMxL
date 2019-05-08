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
#include <unistd.h>
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
void *thread_UDP();


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
    if ((pthread_create(&thread[1], NULL, thread_regular, NULL)) != 0)    printf("Thread Regular create fail ...\n");       
    else    ;
    if ((pthread_create(&thread[2], NULL, thread_UDP, NULL)) != 0)        printf("Thread UDP create fail ...\n");
    else ;
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

enum SOCK_STATE
{
    SOCK_INIT = 0,
    SOCK_BIND,
    SOCK_WORK,
    SOCK_CLOSE
};


void *thread_UDP(){
    uint8_t state = SOCK_INIT;    int sock_client = 0;
    struct sockaddr_in server = { 0 }, client = { 0 };
    socklen_t client_addr_length = sizeof(client);
    char buf[16] = { 0 };
    char opcode[16] = { 0 };
    int opdata = 0, rcv = 0;
    while (1){
        switch (state){
            case SOCK_INIT:
                sock_client = socket(AF_INET, SOCK_DGRAM, 0);
                if (sock_client < 0) {
                    printf("Creat UDP socket fail, thread ends\n");
                    pthread_exit(NULL);
                }
                server.sin_family = AF_INET;
                server.sin_addr.s_addr = INADDR_ANY;
                server.sin_port = htons(UDP_RECEIVEPORT);
                int on = 1;  
                if ((setsockopt(sock_client, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {  
                    perror("setsockopt failed");  
                    pthread_exit(NULL);  
                } 
                state = SOCK_BIND;
            
            case SOCK_BIND:
                if (bind(sock_client, (struct sockaddr *)&server, sizeof(server)) < 0) {
                    perror("Bind Failed");
                    break;
                }
                state = SOCK_WORK;
        
            case SOCK_WORK:
            if (recvfrom(sock_client, buf, sizeof(buf), 0, (struct sockaddr*)&client, &client_addr_length) < 0) {
                //?why always 0
                perror("Receving Failed");
                break;
            }
            else {
                printf("UDP Buffer = %s\n",buf);
                rcv = cmdAnalysis(buf, opcode, &opdata);
                if (rcv == 1) {
                    if (strstr(opcode, "end")) ;
                    else    printf("Invalid Command: %s\n", opcode);
                }
                else if (rcv == 2) {
                    if (strstr(opcode, "start")) ;
                    else    printf("Invalid Command: %s, data= %d\n", opcode, opdata);
                }
                else    printf("Unknown received, rcv_num=%d, buf=%s\n", rcv, buf);
            }    
        }
        
        
        
    }

}


