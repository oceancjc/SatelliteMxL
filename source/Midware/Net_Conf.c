#include "Net_Conf.h"

char ipaddr[16]    = "";
char maskaddr[16]  = "255.255.255.0";
char getwayaddr[16]= "";

int setip(char *ip){
    struct ifreq temp;
    struct sockaddr_in *addr;
    int fd = 0;
    int ret = -1;
    strcpy(temp.ifr_name, "eth0" );
    if ((fd=socket(AF_INET, SOCK_STREAM, 0))<0)	return -1;	
    addr = (struct sockaddr_in *)&(temp.ifr_addr);
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(ip);
    ret = ioctl(fd, SIOCSIFADDR, &temp);
    close(fd);
    if (ret < 0)	return -1;
    
    return 0;
}

int getip(char *ip_buf){
    struct ifreq temp;
    struct sockaddr_in *myaddr;
    int fd = 0;
    int ret = -1;
    strcpy(temp.ifr_name, "eth0" );
    if ((fd=socket(AF_INET, SOCK_STREAM, 0))<0)	return -1;
    ret = ioctl(fd, SIOCGIFADDR, &temp);
    close(fd);
    if (ret < 0)	return -1;
    myaddr = (struct sockaddr_in *)&(temp.ifr_addr);
    strcpy(ip_buf, inet_ntoa(myaddr->sin_addr));
    
    return 0;
}

int getmask(char *mask){
    struct sockaddr_in *pAddr;
    struct ifreq ifr;
    int sockfd;
    memset(&ifr,0,sizeof(ifr));
    strcpy(ifr.ifr_name, "eth0");
    if((sockfd = socket(AF_INET,SOCK_DGRAM,0))<0)	return -1;
    if(ioctl(sockfd, SIOCGIFNETMASK, &ifr) < 0){
        close(sockfd);
        return -1;
     }
    pAddr = (struct sockaddr_in *)&(ifr.ifr_addr);
    strcpy(mask, (char *)(inet_ntoa(pAddr->sin_addr)));
    close(sockfd);
    return 0;
}

int getgateway(char *gateway){
    FILE *fp;    
    char buf[256];  
    char iface[16];    
    unsigned char tmp[100]={'\0'};
    unsigned int dest_addr=0, gate_addr=0;
    if(NULL == gateway)	return -1; 
    fp = fopen("/proc/net/route", "r");    
    if(fp == NULL)	return -1;   
    fgets(buf, sizeof(buf), fp);    
    while(fgets(buf, sizeof(buf), fp)){    
        if((sscanf(buf, "%s\t%X\t%X", iface, &dest_addr, &gate_addr) == 3) 
            && (memcmp("eth0", iface, strlen("eth0")) == 0)
            && gate_addr != 0) 
        {
                 memcpy(tmp, (unsigned char *)&gate_addr, 4);
                 sprintf(gateway, "%d.%d.%d.%d", (unsigned char)*tmp, (unsigned char)*(tmp+1), (unsigned char)*(tmp+2), (unsigned char)*(tmp+3));
                 break;    
        }
    }    
       
    fclose(fp);
    return 0;
}

void log_ip_change(char* ip, char* mask, char* gw){
    FILE *fp = NULL, *f = NULL;
    if((fp=fopen("/etc/network/.ip_config","r"))==NULL){
        printf("cannot open file");
    }
    if((f=fopen("text2.txt","w"))==NULL){
        printf("cannot open file");
    }
    char line[32];
    while(fgets(line,sizeof(line),fp)){
        puts(line);
        if(strstr(line,"IPADDR")!=NULL){
            printf("%s",line);
            fprintf(f,"%s\t\t%s\n","IPADDR:",ip); /* .....*/

        }
        else if(strstr(line,"MASK")!=NULL){
            printf("%s",line);
            fprintf(f,"%s\t\t%s\n","MASK:",mask); /* .....*/

        }
        else if(strstr(line,"GATEWAY")!=NULL){
            printf("%s",line);
            fprintf(f,"%s\t%s\n","GATEWAY:",gw); /* .....*/

        }
        else    fprintf(f,"%s",line);

    }
    fclose(fp); fclose(f);
    system("mv text2.txt /etc/network/.ip_config");
}

void log_ip_load(char* ip, char* mask, char* gw){
    FILE *fp = NULL;
    if((fp=fopen("/etc/network/.ip_config","r"))==NULL){
        printf("cannot open file");
        return; 
    }
    char title[16], val[16];
    while(fscanf(fp,"%s%s",title,val)==2){
        if(strstr(title,"IPADDR"))  strcpy(ip,val);
        else if(strstr(title,"MASK"))   strcpy(mask,val);
        else if(strstr(title,"GATEWAY"))    strcpy(gw,val);
    }
    fclose(fp);
}

int setIfAddr( char *Ipaddr, char *mask,char *gateway){  
    int fd;  
    struct ifreq ifr;   
    struct sockaddr_in *sin;  
    struct rtentry  rt;  
  
    fd = socket(AF_INET, SOCK_DGRAM, 0);  
    if(fd < 0){  
        perror("socket   error");       
        return -1;       
    }  
    memset(&ifr,0,sizeof(ifr));   
    strcpy(ifr.ifr_name,"eth0");   
    sin = (struct sockaddr_in*)&ifr.ifr_addr;       
    sin->sin_family = AF_INET;       
    //IP..  
    if(inet_aton(Ipaddr,&(sin->sin_addr)) < 0){       
        perror("inet_aton   error");       
        return -2;       
    }      
  
    if(ioctl(fd,SIOCSIFADDR,&ifr) < 0){       
        perror("ioctl   SIOCSIFADDR   error");       
        return -3;       
    }  
    //....  
    if(inet_aton(mask,&(sin->sin_addr)) < 0){       
        perror("inet_pton   error");       
        return -4;       
    }      
    if(ioctl(fd, SIOCSIFNETMASK, &ifr) < 0){  
        perror("ioctl");  
        return -5;  
    }  
    //..  
    memset(&rt, 0, sizeof(struct rtentry));  
    memset(sin, 0, sizeof(struct sockaddr_in));  
    sin->sin_family = AF_INET;  
    sin->sin_port = 0;  
    if(inet_aton(gateway, &sin->sin_addr)<0)  printf ( "inet_aton error\n" );  
    memcpy ( &rt.rt_gateway, sin, sizeof(struct sockaddr_in));  
    ((struct sockaddr_in *)&rt.rt_dst)->sin_family=AF_INET;  
    ((struct sockaddr_in *)&rt.rt_genmask)->sin_family=AF_INET;  
    rt.rt_flags = RTF_GATEWAY;  
    if (ioctl(fd, SIOCADDRT, &rt)<0){  
        close(fd);  
        return -1;  
    }  
    close(fd);  
    log_ip_change(Ipaddr, mask, gateway); 
    return 0;  
} 

