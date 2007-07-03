#include <string>

#include "jobcontrol_namespace.h"

#include "JobControllerExceptions.h"

using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

ControllerError::ControllerError( void ) : exception(), ce_what() {}

ControllerError::~ControllerError( void ) throw() {}

const char *ControllerError::what( void ) const throw()
{
  this->ce_what.assign( this->reason() );

  return this->ce_what.c_str();
}

CannotCreate::CannotCreate( const string &reason ) : cc_reason( reason )
{}

CannotCreate::~CannotCreate( void ) throw() {}

const string &CannotCreate::reason( void ) const
{ return this->cc_reason; }

CannotExecute::CannotExecute( const string &reason ) : ce_reason( reason )
{}

CannotExecute::~CannotExecute( void ) throw() {}

const string &CannotExecute::reason( void ) const
{ return this->ce_reason; }

}; // namespace controller

} JOBCONTROL_NAMESPACE_END;
