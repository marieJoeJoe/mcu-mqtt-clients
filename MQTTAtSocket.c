#include "MQTTAtSocket.h"


#define MAX_AT_SOCKET 10

int atSocketFd[MAX_AT_SOCKET] = {1,2,3,4,5,6,7,8,9,10};

static unsigned int at_socket_ocu = -1;


uint8_t rx_data = 0;
uint8_t rx_buffer[1460] = {0};
uint16_t rx_index = 0;

uint8_t mqtt_receive = 0;
char mqtt_buffer[1460] = {0};
uint16_t mqtt_index = 0;

static int usb_uart_fd = -1;

const char *tail_token = "\r\n";

int getAtUartFd(void){
    return usb_uart_fd;
}

int getAtSockOcu(void){
    return at_socket_ocu;
}


void clearRxBuffer(void) {
    rx_index = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));
}

void AtDeviceInit(void)
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

int AtDeviceWrite(int device_fd, unsigned char*cmd,int cmd_len)
{
  int n_written = 0;
  do {
    n_written = write( device_fd, cmd, cmd_len );
    printf("[cmd_len:%d] %s\n",n_written,cmd);
  } while (0);
}


int AtDeviceRead(int device_fd)
{
  int n = 0;

  /* Whole response*/
  char response[1024];
  memset(response, '\0', sizeof response);

  do{
    n = read( device_fd, response, sizeof response);
  }while(0);

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
    char *line = strtok(response, tail_token);
    //printf("%s\n", line);
    // Keep printing tokens while one of the
    // delimiters present in str[].
    while (line != NULL)
    {
        //printf("%s\n", line);
        line = strtok(NULL, tail_token);
    }
 
    return 0;
  }
}


int DeviceSendCommand(void* device, uint8_t *pData, uint16_t Size){


}

int SendAtCommand(char *command, char *reply, uint16_t delay){

    int status = -1;

    status = DeviceSendCommand(NULL,(unsigned char *)command,(uint16_t) strlen(command));

#ifdef OS_RTOS
    osDelay(delay);
#endif

#ifdef OS_LINUX
    sleep(delay);
#endif

    if (strstr(mqtt_buffer, reply) != NULL) {
        clearRxBuffer();
        status = -1;
        return status;
    }
    clearRxBuffer();
    status = 1;
    return status; 
}


int AtSocketRead(Network* n, unsigned char* buffer, int len, int timeout_ms){

  return -1;

}


int AtSocketWrite(Network* n, unsigned char* buffer, int len, int timeout_ms){

  return -1;
  
}


void AtSocketDisconnect(Network* n)
{

  return 0;

}


int AtSocket(char* ipaddr,char *port){

    char at_connect_cmd[128];
    memset(at_connect_cmd,0,sizeof at_connect_cmd);

    sprintf(at_connect_cmd,"at+qiopen=1,%d,\"TCP\",\"%s\",%s,0,1",(-1==at_socket_ocu)?atSocketFd[0],at_socket_ocu++:atSocketFd[++at_socket_ocu],ipaddr,port);
    printf("%d,%d\n",at_socket_ocu,atSocketFd[at_socket_ocu]);
    if(atSocketFd[at_socket_ocu] >= MAX_AT_SOCKET) return -1;
    printf("%s\n",at_connect_cmd);




}