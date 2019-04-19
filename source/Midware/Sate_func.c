#include "Sate_func.h"
#include <math.h>


#define HRCLS_FIRMWARE_FILENAME    "/home/pi/oceancjc/C/Satellite/Init/MxL_5xx_FW.mbin"



MXL_STATUS_E MxL_582_Init(unsigned char devId, oem_data_t* oemdata){
    MXL_STATUS_E status = MXL_SUCCESS;
    FILE *fw = NULL;
    MXL_HYDRA_DEV_XTAL_T *xtalParamPtr = (MXL_HYDRA_DEV_XTAL_T *) malloc( sizeof(MXL_HYDRA_DEV_XTAL_T));
    xtalParamPtr->xtalFreq = MXL_HYDRA_XTAL_24_MHz;    xtalParamPtr->xtalCap = 18;
    MXL_BOOL_E tunerEnable0 = MXL_DISABLE, tunerEnable1 = MXL_DISABLE, tunerEnable2 = MXL_ENABLE;

    
    MxLWare_HYDRA_OEM_DeviceReset(devId);

    if((status = MxLWare_HYDRA_API_CfgDrvInit(devId, oemdata,MXL_HYDRA_DEVICE_582C))!= MXL_SUCCESS){
        printf("Error status of CfgDrvInit is %d\n",status);
        return status;
    }
    if((status = MxLWare_HYDRA_API_CfgDevOverwriteDefault(devId))!= MXL_SUCCESS){
        printf("Error status of CfgDevOverwriteDefault is %d\n",status);
        return status;
    }
    if((status = MxLWare_HYDRA_API_CfgDevXtal(devId, xtalParamPtr, MXL_DISABLE))!= MXL_SUCCESS){
        printf("Error status of CfgDevXtal is %d\n",status);
        free(xtalParamPtr);    xtalParamPtr = NULL;
        return status;
    }
    else    MxLWare_HYDRA_OEM_SleepInMs(48);

    if ((status = MxLWare_HYDRA_API_CfgDevPowerMode(devId, MXL_HYDRA_PWR_MODE_ACTIVE)) != MXL_SUCCESS) {
        printf("Error status of CfgDevPowerMode is %d\n", status);
        return status;        
    }        

    if((fw = fopen(HRCLS_FIRMWARE_FILENAME,"rb"))!= NULL){
        fseek(fw, 0, SEEK_END);
        unsigned int firmwareSize = (unsigned int) ftell(fw);
        fseek(fw, 0, SEEK_SET);
        unsigned char* firmwareBuf = (unsigned char*) malloc(firmwareSize);
        if(firmwareBuf){
            if (fread(firmwareBuf, sizeof(unsigned char), firmwareSize, fw) == firmwareSize){
                    puts("[Info] Start Firmware downloading ..."); 
                    if((status = MxLWare_HYDRA_API_CfgDevFWDownload(devId,firmwareSize,firmwareBuf,NULL))!= MXL_SUCCESS){
                    printf("Error status of CfgDevFWDownload is %d\n",status);
                    free(firmwareBuf);    firmwareBuf = NULL;    fclose(fw);
                    return status;
                }
            }
            else{
                printf("Error in fread for firmware download\n");
                free(firmwareBuf);    firmwareBuf = NULL;    fclose(fw);
            }    
        }
    }
    else{
        printf("Error in fopen for firmware download\n");
        return 0;
    }    
    MXL_HYDRA_VER_INFO_T* rev_info = (MXL_HYDRA_VER_INFO_T*)malloc(sizeof(MXL_HYDRA_VER_INFO_T));
    MxLWare_HYDRA_API_ReqDevVersionInfo(devId,rev_info);
    printf("Device[%d] Chip Version Info: ChipId = %d, chipVersion = %d, API_version = %d.%d.%d.%d-%d, firmware_version = %d.%d.%d.%d-%d, patch_version = %d.%d.%d.%d-%d, fwdownloaded = %d\n",
            devId,rev_info->chipId,rev_info->chipVer,
            rev_info->mxlWareVer[0],rev_info->mxlWareVer[1],rev_info->mxlWareVer[2],rev_info->mxlWareVer[3],rev_info->mxlWareVer[4],
            rev_info->firmwareVer[0],rev_info->firmwareVer[1],rev_info->firmwareVer[2],rev_info->firmwareVer[3],rev_info->firmwareVer[4],
            rev_info->fwPatchVer[0], rev_info->fwPatchVer[1], rev_info->fwPatchVer[2], rev_info->fwPatchVer[3], rev_info->fwPatchVer[4],
            rev_info->firmwareDownloaded);
    free(rev_info);    rev_info = NULL;
            


    
    if((status = MxLWare_HYDRA_API_CfgTunerEnable(devId,MXL_HYDRA_TUNER_ID_0, MXL_ENABLE))!= MXL_SUCCESS){
        printf("Error status of CfgTunerEnable_0 is %d\n",status);
        return status;        
    }    
    MxLWare_HYDRA_OEM_SleepInMs(500);
    
    
    if ((status = MxLWare_HYDRA_API_CfgTunerEnable(devId, MXL_HYDRA_TUNER_ID_1, MXL_ENABLE)) != MXL_SUCCESS) {
        printf("Error status of CfgTunerEnable_1 is %d\n", status);
        return status;        
    }        
    MxLWare_HYDRA_OEM_SleepInMs(500);
    
    if ((status = MxLWare_HYDRA_API_ReqTunerEnableStatus(devId, MXL_HYDRA_TUNER_ID_0, &tunerEnable0)) != MXL_SUCCESS) {
        printf("Error status of Get TunerEnable_status is %d\n", status);
        return status;        
    }  

    if ((status = MxLWare_HYDRA_API_ReqTunerEnableStatus(devId, MXL_HYDRA_TUNER_ID_1, &tunerEnable1)) != MXL_SUCCESS) {
        printf("Error status of Get TunerEnable_status is %d\n", status);
        return status;        
    }   
    if (!tunerEnable0 || !tunerEnable1 || !tunerEnable2) {
        printf("Tuner fail to enable, Tuner_0 status = %d, Tuner_1 status = %d, Tuner_2 status = %d\n", 
               tunerEnable0, tunerEnable1, tunerEnable2);
        return MXL_FAILURE;
    }
    return status;

}



MXL_STATUS_E getlockstatus(uint8_t devId, int demodId, int* fecLock, int* agcLock){
    MXL_STATUS_E status = MXL_SUCCESS;
    MXL_HYDRA_DEMOD_LOCK_T demodLock = {0,0};
    if((status = MxLWare_HYDRA_API_ReqDemodLockStatus(devId,(MXL_HYDRA_DEMOD_ID_E)demodId,&demodLock))!= MXL_SUCCESS){
        printf("Error status of ReqDemodLockStatus is %d\n",status);
    }
    *fecLock = demodLock.fecLock;    *agcLock = demodLock.agcLock;
    return status;
}


MXL_STATUS_E waitdemodlock(uint8_t devId, int demodId, int* fecLock, int* agcLock, uint32_t timeout_ms) {
    MXL_STATUS_E status = MXL_SUCCESS;
    MXL_HYDRA_DEMOD_LOCK_T demodLock = { 0, 0 };
    uint32_t cnt = timeout_ms / INTERVAL_MS + 1;
    if ((status = MxLWare_HYDRA_API_ReqDemodLockStatus(devId, (MXL_HYDRA_DEMOD_ID_E)demodId, &demodLock)) != MXL_SUCCESS) {
        printf("Error status of ReqDemodLockStatus is %d,in func: %s, line: %d\n", status, __func__, __LINE__);
        return status;
    }
    if (demodLock.fecLock){
        *fecLock = demodLock.fecLock;    *agcLock = demodLock.agcLock;
        return status;
    }
    do {
        MxLWare_HYDRA_OEM_SleepInMs(INTERVAL_MS);
        if ((status = MxLWare_HYDRA_API_ReqDemodLockStatus(devId, (MXL_HYDRA_DEMOD_ID_E)demodId, &demodLock)) != MXL_SUCCESS) {
            printf("Error status of ReqDemodLockStatus is %d,in func: %s, line: %d\n", status, __func__, __LINE__);
            return status;
        }
    } while (!demodLock.fecLock && --cnt > 0);
    *fecLock = demodLock.fecLock;    *agcLock = demodLock.agcLock;
    if (cnt == 0)    status = -2;    //TIMEOUT
    return status;
}




MXL_STATUS_E getfreqparameters(uint8_t devId, int demodId, SATDATA_t* data){
    MXL_STATUS_E status = MXL_SUCCESS;
    if (data == NULL)         return MXL_INVALID_PARAMETER;
    MXL_HYDRA_TUNE_PARAMS_T *demodTuneParamsPtr = (MXL_HYDRA_TUNE_PARAMS_T*)calloc(1, sizeof(MXL_HYDRA_TUNE_PARAMS_T));
    if (demodTuneParamsPtr == NULL)   return -2;
    status = MxLWare_HYDRA_API_ReqDemodChanParams(devId, (MXL_HYDRA_DEMOD_ID_E) demodId, demodTuneParamsPtr);
    if (status != MXL_SUCCESS) {
        free(demodTuneParamsPtr);
        return status;
    }    
    switch (data->standardMask) {
    case MXL_HYDRA_DSS:
        data->fecFormat   = (int)demodTuneParamsPtr->params.paramsDSS.fec;
        break;
    case MXL_HYDRA_DVBS:
        data->fecFormat   = (int)demodTuneParamsPtr->params.paramsS.fec;
        data->modulation  = (int)demodTuneParamsPtr->params.paramsS.modulation;
        data->rollOff     = (int)demodTuneParamsPtr->params.paramsS.rollOff;
        break;
    case MXL_HYDRA_DVBS2:
        data->fecFormat   = (int)demodTuneParamsPtr->params.paramsS2.fec;
        data->modulation  = (int)demodTuneParamsPtr->params.paramsS2.modulation;
        data->rollOff     = (int)demodTuneParamsPtr->params.paramsS2.rollOff;
        data->pilots      = (int)demodTuneParamsPtr->params.paramsS2.pilots;
        break;
    default:
        printf("[Error] Invalid standardMask in %s, standardMask = %d\n", __func__, data->standardMask);

    }
    free(demodTuneParamsPtr);
    return status;
    
} 
    


