#ifndef __GPIO_I2C__
#define __GPIO_I2C__
#include "stdint.h"
#include <wiringPi.h>
#include <unistd.h>

//#define FT2232


#ifdef FT2232
#include "ftd2xx.h"
#include "libMPSSE_i2c.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

//20160701 ALL IRQ Pins are occupied as LED Pins to measure if the qam board is brought up

#define SCL0     7 
#define SDA0     0 
#define RST0     2 
#define IRQ0     3 
#define LED0_ON		digitalWrite(IRQ0,1)
#define LED0_OFF	digitalWrite(IRQ0,0)

#define SCL1     1 
#define SDA1     4 
#define RST1     5 
#define IRQ1     6
#define LED1_ON         digitalWrite(IRQ1,1)
#define LED1_OFF        digitalWrite(IRQ1,0)


#define SCL2     21 
#define SDA2     22 
#define RST2     23 
#define IRQ2     24 
#define LED2_ON         digitalWrite(IRQ2,1)
#define LED2_OFF        digitalWrite(IRQ2,0)


#define SCL3     26 
#define SDA3     27 
#define RST3     28
#define IRQ3     29 
#define LED3_ON         digitalWrite(IRQ3,1)
#define LED3_OFF        digitalWrite(IRQ3,0)



#define LED_PIN	 25	
#define LED_INIT   	pinMode(LED_PIN,OUTPUT);	pinMode(IRQ0,OUTPUT);	pinMode(IRQ1,OUTPUT);	pinMode(IRQ2,OUTPUT);	pinMode(IRQ3,OUTPUT)

#define LED_ON   	digitalWrite(LED_PIN,1)
#define LED_OFF  	digitalWrite(LED_PIN,0)

#define MAX_delay	1024 

extern volatile unsigned int g_status;  //created @ 8/19    Last 4 bits for 4 bord Initialization status   Next 4 bits for if i2c communication status

extern int rpiVersionCheck();
extern void get_i2cdelay();
extern void delay_i2c();
extern void reset(uint8_t devId);
extern char i2c_init(uint8_t devId);
extern void i2c_start(uint8_t devId);
extern void i2c_stop(uint8_t devId);
extern char i2c_sendbyte(uint8_t devId, uint8_t in);
extern char i2c_receivebyte(uint8_t devId, uint8_t* byte, char ack);








#ifdef FT2232
extern FT_HANDLE ftHandle[4];
extern int i2c_init_bridge();
//extern FT_STATUS write_register(uint8_t devId,uint8_t slaveAddress, uint16 regAddr, uint16 regData);
//extern FT_STATUS read_register(uint8_t devId,uint8_t slaveAddress,uint8_t* regData);
//extern FT_STATUS write_block(uint8_t devId,uint8_t slaveAddress, uint16 regAddr, uint16 size, uint8_t* bufptr);
//extern FT_STATUS read_block(uint8_t devId,uint8_t slaveAddress,uint16 size, uint8_t* bufptr);
//extern FT_STATUS read_blockext(uint8_t devId, uint8_t slaveAddress,uint16 cmdId, uint16 offset, uint16 readSize, uint8_t *bufPtr);



#endif

    
#ifdef __cplusplus
}
#endif

#endif


