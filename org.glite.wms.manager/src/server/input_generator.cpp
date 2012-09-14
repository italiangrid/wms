#include <string>
#include <iostream>
#include <unistd.h>

#include <openssl/x509.h>
#include <openssl/pem.h>

#include "glite/lb/producer.h"
#include "glite/lb/consumer.h"
#include "glite/lb/Event.h"
#include "glite/lb/Job.h"
#include "glite/lb/context.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/WMPConfiguration.h"

#include "glite/wms/common/utilities/wm_commands.h"
#include "glite/wms/common/utilities/FileList.h"
#include "glite/wms/common/utilities/FileListLock.h"
#include "glite/wms/common/utilities/scope_guard.h"

#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/JDLAttributes.h"

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wmsutils/classads/classad_utils.h"

#include "glite/security/proxyrenewal/renewal.h"

namespace utilities = glite::wms::common::utilities;
namespace ca = glite::wmsutils::classads;
namespace configuration = glite::wms::common::configuration;
namespace jobid = glite::wmsutils::jobid;
namespace jdl = glite::jdl;
namespace fs = boost::filesystem;
namespace ca = glite::wmsutils::classads;

// HARDCODED:
static const char* hc_sequence_code = "UI=000001:NS=0000000001:WM=000001:\
BH=0000000000:JSS=000000:LM=000000:LRMS=000000:APP=000000";


namespace
{

std::string
get_proxy_subject(std::string const& proxy_file)
{
  static const std::string null_string;

  std::FILE* rfd = std::fopen(proxy_file.c_str(), "r");
  if( !rfd ) {
    return null_string;
  }
  boost::shared_ptr<std::FILE> rfd_(rfd, std::fclose);

  ::X509* rcert = ::PEM_read_X509(rfd, 0, 0, 0);
  if( !rcert ) {
    return null_string;
  }
  boost::shared_ptr<X509> cert_(rcert, ::X509_free);

  ::X509_NAME* name = ::X509_get_subject_name(rcert);
  if( !name ) {
    return null_string;
  }

  char* cp = ::X509_NAME_oneline(name, 0, 0);
  if( !cp ) {
    return null_string;
  }
  boost::shared_ptr<char> cp_(cp, ::free);

  return std::string(cp);
}

bool
create_context_proxy(
  std::string const& x509_proxy,
  edg_wll_Context *ctx,
  jobid::JobId const& jid
)
{
  if( edg_wll_InitContext(ctx) ) {
    return false;
  }

  int errcode = edg_wll_SetParam(
    *ctx,
    EDG_WLL_PARAM_SOURCE,
    EDG_WLL_SOURCE_WORKLOAD_MANAGER
  );
  errcode |= edg_wll_SetParam(
    *ctx,
    EDG_WLL_PARAM_INSTANCE,
    "WMS_TEST"
  );
  errcode |= edg_wll_SetParam(
    *ctx,
    EDG_WLL_PARAM_X509_PROXY,
    x509_proxy.c_str()
  );

  const std::string user_proxy = get_proxy_subject(x509_proxy);
  if( user_proxy.empty() ) {
    return false;
  } else {
    return !errcode and !edg_wll_SetLoggingJobProxy(
      *ctx,
      jid,
      hc_sequence_code,
      user_proxy.c_str(),
      EDG_WLL_SEQ_NORMAL
    );
  }
}

std::string
make_cancel_request(std::string const& job_id) {
  const classad::ClassAd cmd(
    utilities::cancel_command_create(job_id)
  );
  return ca::unparse_classad(cmd);
}

std::string
make_resubmit_request(std::string const& job_id) {
  const classad::ClassAd cmd(
    utilities::resubmit_command_create(job_id, hc_sequence_code)
  );
  return ca::unparse_classad(cmd);
}

std::string
make_submit_request(classad::ClassAd const& jdl) {
  const classad::ClassAd cmd(
    utilities::submit_command_create(jdl)
  );
  return ca::unparse_classad(cmd);
}

std::string
make_match_request(
  classad::ClassAd const& jdl,
  std::string const& output_file
)
{
  const int number_of_results = 10;
  const bool include_brokerinfo = true;

  const classad::ClassAd cmd(
    utilities::match_command_create(
      jdl,
      output_file,
      number_of_results,
      include_brokerinfo
    )
  );

  return ca::unparse_classad(cmd);
}

} // empty namespace