int lock_Demod(SATDEVICE_t* dev, uint8_t tunerId, uint8_t demodId, SATDATA_t* data) {   
    MXL_STATUS_E status = MXL_SUCCESS;
    uint8_t devId = dev->devId;
    if ((status = MxLWare_HYDRA_API_CfgDemodSearchFreqOffset(devId, demodId, MXL_HYDRA_SEARCH_MAX_OFFSET)) != MXL_SUCCESS) {
        printf("Error status of CfgDemodSearchFreqOffset is %d\n", status);
        return -3;       
    }
    MxLWare_HYDRA_OEM_SleepInMs(400);  
    
    MXL_HYDRA_TUNE_PARAMS_T* chanTuneParamsPtr = (MXL_HYDRA_TUNE_PARAMS_T*)calloc(1, sizeof(MXL_HYDRA_TUNE_PARAMS_T));
    if (chanTuneParamsPtr == NULL) {
        printf("[Err] Malloc fail in %s\n", __func__);
        return -2;
    }
    chanTuneParamsPtr->standardMask = (MXL_HYDRA_BCAST_STD_E)data->standardMask;
    chanTuneParamsPtr->frequencyInHz = (uint32_t)(data->frequencyInKHz * 1000);
    chanTuneParamsPtr->symbolRateKSps = (uint32_t)data->symbolRateKSps;
    chanTuneParamsPtr->freqSearchRangeKHz = (uint32_t)data->freqSearchRangeKHz;
    chanTuneParamsPtr->spectrumInfo = (MXL_HYDRA_SPECTRUM_E)data->spectrumInfo;
    switch (data->standardMask) {
    case MXL_HYDRA_DSS:
        chanTuneParamsPtr->params.paramsDSS.fec       = (MXL_HYDRA_FEC_E)data->fecFormat;
        break;
    case MXL_HYDRA_DVBS:
        chanTuneParamsPtr->params.paramsS.fec         = (MXL_HYDRA_FEC_E)data->fecFormat;
        chanTuneParamsPtr->params.paramsS.modulation  = (MXL_HYDRA_MODULATION_E)data->modulation;
        chanTuneParamsPtr->params.paramsS.rollOff     = (MXL_HYDRA_ROLLOFF_E)data->rollOff;
        break;
    case MXL_HYDRA_DVBS2:
        chanTuneParamsPtr->params.paramsS2.fec        = (MXL_HYDRA_FEC_E)data->fecFormat;
        chanTuneParamsPtr->params.paramsS2.modulation = (MXL_HYDRA_MODULATION_E)data->modulation;
        chanTuneParamsPtr->params.paramsS2.rollOff    = (MXL_HYDRA_ROLLOFF_E)data->rollOff;
        chanTuneParamsPtr->params.paramsS2.pilots     = (MXL_HYDRA_PILOTS_E)data->pilots;
        break;
    default:
        printf("[Error] Invalid standardMask in %s, standardMask = %d\n", __func__, data->standardMask);
        free(chanTuneParamsPtr);
        return -3;
    }
 
    if ((status = MxLWare_HYDRA_API_CfgDemodChanTune(devId, tunerId, demodId, chanTuneParamsPtr)) != MXL_SUCCESS) {
        printf("Error status of CfgDemodSearchFreqOffset in %s, value is %d\n", __func__, status);
        free(chanTuneParamsPtr);
        return -3;
    }    
    free(chanTuneParamsPtr);
    return status;
}




MXL_STATUS_E get_pwr_snr(uint8_t devId,int demodId, int* pwr_100dbm, int* snr){
    int16_t snr_short = 0;
    MXL_STATUS_E status =  MxLWare_HYDRA_API_ReqDemodRxPowerLevel(devId,(MXL_HYDRA_DEMOD_ID_E)demodId,pwr_100dbm);
    status |= MxLWare_HYDRA_API_ReqDemodSNR(devId,(MXL_HYDRA_DEMOD_ID_E)demodId, &snr_short);
    *snr = (int)snr_short;
    return status;
}



MXL_STATUS_E getcfo(uint8_t devId,int demodId, int* cfo_Hz, int* sfo_symbol){
    MXL_HYDRA_DEMOD_SIG_OFFSET_INFO_T SigOffset = {0,0};
    MXL_STATUS_E status =  MxLWare_HYDRA_API_ReqDemodSignalOffsetInfo(devId,(MXL_HYDRA_DEMOD_ID_E)demodId,&SigOffset);
    *cfo_Hz = SigOffset.carrierOffsetInHz;        *sfo_symbol = SigOffset.symbolOffsetInSymbol;
    return status;
}



MXL_STATUS_E getscaledber(uint8_t devId, int demodId, uint32_t* scaledBerIter1, uint32_t* scaledBerIter2){
    MXL_HYDRA_DEMOD_SCALED_BER_T bers = {0,0};
    MXL_STATUS_E status = MxLWare_HYDRA_API_ReqDemodScaledBER(devId,(MXL_HYDRA_DEMOD_ID_E)demodId,&bers);
    *scaledBerIter1 = bers.scaledBerIter1;        *scaledBerIter2 = bers.scaledBerIter2;
    return status;
}



MXL_STATUS_E getebn0(uint8_t devId, int demodId, SATDATA_t* sdPtr){
    int modulation = 2;    
    float rolloff = 1.2;
    if (sdPtr->modulation == MXL_HYDRA_MOD_8PSK)          modulation = 3;
    if (sdPtr->rollOff == MXL_HYDRA_ROLLOFF_0_25)         rolloff = 1.25;
    else if (sdPtr->rollOff == MXL_HYDRA_ROLLOFF_0_35)    rolloff = 1.35;
    if (sdPtr->snr == 0 || sdPtr->rollOff == 0)   return MXL_INVALID_PARAMETER;
    sdPtr->ebn0 = sdPtr->snr + 10*log10(sdPtr->rollOff / modulation);
    return MXL_SUCCESS;
}



inline MXL_STATUS_E clearErrorCounter(uint8_t devId, int demodId){
    return MxLWare_HYDRA_API_CfgClearDemodErrorCounters(devId,(MXL_HYDRA_DEMOD_ID_E)demodId);
}



