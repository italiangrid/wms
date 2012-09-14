/***************************************************************************
 *  filename  : JobAdapter.cpp
 *  authors   : Elisabetta Ronchieri <elisbetta.ronchieri@cnaf.infn.it>
 ***************************************************************************/
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


#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cassert>
#include <fstream>
#include <vector>

#include <unistd.h>
#include <errno.h>
#include <netdb.h>

#include <openssl/md5.h>

#include <memory>

#include <classad_distribution.h>

#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

#include "JobAdapter.h"
#include "JobWrapper.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/common/utilities/boost_fs_add.h"
#include "glite/wms/common/utilities/edgstrstream.h"
#include "glite/wms/common/utilities/manipulation.h"
#include "glite/jobid/JobId.h"

#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"
#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/PrivateAttributes.h"

#include "glite/wms/helper/exceptions.h"
#include "exceptions.h"

namespace fs = boost::filesystem;
namespace config = glite::wms::common::configuration;
namespace utilities = glite::wms::common::utilities;
namespace utils = glite::wmsutils::classads;
namespace jdl = glite::jdl;
namespace jobid = glite::jobid;

namespace {

  std::string const helper_id("JobAdapterHelper");

  boost::regex const blah_ce(":[0-9]+/blah-");
  boost::regex const voblah_ce(":[0-9]+/voblah-");
  boost::regex const condor_ce(":[0-9]+/condor-");
  boost::regex const nordugrid_ce(":[0-9]+/nordugrid-");
}

namespace glite {
namespace wms {
namespace helper {
namespace jobadapter {

class URL;

namespace {

class Beginning {
public:
  Beginning(const std::string& str) 
    : m_what(str)
    {
    }

