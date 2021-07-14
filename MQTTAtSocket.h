#ifndef ATSOCKET_H
#define ATSOCKET_H

#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "mqtt_os_adapter.h"

#define AT_DEVICE_FILE "/dev/ttyUSB3"

/*
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
*/


int getAtUartFd(void);

void AtDeviceInit(void);
void AtDeviceSendCommand(void* device, uint8_t *pData, uint16_t Size);
int SendAtCommand(char *command, char *reply, uint16_t delay);
int AtConnectHost(char*ip,char*port);


//hardware uart rw basic api
int AtDeviceRead(int device_fd);
int AtDeviceWrite(int device_fd, unsigned char*cmd,int cmd_len);

int AtSocket(char* ipaddr,char *port);


#endif