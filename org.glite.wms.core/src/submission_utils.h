// File: submission_utils.h
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

// $Id: submission_utils.h,v 1.1.2.1.2.5.2.1.2.1.2.1.4.1 2011/11/25 08:45:24 mcecchi Exp $

#ifndef GLITE_WMS_MANAGER_SERVER_SUBMISSION_UTILS_H
#define GLITE_WMS_MANAGER_SERVER_SUBMISSION_UTILS_H

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <openssl/pem.h>
#include <openssl/x509.h>

#include "submit_request.h"
#include "glite/security/proxyrenewal/renewal.h"

namespace classad {
class ClassAd;
}

namespace glite {

namespace jobid {
class JobId;
}

namespace wms {
namespace manager {
namespace server {

class HitMaxRetryCount
{
  int m_n;
public:
  HitMaxRetryCount(int n): m_n(n) {}
  int count() const { return m_n; }
};

class HitJobRetryCount
{
  int m_n;
public:
  HitJobRetryCount(int n): m_n(n) {}
  int count() const { return m_n; }
};

class HitMaxShallowCount
{
  int m_n;
public:
  HitMaxShallowCount(int n): m_n(n) {}
  int count() const { return m_n; }
};

class HitJobShallowCount
{
  int m_n;
public:
  HitJobShallowCount(int n): m_n(n) {}
  int count() const { return m_n; }
};

class MissingProxy: public std::exception {
  std::string m_what;
public:
  MissingProxy(std::string const& reason) : m_what(reason) { }
  ~MissingProxy() throw() { }
  char const* what() const throw()
  {
    return m_what.c_str();
  }
};

class InvalidProxy: public std::exception {
  std::string m_what;
public:
  InvalidProxy(std::string const& reason) : m_what(reason) { }
  ~InvalidProxy() throw() { }
  char const* what() const throw()
  {
    return m_what.c_str();
  }
};

class CannotRetrieveJDL { };
class CannotCreateToken: public std::exception {
  std::string m_what;
public:
  CannotCreateToken(
    std::string const& token,
    int error
  ) {
    m_what = "Cannot create token " + token + " (" +
      boost::lexical_cast<std::string>(error) + ')';
  }
  CannotCreateToken() { }
  ~CannotCreateToken() throw() { }
  char const* what() const throw()
  {
    return m_what.c_str();
  }
};

enum job_type_t {
   single,
   dag,
   collection
};

job_type_t job_type(classad::ClassAd const& jdl);
boost::filesystem::path get_cancel_token(jobid::JobId const& id);
boost::filesystem::path get_reallyrunning_token(
  glite::jobid::JobId const& id,
  int replans_count
);
void check_shallow_count(classad::ClassAd const& jdl, int count);
void check_deep_count(classad::ClassAd const& jdl, int count);
int match_retry_period();
bool shallow_resubmission_is_enabled(classad::ClassAd const& jdl);
void create_token(boost::filesystem::path const& p);
boost::shared_ptr<X509> read_proxy(std::string const& proxy_file);
bool is_proxy_expired(glite::jobid::JobId const& id);
bool is_proxy_expired(
  boost::shared_ptr<X509>& cert,
  glite::jobid::JobId const& id
);

class RemoveNodeFromCollection
{
  std::string m_node_name;
  boost::shared_ptr<SubmitState> m_request;
public:
  RemoveNodeFromCollection(
    std::string const& node_name,
    boost::shared_ptr<SubmitState>request 
  ) : m_node_name(node_name), m_request(request) { }

  void operator()() {
    boost::mutex::scoped_lock l(m_request->request_mutex());
    m_request->m_collection_pending.erase(m_node_name);
  }
};

class UnregisterProxy {
  std::string m_id;
public:
  UnregisterProxy(jobid::JobId const& id)
    : m_id(id.toString())
  {
  }
  void operator()() const
  {
    glite_renewal_UnregisterProxy(m_id.c_str(), "");
  }
};


}}}} // glite::wms::manager::server

#endif