  bool operator()(const std::string& str) const
  {
    return str.find(m_what) != std::string::npos;
  }
private:
  std::string m_what;
};
}

/******************************************************************
 *  method :   replace
 *  field: string class
 *  Replace "what" value with "with" value
 *******************************************************************/
void
replace(std::string& where, const std::string& what, const std::string& with)
{
  size_t fpos = where.find(what);

  while (fpos  != std::string::npos) {
    where.replace(fpos, what.length(), with);
    fpos = where.find(what, fpos + what.length() + 1);
  }
}

JobAdapter::JobAdapter(const classad::ClassAd* ad, boost::shared_ptr<std::string> jw_template)
 : m_ad(ad), m_jw_template(jw_template)
{
}

JobAdapter::~JobAdapter(void)
{
}

classad::ClassAd*
JobAdapter::resolve(void)  
try {

  // Figure out the local host name (this should really come from
  // some common entity).
  char hostname[1024];
  std::string local_hostname;
  
  if (gethostname(hostname, sizeof(hostname)) >= 0) {
    hostent resolved_host;
    hostent *resolver_result = NULL;
    int resolver_work_buffer_size = 2048;
    char* resolver_work_buffer = new char[resolver_work_buffer_size];
    int resolve_errno = ERANGE;
    while ((gethostbyname_r(hostname, &resolved_host,
                            resolver_work_buffer,
                            resolver_work_buffer_size,
                            &resolver_result,
                            &resolve_errno) ) == ERANGE ) {
      delete[] resolver_work_buffer;
      resolver_work_buffer_size *= 2;
      resolver_work_buffer = new char[resolver_work_buffer_size];
    }
    if (resolver_result != NULL) {
      local_hostname.assign(resolved_host.h_name);
    }
    delete[] resolver_work_buffer;
  }

  config::Configuration const& config(
    *config::Configuration::instance()
  );

  // Mandatory
  // InputSandboxPath is always included
  std::string inputsandboxpath(jdl::get_input_sandbox_path(*m_ad));
  if (inputsandboxpath.empty()) { 
    throw helper::InvalidAttributeValue(jdl::JDLPrivate::INPUT_SANDBOX_PATH,
                                        inputsandboxpath,
                                        "not empty",
                                        helper_id);
  }
	  
  boost::scoped_ptr<URL> isb_url;
  try {
    isb_url.reset(new URL(inputsandboxpath));
  } catch (InvalidURL&) {
    try {
      isb_url.reset(new URL("gsiftp://" + local_hostname + inputsandboxpath));
    } catch (InvalidURL& e) {
      throw CannotCreateJobWrapper(e.what());
    }
  }

  // Mandatory for shallow resubmission
  std::string token_file(config.wm()->token_file());

  std::string ReallyRunningToken;
  bool shallow_retry_count_attr_exists = false, replans_count_exists = false;
  int replans_count(jdl::get_replans_count(*m_ad, replans_count_exists));
  if (!replans_count_exists) {
    replans_count = 0;
  }
  int shallow_retry_count(
    jdl::get_shallow_retry_count(*m_ad, shallow_retry_count_attr_exists)
  );
  {
    if (!shallow_retry_count_attr_exists
        || (shallow_retry_count < 0 && shallow_retry_count != -1)
    ) {
      shallow_retry_count = 0;
    }
    if (shallow_retry_count != -1) {
      std::string const isb_url_str = isb_url->as_string();
      std::string::size_type const p = isb_url_str.rfind("/input");
      std::string const token_url_str(isb_url_str, 0, p);
      ReallyRunningToken =
       token_url_str + '/'
       + token_file + '_'
       + boost::lexical_cast<std::string>(replans_count);
    }
  }

  std::string const ce_id(jdl::get_ce_id(*m_ad));
  boost::regex const cream_ce_id(".+/cream-.+");
  bool const is_cream_ce = boost::regex_match(ce_id, cream_ce_id);
  if (is_cream_ce) {
    classad::ClassAd* cream_jdl(new classad::ClassAd(*m_ad));
    // let's pass the token on to cream (this one only)
    jdl::set_really_running_token(*cream_jdl,
      ReallyRunningToken
    );
    return cream_jdl;
  }

  std::auto_ptr<classad::ClassAd> result(new classad::ClassAd);
  
  {
    bool no_throw = false;
    jdl::set_ceinfo_host_name(
      *result,
      jdl::get_ceinfo_host_name(*m_ad, no_throw)
    );
  }

  /////////////////////////////////////////////////////////////////////////////

  /* Mandatory */
  std::string executable(jdl::get_executable(*m_ad));
  if (executable.empty()) {
    throw helper::InvalidAttributeValue(jdl::JDL::EXECUTABLE,
                                        executable,
                                        "not empty",
                                        helper_id);
  }
  
  // Mandatory
  // It is renamed as usersubjectname and reinserted with
  std::string certificatesubject(jdl::get_certificate_subject(*m_ad));
  if (certificatesubject.empty()) {
    throw helper::InvalidAttributeValue(jdl::JDL::CERT_SUBJ,
                                        certificatesubject,
                                        "not empty",
                                        helper_id);
  }  

  jdl::set_user_subject_name(*result, certificatesubject);
  
  // Not Mandatory
  std::vector<std::string>  outputsandbox;
  utils::EvaluateAttrListOrSingle(*m_ad, "outputsandbox", outputsandbox); 
 
  // Not Mandatory
  bool b_osb_dest_uri = false;
  std::vector<std::string> outputsandboxdesturi;
  if (!outputsandbox.empty()) {
    utils::EvaluateAttrListOrSingle(*m_ad, "outputsandboxdesturi", outputsandboxdesturi);
    if (!outputsandboxdesturi.empty()) {
      b_osb_dest_uri = true;
    }
  }
  
  // Not Mandatory
  std::vector<std::string>  inputsandbox;
  utils::EvaluateAttrListOrSingle(*m_ad, "inputsandbox", inputsandbox);

  // Not Mandatory
  boost::scoped_ptr<URL> wmpisb_base_uri;
  try {
    bool no_throw;
    std::string const s(jdl::get_wmpinput_sandbox_base_uri(*m_ad, no_throw));
    wmpisb_base_uri.reset(new URL(s));
  } catch (InvalidURL&) {
  }

  std::string outputsandboxpath;
  if (!b_osb_dest_uri && !wmpisb_base_uri) {
    /* Mandatory */
    outputsandboxpath.append(jdl::get_output_sandbox_path(*m_ad));
    if (outputsandboxpath.empty()) {
      throw helper::InvalidAttributeValue(jdl::JDLPrivate::OUTPUT_SANDBOX_PATH,
                                          outputsandboxpath,
                                          "not empty",
                                          helper_id);
    }
  }

  /* Not Mandatory */
  bool b_arg;
  std::string arguments(jdl::get_arguments(*m_ad, b_arg));
 
  /* Not Mandatory */
  bool is_in_jdl;
  std::string prologue = jdl::get_prologue(*m_ad, is_in_jdl);
  std::string prologue_arguments;
  if (is_in_jdl) {
    if (prologue[0] != '/') {
      prologue.insert(0, "./", 2);
    }
    prologue_arguments = jdl::get_prologue_arguments(*m_ad, is_in_jdl);
  }

  /* Not Mandatory */
  std::string epilogue = jdl::get_epilogue(*m_ad, is_in_jdl);
  std::string epilogue_arguments;
  if (is_in_jdl) {
    if (epilogue[0] != '/') {
      epilogue.insert(0, "./", 2);
    }
    epilogue_arguments = jdl::get_epilogue_arguments(*m_ad, is_in_jdl);
  }

  /* Not Mandatory */
  std::vector<std::string> env;
  utils::EvaluateAttrListOrSingle(*m_ad, jdl::JDL::ENVIRONMENT, env);

  /* Mandatory */
  /* It is renamed in globusscheduler. (Below) */
  std::string globusresourcecontactstring(jdl::get_globus_resource_contact_string(*m_ad));
  if (globusresourcecontactstring.empty()) {
    throw helper::InvalidAttributeValue(jdl::JDL::GLOBUSRESOURCE,
                                        globusresourcecontactstring,
                                        "not empty",
                                        helper_id);
  }

  // Get the gatekeeper hostname
  std::string::size_type pos = globusresourcecontactstring.find(":");
  if (pos == 0 || pos == std::string::npos) {
    throw helper::InvalidAttributeValue(jdl::JDL::GLOBUSRESOURCE,
                                        globusresourcecontactstring,
                                        "contains a hostname before a :",
                                        helper_id);
  }
  std::string gatekeeper_hostname(globusresourcecontactstring.substr(0, pos));
 
  /* Mandatory */
  /* x509 user proxy is mandatory for the condor submit file. */
  std::string userproxy(jdl::get_x509_user_proxy(*m_ad));
  if (userproxy.empty()) {
    throw helper::InvalidAttributeValue(jdl::JDLPrivate::USERPROXY,
                                        userproxy,
                                        "not empty",
                                        helper_id);
  }
 
  /* Mandatory */
  /* queuname is mandatory to build the globusrsl string. */
  std::string queuename(jdl::get_queue_name(*m_ad));
  if (queuename.empty()) {
    throw helper::InvalidAttributeValue(jdl::JDL::QUEUENAME,
                                        queuename,
                                        "not empty",
                                        helper_id);
  }

  /* Mandatory */
  /* lrms type is mandatory for the mpich job */
  /* and forwarded to Condor-C in any case.   */
  std::string lrmstype(jdl::get_lrms_type(*m_ad));

  /* Mandatory */
  /* job id is mandatory to build the globusrsl string and to create the */
  /* job wrapper                                                         */
  std::string job_id(jdl::get_edg_jobid(*m_ad));
  if (job_id.empty()) {
    throw helper::InvalidAttributeValue(jdl::JDL::JOBID,
                                        job_id,
                                        "not empty",
                                        helper_id);
  }
  jdl::set_edg_jobid(*result, job_id);

  /* FIXME: Test whether this is a 'blahpd' or 'condor' resource so that  */
  /* FIXME: the submit file can be adjusted accordingly.      */
  /* FIXME: Eventually, CondorG should be able to handle this */

  bool is_blahp_resource = false;
  bool is_voblahp_resource = false;
  bool is_condor_resource = false;
  bool is_nordugrid_resource = false;

  boost::smatch ce_type_smatch;

  if (
    boost::regex_search(
      globusresourcecontactstring,
      ce_type_smatch,
      blah_ce
    )
  ) {
    is_blahp_resource = true;
  } else if (
    boost::regex_search(
      globusresourcecontactstring,
      ce_type_smatch,
      voblah_ce
    )
  ) {
    is_blahp_resource = true;
    is_voblahp_resource = true;
  } else if (
    boost::regex_search(
      globusresourcecontactstring,
      ce_type_smatch,
      condor_ce
    )
  ) {
    is_condor_resource = true;
  } else if (
    boost::regex_search(
      globusresourcecontactstring,
      ce_type_smatch,
      nordugrid_ce
    )
  ) {
    is_nordugrid_resource = true;
    is_condor_resource = true;
  } 

  std::string condor_submit_environment;

  // Mandatory
  jdl::set_universe(*result, "grid"); 

  if (!is_blahp_resource && !is_condor_resource) {
    jdl::set_grid_type(*result, "globus");
    jdl::set_copy_to_spool(*result, false);
    jdl::set_transfer_executable(*result, true);
  } else {
    jdl::set_grid_type(*result, "condor");

    // These three attributes are needed for collecting accounting logs.
    jdl::set_remote_remote_user_subject_name(*result, certificatesubject);
    jdl::set_remote_remote_edg_jobid(*result, job_id);
    jdl::set_remote_remote_ceid(*result, globusresourcecontactstring);
    // The VOMS FQAN is used for logging on the CE side.
    // Will be retrieved from teh cert in case it's not set here.
    std::string voms_fqan(jdl::get_voms_fqan(*m_ad));
    if (!voms_fqan.empty()) {
      jdl::set_remote_remote_voms_fqan(*result, voms_fqan);
    }
    // Virtual Organization is used to compose remote Condor daemons
    // unique name, but we don't really care if it's empty
    std::string vo(jdl::get_virtual_organisation(*m_ad));
    if (!vo.empty()) {
      jdl::set_remote_remote_virtual_organisation(*result, vo);
    }
    if ((is_condor_resource && lrmstype == "condor") || is_nordugrid_resource)
    {
      // condor resource, as per Nate Mueller's May 4, 2005 recipe
      jdl::set_remote_job_universe(*result, 9);
      jdl::set_remote_job_grid_type(*result, "condor");
      jdl::set_remote_requirements(*result, "true");
      jdl::set_remote_remote_schedd(*result, gatekeeper_hostname);
      jdl::set_remote_remote_pool(*result, queuename);
      jdl::set_remote_globus_resource(*result, "BOGUS");
      jdl::set_remote_remote_job_universe(*result, 5);
      // FIXME: these should obviously not be hardwired.
      jdl::set_remote_remote_requirements(*result, "((Arch == \"INTEL\") && (Opsys == \"LINUX\")) && HasFileTransfer");
      jdl::set_remote_remote_file_system_domain(*result, gatekeeper_hostname);
      jdl::set_remote_remote_uid_system_domain(*result, gatekeeper_hostname);
      jdl::set_remote_remote_should_transfer_files(*result, "YES");
      jdl::set_remote_remote_when_to_transfer_output(*result, "ON_EXIT");
    } else if (is_blahp_resource) {
      // blah resource
      jdl::set_remote_job_universe(*result, 9);
      jdl::set_remote_sub_universe(*result, "blah");
      jdl::set_remote_job_grid_type(*result, "blah");
      jdl::set_remote_remote_grid_type(*result, lrmstype);
      jdl::set_remote_remote_queue(*result, queuename);
      // We have to make sure the *jobwrapper* gets transferred to 
      // the worker nodes.
      jdl::set_remote_remote_stagecmd(*result, true);
      // Transfer the CE remaining ("flattened") requirements as well.
      try {
        std::string rreq(jdl::get_remote_remote_requirements(*m_ad));
        jdl::set_remote_remote_requirements(*result, rreq);
      } catch (jdl::CannotGetAttribute const& e) {
        // Ignore the remote_remote_requirements if they aren't there.
      }
    } else {
      throw helper::InvalidAttributeValue(jdl::JDL::GLOBUSRESOURCE,
                                          globusresourcecontactstring,
                                          "cannot find a suitable resource",
                                          helper_id);
    }

    unsigned char md5_cert_hash[MD5_DIGEST_LENGTH];
    utilities::oedgstrstream md5_hex_hash;

    // Without the local host name, the CondorC/BLAH submission
    // will most likely not work. We bail out in this case:
    if (local_hostname.empty()) {
      throw helper::InvalidAttributeValue("local hostname",
                                          local_hostname,
                                          "not empty",
                                          helper_id);
    }

    std::string remote_schedd;

    if (!is_voblahp_resource) {
      std::string hostcertificatesubjectvo(local_hostname);
      hostcertificatesubjectvo.append("/");
      hostcertificatesubjectvo.append(certificatesubject);
      hostcertificatesubjectvo.append("/");
      hostcertificatesubjectvo.append(vo);
      hostcertificatesubjectvo.append(voms_fqan);
      MD5(
        (unsigned char *)hostcertificatesubjectvo.c_str(),
        hostcertificatesubjectvo.length(),
        md5_cert_hash
      );
      md5_hex_hash.setf(std::ios_base::hex, std::ios_base::basefield);
      md5_hex_hash.width(2);
      md5_hex_hash.fill('0');
  
      for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        md5_hex_hash.width(2);
        md5_hex_hash << (unsigned int)md5_cert_hash[i];
      }
  
      std::string hashedcertificatesubject(md5_hex_hash.str());
  
      jdl::set_daemon_unique_name(*result, hashedcertificatesubject);
  
      remote_schedd = hashedcertificatesubject;
    } else { 
      remote_schedd = vo;
    }

    remote_schedd.append("@");
    remote_schedd.append(gatekeeper_hostname);
    jdl::set_remote_schedd(*result, "$$(Name)");

    std::string schedd_requirements("Name==\"");
    schedd_requirements.append(remote_schedd);
    schedd_requirements.append("\"");
    jdl::set_condor_requirements(*result, schedd_requirements);

    jdl::set_globus_resource(*result, "$$(Name)");

    jdl::set_copy_to_spool(*result, false);
    jdl::set_should_transfer_files(*result, "YES");
    jdl::set_when_to_transfer_output(*result, "ON_EXIT");
    jdl::set_transfer_input_files(*result, userproxy);
    // The following directive is only needed for the 'launch-less gLite CE'
    // (Condor-C + glexec on the CE). This is currently unused in this branch.
    //jdl::set_transfer_output(*result, ""); // Make sure we don't get extra files

    // Compute proxy file basename: remove trailing whitespace and slashes.
    std::string userproxy_basename(userproxy);
    char c = userproxy_basename[userproxy_basename.length() - 1];
    while (c == '/' || c == ' ' || c == '\n' || c == '\r' || c == '\t') {
      userproxy_basename.erase(userproxy_basename.length() - 1);
      c = userproxy_basename[userproxy_basename.length() - 1];
    }

    // Then remove leading dirname.
    userproxy_basename.assign(userproxy_basename.substr(userproxy_basename.rfind('/')+1));

    if (!is_condor_resource) {
      condor_submit_environment.assign("X509_USER_PROXY=");
      condor_submit_environment.append(userproxy_basename);
    }

    // Handle remotely-managed environment additions.
    // (GRAM cannot do this).
    if (!condor_submit_environment.empty()) {
      condor_submit_environment.append(";");
    }

    condor_submit_environment.append("$$(GLITE_ENV:)");

    jdl::set_remote_env(*result,condor_submit_environment);

    if (!is_nordugrid_resource) {
      // Cause the job to pop out of the Condor queue if no successful
      // match with 'B' daemons could occur in 15 (default) minutes.
      utilities::oedgstrstream periodic_hold_expression;
      periodic_hold_expression << "JobStatus == 1 && Matched =!= TRUE && CurrentTime > QDate + ";
      periodic_hold_expression << config.jc()->maximum_time_allowed_for_condor_match();
      jdl::set_periodic_hold(*result,periodic_hold_expression.str());
    }

    // Build the jobmanager-fork contact string.
    std::string::size_type pos = globusresourcecontactstring.find("/");
    if (pos == 0 || pos == std::string::npos) {
      throw helper::InvalidAttributeValue(jdl::JDL::GLOBUSRESOURCE,
                                          globusresourcecontactstring,
                                          "contains a slash",
                                          helper_id);
    }
    std::string gatekeeper_fork(globusresourcecontactstring.substr(0, pos));
    gatekeeper_fork.append("/jobmanager-fork");

    jdl::set_site_name(*result, gatekeeper_hostname);
    jdl::set_site_gatekeeper(*result, gatekeeper_fork);
  } /* End of blah || condor case */

