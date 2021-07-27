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

#define MQTT_HOST "47.106.204.139"		//ip addr
#define MQTT_PORT 1883					//port

#define MQTT_USER "helloMQTT"				//user
#define MQTT_PASS "ackmqtt"			//passwd
#define MQTT_CLIENT_ID_PC_LINUX    "mcu-mqtt-client-pc-linux"		//client id
#define MQTT_CLIENT_ID_PC_LINUX_AT "mcu-mqtt-client-pc-linux-at"		//client id
#define MQTT_CLIENT_ID_MCU_RTOS_AT "mcu-mqtt-client-mcu-rtos-at"		//client id


#define TERM_MQTT_USER "terminal"			//user
#define TERM_MQTT_PASS "termackmqtt"			//passwd
#define TERM_MQTT_CLIENT_ID_PC_LINUX_CYC    "term-mqtt-client-linux-cyclic"		//client id
#define TERM_MQTT_CLIENT_ID_PC_LINUX_INST    "term-mqtt-client-linux-inst-keeper"		//client id

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
    (char *)MQTT_CLIENT_ID_PC_LINUX, 0, (char *)"\n", QOS2, MQTT_USER, MQTT_PASS, (char *)MQTT_HOST, MQTT_PORT, 0
};


Cloud_MQTT_t Iot_mqtt;

iot_device_info_t gateway = {
    .iotstatus = IOT_STATUS_LOGIN,
    .model = {"hello"},
    .company = {"/world"}
};

Cloud_MQTT_t cyclic_mqtt;

Cloud_MQTT_t ints_reply_mqtt;

void mqtt_inst_msg_arrived_cb(MessageData* md)
{
    MQTTMessage *message = md->message; 

    Cloud_MQTT_t *piot_mqtt = &ints_reply_mqtt;

    if (NULL != piot_mqtt->DataArrived_Cb) {
        piot_mqtt->DataArrived_Cb((void *)message->payload, message->payloadlen);
    }
}

