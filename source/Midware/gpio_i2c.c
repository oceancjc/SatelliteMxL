#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"gpio_i2c.h"


#define SDA_L    digitalWrite(SDA[devId],0)
#define SDA_H    digitalWrite(SDA[devId],1)
#define SCL_L    pinMode(SCL[devId],OUTPUT);digitalWrite(SCL[devId],0)
#define SCL_H    digitalWrite(SCL[devId],1)
#define SDA_R    pinMode(SDA[devId],INPUT)
#define SCL_R    pinMode(SCL[devId],INPUT)

int SCL[4] = {SCL0,SCL1,SCL2,SCL3};
int SDA[4] = {SDA0,SDA1,SDA2,SDA3};
int RST[4] = {RST0,RST1,RST2,RST3};
int IRQ[4] = {IRQ0,IRQ1,IRQ2,IRQ3};


int rpiVersionCheck(){
    FILE *f = popen("cat /proc/cpuinfo | grep Revision | awk '{print $3}' | sed s/^1000//","r");
    char buf[12]={0};
    if( fgets(buf,12,f)!=NULL ){
        if(buf[strlen(buf)-1]=='\n')    buf[strlen(buf)-1]=0;
        //puts(buf);
    }
    pclose(f); 
    if(strstr(buf,"2082")){
        return 3;
    }
    return 2;
}

unsigned int I2C_delay = 90;

void get_i2cdelay(){
    FILE *f = popen("cat ./.delay.conf","r");
    char buf[8] = {0};
    if( fgets(buf,8,f)!=NULL ){
        if(buf[strlen(buf)-1]=='\n')    buf[strlen(buf)-1]=0;
        //puts(buf);
        I2C_delay = atoi(buf);
    }
    pclose(f);
    printf("IIC Delay = %d\n",I2C_delay);
}


void delay_i2c(){
    volatile unsigned int i = 0;
    for (i = I2C_delay; i > 0; i--)   ;
}


#ifndef FT2232

void reset(uint8_t devId){
        i2c_init(devId);
        pinMode(RST[devId],OUTPUT);
        digitalWrite(RST[devId],0);
        int i=20;
        while(i--)      delay_i2c();
        digitalWrite(RST[devId],1);
        delayMicroseconds(20);
        printf("Reset[%d] OK!\n",devId);
        delayMicroseconds(2000);
}



static int limit_SCL(uint8_t devId){
    int i = 0;
    while (1){
        if(digitalRead(SCL[devId]))    break; 
        else{
            if(i++ >  MAX_delay)    return 1;    
        } 
    }
    return 0;
}


char i2c_init(uint8_t devId){

    
    pullUpDnControl(SDA[devId],PUD_UP);
    pullUpDnControl(SCL[devId],PUD_UP);
    SDA_R;
    SCL_R;
    delay_i2c();
    return 0;
}



void i2c_start(uint8_t devId){
    SDA_R;
    SCL_R;
    pinMode(SDA[devId], OUTPUT);
    SDA_L;
    delay_i2c();   
    delay_i2c();
    SCL_L;
    delay_i2c();
    delay_i2c();
}




void i2c_stop(uint8_t devId){
    pinMode(SDA[devId],OUTPUT);    
    SDA_L;
    delay_i2c();
    delay_i2c(); 

    SCL_R;
    if (0 != limit_SCL(devId))    printf("Stop i2c failed! Please check \n");
    delay_i2c();
    SDA_R;
    delay_i2c();
    delay_i2c();
    delay_i2c();
}

char i2c_sendbyte(uint8_t devId,uint8_t in){
    char i,j=0;
    for ( i = 0; i < 8; i++,j++){
    //b    delay_i2c();
    //b    delay_i2c();
        if (in & 0x80)        SDA_H;
        else                  SDA_L;
        in <<= 1;
        delay_i2c();
        SCL_R;
        if (0 != limit_SCL(devId)){ 
            printf("Send %d bit failed! Please check clock streching \n",j);
            return 2;//clock streching overtime error

        }    
        delay_i2c();    
        SCL_L;
    }
    SDA_R;
    delay_i2c();
    delay_i2c();
//@        delay_i2c();
    SCL_R;
    if (0 != limit_SCL(devId)){
             printf("Send %d bit failed! Please check clock streching \n",j);
             return 2;//clock streching overtime error
    }
 
    i = digitalRead(SDA[devId]);
    delay_i2c();
//    printf("SDA_Value = %d\n",i);
    SCL_L; 
    delay_i2c();
    pinMode(SDA[devId],OUTPUT);
    delay_i2c();
    if(i)    return 1;
    return 0;

                    
}    