  std::string const grid_resource("gt2 " + globusresourcecontactstring);
  result->InsertAttr("grid_resource", grid_resource);

  jdl::set_globus_scheduler(*result, globusresourcecontactstring);
  jdl::set_x509_user_proxy(*result, userproxy);

  bool feedback_nothrow;
  bool wms_feedback(jdl::get_enable_wms_feedback(*m_ad, feedback_nothrow));
  if (wms_feedback) {
    // Will cause the job to pop out of the Condor queue to be then replanned
    std::string periodic_remove_expr("JobStatus == 1 && Matched =!= TRUE && CurrentTime > QDate + ");
    int const grace_period(
      config.wm()->replan_grace_period()
    );
    periodic_remove_expr += boost::lexical_cast<std::string>(grace_period);
    jdl::set_periodic_remove(*result, periodic_remove_expr.c_str());
  }

  /* Mandatory */
  jdl::set_notification(*result, "never");

  /* Not Mandatory */
  bool b_std;
  std::string stdinput(jdl::get_std_input(*m_ad, b_std));
  std::string stderror(jdl::get_std_error(*m_ad, b_std));
  std::string stdoutput(jdl::get_std_output(*m_ad, b_std));

  /* TEMP patch: forward ce_id */
  bool   b_ce_id;
  jdl::set_ce_id(*result, ce_id, b_ce_id);