MXL_STATUS_E getberErrcounters(uint8_t devId, int demodId, SATDATA_t* sdPtr ){
    MXL_HYDRA_DEMOD_STATUS_T *demodStatusPtr = (MXL_HYDRA_DEMOD_STATUS_T*)malloc(sizeof(MXL_HYDRA_DEMOD_STATUS_T));
    MXL_STATUS_E status = MxLWare_HYDRA_API_ReqDemodErrorCounters(devId,(MXL_HYDRA_DEMOD_ID_E)demodId,demodStatusPtr);
    if(status == MXL_SUCCESS){
        switch(demodStatusPtr->standardMask){
            case MXL_HYDRA_DSS:
                sdPtr->corrRsErrors    = demodStatusPtr->u.demodStatus_DSS.corrRsErrors;
                sdPtr->rsErrors        = demodStatusPtr->u.demodStatus_DSS.rsErrors;
                sdPtr->berWindow       = demodStatusPtr->u.demodStatus_DSS.berWindow;
                sdPtr->berCount        = demodStatusPtr->u.demodStatus_DSS.berCount;
                sdPtr->corrRsErrors1   = demodStatusPtr->u.demodStatus_DSS.corrRsErrors1;
                sdPtr->rsErrors1       = demodStatusPtr->u.demodStatus_DSS.rsErrors1;
                sdPtr->berWindow_Iter1 = demodStatusPtr->u.demodStatus_DSS.berWindow_Iter1;
                sdPtr->berCount_Iter1  = demodStatusPtr->u.demodStatus_DSS.berCount_Iter1;
                sdPtr->ber_1e_7 = (10000000.0 * demodStatusPtr->u.demodStatus_DSS.berCount) / demodStatusPtr->u.demodStatus_DSS.berWindow;
                break;
            case MXL_HYDRA_DVBS:
                sdPtr->corrRsErrors    = demodStatusPtr->u.demodStatus_DVBS.corrRsErrors;
                sdPtr->rsErrors        = demodStatusPtr->u.demodStatus_DVBS.rsErrors;
                sdPtr->berWindow       = demodStatusPtr->u.demodStatus_DVBS.berWindow;
                sdPtr->berCount        = demodStatusPtr->u.demodStatus_DVBS.berCount;
                sdPtr->corrRsErrors1   = demodStatusPtr->u.demodStatus_DVBS.corrRsErrors1;
                sdPtr->rsErrors1       = demodStatusPtr->u.demodStatus_DVBS.rsErrors1;
                sdPtr->berWindow_Iter1 = demodStatusPtr->u.demodStatus_DVBS.berWindow_Iter1;
                sdPtr->berCount_Iter1  = demodStatusPtr->u.demodStatus_DVBS.berCount_Iter1;
                sdPtr->ber_1e_7 = (10000000.0 * demodStatusPtr->u.demodStatus_DVBS.berCount) / demodStatusPtr->u.demodStatus_DVBS.berWindow;
                break;
            case MXL_HYDRA_DVBS2:
                sdPtr->crcErrors       = demodStatusPtr->u.demodStatus_DVBS2.crcErrors;
                sdPtr->packetErrorCount= demodStatusPtr->u.demodStatus_DVBS2.packetErrorCount;
                sdPtr->totalPackets    = demodStatusPtr->u.demodStatus_DVBS2.totalPackets;
                sdPtr->ber_1e_7 = (10000000.0* demodStatusPtr->u.demodStatus_DVBS2.packetErrorCount) / demodStatusPtr->u.demodStatus_DVBS2.totalPackets;
                break;
            default:
                sdPtr->ber_1e_7 = 1024;
                break;
        }
    }
    free(demodStatusPtr);
    return status;
}



int constInitV2(char** vi, char** vq, int channelPerDev, int numPerLine) {
    numPerLine += 2;
    if (vi == NULL || vq == NULL) {
        return -1;   
    } 

    int i = 0, j = 0;
    for (i = 0; i < channelPerDev; i++) {
        vi[i] = (char*)calloc(numPerLine, sizeof(char));
        vq[i] = (char*)calloc(numPerLine, sizeof(char));
        if (NULL == vi[i] || NULL == vq[i]){
            printf("Malloc vi or vq fail in %s\n",__func__);
            return -2;
        }
    }
    return 0;
    
}



void constClear(char**vi, char**vq, int numOfdev, int channelPerDev) {
    int i = 0, j = 0;
    if (vi == NULL && vq == NULL)    return ;
    for (j = 0; j < channelPerDev; j++) {
            if (vi[j] != NULL)    free(vi[j]);
            if (vq[j] != NULL)    free(vq[j]);
    }
    if (vi != NULL){
        free(vi);    vi = NULL;
    }
    if (vq != NULL){
        free(vq);    vq = NULL;
    }
    
}


MXL_STATUS_E getconst(uint8_t devId, int demodId, char ***vi, char ***vq,  int *index, char len, uint8_t fake_flag){
    int16_t iv,qv;
    while(len--){
        if(MXL_SUCCESS != MxLWare_HYDRA_API_ReqDemodConstellationData(devId,demodId, &iv,&qv))    return MXL_FAILURE;
        if(fake_flag){    iv+= (rand()/1000 -500);    qv+=(rand()/1000 -500);    }
    
        (*index) %= 508;
        vi[2*devId + (*index)/254][demodId][(*index)%254] = iv>>8;    vi[2*devId + (*index)/254][demodId][(*index)%254+1] = iv;
        vq[2*devId + (*index)/254][demodId][(*index)%254] = qv>>8;    vq[2*devId + (*index)/254][demodId][(*index)%254+1] = qv;

        if(vi[2*devId + (*index)/254][demodId][(*index)%254]==0)    vi[2*devId + (*index)/254][demodId][(*index)%254]+=1;
        if(vi[2*devId + (*index)/254][demodId][(*index)%254+1]==0)    vi[2*devId + (*index)/254][demodId][(*index)%254+1]+=1;
        if(vq[2*devId + (*index)/254][demodId][(*index)%254]==0)    vq[2*devId + (*index)/254][demodId][(*index)%254]+=1;
        if(vq[2*devId + (*index)/254][demodId][(*index)%254+1]==0)    vq[2*devId + (*index)/254][demodId][(*index)%254+1]+=1;

//        printf("vi_h = %d, vi_l = %d\nvq_h = %d, vq_l = %d\n",vi[2*devId + (*index)/254][demodId][(*index)%254], vi[2*devId + (*index)/254][demodId][(*index)%254+1],vq[2*devId + (*index)/254][demodId][(*index)%254],vq[2*devId + (*index)/254][demodId][(*index)%254+1]);    
        (*index) +=2;
    }
    return MXL_SUCCESS;
}



