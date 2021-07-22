#include "cpe_info.h"
#include "string.h"




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

static int get_online_time( int* online_time ){

  return SUCCESS;
}

static int get_wan_recv_pkts_cnt( int* recv_pkts ){

  return SUCCESS;
}

static int get_wan_send_pkts_cnt( int* send_pkts ){

  return SUCCESS;
}

static int get_cpe_rsrp_rssi( int* rsrp_rssi ){

  return SUCCESS;
}

static int get_cpe_sinr_rsrq( int* sinr_rsrq ){

  return SUCCESS;
}

int get_online_status(term_online_report_t* online_status){

  int status = 0, ret = -1;
  get_cpe_id( online_status->cpe_id);
  get_imsi( online_status->imsi );
  get_vendor_name( online_status->vendor_name );
  get_cpe_ip( online_status->cpe_ip );
  get_sw_version( online_status->cpe_id );
  get_modem_version( online_status->modem_version );
  get_product_type( online_status->product_type );
  get_cell_id( online_status->cell_id );

  return status;
}



int get_cyclic_ststus(term_cyclic_report_t* online_status){

    


  return SUCCESS;
}


