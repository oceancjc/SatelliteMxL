#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
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
#include "globals.h"




/***** Global sign for capturing wrong status *******/
volatile unsigned int g_status = 0;
//int fake_flag = 0;    // Delceared in protect.h
int SW_MODE = 0;
UDPIP_t* udp_ipPtr = NULL;
int point[FFT_FREAME_LEN][2] = { 0 };


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
    SATDEVICE_t* satDevice = NULL;

    
/******* Data Structure Initalization & Config Files Load *******************/
    satDevice = (SATDEVICE_t*)calloc(1, sizeof(SATDEVICE_t));
    if (satDevice == NULL) {
        printf("Malloc LoopApp Top Device fail\n");
        return 1;
    }
    satDevice->i2c_status = (uint8_t*)&g_status;



    if (-1 == wiringPiSetup()) {
        puts("Setup wiringPi failed!");
        return 1; 
    }
    get_i2cdelay();
    i2c_init(devId);
    

    
/**************** Chip Initalize Configurations *********************************/
#if DEBUG_BYPASS_MXL582 == 0
    gettimeofday(&t_start, NULL); 
    if ((status = MxL_582_Init(devId, &(satDevice->oemdata))) != 0)    puts("Initalization failuer\n");
    gettimeofday(&t_end, NULL);    printf("Total Time cost is %lds\n", t_end.tv_sec - t_start.tv_sec);
  

    status = getspecturm(devId, 0, MXL_HYDRA_TUNER_ID_0, 300000,  256, 0, point);
    
    
    if ((pthread_create(&thread[0], NULL, thread_conste0, NULL)) != 0)    printf("Thread Conste create fail ...\n");
    else       printf("Thread Conste create success O(∩_∩)O~~");
    if ((pthread_create(&thread[4], NULL, thread_regular, NULL)) != 0)    puts("Thread Regular create fail ...");       
    else        puts("Thread Regular create success O(∩_∩)O~~");
#endif // DEBUG_BYPASS_MXL582


    /* Last Thread is for monitor Program, thread[5] = cpu*/
    if ((pthread_create(&thread[5], NULL, thread_cpu, NULL)) != 0)    puts("Thread cjc create fail ..."); 
    else        puts("Thread cjc create success O(∩_∩)O~~");     
    
    while(1){  
        if(!(*satDevice->i2c_status))    LED_ON;
        else             LED_OFF;
    }  

    return 0;
}


void *thread_regular(){
    int i = 0;
    while(1){
        MxLWare_HYDRA_OEM_SleepInMs(300);
    }
}


void *thread_conste0(){
    int i = 0,status = 0;
    while (1){
        MxLWare_HYDRA_OEM_SleepInMs(300);        
    }
}



void *thread_cpu(){
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