char i2c_receivebyte(uint8_t devId, uint8_t* byte, char ack){
    char i;
    *byte = 0;
    SDA_R;
    for (i = 0; i < 8; i++){
        SCL_R;
        if (0 != limit_SCL(devId)){
            printf("[ERR -1] i2c receive byte failed! Please check clock streching,Id = %d \n",devId);
            return 2;

        }    
        delay_i2c();
        (*byte) <<= 1;
        if (digitalRead(SDA[devId]))    (*byte) |= 1;
        pinMode(SCL[devId],OUTPUT); 
        SCL_L;
        delay_i2c();
    }    
    if(!ack){
        pinMode(SDA[devId],OUTPUT);
        SDA_L;
    }
    delay_i2c();
    SCL_R;
    if (0 != limit_SCL(devId)){
        printf("[ERR -2] i2c receive byte failed! Please check clock streching, Id = %d \n",devId);
        return 2;
    }
    delay_i2c();
    delay_i2c();
    SCL_L;
    delay_i2c();
    delay_i2c();
    return 0;
}

#else
/*********************************With FT2232************************************************/
const BYTE AA_ECHO_CMD_1 = '\xAA';
const BYTE AB_ECHO_CMD_2 = '\xAB';
const BYTE BAD_COMMAND_RESPONSE = '\xFA';

const BYTE MSB_VEDGE_CLOCK_IN_BIT = '\x22';
const BYTE MSB_EDGE_CLOCK_OUT_BYTE = '\x11';
const BYTE MSB_EDGE_CLOCK_IN_BYTE = '\x24';

const BYTE MSB_FALLING_EDGE_CLOCK_BYTE_IN = '\x24';
const BYTE MSB_FALLING_EDGE_CLOCK_BYTE_OUT = '\x11';
const BYTE MSB_DOWN_EDGE_CLOCK_BIT_IN = '\x26';
const BYTE MSB_DOWN_EDGE_CLOCK_BIT_OUT = '\x1B';
const BYTE MSB_UP_EDGE_CLOCK_BYTE_IN = '\x20';
const BYTE MSB_UP_EDGE_CLOCK_BYTE_OUT = '\x10';
const BYTE MSB_RISING_EDGE_CLOCK_BIT_IN = '\x22';

BYTE OutputBuffer[4][1024];        //Buffer to hold MPSSE commands and data to be sent to FT2232H
BYTE InputBuffer[4][1024];        //Buffer to hold Data bytes to be read from FT2232H
DWORD dwClockDivisor = 0x0031;      //Value of clock divisor, SCL Frequency = 60/((1+0x004A)*2) (MHz) = 400khz
//DWORD dwClockDivisor = 0x0095;   //200khz
DWORD dwNumBytesToSend[4] = {0,0,0,0};     //Index of output buffer
DWORD dwNumBytesSent[4] = {0,0,0,0},     dwNumBytesRead[4] = {0,0,0,0}, dwNumInputBuffer[4] = {0,0,0,0};

BYTE ByteDataRead[4]={0,0,0,0};//ByteAddress
BYTE ByteAddresshigh = 0x00, ByteAddresslow = 0x80;        //EEPROM address is '0x0080'
BYTE ByteDataToBeSend = 0x5A;

FT_HANDLE ftHandle[4];

