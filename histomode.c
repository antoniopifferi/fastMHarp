/************************************************************************

  Demo access to MultiHarp 150/160 hardware via MHLIB v 4.0
  The program performs a measurement based on hardcoded settings.
  The resulting histogram is stored in an ASCII output file.

  Michael Wahl, PicoQuant GmbH, January 2025

  Note: This is a console application

  Note: At the API level channel numbers are indexed 0..N-1 
    where N is the number of channels the device has.

  Tested with the following compilers:

  - MinGW-W64 4.3.5 (Windows 64 bit)
  - MS Visual C++ 2019 (Windows 64 bit)
  - gcc 9.4.0 and 11.4.0 (Linux 64 bit)

************************************************************************/

#ifndef _WIN32
#include <unistd.h>
#define Sleep(msec) usleep(msec*1000)
#else
#include <windows.h>
#include <dos.h>
#include <conio.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mhdefin.h"
#include "mhlib.h"
#include "errorcodes.h"


unsigned int counts[MAXINPCHAN][MAXHISTLEN];

// helper macro and associated function for API calls with error check
// the stringize operator # makes a printable string from the macro's input argument
#define APICALL(call) doapicall(call, #call, __LINE__)
int doapicall(int retcode, char* callstr, int line)
{
    if (retcode < 0)
    {
        char errorstring[100] = { 0 };
        MH_GetErrorString(errorstring, retcode);
        printf("\nThe API call %s at line %d\nreturned error %d (%s)\n", callstr, line, retcode, errorstring);
    }
    return retcode;
}


