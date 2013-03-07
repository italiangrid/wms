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
#include <cstring>

#include "exceptions.h"

using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor {

MonitorException::MonitorException( void ) : exception() {}

MonitorException::~MonitorException( void ) throw() {}

const char *MonitorException::what( void ) const throw()
{
  this->me_what.assign( this->reason() );

  return this->me_what.c_str();
}

CannotOpenFile::CannotOpenFile( const std::string &filename, int error ) : MonitorException(),
									   cof_errno( error ), cof_name( filename )
{}

CannotOpenFile::~CannotOpenFile( void ) throw() {}

string CannotOpenFile::reason( void ) const
{
  string     reason( "Cannot open file: \"" );

  reason.append( this->cof_name );

  if( this->cof_errno != 0 ) {
    reason.append( "\", reason: \"" );
    reason.append( strerror(this->cof_errno) );
  }

  reason.append( "\"." );

  return reason;
}

FileSystemError::FileSystemError( const string &error ) : MonitorException(), fse_error( error ) {}

FileSystemError::~FileSystemError( void ) throw() {}

string FileSystemError::reason( void ) const
{
  string     reason( "Error during file access \"" );

  reason.append( this->fse_error ); reason.append( "\"." );

  return reason;
}

CannotExecute::CannotExecute( const string &reason ) : ce_reason( reason ) {}

CannotExecute::~CannotExecute( void ) throw() {}

string CannotExecute::reason( void ) const
{
  string    reason( "Error while running logmonitor: \"" );

  reason.append( this->ce_reason ); reason.append( "\"." );

  return reason;
}

InvalidJobId::InvalidJobId( const string &id ) : iji_id( id ) {}

InvalidJobId::~InvalidJobId( void ) throw() {}

string InvalidJobId::reason( void ) const
{
  string    reason( "Passed condor cluster \"" );

  reason.append( this->iji_id ); reason.append( "\" not valid for LogMonitor." );

  return reason;
}

InvalidLogFile::InvalidLogFile( const string &reason ) : ilf_reason( reason ) {}

InvalidLogFile::~InvalidLogFile( void ) throw() {}

string InvalidLogFile::reason( void ) const
{
  string    reason( "Log file contains an error: \"" );

  reason.append( this->ilf_reason ); reason.append( "\"" );

  return reason;
}

InvalidFileName::InvalidFileName( const string &filename ) : ifn_filename( filename ) {}

InvalidFileName::~InvalidFileName( void ) throw() {}

string InvalidFileName::reason( void ) const
{
  string    reason( "Passed file name \"" );

  reason.append( this->ifn_filename );
  reason.append( "\" is not valid." );

  return reason;
}

} // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END