  /* keep the dag id if present */
  bool b_present = false;
  std::string dag_id(jdl::get_edg_dagid(*m_ad, b_present));
  if (b_present && !dag_id.empty()) {
    jdl::set_edg_dagid(*result, dag_id);
  }  

  /* Mandatory */
  std::string requirements("EDG_WL_JDL_REQ");
  requirements.append(" '");
  std::string reqvalue(jdl::unparse_requirements(*m_ad));
  if (reqvalue.empty()) {
    throw helper::InvalidAttributeValue(jdl::JDL::REQUIREMENTS,
                                        reqvalue,
                                        "not empty",
                                        helper_id);
  }
  replace(reqvalue, "\"", "\\\"");
  
  requirements.append(reqvalue);
  requirements.append("'");
  
  /* Create globusrsl */
  std::string globusrsl("(queue=" + queuename + ")(jobtype=single)");
  globusrsl.append("(environment=");
  std::string jidvalue(job_id);
  replace(jidvalue, "\"", "\\\"");
  std::string jid("(EDG_WL_JOBID '" + jidvalue + "')");
  globusrsl.append(jid);

  //globusrsl.append(requirements);

  bool valid_mw_version;
  std::string mw_version(
    jdl::get_mw_version(*m_ad, valid_mw_version)
  );
  if (valid_mw_version && !mw_version.empty()) {
    globusrsl.append("(EDG_MW_VERSION '" + mw_version + "')");
  }

