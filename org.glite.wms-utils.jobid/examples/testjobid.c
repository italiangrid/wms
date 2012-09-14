/* test code for jobid routines */

#include <stdio.h>
#include <stdlib.h>

#include "glite/wmsutils/jobid/cjobid.h"

int main(int argc, char* argv[])
{
  char* ju;
  char* bkserver = "ujsa.uhjs";

  edg_wlc_JobId ji = 0;
  edg_wlc_JobId ji2 = 0;

  int r = edg_wlc_JobIdCreate(bkserver, 0, &ji);
  printf("Create: %d\n", r);

  ju = edg_wlc_JobIdUnparse(ji);
  printf("Unparse:  %s\n", ju);

  edg_wlc_JobIdParse(ju, &ji2);
  free(ju);

  ju = edg_wlc_JobIdUnparse(ji);
  printf("Unparse2: %s\n", ju);
  free(ju);

  edg_wlc_JobIdFree(ji);
  edg_wlc_JobIdFree(ji2);

  return 0;
}
