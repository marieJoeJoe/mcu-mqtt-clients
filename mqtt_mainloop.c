#include "mqtt_app.h"
#include "os_config.h"

#if defined(OS_LINUX)
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


#if defined(SOCKET_LINUX)

int main(int argc, char *argv[])
{
    pthread_t thread_ID;		//linux thread id

    pthread_create(&thread_ID, NULL, &mqtt_task_thread, NULL);
    pthread_detach(thread_ID);

    char msg[20] = {'\0'};
    int msg_count = 0;
    while (1) {
        memset(msg,'\0',sizeof(msg));
        sprintf(msg,"[mqtt_msg] %d",msg_count++);
        mqtt_data_write(msg, strlen(msg), 0);
        sleep(3);
    }

    return 0;
}

#endif



#if defined(SOCKET_AT)

#include "MQTTAtSocket.h"

int main(int argc, char *argv[])
{

    AtDeviceInit();

    while (1) {
        //AtDeviceWrite(getAtUartFd(),"at\r\n",sizeof("at\r\n"));
        //sssssssAtDeviceRead(getAtUartFd());
        //printf("[mqtt socket at]\n");
        //sleep(1);
        //AtDeviceWrite(getAtUartFd(),"ati\r\n",sizeof("ati\r\n"));
        //AtDeviceRead(getAtUartFd());
        AtSocket("***,***.***.***","*****");
        sleep(1);
    }

    return 0;
}

#endif

#endif


#if defined(OS_RTOS) && defined(SOCKET_AT)
#include "task.h"
#endif