  /* Not Mandatory */
  bool b_hlr;
  std::string hlrvalue(jdl::get_hlrlocation(*m_ad, b_hlr));
  if (!hlrvalue.empty()) {
    if (is_blahp_resource || is_condor_resource) {
      condor_submit_environment.append(";EDG_WL_HLR_LOCATION=");
      condor_submit_environment.append(hlrvalue);
      jdl::set_remote_env(*result, condor_submit_environment);
    }
    replace(hlrvalue, "\"", "\\\"");
    std::string hlrlocation("(EDG_WL_HLR_LOCATION '" + hlrvalue + "')");
    globusrsl.append(hlrlocation);
  }
  globusrsl.append(")"); //environment

  /* Mandatory */
  std::string type(jdl::get_type(*m_ad));
  if (type.empty()) {
    throw helper::InvalidAttributeValue(jdl::JDL::TYPE,
                                        type,
                                        "not empty",
                                        helper_id);
  }
  jdl::set_type(*result, type);
  
  /* Mandatory */
  /* job type is mandatory to build the correct job wrapper file */
  std::string jobtype(jdl::get_job_type(*m_ad));
  if (jobtype.empty()) {
    throw helper::InvalidAttributeValue(jdl::JDL::JOBTYPE,
                                        jobtype,
                                        "not empty",
                                        helper_id);
  }
  
