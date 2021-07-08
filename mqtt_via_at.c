#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <termio.h>
#include <unistd.h>



#define AT_DEVICE_FILE "/dev/ttyUSB3"


FILE *fdr, *fdw;
int fd;
struct termio term_save, stdin_save;
void Exit(int sig)
{
   if (fdr) fclose(fdr);
   if (fdw) fclose(fdw);
   ioctl(fd, TCSETA, &term_save);
   close(fd);
   ioctl(fileno(stdin), TCSETA, &stdin_save);
   exit(sig);
}
int main(int argc, char *argv[])
{

   char buffer[80];

   int baud=0, num;
   struct termio term, tstdin;

   if ((fd = open(AT_DEVICE_FILE, O_RDWR | O_NDELAY)) < 0)
   {
      perror(AT_DEVICE_FILE);
      exit(errno);
   }


   baud = B1152000;
   /* Save stdin and tty state and trap some signals */
   ioctl(fd, TCGETA, &term_save);
   ioctl(fileno(stdin), TCGETA, &stdin_save);
   signal(SIGHUP, Exit);
   signal(SIGINT, Exit);
   signal(SIGQUIT, Exit);
   signal(SIGTERM, Exit);
   /*  Set stdin to raw mode, no echo */
   ioctl(fileno(stdin), TCGETA, &tstdin);
   tstdin.c_iflag = 0;
   tstdin.c_lflag &= ~(ICANON | ECHO);
   tstdin.c_cc[VMIN] = 0;
   tstdin.c_cc[VTIME] = 0;
   ioctl(fileno(stdin), TCSETA, &tstdin);
   /*  Set tty state */
   ioctl(fd, TCGETA, &term);
   term.c_cflag |= CLOCAL|HUPCL;
   if (baud > 0)
   {
      term.c_cflag &= ~CBAUD;
      term.c_cflag |= baud;
   }
   term.c_lflag &= ~(ICANON | ECHO); /* to force raw mode */
   term.c_iflag &= ~ICRNL; /* to avoid non-needed blank lines */
   term.c_cc[VMIN] = 0;
   term.c_cc[VTIME] = 10;
   ioctl(fd, TCSETA, &term);
   fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~O_NDELAY);
   /*  Open tty for read and write */
   if ((fdr = fopen(AT_DEVICE_FILE, "r")) == NULL )
   {
      perror(AT_DEVICE_FILE);
      exit(errno);
   }
   if ((fdw = fopen(AT_DEVICE_FILE, "w")) == NULL )
   {
      perror(AT_DEVICE_FILE);
      exit(errno);
   }
   while (1)
   {
      memset(buffer,'\0',sizeof(buffer));
      if ((num = read(fileno(stdin), buffer, 80)) > 0){
         write(fileno(fdw), buffer, num);
         printf("%s\n",buffer);
      }

      memset(buffer,'\0',sizeof(buffer));
      if ((num = read(fileno(fdr), buffer, 80)) > 0)
         write(fileno(stdout), buffer, num);
   }
} 
