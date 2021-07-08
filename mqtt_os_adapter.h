#ifndef __MQTT_OS_ADAPTER_
#define __MQTT_OS_ADAPTER_

#include "os_config.h"

#ifdef OS_LINUX

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>
#include <signal.h>

typedef struct Timer Timer;

struct Timer {
	struct timeval end_time;
};

typedef struct Network Network;

struct Network
{

#if IOT_SSL_ENABLE
	SSL_CTX *ctx;
	SSL *ssl;
#endif
	int my_socket;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
	void (*disconnect) (Network*);
};

char expired(Timer*);
void countdown_ms(Timer*, unsigned int);
void countdown(Timer*, unsigned int);
int left_ms(Timer*);

void InitTimer(Timer*);

int linux_read(Network*, unsigned char*, int, int);
int linux_write(Network*, unsigned char*, int, int);
void linux_disconnect(Network*);
void NewNetwork(Network*);

int ConnectNetwork(Network*, char*, int);

#endif


#ifdef OS_RTOS

typedef struct Timer Timer;

struct Timer {
	struct timeval end_time;
};

typedef struct Network Network;

struct Network
{

#if IOT_SSL_ENABLE
	SSL_CTX *ctx;
	SSL *ssl;
#endif
	int my_socket;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
	void (*disconnect) (Network*);
};

char expired(Timer*);
void countdown_ms(Timer*, unsigned int);
void countdown(Timer*, unsigned int);
int left_ms(Timer*);

void InitTimer(Timer*);

int linux_read(Network*, unsigned char*, int, int);
int linux_write(Network*, unsigned char*, int, int);
void linux_disconnect(Network*);
void NewNetwork(Network*);

int ConnectNetwork(Network*, char*, int);


#endif

#endif