MXL_STATUS_E getconstV2(uint8_t devId, int demodId, char **vi, char **vq, SATDATA_t* sdPtr, int *index, char len, uint8_t fake_flag) {
    uint32_t i = (uint32_t)*index;
    while (len--) {
        if (MXL_SUCCESS != MxLWare_HYDRA_API_ReqDemodConstellationData(devId, demodId, &sdPtr->consti[i>>1], &sdPtr->constq[i>>1]))    return MXL_FAILURE;
        if (fake_flag){
            sdPtr->consti[i >> 1] += (rand() / 1000 - 500); 
            sdPtr->constq[i >> 1] += (rand() / 1000 - 500); 
        }
        i %= (CONST_LEN << 1)-2;
        vi[demodId][i] = sdPtr->consti[i >> 1] >> 8;    vi[demodId][i + 1] = sdPtr->consti[i >> 1];
        vq[demodId][i] = sdPtr->constq[i >> 1] >> 8;    vq[demodId][i + 1] = sdPtr->constq[i >> 1];

        if (vi[demodId][i] == 0)        vi[demodId][i]     += 1;
        if (vi[demodId][i + 1] == 0)    vi[demodId][i + 1] += 1;
        if (vq[demodId][i] == 0)        vq[demodId][i]     += 1;
        if (vq[demodId][i + 1] == 0)    vq[demodId][i + 1] += 1;

        MXL_HYDRA_DEBUG("vi_h = %d, vi_l = %d\nvq_h = %d, vq_l = %d\n", vi[demodId][(*index) % 254], vi[demodId][(*index) % 254 + 1], vq[demodId][(*index) % 254], vq[demodId][(*index) % 254 + 1]);    
        i+= 2;
        MxLWare_HYDRA_OEM_SleepInMs(20);
    }
    *index = i;
    return MXL_SUCCESS;
}


int reshape(unsigned char high, unsigned char low) {
    //10bit with D9 is sign bit
    int posi_neg = 1;	
    if (high & 2) {
        posi_neg = -1;
        low = (~low) + 1;	
        high &= 0x01;
        high = 1 - high;
        return posi_neg*((high << 8) + low);	
    }
    return (high << 8) + low ;	
}



int getmer_qpsk(uint8_t devId, int demodId, char **vi, char **vq, SATDATA_t* sdPtr) {
    int x = 0, y = 0;
    int i = 0;
    float pointx, pointy, pointx_s, pointy_s, mer = 0;
    const float f4 = 4.0;
    for (i = 0; i < 508; i += 2) {
        x = reshape(vi[demodId][i % 254], vi[demodId][i % 254 + 1]);	
        y = reshape(vq[demodId][i % 254], vq[demodId][i % 254 + 1]);	
        if (x == 0 && y == 0)	return 0;
        pointx_s = fabs(x / f4);	
        pointx = (int)pointx_s + 0.5;
        pointy_s = fabs(y / f4);	
        pointy = (int)pointy_s + 0.5;

        mer += 10*log10((pow(pointx, 2) + pow(pointy, 2)) / (pow(pointx - pointx_s, 2) + pow(pointy - pointy_s, 2)));
    }
    sdPtr->mer = (int)(mer * 100 / 254);
    return 0;

}



/* Get specturm from receiver, it needs demod and tuner ID
 * Input: fs, fe, st are all in KHz
 * Input: index_s the start index for the spectrum data, need to make conjunction for upper machine
 * Output:point[][3], point[][0] stands for index, point[][1] for spectrum power 
 * */
MXL_STATUS_E getspecturm(uint8_t devId, int demodId, int tunerId, int fs, uint32_t numofFreqSteps, int index_s, int point[FFT_FREAME_LEN][2]) {
    MXL_STATUS_E status = MXL_SUCCESS;
    MXL_HYDRA_SPECTRUM_ERROR_CODE_E spectrumReadbackStatus = MXL_SPECTRUM_NO_ERROR; 
    if (fs < 250000 || numofFreqSteps > MXL_HYDRA_OEM_MAX_BLOCK_WRITE_LENGTH) {
        printf("Error fetch Spectrum, fs = %dshould smaller than 250MHz and a batch = %d number should less than 360", fs, numofFreqSteps);
        return -2;
    }
    int16_t *powerBufPtr = (int16_t*)calloc(numofFreqSteps, sizeof(int16_t));
    status = MxLWare_HYDRA_API_ReqTunerPowerSpectrum(devId, tunerId, demodId, fs, numofFreqSteps, powerBufPtr, &spectrumReadbackStatus);
    if (status != MXL_SUCCESS){
        printf("Error fetch Spectrum, return: %d, SPECTRUM_ERR_CODE: %d\n", status, spectrumReadbackStatus);
        return status;
    }
    
    int i = 0;
    for (i = 0; i < numofFreqSteps; ++i) {
        point[i][0] = i + index_s;
//        if (fake_flag)	point[i][1] = 1 + (int)(210.0*rand() / (RAND_MAX + 1.0));
//        else	
        point[i][1] = powerBufPtr[i];
        MXL_HYDRA_DEBUG("index = %d\t\tpwr = %d\n", point[i][0], point[i][1]);	
    }
    free(powerBufPtr); powerBufPtr = NULL;    
    return status;
}


