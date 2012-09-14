/*
 * DagCommandFactoryServerImpl.cpp
 * 
 * Copyright (c) 2001, 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */


#include "Command.h"
#include "common.h"
#include "CommandFactoryServerImpl.h"
#include "logging.h"
#include "exception_codes.h"
#include "NetworkServer.h"
#include "JobId.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/jdl/DAGAd.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wmsutils/tls/socket++/GSISocketAgent.h"
#include "glite/wmsutils/tls/socket++/SocketAgent.h"

#include <string>
#include "globus_gss_assist.h"
#include <pwd.h>           
#include <classad_distribution.h>
#include <unistd.h>
#include <errno.h>
#include <boost/tuple/tuple.hpp>

namespace utils = glite::wmsutils::classads;

namespace logger    = glite::wms::common::logger;
namespace requestad = glite::jdl;
namespace commands  = glite::wms::manager::ns::commands;
namespace socket_pp = glite::wmsutils::tls::socket_pp;
namespace nsjobid   = glite::wms::ns::jobid;

using namespace boost::details::pool; 

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {
namespace dag {

#ifndef WITH_GLOBUS_FTP_CLIENT_API	

  bool createJobDirs() {

    edglog_fn("DAG::CFSI");

    std::string input_sandbox_local_path(local_path + "/input");
    std::string output_sandbox_local_path(local_path + "/output");

    mkdir(local_path.c_str(), 0771);
    chmod(local_path.c_str(), 0771);
	
    mkdir(input_sandbox_local_path.c_str(), 0770);
    chmod(input_sandbox_local_path.c_str(), 0770);
      
    mkdir(output_sandbox_local_path.c_str(), 0770);
    chmod(output_sandbox_local_path.c_str(), 0770);

    // *** Use next block insteas of following two lines
    // chown(output_sandbox_local_path.c_str(), (uid_t)-1, local_group->gr_gid);
    // chown(input_sandbox_local_path.c_str(), (uid_t)-1, local_group->gr_gid);
    
    uid_t my_uid = geteuid();
    struct passwd *my_uinfo = getpwuid(my_uid);
    if (my_uinfo != NULL) {
      // Change sandbox staging dir group ownership to the default group of
      // the non-privileged user that the WM process is running as.
      chown(output_sandbox_local_path.c_str(), (uid_t)-1, my_uinfo->pw_gid);
      chown(input_sandbox_local_path.c_str(), (uid_t)-1, my_uinfo->pw_gid);
    } else {
      edglog(fatal) << "Warning: could not obtain PW information about UID " << input_sandbox_local_path << std::endl;
      return false;
    }
    return true;
  }

#endif


  bool createStagingDirectories(commands::Command* cmd)
  { 
    edglog_fn("DAG::CSD");
    char *local_account = NULL;
    std::string jdl, errstr;
    cmd -> getParam("jdl", jdl);
  
    classad::ClassAdParser parser;
    classad::ClassAd* jdlad = parser.ParseClassAd( jdl );
    if (! jdlad ) {
      cmd -> setParam("SDCreationError", client::NSE_MKDIR);
      cmd -> setParam("SDCreationMessage", "Unparsable Dag Jdl.");
      return true;
    }
    requestad::DAGAd dagad(*jdlad);
    std::string id( utils::unparse_expression(*dagad.get_generic("edg_jobid")) );

    std::string authentication_key;
    std::string key_value;
    // <<<<<<<<<<<<<<<<<<<<<<<<<
    //singleton_default<daemon::NetworkServer>::instance().configuration().authentication_key();
    // =========================
    authentication_key="CertificateSubject";
    // >>>>>>>>>>>>>>>>>>>>>>>
    if( ! jdlad -> EvaluateAttrString(authentication_key, key_value) ) {
      cmd->setParam("SDCreationMessage","Cannot retrieve Authentication Key.");
      cmd->setParam("SDCreationError", (int)client::NSE_AUTHENTICATION_ERROR);
      return true;
    }
  	
    if (globus_gss_assist_gridmap((char*)key_value.c_str(), &local_account)) { 
      std::string msg( "No authentication mapping for:" + key_value );
      std::cerr << msg << std::endl;
      cmd->setParam("SDCreationMessage",msg);
      cmd->setParam("SDCreationError", int(client::NSE_AUTHENTICATION_ERROR));
    } else {
      // *** Use next two lines instead of those
      // struct group *local_group = getgrnam(local_account);
      // if (local_group) {

      struct passwd *local_uinfo = getpwnam(local_account);
      if (local_uinfo) {
      
	std::string root( singleton_default<ns::daemon::NetworkServer>::instance().configuration().sandbox_staging_path() );
	std::string local_path( root + "/" + nsjobid::to_filename( wmsutils::jobid::JobId(id) ) );

	requestad::DAGAd::node_iterator node_b;
	requestad::DAGAd::node_iterator node_e;

#ifndef WITH_GLOBUS_FTP_CLIENT_API	

	if ( createJobDirs( cmd, local_path) ) {

	  boost::tie(node_b, node_e) = dagad.nodes();

	  while ( node_b != node_e) {
	    const requestad::DAGNodeInfo& node_info = (*node_b).second;
	    const classad::ClassAd* ad = node_info.description_ad();

	    if (!ad->EvaluateAttrString("edg_jobid", id) ) {
	      edglog(fatal) << "Error while retrieving jobId from classad." << std::endl;
	      cmd -> setParam("SDCreationError", client::NSE_MKDIR);
	      cmd -> setParam("SDCreationMessage", "Error while retrieving jobId from classad. Node: " + (*node_b).first);
	      return true;
	    }
	    
	    local_path.assign( root + "/" + nsjobid::to_filename( wmsutils::jobid::JobId(id) ) );

	    if ( !createJobDirs(cmd, local_path) ) {
	      errstr.assign("Error creating DAG directories at\n\t Host: " + to_host + "\n\t DAGId:" + id);
	      edglog(fatal) << errstr << std::endl;	  	      
	      return true;
	    }
	    
	    node_b++;
	  }

	  cmd -> setParam("ClientCreateDirsPassed", true);
	  edglog(fatal) << "Remote Dirs Cretion Successful" << std::endl;
	} else {
	  errstr.assign("Error creating DAG directories at\n\t Host: " + to_host + "\n\t DAGId:" + id);
	  edglog(fatal) << errstr << std::endl;
	}
#endif

	id.assign( utils::unparse_expression(*dagad.get_generic("edg_jobid")) );
	local_path.assign( root + "/" + nsjobid::to_filename( wmsutils::jobid::JobId(id) ) );
 
	socket_pp::GSISocketAgent *agent = 
	  static_cast<socket_pp::GSISocketAgent*>(&cmd -> agent());
      
	if(!fcopy( agent -> CredentialsFile(), local_path +"/user.proxy")){
	
	  std::string error_msg(std::string("Unable to copy ") + agent -> CredentialsFile() +
				std::string(" to ") + local_path + std::string("/user.proxy") + 
				std::string(" (") + strerror( errno ) + std::string(")") );
	
	  edglog(fatal) << error_msg << std::endl;
	  cmd->setParam("SDCreationMessage", error_msg);
	  cmd->setParam("SDCreationError",(int)client::NSE_AUTHENTICATION_ERROR);
	} 
	else {
	  cmd->setParam("X509UserProxy", local_path + "/user.proxy");	
	  cmd->setParam("SDCreationMessage","No Error");
	  cmd->setParam("SDCreationError", (int)client::NSE_NO_ERROR);

	  //Creating a link to user proxy for each node. 
	  boost::tie(node_b, node_e) = dagad.nodes();

	  while ( node_b != node_e) {
	    const requestad::DAGNodeInfo& node_info = (*node_b).second;
	    const classad::ClassAd* ad = node_info.description_ad();

	    if (!ad->EvaluateAttrString("edg_jobid", id) ) {
	      edglog(fatal) << "Error while retrieving jobId from classad." << std::endl;
	      cmd -> setParam("SDCreationError", client::NSE_MKDIR);
	      cmd -> setParam("SDCreationMessage", "Error while retrieving jobId from classad. Node: " + (*node_b).first);
	      return true;
	    }    	    
	    std::string job_path( root + "/" + 
				  nsjobid::to_filename( wmsutils::jobid::JobId(id) ) + 
				  "/user.proxy" );
	  
	    edglog(fatal) << "Creating link: " << job_path << std::endl;
	    if ( symlink( (local_path + "/user.proxy").c_str(), job_path.c_str()) ) {
	      errstr.assign("\nError creating user proxy link at\n\t source:     " +
			    local_path + "/user.proxy \n\t destination: " + job_path + 
			    "\n\t JobId:      " + id + ".\n" + strerror(errno));
	      edglog(fatal) << errstr << std::endl;
	      break;
	    }

	    node_b++;
	  }
	} 
      } else {
	std::string msg( "Could not find any UNIX user named: " + std::string( local_account ) );
	edglog(fatal) << msg << std::endl;
	cmd->setParam("SDCreationMessage", msg);
	cmd->setParam("SDCreationError", (int)client::NSE_AUTHENTICATION_ERROR);
      }
    }
 
    if( local_account ) {
      free( local_account );
    }
    return true;
  }


} // namespace dag  
} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite 
