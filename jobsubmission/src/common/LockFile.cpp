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
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

#include "jobcontrol_namespace.h"
#include "glite/wms/common/utilities/boost_fs_add.h"

#include "LockFile.h"

using namespace std;
namespace fs = boost::filesystem;
namespace utilities = glite::wms::common::utilities;

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
  lf_file( new fs::path(utilities::normalize_path(filename), fs::native) )
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

} // namespace jccommon

} JOBCONTROL_NAMESPACE_END
