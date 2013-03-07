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

#ifndef EDG_WORKLOAD_JOBCONTROL_COMMON_FILES_H
#define EDG_WORKLOAD_JOBCONTROL_COMMON_FILES_H

#include <ctime>

#include <memory>

#include <boost/filesystem/path.hpp>
#include <glite/jobid/JobId.h>

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

class Files {
  typedef  boost::filesystem::path   path;

public:
  Files( const glite::jobid::JobId &id );
  ~Files( void );

  const path &submit_file( void );
  const path &jobwrapper_file( void );
  const path &classad_file( void );
  const path &output_directory( void );
  const path &standard_output( void );
  const path &standard_error( void );
  const path &maradona_file( void );
  const path &log_file( time_t epoch );
  const path &log_file( void );
  const path &sandbox_root( void );
  const path &input_sandbox( void );
  const path &output_sandbox( void );

private:
  Files( const Files & ); // Not implemented
  Files &operator=( const Files & ); // Not implemented

  time_t                f_epoch;
  std::auto_ptr<path>   f_submit, f_classad, f_outdir, f_logfile, f_maradona, f_stdout, f_stderr, f_wrapper;
  std::auto_ptr<path>   f_sandbox, f_insbx, f_outsbx;
  std::string           f_jobid;
  path                  f_jobReduced;

  static const std::string   f_s_submitPrefix, f_s_submitSuffix, f_s_wrapperPrefix, f_s_scriptSuffix;
  static const std::string   f_s_classadPrefix;
  static const std::string   f_s_stdout, f_s_stderr;
  static const std::string   f_s_maradona;
  static const std::string   f_s_logPrefix, f_s_logSuffix;
  static const std::string   f_s_Output, f_s_Input;
  static const std::string   f_s_prePrefix, f_s_postPrefix;
};

} // namespace jccommon
} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_COMMON_FILES_H */
