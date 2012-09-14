#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

#define BUF_SIZE 1000

int main(int argc,  char **argv) {
 struct timeval *tv;
 struct timezone *tz;
 char *pipin;
 char *pipout;
 char *piperr;
 char *execval;
 char *pipinval;
 char *pipoutval;
 char *piperrval;
 char *totval;
 char *localdir;
 char *localenv;
 char *localpath;
 char *globusenv;
 char *ld_path;
 char *path;

 int size=BUF_SIZE;
 
 char *agent_location="./libglite-wms-grid-console-agent.so.0";
 
 if(argc!=4){
  printf("Usage: %s <remote_host> <remote_port> <exe>\n",argv[0]);
  exit(-1);
 }
 
 localdir=malloc(BUF_SIZE);
 if(localdir==NULL){
  printf("Not enough memory\n");
  exit(-1);  
 }
  globusenv=malloc(BUF_SIZE);
  if(globusenv==NULL){
   printf("Not enough memory\n");
    exit(-1);
  }
 tv=malloc(BUF_SIZE);
 if(tv==NULL){
  printf("Not enough memory\n");
  exit(-1);  
 }
 tz=malloc(BUF_SIZE);
 if(tz==NULL){
  printf("Not enough memory\n");
  exit(-1);  
 }
 pipin=malloc(BUF_SIZE);
 if(pipin==NULL){
  printf("Not enough memory\n");
  exit(-1);  
 }
 pipout=malloc(BUF_SIZE);
 if(pipout==NULL){
  printf("Not enough memory\n");
  exit(-1);  
 }
 piperr=malloc(BUF_SIZE);
 if(piperr==NULL){
  printf("Not enough memory\n");
  exit(-1);  
 }
 execval=malloc(BUF_SIZE);
 if(execval==NULL){
  printf("Not enough memory\n");
  exit(-1);  
 }
 pipinval=malloc(BUF_SIZE);
 if(pipinval==NULL){
  printf("Not enough memory\n");
  exit(-1);  
 }
 pipoutval=malloc(BUF_SIZE);
 if(pipoutval==NULL){
  printf("Not enough memory\n");
  exit(-1);  
 }
 piperrval=malloc(BUF_SIZE);
 if(piperrval==NULL){
  printf("Not enough memory\n");
  exit(-1);  
 }
 totval=malloc(BUF_SIZE);
 if(totval==NULL){
  printf("Not enough memory\n");
  exit(-1);  
 }
 localenv=malloc(BUF_SIZE);
 if(localenv==NULL){
  printf("Not enough memory\n");
  exit(-1);
 }
 ld_path=malloc(BUF_SIZE);
 if(ld_path==NULL){
  printf("Not enough memory\n");
  exit(-1);
 }
 path=malloc(BUF_SIZE);
 if(path==NULL){
  printf("Not enough memory\n");
  exit(-1);
 }
   
 gettimeofday(tv, tz);
 
 if (getcwd(localdir, size) == NULL) {
  printf("getcwd failed\n");
  exit(EXIT_FAILURE);
 }
 sprintf(localdir, "%s%s", localdir, "/");

 localenv=getenv("LD_LIBRARY_PATH");
 globusenv=getenv("GLOBUS_LOCATION");

 sprintf(ld_path, "%s%s%s%s", localenv, ":", globusenv, "/lib");
 setenv ("LD_LIBRARY_PATH", ld_path, 1);

 localpath=getenv("PATH");
 sprintf(path, "%s%s%s", localpath, ":", localdir);
 setenv ("PATH", path, 1);

 setenv ("LD_FAKE_PRELOAD", agent_location, 1);
 
 setenv ("GRID_CONSOLE_RETRY_TIMEOUT", "6", 1);
 
/* setenv ("GRID_CONSOLE_STDOUT", "-", 1);
 setenv ("GRID_CONSOLE_STDIN", "-", 1);
 setenv ("GRID_CONSOLE_STDERR", "-", 1);*/

 setenv ("BYPASS_SHADOW_HOST", argv[1], 1);
 setenv ("BYPASS_SHADOW_PORT", argv[2], 1);
 
 sprintf(pipin, "%s%s%ld", localdir, "pipin", tv->tv_sec);
 mknod(pipin, (S_IFIFO | 00644),S_IFIFO);
 
 sprintf(pipout, "%s%s%ld", localdir, "pipout", tv->tv_sec);
 mknod(pipout, (S_IFIFO | 00644),S_IFIFO);

 sprintf(pipinval, "%s%s%s%s%s", localdir, "glite-wms-pipe-input", " > ", pipin, "&");
 sprintf(pipoutval, "%s%s%s%s%s", localdir, "glite-wms-pipe-output", " < ", pipout, "&");

 sprintf(execval, "%s%s%s%s%s", argv[3], " < ", pipin, " 2>&1 | tee ", pipout);
   
 system(pipinval);
 system(pipoutval);
 system(execval);

 system("killall glite-wms-pipe-input");

 remove(pipin);
 remove(pipout);

 free(localdir); 
 exit(0);
} 
