#include "MQTTAtSocket.h"

#define MAX_AT_SOCKET 10

#define DBG_ABORT(STR)                                \
        do{                                           \
          printf("[%s]: %s\n",__FUNCTION__,(STR));    \
          abort();                                    \
        }while(0)

int atSocketFd[MAX_AT_SOCKET] = {1,2,3,4,5,6,7,8,9,10};

#define LOG_D(...) printf(__VA_ARGS__)
#define LOG_E(...) printf(__VA_ARGS__)
#define LOG_W(...) printf(__VA_ARGS__)

static unsigned int at_socket_ocu = -1;

uint8_t rx_data = 0;
uint8_t rx_buffer[1460] = {0};
uint16_t rx_index = 0;

uint8_t mqtt_receive = 0;
char mqtt_buffer[1460] = {0};
uint16_t mqtt_index = 0;

static int usb_uart_fd = -1;

const char *enter_token = "\r\n";

int get_at_uart_fd(void){
    return usb_uart_fd;
}

int get_at_sock_ocu(void){
    return at_socket_ocu;
}

void clear_rx_buffer(void) {
    rx_index = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));
}

static void at_tcp_ip_errcode_parse(int result)//TCP/IP_QIGETERROR
{
    switch(result)
    {
    case 0   : LOG_D("%d : Operation successful\n",         result); break;
    case 550 : LOG_E("%d : Unknown error\n",                result); break;
    case 551 : LOG_E("%d : Operation blocked\n",            result); break;
    case 552 : LOG_E("%d : Invalid parameters\n",           result); break;
    case 553 : LOG_E("%d : Memory not enough\n",            result); break;
    case 554 : LOG_E("%d : Create socket failed\n",         result); break;
    case 555 : LOG_E("%d : Operation not supported\n",      result); break;
    case 556 : LOG_E("%d : Socket bind failed\n",           result); break;
    case 557 : LOG_E("%d : Socket listen failed\n",         result); break;
    case 558 : LOG_E("%d : Socket write failed\n",          result); break;
    case 559 : LOG_E("%d : Socket read failed\n",           result); break;
    case 560 : LOG_E("%d : Socket accept failed\n",         result); break;
    case 561 : LOG_E("%d : Open PDP context failed\n",      result); break;
    case 562 : LOG_E("%d : Close PDP context failed\n",     result); break;
    case 563 : LOG_W("%d : Socket identity has been used\n", result); break;
    case 564 : LOG_E("%d : DNS busy\n",                     result); break;
    case 565 : LOG_E("%d : DNS parse failed\n",             result); break;
    case 566 : LOG_E("%d : Socket connect failed\n",        result); break;
    // case 567 : LOG_W("%d : Socket has been closed",       result); break;
    case 567 : break;
    case 568 : LOG_E("%d : Operation busy\n",               result); break;
    case 569 : LOG_E("%d : Operation timeout\n",            result); break;
    case 570 : LOG_E("%d : PDP context broken down\n",      result); break;
    case 571 : LOG_E("%d : Cancel send\n",                  result); break;
    case 572 : LOG_E("%d : Operation not allowed\n",        result); break;
    case 573 : LOG_E("%d : APN not configured\n",           result); break;
    case 574 : LOG_E("%d : Port busy\n",                    result); break;
    default  : LOG_E("%d : Unknown err code\n",             result); break;
    }
}


void atdevice_init(void)
{

  struct termios tty;//, tty_old;

  memset(&tty,0,sizeof tty);
  
  usb_uart_fd = open( AT_DEVICE_FILE, O_RDWR|O_NOCTTY);
  if (-1 == usb_uart_fd){
    printf("can not open uart\n");
  }
  /* Error Handling */
  if ( tcgetattr ( usb_uart_fd, &tty ) != 0 ) {
    printf("Error %d from tcgetattr %s\n",errno,strerror(errno));
  }
  /* Save old tty parameters */
  //tty_old = tty;

  /* Set Baud Rate */
  cfsetospeed (&tty, (speed_t)B115200);
  //cfsetispeed (&tty, (speed_t)B9600);

  /* Setting other Port Stuff */
  tty.c_cflag     &=  ~PARENB;            // Make 8n1
  tty.c_cflag     &=  ~CSTOPB;
  tty.c_cflag     &=  ~CSIZE;
  tty.c_cflag     |=  CS8;

  tty.c_cflag     &=  ~CRTSCTS;           // no flow control
  tty.c_cc[VMIN]   =  1;                  // read doesn't block
  tty.c_cc[VTIME]  =  20;                  // 0.5 seconds read timeout
  tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

  /* Make raw */
  cfmakeraw(&tty);

  /* Flush Port, then applies attributes */
  tcflush( usb_uart_fd, TCIFLUSH );
  if ( tcsetattr ( usb_uart_fd, TCSANOW, &tty ) != 0) {
     printf("Error %d from tcsetattr\n",errno);
  }
}

