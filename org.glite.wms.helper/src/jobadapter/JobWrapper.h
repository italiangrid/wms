/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/***************************************************************************
 *  filename  : JobWrapper.h
 *  authors   : Alessio Gianelle <alessio.gianelle@pd.infn.it>
 *              Francesco Giacomini <francesco.giacomini@cnaf.infn.it>
 *              Rosario Peluso <rosario.peluso@pd.infn.it>
 *              Elisabetta Ronchieri <elisabetta.ronchieri@cnaf.infn.it>
 *              Marco Cecchi <marco.cecchi@cnaf.infn.it>
 *  Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
 *  For license conditions see LICENSE file or
 *  http://www.edg.org/license.html
 ***************************************************************************/

#ifndef GLITE_WMS_HELPER_JOBADAPTER_JOBWRAPPER_JOB_WRAPPER_H
#define GLITE_WMS_HELPER_JOBADAPTER_JOBWRAPPER_JOB_WRAPPER_H

#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include <boost/shared_ptr.hpp>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "url.h"

namespace classad {
  class ExprList;
}

namespace glite {
namespace wms {
namespace helper {
namespace jobadapter {
	
enum job_type {NORMAL = 0, MPI_LSF = 1, MPI_PBS = 2, INTERACTIVE = 3};

class JobWrapperException
{
public:
  JobWrapperException(std::string const& url);
  std::string message() const;

private:
  std::string m_message;
};

class JobWrapper
{
public:

  /**
   *  Initializes the object passing the parameters to the job wrapper.
   *  \param job
   *  \ingroup jobadapter
   */
  JobWrapper(
    const std::string& job,
    boost::shared_ptr<std::string> jw_template  
  );
  ~JobWrapper();

  struct pimpl;
  boost::shared_ptr<pimpl> m_pimpl;

  /**
   * Declare the standard input file.
   * @param file file to be used as standard input for the user job
   * \ingroup jobadapter
   */
  void standard_input(const std::string& file);

  /**
   * Declare the standard output file.
   * @param file file to be used as standard output of the user job
   * \ingroup jobadapter
   */
  void standard_output(const std::string& file);

  /**
   * Declare the standard error file.
   * @param file file to be used as standard error of the user job
   * \ingroup jobadapter
   */
  void standard_error(const std::string& file);

  /**
   * Declare which files are in the input sandbox and where they are located.
   * @param base_url location of all the files in the input sandbox
   * @param files    list of files in the input sandbox
   * \ingroup jobadapter
   */
  void input_sandbox(const URL& base_url, 
                     const std::vector<std::string>& files);
  
  /**
   * Declare which files are in the output sandbox and where they have to be moved.
   * @param base_url location the files in the output sandbox have to be moved to
   * @param files    list of files in the output sandbox
   * \ingroup jobadapter
   */
  void output_sandbox(const URL& base_url,
                      const std::vector<std::string>& files);

  /**
   * Set the value of the EDG_WL_RB_BROKERINFO environment variable.
   * If the function is not called the variable is not set.
   * @param file file containing Broker information in the job working
   *             directory. If file is not specified a default value is used.
   * \ingroup jobadapter
   */
  void brokerinfo(const std::string& file = s_brokerinfo_default);

  /**
  * Set the value of the EDG_WL_JOBID environment variable.
  * If the function is not called the variable is not set.
  * @param jobid
  * \ingroup jobadapter
  */
  void job_Id(const std::string& jobid);
  
  /**
   * Set the value of the workdir.
   * @param jobid
   * \ingroup jobadapter
   */
  void job_id_to_filename(const std::string& jobid_to_filename);
  
  /**
  * Declare arguments
  * @param args
  * \ingroup jobadapter
  */
  void arguments(const std::string& args);

  /*
  * Declare maradona_url
  * @param maradona_url
  * \ingroup maradona_url
  */
  void maradona_url(const std::string& protocol, 
		        const std::string& filename);

  /**
   * Declare gatekeeper_hostname
   * @param gatekeeper
   * \ingroup jobadapter
   */
  void gatekeeper_hostname(const std::string& scode);

  /**
   * Declare globus_resource_contact_string
   * @param globus_resource_contact_string
   * \ingroup jobadapter
   */
  void globus_resource_contact_string(
                  const std::string& globus_resource_contact_string);

  /**
   * Declare which environment variables must be exported.
   * @param env    list of the environment variables
   * \ingroup jobadapter
   */
  void environment(const std::vector<std::string>& env);

  void nodes(int node);

  /**
   * Field the outputdata attribute.
   * @param output_data    list of classads
   * \ingroup jobadapter
   */
  void outputdata(const classad::ExprList* output_data);

  /**
   * Declare virtual organization
   * @param vo 
   * \ingroup jobadapter
   */ 
  void vo(const std::string& vo);

  /**
   * Set the name of the DSUpload output file.
   * @param jobid
   * \ingroup jobadapter
   */
  void dsupload(const URL& id);

  /**
   * Set the support of Input/Output Sandboxes in WMProxy.
   * @param wm
   * \ingroup jobadapter
   */
  void wmp_support(void);

  /**
   * Declare which files are in the input sandbox and where they are located.
   * @param base_url location of all the files in the input sandbox
   * @param files    list of files in the input sandbox
   * \ingroup jobadapter
   */
  void wmp_input_sandbox_support(const URL& base_url,
                                  const std::vector<std::string>& input_base_files);

  /**
   * Declare which files are in the output sandbox and where they have to be moved.
   * @param base_url location the files in the output sandbox have to be moved to
   * @param files    list of files in the output sandbox
   * \ingroup jobadapter
   */
  void wmp_output_sandbox_support(const std::vector<std::string>& output_files,
				 const std::vector<std::string>& output_dest_files);   
   
  /**
   * Enable the shallow resubmission for this job.
   * @param token_file the shallow resubmission token
   * \ingroup token
   */
  void enable_shallow_resubmission(std::string const& token_file);

  /**
    * Set the support of Perusal.
    * \ingroup jobadapter
    */
  void perusal_support(void);
  void perusal_timeinterval(int perusaltimeinterval);
  void perusal_filesdesturi(std::string const& filesdesturi);
  void perusal_listfileuri(std::string const& listfileuri);

  /**
    * Set the job type (NORMAL, MPI..., INTERACTIVE)
    *  for use by the template script
    */
  void set_job_type(job_type);

  /**
    * Set the support for the wildcard
    * support in the output sandbox
    */
  void set_osb_wildcards_support(bool);

  void set_output_sandbox_base_dest_uri(URL const&);

  /**
    * Prologue is a custom script provided by the user
    * for environment checking or to handle MPI calls
    */
  void prologue(std::string const&);
  void prologue_arguments(std::string const&);

  void epilogue(std::string const&);
  void epilogue_arguments(std::string const&);

  void broker_hostname(std::string const&);
  void ce_application_dir(std::string const&);

  void max_osb_size(const int64_t &);
  void sb_retry_different_protocols(bool const& r);

private:

  static const std::string s_brokerinfo_default;

  std::ostream& print(std::ostream& os) const;  
  friend std::ostream& operator<<(std::ostream& os, const JobWrapper& jw);
  bool fill_out_script(std::ostream&) const;
  bool dump_vars(std::ostream&) const;
};

} // namespace jobadapter
} // namespace helper
} // namespace wms
} // namespace glite

#endif // GLITE_WMS_HELPER_JOBADAPTER_JOBWRAPPER_JOB_WRAPPER_H
