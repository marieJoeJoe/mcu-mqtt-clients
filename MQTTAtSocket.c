#include "MQTTAtSocket.h"

uint8_t rx_data = 0;
uint8_t rx_buffer[1460] = {0};
uint16_t rx_index = 0;

uint8_t mqtt_receive = 0;
char mqtt_buffer[1460] = {0};
uint16_t mqtt_index = 0;


void clearRxBuffer(void) {
    rx_index = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));
}

void AtDeviceInit(void){



}

int DeviceSendCommand(void* device, uint8_t *pData, uint16_t Size){


}

int SendAtCommand(char *command, char *reply, uint16_t delay){

    int status = -1;

    status = DeviceSendCommand(NULL,(unsigned char *)command,(uint16_t) strlen(command));

#if OS_RTOS
    osDelay(delay);
#endif

#if OS_LINUX
    Delay(delay);
#endif

    if (strstr(mqtt_buffer, reply) != NULL) {
        clearRxBuffer();
        status = -1;
        return status;
    }
    clearRxBuffer();
    status = 1;
    return status; 
}
