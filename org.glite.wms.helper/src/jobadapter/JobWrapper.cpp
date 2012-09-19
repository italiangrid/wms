//**************************************************************************
//  Filename  : JobWrapper.cpp
//  Authors   : 
//              Marco Cecchi <marco.cecchi@cnaf.infn.it>
//              Francesco Giacomini <francesco.giacomini@cnaf.infn.it>
//              Alessio Gianelle <alessio.gianelle@pd.infn.it>
//              Rosario Peluso <rosario.peluso@pd.infn.it>
//              Elisabetta Ronchieri <elisabetta.ronchieri@cnaf.infn.it>
//**************************************************************************
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


#include <algorithm>
#include <cassert>
#include <string>
#include <fstream>

#include <classad_distribution.h>

#include "glite/jdl/JobAdManipulation.h"
#include "JobWrapper.h"

namespace configuration = glite::wms::common::configuration;
namespace jdl = glite::jdl;

namespace glite {
namespace wms {
namespace helper {
namespace jobadapter {

JobWrapperException::JobWrapperException(std::string const& msg)
  : m_message(msg)
{
}

std::string
JobWrapperException::message() const
{
  return m_message;
}

struct JobWrapper::pimpl {

  std::string               m_job;
  std::string               m_standard_input;
  std::string               m_standard_output;
  std::string               m_standard_error;
  std::string               m_arguments;
  std::string               m_maradona_url;

  boost::shared_ptr<URL>    m_input_base_url;
  std::vector<std::string>  m_input_files;

  boost::shared_ptr<URL>    m_output_base_url;
  boost::shared_ptr<URL>    m_output_sandbox_base_dest_uri;

  std::vector<std::string>  m_output_files;

  std::string               m_brokerinfo;
  std::string               m_jobid;
  std::string               m_jobid_to_filename;
  std::string               m_gatekeeper_hostname;
  std::string               m_globus_resource_contact_string;
  std::vector<std::string>  m_environment;
  int                       m_nodes; // nodes for parallel jobs
  boost::shared_ptr<classad::ExprList>
                            m_outputdata;
  std::string               m_vo;
  std::string               m_dsupload;

  bool	                    m_wmp_support;
  std::vector<std::string>  m_wmp_input_files;
  std::vector<std::string>  m_wmp_input_base_files;
  std::vector<std::string>  m_wmp_output_files;
  std::vector<std::string>  m_wmp_output_dest_files;
  
  std::string               m_shallow_resubmission_token;

  bool                      m_perusal_support;
  int                       m_perusal_timeinterval;
  std::string               m_perusal_filesdesturi;
  std::string               m_perusal_listfileuri;
  
  std::string               m_prologue;
  std::string               m_prologue_arguments;

  std::string               m_epilogue;
  std::string               m_epilogue_arguments;

  int                       m_job_type;
  bool                      m_osb_wildcards_support;
  bool                      m_sb_retry_different_protocols;

  std::string               m_broker_hostname;
  std::string               m_ce_application_dir;
  int64_t                   m_max_osb_size;

  boost::shared_ptr<std::string> m_jw_template;
};

const std::string JobWrapper::s_brokerinfo_default = ".BrokerInfo";

JobWrapper::JobWrapper(
  const std::string& job,
  boost::shared_ptr<std::string> jw_template
)
 : m_pimpl(new pimpl)
{  
  m_pimpl->m_nodes = 0;
  m_pimpl->m_wmp_support = false;
  m_pimpl->m_perusal_support = false;
  m_pimpl->m_osb_wildcards_support = false;
  m_pimpl->m_job = job;
  m_pimpl->m_jw_template = jw_template;
}

JobWrapper::~JobWrapper()
{
}

void
JobWrapper::prologue(std::string const& p)
{
  m_pimpl->m_prologue = p;
}

void
JobWrapper::prologue_arguments(std::string const& a)
{
  m_pimpl->m_prologue_arguments = a;
}

void
JobWrapper::epilogue(std::string const& e)
{
  m_pimpl->m_epilogue = e;
}

void
JobWrapper::epilogue_arguments(std::string const& a)
{
  m_pimpl->m_epilogue_arguments = a;
}

void
JobWrapper::standard_input(const std::string& file)
{
  m_pimpl->m_standard_input = file;
}

void
JobWrapper::standard_output(const std::string& file)
{
  m_pimpl->m_standard_output = file;
}

void
JobWrapper::standard_error(const std::string& file)
{
  m_pimpl->m_standard_error = file;
}

void
JobWrapper::input_sandbox(const URL& base_url,
                          const std::vector<std::string>& files)
{
  m_pimpl->m_input_base_url.reset(new URL(base_url));
  copy(files.begin(), files.end(), back_inserter(m_pimpl->m_input_files));
}

void
JobWrapper::output_sandbox(URL const& base_url,
                           std::vector<std::string> const& files)
{
  m_pimpl->m_output_base_url.reset(new URL(base_url));
  copy(files.begin(), files.end(), back_inserter(m_pimpl->m_output_files));
}
 
void
JobWrapper::set_output_sandbox_base_dest_uri(URL const& osb_base_dest_uri)
{
  m_pimpl->m_output_sandbox_base_dest_uri.reset(new URL(osb_base_dest_uri));
}

void
JobWrapper::brokerinfo(const std::string& file)
{
  assert(file.find_first_of("/") == std::string::npos);
  assert(file != "");

  m_pimpl->m_brokerinfo = file;
}

void
JobWrapper::job_Id(const std::string& jobid)
{
  m_pimpl->m_jobid = jobid;
}

void
JobWrapper::set_job_type(job_type job_type_)
{
  m_pimpl->m_job_type = job_type_;
}

void
JobWrapper::job_id_to_filename(const std::string& jobid_to_filename)
{
  m_pimpl->m_jobid_to_filename = jobid_to_filename;
}

void
JobWrapper::arguments(const std::string& args){
  m_pimpl->m_arguments = args;
}

void
JobWrapper::maradona_url(const std::string& protocol, const std::string& filename)
{
  m_pimpl->m_maradona_url = protocol + filename;
}

void
JobWrapper::gatekeeper_hostname(const std::string& gatekeeper)
{
  m_pimpl->m_gatekeeper_hostname = gatekeeper;
}

void
JobWrapper::globus_resource_contact_string(
                const std::string& globus_resource_contact_string)
{
  m_pimpl->m_globus_resource_contact_string = globus_resource_contact_string;
}

void 
JobWrapper::environment(const std::vector<std::string>& env)
{
  m_pimpl->m_environment = env;
}

void
JobWrapper::nodes(int node)
{
  m_pimpl->m_nodes = node;
}

void
JobWrapper::outputdata(const classad::ExprList* output_data)
{
  m_pimpl->m_outputdata.reset(static_cast<classad::ExprList*>(output_data->Copy()));
}

void
JobWrapper::vo(const std::string& vo)
{
  m_pimpl->m_vo = vo;
}

void
JobWrapper::dsupload(const URL& id)
{
  m_pimpl->m_dsupload = "DSUpload_" + id.path().substr(1) + ".out";
}

void
JobWrapper::wmp_support(void)
{
  m_pimpl->m_wmp_support = true;
}

void
JobWrapper::wmp_input_sandbox_support(const URL& base_url,
				      const std::vector<std::string>& input_base_files)
{
  m_pimpl->m_input_base_url.reset(new URL(base_url));

  copy(input_base_files.begin(), input_base_files.end(), back_inserter(m_pimpl->m_wmp_input_base_files));

  for (std::vector<std::string>::const_iterator it = input_base_files.begin();
      it != input_base_files.end(); it++) {

    std::string::size_type pos = it->find_last_of("/");
    if (pos != std::string::npos) {
      std::string filename = it->substr(pos);
      m_pimpl->m_wmp_input_files.push_back(filename);
    }
  }
}

void 
JobWrapper::wmp_output_sandbox_support(const std::vector<std::string>& output_files,
                                  const std::vector<std::string>& output_dest_files)
{
  copy(output_files.begin(), output_files.end(), back_inserter(m_pimpl->m_wmp_output_files));
  copy(output_dest_files.begin(), output_dest_files.end(), back_inserter(m_pimpl->m_wmp_output_dest_files));
}

void JobWrapper::set_osb_wildcards_support(bool value)
{
  m_pimpl->m_osb_wildcards_support = value;
}

void
JobWrapper::broker_hostname(std::string const& _)
{
  m_pimpl->m_broker_hostname = _;
}

void
JobWrapper::ce_application_dir(std::string const& _)
{
  m_pimpl->m_ce_application_dir = _;
}

void
JobWrapper::enable_shallow_resubmission(std::string const& token)
{
  m_pimpl->m_shallow_resubmission_token = token;
}

void
JobWrapper::perusal_support(void)
{
  m_pimpl->m_perusal_support = true;
}

void
JobWrapper::perusal_timeinterval(int perusaltimeinterval)
{
  m_pimpl->m_perusal_timeinterval = perusaltimeinterval;
}

void
JobWrapper::perusal_filesdesturi(const std::string& filesdesturi)
{
  m_pimpl->m_perusal_filesdesturi = filesdesturi;
}

void 
JobWrapper::perusal_listfileuri(const std::string& listfileuri)
{
  m_pimpl->m_perusal_listfileuri = listfileuri;
}

void
JobWrapper::max_osb_size(int64_t const& m)
{
  m_pimpl->m_max_osb_size = m;
}

void
JobWrapper::sb_retry_different_protocols(bool const& r)
{
  m_pimpl->m_sb_retry_different_protocols = r;
}

namespace {

template<typename T>
bool
dump(std::ostream& os,
  const std::string& name,
  const T& value)
{
  return os << name << '=' << value << '\n';
}

template<typename T>
bool
dump(std::ostream& os,
  const std::string& name,
  const std::vector<T>& values)
{
  int i = 0;

  for(typename std::vector<T>::const_iterator it = values.begin(); it != values.end() ; ++it )  {
    os << name << '[' << i << "]=" << *it << '\n';
    ++i;
  }

  return os;
}

bool
dump(std::ostream& os,
  const std::string& name,
  const std::vector<std::string>& values)
{
  int i = 0;

  if(values.empty()) {
    os << "declare -a " << name << '\n';
  }
  else {
    for(std::vector<std::string>::const_iterator it = values.begin(); it != values.end() ; ++it ) {
      os << name << '[' << i << "]=\"" << *it << "\"\n";
      ++i;
    }
  }

  return os;
}

bool
dump(std::ostream& os,
  const std::string& name,
  const std::vector<char*> values)
{
  std::vector<std::string> string_values;

  for(std::vector<char*>::const_iterator it = values.begin(); it != values.end() ; ++it ) {
    string_values.push_back(std::string(*it));
  }
  return dump(os, name, string_values);
}

bool
dump(std::ostream& os,
  const std::string& name,
  const std::string& value)
{
  os << name << '=';
  if (!value.empty()) {
    os << '"' << value << '"';
  }
  return os << '\n';
}

bool
dump(std::ostream& os,
  const std::string& name,
  const char* var)
{
  return dump(os, name, std::string(var));
}

} // anonymous namespace

bool 
JobWrapper::dump_vars(std::ostream& os) const
{
  std::vector< std::string > output_files;
  std::vector< std::string > logical_file_names;
  std::vector< std::string > storage_elements;
  bool check;

  if (m_pimpl->m_outputdata) {
    classad::ExprList::const_iterator const it_end = m_pimpl->m_outputdata->end();
    for (classad::ExprList::const_iterator it = m_pimpl->m_outputdata->begin(); it != it_end; ++it) {
      classad::ClassAd* ad = dynamic_cast< classad::ClassAd* >(*it);
      if (ad != 0) {
        output_files.push_back(jdl::get_output_file(*ad));
        logical_file_names.push_back(jdl::get_logical_file_name(*ad, check));
        storage_elements.push_back(jdl::get_storage_element(*ad, check));
      }
    }
  }

  return dump(os, "__brokerinfo", m_pimpl->m_brokerinfo) &&
    dump(os, "__jobid", m_pimpl->m_jobid) &&
    dump(os, "__job", m_pimpl->m_job) &&
    dump(os, "__standard_input", m_pimpl->m_standard_input) &&
    dump(os, "__standard_output", m_pimpl->m_standard_output) &&
    dump(os, "__standard_error", m_pimpl->m_standard_error) &&
    dump(os, "__arguments", m_pimpl->m_arguments) &&
    dump(os, "__gatekeeper_hostname", m_pimpl->m_gatekeeper_hostname) &&
    dump(os, "__maradona_url", m_pimpl->m_maradona_url) &&
    dump(os, "__input_base_url", (m_pimpl->m_input_base_url == 0 ?
      "" : m_pimpl->m_input_base_url->as_string())) &&
    dump(os, "__input_file", m_pimpl->m_input_files) &&
    dump(os, "__output_base_url", (m_pimpl->m_output_base_url == 0 ?
      "" : m_pimpl->m_output_base_url->as_string())) &&
    dump(os, "__output_file", m_pimpl->m_output_files) &&
    dump(os, "__jobid_to_filename", m_pimpl->m_jobid_to_filename) &&
    dump(os, "__globus_resource_contact_string", 
      m_pimpl->m_globus_resource_contact_string
    ) &&
    dump(os, "__environment", m_pimpl->m_environment) &&
    dump(os, "__nodes", m_pimpl->m_nodes) &&
    dump(os, "__vo", m_pimpl->m_vo) &&
    dump(os, "__dsupload", m_pimpl->m_dsupload) &&
    dump(os, "__wmp_support", m_pimpl->m_wmp_support) &&
    dump(os, "__wmp_input_file", m_pimpl->m_wmp_input_files) &&
    dump(os, "__wmp_input_base_file", 
      m_pimpl->m_wmp_input_base_files
    ) &&
    dump(os, "__wmp_output_file", m_pimpl->m_wmp_output_files) &&
    dump(os, "__wmp_output_dest_file", 
      m_pimpl->m_wmp_output_dest_files
    ) &&
    dump(
      os,
      "__shallow_resubmission_token",
      m_pimpl->m_shallow_resubmission_token
    ) &&
    dump(os, "__perusal_support", m_pimpl->m_perusal_support) &&
    dump(os, "__perusal_timeinterval",
      (
        m_pimpl->m_perusal_support
        ? m_pimpl->m_perusal_timeinterval
        : 0
      )
    ) &&
    dump(os, "__perusal_filesdesturi", m_pimpl->m_perusal_filesdesturi) &&
    dump(os, "__perusal_listfileuri", m_pimpl->m_perusal_listfileuri) &&
    dump(os, "__prologue", m_pimpl->m_prologue) &&
    dump(os, "__prologue_arguments", m_pimpl->m_prologue_arguments) &&
    dump(os, "__epilogue", m_pimpl->m_epilogue) &&
    dump(os, "__epilogue_arguments", m_pimpl->m_epilogue_arguments) &&
    dump(os, "__output_data", m_pimpl->m_outputdata != 0) &&
    dump(os, "__output_file", output_files) &&
    dump(os, "__output_lfn", logical_file_names) &&
    dump(os, "__output_se", storage_elements) &&
    dump(os, "__osb_wildcards_support", m_pimpl->m_osb_wildcards_support) &&
    dump(os, "__broker_hostname", m_pimpl->m_broker_hostname) &&
    dump(os, "__ce_application_dir", m_pimpl->m_ce_application_dir) &&
    dump(os, "__output_sandbox_base_dest_uri", 
      (m_pimpl->m_output_sandbox_base_dest_uri == 0 ? "" 
      : m_pimpl->m_output_sandbox_base_dest_uri->as_string())
    ) &&
    dump(os, "__job_type", m_pimpl->m_job_type) &&
    dump(os, "__retry_different_transports", m_pimpl->m_sb_retry_different_protocols) &&
    dump(os, "__max_outputsandbox_size", m_pimpl->m_max_osb_size);
}

bool 
JobWrapper::fill_out_script(std::ostream& output_stream) const
{
  output_stream << "#!/bin/sh\n\n";

  if (!dump_vars(output_stream)) {
    return false;
  }

  output_stream << '\n' << *m_pimpl->m_jw_template;
  return true;
}

std::ostream&
JobWrapper::print(std::ostream& os) const
{
  if (!fill_out_script(os)) {
    throw JobWrapperException("Cannot create jobwrapper script");
  }

  return os;
}

std::ostream&
operator<<(std::ostream& os, const JobWrapper& jw)
{
  return jw.print(os);
}

} // namespace jobadapter
} // namespace helper
} // namespace wms
} // namespace glite
