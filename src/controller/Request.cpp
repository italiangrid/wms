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
#include <iostream>
#include <string>

#include <classad_distribution.h>

#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"
#include "jobcontrol_namespace.h"
#include "Request.h"
#include "RequestExceptions.h"
#include "CondorG.h"

USING_COMMON_NAMESPACE;
using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

const char   *Request::r_s_commands[] = { "Unknown", "Submit", "Cancel", "CondorCancel" , "CondorRelease"};
const char   *Request::r_s_proto_version = "1.0.0";
const char   *Request::r_s_Arguments = "Arguments", *Request::r_s_Protocol = "Protocol", *Request::r_s_Command = "Command";
const char   *Request::r_s_Source = "Source";

const char   *SubmitRequest::sr_s_JobAd = "JobAd";

const char   *RemoveRequest::cr_s_JobId = "JobId";
const char   *RemoveRequest::cr_s_SequenceCode = "SequenceCode", *RemoveRequest::cr_s_LogFile = "LogFile";
const char   *RemoveRequest::cr_s_ProxyFile = "ProxyFile";

const char   *CondorRemoveRequest::crr_s_CondorId = "CondorId";
const char   *CondorRemoveRequest::crr_s_LogFile = "LogFile";

const char   *CondorReleaseRequest::crr_s_CondorId = "CondorId";
const char   *CondorReleaseRequest::crr_s_LogFile = "LogFile";

const char *Request::string_command( request_code_t command )
{
  if( (command < unknown) || (command >= __last_command) )
    command = unknown;

  return r_s_commands[static_cast<int>(command)];
}

void Request::finalClassAdSet( void )
{
  this->r_arguments = dynamic_cast<classad::ClassAd *>( this->r_request->Lookup(r_s_Arguments) );

  if( !this->r_arguments ) throw MalformedRequest( *this->r_request );

  this->checkProtocol();

  return;
}

void Request::checkRequest( void ) const
{
  if( (this->r_arguments == NULL) || (this->r_request.get() == NULL) )
    throw UninitializedRequest();

  return;
}

void Request::checkProtocol( void ) const
{
  string      current( this->get_protocol() ), def( r_s_proto_version );

  if( current != def )
    throw MismatchedProtocol( def, current );

  return;
}

Request::Request( void ) : r_arguments( NULL ), r_request() {}

Request::Request(const classad::ClassAd &ad)
  : r_arguments(NULL),
    r_request(static_cast<classad::ClassAd*>(ad.Copy()))
{
  this->finalClassAdSet();
}

Request::Request(const Request &r)
  : r_arguments(NULL),
    r_request(static_cast<classad::ClassAd*>(r.r_request->Copy()))
{
  this->r_arguments = dynamic_cast<classad::ClassAd *>( this->r_request->Lookup(r_s_Arguments) );
}

Request::Request( request_code_t command, int source ) : r_arguments( new classad::ClassAd ), r_request( new classad::ClassAd )
{
  this->r_request->InsertAttr( r_s_Source, source );
  this->r_request->InsertAttr( r_s_Protocol, string(r_s_proto_version) );
  this->r_request->InsertAttr( r_s_Command, string(r_s_commands[static_cast<int>(command)]) );
  this->r_request->Insert( r_s_Arguments, this->r_arguments );
}

Request::~Request( void ) {}

Request &Request::operator=( const Request &that )
{
  if( this != &that ) {
    this->r_request.reset(static_cast<classad::ClassAd*>(that.r_request->Copy()));
    this->r_arguments = dynamic_cast<classad::ClassAd *>( this->r_request->Lookup(r_s_Arguments) );
  }

  return *this;
}

Request &Request::reset( const classad::ClassAd &ad )
{
  this->r_request.reset(static_cast<classad::ClassAd*>(ad.Copy()));

  this->finalClassAdSet();

  return *this;
}

int Request::get_source( void ) const
{
  int     source;

  this->checkRequest();

  if( !this->r_arguments->EvaluateAttrInt(r_s_Source, source) )
    throw MalformedRequest( *this->r_request );

  return source;
}

Request::request_code_t Request::get_command( void ) const
{
  int       com;
  string    scom( this->get_string_command() );

  for( com = static_cast<int>(unknown); com < static_cast<int>(__last_command); ++com )
    if( scom == string(r_s_commands[com]) )
      break;

  if( com == static_cast<int>(__last_command) )
    com = static_cast<int>( unknown );

  return static_cast<request_code_t>( com );
}

string Request::get_protocol( void ) const
{
  string        protocol;

  this->checkRequest();

  if( !this->r_request->EvaluateAttrString(r_s_Protocol, protocol) )
    protocol.assign( "0.0.1" );

  return protocol;
}

string Request::get_string_command( void ) const
{
  string    command;

  this->checkRequest();

  if( !this->r_request->EvaluateAttrString(r_s_Command, command) )
    command.assign( r_s_commands[static_cast<int>(unknown)] );

  return command;
}

SubmitRequest::SubmitRequest( const classad::ClassAd &job, int source ) : Request( submit, source )
{
  this->r_arguments->Insert( sr_s_JobAd, job.Copy() );
}

