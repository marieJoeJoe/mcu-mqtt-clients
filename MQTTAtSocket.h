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

typedef struct retstr
{
    char** ret_str_list;
    int ret_str_num;
}at_ret_strings_t;






int get_at_uart_fd(void);


int at_socket_read(Network* n, unsigned char* buffer, int len, int timeout_ms);
int at_socket_write(Network* n, unsigned char* buffer, int len, int timeout_ms);
void at_socket_disconnect(Network* n);


//hardware uart rw basic api
void atdevice_init(void);
int atdevice_write(int device_fd, unsigned char*cmd,int cmd_len);
int atdevice_read(int device_fd,char *retstr);

int at_socket_init(char* ipaddr,char *port);
int at_retstr_split(char* pkt,at_ret_strings_t* ret);
void at_retstr_free(at_ret_strings_t* ret);


#endif