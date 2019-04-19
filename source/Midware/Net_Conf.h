#ifndef NET_CONF_H
#define NET_CONF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/route.h> 
#include <unistd.h>

extern char ipaddr[16];
extern char maskaddr[16];
extern char getwayaddr[16];
extern int mode;


extern int setip(char *ip);
extern int getip(char *ip_buf);
extern int getmask(char *mask);
extern int getgateway(char *gateway);
extern void log_ip_change(char* ip, char* mask, char* gw);
extern void log_ip_load(char* ip, char* mask, char* gw);
extern int setIfAddr( char *Ipaddr, char *mask,char *gateway);


#endif
