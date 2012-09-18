/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#define DEFAULT_ADJUST_DIRECTORY_GROUP "glite"
#define DEFAULT_ADJUST_DIRECTORY_MODE  0770

#ifdef ALLOW_COMMAND_LINE_OVERRIDE
#define ADJUST_DIRECTORY_USAGE "Usage:%s \n[-v] verbosity\n[-h] help\n[-c uid] [-g group] [-m mode] directory name, ...\n"
#define ADJUST_DIRECTORY_GETOPT_STRING "vhc:x:g:m:"
#else
#define ADJUST_DIRECTORY_USAGE "Usage:%s  \n[-v] verbosity\n[-h] help\n[-c uid] directory name, ...\n"
#define ADJUST_DIRECTORY_GETOPT_STRING "vhc:"
#endif

#include <stdio.h>
#include <signal.h>

#include <string.h> // strdup
#include <stdlib.h> // exit
#include <errno.h> // errno
#include <grp.h> // getgrnam
#include <unistd.h> // getopt

#include <sys/types.h>  // mode_t
#include <errno.h> // errno
#include <unistd.h> // rmdir, chown
#include <sys/stat.h> // stat

#include <fcntl.h> // O_RDONLY
#include <sys/param.h> // MAXPATHLEN

#define ADJUST_DIRECTORY_ERR_NO_ERROR       0
#define ADJUST_DIRECTORY_ERR_OPTIONS        1
#define ADJUST_DIRECTORY_ERR_OUT_OF_MEMORY  2
#define ADJUST_DIRECTORY_ERR_NO_SUCH_GROUP  3
#define ADJUST_DIRECTORY_ERR_STAT           4
#define ADJUST_DIRECTORY_ERR_CHOWN          8
#define ADJUST_DIRECTORY_ERR_CHMOD         16
#define ADJUST_DIRECTORY_ERR_NOT_A_DIR     32
#define ADJUST_DIRECTORY_ERR_MKDIR         64

// Untar option
#define UNTAR_ERR_NO_ERROR                  0
#define UNTAR_ERR_SETUID_SETGID            80
#define UNTAR_ERR_OPEN_FILE                82
#define UNTAR_ERR_EXTRACTING_FILE          84
#define UNTAR_ERR_ARCHIVE_FILE_EXISTS      86


// Untar functionalities
#include "zlib.h"
#include "libtar.h"

extern char *optarg;
extern int optind, opterr, optopt;

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

typedef void handler_func(int);
void install_signal(int signo, handler_func* func)
{
   struct sigaction act, old_act;
   sigemptyset(&act.sa_mask);
   act.sa_flags = 0;
   act.sa_handler = func;
   sigaction(signo, &act, &old_act);
}

void
initsignalhandler()
{
   install_signal(SIGUSR1, SIG_IGN);
   install_signal(SIGPIPE, SIG_IGN);
   install_signal(SIGTERM, SIG_IGN);
   install_signal(SIGXFSZ, SIG_IGN);
   install_signal(SIGHUP, SIG_IGN);
   install_signal(SIGINT, SIG_IGN);
   install_signal(SIGUSR1, SIG_IGN);
   install_signal(SIGQUIT, SIG_IGN);
}

long
computeFileSize(char *path)
{
   int fd = -1;
   long size = 0;
   fd = open(path, O_RDONLY);
   if (fd != -1) {
      struct stat buf;
      if (!fstat(fd, &buf)) {
         size = buf.st_size;
      }
      close(fd);
   }
   // If file not found it returns 0
   return size;
}

int
fileExists(char *path)
{
   struct stat buffer;
   if (stat(path, &buffer)) {
      return FALSE;
   }
   return TRUE;
}

int
gzUncompress(char *source, char *dest)
{
   FILE *out = NULL;
   // char *zmsg = NULL;
   gzFile in;
   int len = 0;
   int err = 0;
   long size = computeFileSize(source);
   if (size == 0) {
      fprintf(stderr, "Unable to uncompress the ISB file: %s\n"
              "(error opening file or null size)\n", source);
      return UNTAR_ERR_OPEN_FILE;
   }
   in = gzopen(source, "rb");
   if (in == NULL) {
      fprintf(stderr, "Unable to uncompress the ISB file: %s\n"
              "(error opening file)\n", source);
      return UNTAR_ERR_OPEN_FILE;
   }
   out = fopen(dest, "wb");
   if (out == NULL) {
      fprintf(stderr, "Unable to uncompress zip file: %s\n"
              "(unable to create the uncompressed file: %s)\n", source, dest);
      return UNTAR_ERR_OPEN_FILE;
   }
   unsigned char buf[65535];
   for (;;) {
      len = gzread(in, buf, sizeof(buf));
      if (len < 0)  {
         const char * zmsg = gzerror(in, &err);
         if (*zmsg != '\0') {
            fprintf(stderr, "Error while uncompressing zip file: %s\n(%s)\n",
                    source, zmsg);
         } else {
            fprintf(stderr, "Error while uncompressing zip file: %s\n",
                    source);
         }
         return UNTAR_ERR_EXTRACTING_FILE;
      } else if (len == 0) {
         break;
      }
      if ((int) fwrite(buf, 1, (unsigned) len, out) != len) {
         fprintf(stderr, "Unable to uncompress zip file: %s\n"
                 "(error while writing the file: %s)\n", source, dest);
         return UNTAR_ERR_EXTRACTING_FILE;
      }
   }
   if (fclose(out)!=0) {
      fprintf(stderr, "Unable to uncompress zip file: %s\n"
              "(error while writing the file: %s)\n", source, dest);
      return UNTAR_ERR_EXTRACTING_FILE;
   }
   if (gzclose(in) != Z_OK) {
      fprintf(stderr, "Unable to remove the .gz file: %s\n"
              "(error in closing the file)\n", source);
   } else {
      remove(source);
   }
   return Z_OK;
}

char *
gzError(int ret)
{
   char *err = NULL;
   switch (ret) {
   case Z_ERRNO:
      err = "i/o error";
      break;
   case Z_STREAM_ERROR:
      err = "invalid compression level";
      break;
   case Z_DATA_ERROR:
      err = "invalid or incomplete deflate data";
      break;
   case Z_MEM_ERROR:
      err = "out of memory";
      break;
   case Z_VERSION_ERROR:
      err = "zlib version mismatch";
      break;
   }
   return err;
}

int
chmod_tar_extract_all(TAR *t, char *prefix)
{
   char *filename = NULL;
   char buf[MAXPATHLEN];
   int i;
   mode_t mode = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH;
   while ((i = th_read(t)) == 0) {
      filename = th_get_pathname(t);

      // Checking if the item is a regular file
      if (TH_ISREG(t)) {
         if (t->options & TAR_VERBOSE) {
            th_print_long_ls(t);
         }
         if (prefix != NULL) {
            snprintf(buf, sizeof(buf), "%s/%s", prefix, filename);
         } else {
            strncpy(buf, filename, sizeof(buf));
         }
         if (fileExists(buf)) {
            fprintf(stderr, "Already existing file in ISB file: %s\n", buf);
            return UNTAR_ERR_ARCHIVE_FILE_EXISTS;
         }
         //fprintf(stderr, "Extracting file: %s\n", buf);
         if (tar_extract_file(t, buf)) {
            fprintf(stderr, "Unable to uncompress ISB file: %s\n", buf);
            return UNTAR_ERR_EXTRACTING_FILE;
         }

         int outcome = chmod(buf, mode);
         //fprintf(stderr, "chmod result: %d\n", outcome);
         if (outcome) {
            return UNTAR_ERR_EXTRACTING_FILE;
         }

      } else if (TH_ISLNK(t)) {

         // In case of DAG ZippedISB the item could be an hardlink
         if (prefix != NULL) {
            snprintf(buf, sizeof(buf), "%s/%s", prefix, filename);
         } else {
            strncpy(buf, filename, sizeof(buf));
         }
         // Extracting the hardlink
         if (tar_extract_hardlink(t, buf)) {
            fprintf(stderr, "Unable to uncompress ISB file: %s\n", buf);
            return UNTAR_ERR_EXTRACTING_FILE;
         }

         int outcome = chmod(buf, mode);
         if (outcome) {
            return UNTAR_ERR_EXTRACTING_FILE;
         }

      } else {
         fprintf(stderr, "Item in ISB file is not a regular file: %s\n",
                 filename);
      }
   }

   if (i != 1) {
      return UNTAR_ERR_EXTRACTING_FILE;
   }
   return UNTAR_ERR_NO_ERROR;
}