SubmitRequest::~SubmitRequest( void ) {}

void SubmitRequest::set_sequence_code( const string &code )
{
  classad::ClassAd     *jobad = dynamic_cast<classad::ClassAd *>( this->r_arguments->Lookup(sr_s_JobAd) );

  if( jobad ) glite::jdl::set_lb_sequence_code( *jobad, code );
  else throw MalformedRequest( *this->r_request );

  return;
}

const classad::ClassAd *SubmitRequest::get_jobad( void ) const
{
  classad::ClassAd     *jobad;

  this->checkProtocol();

  jobad = dynamic_cast<classad::ClassAd *>( this->r_arguments->Lookup(sr_s_JobAd) );

  if( jobad == NULL ) throw MalformedRequest( *this->r_request );

  return jobad;
}

RemoveRequest::RemoveRequest( const string &jobid, int source ) : Request( remove, source )
{
  this->r_arguments->InsertAttr( cr_s_JobId, jobid );
}

RemoveRequest::~RemoveRequest( void ) {}

RemoveRequest &RemoveRequest::set_sequence_code( const string &code )
{
  this->r_arguments->InsertAttr( cr_s_SequenceCode, code );

  return *this;
}

RemoveRequest &RemoveRequest::set_logfile( const string &logfile )
{
  this->r_arguments->InsertAttr( cr_s_LogFile, logfile );

  return *this;
}

RemoveRequest &RemoveRequest::set_proxyfile( const string &proxyfile )
{
  this->r_arguments->InsertAttr( cr_s_ProxyFile, proxyfile );

  return *this;
}

string RemoveRequest::get_jobid( void ) const
{
  string     jobid;

  this->checkProtocol();

  if( !this->r_arguments || !this->r_arguments->EvaluateAttrString(cr_s_JobId, jobid) )
    throw MalformedRequest( *this->r_request );

  return jobid;
}

string RemoveRequest::get_sequence_code( void ) const
{
  string     code;

  this->checkProtocol();

  if( !this->r_arguments || !this->r_arguments->EvaluateAttrString(cr_s_SequenceCode, code) )
    throw MalformedRequest( *this->r_request );

  return code;
}

string RemoveRequest::get_logfile( void ) const
{
  string   file;

  this->checkProtocol();

  if( this->r_arguments ) {
    if( !this->r_arguments->EvaluateAttrString(cr_s_LogFile, file) )
      file.erase();
  }
  else throw MalformedRequest( *this->r_request );

  return file;
}

string RemoveRequest::get_proxyfile( void ) const
{
  string   file;

  this->checkProtocol();

  if( this->r_arguments ) {
    if( !this->r_arguments->EvaluateAttrString(cr_s_ProxyFile, file) )
      file.erase();
  }
  else throw MalformedRequest( *this->r_request );

  return file;
}

CondorReleaseRequest::CondorReleaseRequest(int condorid, int source)
  : Request(condorrelease, source)
{
  this->r_arguments->InsertAttr(crr_s_CondorId, condorid);
}

CondorReleaseRequest::~CondorReleaseRequest() { }

CondorReleaseRequest&
CondorReleaseRequest::set_logfile(string const& logfile)
{
  this->r_arguments->InsertAttr( crr_s_LogFile, logfile );

  return *this;
}

int CondorReleaseRequest::get_condorid() const
{
  int  condorid = 0;
  this->checkProtocol();
  if( !this->r_arguments || !this->r_arguments->EvaluateAttrInt(crr_s_CondorId, condorid) ) {
    throw MalformedRequest( *this->r_request );
  }

  return condorid;
}

std::string CondorReleaseRequest::get_logfile( void ) const
{
  string   file;

  this->checkProtocol();
  if( this->r_arguments ) {
    if( !this->r_arguments->EvaluateAttrString(crr_s_LogFile, file) )
      file.erase();
  }
  else throw MalformedRequest( *this->r_request );

  return file;
}

CondorRemoveRequest::CondorRemoveRequest( int condorid, int source ) : Request( condorremove, source )
{
  this->r_arguments->InsertAttr( crr_s_CondorId, condorid );
}

CondorRemoveRequest::~CondorRemoveRequest( void ) {}

CondorRemoveRequest&
CondorRemoveRequest::set_logfile( const string &logfile )
{
  this->r_arguments->InsertAttr( crr_s_LogFile, logfile );

  return *this;
}

int CondorRemoveRequest::get_condorid( void ) const
{
  int   condorid;

  this->checkProtocol();

  if( !this->r_arguments || !this->r_arguments->EvaluateAttrInt(crr_s_CondorId, condorid) )
    throw MalformedRequest( *this->r_request );

  return condorid;
}

string CondorRemoveRequest::get_logfile( void ) const
{
  string   file;

  this->checkProtocol();

  if( this->r_arguments ) {
    if( !this->r_arguments->EvaluateAttrString(crr_s_LogFile, file) )
      file.erase();
  }
  else throw MalformedRequest( *this->r_request );

  return file;
}

} // Namespace controller

} JOBCONTROL_NAMESPACE_END
