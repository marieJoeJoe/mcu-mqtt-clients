#include "MQTTAtSocket.h"


#define MAX_AT_SOCKET 10

int atSocketFd[MAX_AT_SOCKET] = {2,3,4,5,6,7,8,9,10,11};

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

void atdevice_init(void)
{

  struct termios tty;
  struct termios tty_old;

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
  tty_old = tty;

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
  tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
  tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

  /* Make raw */
  cfmakeraw(&tty);

  /* Flush Port, then applies attributes */
  tcflush( usb_uart_fd, TCIFLUSH );
  if ( tcsetattr ( usb_uart_fd, TCSANOW, &tty ) != 0) {
     printf("Error %d from tcsetattr\n",errno);
  }
}

int atdevice_write(int device_fd, unsigned char*cmd,int cmd_len)
{
  int n_written = 0;
  do {
    n_written = write( device_fd, cmd, cmd_len );
    //printf("[cmd_len:%d] %s\n",n_written,cmd);
  } while (0);
}


int atdevice_read(int device_fd,char *retstr)
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

  printf("[%d]\n",n);

  if(n < 0){
    printf("Error reading: %s\n",strerror(errno));
  }else if(n == 0) {
    printf("Read nothing!\n");
  }else {
    //printf("Resp: %d\n%s\n",strlen(response),response);
    //printf("\n");
    //for(int i=0; i< strlen(response)+1; i++){
        //printf("%c ",response[i]);
        //if(0 == i%64) printf("\n");
    //}
    //printf("\n");
    //for(int i=0; i< strlen(response)+1; i++){
        //printf("%02x ",response[i]);
        //if(0 == i%64) printf("\n");
    //}
    //char *last_line,*line = strtok(response, enter_token);
    //printf("%s\n", line);
    // Keep printing tokens while one of the
    // delimiters present in str[].
    //while (line != NULL)
    //{
        //printf("%s\n", line);
        //last_line = line;
        //line = strtok(NULL, enter_token);
        //if(NULL == line)
    //}
    retstrlen = strlen(response);
    strncpy(retstr,response,retstrlen);
    free(response);
    //printf("%s\n", last_line);
    return ;
  }
}


int device_send_command(void* device, uint8_t *pData, uint16_t Size){


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


int at_socket_read(Network* n, unsigned char* buffer, int len, int timeout_ms){

  return -1;

}


int at_socket_write(Network* n, unsigned char* buffer, int len, int timeout_ms){

  return -1;
  
}


void at_socket_disconnect(Network* n)
{

  return 0;

}


int at_socket_init(char* ipaddr,char *port){

  char at_connect_cmd[128];
  char at_retstr[1024];
  at_ret_strings_t ars;


  int sockid = atSocketFd[++at_socket_ocu];
  
  //if(-1==at_socket_ocu){
    //sockid = atSocketFd[0];
    //++at_socket_ocu;
  //}else{
    //sockid = atSocketFd[++at_socket_ocu];
  //}

  memset(at_connect_cmd,0,sizeof at_connect_cmd);
  memset(at_retstr,'\0',sizeof at_retstr);
  memset(&ars,0,sizeof(at_ret_strings_t));
  sprintf(at_connect_cmd,"AT+QIOPEN=1,%d,\"TCP\",\"%s\",%s,0,0",atSocketFd[++at_socket_ocu],ipaddr,port);
  //printf("%d,%d\n",at_socket_ocu,atSocketFd[at_socket_ocu]);
  if(atSocketFd[at_socket_ocu] >= MAX_AT_SOCKET) return -1;
  //printf("%s\n",at_connect_cmd);
  printf("[%s] %s\n",__FUNCTION__,at_connect_cmd);
  atdevice_write(get_at_uart_fd(), at_connect_cmd, strlen(at_connect_cmd)+1);
  atdevice_write(get_at_uart_fd(), "\r\n", sizeof("\r\n"));
  sleep(2);
  atdevice_read(get_at_uart_fd(),at_retstr);
  printf("[%s] %s\n",__FUNCTION__,at_retstr);
  at_retstr_split(at_retstr,&ars);

  at_retstr_print(&ars);

  return atSocketFd[at_socket_ocu];

}


int at_retstr_print(at_ret_strings_t* ret)
{
  printf("ret %d lines:\n",ret->ret_str_num);
  for(int i = 0; i<ret->ret_str_num; i++){
     printf("%d %s\n",i+1,ret->ret_str_list[i]);
  }
}


int at_retstr_split(char* pkt, at_ret_strings_t *ret)
{
  if(0!= ret->ret_str_num) ret->ret_str_num = 0;
  char *last_line,*line = strtok(pkt, enter_token);
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
}


void at_retstr_free(at_ret_strings_t* ret)
{

/*
  char* pn=NULL;
  printf("line_num %d %p\n",ret->ret_str_num,ret->ret_str_list);

  for(int i = 0; i < ret->ret_str_num; i++){
    pn = ret->ret_str_list[i];
    printf("%p\n",pn);
  }
*/

  for(int i = 0; i < ret->ret_str_num; i++){
    //pn = ret->ret_str_list[i];
    free(ret->ret_str_list[i]);
    //printf("%p\n",pn);
    //free(pn);
    //ret->ret_str_list[i]=NULL;
  }
  free(ret->ret_str_list);
  //ret->ret_str_list=NULL;
}

