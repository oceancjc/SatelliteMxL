#include "protect.h"
#include <unistd.h>

int fake_flag = 0;


int aes_cod(unsigned char* input, unsigned char* output, unsigned char* key){
    char cmd[256]="echo ";
    strcat(cmd, (char*)input);
    strcat(cmd," | openssl enc -aes-128-cbc  -k \"");
    strcat(cmd, (char*)key);
    strcat(cmd,"\" -base64");
    FILE *f = popen(cmd,"r");	
    char buf[256]={0};
    if( fgets(buf,256,f)!=NULL ){
        if(buf[strlen(buf)-1]=='\n')    buf[strlen(buf)-1]=0;
    }
    pclose(f);
    strcpy((char*)output, buf);
    return 0;

}

int aes_decod(unsigned char* input, unsigned char* output, unsigned char* key){
    char cmd[256] = "echo ";
    strcat(cmd, (char*)input);
    strcat(cmd," | openssl enc -aes-128-cbc -d -k \"");
    strcat(cmd, (char*)key);
    strcat(cmd,"\" -base64");
    FILE *f = popen(cmd,"r");
    char buf[256]={0};
    if( fgets(buf,256,f)!=NULL ){
            if(buf[strlen(buf)-1]=='\n')    buf[strlen(buf)-1]=0;
    }
    pclose(f);
    strcpy((char*)output, buf);
    return 0;
}

int aes_ecb_cod(unsigned char* input, unsigned char* output, unsigned char* key){
    char cmd[256]="echo ";
    strcat(cmd, (char*)input);
    strcat(cmd," | openssl enc -aes-128-ecb  -k ");
    strcat(cmd, (char*)key);
    strcat(cmd," -base64");
    FILE *f = popen(cmd,"r");
    char buf[256]={0};
    if( fgets(buf,256,f)!=NULL ){
    if(buf[strlen(buf)-1]=='\n')    buf[strlen(buf)-1]=0;
//                puts(buf);
    }
    pclose(f);
    strcpy((char*)output, buf);
    return 0;

}

void getmd5(unsigned char* data, unsigned char* output){
    /*unsigned char md[26]={0};
    int i;
    char tmp[3]={'\0'};
//	printf("%d\n",strlen(data));
    MD5(data,strlen(data),md);
    for (i = 0; i < 16; i++){
        sprintf(tmp,"%2.2x",md[i]);
        strcat(output,tmp);
    }
*/
    unsigned char cmd[100]="echo -n \'";
    strcat((char*)cmd,(char*)data);
    strcat((char*)cmd, "\'|md5sum|cut -d \' \' -f1");
//	puts(cmd);
    FILE *f = popen((char*)cmd, "r");
    char buf[36]={0}; 
    if( fgets(buf,36,f)!=NULL ){
            if(buf[strlen(buf)-1]=='\n')    buf[strlen(buf)-1]=0;
//          puts(buf);
    }
    pclose(f);
    strcpy((char*)output, buf);


}

int getcpunum(char* cpu){
    FILE *f = popen("cat /proc/cpuinfo |grep Serial","r");
    if(!f)	return -1;
    char buf[64]={0}; char* p;	
    if( fgets(buf,64,f)!=NULL ){
        if(buf[strlen(buf)-1]=='\n')	buf[strlen(buf)-1]=0;
        if( (p= strchr(buf,':'))!=NULL)	strcpy(cpu,p+2);
    //	puts(cpu);		
    }
    pclose(f);
    return 0;
}


int getmac(char *mac){
        FILE *f = popen("ifconfig eth0 | grep HWaddr","r");
        if(!f)  return -1;
        char buf[64]={0}; char* p=NULL;
        if( fgets(buf,64,f)!=NULL ){
                if(buf[strlen(buf)-1]=='\n' )    buf[strlen(buf)-1]=0;
                while( buf[strlen(buf)-1]== ' ')	buf[strlen(buf)-1]=0; 
        if( (p= strstr(buf,"HWaddr"))!=NULL) strcpy(mac,p+strlen("HWaddr "));
           //     puts(mac);
        }
        pclose(f);
    
    return 0;
}

