#ifndef MQTT_APP_H
#define MQTT_APP_H

#ifdef __cplusplus
extern "C" {
#endif

#define REPORT_MSG_LEN 512

#define TOPICX "/toptopic/hxc/mqttbroker"

#define TPOICA "/Attr2NetManager"
#define TPOICB "/QueryInst2Term"
#define TPOICC "/QueryReply2NetManager"

enum  iot_ctrl_status_t
{
	IOT_STATUS_LOGIN,
	IOT_STATUS_CONNECT,
	IOT_STATUS_DROP,
};

typedef void  (*pMessageArrived_Fun)(void*,int len);

void mqtt_data_rx_cb(void *pbuf, int len);

void* mqtt_cyclic_report_task_thread(void *arg);

void* mqtt_query_reply_task_thread(void* arg);


#ifdef __cplusplus

}
#endif

#endif
