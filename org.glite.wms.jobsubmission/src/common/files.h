#ifndef EDG_WORKLOAD_JOBCONTROL_COMMON_FILES_H
#define EDG_WORKLOAD_JOBCONTROL_COMMON_FILES_H

#include <ctime>

#include <memory>

#include <boost/filesystem/path.hpp>
#include <glite/wmsutils/jobid/JobId.h>

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

class Files {
  typedef  boost::filesystem::path   path;

public:
  Files( const glite::wmsutils::jobid::JobId &id );
  Files( const glite::wmsutils::jobid::JobId &dagid, const glite::wmsutils::jobid::JobId &jobid );
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
  const path &dag_log_file( void );
  const path &dag_submit_directory( void );
  const path &sandbox_root( void );
  const path &input_sandbox( void );
  const path &output_sandbox( void );

private:
  Files( const Files & ); // Not implemented
  Files &operator=( const Files & ); // Not implemented

  path *createDagLogFileName( const std::string &jobid );

  time_t                f_epoch;
  std::auto_ptr<path>   f_submit, f_classad, f_outdir, f_logfile, f_maradona, f_stdout, f_stderr, f_wrapper;
  std::auto_ptr<path>   f_sandbox, f_insbx, f_outsbx, f_dagsubdir;
  std::string           f_jobid, f_dagid;
  path                  f_jobReduced, f_dagReduced;

  static const std::string   f_s_submitPrefix, f_s_submitSuffix, f_s_wrapperPrefix, f_s_scriptSuffix;
  static const std::string   f_s_classadPrefix, f_s_dagPrefix;
  static const std::string   f_s_stdout, f_s_stderr;
  static const std::string   f_s_maradona;
  static const std::string   f_s_logPrefix, f_s_dagLogPrefix, f_s_logSuffix;
  static const std::string   f_s_Output, f_s_Input;
  static const std::string   f_s_prePrefix, f_s_postPrefix;
};

}; // Namespace jccommon

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_COMMON_FILES_H */

// Local Variables:
// mode: c++
// End:
