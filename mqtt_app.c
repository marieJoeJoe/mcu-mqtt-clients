#include <stdio.h>

#include "MQTTClient.h"
#include "mqtt_app.h"
#include "pthread.h"
#include "string.h"
#include "unistd.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "fcntl.h"


#define MQTT_TOPIC_SIZE     (128)		// topic length limit
#define MQTT_BUF_SIZE       (8 * 1024) 	//mqtt buffer size

#define MQTT_HOST "***.***.***.***"		//ip addr
#define MQTT_PORT 1883					//port
#define MQTT_USER "admin"				//user
#define MQTT_PASS "password"			//passwd
#define MQTT_CLIENT_ID "7474747474"		//client id

typedef struct {
    Network Network;
    Client Client;
    char sub_topic[MQTT_TOPIC_SIZE];		//sub topic
    char pub_topic[MQTT_TOPIC_SIZE];		//pub topic
    char mqtt_buffer[MQTT_BUF_SIZE];		//send buffer
    char mqtt_read_buffer[MQTT_BUF_SIZE];	//recv buffer

    unsigned char willFlag;					
    MQTTPacket_willOptions will;
    char will_topic[MQTT_TOPIC_SIZE];       //will storage

    pMessageArrived_Fun DataArrived_Cb;
}Cloud_MQTT_t;

typedef struct{
    enum iot_ctrl_status_t iotstatus;
    char model[5];
    char company[32];
} iot_device_info_t;


struct opts_struct {
    char    *clientid;
    int     nodelimiter;
    char    *delimiter;
    enum    QoS qos;
    char    *username;
    char    *password;
    char    *host;
    int     port;
    int     showtopics;
} opts = {
    (char *)"iot-dev", 0, (char *)"\n", QOS0, "admin", "password", (char *)"localhost", 1883, 0
};

Cloud_MQTT_t Iot_mqtt;

iot_device_info_t gateway = {
    .iotstatus = IOT_STATUS_LOGIN,
    .model = {"hello"},
    .company = {"/world"}
};


void iot_mqtt_init(Cloud_MQTT_t *piot_mqtt) 
{
    memset(piot_mqtt, '\0', sizeof(Cloud_MQTT_t));

    sprintf(piot_mqtt->sub_topic, "%s%s/todev", gateway.model, gateway.company);	//sey sub topic
    printf("subscribe:%s\n", piot_mqtt->sub_topic);

    sprintf(piot_mqtt->pub_topic, "%s%s/toapp", gateway.model, gateway.company);	//set pub topic
    printf("pub:%s\n", piot_mqtt->pub_topic);

    piot_mqtt->DataArrived_Cb = mqtt_data_rx_cb;		//set rx mqtt data callback

}
void MQTTMessageArrived_Cb(MessageData* md)
{
    MQTTMessage *message = md->message; 

    Cloud_MQTT_t *piot_mqtt = &Iot_mqtt;

    if (NULL != piot_mqtt->DataArrived_Cb) {
        piot_mqtt->DataArrived_Cb((void *)message->payload, message->payloadlen);
    }
}

int mqtt_device_connect(Cloud_MQTT_t *piot_mqtt)
{
    int rc = 0, ret = 0;

    NewNetwork(&piot_mqtt->Network);

    printf("topic = %s\n", piot_mqtt->sub_topic);

    rc = ConnectNetwork(&piot_mqtt->Network, MQTT_HOST, (int)MQTT_PORT);	
    if (rc != 0) {
        printf("mqtt connect network fail \n");
        ret = -101;
        goto __END;
    }
    MQTTClient(&piot_mqtt->Client, &piot_mqtt->Network, 1000, piot_mqtt->mqtt_buffer, MQTT_BUF_SIZE, piot_mqtt->mqtt_read_buffer, MQTT_BUF_SIZE);
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

    if (piot_mqtt->willFlag) {
        data.willFlag = 1;
        memcpy(&data.will, &piot_mqtt->will, sizeof(MQTTPacket_willOptions));
    } else {
        data.willFlag = 0;
    }
    data.MQTTVersion = 3;
    data.clientID.cstring = MQTT_CLIENT_ID;
    data.username.cstring = MQTT_USER;
    data.password.cstring = MQTT_PASS;
    data.keepAliveInterval = 30;
    data.cleansession = 1;
    
    rc = MQTTConnect(&piot_mqtt->Client, &data);
    if (rc) {
        printf("mqtt connect broker fail \n");
        printf("rc = %d\n", rc);
        ret = -102;
        goto __END;
    }
    rc = MQTTSubscribe(&piot_mqtt->Client, piot_mqtt->sub_topic, opts.qos, MQTTMessageArrived_Cb);
    if (rc) {
        printf("mqtt subscribe fail \n");
        ret = -105;
        goto __END;
    }
    gateway.iotstatus = IOT_STATUS_CONNECT;
    printf("Subscribed %d\n", rc);

__END:
    return ret;
}

