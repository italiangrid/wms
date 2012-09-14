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

}; // Namespace controller

} JOBCONTROL_NAMESPACE_END;
