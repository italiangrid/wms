#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

#include "../jobcontrol_namespace.h"
#include "glite/wms/common/utilities/boost_fs_add.h"

#include "LockFile.h"

using namespace std;
namespace fs = boost::filesystem;

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

void LockFile::checkLockFile( void )
{
  if( !fs::exists(*this->lf_file) ) {
    ofstream   ofs( this->lf_file->native_file_string().c_str(), ios::out );

    if( !(this->lf_error = (!ofs.good() || ofs.bad())) ) {
      this->lf_good = true;

      ofs << ::getpid() << endl;
    }
  }

  return;
}

LockFile::LockFile( const string &filename ) : lf_good( false ), lf_error( false ),
  lf_file( new fs::path(fs::normalize_path(filename), fs::native) )
{
  this->checkLockFile();
}

LockFile::LockFile( const fs::path &file ) : lf_good( false ), lf_error( false ), lf_file( new fs::path(file) )
{
  this->checkLockFile();
}

LockFile::~LockFile( void )
{
  delete this->lf_file;
}

void LockFile::remove( void ) const
{
  try {
    if( this->good() ) fs::remove( *this->lf_file );
  }
  catch( ... ) {
    /* Simply ignore any error coming from downside */
  }

  return;
}

void LockFile::reset_pid( void )
{
  if( this->good() ) {
    ofstream   ofs( this->lf_file->native_file_string().c_str(), ios::out );

    if( !(this->lf_error = (!ofs.good() || ofs.bad())) ) {
      this->lf_good = true;

      ofs << ::getpid() << endl;
    }
    else this->lf_good = false;
  }

  return;
}

}; // namespace jccommon

} JOBCONTROL_NAMESPACE_END;