void reset(uint8_t devId){
        FT_STATUS ftStatus = FT_OK;
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x82';        //Command to set directions of higher 8 pins and force value on bits set as output
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x00';        //Set SCL low, WP disabled by SK, GPIOL0 at bit ¡®0¡¯
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x01';        //Set SK, GPIOL0 pins as output with bit ¡¯1¡¯, DO and other pins as input with bit ¡®0¡¯
        ftStatus = FT_Write(ftHandle[devId], OutputBuffer[devId], dwNumBytesToSend[devId], &dwNumBytesSent[devId]);             //Send off the commands
        dwNumBytesToSend[devId] = 0;                    //Clear output buffer
          delay(200); 
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x82';        //Command to set directions of higher 8 pins and force value on bits set as output
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x01';        //Set SCL low, WP disabled by SK, GPIOL0 at bit ¡®0¡¯
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x01';        //Set SK, GPIOL0 pins as output with bit ¡¯1¡¯, DO and other pins as input with bit ¡®0¡¯
        ftStatus = FT_Write(ftHandle[devId], OutputBuffer[devId], dwNumBytesToSend[devId], &dwNumBytesSent[devId]);             //Send off the commands
        dwNumBytesToSend[devId] = 0;                    //Clear output buffer
        printf("Reset OK!\n");
    delay(200);
}


void i2c_start(uint8_t devId)
{
    DWORD dwCount;
    for(dwCount=0; dwCount < 3; dwCount++)    // Repeat commands to ensure the minimum period of the start hold time ie 600ns is achieved
    {
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x80';    //Command to set directions of lower 8 pins and force value on bits set as output
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x03';  //Set SDA, SCL high, WP disabled by SK, DO at bit ¡®1¡¯, GPIOL0 at bit ¡®0¡¯
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x13';    //Set SK,DO,GPIOL0 pins as output with bit ¡¯1¡¯, other pins as input with bit ¡®0¡¯
    }
    for(dwCount=0; dwCount < 3; dwCount++)    // Repeat commands to ensure the minimum period of the start setup time ie 600ns is achieved
    {
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x80';     //Command to set directions of lower 8 pins and force value on bits set as output
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x01';     //Set SDA low, SCL high, WP disabled by SK at bit ¡®1¡¯, DO, GPIOL0 at bit ¡®0¡¯
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x13';    //Set SK,DO,GPIOL0 pins as output with bit ¡¯1¡¯, other pins as input with bit ¡®0¡¯
    }
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x80';     //Command to set directions of lower 8 pins and force value on bits set as output
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x00';     //Set SDA, SCL low high, WP disabled by SK, DO, GPIOL0 at bit ¡®0¡¯
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x13';    //Set SK,DO,GPIOL0 pins as output with bit ¡¯1¡¯, other pins as input with bit ¡®0¡¯
}

void i2c_stop(uint8_t devId)
{
    DWORD dwCount;
    for(dwCount=0; dwCount<3; dwCount++)    // Repeat commands to ensure the minimum period of the stop setup time ie 600ns is achieved
    {
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x80';     //Command to set directions of lower 8 pins and force value on bits set as output
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x01';     //Set SDA low, SCL high, WP disabled by SK at bit ¡®1¡¯, DO, GPIOL0 at bit ¡®0¡¯
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x13';    //Set SK,DO,GPIOL0 pins as output with bit ¡¯1¡¯, other pins as input with bit ¡®0¡¯
    }
    for(dwCount=0; dwCount<3; dwCount++)    // Repeat commands to ensure the minimum period of the stop hold time ie 600ns is achieved
    {
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x80';     //Command to set directions of lower 8 pins and force value on bits set as output
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x03';     //Set SDA, SCL high, WP disabled by SK, DO at bit ¡®1¡¯, GPIOL0 at bit ¡®0¡¯
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x13';    //Set SK,DO,GPIOL0 pins as output with bit ¡¯1¡¯, other pins as input with bit ¡®0¡¯
    }
    //Tristate the SCL, SDA pins
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x80';     //Command to set directions of lower 8 pins and force value on bits set as output
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x00';     //Set WP disabled by GPIOL0 at bit 0¡¯
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x10';    //Set GPIOL0 pins as output with bit ¡¯1¡¯, SK, DO and other pins as input with bit ¡®0¡¯
    FT_Write(ftHandle[devId], OutputBuffer[devId], dwNumBytesToSend[devId], &dwNumBytesSent[devId]);
}

