#ifndef MQTT_APP_H
#define MQTT_APP_H

#ifdef __cplusplus
extern "C" {
#endif

#define TOPICX "/toptopic/hxc/mqttbroker"

#define TPOICA "/Attr2NetManager"
#define TPOICB "/QueryInst2Term"
#define TPOICC "/QueryReply2NetManager"


typedef struct term_online_report{
	char cpe_id[128];
	char imsi[128];
	char vendor_name[50];
	char cpe_ip[4];
	char modem_version[50];
	char product_type[20];
	char cell_id[10];
}__attribute__((packed)) term_online_report_t;

typedef struct term_cyclic_report{
	char cpe_id[64];
	char locationinfo[64];
	int online_time;
	int wanrpkts;
	int wanspkts;
	int rsrp;
	int rssi;
	int sinr;
	int rsrq;
}__attribute__((packed)) term_cyclic_report_t;


typedef struct term_info_replay
{
	/* data */
}__attribute__((packed)) term_info_replay_t;




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

#define mDEBUG(fmt, ...)  printf("%s[%s](%d):" fmt,__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
