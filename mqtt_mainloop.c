#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "mqtt_app.h"


int main(int argc, char *argv[])
{
    pthread_t thread_ID;		//linux thread id

    pthread_create(&thread_ID, NULL, &mqtt_task_thread, NULL);
    pthread_detach(thread_ID);

    while (1) {
        mqtt_data_write("hello world", 11, 0);
        sleep(3);
    }

    return 0;
}
