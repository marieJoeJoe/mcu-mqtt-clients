#ifndef MQTT_APP_H
#define MQTT_APP_H

#ifdef __cplusplus
extern "C" {
#endif

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

void mqtt_module_init(void);

int mqtt_data_write(char *pbuf, int len, char retain);

void mqtt_data_rx_cb(void *pbuf, int len);

void *mqtt_task_thread(void *arg);

void* mqtt_cyclic_report_task_thread(void *arg);

void* mqtt_inst_reply_task_thread(void* arg);

#define mDEBUG(fmt, ...)  printf("%s[%s](%d):" fmt,__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)

#ifdef __cplusplus

}
#endif

#endif
