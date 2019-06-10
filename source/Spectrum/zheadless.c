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


#define UDP_RECEIVEPORT     12345
#define KEY	"sunt1ng@2hanl1n@chenjuncha0!"

/***** Global sign for capturing wrong status *******/
volatile unsigned int g_status = 0;
//int fake_flag = 0;    // Delceared in protect.h
int SW_MODE = 0;
UDPIP_t* udp_ipPtr = NULL;
int point[FFT_FREAME_LEN][2] = { 0 };


/* UDP Send 4 fast Parameters Part*/
enum SOCK_STATE
{
    SOCK_INIT  = 0,
    SOCK_BIND,
    SOCK_WORK,
    SOCK_CLOSE
};

typedef struct {
    int sock; 
    struct sockaddr_in* c_addr; 
    int fs;
    int num;
    SATDEVICE_t* dev;
    
}UDP_PACKAGE_PARA_t;


void *thread_cpu();
void *thread_spectrum();




int cmd_analysis(char* cmd, int* fs, int* numofPoints) {
    // <0 error 1,2 cmds, 0 no cmds
    if (cmd == NULL) return -1;
    if (strstr(cmd, "$cmd:end_spec") != NULL) return 1;
    if (strstr(cmd, "$cmd:key") != NULL) return 3; 
    if (strstr(cmd, "$cmd:start_spec$") != NULL) {
        int start = 0, num = 0, i = 17;
        start = atoi(&cmd[16]);
        for (; i < strlen(cmd); i++) {
            if (cmd[i] != '$') continue;
            num = atoi(&cmd[++i]);
            break;
        }
        if (start && num) {
            *fs = start; *numofPoints = num;
            return 2;
        }
        return -2;
    }
    return -2;

}



void framer(int num_get, int point[FFT_FREAME_LEN][2], char* frame, int* len) {
    if (frame == NULL)    return;
    sprintf(frame, "$cmd:spec_data$points:%d$", num_get);
    int length = strlen(frame);
//    printf("Before MEMCPY, num_get = %d\tsize_point = %d\n", num_get, sizeof(point[FFT_FREAME_LEN][2]));	
    memcpy(&frame[length], point, num_get * 8);   //sizeof(int)*point.shape[1]
    *len = length + num_get * 2 * sizeof(int);
}


