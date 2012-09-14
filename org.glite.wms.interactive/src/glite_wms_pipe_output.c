#include <stdio.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include "glite/wms/thirdparty/bypass/layer.h"

#define BUFFER_SIZE 131072

void bypass_layer_init();
void bypass_grid_console_remote(int);

int set_term_();

/* How often should we try to restore broken connections ? */
#define GLITE_WMS_PIPE_OUTPUT_RESTORE_REMOTE_CONNECTION_TIMEOUT 5 

int main()
 {
  char buffer[BUFFER_SIZE];
  fd_set input;
  int retcod;
  int fin,fout;
  int read_ret,write_ret;
  struct timeval periodic_restore;
  char *mess1="\n***********************************\n";
  char *mess2="*    INTERACTIVE JOB FINISHED     *\n";
  char *mess3="***********************************\n";

  bypass_layer_init();
  layer_descend();

  fin = fileno(stdin);
  fout = fileno(stdout);

  set_term_(); 
  while (1)
   {
    FD_SET(fin,&input);
    periodic_restore.tv_sec = GLITE_WMS_PIPE_OUTPUT_RESTORE_REMOTE_CONNECTION_TIMEOUT;
    periodic_restore.tv_usec = 0;
    retcod=select(1,&input, NULL, NULL, &periodic_restore);

    if (retcod == 0)
     {
      layer_ascend();
      /* Try to synchronize with remote end */
      bypass_grid_console_remote(fout);
      layer_descend();

      continue;
     }

    read_ret=read(fin,buffer,BUFFER_SIZE);
    if (read_ret<1) break;

    layer_ascend();
    write_ret=write(fout,buffer,read_ret);
    layer_descend();
    if(write_ret!=read_ret) break;
   }
  layer_ascend();
  write(fout,mess1,strlen(mess1));
  write(fout,mess2,strlen(mess2));
  write(fout,mess3,strlen(mess3));
  fflush(stdout);
  exit(0);
 }

int set_term_()
 {
   int fd;
   struct termios mio_term;

   fd = fileno(stdin);

   if(tcgetattr(fd, &mio_term)<0) return(-1);

   mio_term.c_lflag &= ~ICANON;
   mio_term.c_lflag &= ~ECHO;
   /* Don't wait for terminal input */
   mio_term.c_cc[VMIN] = 0;
   mio_term.c_cc[VTIME] = 0;

   if(tcsetattr(fd, TCSANOW, &mio_term)<0) return(-1);
   fflush(stdin);

   return(0);
 }