int atdevice_write( int device_fd, char*cmd, int cmd_len )
{
  int n_written = 0;
  do {
    n_written = write( device_fd, cmd, cmd_len );
    printf("[cmd_len:%d] %s\n",n_written,cmd);
  } while (0);

  return 0;
}


int atdevice_read( int device_fd, char *retstr )
{
  int n = 0,retstrlen=0;

  /* Whole response*/
  //char response[1024];
  char *response = malloc(1024);
  //memset(response, '\0', sizeof response);
  memset(response,'\0',1024);
  //printf("res size %d\n",sizeof *response);
  do{
    //n = read( device_fd, response, sizeof response);
    n = read( device_fd, response, 1024);
  }while(0);

  //printf("[%s %d]\n",__FUNCTION__,n);

  if(n < 0){
    printf("Error reading: %s\n",strerror(errno));
  }else if(n == 0) {
    printf("Read nothing!\n");
  }else {
    retstrlen = strlen(response);
    strncpy(retstr, response, retstrlen);
    free(response);
    //printf("%s\n", last_line);
    return retstrlen;
  }

  return 0;
}

int device_send_command(void* device, uint8_t *pData, uint16_t Size){


  return 0;
}

int send_at_command(char *command, char *reply, uint16_t delay){

    int status = -1;

    status = device_send_command(NULL,(unsigned char *)command,(uint16_t) strlen(command));

#ifdef OS_RTOS
    osDelay(delay);
#endif

#ifdef OS_LINUX
    sleep(delay);
#endif

    if (strstr(mqtt_buffer, reply) != NULL) {
        clear_rx_buffer();
        status = -1;
        return status;
    }
    clear_rx_buffer();
    status = 1;
    return status; 
}


int at_socket_read(int socketfd, unsigned char* buffer, int len, int timeout_ms){

  char at_retstr[1024];
  at_ret_strings_t ars;

  if(-1 == socketfd){
    printf("socketfd %d error !\n",socketfd);
  }

  memset(at_retstr, '\0', sizeof(at_retstr));
  memset(&ars, 0, sizeof(at_ret_strings_t));

  atdevice_read(get_at_uart_fd(), at_retstr);

  at_retstr_split(at_retstr,&ars);

  at_retstr_print(&ars);

  at_retstr_free(&ars);

  return 0;
}

int at_socket_write(int socketfd, unsigned char* buffer, int len){

  char at_send_cmd[128];
  char at_retstr[1024];
  at_ret_strings_t ars;

  
  if(-1 == socketfd){
    printf("socketfd %d error !\n",socketfd);
  }

  memset(at_send_cmd,0,sizeof at_send_cmd);
  memset(at_retstr,'\0',sizeof at_retstr);
  memset(&ars,0,sizeof(at_ret_strings_t));

  sprintf(at_send_cmd,"at+qisend=%d,%d,%s\\0x1a",socketfd,len,buffer);
  //printf("%s len %ld\n",at_send_cmd,strlen(at_send_cmd));
  atdevice_write(get_at_uart_fd(), at_send_cmd, strlen(at_send_cmd));
  sleep(2);
  atdevice_read(get_at_uart_fd(), at_retstr);
  //printf("[%s] %s\n",__FUNCTION__, at_retstr);
  at_retstr_split(at_retstr,&ars);

  at_retstr_print(&ars);
  at_retstr_free(&ars);

  return 0;
}


void at_socket_disconnect(Network* n)
{


}

void at_socket_reset(int socketfd)
{

  char at_reset_cmd[128];
  char at_retstr[512];
  //at_ret_strings_t ars;

  memset(at_reset_cmd,0,sizeof at_reset_cmd);
  memset(at_retstr,'\0',sizeof at_retstr);
  //memset(&ars,0,sizeof(at_ret_strings_t));
  sprintf(at_reset_cmd,"at+qiclose=%d\r\n",socketfd);
  atdevice_write(get_at_uart_fd(), at_reset_cmd, strlen(at_reset_cmd));
}

void at_module_reset(void)
{
  atdevice_write(get_at_uart_fd(), "at+cfun=1\r\n", strlen("at+cfun=1\r\n"));
}



