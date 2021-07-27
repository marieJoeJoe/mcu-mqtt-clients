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

  //struct timeval utc_online;

  strncpy(cpe_hw_info.cpe_id, cpe_id, strlen(cpe_id));
  strncpy(cpe_hw_info.imsi, cpe_imsi, strlen(cpe_imsi));
  strncpy(cpe_hw_info.vendor_name, cpe_vendor_name, strlen(cpe_vendor_name));
  strncpy(cpe_hw_info.sw_version, s_version, strlen(s_version));
  strncpy(cpe_hw_info.modem_version, m_version, strlen(m_version));
  strncpy(cpe_hw_info.product_type, p_type, strlen(p_type));
  gettimeofday( &cpe_hw_info.start_online_time, NULL );
  //printf("%ld\n",cpe_hw_info.start_online_time.tv_sec);


}


static int get_cpe_id( char* id ){

  char* cpe_id = "cpe_id:283712";

  strncpy(id, cpe_id, strlen(cpe_id));

  return SUCCESS;
}

static int get_imsi( char* imsi ){

  char* cpe_imsi = "cpe:imsi";

  strncpy(imsi, cpe_imsi, strlen(cpe_imsi));

  return SUCCESS;
}

static int get_vendor_name( char* vendor_name ){

  char* cpe_vendor_name = "vendor_name:quectel";

  strncpy(vendor_name, cpe_vendor_name, strlen(cpe_vendor_name));

  return SUCCESS;
}

static int get_cpe_ip( char* ip ){

  char* cpe_ip = "cpe:ip:10.10.113.189";

  strncpy(ip, cpe_ip, strlen(cpe_ip));

  return SUCCESS;
}

static int get_sw_version( char* sw_version ){

  char* version = "cpe:sw_ver:83920";

  strncpy(sw_version, version, strlen(version));

  return SUCCESS;
}

static int get_modem_version( char* modem_version ){

  char* m_version = "cpe:mod_ver:57482";

  strncpy(modem_version, m_version, strlen(m_version));

  return SUCCESS;
}

static int get_product_type( char* product_type ){

  char* type = "product_type:39";

  strncpy(product_type, type, strlen(type));

  return SUCCESS;
}

static int get_cell_id( char* cell_id ){

  char* cell = "cell_id:182";

  strncpy(cell_id, cell, strlen(cell));

  return SUCCESS;
}


static int get_location_info( char* location_info ){

  char* loc_info = "quectel_cpe_id:283712";

  strncpy(location_info, loc_info, strlen(loc_info));

  return SUCCESS;
}

static long get_online_time(void){

  struct timeval cur_time;

  gettimeofday( &cur_time, NULL );
  //printf("%ld,%ld,%ld\n",cpe_hw_info.start_online_time.tv_sec,cur_time.tv_sec,cur_time.tv_usec);
  return cur_time.tv_sec - cpe_hw_info.start_online_time.tv_sec;
}

static int get_wan_recv_pkts_cnt( void ){

  return SUCCESS;
}

static int get_wan_send_pkts_cnt( void ){

  return SUCCESS;
}

static int get_cpe_rsrp( void ){

  return SUCCESS;

}

static int get_cpe_rssi( void ){

  return SUCCESS;

}

static int get_cpe_sinr( void ){

  return SUCCESS;

}
static int get_cpe_rsrq( void ){

  return SUCCESS;

}

void init_cpe_hw_info(void){
  get_cpe_hw_info();
}

int create_cyclic_report_msg(char * msg){

  if(NULL == msg) return -1;
  //printf("debug\n");
  /*            1   2             3                      4              5              6              7             8          9 */
  sprintf(msg,"%s\"%s\":\"%s\",\"%s\":\"%d-%d-%d-%d\",\"%s\":\"%ld\",\"%s\":\"%u\",\"%s\":\"%u\",\"%s\":\"%d\",\"%s\":\"%d\"%s",\
/* 1 */       "{",\
/* 2 */       "cpeid",cpe_hw_info.cpe_id,\
/* 3 */       "locationinfo",0,0,0,0,\
/* 4 */       "onlinetime",get_online_time(),\
/* 5 */       "WanRPkts",128000,\
/* 6 */       "WanSPkts",128001,\
/* 7 */       "RSRP",9089,\
/* 8 */       "RSSI",-89,\
/* 9 */       "}");

  return SUCCESS;

}


int create_online_report_msg(char * msg){

  if(NULL == msg) return -1;
  //printf("debug\n");
  /*            1   2             3             4             5                      6              7             8              9          10*/
  sprintf(msg,"%s\"%s\":\"%s\",\"%s\":\"%ld\",\"%s\":\"%s\",\"%s\":\"%d.%d.%d.%d\",\"%s\":\"%d\",\"%s\":\"%d\",\"%s\":\"%d\",\"%s\":\"%d\"%s",\
/* 1  */       "{",\
/* 2  */       "CPEID",cpe_hw_info.cpe_id,\
/* 3  */       "IMSI",46225062244429,\
/* 4  */       "VendorName",cpe_hw_info.vendor_name,\
/* 5  */       "CPEIP",10,10,111,24,\
/* 6  */       "SWVersion",283791,\
/* 7  */       "ModemVersion",1902,\
/* 8  */       "ProductType",39,\
/* 9  */       "CellID",3456,\
/* 10 */       "}");

  return SUCCESS;
}


