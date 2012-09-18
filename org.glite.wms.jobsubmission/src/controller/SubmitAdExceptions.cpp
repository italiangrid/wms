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
#include "jobcontrol_namespace.h"

#include "SubmitAdExceptions.h"

using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

SubmitAdException::SubmitAdException( void ) {}

SubmitAdException::~SubmitAdException( void ) throw() {}

CannotOpenStatusFile::CannotOpenStatusFile( const string &path, int mode ) : SubmitAdException(),
									     cosf_mode( mode ), cosf_path( path )
{}

CannotOpenStatusFile::~CannotOpenStatusFile( void ) throw() {}

const char *CannotOpenStatusFile::what( void ) const throw()
{
  this->sae_what.assign( "Cannot open status file \"" );
  this->sae_what.append( this->cosf_path );

  if( this->cosf_mode ) this->sae_what.append( "\" for writing." );
  else this->sae_what.append( "\" for reading." );

  return this->sae_what.c_str();
}

FileSystemError::FileSystemError( const char *error ) : SubmitAdException(), fse_error( error ? error : "" )
{}

FileSystemError::~FileSystemError( void ) throw() {}

const char *FileSystemError::what( void ) const throw()
{
  this->sae_what.assign( "boost::filesystem error: \"" );
  this->sae_what.append( this->fse_error );
  this->sae_what.append( "\"." );

  return this->sae_what.c_str();
}

CannotCreateDirectory::CannotCreateDirectory( const char *dirType, const string &path, const char *reason ) : SubmitAdException(), 
  ccd_path( path ), ccd_dirType( dirType ? dirType : "" ), ccd_reason( reason ? reason : "" )
{}

CannotCreateDirectory::~CannotCreateDirectory( void ) throw() {}

const char *CannotCreateDirectory::what( void ) const throw()
{
  this->sae_what.assign( "Failed to create " );
  this->sae_what.append( this->ccd_dirType );
  this->sae_what.append( " path: \"" );
  this->sae_what.append( this->ccd_path );
  this->sae_what.append( "\". Reason: \"" );
  this->sae_what.append( this->ccd_reason );
  this->sae_what.append( "\"." );

  return this->sae_what.c_str();
}

} // Namespace controller

} JOBCONTROL_NAMESPACE_END
