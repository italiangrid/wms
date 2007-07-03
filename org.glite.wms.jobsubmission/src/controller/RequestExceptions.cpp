#include <string>

#include <classad_distribution.h>

#include "jobcontrol_namespace.h"
#include "RequestExceptions.h"

using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

RequestException::RequestException( void ) : exception(), re_what() {}

RequestException::~RequestException( void ) throw() {}

MalformedRequest::MalformedRequest(const classad::ClassAd &ad)
  : mr_ad(static_cast<classad::ClassAd*>(ad.Copy()))
{
}

MalformedRequest::~MalformedRequest( void ) throw()
{
  delete this->mr_ad;
}

const char *MalformedRequest::what( void ) const throw()
{
  string                     ad;
  classad::ClassAdUnParser   unparser;

  this->re_what.assign( "Malformed JobController request, classad = " );

  unparser.Unparse( ad, const_cast<classad::ClassAd *>(this->mr_ad) );
  this->re_what.append( ad );

  return this->re_what.c_str();
}

UninitializedRequest::UninitializedRequest( void ) {}

UninitializedRequest::~UninitializedRequest( void ) throw() {}

const char *UninitializedRequest::what( void ) const throw()
{
  return "Operation attempted on an uninitialized JobController request.";
}

MismatchedProtocol::MismatchedProtocol( const string &def, const string &current ) : mp_default( def ), mp_current( current )
{}

MismatchedProtocol::~MismatchedProtocol( void ) throw() {}

const char *MismatchedProtocol::what( void ) const throw()
{
  this->re_what.assign( "Get request for a different JobController protocol, needed \"" );

  this->re_what.append( this->mp_default ); this->re_what.append( "\", got \"" );
  this->re_what.append( this->mp_current ); this->re_what.append( "\"." );

  return this->re_what.c_str();
}

}; // Namespace controller

} JOBCONTROL_NAMESPACE_END;
