#include "mqtt_app.h"
#include "cpe_info.h"
#include "core_json.h"

#include "os_config.h"

#if defined(OS_LINUX)
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#if defined(SOCKET_LINUX)

int main(int argc, char *argv[])
{

    init_cpe_hw_info();

    pthread_t t_cyclic_report, t_inst_reply;//linux thread id

    pthread_create(&t_inst_reply, NULL, &mqtt_query_reply_task_thread, NULL);

    pthread_create(&t_cyclic_report, NULL, &mqtt_cyclic_report_task_thread, NULL);

    while(1);

    return 0;

}

#endif

#if defined(SOCKET_AT)

#include "MQTTAtSocket.h"

int main(int argc, char *argv[])
{

  char retstr[1024];
  at_ret_strings_t ars;

  memset(retstr,'\0',sizeof(retstr));
  memset(&ars,0,sizeof(ars));

  atdevice_init();

#if 0
  atdevice_write(get_at_uart_fd(),"ati\r\n",sizeof("ati\r\n"));
  atdevice_read(get_at_uart_fd(),retstr);

  at_retstr_split(retstr,&ars);
  at_retstr_print(&ars);
  at_retstr_free(&ars);


  memset(retstr,'\n',sizeof(retstr));
  memset(&ars,0,sizeof(ars));
#endif
  int sockfd = -1;

#define BUFFER_MODE 0
#define TRANSPARENT_MODE 2


  sockfd = at_socket_init("47.106.204.139","10003",BUFFER_MODE);

  printf("sockfd %d\n",sockfd);

  if(-1 == sockfd) abort();
  //at_socket_write(sockfd,"hello world\n",sizeof("hello world\n"));


  while (1) {
    //AtDeviceWrite(getAtUartFd(),"at\r\n",sizeof("at\r\n"));
    //sssssssAtDeviceRead(getAtUartFd());
    //printf("[mqtt socket at]\n");
    //sleep(1);
    //AtDeviceWrite(getAtUartFd(),"ati\r\n",sizeof("ati\r\n"));
    //AtDeviceRead(getAtUartFd());
    //AtSocket("***,***.***.***","*****");
    at_socket_read(sockfd,NULL,0,0);
    //at_socket_write(sockfd,"hello",strlen("hello"));

    sleep(3);
  }

  return 0;
}

#endif

#endif


#if defined(OS_RTOS) && defined(SOCKET_AT)
#include "task.h"
#endif