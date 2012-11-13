/* 
 * File: glite-process-counter-args.c
 * Author: Francesco Prelz <Francesco.Prelz@mi.infn.it>
 * Copyright (c) 2007 EGEE
 * For license conditions see http://public.eu-egee.org/license/license.htm
 *
 * $Id$
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>

#define GLITE_PROCESS_COUNTER_PROC_DIR "/proc"
#define GLITE_PROCESS_COUNTER_DEFAULT_MAX_PROC 30
#define GLITE_PROCESS_COUNTER_DEFAULT_RETRY_CODE 255
#define GLITE_PROCESS_COUNTER_DEFAULT_RETRY_WAIT 300
#define GLITE_PROCESS_COUNTER_MAX_WAIT 60
#define GLITE_PROCESS_COUNTER_WAIT_PER_SLOT 10

typedef struct dirlent_s
{
  char *name;
  struct dirlent_s *next;
} dirlent;

dirlent *push_dir(dirlent *head, const char *d)
{
  if (d == NULL) return head;

  dirlent *new;
  new = (dirlent *)malloc(sizeof(dirlent));
  if (new == NULL)
   {
    fprintf(stderr,"Out of Memory\n");
    return head;
   }
  new->name = strdup(d);
  if (new->name == NULL)
   {
    fprintf(stderr,"Out of Memory\n");
    free(new);
    return head;
   }

  new->next = head; 

  return new;
}

void free_dirlent(dirlent *head)
{
  dirlent *cur, *next;

  for (cur = head; cur != NULL; cur=next)
   {
    next = cur->next;
    if (cur->name != NULL) free(cur->name);
    free(cur);
   }
}

int count_files(const dirlent *fds, const struct stat *count_stat)
{
  const dirlent *cur;
  DIR *fddir;
  struct dirent *cur_ent;
  struct stat f_stat;
  int ret_count = 0;
  int nlen;
  char *fname;

  for (cur = fds; cur != NULL; cur=cur->next)
   {
    nlen = strlen(cur->name);
    fddir = opendir(cur->name);
    if (fddir != NULL)
     {
      while ((cur_ent = readdir(fddir)) != NULL)
       {
        if (cur_ent->d_type == DT_LNK)
         {
          fname = (char *)malloc(nlen + strlen(cur_ent->d_name) + 2);
          if (fname != NULL)
           {
            sprintf(fname,"%s/%s",cur->name,cur_ent->d_name);
            /* Need to call stat() to follow the symlink */
            if (stat(fname, &f_stat) >= 0)
             {
              if ((f_stat.st_dev == count_stat->st_dev) &&
                  (f_stat.st_ino == count_stat->st_ino)) ret_count++;
             }
            free(fname);
           }
          else
           {
            /* Out of memory: A good reason to err on the safe side. */
            ret_count++;
           }
         }
       }
      closedir(fddir);
     }
   }
  return(ret_count);
}

dirlent *find_fds(char *pdirname)
{
  DIR *pdir; 
  int plen;
  struct dirent *proc_ent;
  dirlent *d_head = NULL;
  struct stat fdstat;
  char *fname;

  plen = strlen(pdirname);
  pdir = opendir(pdirname);

  if (pdir != NULL)
   {
    while ((proc_ent = readdir(pdir)) != NULL)
     {
      if (proc_ent->d_type == DT_DIR)
       {
        if (atoi(proc_ent->d_name) > 0) /* Process ID dir ? */
         {
          fname = (char *)malloc(plen + strlen(proc_ent->d_name) + 5);
          if (fname != NULL)
           {
            sprintf(fname,"%s/%s/fd",pdirname,proc_ent->d_name);
            if (stat(fname, &fdstat) >= 0 )
             {
              if (S_ISDIR(fdstat.st_mode)) d_head = push_dir(d_head,fname);
             }
            free(fname);
           } 
         }
       }
     }
    closedir(pdir);
   }
  return(d_head); 
}

void
wait_for_slot(const char *count_file, int max_procs)
{
  dirlent *fds;
  struct stat count_stat;
  struct stat wait_stat;
  char *wait_file_name;
  const char *wait_ext="_wait";
  FILE *waitfd=NULL;
  int count;
  int wait_count;
  int sleep_secs;

  /* If the file is not there, why bother waiting ? */
  if (stat(count_file, &count_stat) < 0) return;

  wait_file_name = (char *) malloc(strlen(count_file) + strlen(wait_ext) + 1);
  if (wait_file_name != NULL)
   {
    strcpy(wait_file_name,count_file);
    strcat(wait_file_name,wait_ext);
    waitfd = fopen(wait_file_name,"w+");
    if (waitfd != NULL) 
     {
      if (fstat(fileno(waitfd),&wait_stat) < 0)
       {
        fclose(waitfd);
        waitfd = NULL;
       }
     }
    /* We can safely do without the linear backoff if any of the above fails. */
   }

  count = max_procs + 1;
  while (count > max_procs)
   {
    fds = find_fds(GLITE_PROCESS_COUNTER_PROC_DIR);
    if (fds == NULL) break ; /*Don't wait if fd dirs cannot be found in /proc */
    
    count = count_files(fds,&count_stat);
#ifdef DEBUG
    fprintf(stderr,"DEBUG: count == %d\n",count);
#endif
    if (count <= max_procs) 
     {
      free_dirlent(fds);
      break;
     }

    sleep_secs = rand()%GLITE_PROCESS_COUNTER_MAX_WAIT ; 
    if (waitfd != NULL)
     {
      wait_count = count_files(fds,&wait_stat);
#ifdef DEBUG
      fprintf(stderr,"DEBUG: wait_count == %d\n",wait_count);
#endif
      sleep_secs += (wait_count * GLITE_PROCESS_COUNTER_WAIT_PER_SLOT);
     }
#ifdef DEBUG
    fprintf(stderr,"DEBUG: sleeping for %d seconds.\n",sleep_secs);
#endif
    free_dirlent(fds);
    sleep(sleep_secs);
   }
  if (waitfd != NULL) fclose(waitfd);
}

