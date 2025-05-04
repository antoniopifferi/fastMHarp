
/*
    MHLib programming library for MultiHarp 150/160
    PicoQuant GmbH

    Ver. 4.0.0.0     February 2025
*/


extern int MH_GetLibraryVersion(char* vers);
extern int MH_GetErrorString(char* errstring, int errcode);

extern int MH_OpenDevice(int devidx, char* serial);
extern int MH_CloseDevice(int devidx);
extern int MH_Initialize(int devidx, int mode, int refsource);

//all functions below can only be used after MH_Initialize

extern int MH_GetHardwareInfo(int devidx, char* model, char* partno, char* version);
extern int MH_GetSerialNumber(int devidx, char* serial);
extern int MH_GetFeatures(int devidx, int* features);
extern int MH_GetBaseResolution(int devidx, double* resolution, int* binsteps);
extern int MH_GetNumOfInputChannels(int devidx, int* nchannels);

extern int MH_SetSyncDiv(int devidx, int div);
extern int MH_SetSyncEdgeTrg(int devidx, int level, int edge);
extern int MH_SetSyncChannelOffset(int devidx, int value);
extern int MH_SetSyncChannelEnable(int devidx, int enable);   //new since v3.1
extern int MH_SetSyncDeadTime(int devidx, int on, int deadtime);  //new since v1.1

extern int MH_SetInputEdgeTrg(int devidx, int channel, int level, int edge);
extern int MH_SetInputChannelOffset(int devidx, int channel, int value);
extern int MH_SetInputDeadTime(int devidx, int channel, int on, int deadtime);  //new since v1.1
extern int MH_SetInputHysteresis(int devidx, int hystcode);   //new since v3.0
extern int MH_SetInputChannelEnable(int devidx, int channel, int enable);

extern int MH_SetStopOverflow(int devidx, int stop_ovfl, unsigned int stopcount);
extern int MH_SetBinning(int devidx, int binning);
extern int MH_SetOffset(int devidx, int offset);
extern int MH_SetHistoLen(int devidx, int lencode, int* actuallen);
extern int MH_SetMeasControl(int devidx, int control, int startedge, int stopedge);
extern int MH_SetTriggerOutput(int devidx, int period);

extern int MH_ClearHistMem(int devidx);
extern int MH_StartMeas(int devidx, int tacq);
extern int MH_StopMeas(int devidx);
extern int MH_CTCStatus(int devidx, int* ctcstatus);

extern int MH_GetHistogram(int devidx, unsigned int *chcount, int channel);
extern int MH_GetAllHistograms(int devidx, unsigned int *chcount);
extern int MH_GetResolution(int devidx, double* resolution);
extern int MH_GetSyncPeriod(int devidx, double* period);
extern int MH_GetSyncRate(int devidx, int* syncrate);
extern int MH_GetCountRate(int devidx, int channel, int* cntrate);
extern int MH_GetAllCountRates(int devidx, int* syncrate, int* cntrates);
extern int MH_GetFlags(int devidx, int* flags);
extern int MH_GetElapsedMeasTime(int devidx, double* elapsed);
extern int MH_GetStartTime(int devidx, unsigned int* timedw2, unsigned int* timedw1, unsigned int* timedw0);

extern int MH_GetWarnings(int devidx, int* warnings);
extern int MH_GetWarningsText(int devidx, char* text, int warnings);

//for the time tagging modes only
extern int MH_SetOflCompression(int devidx, int holdtime);  //new since v3.1
extern int MH_SetMarkerHoldoffTime(int devidx, int holdofftime);
extern int MH_SetMarkerEdges(int devidx, int me1, int me2, int me3, int me4);
extern int MH_SetMarkerEnable(int devidx, int en1, int en2, int en3, int en4);
extern int MH_ReadFiFo(int devidx, unsigned int* buffer, int* nactual);

//for event filtering, time tagging modes only, all new since v3.1
extern int MH_SetRowEventFilter(int devidx, int rowidx, int timerange, int matchcnt, int inverse, int usechannels, int passchannels);
extern int MH_EnableRowEventFilter(int devidx, int rowidx, int enable);
extern int MH_SetMainEventFilterParams(int devidx, int timerange, int matchcnt, int inverse);
extern int MH_SetMainEventFilterChannels(int devidx, int rowidx, int usechannels, int passchannels);
extern int MH_EnableMainEventFilter(int devidx, int enable);
extern int MH_SetFilterTestMode(int devidx, int testmode);
extern int MH_GetRowFilteredRates(int devidx, int* syncrate, int* cntrates);
extern int MH_GetMainFilteredRates(int devidx, int* syncrate, int* cntrates);

//for debugging only
extern int MH_GetDebugInfo(int devidx, char *debuginfo);
extern int MH_GetNumOfModules(int devidx, int* nummod);
extern int MH_GetModuleInfo(int devidx, int modidx, int* modelcode, int* versioncode);
extern int MH_SaveDebugDump(int devidx, char* filepath); //new since v4.0

//for White Rabbit only
extern int MH_WRabbitGetMAC(int devidx, unsigned char* mac_addr);  //changed in v4.0
extern int MH_WRabbitSetMAC(int devidx, unsigned char* mac_addr);  //changed in v4.0
extern int MH_WRabbitGetInitScript(int devidx, char* initscript);
extern int MH_WRabbitSetInitScript(int devidx, char* initscript);
extern int MH_WRabbitGetSFPData(int devidx, char* sfpnames, int* dTxs, int* dRxs, int* alphas);
extern int MH_WRabbitSetSFPData(int devidx, char* sfpnames, int* dTxs, int* dRxs, int* alphas);
extern int MH_WRabbitInitLink(int devidx, int link_on);
extern int MH_WRabbitSetMode(int devidx, int bootfromscript, int reinit_with_mode, int mode);
extern int MH_WRabbitSetTime(int devidx, unsigned int timehidw, unsigned int timelodw);
extern int MH_WRabbitGetTime(int devidx, unsigned int* timehidw, unsigned int* timelodw, unsigned int* subsec16ns);
extern int MH_WRabbitGetStatus(int devidx, int* wrstatus);
extern int MH_WRabbitGetTermOutput(int devidx, char* buffer, int* nchar);

//for external FPGA interface only, all new since v3.0
extern int MH_ExtFPGAInitLink(int devidx, int linknumber, int on);
extern int MH_ExtFPGAGetLinkStatus(int devidx, int linknumber, unsigned int* status);
extern int MH_ExtFPGASetMode(int devidx, int mode, int loopback);
extern int MH_ExtFPGAResetStreamFifos(int devidx);
extern int MH_ExtFPGAUserCommand(int devidx, int write, unsigned int addr, unsigned int* data);
