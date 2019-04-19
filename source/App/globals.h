#ifndef __GLOBALS_H__
#define __GLOBALS_H__


#define DEBUG_BYPASS_MXL582        (0)
#define DEBUG_BYPASS_SNMP          (0)
#define DEBUG_BYPASS_MONITOR       (1)
#define DEBUG_BPYASS_DOG           (1)



#define RUN_CPU_PROGRAM    "/home/pi/oceancjc/C/cpu &"

#define logcjc                  printf


/** Configuration Files definition **/
#define UDPADDR_CONFIG_FILE                "/etc/dpkg/origins/.udp_config"
#define SATLITE_CHANNEL_CONFIG_FILE        "/etc/dpkg/origins/.sate_config"





/* Spectrum Function Part */
#define SWMODE_UDP_LOCAL_PORT          (12345)
#define MAX_CLIENT 1
#define Rx_FIFO_LEN	64
#define	Tx_FIFO_LEN	65535




#endif // !__GLOBALS_H__