  // start preparing JobWrapper file
  boost::scoped_ptr<JobWrapper> jw;
   
  // convert the jobid into filename
  std::string jobid_to_file(utilities::to_filename(jobid::JobId(job_id)));
  // concert the dagid into filename
  std::string dagid_to_file;
  if (!dag_id.empty()) {
    dagid_to_file.append(utilities::to_filename(jobid::JobId(dag_id)));
  }

  // check if there is '/' in the executable
  if (executable[0] != '/') {
    executable.insert(0, "./", 2);
  }    

  // lowercase all jobtype characters
  std::string ljobtype(jobtype);
  transform(ljobtype.begin(), ljobtype.end(), ljobtype.begin(), ::tolower);

  int cpu_node_number = 0;
  bool cpu_node_number_exist = false;
  cpu_node_number = jdl::get_cpu_number(*m_ad, cpu_node_number_exist);
  if (!cpu_node_number_exist) {
    cpu_node_number = jdl::get_node_number(*m_ad, cpu_node_number_exist);
  }
  if (cpu_node_number_exist && cpu_node_number) {
    std::string cpu_num(boost::lexical_cast<std::string>(cpu_node_number));
    globusrsl += "(count=" + cpu_num + ")(hostCount=" + cpu_num + ')';
  }

  std::string rslarguments;
  if ( m_ad->EvaluateAttrString("RSLArguments", rslarguments) &&
    !rslarguments.empty() ) {
     globusrsl += "(arguments='" + rslarguments + "')";
  }

  if (ljobtype == "mpich") {
    // lowercase all lrmstype characters
    std::string llrmstype(lrmstype);
    std::transform(llrmstype.begin(), llrmstype.end(), llrmstype.begin(), ::tolower);    
    if (llrmstype != "lsf" && llrmstype != "pbs") {
      throw helper::InvalidAttributeValue(jdl::JDL::LRMS_TYPE,
                                          lrmstype,
                                          "lsf or pbs",
                                          helper_id);
    }
    
    if (is_blahp_resource || is_condor_resource) {
      jdl::set_remote_remote_nodenumber(*result, cpu_node_number);
    }

    std::string exec;
    std::string::size_type pos = executable.find("./");
    if (pos == std::string::npos) {
      exec.append(executable);
    } else {
      exec.append(executable.substr(pos+2));
    }
 
    jw.reset(new JobWrapper(exec, m_jw_template));
    if (llrmstype == "lsf") {
      jw->set_job_type(MPI_LSF);
    } else if ((llrmstype == "pbs") || (llrmstype == "torque")) {
      jw->set_job_type(MPI_PBS);
    } else {
      // not possible;
    }
  } else if (ljobtype == "interactive") {
    if (find_if(env.begin(), env.end(), 
          Beginning("BYPASS_SHADOW_PORT=")) == env.end()
       )
    {
      std::ostringstream env_str;
      std::copy(env.begin(), env.end(),
                std::ostream_iterator<std::string>(env_str, ":"));
      throw helper::InvalidAttributeValue(jdl::JDL::ENVIRONMENT,
                                          env_str.str(),
                                          "contains BYPASS_SHADOW_PORT",
                                          helper_id);
    }
    if (find_if(env.begin(), env.end(),
	    Beginning(std::string("BYPASS_SHADOW_HOST="))) == env.end()
    )
    {
      std::ostringstream env_str;
      std::copy(env.begin(), env.end(),
                std::ostream_iterator<std::string>(env_str, ":"));
      throw helper::InvalidAttributeValue(jdl::JDL::ENVIRONMENT,
                                          env_str.str(),
                                          "contains BYPASS_SHADOW_HOST",
                                          helper_id);
    }

    env.push_back("LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH");

    std::string::size_type pos = executable.find("./");
    if (pos == std::string::npos) {
      jw.reset(new JobWrapper(executable, m_jw_template));
    } else {
      jw.reset(new JobWrapper(executable.substr(pos+2), m_jw_template));
    }
    jw->set_job_type(INTERACTIVE);
  } else {
    jw.reset(new JobWrapper(executable, m_jw_template));
    jw->set_job_type(NORMAL);
  }
 
  if (cpu_node_number_exist && cpu_node_number) {
    jw->nodes(cpu_node_number);
  }

  // GlueCEInfoApplicationDir
  std::string application_dir = jdl::get_ce_application_dir(*m_ad, is_in_jdl);
  if (is_in_jdl) {
    jw->ce_application_dir(application_dir);
  }