char i2c_sendbyte(uint8_t devId,uint8_t in)
{
    FT_STATUS ftStatus = FT_OK;
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = MSB_FALLING_EDGE_CLOCK_BYTE_OUT;     //clock data byte out on ¨Cve Clock Edge MSB first
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x00';
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x00';    //Data length of 0x0000 means 1 byte data to clock out
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = in;    //Set control byte, bit 4-7 of ¡®1010¡¯ as 24LC02 control code, bit 1-3 as block select bits  //which is don¡¯t care here, bit 0 of ¡®0¡¯ represent Write operation
    //Get Acknowledge bit from EEPROM   
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x80';     //Command to set directions of lower 8 pins and force value on bits set as output
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x00';     //Set SCL low, WP disabled by SK, GPIOL0 at bit ¡®0¡¯
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x11';        //Set SK, GPIOL0 pins as output with bit ¡¯1¡¯, DO and other pins as input with bit ¡®0¡¯

    OutputBuffer[devId][dwNumBytesToSend[devId]++] = MSB_RISING_EDGE_CLOCK_BIT_IN;     //Command to scan in acknowledge bit , -ve clock Edge MSB first
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x0';    //Length of 0x0 means to scan in 1 bit

    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x87';    //Send answer back immediate command
    ftStatus = FT_Write(ftHandle[devId], OutputBuffer[devId], dwNumBytesToSend[devId], &dwNumBytesSent[devId]);        //Send off the commands
    dwNumBytesToSend[devId] = 0;            //Clear output buffer
    //Check if ACK bit received, may need to read more times to get ACK bit or fail if timeout
//    usleep(50);
    ftStatus = FT_Read(ftHandle[devId], InputBuffer[devId], 1, &dwNumBytesRead[devId]);      //Read one byte from device receive buffer
    if ((ftStatus != FT_OK) || (dwNumBytesRead[devId] == 0))
    {
        printf("fail to get ACK when send control byte 1 [Program Section] \n");
            return FALSE; //Error, can't get the ACK bit from EEPROM
    }
    else 
    {
        if ((InputBuffer[devId][0] & 0x01)  != 0    )    //Check ACK bit 0 on data byte read out

        {    
            printf("fail to get ACK when send control byte 2 [Program Section] \n");
            return 1; //Error, can't get the ACK bit from EEPROM

        }
    }

    
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x80';     //Command to set directions of lower 8 pins and force value on bits set as output
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x02';     //Set SDA high, SCL low, WP disabled by SK at bit '0', DO, GPIOL0 at bit '1'
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x13';    //Set SK,DO,GPIOL0 pins as output with bit ¡¯1¡¯, other pins as input with bit ¡®0¡¯
    return 0;
}


void deviceinfo(DWORD numDevs){
    FT_STATUS ftStatus;
    FT_DEVICE_LIST_INFO_NODE *devInfo;
    // create the device information list

    if (numDevs > 0) {
    // allocate storage for list based on numDevs
        devInfo =(FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs); 
        // get the device information list
        ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs); 
        if (ftStatus == FT_OK) {
            int i;    
            for ( i = 0; i < numDevs; i++) {
            printf("Dev %d:\n",i); 
            printf(" Flags=0x%x\n",devInfo[i].Flags); 
            printf(" Type=0x%x\n",devInfo[i].Type); 
            printf(" ID=0x%x\n",devInfo[i].ID); 
            printf(" LocId=0x%x\n",devInfo[i].LocId); 
            printf(" SerialNumber=%s\n",devInfo[i].SerialNumber); 
            printf("  Description=%s\n",devInfo[i].Description); 
            printf(" ftHandle=0x%x\n",devInfo[i].ftHandle); 


            }
        }
        free(devInfo);
    }
    else    puts("No FTDI Device found!");
}

// void channelinfo(uint32 channels){
    // if(channels<=0){
        // puts("No channel available!");
        // return ;
    // }    
    // uint8_t i;
    // FT_STATUS status;
    // FT_DEVICE_LIST_INFO_NODE devList;    
    // for(i=0;i<channels;i++){
        // status = I2C_GetChannelInfo(i,&devList);
        // APP_CHECK_STATUS(status);
        // printf("Information on channel number %d:\n",i);
        // /*print the dev info*/
        // printf(" Flags=0x%x\n",devList.Flags);
        // printf(" Type=0x%x\n",devList.Type);
        // printf(" ID=0x%x\n",devList.ID);
        // printf(" LocId=0x%x\n",devList.LocId);
        // printf(" SerialNumber=%s\n",devList.SerialNumber);
        // printf(" Description=%s\n",devList.Description);
        // printf(" ftHandle=0x%x\n",devList.ftHandle);/*is 0 unless open*/
    // }

    
