#include "cpe_info.h"
#include <sys/time.h>

static cpe_static_info_t cpe_hw_info;


static int get_cpe_hw_info(void){

  memset(&cpe_hw_info,'\0',sizeof(cpe_static_info_t));

  cpe_hw_info.is_fillfulled = 1;

  char* cpe_id = "283712";
  char* cpe_imsi = "123";
  char* cpe_vendor_name = "quectel";
  char* s_version = "83920";
  char* m_version = "57482";
  char* p_type = "39";
  //strncpy(, cpe_hw_info.cpe_id, strlen(cpe_id));

  struct timeval utc_online;

  strncpy(cpe_hw_info.cpe_id, cpe_id, strlen(cpe_id));
  strncpy(cpe_hw_info.imsi, cpe_imsi, strlen(cpe_imsi));
  strncpy(cpe_hw_info.vendor_name, cpe_vendor_name, strlen(cpe_vendor_name));
  strncpy(cpe_hw_info.sw_version, s_version, strlen(s_version));
  strncpy(cpe_hw_info.modem_version, m_version, strlen(m_version));
  strncpy(cpe_hw_info.product_type, p_type, strlen(p_type));
  //gettimeofday( &cpe_hw_info.start_online_time, NULL );
  gettimeofday( &utc_online, NULL );
  cpe_hw_info.start_online_time.tv_sec  =utc_online.tv_sec;
  cpe_hw_info.start_online_time.tv_usec =utc_online.tv_usec;
  //printf("%ld\n",cpe_hw_info.start_online_time.tv_sec);

  return CPE_SUCCESS;

}

/*
static int get_cpe_id( char* id ){

  char* cpe_id = "cpe_id:283712";

  strncpy(id, cpe_id, strlen(cpe_id));

  return SUCCESS;
}
*/

static unsigned int get_cpe_ip( void ){

  //char* cpe_ip = "cpe:ip:10.10.113.189";

  //strncpy(ip, cpe_ip, strlen(cpe_ip));

  return 0xa0a71bd;
}

static int get_sw_version( void ){

  //char* version = "cpe:sw_ver:83920";

  return 83;
}

static int get_modem_version( void ){

  //char* m_version = "cpe:mod_ver:57482";

  //strncpy(modem_version, m_version, strlen(m_version));

  return 57482;
}

static int get_product_type( void ){

  return 39;
}

static int get_cell_id( void ){

  return 182;
}


static unsigned int get_location_info( void ){

  //char* loc_info = "quectel_cpe_id:283712";

  return 0xffffffff;
}

static long get_online_time(void){

  struct timeval cur_time;

  gettimeofday( &cur_time, NULL );
  //printf("%ld,%ld,%ld\n",cpe_hw_info.start_online_time.tv_sec,cur_time.tv_sec,cur_time.tv_usec);
  return cur_time.tv_sec - cpe_hw_info.start_online_time.tv_sec;
}

static int get_wan_recv_pkts_cnt( void ){

  return 128000;
}

static int get_wan_send_pkts_cnt( void ){

  return 128001;
}

static int get_cpe_rsrp( void ){

  return 13;

}

static int get_cpe_rssi( void ){

  return 22;

}

static int get_cpe_sinr( void ){

  return -77;

}
static int get_cpe_rsrq( void ){

  

  return -9;

}

void init_cpe_hw_info(void){
  get_cpe_hw_info();
}


#define GET_MASK_LOACTION(LOCATION) (LOCATION>>24),(0xff&(LOCATION>>16)),(0xff&(LOCATION>>8)),(0xff&LOCATION)

int create_cyclic_report_msg(char * msg){

  if(NULL == msg) return -1;
  //printf("debug\n");
  /*            1   2             3                      4              5             6             7             8             9             10         11*/
  sprintf(msg,"%s\"%s\":\"%s\",\"%s\":\"%d-%d-%d-%d\",\"%s\":\"%ld\",\"%s\":\"%u\",\"%s\":\"%u\",\"%s\":\"%d\",\"%s\":\"%d\",\"%s\":\"%d\",\"%s\":\"%d\"%s",\
/* 1 */       "{",\
/* 2 */       "cpeid",cpe_hw_info.cpe_id,\
/* 3 */       "locationinfo",GET_MASK_LOACTION(get_location_info()),/*0,0,0,0,\*/
/* 4 */       "onlinetime",get_online_time(),\
/* 5 */       "WanRPkts",get_wan_recv_pkts_cnt(),\
/* 6 */       "WanSPkts",get_wan_send_pkts_cnt(),\
/* 7 */       "RSRP",get_cpe_rsrp(),\
/* 8 */       "RSSI",get_cpe_rssi(),\
/* 9 */       "SINR",get_cpe_sinr(),\
/* 10 */      "RSRQ",get_cpe_rsrq(),\
/* 11 */       "}");

  return CPE_SUCCESS;

}

#define CONVE_IP_HEX(LOCATION) (LOCATION>>24),(0xff&(LOCATION>>16)),(0xff&(LOCATION>>8)),(0xff&LOCATION)

int create_online_report_msg(char * msg){

  if(NULL == msg) return -1;
  //printf("debug\n");
  /*            1   2             3             4             5                      6              7             8              9          10*/
  sprintf(msg,"%s\"%s\":\"%s\",\"%s\":\"%ld\",\"%s\":\"%s\",\"%s\":\"%d.%d.%d.%d\",\"%s\":\"%d\",\"%s\":\"%d\",\"%s\":\"%d\",\"%s\":\"%d\"%s",\
/* 1  */       "{",\
/* 2  */       "CPEID",cpe_hw_info.cpe_id,\
/* 3  */       "IMSI",46225062244429,\
/* 4  */       "VendorName",cpe_hw_info.vendor_name,\
/* 5  */       "CPEIP",CONVE_IP_HEX(get_cpe_ip()),/*10,10,111,24,\*/
/* 6  */       "SWVersion",get_sw_version(),\
/* 7  */       "ModemVersion",get_modem_version(),\
/* 8  */       "ProductType",get_product_type(),\
/* 9  */       "CellID",get_cell_id(),\
/* 10 */       "}");

  return CPE_SUCCESS;
}