int
main(int argc, char *argv[])
{
  char *my_name;
  char *count_file_name;
  char *count_file_name_format = "/tmp/%s.process_count";
  FILE *countfd;
  char *real_file_name;
  char *real_file_name_format = "%s_real";
  const char *usage_format="Usage: %s [-max n_procs] [-rcode retry_exit_code] [-rwait retry_wait_seconds] -- args to pass\n"; 
  int max_procs = GLITE_PROCESS_COUNTER_DEFAULT_MAX_PROC;
  int retry_wait = GLITE_PROCESS_COUNTER_DEFAULT_RETRY_WAIT;
  int retry_code = GLITE_PROCESS_COUNTER_DEFAULT_RETRY_CODE;
  pid_t child_pid;
  int child_retcod;
  int child_exitstatus=0;
  char **new_argv;
  int first_argc_to_pass=-1;
  int i;

  for (i=1; i<argc; i++)
   {
    if (strcmp(argv[i], "-max")==0)
     {
       if ((i+1) >= argc) break;
       i++;
       max_procs = atoi(argv[i]); 
     }
    else if (strcmp(argv[i], "-rcode")==0)
     {
       if ((i+1) >= argc) break;
       if (strcmp(argv[i+1], "--")==0) break;
       i++;
       retry_code = atoi(argv[i]); 
     }
    else if (strcmp(argv[i], "-rwait")==0)
     {
       if ((i+1) >= argc) break;
       if (strcmp(argv[i+1], "--")==0) break;
       i++;
       retry_wait = atoi(argv[i]); 
       if (retry_wait == 0) break;
     }
    else if (strcmp(argv[i], "--")==0)
     {
      first_argc_to_pass = i+1;
      break;
     }
   }

  if ((first_argc_to_pass <= 0) || (max_procs <= 0))
   {
    fprintf(stderr,usage_format,argv[0]);
    return 1;
   }

  srand(time(0));

  my_name = strrchr(argv[0],'/');
  if (my_name == NULL) my_name=argv[0];
  else my_name++; /* Skip past '/'. */

  count_file_name = (char *)malloc(strlen(count_file_name_format)+
                                   strlen(my_name)); 
  if (count_file_name == NULL)
   {
    fprintf(stderr,"%s: Out of memory.\n",argv[0]);
    return 1;
   }
  real_file_name = (char *)malloc(strlen(real_file_name_format)+
                                  strlen(argv[0])); 
  if (real_file_name == NULL)
   {
    free(count_file_name);
    fprintf(stderr,"%s: Out of memory.\n",argv[0]);
    return 1;
   }

  sprintf(count_file_name, count_file_name_format, my_name);
  sprintf( real_file_name,  real_file_name_format, argv[0]);

  new_argv = (char **)malloc(sizeof(char *) * (argc-first_argc_to_pass+2));
  if (new_argv == NULL)
   {
    free(count_file_name);
    free(real_file_name);
    fprintf(stderr,"%s: Out of memory.\n",argv[0]);
    return 1;
   }
  
  new_argv[0] = real_file_name;
  for (i=1; i<=(argc-first_argc_to_pass); i++) 
   new_argv[i] = argv[i+first_argc_to_pass-1];
  new_argv[i] = NULL;

  for (;;) /* Try executing while we get retry_code */
   {
    wait_for_slot(count_file_name, max_procs);

    countfd = fopen(count_file_name,"w+");

    if ((child_pid = fork())==0)
     {
      /* Child */
      fclose(countfd); /* Let's not double-count. */
      execv(real_file_name,new_argv);
     }

    if (child_pid > 0)
     {
      /* Wait for completion */
      if (waitpid(child_pid, &child_retcod, 0) > 0)
       {
        if (WIFSIGNALED(child_retcod))
         {
          /* Propagate the signal to the current proc. */
          kill(getpid(),WTERMSIG(child_retcod));
         } 
        else if (WIFEXITED(child_retcod)) child_exitstatus = WEXITSTATUS(child_retcod);
        if (child_exitstatus != retry_code) break;
       }
     }

    if (countfd != NULL) fclose(countfd);

    sleep(retry_wait);
   }

  /* Clean up. */
  if (countfd != NULL) fclose(countfd);
  free(count_file_name);
  free(real_file_name);
  free(new_argv);

  return child_exitstatus;
}