void cod_serial_mac(char *code, char *key){
        if(code == NULL)        return;
        char cpu[36]={0};
        char mac[28]={0};
        char cpucode[255] = {0};
        char cpucode_bk[128] = {0};
        unsigned int i = 0;
        getcpunum(cpu);
        if(key==NULL){
                strcpy(code,cpu);       return;
        }

        srand((unsigned)time(NULL));

        for(i=0;i<strlen(cpu);i++){
                cpucode[2*i] = cpu[i];
                cpucode[2*i+1] = rand()%250 +1;
        }
        getmac(mac);
        for(i=0;i<strlen(mac);i++)      if(mac[i]==':')         mac[i] = rand()%250 + 1;
        strcat(cpucode,mac);
        strcpy(cpucode_bk,cpucode);
        /************************* Generate Pseudo Code **********************************************/
        char key_bk[64] = {0};
        do{
            strcpy(key_bk,key);
            for (i=0;i<strlen(key_bk);i++)  key_bk[i]^= rand()%250 + 1;
            if(strlen(key)!=strlen(key_bk)) continue;
            /************************* Encode cpucode with Pseudo ****************************************/
            memset(cpucode,0,255);
            strcpy(cpucode,cpucode_bk);
            for(i=0;i<strlen(cpucode);i++)  cpucode[i]^= (key[i%strlen(key)]^key_bk[i%strlen(key)]);
            sprintf(code,"%s%s",cpucode,key_bk);
        }
        while(strlen(code)!=strlen(mac)+2*strlen(cpu)+strlen(key));
//	puts("CODE IS:");	
//	for(i=0;i<strlen(code);i++)	printf("0x%x\t",code[i]);	puts(" ");
}

void dec_serial_mac(char *code, char*key, char*serial, char* mac){
        if (code == NULL)       return;
        unsigned int i;
        char rand_key[48] = {0};
        int mask = strlen(code) - strlen(key);
        for(i = mask;i<strlen(code);i++){
                rand_key[i-mask] = key[i-mask]^code[i];
        }
        char ser_mac_code[64] = {0};
        for(i=0;i<strlen(code)-strlen(key);i++)  ser_mac_code[i]= rand_key[i%strlen(rand_key)]^code[i];
        mask = strlen(ser_mac_code)-17;
        unsigned int j=0;
        for(i=mask,j=0;i<strlen(ser_mac_code);i++)
                if( (i-mask)%3 !=2) mac[j++]= ser_mac_code[i];
        puts("MAC:");    puts(mac);
        for(i=0,j=0;i<(unsigned int)mask;i++) if(i%2==0)      serial[j++] = ser_mac_code[i];
        puts("SERIAL:"); puts(serial);
}

char check_vali(char* path){
    if(path == NULL )       return 1;
    char md5[36]={0}, mac[20]={ 0 }, cpuid[64]={ 0 };
    getcpunum(cpuid);       getmac(mac);
    strcat(cpuid,"oceancjc");
    strcat(cpuid,mac);
    getmd5((unsigned char*)cpuid, (unsigned char*)md5);

    FILE *fp = NULL;
    if((fp=fopen(path,"r"))==NULL)	return 1;
    char val[36];
    if(fgets(val,36,fp)!= NULL){
    if(val[strlen(val)-1]=='\n')    val[strlen(val)-1]=0; 
    return strcmp(val,md5);	
    }
    return 1;
}

char check_vali_timecut(char* path, int time_s){
    struct timeval tv_begin = { 0 }, tv_end = { 0 };
    int ret = 0;
    gettimeofday(&tv_begin, NULL);
    ret = check_vali(path);
    gettimeofday(&tv_end, NULL);
    if (tv_end.tv_sec - tv_begin.tv_sec > time_s){
        system("shutdown -h now");
        sleep(60);
    }
    return ret;

}


char check_running(char* app){	//No running return 0 or return 1
    if(app == NULL)	return 0;
    char cmd[128] = "ps -ef |grep ";
    strcat(cmd,app);
    strcat(cmd," | grep -v \"grep\"");
    FILE *fp = NULL;
//	puts(cmd);
    if((fp=popen(cmd,"r"))==NULL)      return 0;
    char val[1024];
    if(fgets(val,1024,fp)!= NULL){
        pclose(fp); 
        return 1; 
    }
    pclose(fp);	
    return 0;
}


char check_valid(){
    struct tm *t = NULL;    time_t tt = 0;
    time(&tt);
    if(t->tm_year > 117)            return 1;  //2017-1900
    return 0;
}

