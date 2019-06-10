#include "LoopApp.h"




void loopAppDev_clear(LoopApp_t* dev){
    if (dev == NULL)    return ;
    if (dev->satDevice != NULL) {
        free(dev->satDevice);
        dev->satDevice = NULL;
    }
    uint8_t i = 0;
    while (i < MAX_CHANNEL_NUM){
        if (dev->sdPtr[i] != NULL) {
            free(dev->sdPtr[i]);       dev->sdPtr[i] = NULL;
        }
        if (dev->sd_thPtr[i] != NULL) {
            free(dev->sd_thPtr[i]);    dev->sd_thPtr[i] = NULL;
        }
        i++;
    }
    constClear(dev->vi, dev->vq, 1, MAX_CHANNEL_NUM);
    free(dev);    dev = NULL;
    
}




int loopAppDev_init(LoopApp_t* dev, uint8_t devId, uint8_t* i2c_status){
    if (dev == NULL) {
        logcjc("NULL pointer @ input parameter in %s\n", __func__);
        return -1;
    }    
    if (NULL == (dev->satDevice = (SATDEVICE_t*)calloc(1, sizeof(SATDEVICE_t)))){
        logcjc("[ERR] MALLOC memory for satDevice fail in %sn", __func__);
        return -2;
    }
    uint8_t i = 0;
    for (i = 0; i < MAX_CHANNEL_NUM; i++) {
        dev->sdPtr[i]    = (SATDATA_t*)calloc(1, sizeof(SATDATA_t));
        dev->sd_thPtr[i] = (SATDATATH_t*)calloc(1, sizeof(SATDATATH_t));
        if (dev->sdPtr[i] == NULL || dev->sd_thPtr[i] == NULL) {
            logcjc("[ERR] MALLOC memory for sd(th)Ptr[%d] fail in %sn", i, __func__);
            return -2;
        }
    }   
    dev->vi = (char**)malloc(MAX_CHANNEL_NUM*sizeof(char*));
    dev->vq = (char**)malloc(MAX_CHANNEL_NUM*sizeof(char*));
    if (dev->vi == NULL || dev->vq == NULL) {
        return -2;
    }
    constInitV2(dev->vi, dev->vq, MAX_CHANNEL_NUM, 512);
    dev->satDevice->devId = devId;
    dev->satDevice->i2c_status = i2c_status;
    dev->LoopAppDevice_Init = NULL;
    dev->LoopAppDevice_Clear = NULL;
    return 0;
    
}





struct pack_t {
    LoopApp_t* loopDevice; MXL_HYDRA_TUNER_ID_E tunerId; MXL_HYDRA_DEMOD_ID_E demodId;
    uint8_t updateFlag;
};



void* mxl_lockDemod_inThread(void* package) {
    pthread_detach(pthread_self());    
    struct pack_t pack = *(struct pack_t*)package;
    MXL_STATUS_E status = MXL_SUCCESS;
    uint8_t devId = pack.loopDevice->satDevice->devId;
    
    status = lock_Demod(pack.loopDevice->satDevice, pack.tunerId, pack.demodId, pack.loopDevice->sdPtr[pack.demodId]);
    if ( status != MXL_SUCCESS)        printf("Error status of lock_Demod in %s of Line %d is %d\n", __func__, __LINE__, status);
    free(package);
    pack.loopDevice->sdPtr[pack.demodId]->parachange = 0;
    pthread_exit(NULL);
}






