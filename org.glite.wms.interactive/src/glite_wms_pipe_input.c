#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/poll.h>
#include "glite/wms/thirdparty/bypass/layer.h"

#define GLITE_WMS_PIPE_INPUT_ERROR_RETRY_PERIOD 5 /* Sleep on any error when polling, in seconds. */

void bypass_layer_init();

int main()
 {
  int c;
  int fin,fout;
  struct pollfd rfd;
  int retcod;

  setenv("BYPASS_FAILURE_PASSTHROUGH","on",1);
  bypass_layer_init();

  fin = fileno(stdin);
  fout = fileno(stdout);

  while (1)
   {
    rfd.fd = fin;
    rfd.events = POLLIN;
    if ((retcod=poll(&rfd, 1, -1)) > 0) 
     {
      if (read(fin,&c,1)<1) break;
     }
    else 
     {
      sleep(GLITE_WMS_PIPE_INPUT_ERROR_RETRY_PERIOD);
      continue;
     }

    layer_descend();
    write(fout,&c,1);
    layer_ascend();
   } 

  exit(0);
 }