int mqtt_device_disconnect(Cloud_MQTT_t *piot_mqtt)
{
    int ret = 0;

    ret = MQTTDisconnect(&piot_mqtt->Client);
    printf("disconnectNetwork ret = %d\n", ret);

    return ret;
}

void iot_yield(Cloud_MQTT_t *piot_mqtt)
{
    int ret;

    switch (gateway.iotstatus) {
        case IOT_STATUS_LOGIN:
        ret = mqtt_device_connect(piot_mqtt);
        if (ret < 0) {
            printf("iot connect error code %d\n", ret);
            sleep(1);
        }
        break;
        case IOT_STATUS_CONNECT:
        ret = MQTTYield(&piot_mqtt->Client, 100);
        if (ret != SUCCESS) {
            gateway.iotstatus = IOT_STATUS_DROP;
        }
        break;
        case IOT_STATUS_DROP:
        printf("IOT_STATUS_DROP\n");
        mqtt_device_disconnect(piot_mqtt);
        gateway.iotstatus = IOT_STATUS_LOGIN;
        usleep(1000);
        break;
        default:
        break;
    }
}

int mqtt_will_msg_set(Cloud_MQTT_t *piot_mqtt, char *pbuf, int len)//set will dara
{
    memset(piot_mqtt->will_topic, '\0', MQTT_TOPIC_SIZE);
    MQTTPacket_willOptions mqtt_will = MQTTPacket_willOptions_initializer;

    strcpy(piot_mqtt->will_topic, Iot_mqtt.pub_topic);
    memcpy(&Iot_mqtt.will, &mqtt_will, sizeof(MQTTPacket_willOptions));

    Iot_mqtt.willFlag = 1;
    Iot_mqtt.will.retained = 1;
    Iot_mqtt.will.topicName.cstring = (char *)piot_mqtt->will_topic;
    Iot_mqtt.will.message.cstring = (char *)pbuf;
    Iot_mqtt.will.qos = QOS2;

}

void mqtt_data_rx_cb(void *pbuf, int len) 
{

    printf("data = %s\n", (unsigned char *)pbuf);
}

int mqtt_data_write(char *pbuf, int len, char retain)
{
    Cloud_MQTT_t *piot_mqtt = &Iot_mqtt; 
    int ret = 0;
    MQTTMessage message;
    char my_topic[128] = {0};

    strcpy(my_topic, piot_mqtt->pub_topic);

    printf("publish topic is :%s\r\n", my_topic);
    printf("mqtt tx len = %d\r\n", len);

    message.payload = (void *)pbuf;
    message.payloadlen = len;
    message.dup = 0;
    message.qos = QOS2;
    if (retain) {
        message.retained = 1;
    } else {
        message.retained = 0;
    }

    ret = MQTTPublish(&piot_mqtt->Client, my_topic, &message);	//pub a topic

    return ret;
}

void *cloud_mqtt_thread(void *arg)
{
    int ret, len; 
    char will_msg[256] = {"hello world"};						//initializ will data
    
    iot_mqtt_init(&Iot_mqtt);									//initial topic
    mqtt_will_msg_set(&Iot_mqtt, will_msg, strlen(will_msg));

    ret = mqtt_device_connect(&Iot_mqtt);
    while (ret < 0) {
        printf("ret = %d\r\n", ret);
        sleep(3);
        ret = mqtt_device_connect(&Iot_mqtt);
    }

    while (1){
        iot_yield(&Iot_mqtt);
    }
}