// }

int i2c_init_bridge(){
    FT_STATUS status;
    uint32 numDevs = 0;
    system("rmmod ftdi_sio;rmmod usbserial");    
    status = FT_CreateDeviceInfoList(&numDevs);
    if (status == FT_OK)     printf("Number of devices is %d\n",numDevs);

    
    //channelinfo(channels);
//    deviceinfo(numDevs);
    return numDevs;
}

char i2c_init(uint8_t devId){
    FT_STATUS ftStatus;

    
    long Buffer;    // buffer for description of first device 
    ftStatus = FT_ListDevices((PVOID)devId,&Buffer,FT_LIST_BY_INDEX|FT_OPEN_BY_LOCATION);
    if (ftStatus == FT_OK) {
    // FT_ListDevices OK, product descriptions are in Buffer1 and Buffer2, and 
    // numDevs contains the number of devices connected
//        puts(Buffer);    
//        ftStatus = FT_OpenEx((PVOID)Buffer,FT_OPEN_BY_SERIAL_NUMBER,&ftHandle[devId]);
        ftStatus = FT_Open(devId,&ftHandle[devId]);
        if (ftStatus == FT_OK) {
            printf("Open DEV[%d] OK, Serial Num is 0x%x\n",devId,Buffer);// success - device with serial number "FT000001" is open
            ftStatus |= FT_ResetDevice(ftHandle[devId]);     //Reset USB device
            //Purge USB receive buffer first by reading out all old data from FT2232H receive buffer
            ftStatus |= FT_GetQueueStatus(ftHandle[devId], &dwNumInputBuffer[devId]);     // Get the number of bytes in the FT2232H receive buffer
            if ((ftStatus == FT_OK) && (dwNumInputBuffer[devId] > 0))
                    FT_Read(ftHandle[devId], &InputBuffer[devId], dwNumInputBuffer[devId], &dwNumBytesRead[devId]);      //Read out the data from FT2232H receive buffer
            ftStatus |= FT_SetUSBParameters(ftHandle[devId], 65536, 65535);    //Set USB request transfer size
            ftStatus |= FT_SetChars(ftHandle[devId], FALSE, 0, FALSE, 0);     //Disable event and error characters
            ftStatus |= FT_SetTimeouts(ftHandle[devId], 5000, 5000);        //Sets the read and write timeouts in milliseconds for the FT2232H
            ftStatus |= FT_SetLatencyTimer(ftHandle[devId], 10);        //Set the latency timer
            ftStatus |= FT_SetBitMode(ftHandle[devId], 0x0, 0x00);         //Reset controller
            ftStatus |= FT_SetBitMode(ftHandle[devId], 0x0, 0x02);         //Enable MPSSE mode

    
            if (ftStatus != FT_OK)

            {
                printf("fail on initialize FT2232H device! \n");
                getchar();
                return -1;

            }
            usleep(50);    // Wait for all the USB stuff to complete and work



    
        
            
            //////////////////////////////////////////////////////////////////
            // Synchronize the MPSSE interface by sending bad command ¡®0xAA¡¯
            //////////////////////////////////////////////////////////////////
            OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\xAA';        //Add BAD command ¡®0xAA¡¯
            ftStatus = FT_Write(ftHandle[devId], OutputBuffer[devId], dwNumBytesToSend[devId], &dwNumBytesSent[devId]);    // Send off the BAD commands
            dwNumBytesToSend[devId] = 0;            //Clear output buffer
            do{
                ftStatus = FT_GetQueueStatus(ftHandle[devId], &dwNumInputBuffer[devId]);     // Get the number of bytes in the device input buffer
            }while ((dwNumInputBuffer[devId] == 0) && (ftStatus == FT_OK));       //or Timeout

            
            bool bCommandEchod = FALSE;
            ftStatus = FT_Read(ftHandle[devId], &InputBuffer[devId], dwNumInputBuffer[devId], &dwNumBytesRead[devId]);  //Read out the data from input buffer
            int dwCount;    
            for (dwCount = 0; dwCount < dwNumBytesRead[devId] - 1; dwCount++)    //Check if Bad command and echo command received

            {
                if ((InputBuffer[devId][dwCount] == 0xFA) && (InputBuffer[devId][dwCount+1] == 0xAA))

                {
                    bCommandEchod = TRUE;
                    break;


                }
            }
            if (bCommandEchod == FALSE) 

            {    
                printf("fail to synchronize MPSSE with command '0xAA' \n");
                getchar();
                return -1; /*Error, can¡¯t receive echo command , fail to synchronize MPSSE interface;*/ 


            }
            
            //////////////////////////////////////////////////////////////////
            // Synchronize the MPSSE interface by sending bad command ¡®0xAB¡¯
            //////////////////////////////////////////////////////////////////
            //dwNumBytesToSend = 0;            //Clear output buffer
            OutputBuffer[devId][dwNumBytesToSend[devId]++] = 0xAB;    //Send BAD command ¡®0xAB¡¯
            ftStatus = FT_Write(ftHandle[devId], OutputBuffer[devId], dwNumBytesToSend[devId], &dwNumBytesSent[devId]);    // Send off the BAD commands
            dwNumBytesToSend[devId] = 0;            //Clear output buffer
            do{
                ftStatus = FT_GetQueueStatus(ftHandle[devId], &dwNumInputBuffer[devId]);    //Get the number of bytes in the device input buffer
            }while ((dwNumInputBuffer[devId] == 0) && (ftStatus == FT_OK));   //or Timeout
            bCommandEchod = FALSE;
            ftStatus = FT_Read(ftHandle[devId], &InputBuffer[devId], dwNumInputBuffer[devId], &dwNumBytesRead[devId]);  //Read out the data from input buffer
            for (dwCount = 0;dwCount < dwNumBytesRead[devId] - 1; dwCount++)    //Check if Bad command and echo command received

            {
                if ((InputBuffer[devId][dwCount] == 0xFA) && (InputBuffer[devId][dwCount+1] == 0xAB))

                {
                    bCommandEchod = TRUE;
                    break;


                }
            }
            if (bCommandEchod == FALSE)  

            {    
                printf("fail to synchronize MPSSE with command '0xAB' \n");
                getchar();
                return -1; 
                /*Error, can¡¯t receive echo command , fail to synchronize MPSSE interface;*/ 


            }
    
            printf("MPSSE synchronized with BAD command \n");

    
            ////////////////////////////////////////////////////////////////////
            //Configure the MPSSE for I2C communication with 24LC02B
            //////////////////////////////////////////////////////////////////
            OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x8A';     //Ensure disable clock divide by 5 for 60Mhz master clock
            OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x97';     //Ensure turn off adaptive clocking
            OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x8C';     //Enable 3 phase data clock, used by I2C to allow data on both clock edges
            ftStatus = FT_Write(ftHandle[devId], OutputBuffer[devId], dwNumBytesToSend[devId], &dwNumBytesSent[devId]);    // Send off the commands
            dwNumBytesToSend[devId] = 0;            //Clear output buffer
            OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x80';     //Command to set directions of lower 8 pins and force value on bits set as output 
            OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x03';     //Set SDA, SCL high, WP disabled by SK, DO at bit ¡®1¡¯, GPIOL0 at bit ¡®0¡¯
            OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x13';    //Set SK,DO,GPIOL0 pins as output with bit ¡¯1¡¯, other pins as input with bit ¡®0¡¯
            // The SK clock frequency can be worked out by below algorithm with divide by 5 set as off
            // SK frequency  = 60MHz /((1 +  [(1 +0xValueH*256) OR 0xValueL])*2)
            OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x86';             //Command to set clock divisor
            OutputBuffer[devId][dwNumBytesToSend[devId]++] = dwClockDivisor & '\xFF';    //Set 0xValueL of clock divisor
            OutputBuffer[devId][dwNumBytesToSend[devId]++] = (dwClockDivisor >> 8) & '\xFF';    //Set 0xValueH of clock divisor
            ftStatus = FT_Write(ftHandle[devId], OutputBuffer[devId], dwNumBytesToSend[devId], &dwNumBytesSent[devId]);    // Send off the commands
            dwNumBytesToSend[devId] = 0;            //Clear output buffer
            usleep(10);        //Delay for a while

    
            //Turn off loop back in case
            OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x85';        //Command to turn off loop back of TDI/TDO connection
            ftStatus = FT_Write(ftHandle[devId], OutputBuffer[devId], dwNumBytesToSend[devId], &dwNumBytesSent[devId]);    // Send off the commands
            dwNumBytesToSend[devId] = 0;            //Clear output buffer
            usleep(10);        //Delay for a while



                
            
        }
        else    printf("Initialize DEV[%d] Failed\n",devId);

    }    
    else         printf("Can't Find DEV[%d]\n",devId);
    return 0;

    
}