  // PerusalFileEnable is not mandatory
  bool b_perusal;
  bool perusal = jdl::get_perusal_file_enable(*m_ad, b_perusal);
  if (b_perusal & perusal) {
    // PerusalTimeInterval is mandatory
    int perusaltimeinterval = jdl::get_perusal_time_interval(*m_ad);

    // PerusalFilesDestURI is mandatory
    std::string perusalfilesdesturi = jdl::get_perusal_files_dest_uri(*m_ad);
    if (perusalfilesdesturi.empty()) {
      throw helper::InvalidAttributeValue(jdl::JDL::PU_FILES_DEST_URI,
                                          perusalfilesdesturi,
                                          "not empty",
                                          helper_id);
    }

    // PerusalListFileURI is mandatory
    std::string perusallistfileuri = jdl::get_perusal_list_file_uri(*m_ad);
    if (perusallistfileuri.empty()) {
      throw helper::InvalidAttributeValue(jdl::JDLPrivate::PU_LIST_FILE_URI,
                                          perusallistfileuri,
                                          "not empty",
                                          helper_id);
    }
    jw->perusal_support();
    jw->perusal_timeinterval(perusaltimeinterval);
    jw->perusal_filesdesturi(perusalfilesdesturi);
    jw->perusal_listfileuri(perusallistfileuri);
  }
 
 
  // OutputData is not mandatory
  bool b_od;
  classad::ExprTree* odtree = jdl::get_output_data(*m_ad, b_od);
  std::string otptdata("Warning");
  if (b_od) {
    classad::ExprList* odtree_list = dynamic_cast<classad::ExprList*>(odtree);

    if (odtree_list) {
      jw->outputdata(odtree_list);
    } else {
      classad::ClassAd *odtree_classad = dynamic_cast<classad::ClassAd*>(odtree);
      if (odtree_classad) {
        std::auto_ptr<classad::ExprList> list(new classad::ExprList);
        list->push_back(odtree->Copy());
        jw->outputdata(list.get());
      } else {
        throw helper::InvalidAttributeValue(jdl::JDL::OUTPUTDATA,
			                    otptdata,
					    "either list of classad or a classad",
					    helper_id);
      }
    }
      
    // Virtual Organization is mandatory if OutputData is in the JDL
    std::string vo(jdl::get_virtual_organisation(*m_ad));
    if (vo.empty()) {
      throw helper::InvalidAttributeValue(jdl::JDL::VIRTUAL_ORGANISATION,
                                          vo,
                                          "not empty",
                                          helper_id);
    } else {
       jw->vo(vo);	    
    }
    try {
      jw->dsupload(URL(job_id));
    } catch (InvalidURL& ex) {
      throw CannotCreateJobWrapper(ex.what());
    }
  }
	
  jdl::set_globus_rsl(*result, globusrsl);

  jw->max_osb_size((int64_t)config.wm()->max_output_sandbox_size());
  jw->sb_retry_different_protocols(config.wm()->sb_retry_different_protocols());
  jw->standard_input(stdinput);
  jw->standard_output(stdoutput);
  jw->standard_error(stderror);
  jw->brokerinfo();
  jw->arguments(arguments);
  jw->prologue(prologue);
  jw->prologue_arguments(prologue_arguments);
  jw->epilogue(epilogue);
  jw->epilogue_arguments(epilogue_arguments);
  jw->job_Id(job_id);
  jw->job_id_to_filename(jobid_to_file);
  jw->broker_hostname(local_hostname);
  jw->environment(env);
  jw->gatekeeper_hostname(gatekeeper_hostname);
  jw->globus_resource_contact_string(globusresourcecontactstring);
  jw->set_osb_wildcards_support(false);
  if (!b_osb_dest_uri) {
    // the support for wildcards is deducted by the absence of this attribute
    bool found_in_jdl;
    std::string osb_base_dest_uri = 
      jdl::get_output_sandbox_base_dest_uri(*m_ad, found_in_jdl);
    if(found_in_jdl && !osb_base_dest_uri.empty()) {
      try {
        URL url_(osb_base_dest_uri);
        jw->set_output_sandbox_base_dest_uri(url_);
        jw->set_osb_wildcards_support(true);
      } catch (InvalidURL& ex) {
        throw CannotCreateJobWrapper(ex.what());
      }
    }
  }

  if (shallow_retry_count != -1) {
    jw->enable_shallow_resubmission(ReallyRunningToken);
  }

  if (wmpisb_base_uri) {
    jw->wmp_input_sandbox_support(*isb_url, inputsandbox);
  } else {
    jw->input_sandbox(*isb_url, inputsandbox);
  }

  if (!wmpisb_base_uri) {
    try {
      URL outputsandboxpath_url(outputsandboxpath);
      jw->output_sandbox(outputsandboxpath_url, outputsandbox);
    } catch (InvalidURL&) {
      std::string new_outputsandboxpath(
        "gsiftp://" + local_hostname + outputsandboxpath
      );
      try {
        URL outputsandboxpath_url(new_outputsandboxpath);
        jw->output_sandbox(outputsandboxpath_url, outputsandbox);
      } catch (InvalidURL& ex) {
        throw CannotCreateJobWrapper(ex.what());
      }
    }
  } else if (b_osb_dest_uri) {
    jw->wmp_output_sandbox_support(outputsandbox, outputsandboxdesturi);
  }