//void regular_check(int i, SATDATA_t* sdPtr, SATDATATH_t* sdth){
//    static int flag[8] = {0};    // 0---qamlock  1---mpeglock  2---pwrlow   3---pwrhigh   4---berhgh   5---cfo   6---snr    7----mer
//    uint8_t devId = i/8;    int demodId = i%8;
//    getlockstatus(devId, demodId, &sdPtr->fecLock,);
//    get_pwr_snr(devId, demodId, &sdPtr->pwr_100dbm, &sdPtr->snr);
//    getcfo(devId, demodId, &sdPtr->cfo_Hz, &sdPtr->fo_symbol);
//    getberErrcounters(devId, demodId, sdPtr);
//    
//}


MXL_STATUS_E getRegularParamters(uint8_t devId, uint8_t demodId, SATDATA_t* sdPtr) {  
    MXL_STATUS_E status = getlockstatus(devId, demodId, &sdPtr->fecLock, &sdPtr->agcLock);    
    if (status != MXL_SUCCESS) {
        MXL_HYDRA_DEBUG("Get Regular Parameter fail in %s, Line %d, status = %d\n", __func__, __LINE__, status);
        return status;
    } 
    if (sdPtr->fecLock == 0){ 
        sdPtr->cfo_Hz = 0;    sdPtr->mer = 0;    sdPtr->pwr_100dbm = 0;    sdPtr->snr = 0;
        return status;
    }
    status = getfreqparameters(devId, demodId, sdPtr);
    if (status != MXL_SUCCESS) {
        MXL_HYDRA_DEBUG("Get Regular Parameter fail in %s, Line %d, status = %d\n", __func__, __LINE__, status);
        return status;
    }   
    status = getcfo(devId, demodId, &sdPtr->cfo_Hz, &sdPtr->fo_symbol);
    if (status != MXL_SUCCESS) {
        MXL_HYDRA_DEBUG("Get Regular Parameter fail in %s, Line %d, status = %d\n", __func__, __LINE__, status);
        return status;
    }   
    status = get_pwr_snr(devId, demodId, &sdPtr->pwr_100dbm, &sdPtr->snr);
    if (status != MXL_SUCCESS) {
        MXL_HYDRA_DEBUG("Get Regular Parameter fail in %s, Line %d, status = %d\n", __func__, __LINE__, status);
        return status;
    }   
    status = getberErrcounters(devId, demodId, sdPtr);
    if (status != MXL_SUCCESS) {
        MXL_HYDRA_DEBUG("Get Regular Parameter fail in %s, Line %d, status = %d\n", __func__, __LINE__, status);
    }
    status = getebn0(devId, demodId, sdPtr);
    if (status != MXL_SUCCESS) {
        MXL_HYDRA_DEBUG("Get Regular Parameter fail in %s, Line %d, status = %d\n", __func__, __LINE__, status);
    }
    return status;
}



void split(char a, char*in, char*o1, char*o2){
    int i = 0;
    for(i=0;i<strlen(in);i++){
        if(in[i]!=a)    o1[i] = in[i];
        else{
            memset(&o1[i],0,strlen(&o1[i]));
            break;
        }
    }
    strcpy(o2,&in[i+1]);
}

void remote_ip_load(TRAPIP_t* trapip){
    FILE *fp = NULL;
    if((fp=fopen("/usr/local/netsnmp4rasp/share/snmp/snmpd.conf","r"))==NULL){
        printf("cannot open file");
        return;
    }
    char start[36],title[36], ip[28],pub[38],port[8],sink[16];     int index = 0;
    while(fgets(start,35,fp)){
        if(strstr(start,"master"))      break;
    }
    while(!feof(fp)){
        if( fscanf(fp,"%s\t%d\n%s%s%s",title,&index,sink,ip,pub)!=5 )  continue;
//`       {puts("begin------------------");puts(title);puts(sink);puts(ip);puts(pub);}
        if(strstr(title,"This_") ){
//                              puts(ip);
            split(':',ip,trapip[index].ip,port);
            trapip[index].port = atoi(port);
            if( ++index== MAX_TRAP_NUM )    break;
       }
    }
    fclose(fp);
}


void remote_ip_change(char index, TRAPIP_t* trapip){
    FILE *fp = NULL;
    if((fp=fopen("/usr/local/netsnmp4rasp/share/snmp/snmpd.conf","r+"))==NULL){
        printf("cannot open file");
        return;
    }
    char cmd[256] = ""; 
    char start[36],title[28], ip[28],pub[38],port[8],sink[16];    int flag_find = 0;
    char TextStr[2048] = {0}, buffer[256];
    char TargetStr[36]="", ModifyStr[256]="";
    sprintf(TargetStr,"%s%d%s","#This_is\t",index,"\n");  
    sprintf(ModifyStr,"%s%s%c%d%s","trap2sink\t",trapip[index].ip,':',trapip[index].port,"\tpublic\n");
    while(!feof(fp)){
        memset(buffer, 0, sizeof(buffer)); 
        fgets(buffer, sizeof(buffer)-1, fp);
        if (!strcmp(buffer, TargetStr)){
            strcat(TextStr, buffer);
            fgets(buffer, sizeof(buffer)-1, fp);
            strcat(TextStr, ModifyStr);
            flag_find = 1; 
        }
        else
           strcat(TextStr, buffer);
    } 
    fclose(fp);
    if(flag_find){ 
        if((fp=fopen("/usr/local/netsnmp4rasp/share/snmp/snmpd.conf","r+"))==NULL){
            printf("cannot open file");
            return;
        }
        fwrite(TextStr, strlen(TextStr),1, fp);
        fclose(fp);    
        return;
    }
    sprintf(cmd,"sed '$ a%s%d\\n%s\\t%s%c%d\\t%s' -i /usr/local/netsnmp4rasp/share/snmp/snmpd.conf","#This_is\t",index,"trap2sink",trapip[index].ip,':',trapip[index].port,"public");
    MXL_HYDRA_DEBUG("%s\n",cmd);
    system(cmd);
}