char i2c_receivebyte(uint8_t devId,uint8_t* byte, char ack){
    FT_STATUS ftStatus = FT_OK;
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x80';     //Command to set directions of lower 8 pins and force value on bits set as output
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x00';     //Set SCL low, WP disabled by SK, GPIOL0 at bit ¡®0¡¯
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x11';    //Set SK, GPIOL0 pins as output with bit ¡¯1¡¯, DO and other pins as input with bit ¡®0¡¯
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = MSB_UP_EDGE_CLOCK_BYTE_IN; //Command to clock data byte in on ¨Cve Clock Edge MSB first
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x00';
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x00';    //Data length of 0x0000 means 1 byte data to clock in
    //Get Acknowledge bit from EEPROM   
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = 0x80; //Command to set directions of lower 8 pins and force value on bits set as output
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = 0x00; //Set SDA high, SCL low, WP disabled by SK at bit '0', DO, GPIOL0 at bit '1'
        OutputBuffer[devId][dwNumBytesToSend[devId]++] = 0x13; //Set SK,DO,GPIOL0 pins as output with bit ??, other pins as input with bit ??

    
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = MSB_DOWN_EDGE_CLOCK_BIT_OUT;     //Command to scan in acknowledge bit , -ve clock Edge MSB first
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x0';    //Length of 0x0 means to scan in 1 bit
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = ack;
//    OutputBuffer[devId][dwNumBytesToSend[devId]++] = '\x87';    //Send answer back immediate command
    ftStatus = FT_Write(ftHandle[devId], OutputBuffer[devId], dwNumBytesToSend[devId], &dwNumBytesSent[devId]);        //Send off the commands
    dwNumBytesToSend[devId] = 0;            //Clear output buffer
    //Read two bytes from device receive buffer, first byte is data read from EEPROM, second byte is ACK bit
    ftStatus = FT_Read(ftHandle[devId], InputBuffer[devId], 1, &dwNumBytesRead[devId]);
    (*byte) = InputBuffer[devId][0]; //Return the data read from EEPROM
//    printf("%x\t%x\t%d\n",InputBuffer[devId][0],InputBuffer[devId][1],dwNumBytesRead[devId]);
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = 0x80; //Command to set directions of lower 8 pins and force value on bits set as output
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = 0x02; //Set SDA high, SCL low, WP disabled by SK at bit '0', DO, GPIOL0 at bit '1'
    OutputBuffer[devId][dwNumBytesToSend[devId]++] = 0x11; //Set SK,DO,GPIOL0 pins as output with bit ??, other pins as input with bit ??
//    ftStatus = FT_Write(ftHandle[devId], OutputBuffer[devId], dwNumBytesToSend[devId], &dwNumBytesSent[devId]);
//    dwNumBytesToSend[devId] = 0; //Clear output buffer
    return 0;

}

