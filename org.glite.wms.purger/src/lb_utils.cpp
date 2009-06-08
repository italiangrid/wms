// File: lb_utils.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it> 

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


#include "lb_utils.h"
#include <sys/types.h>
#include <unistd.h>
#include <map>
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include "glite/jobid/JobId.h"
#include "glite/wms/common/utilities/manipulation.h"
#include "glite/lb/producer.h"
#include "glite/security/proxyrenewal/renewal.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"

namespace jobid = glite::jobid;

namespace configuration = glite::wms::common::configuration;

namespace glite {
namespace wms {
namespace purger {

ContextPtr
create_context(
  jobid::JobId const& id,
  std::string const& x509_proxy,
  std::string const& sequence_code
)
{
  edg_wll_Context context;
  int errcode = edg_wll_InitContext(&context);
  if (errcode) {
    return ContextPtr();
  }

  errcode |= edg_wll_SetParam(context,
                              EDG_WLL_PARAM_SOURCE,
                              EDG_WLL_SOURCE_NETWORK_SERVER);
  errcode |= edg_wll_SetParam(context, EDG_WLL_PARAM_INSTANCE, "NS");
  
  if(!x509_proxy.empty()) 
    errcode |= edg_wll_SetParam(context,
                                EDG_WLL_PARAM_X509_PROXY,
                                x509_proxy.c_str());
  
  const int flag = EDG_WLL_SEQ_NORMAL;
  // the following fails if the sequence code is not formally correct
  errcode |= edg_wll_SetLoggingJob(context, id.c_jobid(), sequence_code.c_str(), flag);

  if (errcode) {
    throw CannotCreateLBContext(errcode);
  }

  return ContextPtr(context, edg_wll_FreeContext);
}

namespace {


bool
proxy_expires_within(std::string const& x509_proxy, time_t seconds)
{
  std::FILE* fd = std::fopen(x509_proxy.c_str(), "r");
  if (!fd) return true; // invalid proxy

  boost::shared_ptr<std::FILE> fd_(fd, std::fclose);

  ::X509* const cert = ::PEM_read_X509(fd, 0, 0, 0);
  if (!cert) return true;

  boost::shared_ptr< ::X509> cert_(cert, ::X509_free);

  return (
    ASN1_UTCTIME_cmp_time_t(
      X509_get_notAfter(cert), std::time(0)+seconds
    ) < 0
  );
}

std::string
get_proxy_subject(std::string const& x509_proxy)
{
  static std::string const null_string;

  std::FILE* fd = std::fopen(x509_proxy.c_str(), "r");
  if (!fd) return null_string;
  boost::shared_ptr<std::FILE> fd_(fd, std::fclose);

  ::X509* const cert = ::PEM_read_X509(fd, 0, 0, 0);
  if (!cert) return null_string;
  boost::shared_ptr< ::X509> cert_(cert, ::X509_free);

  char* const s = ::X509_NAME_oneline(::X509_get_subject_name(cert), 0, 0);
  if (!s) return null_string;
  boost::shared_ptr<char> s_(s, ::free);

  return std::string(s);
}

}

ContextPtr
create_context_proxy(
  jobid::JobId const& id,
  std::string const& x509_proxy,
  std::string const& sequence_code
)
{
  edg_wll_Context context;
  int errcode = edg_wll_InitContext(&context);
  if (errcode) {
    return ContextPtr();
  }

  errcode |= edg_wll_SetParam(
    context,
    EDG_WLL_PARAM_SOURCE,
    EDG_WLL_SOURCE_NETWORK_SERVER
  );
  errcode |= edg_wll_SetParam(
    context,
    EDG_WLL_PARAM_INSTANCE,
    boost::lexical_cast<std::string>(::getpid()).c_str()
  );

  errcode |= edg_wll_SetParam(context,
                              EDG_WLL_PARAM_X509_PROXY,
                              x509_proxy.c_str());

  std::string const user_dn = get_proxy_subject(x509_proxy);
  int const flag = EDG_WLL_SEQ_NORMAL;
  errcode |= edg_wll_SetLoggingJobProxy(
    context,
    id.c_jobid(),
    sequence_code.empty() ? 0 : sequence_code.c_str(),
    user_dn.c_str(),
    flag
  );

  if (errcode) {
    throw CannotCreateLBContext(errcode);
  }

  return ContextPtr(context, edg_wll_FreeContext);
}

std::string
get_original_jdl(edg_wll_Context context, jobid::JobId const& id)
{
  std::string result;

  edg_wll_QueryRec job_conditions[2];
  job_conditions[0].attr    = EDG_WLL_QUERY_ATTR_JOBID;
  job_conditions[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  glite_jobid_t id_;
  glite_jobid_dup(id.c_jobid(),&id_);
  job_conditions[0].value.j = id_;
  job_conditions[1].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec event_conditions[3];
  event_conditions[0].attr    = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  event_conditions[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  event_conditions[0].value.i = EDG_WLL_EVENT_ENQUEUED;
  event_conditions[1].attr    = EDG_WLL_QUERY_ATTR_SOURCE;
  event_conditions[1].op      = EDG_WLL_QUERY_OP_EQUAL;
  event_conditions[1].value.i = EDG_WLL_SOURCE_NETWORK_SERVER;
  event_conditions[2].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_Event* events = 0;
  edg_wll_QueryEvents(context, job_conditions, event_conditions, &events);

  if (events) {
    for (int i = 0; events[i].type; ++i) {
      // in principle there is only one, so save the first one and ignore the
      // others
      if (result.empty()
          && events[i].type == EDG_WLL_EVENT_ENQUEUED
          && events[i].enQueued.result == EDG_WLL_ENQUEUED_OK) {
        result = events[i].enQueued.job;
      }
      edg_wll_FreeEvent(&events[i]);
    }
    free(events);
  }

  return result;
}

namespace {

boost::tuple<int,std::string,std::string>
get_error_info(edg_wll_Context context)
{
  int error;
  std::string error_txt;
  std::string description_txt;

  char* c_error_txt = 0;
  char* c_description_txt = 0;
  error = edg_wll_Error(context, &c_error_txt, &c_description_txt);

  if (c_error_txt) {
    error_txt = c_error_txt;
  }
  free(c_error_txt);
  if (c_description_txt) {
    description_txt = c_description_txt;
  }
  free(c_description_txt);

  return boost::make_tuple(error, error_txt, description_txt);
}

} // anonymous

std::string
get_lb_message(ContextPtr const& context)
{
  return get_lb_message(context.get());
}

std::string
get_lb_message(edg_wll_Context context)
{
  std::string result;

  int error;
  std::string error_txt;
  std::string description_txt;
  boost::tie(error, error_txt, description_txt) = get_error_info(context);

  result += error_txt;
  result += " (";
  result += boost::lexical_cast<std::string>(error);
  result += ") - ";
  result += description_txt;

  return result;
}}}} // glite::wms::purger