void udpTarget_load(UDPIP_t* udpPtr, int* mode){
    FILE *fp = NULL;
    if ((fp = fopen(UDPADDR_CONFIG_FILE, "r")) == NULL) {
        printf("No file,create udpTarget\n");
        fp = fopen(UDPADDR_CONFIG_FILE, "a+");
        fprintf(fp,"SW_MODE = 0\n ");
        fclose(fp);
        return; 
    }
    char ip[16] = {0};     int index = 0, node = 0, port = 0;
    while(fscanf(fp,"SW_MODE = %d",mode)== 1){
        printf("Current working mode is %d\n",*mode);      break;
    }
    while(!feof(fp) && udpPtr!= NULL){
        if( fscanf(fp,"%d\t%s\t%d",&node,ip,&port)!= 3 )  continue;
        strcpy(udpPtr[node].ip,ip);    udpPtr[node].port = port;
        if( ++index== MAX_TRAP_NUM )    break;
    }
    fclose(fp);
}


void udpTarget_Modeupdate(int mode){
    FILE *fp = NULL;
    if ((fp = fopen(UDPADDR_CONFIG_FILE, "r")) == NULL) {
        printf("No udpTarget,update fail\n");
        return; 
    }
    int curr_mode = 0;    char cmd[256] = {0};
    while(fscanf(fp,"SW_MODE = %d",&curr_mode)== 1){
        if(mode == curr_mode){
            printf("Current working mode is the same, no change needed\n");
            fclose(fp);        return;
        }
        fclose(fp);
        sprintf(cmd,"sed -i 's/\\(SW_MODE =\\).*/\\1 %d/' /etc/dpkg/origins/.udp_config",mode);
        system(cmd);
        return;
    }
}


void udpTarget_IPupdate(int index, UDPIP_t* udpPtr){
    FILE *fp = NULL;
    if ((fp = fopen(UDPADDR_CONFIG_FILE, "r")) == NULL) {
        printf("No udpTarget,update fail\n");
        return; 
    }
    char cmd[512] = {0};
    int node = 0,    port = 0;    char ip[16] = {0};
    fgets(cmd,255,fp);
    memset(cmd,0,256); 
    while(fscanf(fp,"%d\t%s\t\t%d",&node,ip,&port) == 3 && udpPtr!=NULL){
        if(node == index){
            fclose(fp);
            sprintf(cmd,"sed -i 's/\\(^%d\\)\\t.*/\\1\\t%s\\t%d/' /etc/dpkg/origins/.udp_config",
            index, udpPtr[index].ip, udpPtr[index].port);
            system(cmd);
            return;
        }
    }
    fclose(fp);
    sprintf(cmd,"sed '$ i%d\\t%s\\t%d' -i /etc/dpkg/origins/.udp_config",
    index, udpPtr[index].ip, udpPtr[index].port);
    system(cmd);
    return;
}

void confile_load(SATDATA_t* sdPtr[MAX_CHANNEL_NUM], unsigned int gstatus ){
    FILE *fp = NULL;
    if ((fp = fopen(SATLITE_CHANNEL_CONFIG_FILE, "r")) == NULL) {
        printf("No file,create config\n");
        fp = fopen(SATLITE_CHANNEL_CONFIG_FILE, "a+");
        fwrite(" ",1,1,fp);
        fclose(fp);
        return;    
    }
    int node = 0,std = 0, freq = 0, range = 0, rate = 0, spec = 0, fec = 0, mod = 0;
    while(fscanf(fp,"%d %d %d %d %d %d %d %d",&node,&std,&freq,&range,&rate,&spec,&fec,&mod)== 8){
        sdPtr[node]->standardMask = std;     sdPtr[node]->frequencyInKHz = freq;  sdPtr[node]->freqSearchRangeKHz = range;       
        sdPtr[node]->symbolRateKSps = rate;  sdPtr[node]->spectrumInfo = spec;    sdPtr[node]->fecFormat = fec;
        sdPtr[node]->modulation = mod;
        if(sdPtr[node]->frequencyInKHz && ((1&(gstatus>> node/8))==0) )    
            mxl_lockDemod(gstatus,0, node/MAX_CHANNEL_NUM, 1, node%MAX_CHANNEL_NUM, sdPtr[node]);
        else    printf("load confile line node %d fail, gstatus = %d\n",node,gstatus);    
    }
    fclose(fp);
}



