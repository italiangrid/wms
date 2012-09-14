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
#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_REQUEST_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_REQUEST_H

#include <string>
#include <memory>

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class Request {
public:
  enum request_code_t { unknown, submit, remove, condorremove, condorrelease, __last_command };

  Request( void );
  Request( const classad::ClassAd &ad );
  Request( const Request &r );
  virtual ~Request( void );

  Request &operator=( const Request &r );
  inline Request &operator=( const classad::ClassAd &ad ) { return this->reset( ad ); }
  inline operator const classad::ClassAd &( void ) const { return *this->r_request; }
  Request &reset( const classad::ClassAd &ad );

  int get_source( void ) const;
  request_code_t get_command( void ) const;
  std::string get_protocol( void ) const;
  std::string get_string_command( void ) const;

  inline bool check_protocol( void ) const { return( this->get_protocol() == std::string(r_s_proto_version) ); }
  inline const classad::ClassAd &get_arguments( void ) const { return *this->r_arguments; }
  inline const classad::ClassAd &get_request( void ) const { return *this->r_request; }

  static const char *string_command( request_code_t command );

protected:
  Request( request_code_t command, int source );

  void finalClassAdSet( void );
  void checkRequest( void ) const;
  void checkProtocol( void ) const;

  classad::ClassAd                  *r_arguments;
  std::auto_ptr<classad::ClassAd>    r_request;

  static const char    *r_s_commands[];
  static const char    *r_s_proto_version;
  static const char    *r_s_Arguments, *r_s_Protocol, *r_s_Command;
  static const char    *r_s_Source;
};

class SubmitRequest : public Request {
public:
  SubmitRequest( const classad::ClassAd &job, int source );
  virtual ~SubmitRequest( void );

  void set_sequence_code( const std::string &code );

  const classad::ClassAd *get_jobad( void ) const;

private:
  static const char    *sr_s_JobAd;
};

class RemoveRequest : public Request {
public:
  RemoveRequest( const std::string &jobid, int source );
  virtual ~RemoveRequest( void );

  RemoveRequest &set_sequence_code( const std::string &code );
  RemoveRequest &set_logfile( const std::string &logfile );
  RemoveRequest &set_proxyfile( const std::string &proxyfile );

  std::string get_jobid( void ) const;
  std::string get_sequence_code( void ) const;
  std::string get_logfile( void ) const;
  std::string get_proxyfile( void ) const;

private:
  static const char    *cr_s_JobId;
  static const char    *cr_s_SequenceCode, *cr_s_LogFile, *cr_s_ProxyFile;
};

class CondorReleaseRequest : public Request {
public:
  CondorReleaseRequest(int condorid, int source );
  virtual ~CondorReleaseRequest();

  CondorReleaseRequest& set_logfile(std::string const& logfile);

  int get_condorid() const;
  std::string get_logfile() const;

private:
  static const char   *crr_s_CondorId, *crr_s_LogFile;
};


class CondorRemoveRequest : public Request {
public:
  CondorRemoveRequest( int condorid, int source );
  virtual ~CondorRemoveRequest( void );

  CondorRemoveRequest &set_logfile( const std::string &logfile );

  int get_condorid( void ) const;
  std::string get_logfile( void ) const;

private:
  static const char   *crr_s_CondorId, *crr_s_LogFile;
};

} // Namespace controller

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_REQUEST_H */

// Local Variables:
// mode: c++
// End:

