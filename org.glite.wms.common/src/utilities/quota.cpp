/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/quota.h>
#include <pwd.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <mntent.h>
#include <cstring>
#include <cstdio>
#include <string>

#ifdef B_THREAD_SAFE
#include <boost/thread/mutex.hpp>
#endif


#ifndef QUOTABLOCK_BITS
/* Conservatively assume 512-bytes blocks */
#define QUOTABLOCK_BITS 9
#endif

__BEGIN_DECLS
extern int quotactl (int __cmd, const char *__special, int __id,
		     caddr_t __addr) __THROW;
__END_DECLS

#define qinfo(dbstr, name)  std::cout << #name << ":\t" << dbstr.name << std::endl;

namespace glite {
namespace wms {
namespace common {
namespace utilities {
namespace quota {

#ifdef B_THREAD_SAFE
static boost::mutex f_mnt_mutex;
#endif

uid_t user2uid(const char *name)
{
  struct passwd *entry;
  uid_t ret;
  char *errch;
  
  ret = strtol(name, &errch, 0);
  if (!*errch)
    return ret;
  if (!(entry = getpwnam(name))) {
    std::cerr << "User " << name << " doesn't exist.\n";
    return 0;
  }
  return entry->pw_uid;
}

bool user2home(const std::string &uname, std::string &homedir) {
  int uid = user2uid(uname.c_str());
  struct passwd *pw;

  if (!(pw=getpwuid(uid))) return false;
  
  homedir=std::string(pw -> pw_dir);
  return true;
}


bool file2device(const std::string &filename, std::string &device ) 
{
#ifdef B_THREAD_SAFE
  boost::mutex::scoped_lock lk(f_mnt_mutex);
#endif
  struct stat st, st2;
  if (stat(filename.c_str(), &st) == -1) return false;
  FILE *fp;
  fp = setmntent(MOUNTED, "r");
  struct mntent *mnt;
  while ( (mnt=getmntent(fp)) ) {
    if (memcmp(mnt->mnt_fsname, "/dev/", 5)) continue;
    if (stat(mnt->mnt_fsname, &st2) == -1) continue;
    if (st.st_dev == st2.st_rdev) {
      device = std::string(mnt->mnt_fsname);
      ::fclose(fp);
      return true;
    }
  }
  ::fclose(fp);
  return false;
}

std::pair<long, long> beGrateful2Me4Ever(const std::string &uname, bool totalquota) {

  long sftlmt = -1;
  long hrdlmt = -1;

  struct dqblk dbstr;
  bzero(&dbstr, sizeof(struct dqblk));
  
  std::string device;
  std::string homedir;
  
  if (user2home(uname, homedir)) {
    if (file2device(homedir, device)) {
      long result = quotactl(QCMD(Q_GETQUOTA, USRQUOTA), 
			     device.c_str(), 
			     user2uid(uname.c_str()), 
			     (caddr_t)&dbstr);
  
      if ( !result ) {
	if (totalquota) {
	  sftlmt=(dbstr.dqb_bsoftlimit << QUOTABLOCK_BITS);
	  hrdlmt=(dbstr.dqb_bhardlimit << QUOTABLOCK_BITS);
	} else {
#if _LINUX_QUOTA_VERSION < 2
	  sftlmt=(dbstr.dqb_bsoftlimit << QUOTABLOCK_BITS)- dbstr.dqb_curblocks;
	  hrdlmt=(dbstr.dqb_bhardlimit << QUOTABLOCK_BITS)- dbstr.dqb_curblocks;
#elif _LINUX_QUOTA_VERSION == 2
	  sftlmt=(dbstr.dqb_bsoftlimit << QUOTABLOCK_BITS)- dbstr.dqb_curspace;
	  hrdlmt=(dbstr.dqb_bhardlimit << QUOTABLOCK_BITS)- dbstr.dqb_curspace;
#endif
	}
      }
    }
  }
  return std::make_pair( sftlmt, hrdlmt);
}

std::pair<long, long> getFreeQuota(const std::string &uname) {
  return beGrateful2Me4Ever(uname, false);
}

std::pair<long, long> getQuota(const std::string &uname) {
  return beGrateful2Me4Ever(uname, true);
}
 
} // namespace quota
} // namespace utilities
} // namespace common
} // namespace wms
} // glite