int at_socket_init(char* ipaddr,char *port, int conn_mode){

  char at_connect_cmd[128];
  char at_retstr[1024];
  at_ret_strings_t ars;

  int reset_count = 5, socket_stat = -1;
  int sockid = atSocketFd[++at_socket_ocu];
  if(sockid >= MAX_AT_SOCKET) return -1;
  //printf("[%s]sockid %d\n",__FUNCTION__,sockid);

  while(reset_count-- > 0){
    //printf("[%s]loop %d\n",__FUNCTION__,reset_count);
    memset(at_connect_cmd,0,sizeof at_connect_cmd);
    memset(at_retstr,'\0',sizeof at_retstr);
    memset(&ars,0,sizeof(at_ret_strings_t));
    //sprintf(at_connect_cmd,"at+qiopen=1,%d,\"TCP\",\"%s\",%s,0,%d\r\n",sockid,ipaddr,port,conn_mode);
    sprintf(at_connect_cmd,"at+qiopen=1,%d,\"TCP\",\"%s\",%s,0,%d\r\n",sockid,ipaddr,port,conn_mode);   
    //printf("[%s] %s\n",__FUNCTION__, at_connect_cmd);
    atdevice_write(get_at_uart_fd(), at_connect_cmd, strlen(at_connect_cmd));
    //atdevice_write(get_at_uart_fd(), "\r\n", sizeof("\r\n"));
    sleep(0.5);
    atdevice_read(get_at_uart_fd(), at_retstr);
    //printf("[%s] %s\n",__FssUNCTION__, at_retstr);
    at_retstr_split(at_retstr, &ars);
    //at_retstr_print(&ars);
    int ret = 0, ret_socketfd = -1, ret_code = -1;
    //printf("%s\n",ars.ret_str_list[ars.ret_str_num-1]);
    ret = sscanf(ars.ret_str_list[ars.ret_str_num - 1], "+QIOPEN: %d,%d", &ret_socketfd, &ret_code);

    //printf("[%s] ret %d socketfd %d ret_code %d\n", __FUNCTION__, ret, ret_socketfd, ret_code);

    if( 0 == ret){
      socket_stat = socket_conn_check(sockid,ipaddr,port);
      if((2 == socket_stat)||(1 == socket_stat)){
        ret_code = 0;
      }else{
        DBG_ABORT("qiopen return message do not correct!!");
      }
    }

    if(sockid == ret_socketfd){
        if( 0 == ret_code ){
          at_retstr_free(&ars);
          return ret_socketfd;
        }
        at_tcp_ip_errcode_parse(ret_code);
        if( 563 ==  ret_code )
        {
          at_socket_reset(sockid);
          sleep(3);
        }
    }
    at_retstr_free(&ars);
  }
  at_module_reset();
  DBG_ABORT("retry count expired, reset module !!");

  return -1;
}

int socket_conn_check(int sockid, char* ipaddr, char* port){

  char at_connect_cmd[128];
  char at_retstr[256];
  char sscanf_str[256];
  at_ret_strings_t ars;

  memset(at_connect_cmd,0,sizeof at_connect_cmd);
  memset(at_retstr,'\0',sizeof at_retstr);
  memset(&ars,0,sizeof(at_ret_strings_t));
  sprintf(at_connect_cmd,"AT+QISTATE=1,%d\r\n",sockid);
  //printf("[%s] %s\n",__FUNCTION__, at_connect_cmd);

  atdevice_write(get_at_uart_fd(), at_connect_cmd, strlen(at_connect_cmd));
  //sleep(3);
  atdevice_read(get_at_uart_fd(), at_retstr);
  //printf("[%s] %s\n",__FUNCTION__, at_retstr);
  at_retstr_split(at_retstr, &ars);
  int ret = -1, local_port = 0, socket_stat = -1, srv_id = -1, id4 = -1, id5 = -1;
  //printf("%s\n",ars.ret_str_list[ars.ret_str_num-1]);

  memset( sscanf_str, '\0', sizeof(sscanf_str));

  sprintf(sscanf_str,"+QISTATE: %d,\"TCP\",\"%s\",%s,%s,%s,%s,%s,%s,\"usbmodem\"",sockid,ipaddr,port,"%d","%d","%d","%d","%d");

  printf("%s\n",sscanf_str);

  ret = sscanf(ars.ret_str_list[0], sscanf_str , &local_port, &socket_stat, &srv_id, &id4, &id5);

  at_retstr_print(&ars);
  at_retstr_free(&ars);

  printf("%d %d %d %d %d %d \n",ret, local_port, socket_stat, srv_id, id4, id5);

  return socket_stat;
}


void at_retstr_print(at_ret_strings_t* ret)
{
  printf("ret %d lines:\n",ret->ret_str_num);
  for(int i = 0; i<ret->ret_str_num; i++){
     printf("%d %s\n",i+1,ret->ret_str_list[i]);
  }
}


int at_retstr_split(char* pkt, at_ret_strings_t *ret)
{
  if(0!= ret->ret_str_num) ret->ret_str_num = 0;
  char *last_line = NULL,*line = strtok(pkt, enter_token);
  int line_count = 0;
  //printf("%s\n", line);
  // Keep printing tokens while one of the
  // delimiters present in str[].
  while (line != NULL)
  {
    line_count++;
    //printf("%s\n", line);
    last_line = line;
    ret->ret_str_list = realloc(ret->ret_str_list,line_count*(sizeof(char*)));
    ret->ret_str_list[line_count-1] = malloc(strlen(line)+1);
    strncpy(ret->ret_str_list[line_count-1],line,strlen(line)+1);
    line = strtok(NULL, enter_token);
  }
  ret->ret_str_num = line_count;

  return 1;
}


void at_retstr_free(at_ret_strings_t* ret)
{
  if(NULL==ret) return;

  for(int i = 0; i < ret->ret_str_num; i++){
    free(ret->ret_str_list[i]);
  }
  free(ret->ret_str_list);
}