MXL_STATUS_E mxl_lockDemodV2(LoopApp_t* dev, uint8_t update, MXL_HYDRA_TUNER_ID_E tunerId, MXL_HYDRA_DEMOD_ID_E demodId) {
    pthread_t th = 0;
    int ret = 0;
    if (*dev->satDevice->i2c_status != 0) {
        printf("I2C communication fail in  %s\n", __func__);
        return 0;
    } 
    struct pack_t *pack = (struct pack_t*)malloc(sizeof(struct pack_t));
    if (pack == NULL){
        printf("[Err] Malloc fail in %s\n",__func__);
    }
    pack->loopDevice = dev; pack->tunerId = tunerId; pack->demodId = demodId; pack->updateFlag = update;
    dev->sdPtr[demodId]->fecLock = 0; dev->sdPtr[demodId]->pwr_100dbm = 0; dev->sdPtr[demodId]->snr = 0;
    dev->sdPtr[demodId]->cfo_Hz = 0; dev->sdPtr[demodId]->fo_symbol = 0; dev->sdPtr[demodId]->preber_1e_7 = 0;
    dev->sdPtr[demodId]->corrRsErrors = 0; dev->sdPtr[demodId]->rsErrors = 0; dev->sdPtr[demodId]->berWindow = 0;
    dev->sdPtr[demodId]->berCount = 0; dev->sdPtr[demodId]->corrRsErrors1 = 0; dev->sdPtr[demodId]->rsErrors1 = 0;
    dev->sdPtr[demodId]->berWindow_Iter1 = 0; dev->sdPtr[demodId]->berCount_Iter1 = 0;
    dev->sdPtr[demodId]->crcErrors = 0; dev->sdPtr[demodId]->packetErrorCount = 0; dev->sdPtr[demodId]->totalPackets = 0;
    memset(dev->vi[demodId], 0, 254); memset(dev->vi[demodId], 0, 254);
    memset(dev->vq[demodId], 0, 254); memset(dev->vq[demodId], 0, 254);
    
    if (update)    confile_update(dev->satDevice->devId * 8 + demodId, dev->sdPtr[demodId]);
    if ((ret = pthread_create(&th, NULL, mxl_lockDemod_inThread, (void*)pack)) != 0) {
        free(pack);
        printf("[ERROR] Create thread for Lock channel %d Fail,ret = %d\n", dev->satDevice->devId* 8 + demodId, ret);
        return MXL_FAILURE;
    }
    return MXL_SUCCESS;
}




/* Try to lock all channels
 * lockedchannels[31:16] = active channel
 * lockedchannels[15:0]  = locked channel
 * return active channels but lock fail, if >0, means some channel lock fail
 **/
int lockAllDemodsV2(LoopApp_t* device, uint8_t tunerId, uint32_t* lockedchannels) {
    int node = 0, ret = 0, fecLock = 0, agcLock = 0;
    uint32_t activechannelMsk = 0, lockedchannelMsk = 0, errchannelMsk = 0;
    for (node = 0; node < MAX_CHANNEL_NUM; node++) {
        if (device->sdPtr[node]->frequencyInKHz && (*device->satDevice->i2c_status == 0)) {
            activechannelMsk |= (1 << node);
            ret = mxl_lockDemodV2(device, 0, (MXL_HYDRA_TUNER_ID_E)tunerId, node % MAX_CHANNEL_NUM);
            if (ret != MXL_SUCCESS) {
                printf("Try to Lock Node %d Fail, status = %d, try next ......\n", node, ret);
                errchannelMsk |= (1 << node) ;
            }  
        }
    }
    
    for (node = 0; node < MAX_CHANNEL_NUM; node++) {    
        if (activechannelMsk & (1 << node)){
            ret = waitdemodlock(device->satDevice->devId, node % MAX_CHANNEL_NUM, &device->sdPtr[node]->fecLock, &device->sdPtr[node]->agcLock, 1000);
            if (ret == MXL_SUCCESS && device->sdPtr[node]->fecLock == 1) {
                getfreqparameters(device->satDevice->devId, node, device->sdPtr[node]);
                lockedchannelMsk |= (1 << node); 
            }    
            else{
                printf("Wait Node %d Lock Fail, status = %d, try next ......\n", node,ret);
                errchannelMsk |= (1 << node);
            }
        }

    }
    
    *lockedchannels = (activechannelMsk << 16) | lockedchannelMsk;
    return errchannelMsk;    
}


typedef enum threadshold {
    QAMLOCKMSK  = 1,
    MPEGLOCKMSK = 2,
    PWRLOWMSK   = 4,
    PWRHIGHMSK  = 8,
    BERHIGHMSK  = 0x10,
    CFOHIGHMSK  = 0x20,
    SNRLOWMSK   = 0x40,
    MERHIGHMSK  = 0x80 
}THRESHOLD_e;

