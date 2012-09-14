#include <string>

#include "jobcontrol_namespace.h"
#include "exceptions.h"

using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {

namespace daemons {

DaemonError::DaemonError( const string &reason ) : exception(), de_reason( reason ) {}

DaemonError::~DaemonError( void ) throw() {}

const char *DaemonError::what( void ) const throw()
{
  return this->de_reason.c_str();
}

CannotStart::CannotStart( const string &reason ) : DaemonError( reason ) {}

CannotStart::~CannotStart( void ) throw() {}

CannotExecute::CannotExecute( const string &reason ) : DaemonError( reason ) {}

CannotExecute::~CannotExecute( void ) throw() {}

}; // Namespace daemons

} JOBCONTROL_NAMESPACE_END;

