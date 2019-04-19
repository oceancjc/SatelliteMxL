#ifndef PROTECT_H
#define PROTECT_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif
    
extern int fake_flag;

extern int aes_cod(unsigned char* input, unsigned char* output, unsigned char* key);
extern int aes_decod(unsigned char* input, unsigned char* output, unsigned char* key);
extern void getmd5(unsigned char* data, unsigned char* output); 
extern int getcpunum(char* cpu);
extern int getmac(char *mac);
extern char check_vali(char* path);
extern char check_vali_timecut(char* path, int time_s);
extern char check_running(char* app);
extern void cod_serial_mac(char *code, char *key);
extern void dec_serial_mac(char *code, char*key, char*serial, char* mac);
    
    
#ifdef __cplusplus
}
#endif


#endif