void regular_checkPerChannel(uint8_t devId, uint8_t demodId, SATDATA_t* sdPtr, SATDATATH_t* sdth) {
    static uint32_t flag[MAX_CHANNEL_NUM] = { 0 };       // 0---qamlock  1---mpeglock  2---pwrlow   3---pwrhigh   4---berhgh   5---cfo   6---snr    7----mer
    getRegularParamters(devId, demodId, sdPtr);
    if (sdPtr->fecLock == 0 && flag[demodId] & QAMLOCKMSK)
    {
        send_feclocktrap_trap(demodId, "FEC Unlock, status from 1 to 0");
        flag[demodId] &= ~(uint32_t)QAMLOCKMSK;
    }
    else if (sdPtr->fecLock && !(flag[demodId] & QAMLOCKMSK)) {
        send_feclocktrap_trap(demodId, "FEC Lock, status from 0 to 1");
        flag[demodId] |= (uint32_t)QAMLOCKMSK;
    }
    if (sdPtr->fecLock == 0)    return ;
    

    if (sdPtr->pwr_100dbm < sdth->rxpwrLow && !(flag[demodId] & PWRLOWMSK))
    {
        send_rxpwrtrap_trap(demodId, "Power Low");
        flag[demodId] |= (uint32_t)PWRLOWMSK;    
        flag[demodId] &= ~(uint32_t)PWRHIGHMSK;
    }
    else if (sdPtr->pwr_100dbm > sdth->rxpwrHigh && !(flag[demodId] & PWRHIGHMSK))
    {
        send_rxpwrtrap_trap(demodId, "Power High");
        flag[demodId] |= (uint32_t)PWRHIGHMSK;    
        flag[demodId] &= ~(uint32_t)PWRLOWMSK;
    }
    else if (sdPtr->pwr_100dbm >= sdth->rxpwrLow && sdPtr->pwr_100dbm <= sdth->rxpwrHigh &&
            (flag[demodId]&(PWRLOWMSK | PWRHIGHMSK))) {
        send_rxpwrtrap_trap(demodId, "Power back to normal");
        flag[demodId] &= ~(uint32_t)(PWRLOWMSK | PWRHIGHMSK);
    }
    if (sdPtr->snr < sdth->snrLow && !(flag[demodId] & SNRLOWMSK))
    {
        send_snrtrap_trap(demodId, "SNR Low");
        flag[demodId] |= (uint32_t)SNRLOWMSK;
    }
    else if (sdPtr->snr >= sdth->snrLow && (flag[demodId] & SNRLOWMSK)) {
        send_snrtrap_trap(demodId, "SNR Recover");
        flag[demodId] &= ~(uint32_t)SNRLOWMSK;
    }
    
    
    if (sdPtr->cfo_Hz <= sdth->cfoMax_Hz && (flag[demodId]&CFOHIGHMSK))
    {
        send_freqoffsettrap_trap(demodId, "CFO Recover");
        flag[demodId] &= ~(uint32_t)CFOHIGHMSK;
    }
    else if (sdPtr->cfo_Hz > sdth->cfoMax_Hz && !(flag[demodId] & CFOHIGHMSK)) {
        send_freqoffsettrap_trap(demodId, "CFO Large");
        flag[demodId] |= (uint32_t)CFOHIGHMSK;
    }
    

    if (sdPtr->preber_1e_7 < sdth->berMax && (flag[demodId] & BERHIGHMSK))
    {
        send_mertrap_trap(demodId, "BER Recover");
        flag[demodId] &= ~(uint32_t)BERHIGHMSK;
    }
    else if (sdPtr->preber_1e_7 >= sdth->berMax && !(flag[demodId] & BERHIGHMSK)) {
        send_mertrap_trap(demodId, "BER Large");
        flag[demodId] |= (uint32_t)BERHIGHMSK;        
    }
    
}







/*************** SNMP Related Part *******************/
void snmp_server_init(const char* app_name) {
    //const char *app_name = "Satesingalmon";  
    /* we are a subagent */  
    netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_AGENT_ROLE, 1);  
    /* initialize the agent library */  
    init_agent(app_name);  
    /* Test will be used to read Test.conf files. */  
    init_snmp(app_name);   
    init_devip();
    init_sateTable();
    init_satethTable();
    init_consTable();
    init_sysdescr();
    init_udpTable();  
}