int term_mqtt_device_connect(Cloud_MQTT_t *piot_mqtt,char* ClinetID, pMessageArrived_Fun fun)
{
    int rc = 0, ret = 0;

    NewNetwork(&piot_mqtt->Network);

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
    data.clientID.cstring = ClinetID;
    data.username.cstring = TERM_MQTT_USER;
    data.password.cstring = TERM_MQTT_PASS;
    data.keepAliveInterval = 30;
    data.cleansession = 1;

    printf("Client ID: %s\n", ClinetID);
    printf("pub topic: %s\n", piot_mqtt->pub_topic);
    printf("sub topic: %s\n", piot_mqtt->sub_topic);


    rc = MQTTConnect(&piot_mqtt->Client, &data);
    if (rc) {
        printf("mqtt connect broker fail \n");
        printf("rc = %d\n", rc);
        ret = -102;
        goto __END;
    }

    if(fun){

      rc = MQTTSubscribe(&piot_mqtt->Client, piot_mqtt->sub_topic, QOS2, fun);

      if (rc) {
        printf("mqtt subscribe fail \n");
        ret = -105;
        goto __END;
      }
      printf("Subscribed %d\n", rc);
    }

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

int mqtt_cyclic_pub(Cloud_MQTT_t *piot_mqtt ,char *pbuf, int len, char retain)
{ 
    int ret = 0;
    MQTTMessage message;
    char pub_topic[128] = {0};

    strcpy(pub_topic, piot_mqtt->pub_topic);

    printf("publish topic :%s len :%d\r\n", pub_topic, len);

    message.payload = (void *)pbuf;
    message.payloadlen = len;
    message.dup = 0;
    message.qos = QOS2;
    if (retain) {
        message.retained = 1;
    } else {
        message.retained = 0;
    }

    ret = MQTTPublish(&piot_mqtt->Client, pub_topic, &message);	//pub a topic

    return ret;
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
    int is_inst = -1;
    //printf("data = %s\n", (unsigned char *)pbuf);
    is_inst = ('q' == *(char*)pbuf)?1:0;
    printf("data = %s\n status %d\n", ( char *)pbuf,is_inst);
    if(1 == is_inst){
      int ret = -1;
      char* json_inst_reply_msg = malloc(512);
      memset(json_inst_reply_msg,'\0',sizeof(json_inst_reply_msg));
      ret = create_cyclic_report_msg(json_inst_reply_msg);
      //printf("%s\n",json_inst_reply_msg);
      //printf("valid %d\n",JSON_Validate( json_inst_reply_msg, strlen(json_inst_reply_msg) ));
      mqtt_cyclic_pub(&ints_reply_mqtt,json_inst_reply_msg,strlen(json_inst_reply_msg),0);
      free(json_inst_reply_msg);
    }else{
      mqtt_cyclic_pub(&ints_reply_mqtt,(char*)"{inst:error}",strlen("{inst:error}"),0);
    }
}

void * mqtt_cyclic_report_task_thread(void *arg)
{

    int ret, len; 
    char will_msg[256] = {"[mqtt_cyclic::say goodbye!]"};//initializ will data

    //iot_mqtt_init(&Iot_mqtt);//initial topic

    memset(&cyclic_mqtt, '\0', sizeof(Cloud_MQTT_t));

    sprintf(cyclic_mqtt.pub_topic, "%s%s", TOPICX, TPOICA);	//set pub topic
    //printf("pub:%s\n", piot_mqtt->pub_topic);

    mqtt_will_msg_set(&cyclic_mqtt, will_msg, strlen(will_msg));

    ret = term_mqtt_device_connect(&cyclic_mqtt, TERM_MQTT_CLIENT_ID_PC_LINUX_CYC, NULL);

    while (ret < 0) {
        printf("ret = %d\r\n", ret);
        sleep(3);
        ret = term_mqtt_device_connect(&cyclic_mqtt, TERM_MQTT_CLIENT_ID_PC_LINUX_CYC, NULL);
    }

    char* json_online_msg = malloc(512);

    memset(json_online_msg,'\0',sizeof(json_online_msg));

    ret = create_online_report_msg(json_online_msg);

    //printf("%s\n",json_online_msg);

    //printf("valid %d\n",JSON_Validate( json_online_msg, strlen(json_online_msg) ));

    mqtt_cyclic_pub(&cyclic_mqtt,json_online_msg,strlen(json_online_msg),0);

    free(json_online_msg);

    while (1){

        char* json_cyc_msg = malloc(512);

        memset(json_cyc_msg,'\0',sizeof(json_cyc_msg));

        ret = create_cyclic_report_msg(json_cyc_msg);

        //printf("valid %d\n",JSON_Validate( json_cyc_msg, strlen(json_cyc_msg) ));

        mqtt_cyclic_pub(&cyclic_mqtt,json_cyc_msg,strlen(json_cyc_msg),0);

        //free(json_cyc_msg);

        MQTTYield(&(cyclic_mqtt.Client),30000);
    }

}

void * mqtt_inst_reply_task_thread(void* arg)
{
    int ret, len; 
    char will_msg[256] = {"[mqtt_inst_reply::say goodbye!]"};//initializ will data

    memset(&ints_reply_mqtt, '\0', sizeof(Cloud_MQTT_t));

    sprintf(ints_reply_mqtt.sub_topic, "%s%s", TOPICX, TPOICB);	//set Sub topic
    sprintf(ints_reply_mqtt.pub_topic, "%s%s", TOPICX, TPOICC);	//set Pub topic
  
    ints_reply_mqtt.DataArrived_Cb = mqtt_data_rx_cb;
  
    mqtt_will_msg_set(&ints_reply_mqtt, will_msg, strlen(will_msg));

    ret = term_mqtt_device_connect(&ints_reply_mqtt,TERM_MQTT_CLIENT_ID_PC_LINUX_INST,mqtt_inst_msg_arrived_cb);
  
    while (ret < 0) {
        printf("ret = %d\r\n", ret);
        sleep(3);
        ret = term_mqtt_device_connect(&ints_reply_mqtt,TERM_MQTT_CLIENT_ID_PC_LINUX_INST,mqtt_inst_msg_arrived_cb);
    }

    while (1){
        MQTTYield(&(ints_reply_mqtt.Client),10000);
    }
}