/*
FT_STATUS write_register(uint8_t devId,uint8_t slaveAddress, uint16 regAddr, uint16 regData){
    FT_STATUS status;
    uint8_t buf[4] = {regAddr>>8,regAddr,regData>>8,regData};
    uint8_t bytesTransfered = 0,times = 5;
    //poll to check completition 
    while(times--)
    {
        status = I2C_DeviceWrite(ftHandle[devId], slaveAddress, 4, buf,&bytesTransfered, I2C_TRANSFER_OPTIONS_START_BIT|I2C_TRANSFER_OPTIONS_STOP_BIT|I2C_TRANSFER_OPTIONS_BREAK_ON_NACK);
        if((FT_OK == status) && (4 == bytesTransfered))    break;
        else printf("Write register fail of device[%d],try again\n",devId);
    }
    return status;
}


FT_STATUS read_register(uint8_t devId,uint8_t slaveAddress,uint8_t* regData){
    FT_STATUS status;
    uint8_t bytesTransfered = 0,times = 5;
    status = I2C_DeviceRead(ftHandle[devId], slaveAddress, 2,regData,&bytesTransfered,I2C_TRANSFER_OPTIONS_START_BIT|I2C_TRANSFER_OPTIONS_STOP_BIT|I2C_TRANSFER_OPTIONS_NACK_LAST_BYTE);
    if(status != FT_OK){
        printf("[Read Step Fail]Read register fail of device[%d]\n",devId);
        return status;

    }    
    return status;
}


FT_STATUS write_block(uint8_t devId,uint8_t slaveAddress, uint16 regAddr, uint16 size, uint8_t* bufptr){
    FT_STATUS status;
    uint8_t buf[size+2];
    uint8_t bytesTransfered = 0,times = 5;

    snprintf(buf,size+2,"%c%c%s",regAddr>>8,regAddr,bufptr);
    while(times--)
    {
        status = I2C_DeviceWrite(ftHandle[devId], slaveAddress, size+2, buf,&bytesTransfered, I2C_TRANSFER_OPTIONS_START_BIT|I2C_TRANSFER_OPTIONS_STOP_BIT|I2C_TRANSFER_OPTIONS_BREAK_ON_NACK);
        if((FT_OK == status) && ((size+2) == bytesTransfered))    break;
        else printf("Write block fail of device[%d],try again\n",devId);
    }
    return status;    
}

FT_STATUS read_block(uint8_t devId,uint8_t slaveAddress,uint16 size, uint8_t* bufptr){
    FT_STATUS status;
    uint8_t bytesTransfered = 0,times = 5;
    status = I2C_DeviceRead(ftHandle[devId], slaveAddress, size,bufptr,&bytesTransfered,I2C_TRANSFER_OPTIONS_START_BIT|I2C_TRANSFER_OPTIONS_STOP_BIT|I2C_TRANSFER_OPTIONS_NACK_LAST_BYTE);
    if(status != FT_OK){
        printf("[Read Step Fail]Read block fail of device[%d]\n",devId);
        return status;

    }    
    return status;

}

FT_STATUS read_blockext(uint8_t devId, uint8_t slaveAddress,uint16 cmdId, uint16 offset, uint16 readSize, uint8_t *bufPtr){
    FT_STATUS status;
    uint8_t buf[6]={0xFF,0xFD,cmdId>>8,cmdId,offset>>8,offset};
    uint8_t bytesTransfered = 0,times = 5;
    while(times--)
    {
        status = I2C_DeviceWrite(ftHandle[devId], slaveAddress, 6, buf,&bytesTransfered, I2C_TRANSFER_OPTIONS_START_BIT|I2C_TRANSFER_OPTIONS_STOP_BIT|I2C_TRANSFER_OPTIONS_BREAK_ON_NACK);
        if((FT_OK == status) && (6 == bytesTransfered))    break;
        else printf("Write block fail of device[%d] in read_blockext,try again\n",devId);
    }
    status = I2C_DeviceRead(ftHandle[devId], slaveAddress, readSize,bufPtr,&bytesTransfered,I2C_TRANSFER_OPTIONS_START_BIT|I2C_TRANSFER_OPTIONS_STOP_BIT|I2C_TRANSFER_OPTIONS_NACK_LAST_BYTE);
    if(status != FT_OK){
        printf("[Read Step Fail]Read blockext fail of device[%d]\n",devId);
        return status;
    }
    return status;    
}
*/

#endif



