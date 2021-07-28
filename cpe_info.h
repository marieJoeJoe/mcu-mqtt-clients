#ifndef CPE_INFO_H
#define CPE_INFO_H

#include <sys/time.h>
#include <string.h>
#include <stdio.h>


enum CPE_INFO_RET{
  CPE_ERROR = -1,
  CPE_FAILED = 0,
  CPE_SUCCESS = 1,
};

#define CPE_ID_LEN			128
#define IMSI_LEN			128
#define VENDOR_NAME_LEN		50
#define CPE_IP_LEN			30
#define SW_VERSION_LEN      50
#define MODEM_VERSION_LEN	50
#define PRODUCT_TYPE_LEN	20
#define CELL_ID_LEN			20


#define CPE_LOCATION_INFO_LEN			64


typedef struct static_cpe_info
{
	int is_fillfulled;
	char cpe_id[CPE_ID_LEN];
    char imsi[IMSI_LEN];
	char vendor_name[VENDOR_NAME_LEN];
	char sw_version[SW_VERSION_LEN];
	char modem_version[MODEM_VERSION_LEN];
	char product_type[PRODUCT_TYPE_LEN];

    //time_t start_online_time;
    struct timeval start_online_time;
}__attribute__((packed)) cpe_static_info_t;


typedef struct term_online_report{
	char cpe_id[CPE_ID_LEN];
	char imsi[IMSI_LEN];
	char vendor_name[VENDOR_NAME_LEN];
	char cpe_ip[CPE_IP_LEN];
	char sw_version[SW_VERSION_LEN];
	char modem_version[MODEM_VERSION_LEN];
	char product_type[PRODUCT_TYPE_LEN];
	char cell_id[CELL_ID_LEN];
}__attribute__((packed)) term_online_report_t;


typedef struct term_cyclic_report{
	char cpe_id[CPE_ID_LEN];
	char locationinfo[CPE_LOCATION_INFO_LEN];
	int online_time;
	int wanrpkts;
	int wanspkts;
	unsigned int rsrp_rssi;
	//int rssi;
	unsigned int sinr_rsrq;
	//int rsrq;
}__attribute__((packed)) term_cyclic_report_t;


typedef struct term_info_replay
{
	/* data */
}__attribute__((packed)) term_info_replay_t;

int get_online_status( term_online_report_t* online_status);

void test_get_online_status(void);

int get_cyclic_status(term_cyclic_report_t* cyclic_status);

void test_get_cyclic_ststus(void);

void init_cpe_hw_info(void);

int create_cyclic_report_msg(char * msg);

int create_online_report_msg(char * msg);

#endif