int
uncompressFile(char *file, char *prefix)
{
   //fprintf(stderr, "Uncompressing file: %s\n", file);
   //fprintf(stderr, "Starting path: %s\n", prefix);

   char buf[MAXPATHLEN];
   char * infile = NULL;
   char * outfile = NULL;

   int outcome = UNTAR_ERR_NO_ERROR;

   uInt len = (uInt)strlen(file);
   strcpy(buf, file);

   if (len > 3 && (strcmp(file + len - 3, ".gz") == 0)) {
      infile = file;
      outfile = buf;
      outfile[len - 3] = '\0';
   } else {
      outfile = file;
      infile = buf;
      strcat(infile, ".gz");
   }
   //fprintf(stderr, "Input file: %s\n", infile);
   //fprintf(stderr, "Output file: %s\n", outfile);

   int result = gzUncompress(infile, outfile);
   //fprintf(stderr, "unzip result: %d\n", result);
   if (result != Z_OK)  {
      return UNTAR_ERR_EXTRACTING_FILE;
   }

   TAR * tarfile = NULL;
   tar_open(&tarfile, outfile, NULL, O_RDONLY, S_IRWXU, TAR_GNU);
   outcome = chmod_tar_extract_all(tarfile, prefix);
   //fprintf(stderr, "Outcome: %d\n", outcome);
   tar_close(tarfile);

   remove(outfile);

   return outcome;
}

int
check_dir(char *dir, int opt_create, mode_t new_mode, gid_t new_group,
          uid_t create_uid)
{
   /* Eventually, change the permissions of the named directories
   Continue on errors, and switch on the appropriate status bits. */
   // used to manage return values:
   int ret;
   struct stat stat_result;
   //int summary_status = ADJUST_DIRECTORY_ERR_NO_ERROR;

   // testing that is a directory
   ret = stat(dir, &stat_result);
   if ((ret < 0) && (errno == ENOENT) && opt_create) {
      // Optionally create the directory in case it doesn't exist
      // Trying to create the directory
      if (mkdir(dir, new_mode) < 0) {
         fprintf(stderr, "Cannot create dir %s:%s\n", dir, strerror(errno));
         return ADJUST_DIRECTORY_ERR_MKDIR;
      } else {
         if (chown(dir,create_uid,new_group) < 0) {
            fprintf(stderr,"Cannot change owner of %s to %d: %s\n", dir, create_uid, strerror(errno));
            fprintf(stderr,"Trying to remove %s\n", dir);
            if (rmdir(dir) < 0) {
               fprintf(stderr, "Cannot remove %s: %s\n", dir, strerror(errno));
            }
            return ADJUST_DIRECTORY_ERR_CHOWN;
         }
      }
      //fprintf(stderr, "Created directory: %s (uid: %d gid: %d)\n",
      //dir, create_uid, new_group);
      ret = stat(dir, &stat_result);
   }
   if (ret < 0) {
      /* Stat of dir failed */
      fprintf(stderr, "Cannot access %s: %s\n", dir, strerror(errno));
      return ADJUST_DIRECTORY_ERR_STAT;
   }
   if (S_ISDIR(stat_result.st_mode)) {
      ret = chown(dir, -1, new_group);
      if (ret < 0) {
         fprintf(stderr, "Cannot change group of %s to 0%o: %s\n", dir,
                 new_group, strerror(errno));
         return ADJUST_DIRECTORY_ERR_CHOWN;
      }
      ret = chmod(dir, new_mode);
      if (ret < 0) {
         fprintf(stderr, "Cannot change mode of %s to 0%o: %s\n", dir,
                 new_mode, strerror(errno));
         return ADJUST_DIRECTORY_ERR_CHMOD;
      }
   } else {
      /* Not a directory */
      fprintf(stderr, "%s is not a directory. Skipping\n", dir);
      return ADJUST_DIRECTORY_ERR_NOT_A_DIR;
   }
   //return summary_status;
   return ADJUST_DIRECTORY_ERR_NO_ERROR;
}

int main(int argc, char *argv[])
{
   char *starting_path =NULL;
   int opt_verbose = FALSE;
   int opt_create  = FALSE;
   int untar = FALSE;
   uid_t create_uid =0;
   char *new_group_s = getenv("GLITE_USER")?getenv("GLITE_USER"):DEFAULT_ADJUST_DIRECTORY_GROUP;
   gid_t new_group;
   mode_t new_mode   = DEFAULT_ADJUST_DIRECTORY_MODE;
   int summary_status;
   long int st_result;
   char *end_ptr;
   char c;
   int i;
   if (argc < 2) {
      fprintf(stderr, ADJUST_DIRECTORY_USAGE, argv[0]);
      exit(ADJUST_DIRECTORY_ERR_OPTIONS);
   }
   initsignalhandler();
   while ((c = getopt(argc,argv,ADJUST_DIRECTORY_GETOPT_STRING)) != EOF) {
      switch (c) {
      case 'h':
         printf(ADJUST_DIRECTORY_USAGE, argv[0]);
         exit(ADJUST_DIRECTORY_ERR_NO_ERROR);

      case 'v':
         opt_verbose = TRUE;
         break;

      case 'c':
         st_result = strtol(optarg, &end_ptr, 0);
         if (*end_ptr == '\0') {
            create_uid = st_result;
            opt_create = TRUE;
         } else {
            fprintf(stderr, ADJUST_DIRECTORY_USAGE, argv[0]);
            exit(ADJUST_DIRECTORY_ERR_OPTIONS);
         }
         break;

#ifdef ALLOW_COMMAND_LINE_OVERRIDE

      case 'x': // If specified, argument files are extracted
         starting_path = strdup(optarg);
         untar = TRUE;
         break;
      case 'g':
         new_group_s = strdup(optarg);
         break;
      case 'm':
         st_result = strtol(optarg, &end_ptr, 0);
         if (*end_ptr == '\0') {
            new_mode = st_result;
         } else {
            fprintf(stderr,ADJUST_DIRECTORY_USAGE,argv[0]);
            exit(ADJUST_DIRECTORY_ERR_OPTIONS);
         }
         break;

#endif /* defined ALLOW_COMMAND_LINE_OVERRIDE */

      default:
         fprintf(stderr,ADJUST_DIRECTORY_USAGE,argv[0]);
         exit(1);
      } // End of switch
   } // End of getopt() loop */

   st_result = strtol(new_group_s, &end_ptr, 0);
   if (*end_ptr == '\0') {
      new_group = st_result;
   } else {
      struct group *gr_result;
      gr_result = getgrnam(new_group_s);
      if (gr_result != NULL) {
         new_group = gr_result->gr_gid;
      } else {
         /* Error in gr_result */
         if (errno == ENOMEM) {
            fprintf(stderr, "%s: Out of Memory.\n", argv[0]);
            exit(ADJUST_DIRECTORY_ERR_OUT_OF_MEMORY);
         } else {
            fprintf(stderr, "%s: Cannot find any group named %s.\n",
                    argv[0], new_group_s);
            exit(ADJUST_DIRECTORY_ERR_NO_SUCH_GROUP);
         }
      }
   }
   if (opt_verbose) {
      printf("%s: Changing directories to group %d and mode 0%o\n", argv[0],
             new_group, new_mode);
   }
   summary_status = ADJUST_DIRECTORY_ERR_NO_ERROR;

   if (untar) {
      // -x option is specified -> extracting ISB files...
      for (i = optind; i < argc; i++) {
         //fprintf(stderr,"Group before set: %d\n", getgid());
         //fprintf(stderr,"User before set: %d\n", getuid());
         //fprintf(stderr,"Group to set: %d\n", new_group);
         //fprintf(stderr,"User to set: %d\n", create_uid);
         if (setgid(new_group) || setuid(create_uid)) {
            fprintf(stderr, "Unable to set user/group: %s\n", strerror(errno));
            //summary_status = UNTAR_ERR_SETUID_SETGID;
            exit(UNTAR_ERR_SETUID_SETGID);
         } else {
            //fprintf(stderr,"Group after set: %d\n", getgid());
            //fprintf(stderr,"User after set: %d\n", getuid());
            //summary_status |= uncompressFile(argv[i], starting_path);
            #define BUF 1024
            char cmdline[BUF], arg1[BUF], arg2[BUF];
            snprintf(arg1, sizeof(arg1),"%s", starting_path);
            snprintf(arg2, sizeof(arg2),"%s", argv[i]);
            if (fileExists(argv[i])) {
               if ((summary_status =
                  execl(
                     "/bin/tar",
                     "tar",
                     "--directory",
                     arg1,
                     "-xzf",
                     arg2,
                     (char *)0
                  )
               )) {
                  exit(errno);
               }
            } else {
               fprintf(stderr,"Cannot read nor find file %s\n", argv[i]);
               exit(UNTAR_ERR_OPEN_FILE);
            }
         }
      }
   } else {
      // -x no specified -> creating directory...
      for (i = optind; i < argc; i++) {
         //summary_status |= check_dir(argv[i], opt_create, new_mode,
         // new_group, create_uid);
         if ((summary_status = check_dir(argv[i], opt_create, new_mode,
                                         new_group, create_uid))) {
            exit(summary_status);
         }
      }
   }
   /*if (summary_status != ADJUST_DIRECTORY_ERR_NO_ERROR) {
      printf("Warning!! some error occurred\n");
   }
   fprintf(stderr,"summary_status: %d\n", summary_status);
   exit(summary_status);*/
   exit(ADJUST_DIRECTORY_ERR_NO_ERROR);
}