  //check if we need to support new Input/Output Sandboxes in WMProxy
  if (b_osb_dest_uri || wmpisb_base_uri) {
    jw->wmp_support();
  }
  //end preparation of the JobWrapper dump

  if (!wmpisb_base_uri) {
    // Mandatory
    // Maradona file path
    try {
      fs::path maradona_path(config.ns()->sandbox_staging_path(), fs::native);
      maradona_path /= fs::path(utilities::get_reduced_part(jobid::JobId(job_id)), fs::native);
      maradona_path /= fs::path(jobid_to_file, fs::native);
      maradona_path /= fs::path("Maradona.output", fs::native);
      
      jw->maradona_url(
        config.lm()->maradona_transport_protocol() + "://" + local_hostname + "/",
        maradona_path.native_file_string()
      );
    } catch(fs::filesystem_error const&) {
      throw CannotCreateJobWrapper("FS error creating maradona path\n");
    }

  } else {
    jw->maradona_url(wmpisb_base_uri->as_string(), "/Maradona.output");
  }

  // read the submit file path
  std::string jw_name("JobWrapper." + jobid_to_file + ".sh");
  std::string jw_path_string;
  try {
    fs::path jw_path(
      utilities::normalize_path(config.jc()->submit_file_dir()),
      fs::native
    );
    if (!dag_id.empty()) {
      jw_path /= fs::path(utilities::get_reduced_part(jobid::JobId(dag_id)), fs::native);
      std::string jw_dagid_name("dag.");
      jw_dagid_name.append(dagid_to_file);
      jw_path /= fs::path(jw_dagid_name, fs::native);
    } else {  
      jw_path /= fs::path(utilities::get_reduced_part(jobid::JobId(job_id)), fs::native);
    }
    jw_path /= fs::path(jw_name, fs::native);
    jw_path_string = jw_path.native_file_string();
    
    utilities::create_parents(jw_path.branch_path());
  } catch(fs::filesystem_error const&) {
    throw CannotCreateJobWrapper("FS error creating jobwrapper path\n");
  } catch(utilities::CannotCreateParents const& err) {
    throw CannotCreateJobWrapper("Cannot create parent path " + jw_path_string + ": " + err.what() + "\n");
  }

  // write the JobWrapper in the submit file path
  std::ofstream ofJW(jw_path_string.c_str());
  if (!ofJW) {
    throw CannotCreateJobWrapper(jw_path_string);
  }
  try {
    ofJW << *jw;
  } catch (JobWrapperException e) {
    throw CannotCreateJobWrapper(e.message());
  }

  // The jdl::get_output_data function returns a _copy_ of the object.
  delete odtree;
  
  // Mandatory
  // the new executable value is the jobwrapper file path
  jdl::set_executable(*result, jw_path_string);

  // Prepare the output file path
  
  std::string output_file_path(config.jc()->output_file_dir());
  output_file_path.append("/");
  if (!dag_id.empty()) {
    output_file_path.append(utilities::get_reduced_part(jobid::JobId(dag_id)));
    output_file_path.append("/");
    output_file_path.append(dagid_to_file);
  } else {
    output_file_path.append(utilities::get_reduced_part(jobid::JobId(job_id)));
  }  
  output_file_path.append("/");
  output_file_path.append(jobid_to_file);
  output_file_path.append("/");

  if (is_blahp_resource || is_condor_resource) {
    // Condor-C will not move Standard Output and Error to the absolute
    // path specified in the submit file, but to the Initial Dir.
    // This defaults to /tmp, so StandardOutput/Error files from different
    // jobs may overwrite each other as well.
    // So, set a different InitialDir per job.
    jdl::set_initial_dir(*result, output_file_path);
  }

  if (is_nordugrid_resource) {
    jdl::set_grid_resource(*result, "nordugrid " + gatekeeper_hostname);
    jdl::set_nordugrid_rsl(
      *result,
      "(runTimeEnvironment=ENV/GLITE)" + globusrsl
        + "(jobName=^*" + job_id + "^*)"
    );
  }

  // New parameter Mandatory
  // Output file path is mandatory
  std::string output(output_file_path);
  output.append("StandardOutput");
  jdl::set_output(*result, output);

  // New parameter Mandatory
  // Error file path is mandatory
  std::string error(output_file_path);
  error.append("StandardError");
  jdl::set_error_(*result, error);

  // Mandatory
  jdl::set_stream_output(*result, false);
  jdl::set_stream_error(*result, false);

  // Mandatory
  // Condor Log file path
  std::string condor_file_path(config.lm()->condor_log_dir());
  std::string log(condor_file_path);
  log.append("/");
  log.append("CondorG.log");
  jdl::set_log(*result, log);
  
  return result.release();

} catch (jdl::CannotGetAttribute const& e) {
  throw helper::CannotGetAttribute(e, helper_id);
} catch (jdl::CannotSetAttribute const& e) {
  throw helper::CannotSetAttribute(e, helper_id);
} 

} // namespace jobadapter
} // namespace helper
} // namespace wms
} // namespace glite

