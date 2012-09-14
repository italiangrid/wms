/* Copyright (c) Members of the EGEE Collaboration. 2004.
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
limitations under the License. */
#ifndef EDG_WORKLOAD_JOBCONTROL_JCCOMMON_LOCKFILE_H
#define EDG_WORKLOAD_JOBCONTROL_JCCOMMON_LOCKFILE_H

#include <string>
#include <boost/filesystem/path.hpp>

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

class LockFile {
public:
  LockFile( const std::string &filename );
  LockFile( const boost::filesystem::path &file );
  ~LockFile( void );

  inline bool good( void ) const { return( this->lf_good && !this->lf_error ); }
  inline bool error( void ) const { return this->lf_error; }
  inline const boost::filesystem::path &file( void ) const { return *this->lf_file; }

  inline operator bool( void ) const { return( this->lf_good && !this->lf_error ); }
  inline bool operator !( void ) const { return( !this->lf_good || this->lf_error ); }

  void reset_pid( void );
  void remove( void ) const;

private:
  LockFile( void );                         //
  LockFile( const LockFile & );             //  Unimplemented
  LockFile &operator=( const LockFile & );  //

  void checkLockFile( void );

  bool                      lf_good, lf_error;
  boost::filesystem::path  *lf_file;
};

} // Namespace jccommon

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_JCCOMMON_LOCKFILE_H */

// Local Variables:
// mode: c++
// End:
