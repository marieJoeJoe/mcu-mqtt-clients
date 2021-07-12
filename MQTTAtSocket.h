#ifndef ATSOCKET_H
#define ATSOCKET_H

#include <string.h>
#include <stdint.h>

#define AT_DEVICE_FILE "/dev/ttyUSB3"


char *atCmdList[]={
    "at",
    "at+qiopen=",
    "at+qiclose=",
    "at+qisend=",
    "at+qird="
};

char *atEnter = "\r\n";

char *atAckList[]={
    "ok",
    "error",
};





void AtDeviceInit(void);

void AtDeviceSendCommand(void* device, uint8_t *pData, uint16_t Size);

int SendAtCommand(char *command, char *reply, uint16_t delay);

int AtConnectHost(char*ip,char*port);



#endif