int confile_loadV2(SATDATA_t* sdPtr[MAX_CHANNEL_NUM]) {
    FILE *fp = NULL;
    if ((fp = fopen(SATLITE_CHANNEL_CONFIG_FILE, "r")) == NULL) {
        printf("No file,create config\n");
        fp = fopen(SATLITE_CHANNEL_CONFIG_FILE, "a+");
        fwrite(" ", 1, 1, fp);
        fclose(fp);
        return 0;    
    }
    if (sdPtr == NULL){
        printf("[Err] NULL Pointer for SATDATA_t\n");
        return -1;
    }
    int node = 0, std = 0, freq = 0, range = 0, rate = 0, spec = 0, fec = 0, mod = 0;
    while (fscanf(fp, "%d %d %d %d %d %d %d %d", &node, &std, &freq, &range, &rate, &spec, &fec, &mod) == 8) {
        sdPtr[node]->standardMask = std; sdPtr[node]->frequencyInKHz = freq; sdPtr[node]->freqSearchRangeKHz = range;       
        sdPtr[node]->symbolRateKSps = rate; sdPtr[node]->spectrumInfo = spec; sdPtr[node]->fecFormat = fec;
        sdPtr[node]->modulation = mod;  
    }
    fclose(fp);
    return 0;
}



void confile_update(char index, SATDATA_t* sdPtr){
    FILE *fp = NULL;
    if ((fp = fopen(SATLITE_CHANNEL_CONFIG_FILE, "r+")) == NULL) {
            printf("cannot open file");
            return;
    }
    char cmd[128] = ""; 
    int node = 0,std = 0, freq = 0, range = 0, rate = 0, spec = 0, fec = 0, mod = 0;
        while(fscanf(fp,"%d %d %d %d %d %d %d %d",&node,&std,&freq,&range,&rate,&spec,&fec,&mod)== 8){
            if (node == (int)index){
            fclose(fp);    
            sprintf(cmd,"sed -i 's/\\(^%d\\)\\t.*/\\1\\t%d\\t%d\\t%d\\t%d\\t%d\\t%d\\t%d/' /etc/dpkg/origins/.sate_config",
            index, sdPtr->standardMask, sdPtr->frequencyInKHz, sdPtr->freqSearchRangeKHz, sdPtr->symbolRateKSps,
            sdPtr->spectrumInfo = spec, sdPtr->fecFormat, sdPtr->modulation);
            system(cmd);
            return;
        } 
    }
    fclose(fp);
    sprintf(cmd,"sed '$ i%d\\t%d\\t%d\\t%d\\t%d\\t%d\\t%d\\t%d' -i /etc/dpkg/origins/.sate_config",
    index, sdPtr->standardMask, sdPtr->frequencyInKHz, sdPtr->freqSearchRangeKHz, sdPtr->symbolRateKSps,
    sdPtr->spectrumInfo = spec, sdPtr->fecFormat, sdPtr->modulation);
    system(cmd);
}


void confileth_load (SATDATATH_t* sate_thPtr[MAX_CHANNEL_NUM] ){
    FILE *fp = NULL;
    if((fp=fopen("/etc/network/sateth_config","r"))==NULL){
        printf("No file,create thconfig\n");
        fp=fopen("/etc/network/sateth_config","a+");
        fwrite(" ",1,1,fp);
        fclose(fp);
        return;    
    }
    int node,pwrlow,pwrhigh,snrlow,berlow,merlow,evmlow,cfomax,sfomax,qamlocksw;
    while(fscanf(fp,"%d %d %d %d %d %d %d %d %d %d",&node,&pwrlow,&pwrhigh,&snrlow,&berlow,&merlow,&evmlow,&cfomax,&sfomax,&qamlocksw)== 10){
        sate_thPtr[node]->rxpwrLow = pwrlow;        sate_thPtr[node]->rxpwrHigh = pwrhigh;      sate_thPtr[node]->snrLow = snrlow;    
        sate_thPtr[node]->lockSwitch = qamlocksw;   sate_thPtr[node]->berMax = berlow;          sate_thPtr[node]->merLow = merlow;           
        sate_thPtr[node]->cfoMax_Hz = cfomax;       sate_thPtr[node]->evmLow = evmlow;          sate_thPtr[node]->sfo_symbol = sfomax;
    }
    fclose(fp);
}

void confileth_update(char index, SATDATATH_t* sate_thPtr){
    FILE *fp = NULL;
    if((fp=fopen("/etc/network/sateth_config","r+"))==NULL){
        printf("cannot open file");
        return;
    }
    char cmd[256] = ""; 
    int node,pwrlow,pwrhigh,snrlow,berlow,merlow,evmlow,cfomax,sfomax,qamlocksw;
    while(fscanf(fp,"%d %d %d %d %d %d %d %d %d %d",&node,&pwrlow,&pwrhigh,&snrlow,&berlow,&merlow,&evmlow,&cfomax,&sfomax,&qamlocksw)== 10){
        if (node == (int)index){
            fclose(fp);    
            sprintf(cmd,"sed -i 's/\\(^%d\\)\\t.*/\\1\\t\%d\\t%d\\t%d\\t%d\\t%d\\t%d\\t%d\\t%d\\t%d/' /etc/network/sateth_config",index,sate_thPtr->rxpwrLow,
                sate_thPtr->rxpwrHigh,sate_thPtr->snrLow,sate_thPtr->berMax,sate_thPtr->merLow,sate_thPtr->evmLow,sate_thPtr->cfoMax_Hz,
                sate_thPtr->sfo_symbol,sate_thPtr->lockSwitch);
//          puts(cmd);
            system(cmd);
            return;
        } 
    }
    fclose(fp);
    sprintf(cmd,"sed '$ i%d\\t\%d\\t%d\\t%d\\t%d\\t%d\\t%d\\t%d\\t%d\\t%d' -i /etc/network/sateth_config",index,sate_thPtr->rxpwrLow,sate_thPtr->rxpwrHigh,
        sate_thPtr->snrLow,sate_thPtr->berMax,sate_thPtr->merLow,sate_thPtr->evmLow,sate_thPtr->cfoMax_Hz,sate_thPtr->sfo_symbol,sate_thPtr->lockSwitch);
//    puts(cmd);
    system(cmd);
}