int main(){
    
/************** Main veriables definations ************************************/
    int status = MXL_SUCCESS;
    unsigned char devId = 0, i = 0;
    pthread_t thread[6] = { 0 };
    struct timeval t_start = { 0 }, t_end = { 0 };
    SATDEVICE_t* satDevice = NULL;

    
    uint8_t state = SOCK_INIT; int sock_client = 0;
    struct sockaddr_in server = { 0 }, client = { 0 };
    pthread_t thread_send = 0;
    socklen_t client_addr_length = sizeof(client);
    UDP_PACKAGE_PARA_t pack = { 0 };
    char buf[64] = { 0 }, response[32] = { 0 };
    int rcv = 0;
    int fs = 0, num = 0;
    
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
  
//    if ((pthread_create(&thread[4], NULL, thread_spectrum, (void*)&pack)) != 0)    puts("Thread Regular create fail ...");       
//    else        puts("Thread Regular create success O(∩_∩)O~~");
#endif // DEBUG_BYPASS_MXL582

    /* Last Thread is for monitor Program, thread[5] = cpu*/
    if ((pthread_create(&thread[5], NULL, thread_cpu, NULL)) != 0)    puts("Thread cjc create fail ..."); 
    else        puts("Thread cjc create success O(∩_∩)O~~");     
    
    while(1){  
        if(!(*satDevice->i2c_status))    LED_ON;
        else             LED_OFF;
        
        switch (state) {
        case SOCK_INIT:
            sock_client = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock_client < 0) {
                printf("Creat UDP socket fail, program ends\n");
                break;
            }
            server.sin_family = AF_INET;
            server.sin_addr.s_addr = INADDR_ANY;
            server.sin_port = htons(UDP_RECEIVEPORT);
            int on = 1;  
            if ((setsockopt(sock_client, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {  
                perror("setsockopt failed");  
                break;  
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
                perror("Receving Failed");
                break;
            }
            else {
                rcv = cmd_analysis(buf, &fs, &num); 
                if (rcv == 1) {
                    if (thread_send) {
                        pthread_cancel(thread_send);	
                        printf("Parameter send process ends ...\n");
                        thread_send = 0;    
                    }
                    sprintf(response, "%s", "$cmd:break_spec");
                    if (sendto(sock_client, (void *)response, strlen(response), 0, (struct sockaddr*)&client, sizeof(client)) < 0)
                        perror("Sending Failed!");
                    else 
                        printf("I say:\n%s\n", response);
                }
                else if (rcv == 2) {
                    if (thread_send){
                        pthread_cancel(thread_send);	
                        thread_send = 0;
                    }
                    pack.fs     = fs;
                    pack.c_addr = &client;
                    pack.sock   = sock_client;
                    pack.dev    = satDevice;
                    pack.num     = num;

                    //printf(" from %s\t %s ,port %d \n", inet_ntoa(pack.c_addr->sin_addr), inet_ntoa(client.sin_addr), ntohs(pack.c_addr->sin_port));
                    if((pthread_create(&thread[4], NULL, thread_spectrum, (void*)&pack)) != 0)    puts("Thread Regular create fail ...");       
                    else        puts("Thread Regular create success O(∩_∩)O~~"); 
                }
                else if(rcv == 3)
                {
                    if (thread_send) {
                        pthread_cancel(thread_send);	
                        printf("Parameter send process ends ...\n");
                        thread_send = 0;    
                    }
                    sprintf(response, "%s", KEY);
                    if (sendto(sock_client, (void *)response, strlen(response), 0, (struct sockaddr*)&client, sizeof(client)) < 0)
                        perror("Sending Failed!");
                    else 
                        printf("I say:\n%s\n", response);
                }
            
                else    printf("Unknown received, rcv_num=%d, buf=%s\n", rcv, buf);
            }    
        }   
    }  

    return 0;
}


#define FS_MIN_KHz        300000
#define FFT_MAX_SIZE      300
#define FFT_FREQ_STEP_KHz 408.2
#define FREQ_RANGE_PERTRUN   122460

void *thread_spectrum(void *arg) {
    int i = 0, status = 0;
    UDP_PACKAGE_PARA_t *pack = (UDP_PACKAGE_PARA_t*) arg;
    int fs = pack->fs > FS_MIN_KHz ? pack->fs : FS_MIN_KHz;
    int cycle = pack->num / FFT_MAX_SIZE ;    int offset = pack->num % FFT_MAX_SIZE;
    //    char *response = calloc(32 + FFT_MAX_SIZE * 2 * sizeof(int), sizeof(char));
        char response[FFT_MAX_SIZE * 2 * sizeof(int) + 100] = { 0 };
    int response_len = 0;
    pthread_detach(pthread_self());
//    if (response == NULL){
//        printf("Error in malloc for spectrum in Func = %s, Line = %d\n",__func__,__LINE__);
//        pthread_exit(NULL);
//    }
    while(1){
        for (i = 0; i <= cycle; i++){
            if (i < cycle) {
                status = getspecturm(pack->dev->devId, 0, MXL_HYDRA_TUNER_ID_0, fs + FREQ_RANGE_PERTRUN*i, FFT_MAX_SIZE, FFT_MAX_SIZE*i, point);  
                framer(FFT_MAX_SIZE, point, response, &response_len);  
                if (status != 0)      sprintf(response, "Something wrong in get spectrum, fs = %d, status = %d\n", fs + FREQ_RANGE_PERTRUN*i, status);
                
            }
            else if (offset > 0) {
                status = getspecturm(pack->dev->devId, 0, MXL_HYDRA_TUNER_ID_0, fs + FREQ_RANGE_PERTRUN*i, offset, FFT_MAX_SIZE*i, point);  
                framer(offset, point, response, &response_len);  
                if (status != 0)      sprintf(response, "Something wrong in get spectrum, fs = %d, status = %d\n", fs + FREQ_RANGE_PERTRUN*i, status);
            }
            else    break;
            if(sendto(pack->sock, (void *)response, response_len, 0, (struct sockaddr*)pack->c_addr, sizeof(*pack->c_addr)) < 0)
                perror("Sending Failed!");
            else 
                printf("I say:\n%s\n", response);
            if (status < 0)    pthread_exit(NULL);
            //            delayMicroseconds(2000);
            sleep(20);
        }
        pthread_testcancel();

    }
}



void *thread_cpu(){
    char timeout = 0;
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
        
#if CJC_PROTECTION_ON == 1
        timeout = check_valid(2021);
        if (timeout)    system("shutdown -h now");
#endif
        
    }

}
