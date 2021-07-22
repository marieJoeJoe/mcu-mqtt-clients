#ifndef CPE_INFO_H
#define CPE_INFO_H

#include "mqtt_app.h"

enum CPE_INFO_RET{
  ERROR = -1,
  FAILED = 0,
  SUCCESS = 1,
};


int get_cpe_id( char* id );
int get_imsi( char* imsi );
int get_vendor_name( char* vendor_name );
int get_cpe_ip( char* cpe_id );
int get_sw_version( char* sw_version );
int get_modem_version( char* modem_version );
int get_product_type( char* product_type );
int get_cell_id( char* cell_id );


int get_location_info( char* location_info );
int get_online_time( int* online_time );
int get_wan_recv_pkts_cnt( int* recv_pkts );
int get_wan_send_pkts_cnt( int* send_pkts );

int get_cpe_rsrp_rssi( int* rsrp_rssi );
int get_cpe_sinr_rsrq( int* sinr_rsrq );

int get_online_ststus(term_online_report_t* online_status);

#endif