int
main(int argc, char *argv[])
//try
{

  if(argc == 5) {

    std::ifstream is(argv[2]);
    assert(is);

    std::string const local_proxy_full_name(argv[4]);

    classad::ClassAd jdl;
    classad::ClassAdParser parser;
    const bool parsed = parser.ParseClassAd(is, jdl);
    assert(parsed);

    utilities::FileList<std::string> fl(argv[3]);
    //useless like this
    //utilities::FileListMutex mx(fl);
    //utilities::FileListLock lock(mx);

    //
    //SUBMIT
    //
    configuration::Configuration const config(
      "glite_wms.conf", //GLITE_WMS_CONFIG_DIR has to contain the path
      configuration::ModuleType::workload_manager_proxy
    );
    configuration::NSConfiguration const* const ns_config( config.ns() );
    configuration::WMPConfiguration const* const wmp_config( config.wp() );

    std::vector<std::string> lb_addresses = wmp_config->lbserver();
    std::string lb_address;
    if (!lb_addresses.empty()) {
      lb_address = lb_addresses.front();
    }
    if(lb_address.empty())
    {
      lb_address = wmp_config->lblocal_logger();
      std::cout << "LB address: " << lb_address << '\n';
    }
    assert(!lb_address.empty());

    const std::string root_path( ns_config->sandbox_staging_path() );
    fs::path r(root_path, fs::native);

    std::cout << "Staging root path: " << root_path << '\n';

    bool a = true;
    if( !exists(r) ) 
    {
      try {
        a = create_directory(r);
      }
      catch(boost::filesystem::filesystem_error& ) {
        std::cout << root_path << " creation error\n";
        assert(false);
      }
    }
    else {
      if(!is_directory(r)) {
        std::cout << root_path << " is not a directory\n";
        assert(false);
      } 
    }
    utilities::scope_guard r_guard(
      boost::bind(fs::remove_all, r)
    );
    //chmod(root_path.c_str(), 0773);

    edg_wll_Context ctx;
    for( int i = 0; i < boost::lexical_cast< int >(argv[1]); ++i )
    {
        //
        //JOB ID CREATION
        //
        jobid::JobId jid;
        jid.setJobId(lb_address);
        std::string stringjid = jid.toString();

        std::cout << "Registered job id: " << stringjid << '\n';

        //
        //LOGGING STUFF
        //
        std::string user_subject;
        if( !create_context_proxy( local_proxy_full_name , &ctx , jid) )
        {
          std::cout << "LB initialisation failed\n";
          assert(false);
        } else {
          user_subject = get_proxy_subject(local_proxy_full_name);
          std::cout << "LB context created for: " << user_subject << '\n';
        }

        //
        // JDL ATTRIBUTES
        //
        const std::string reduced_path(root_path + "/" + jobid::get_reduced_part(jid, 0));
        const std::string sandbox_root_path(reduced_path + "/" + jobid::to_filename( jid));
        const std::string isb_path(sandbox_root_path + "/input");
        const std::string osb_path(sandbox_root_path + "/output");

        std::cout << "Job sandbox: " << sandbox_root_path << '\n';

        bool a = true;
        try {
          jdl::set_input_sandbox_path(jdl, isb_path);
          jdl::set_output_sandbox_path(jdl, osb_path);
          jdl::set_x509_user_proxy(jdl, sandbox_root_path + "/user.proxy");
          if (!user_subject.empty()) {
            jdl::set_user_subject_name(jdl, user_subject);
          }
          jdl::set_lb_sequence_code(jdl, std::string(hc_sequence_code));
        }
        catch(jdl::CannotSetAttribute&) {
            std::cout << "Error setting JDL attributes\n";
            a = false;
        }

        //
        //SANDBOXES CREATION
        //
        const fs::path rp(reduced_path, fs::native);
        bool b = true;
        if( !exists(rp) ) {
          try {
            b = create_directory(rp);
          }
          catch(boost::filesystem::filesystem_error& ) {
            std::cout << reduced_path << " creation error\n";
          }
        } else {
          if(!is_directory(rp)) {
            b = false;
          }
        }
        utilities::scope_guard rp_guard(
          boost::bind(fs::remove_all, rp)
        );  
        //chmod(reduced_path.c_str(), 0773);

        const fs::path srp(sandbox_root_path, fs::native);
        bool c = true;
        if( !exists(srp) ) {
          try {
            c = create_directory(srp);
          }
          catch(boost::filesystem::filesystem_error& ) {
            std::cout << sandbox_root_path << " creation error\n";
            c = false;
          }
        } else {
          if(!is_directory(srp)) {
            c = false;
          }
        }
        utilities::scope_guard srp_guard(
          boost::bind(fs::remove_all, srp)
        );  
        //chmod(sandbox_root_path.c_str(), 0773);

        const fs::path isp(isb_path, fs::native);
        bool d = true;
        if( !exists(isp) ) {
          try {
            d = create_directory(isp);
          }
          catch(boost::filesystem::filesystem_error& ) {
            std::cout << isb_path << " creation error\n";
            d = false;
          }
        } else {
          if(!is_directory(isp)) {
            d = false;
          }
        }
        utilities::scope_guard isp_guard(
          boost::bind(fs::remove_all, isp)
        );
        //chmod(isb_path.c_str(), 0770);

        const fs::path osp(osb_path, fs::native);
        bool e = true;
        if( !exists(osp) ) {
          try {
            e = create_directory(osp);
          }
          catch(boost::filesystem::filesystem_error& ) {
            std::cout << osb_path << " creation error\n";
            e = false;
          }
        } else {
          if(!is_directory(osp)) {
            e = false;
          }
        }
        utilities::scope_guard osp_guard(
          boost::bind(fs::remove_all, osp)
        );
        //chmod(osb_path.c_str(), 0770);

        if(a and b and c and d and e) { //JDL, fs operations successful

          //
          // JOB REGISTRATION TO LB PROXY
          //
          if( !edg_wll_RegisterJobProxy(
            ctx,
            jid.getId(),
            EDG_WLL_JOB_SIMPLE,
            ca::unparse_classad(jdl).c_str(),
            lb_address.c_str(),
            0, 
            NULL, 
            NULL)
          ) {

            std::cout << "Job successfully registered\n";

            //
            //FILE OPS
            //
            const std::string sym_link_path = sandbox_root_path + "/user.proxy";
            const fs::path slp(sym_link_path, fs::native);
            if( !symbolic_link_exists(slp) ) {
              if( symlink(local_proxy_full_name.c_str(), 
                sym_link_path.c_str()) != 0 ) 
              {
                  std::cout << "Symbolic link of the user proxy failed.\n";
                  assert(false);
              }
            }
            //input files
            //...

            //LOGGING ACTIONS
            edg_wll_LogEnQueuedSTARTProxy(ctx, argv[3], stringjid.c_str(), "");

            //QUEUING IN FILE LIST
            fl.push_back(make_submit_request(jdl));

            //YET LOGGING ACTIONS
            edg_wll_LogEnQueuedOKProxy(ctx, argv[3], stringjid.c_str(), "");
            edg_wll_LogAccepted(
              ctx, 
              EDG_WLL_SOURCE_WM_PROXY, 
              lb_address.c_str(), 
              "", 
              ""
            );
          
            rp_guard.dismiss();
            srp_guard.dismiss();
            isp_guard.dismiss();
            osp_guard.dismiss();

            //CANCEL
            fl.push_back(make_cancel_request(stringjid));

            //RESUBMIT
            fl.push_back(make_resubmit_request(stringjid));

          } else {
            std::cout << "Job registration failed\n";
          }
       } // if a and b and c and d and e

        //LIST MATCH
        fl.push_back(make_match_request(jdl, "/tmp/match_pipe"));
    
    } // for

    r_guard.dismiss();
  }
  else {
    std::cout << "Usage:\n";
    std::cout << "\tbulk #cycles file.jdl input.fl user_proxy\n";
  }
//} catch (utilities::FileContainerError const& e) {
//  std::cout << e.what() << '\n';
//} catch (...) {
//  std::cout << "Generic error\n";
}
