#ifndef EDG_WORKLOAD_JOBCONTROL_JCCOMMON_LOCKFILE_H
#define EDG_WORKLOAD_JOBCONTROL_JCCOMMON_LOCKFILE_H

#include <string>

namespace boost { namespace filesystem {
  class path;
}}

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

}; // Namespace jccommon

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_JCCOMMON_LOCKFILE_H */

// Local Variables:
// mode: c++
// End:
