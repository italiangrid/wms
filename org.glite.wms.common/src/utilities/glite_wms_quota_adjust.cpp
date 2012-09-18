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
 /*
  * File: quota_adjust
  * Author: Marco Pappalardo
  * Copyright (c) 2003 EU DataGrid.
  *      
  */ 

#include <iostream>
#include <string>

#include "classad_distribution.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "configuration/NSConfiguration.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

extern "C" {
#include <pwd.h>
#include <mntent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>

#ifdef QUOTACTL_INCLUDE_LINUX
#include <linux/types.h>
#include <linux/quota.h>
#else
#include <sys/quota.h>
#endif
}

#ifndef QUOTABLOCK_BITS
/* Conservatively assume 512-bytes blocks */
#define QUOTABLOCK_BITS 9
#endif

namespace configuration = glite::wms::common::configuration;

void usage(std::string exe)
{
 std::cerr << "Usage: " << exe << 
   " -q [quota adjustment in blocks - defaults to NS config value] -p device_path UID" 
   << std::endl;
 exit(1);
}

int main(int argc, char* argv[])
{
 std::string ex_name = std::string(argv[0]);

 if (argc < 2) usage(ex_name);
   
 int target_uid = atoi(argv[argc-1]);
 if (target_uid <= 0)
  {
   std::cerr << ex_name << ": Invalid user or superuser. Cannot change quota." 
     << std::endl;
   exit(2);
  }

 int quota_adjustment = 0;
 std::string device_path;
 bool found_q = false, found_p = false;
 bool decrease_quota = false;

 for (int carg=1; carg < (argc-1); carg++)
  {
   if (strcmp(argv[carg],"-q")==0)
    {
     carg++;
     if (carg >= (argc-1)) usage(ex_name);
     int tentative_quota;
     if (strcmp(argv[carg],"-")==0) 
      {
       decrease_quota = true;
       continue;
      }
     else tentative_quota = atoi(argv[carg]);
     if (tentative_quota == 0)
      {
       std::cerr << ex_name << 
         ": Requested a zero or invalid quota amount. Defaulting to config quota." 
         << std::endl;
      }
     else 
      {
       quota_adjustment=tentative_quota;
       found_q = true;
      }
    }
   else if (strcmp(argv[carg],"-p")==0)
    {
     carg++;
     if (carg >= (argc-1)) usage(ex_name);
     device_path = std::string(argv[carg]);
     found_p = true;
    }
   else usage(ex_name);
  }

 if (!found_p || !found_q)
  {
   // Need at least one parameter from GLITE WMS configuration
   // Let's go and fetch it.

   configuration::Configuration *conf;
   const configuration::NSConfiguration *nsconf;
   try
    {
     conf = new configuration::Configuration("glite_wms.conf", "NetworkServer");
     nsconf = configuration::Configuration::instance()->ns();
    }
   catch (...)
    {
     std::cerr << ex_name << ": Error reading GLITE-WMS configuration. Exiting." 
       << std::endl;
     exit(17);
    }

   if (!found_q) quota_adjustment = nsconf->quota_adjustment_amount();
   //if (!found_q) quota_adjustment=10000;
   if (decrease_quota) quota_adjustment = -abs(quota_adjustment);

   if (!found_p)
    {
     struct stat st_st;
     std::string st_path(nsconf->sandbox_staging_path());
     if (stat(st_path.c_str(), &st_st) < 0)
      {
       std::cerr << ex_name << ": Error getting information about " <<
         st_path << " - " << strerror(errno) << ". Exiting" <<
         std::endl;
       exit(9);
      }

     FILE *mnt_fp;
     struct mntent *target_mount;
     struct stat dev_st;
     mnt_fp = setmntent(MOUNTED, "r");
     while ( (target_mount=getmntent(mnt_fp)) != NULL )
      {
       if (stat(target_mount->mnt_fsname, &dev_st) < 0) continue; 
       if (dev_st.st_rdev == st_st.st_dev) break;
      }

     if (target_mount == NULL)
      {
       std::cerr << ex_name << ": Failed to find mount information for " <<
         st_path << ". Exiting." << std::endl;
       exit(10);
      }

     device_path = std::string(target_mount->mnt_fsname);
     endmntent(mnt_fp);

     if ((dev_st.st_mode & S_IFBLK) == 0)
      {
       std::cerr << ex_name << ": Error. " << device_path << 
         " is not a block device. Cannot handle quotas. Exiting" << std::endl;
       exit(11);
      }
    }
  }

 if (quota_adjustment == 0)
  {
   std::cerr << ex_name << 
     ": Requested a zero or invalid quota amount. Cannot change quota." 
     << std::endl;
   exit(3);
  }


 struct passwd *cur_entry;
#ifdef QUOTACTL_INCLUDE_LINUX
 struct mem_dqblk   quota_entry;
 struct mem_dqblk   target_quota_entry;
#else
 struct dqblk   quota_entry;
 struct dqblk   target_quota_entry;
#endif
 bool found_target = false;
 unsigned long total_allocated_quota_blocks = 0;
 if (quota_adjustment > 0)
  {
   // Compute the grand total of hard quotas allocated on the requested device
   setpwent();
   while ((cur_entry = getpwent()) != NULL)
    {
     if ( quotactl(QCMD(Q_GETQUOTA, USRQUOTA),
                   device_path.c_str(),
                   cur_entry->pw_uid,
                   (caddr_t)&quota_entry) >= 0)
      {
       total_allocated_quota_blocks += quota_entry.dqb_bhardlimit;
       if (cur_entry->pw_uid == (uid_t)target_uid) 
        {
         memcpy(&target_quota_entry,&quota_entry,sizeof(target_quota_entry));
         found_target = true;
        }
      }
     else
      {
       std::cerr << ex_name << ": Warning cannot obtain quota of UID " 
         << cur_entry->pw_uid << ":" << strerror(errno) 
         << "." << std::endl;
      }
    }
   endpwent();
  }
 else
  {
   if ( quotactl(QCMD(Q_GETQUOTA, USRQUOTA),
                 device_path.c_str(),
                 target_uid,
                 (caddr_t)&target_quota_entry) >= 0) found_target = true;
  }

 if (!found_target)
  { 
   std::cerr << ex_name << ": Cannot obtain quota of UID " << target_uid 
     << ": " << strerror(errno) << ". Exiting." << std::endl;
   exit(4);
  }
 
// A few sanity checks.
 if (quota_adjustment < 0)
  {
   if (target_quota_entry.dqb_bhardlimit < (unsigned)abs(quota_adjustment))
    {
     std::cerr << ex_name << ": UID " << target_uid << "'s quota is just " <<
       target_quota_entry.dqb_bhardlimit <<
       "<"<<(-quota_adjustment)<<". Exiting." << std::endl;
     exit(5);
    }
  }
 else
  {
   // Find mount point of requested device, so that we can
   // obtain the available disk space.

   FILE *mnt_fp;
   struct mntent *target_mount;
   mnt_fp = setmntent(MOUNTED, "r");
   while ( (target_mount=getmntent(mnt_fp)) != NULL ) 
    {
     if (strcmp(target_mount->mnt_fsname, device_path.c_str()) == 0) break;
    }

   if (target_mount == NULL)
    {
     std::cerr << ex_name << ": Failed to find mount information for " <<
       device_path << ". Exiting.";
     exit(12);
    }

   struct statfs target_filesystem_stats;
   if (statfs(target_mount->mnt_dir, &target_filesystem_stats) < 0)
    {
     std::cerr << ex_name << ": Cannot obtain FS stats of " << 
       std::string(target_mount->mnt_dir)
       << ": " << strerror(errno) << ". Exiting." << std::endl;
     exit(6);
    }

   endmntent(mnt_fp);

   int block_multiplier = target_filesystem_stats.f_bsize >> QUOTABLOCK_BITS;
   if (block_multiplier <= 0) block_multiplier = 1;
   int fs_total_blocks = target_filesystem_stats.f_blocks * block_multiplier;
   int left_blocks = fs_total_blocks-total_allocated_quota_blocks;
   if ( left_blocks < quota_adjustment )
    {
     std::cerr << ex_name << ": Non-allocated blocks left on " <<
       device_path << " = " << left_blocks <<
     "<"<<quota_adjustment<<". Exiting." << std::endl;
     exit(7);
    }
  }

// Now do adjust the quota.
 target_quota_entry.dqb_bhardlimit += quota_adjustment;

 if ( quotactl(QCMD(Q_SETQUOTA, USRQUOTA),
               device_path.c_str(),
               target_uid,
               (caddr_t)&target_quota_entry) < 0)
  {
   std::cerr << ex_name << ": Cannot adjust quota of UID " << target_uid 
     << ": " << strerror(errno) << ". Exiting." << std::endl;
   exit(8);
  }

 exit(0);
 
}

