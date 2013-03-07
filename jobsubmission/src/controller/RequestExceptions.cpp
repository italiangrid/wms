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

} // Namespace controller

} JOBCONTROL_NAMESPACE_END