int main(int argc, char* argv[])
{
    int dev[MAXDEVNUM];
    int found = 0;
    FILE* fpout = NULL;
    int retcode;
    int ctcstatus;
    char LIB_Version[8];
    char HW_Model[32];
    char HW_Partno[8];
    char HW_Version[16];
    char HW_Serial[32];
    char Errorstring[40];
    char debuginfobuffer[16384]; // must have 16384 bytes of text buffer
    int NumChannels;
    int HistLen;
    int Binning = 0; // you can change this
    int Offset = 0;
    int Tacq = 1000; // measurement time in millisec, you can change this
    int SyncDivider = 1; // you can change this

    int SyncTiggerEdge = 0;      // 0 or 1, you can change this
    int SyncTriggerLevel = -50;  // in mV,  you can change this
    int InputTriggerEdge = 0;    // 0 or 1, you can change this
    int InputTriggerLevel = -50; // in mV,  you can change this

    double Resolution;
    int Syncrate;
    int Countrate;
    double Integralcount;
    int i, j;
    int flags;
    int warnings;
    char warningstext[16384]; // must have 16384 bytes of text buffer
    char cmd = 0;

    memset(Errorstring, 0x00, sizeof(Errorstring));
    memset(warningstext, 0x00, sizeof(warningstext));

    printf("\nMultiHarp MHLib Demo Application                   PicoQuant GmbH, 2025");
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

    if (APICALL(MH_GetLibraryVersion(LIB_Version)) < 0)
        goto ex;
    printf("\nLibrary version is %s", LIB_Version);
    if (strncmp(LIB_Version, LIB_VERSION, sizeof(LIB_VERSION)) != 0)
        printf("\nWarning: The application was built for version %s.", LIB_VERSION);

    if ((fpout = fopen("histomodeout.txt", "w")) == NULL)
    {
        printf("\ncannot open output file\n");
        goto ex;
    }

    printf("\nSearching for MultiHarp devices...");
    printf("\nDevidx     Serial     Status");

    for (i = 0; i < MAXDEVNUM; i++)
    {
        memset(HW_Serial, 0, sizeof(HW_Serial));
        retcode = MH_OpenDevice(i, HW_Serial); // not using APICALL here as fails must be expected
        if (retcode == 0) // grab any device we can open
        {
            printf("\n  %1d        %7s    open ok", i, HW_Serial);
            dev[found] = i; // keep index to device(s) we want to use
            found++;
        }
        else
        {
            if (retcode == MH_ERROR_DEVICE_OPEN_FAIL)
                printf("\n  %1d        %7s    no device", i, HW_Serial);
            else
            {
                MH_GetErrorString(Errorstring, retcode);
                printf("\n  %1d        %7s    %s", i, HW_Serial, Errorstring);
            }
        }
    }

    // In this demo we will use the first device we find, i.e. dev[0].
    // You could also use multiple devices in parallel.
    // You can also check for specific serial numbers, so that you know 
    // which physical device you are talking to.

    if (found < 1)
    {
        printf("\nNo device available.");
        goto ex;
    }

    printf("\nUsing device #%1d", dev[0]);
    printf("\nInitializing the device...");
    fflush(stdout);

    if (APICALL(MH_Initialize(dev[0], MODE_HIST, 0)) < 0) // Histo mode with internal clock
    {
        // in case of an obscure error (a hardware error in particular) 
        // it may be helpful to obtain debug information like so:
        MH_GetDebugInfo(dev[0], debuginfobuffer);
        printf("\nDEBUGINFO:\n%s", debuginfobuffer);
        goto ex;
    }

    if (APICALL(MH_GetHardwareInfo(dev[0], HW_Model, HW_Partno, HW_Version)) < 0)
        goto ex;
    else
        printf("\nFound Model %s Part no %s Version %s", HW_Model, HW_Partno, HW_Version);

    if (APICALL(MH_GetNumOfInputChannels(dev[0], &NumChannels)) < 0)
        goto ex;
    else
        printf("\nDevice has %i input channels.", NumChannels);

    if (APICALL(MH_SetSyncDiv(dev[0], SyncDivider)) < 0)
        goto ex;

    if (APICALL(MH_SetSyncEdgeTrg(dev[0], SyncTriggerLevel, SyncTiggerEdge)) < 0)
        goto ex;

    if (APICALL(MH_SetSyncChannelOffset(dev[0], 0)) < 0) // can emulate a cable delay
        goto ex;

    for (i = 0; i < NumChannels; i++) // we use the same input settings for all channels
    {
        if (APICALL(MH_SetInputEdgeTrg(dev[0], i, InputTriggerLevel, InputTriggerEdge)) < 0)
            goto ex;

        if (APICALL(MH_SetInputChannelOffset(dev[0], i, 0)) < 0) // can emulate a cable delay
            goto ex;

        if (APICALL(MH_SetInputChannelEnable(dev[0], i, 1)) < 0)
            goto ex;
    }

    if (APICALL(MH_SetHistoLen(dev[0], MAXLENCODE, &HistLen)) < 0)
        goto ex;  
    printf("\nHistogram length is %d", HistLen);

    if (APICALL(MH_SetBinning(dev[0], Binning)) < 0)
        goto ex;

    if (APICALL(MH_SetOffset(dev[0], Offset)) < 0)
        goto ex;

    if (APICALL(MH_GetResolution(dev[0], &Resolution)) < 0)
        goto ex;
    printf("\nResolution is %1.0lfps\n", Resolution);

    // after Init allow 150 ms for valid  count rate readings
    // subsequently you get new values after every 100ms
    Sleep(150);

    if (APICALL(MH_GetSyncRate(dev[0], &Syncrate)) < 0)
        goto ex;
    printf("\nSyncrate=%1d/s", Syncrate);

    for (i = 0; i < NumChannels; i++) // for all channels
    {
        if (APICALL(MH_GetCountRate(dev[0], i, &Countrate)) < 0)
            goto ex;
        printf("\nCountrate[%1d]=%1d/s", i, Countrate);
    }

    printf("\n");

    // after getting the count rates you can check for warnings
    if (APICALL(MH_GetWarnings(dev[0], &warnings)) < 0)
        goto ex;

    if (warnings)
    {
        if (APICALL(MH_GetWarningsText(dev[0], warningstext, warnings)) < 0)
            goto ex;
        printf("\n\n%s", warningstext);
    }

    if (APICALL(MH_SetStopOverflow(dev[0], 0, 10000)) < 0) // for example only
        goto ex;

    while (cmd != 'q')
    {
        if (APICALL(MH_ClearHistMem(dev[0])) < 0)
            goto ex;

        printf("\npress RETURN to start measurement");
        getchar();

        if (APICALL(MH_GetSyncRate(dev[0], &Syncrate)) < 0)
            goto ex;
        printf("\nSyncrate=%1d/s", Syncrate);

        for (i = 0; i < NumChannels; i++) // for all channels
        {
            if (APICALL(MH_GetCountRate(dev[0], i, &Countrate)) < 0)
                goto ex;
            printf("\nCountrate[%1d]=%1d/s", i, Countrate);
        }

        // here you could check for warnings again

        if (APICALL(MH_StartMeas(dev[0], Tacq)) < 0)
            goto ex;

        printf("\n\nMeasuring for %1d milliseconds...", Tacq);

        ctcstatus = 0;
        while (ctcstatus == 0)
        {
            if (APICALL(MH_CTCStatus(dev[0], &ctcstatus)) < 0)
                goto ex;
        }

        if (APICALL(MH_StopMeas(dev[0])) < 0)
            goto ex;

        printf("\n");
        for (i = 0; i < NumChannels; i++) // for all channels
        {
            if (APICALL(MH_GetHistogram(dev[0], counts[i], i)) < 0)
                goto ex;

            Integralcount = 0;
            for (j = 0; j < HistLen; j++)
                Integralcount += counts[i][j];

            printf("\n  Integralcount[%1d]=%1.0lf", i, Integralcount);
        }
        printf("\n");

        if (APICALL(MH_GetFlags(dev[0], &flags)) < 0)
            goto ex;

        if (flags & FLAG_OVERFLOW)
            printf("\n  Overflow.");

        printf("\nEnter c to continue or q to quit and save the count data.");
        cmd = getchar();
        getchar();
    }

    for (i = 0; i < NumChannels; i++)
        fprintf(fpout, "  ch%02d ", i+1);
    fprintf(fpout, "\n");
    for (j = 0; j < HistLen; j++)
    {
        for (i = 0; i < NumChannels; i++)
            fprintf(fpout, "%6d ", counts[i][j]);
        fprintf(fpout, "\n");
    }

ex:
    for (i = 0; i < MAXDEVNUM; i++) // no harm to close all
        MH_CloseDevice(i);
    
    if (fpout)
        fclose(fpout);

    printf("\npress RETURN to exit");
    getchar();

    return 0;
}