inline void snmp_server_loop(int blockinterval_second){
    agent_check_and_process(blockinterval_second); /* block every 1 second */
}



void snmp_server_close(const char* app_name){
    /* at shutdown time */  
    snmp_shutdown(app_name);  
    /* shutdown the agent library */  
    shutdown_agent(); 
}



/* frame format: #$PW#$SNR#$BER#$POSTBER */
int framerParam(LoopApp_t* dev,char* frame) {
    uint8_t i = 0;
    if (dev == NULL || frame == NULL)    return -1;
    char *subframePWR     = (char*)malloc(sizeof(char) * 256);
    char *subframeSNR     = (char*)malloc(sizeof(char) * 256);
    char *subframeBER     = (char*)malloc(sizeof(char) * 256);
    char *subframePOSTBER = (char*)malloc(sizeof(char) * 256);
    if (subframePWR == NULL || subframeSNR == NULL || subframeBER == NULL || subframePOSTBER == NULL){
        if (subframePWR != NULL)        free(subframePWR);
        if (subframeSNR != NULL)        free(subframeSNR);
        if (subframeBER != NULL)        free(subframeBER);
        if (subframePOSTBER != NULL)    free(subframePOSTBER);
        return -1;
    }
    sprintf(subframePWR, "#$%d$%%%d$%%%d$%%%d$%%%d$%%%d$%%%d$%%%d", 
            dev->sdPtr[0]->pwr_100dbm, dev->sdPtr[1]->pwr_100dbm,
            dev->sdPtr[2]->pwr_100dbm, dev->sdPtr[3]->pwr_100dbm,
            dev->sdPtr[4]->pwr_100dbm, dev->sdPtr[5]->pwr_100dbm,
            dev->sdPtr[6]->pwr_100dbm, dev->sdPtr[7]->pwr_100dbm);
    sprintf(subframeSNR, "#$%d$%%%d$%%%d$%%%d$%%%d$%%%d$%%%d$%%%d", 
            dev->sdPtr[0]->snr, dev->sdPtr[1]->snr,
            dev->sdPtr[2]->snr, dev->sdPtr[3]->snr,
            dev->sdPtr[4]->snr, dev->sdPtr[5]->snr,
            dev->sdPtr[6]->snr, dev->sdPtr[7]->snr);
    sprintf(subframeBER, "#$%d$%%%d$%%%d$%%%d$%%%d$%%%d$%%%d$%%%d", 
            dev->sdPtr[0]->preber_1e_7, dev->sdPtr[1]->preber_1e_7,
            dev->sdPtr[2]->preber_1e_7, dev->sdPtr[3]->preber_1e_7,
            dev->sdPtr[4]->preber_1e_7, dev->sdPtr[5]->preber_1e_7,
            dev->sdPtr[6]->preber_1e_7, dev->sdPtr[7]->preber_1e_7);
    sprintf(subframePOSTBER, "#$%d$%%%d$%%%d$%%%d$%%%d$%%%d$%%%d$%%%d", 
            dev->sdPtr[0]->postber_1e_7, dev->sdPtr[1]->postber_1e_7,
            dev->sdPtr[2]->postber_1e_7, dev->sdPtr[3]->postber_1e_7,
            dev->sdPtr[4]->postber_1e_7, dev->sdPtr[5]->postber_1e_7,
            dev->sdPtr[6]->postber_1e_7, dev->sdPtr[7]->postber_1e_7);
    sprintf(frame, "%s%s%s%s", subframePWR, subframeSNR, subframeBER, subframePOSTBER);
    free(subframePWR);    free(subframeSNR);    free(subframeBER);    free(subframePOSTBER);
    return 0;
    
}



int cmdAnalysis(char* cmd, char* opcode, int* opdata){
    if (opcode == NULL || opdata == NULL || cmd == NULL)    return -1;
    return sscanf(cmd, "#$%[^#$]#$%d#$", opcode, opdata);
}