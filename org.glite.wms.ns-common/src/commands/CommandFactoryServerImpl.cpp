
/* 
 * CommandFactoryServerImpl.cpp
 *
 * Copyright (c) 2001, 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_smallint.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/regex.hpp>

#include <classad_distribution.h>
#include "globus_gss_assist.h"
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fstream>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <malloc.h>
#include <cerrno> 
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "CommandState.h"
#include "ForwardRequest.h"
#include "ReceiveString.h"
#include "ReceiveLong.h"
#include "ReceiveBoolean.h"
#include "SendBoolean.h"
#include "SendString.h"
#include "SendInt.h"
#include "SendLong.h"
#include "SendVector.h"
#include "ExecuteFunction.h"

#include "DagCommandFactoryServerImpl.h"
#include "CommandFactoryServerImpl.h"
#include "Command.h"
#include "common.h"
#include "NetworkServer.h" 

#include "exception_codes.h"

#include "../versions/Version.h"
#include "../versions/server_ver.h"
#include "const.h"
#include "JobId.h"
#include "logging.h"

#include "glite/wmsutils/tls/socket++/GSISocketAgent.h"
#include "glite/wmsutils/tls/socket++/SocketAgent.h"
#include "glite/wms/common/utilities/ii_attr_utils.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/common/utilities/quota.h"
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h" 
#include "glite/lb/producer.h"
#include "glite/security/proxyrenewal/renewal.h"

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/purger/purger.h"
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wms/common/utilities/boost_fs_add.h"
#include "glite/wms/common/utilities/edgstrstream.h"
#include "listjobmatch.h"
#include "listfiles.h"

#include "glite/jdl/DAGAd.h"

namespace socket_pp     = glite::wmsutils::tls::socket_pp;
namespace utilities     = glite::wms::common::utilities;
namespace utils		= glite::wmsutils::classads;
namespace client        = glite::wms::manager::ns::client;
namespace configuration = glite::wms::common::configuration;
namespace logger        = glite::wms::common::logger;
namespace purger        = glite::wms::purger;
namespace versions      = glite::wms::manager::ns::versions;
namespace nsjobid       = glite::wms::ns::jobid;
namespace fsm           = glite::wms::manager::ns::fsm;
namespace jobid         = glite::wmsutils::jobid;
namespace jdl           = glite::jdl;
namespace fs            = boost::filesystem;

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

static bool LogRefusedJob(Command*,char*, bool, bool);
static bool LogAcceptedJob(Command*, bool, bool);
static bool LogCancelJob(Command*, char*, bool, bool);
static bool LogPurgeJob(Command*, bool, bool);
static bool LogAbortedJob(Command*, char*, bool, bool);


bool serializeServerImpl(socket_pp::SocketAgent* sck, Command* cmd)
{
  edglog_fn("CFSI::serializeServer");
  edglog(info) << "Serializing Server." << std::endl;
  std::string strAd;
  if( sck -> Receive( strAd ) ) {
    if (cmd -> ad) {
      delete (cmd -> ad);
      cmd -> ad = NULL;
    }
    cmd->ad = utils::parse_classad(strAd);
    return cmd->ad ? true : false;  
  } else {

  }
  return false;
}

std::string retrieveHostName() {
  struct hostent hent, *hp=0;
  size_t hstbuflen = 1024;
  char *tmphstbuf;
  char hostname[101];
  int res;
  int herr;

  edglog_fn("CFSI::retrHostName");    
  if (gethostname(hostname, 100) == -1) {
    edglog(severe) << "Error while retrieving host name." << std::endl;
  }
  
  tmphstbuf = (char *)malloc (hstbuflen);
  while ( (res = gethostbyname_r(hostname, 
				 &hent, 
				 tmphstbuf,
				 hstbuflen,
				 &hp,
				 &herr)) == ERANGE ) {
    hstbuflen *= 2;
    tmphstbuf = (char *)realloc (tmphstbuf, hstbuflen);
  }

  if (res || hp == NULL) {
    free(tmphstbuf);
    return "";
  }

  edglog(debug) << "Hostname: " << hp->h_name << std::endl;
  std::string str(hp->h_name);

  free(tmphstbuf);

  return str; 
}

static bool copy_file(const std::string& from, const std::string& to)
{
  edglog_fn("CFSI::copyFile");
  edglog(info) << "Copying file.." << std::endl;
  edglog(debug) << "From: " << from << " To: " << to << std::endl;
  std::ifstream in ( from.c_str() );
  
  if( !in.good() ) {
    return false;  
  }
  std::ofstream out( to.c_str() );
  
  if( !out.good() ) {
    return false;
  }
  out << in.rdbuf(); // read original file into target
  
  struct stat from_stat;
  if(   stat(from.c_str(), &from_stat) ||
        chown( to.c_str(), from_stat.st_uid, from_stat.st_gid ) ||
        chmod( to.c_str(), from_stat.st_mode ) ) {
    edglog(severe) << "Copy failed. From: " << from << " To: " << to << std::endl;
    return false;
  }
  edglog(debug) << "Copy done." << std::endl;
  return true;
}

static bool setMultiValuedAttributes(Command* cmd)
{
  edglog_fn("CFSI::setMVA");
  edglog(info) << "Setting MultiValueAttributes." << std::endl;
  utilities::ii_attributes::const_iterator attrs_begin, attrs_end;
  boost::tie(attrs_begin,attrs_end) = utilities::ii_attributes::multiValued();
  std::vector<std::string> mva(attrs_begin, attrs_end);
  bool result = cmd -> setParam("MultiAttributeList",mva);	
  return result;
}

static bool setSandboxRootPath (Command* cmd) {
  edglog_fn("CFSI::setSRP");
  edglog(info) << "Setting SandboxRootPath." << std::endl;
  std::string path(singleton_default<ns::daemon::NetworkServer>::instance().configuration().sandbox_staging_path());
  return cmd -> setParam("SandboxRootPath", path);	
}

static bool setMaxInputSandboxSize (Command* cmd) {
  edglog_fn("CFSI::setMISS");
  edglog(info) << "Setting MaxInputSandboxSize." << std::endl;
  double jobsize = singleton_default<ns::daemon::NetworkServer>::instance().configuration().max_input_sandbox_size();
  return cmd -> setParam("MaxInputSandboxSize", jobsize);
}

static bool setUserQuota (Command* cmd) {
  edglog_fn("CFSI::setUQ");
  edglog(info) << "Setting UserQuota." << std::endl;
  socket_pp::GSISocketAgent *agent =
            static_cast<socket_pp::GSISocketAgent*>(&cmd -> agent());
  std::string uname( agent -> GridmapName() );
  std::pair<long, long> quota( utilities::quota::getQuota(uname) );
  return ( cmd -> setParam("SoftLimit", (double)quota.first)
           && cmd -> setParam("HardLimit", (double)quota.second) );
}

static bool setUserFreeQuota (Command* cmd) {
  edglog_fn("CFSI::setUFQ");
  edglog(info) << "Setting UserFreeQuota." << std::endl;
  socket_pp::GSISocketAgent *agent =
              static_cast<socket_pp::GSISocketAgent*>(&cmd -> agent());
  std::string uname( agent -> GridmapName() );
  std::pair<long, long> quota( utilities::quota::getFreeQuota(uname) );
  return ( cmd -> setParam("SoftLimit", (double)quota.first)
           && cmd -> setParam("HardLimit", (double)quota.second) );  
}

static bool isQuotaManagementOn (Command* cmd) {
  edglog_fn("CFSI::isQMO");
  edglog(info) << "Checking QuotaManagementStatus." << std::endl;
  bool quotaon = singleton_default<ns::daemon::NetworkServer>::instance().configuration().enable_quota_management();
  return cmd -> setParam("QuotaOn", quotaon);
}

static bool insertPipePath (Command* cmd) {
  edglog_fn("CFSI::insPPath");
  edglog(info) << "Inserting ListMatch Pipe Name" << std::endl;
  char time_string[32];
  utilities::oedgstrstream s;
  struct timeval tv;
  struct tm* ptm;
  long milliseconds;

  /* Obtain the time of day, and convert it to a tm struct. */
  gettimeofday (&tv, NULL);                                                                    
  ptm = localtime (&tv.tv_sec);                                                                
  /* Format the date and time, down to a single second. */                                     
  strftime (time_string, sizeof (time_string), "%Y%m%d%H%M%S", ptm); 
  milliseconds = tv.tv_usec;

  s << singleton_default<ns::daemon::NetworkServer>::instance().configuration().list_match_root_path() << "/" <<
    cmd << "." << std::string(time_string) << milliseconds;
  return cmd -> setParam("file", s.str());
}


static bool doPurge(Command* cmd) {
  edglog_fn("CFSI::doPurge");
  edglog(info) << "Preparing to Purge." << std::endl;
  std::string dg_jobid;
  if ( cmd -> getParam("JobId", dg_jobid) ) {
     const wmsutils::jobid::JobId jobid(dg_jobid);	  
     edglog(warning) << "JobId object for purging created: "<< dg_jobid << std::endl;

     bool result = false;
     result = purger::purgeStorage(jobid);
     
     if (result) {
        LogPurgeJob(cmd, true, false);
	return false;
     } else {
       edglog(warning) << "Warning: Call to Purger returned an error." << std::endl;
     }
     return result;
  } else {
    edglog(critical) << logger::setfunction("CommandFactoryServerImpl::doPurge()") << 
      "Error in Purging: Job id not found in command. Not done." << std::endl;
    return false;
  }
}

bool setJobPaths(Command* cmd)
{
  edglog_fn("CFSI::setJobPaths");
  edglog(info) << "Setting Pathnames." << std::endl;
  std::string path(singleton_default<ns::daemon::NetworkServer>::instance().configuration().sandbox_staging_path());
  std::string jdl;
  std::string jobid, cmdname;
  assert( cmd -> getParam ("Command", cmdname) );
  
  // START MOVE: To Move to Proper function 
  std::string dagpath(singleton_default<ns::daemon::NetworkServer>::instance().configuration().sandbox_staging_path());
  // END MOVE

  if (cmdname == "JobSubmit") {
	  
     if ( !cmd -> getParam("jdl", jdl) ) {
	edglog(critical) << "Submit: jdl not found." << std::endl;     
        return false; 
     } 
     classad::ClassAdParser parser;
     classad::ClassAd jdlad;
     if ( !parser.ParseClassAd( jdl, jdlad )  ) {
       edglog(critical) << "Submit: error parsing jdl." << std::endl;
       return false;
     }
     
     jdlad.EvaluateAttrString("edg_jobid", jobid);
     // START MOVE: To move to a proper function.
  } else if ( cmdname == "DagSubmit" ) {
    
    if ( !cmd -> getParam("jdl", jdl) ) { 
      cmd -> setParam("SDCreationError", client::NSE_MKDIR); 
      cmd -> setParam("SDCreationMessage", "Dag Jdl not found."); 
      return true; 
    } 
    
    classad::ClassAdParser parser; 
    classad::ClassAd* jdlad = parser.ParseClassAd( jdl ); 
    if (! jdlad ) {
      cmd -> setParam("SDCreationError", client::NSE_MKDIR);
      cmd -> setParam("SDCreationMessage", "Unparsable Dag Jdl.");
      return true; 
    }

    jdl::DAGAd dagad(*jdlad); 
    jobid = std::string( utils::unparse_expression(*dagad.get_generic("edg_jobid")) );

    edglog(debug) << utils::asString(cmd -> asClassAd()) << std::endl;
    // END MOVE: add a } at the end       
  } else if ( (cmdname == "JobCancel") || (cmdname == "JobPurge") ) {
     if ( !cmd -> getParam("JobId", jobid) ) {
        edglog(critical) << "Cancel: jobid not found." << std::endl;
     	return false;
     }	  
  }
  path.assign(path + "/" + nsjobid::to_filename( wmsutils::jobid::JobId(jobid) )); 

  // START MOVE: spostare ma non cancellare
  if ( cmdname == "JobSubmit" || cmdname == "DagSubmit" ) { 
    cmd -> setParam("OutputSandboxPath",path + "/output");
    cmd -> setParam("InputSandboxPath",path + "/input");
    edglog(debug) << "OSB..: " << path << "/output" << std::endl;
    edglog(debug) << "ISB..: " << path << "/input" << std::endl;

    if ( cmdname == "DagSubmit" ) {

      classad::ClassAdParser parser;
      boost::scoped_ptr<classad::ClassAd> jdlad;
      jdlad.reset( parser.ParseClassAd( jdl ) );
      if (! jdlad ) { 
	cmd -> setParam("SDCreationError", client::NSE_MKDIR);
	cmd -> setParam("SDCreationMessage", "Unparsable Dag Jdl.");
	return true;
      } 
 
      jdl::DAGAd dagad(*jdlad);

      // Let's Set nodes Sandboxes                                                                                     
      jdl::DAGAd::node_iterator node_b;
      jdl::DAGAd::node_iterator node_e;
      boost::tie(node_b, node_e) = dagad.nodes();
 
      while ( node_b != node_e) {
        const jdl::DAGNodeInfo& node_info = (*node_b).second;
	jdl::DAGNodeInfo newnode_info( node_info.as_classad() );
        classad::ClassAd* ad = static_cast<classad::ClassAd*>(node_info.description_ad()->Copy());
        classad::ClassAd* newad = static_cast<classad::ClassAd*>(newnode_info.description_ad()->Copy());

        if (!ad->EvaluateAttrString("edg_jobid", jobid) ) {
	  edglog(fatal) << "Error while retrieving jobId from classad." << std::endl; 
        }
 
	//Set the Input&OutputSandboxPath
	std::string rootpath;
	rootpath.assign(dagpath + "/" + nsjobid::to_filename( wmsutils::jobid::JobId(jobid) ));
        newad->InsertAttr("InputSandboxPath",  rootpath + "/input");
	newad->InsertAttr("OutputSandboxPath", rootpath + "/output");
	
#ifdef DEBUG	
        edglog(debug) << "Node Name       : " << (*node_b).first << std::endl;
	edglog(debug) << "New Node        : " << std::endl; utils::asString(*newad) << std::endl;
	edglog(debug) << "DagNodeInfo     : " << std::endl; newnode_info.as_classad() << std::endl;
#endif	

	if ( !newnode_info.replace_description_ad(newad) ) {
	  edglog(critical) << "Unable to modify DagAd attributes." << std::endl;
	}

#ifdef DEBUG
	edglog(debug) << "New DagNodeInfo : " << newnode_info.as_classad() << std::endl;
#endif
	dagad.replace_node((*node_b).first, newnode_info);
	jdl = utils::unparse_classad(dagad.ad());

#ifdef DEBUG
	edglog(debug) << "New Jdl         : " << std::endl << edglog(debug) << jdl << std::endl;
#endif
	node_b++;
      }
      cmd -> setParam("jdl", jdl);

    }

  }
  // END MOVE

  edglog(debug) << "Root.: " << path  << std::endl;
  return cmd -> setParam("JobPath", path);
}

bool lookupJob(Command* cmd)
{
  return true;
}

bool stagingDirectoryCreationPostControl(Command* cmd)
{
  int creationError;
  edglog_fn("CFSI::SDCPostControl");
  if (! cmd -> getParam("SDCreationError", creationError)) {
    edglog(fatal) << "Staging Dirs Creation Error :" << creationError << std::endl;
    LogRefusedJob(cmd, "Undefined Error during Staging Dir creation:", true, true);
    return false;
  }

  std::string reason;
  if (creationError == client::NSE_NO_ERROR) {

    std::string jdl;
    assert( cmd -> getParam("jdl", jdl) );
    std::string myproxy;
    classad::ClassAd ad;
    classad::ClassAdParser parser;

    if( parser.ParseClassAd(jdl, ad) ) {
       std::string proxypath;
       std::string proxyhost;
       char *renewalproxypath = NULL;
       if ( ad.EvaluateAttrString("MyProxyServer", proxyhost) ) {

	  edglog(warning) << "Registering job for proxy renewal." << std::endl;

          if ( cmd -> getParam("X509UserProxy", proxypath) ) {
	     int i=10;
	     for (; 
	          i>0 && glite_renewal_RegisterProxy( (char*)proxypath.c_str(),
  			 	                      (char*)proxyhost.c_str(),
		                 	              7512,
						      edg_wlc_JobIdUnparse(*(cmd -> getLogJobId())),
		                        	      EDG_WLPR_FLAG_UNIQUE,
		      		             	      &renewalproxypath);
		  i--); 
	     // If registration was successful, log the job as accepted.
	     for (int j=0; 
		  j<10 && 
		    !edg_wll_SetParam(*(cmd -> getLogContext()),
				      EDG_WLL_PARAM_X509_PROXY,
				      (i > 0 ? 
				          renewalproxypath
				          :
				          proxypath.c_str())
       	          );
		  j++);

	     if (i>0) {
		cmd -> setParam("X509UserProxy", std::string(renewalproxypath));		
		LogAcceptedJob(cmd, true, true);
		cmd -> setParam("ProxyRenewalDone", true);
	     } else {
	        LogAbortedJob(cmd, "Error during proxy renewal registration", true, true);
		cmd -> setParam("ProxyRenewalDone", false);
	     }
	     free(renewalproxypath);
	     return true;
	  } else {
	     LogAbortedJob(cmd, "Error: user proxy pathname not found.", true, true);
	     cmd -> setParam("ProxyRenewalDone", false);
	  }	
       } else {
	  // no renewal registration needed
	  LogAcceptedJob( cmd, true, true);
	  cmd -> setParam("ProxyRenewalDone", true);	     
       }
       return true;
    } else {
      LogRefusedJob(cmd, "Error parsing jdl.", true, true);
      return false;
    }
  } else {
    std::string creationMsg("Message Unavailable.");
    cmd -> getParam("SDCreationMessage", creationMsg);
    LogRefusedJob(cmd, (char *)creationMsg.c_str(), true, true);  
    return false;
  }
}

static bool insertCertificateSubject(Command* cmd)
{
  edglog_fn("CFSI::insertCertSubj");
  edglog(info) << "Inserting Certificate Subject" << std::endl;
  socket_pp::GSISocketAgent *agent = 
	static_cast<socket_pp::GSISocketAgent*>(&cmd -> agent());
  std::string certificate_subject( std::string("CertificateSubject=\"") + 
				   agent -> CertificateSubject() + "\";" );    
  std::string jdl;
  cmd -> getParam("jdl", jdl);
  jdl.insert(1, certificate_subject);
  cmd -> setParam("jdl", jdl);
  return true;
}

static bool insertUserProxy(Command* cmd) 
{
  edglog_fn("CFSI::insertUserProxy");
  socket_pp::GSISocketAgent *agent = 
    static_cast<socket_pp::GSISocketAgent*>(&cmd -> agent()); 

  std::string local_path, cmd_name, credentials_file;

  cmd -> getParam ("Command", cmd_name);

  if (cmd_name == "ListJobMatch") {
    std::string pipe_path;
    cmd -> getParam ("file", pipe_path);
    edglog(fatal) << pipe_path  << std::endl;
    std::string::size_type idx = pipe_path.rfind("/");
    if (idx != std::string::npos)
      pipe_path = pipe_path.substr(idx + 1);
    edglog(fatal) << pipe_path  << std::endl;
    local_path.assign(singleton_default<ns::daemon::NetworkServer>::instance().configuration().list_match_root_path() + "/user.proxy." + pipe_path); 
    edglog(severe) << "Proxy copied into: " << local_path << std::endl;
  } else {
    edglog(severe) << "Local Path not set for User Proxy: setting default." << std::endl;
    local_path = std::string("/tmp/user.proxy");
  }

  credentials_file = agent -> CredentialsFile();
  if ( !credentials_file.empty() ) {
 
    if(!copy_file( credentials_file, local_path)){ 
      std::string error_msg( 
			  std::string("Unable to copy ") + agent -> CredentialsFile() + 
			  std::string(" to ") + local_path + std::string(" (") + strerror( errno ) + 
			  std::string(")") ); 
 
      edglog(fatal) << error_msg << std::endl; 
      cmd -> setParam ("ProxyCopyDone", false);
    } else { 
      cmd -> setParam("X509UserProxy", local_path); 
      cmd -> setParam("ProxyCopyDone", true); 
    } 
    
  } else {
    edglog(severe) << "Credentials File is empty." << std::endl; 
    cmd->setParam("X509UserProxy", std::string("")); 
    cmd -> setParam ("ProxyCopyDone", true); 
  }
  
  return true;
}

static bool removeUserProxy(Command* cmd) {

  edglog_fn("CFSI::removeUserProxy");
  std::string uproxy;
  bool uproxy_cpy;
  // bool remove_done = false;

  if ( cmd -> getParam("ProxyCopyDone", uproxy_cpy) && cmd -> getParam("X509UserProxy", uproxy) ) { 
    boost::filesystem::path uproxyfile( uproxy, fs::native);
    // remove_done = remove( uproxyfile ); switch these two lines when boost version changes
    edglog(debug) << "Removing: " << uproxy << std::endl;    
    remove(uproxyfile);
  }
  
  //  if (remove_done) {
  //  edglog(debug) << "Removed: " << uproxy << std::endl;    
  //} else {
  else { // al ripristino delle tre righe precedenti eliminare questa
    edglog(severe) << "Cannot remove  User Proxy file during listmatch. The following file" << std::endl;
    edglog(severe) << "could not be found or deleted:" << std::endl;
    edglog(severe) << uproxy << std::endl << "If still present, the file will be removed at next NS startup." << std::endl;
  }

  return true;
}

static bool checkJobSize(Command* cmd) {
  // Retrieves needed free space amount
  edglog_fn("CFSI::ckJobSize");
  edglog(info) << "Checking Job Size." << std::endl;
  double needed_space;
  double maxjobsize = singleton_default<ns::daemon::NetworkServer>::instance().configuration().max_input_sandbox_size();
  if ( cmd -> getParam("SandboxSize", needed_space) ) {
    if ( (needed_space <= maxjobsize) || (maxjobsize < 0)) {
      return ( cmd -> setParam("CheckSizePassed", true) );
    } else {
      LogRefusedJob(cmd, "Input Sandbox Size Not allowed.", true, false);
    }
  } 
  return cmd -> setParam("CheckSizePassed", false);  
}

static bool evaluateCheckSize(Command* cmd) {
   bool check;
   cmd -> getParam("CheckSizePassed", check);
   edglog_fn("CFSI::evCkJobSize");
   edglog(debug) << "Evaluating job size: " << (check ? "OK" : "KO") << std::endl;
   return check;
}

static bool checkUserQuota(Command* cmd) {

  edglog_fn("CFSI::ckUserQuota");
  edglog(info) << "Evaluating User Quota." << std::endl;
  if ( !(singleton_default<ns::daemon::NetworkServer>::instance().configuration().enable_quota_management()) ) {
     return cmd -> setParam("CheckQuotaPassed", true);	  
  }
 
  socket_pp::GSISocketAgent *agent = 
     static_cast<socket_pp::GSISocketAgent*>(&cmd -> agent());
  std::string uname( agent -> GridmapName() );
  
  std::pair<long, long> quota( utilities::quota::getQuota(uname) );

  if ( !(singleton_default<ns::daemon::NetworkServer>::instance().configuration().enable_dynamic_quota_adjustment()) ) {
     
    // if soft limit is zero ... 
    // we assume that the original quota was 0,0 and it must be
    // subject to dynamic adjustment.
    if ( !quota.first ) {
      // raise up quota
      // All params are taken from Ns configuration
      int uid = utilities::quota::user2uid(uname.c_str());
      std::string uid_str(boost::lexical_cast<std::string>(uid));
      std::string commandline(std::string("edg-wl-quota-adjustment ") + uid_str);
      if (!system(commandline.c_str())) {
	edglog(severe) << "Error while bumping up quota for user: " << uname << ", uid: " <<
	  uid << ". The job will not be submitted.";
	cmd -> setParam("Uid", uid_str);
	return cmd -> setParam("CheckQuotaPassed", false); 
      }           
    } else {
      // Even dinamic quota is active, the user has
      // a fixed amount of disk space. 
      // do the check 
      double needed_space;
      if ( cmd -> getParam("SandboxSize", needed_space) ) {
        // Get quota limits from OS
        // quotalimit is a pair <softFreeKb, hardFreeKb >
        quota = utilities::quota::getFreeQuota(uname);
        if (quota.second >= needed_space) {
           LogRefusedJob(cmd, "Job Size Not allowed.", true, false);
        }
        return cmd -> setParam("CheckQuotaPassed", 
			       (quota.second >= needed_space)); 
      }
      return cmd -> setParam("CheckQuotaPassed", false);       
    }
    
    // Succesful quota check in both cases
    return cmd -> setParam("CheckQuotaPassed", true); 

  } else {
		
     if (!quota.first && !quota.second) {
        return cmd -> setParam("CheckQuotaPassed", true);
     }
     double needed_space;
     if ( cmd -> getParam("SandboxSize", needed_space) ) {
        // Get quota limits from OS
        // quotalimit is a pair <softFreeKb, hardFreeKb >
        quota = utilities::quota::getFreeQuota(uname);
        if (quota.second >= needed_space) {
           LogRefusedJob(cmd, "Job Size Not allowed.", true, false);
        }
        return cmd -> setParam("CheckQuotaPassed", (quota.second >= needed_space)); 
     }
     return cmd -> setParam("CheckQuotaPassed", false); 
  }
}

static bool evaluateCheckQuota(Command* cmd) {
  bool check = false;
  cmd -> getParam("CheckQuotaPassed", check);
  edglog_fn("CFSI::evCheckQuota");
  edglog(debug) << "Evaluating user quota: " << (check ? "OK" : "KO") << std::endl;
  return check;
}

static bool evaluateClientCreateDirs(Command* cmd) {
  edglog_fn("CFSI::evClientCreateDirs");
  bool check = false;
  cmd -> getParam("ClientCreateDirsPassed", check);
  if (check)   edglog(info) << "Evaluating Remote Dirs Creation: Success." << std::endl;
  else         edglog(critical) << "Evaluating Remote Dirs Creation: Failure." << std::endl;
  return check;
}


static bool createReducedPathDirs(Command* cmd)
{ 
  edglog_fn("CFSI::crReducedPathDirs");
  edglog(info) << "Creating reduced path dirs." << std::endl;
  std::string jdl;
  assert( cmd -> getParam("jdl", jdl) );

  std::string jobid, cmdname;
  assert( cmd -> getParam ("Command", cmdname) );

  classad::ClassAd ad;
  classad::ClassAdParser parser;
  if( !parser.ParseClassAd(jdl, ad) ) {
    edglog(fatal) << "*** Error ***\n\t - Unable to parse Jdl string." << std::endl;
    return true;
  }
  ad.EvaluateAttrString("edg_jobid", jobid);

  // Must put an iteration here for level times as defined in configuration
  std::string root(singleton_default<ns::daemon::NetworkServer>::instance().configuration().sandbox_staging_path());
  std::string local_path(root + "/" + wmsutils::jobid::get_reduced_part( wmsutils::jobid::JobId(jobid), 0 ));
  
  if ( mkdir(local_path.c_str(), 0773) == -1) {
    if  (errno == EEXIST ) {
      edglog(severe) << local_path << " already exists." << std::endl;
    } else {
      edglog(fatal) << "* Error * Error in mkdir for reduced part:\n\t" << local_path << std::endl; 
      edglog(fatal) << "          Execution will proceede since directory can be created by client." << std::endl; 
      edglog(fatal) << "          IMPORTANT: Directory permissions will need to be manually changed." << std::endl; 
    }
  } else if (chmod(local_path.c_str(), 0773) == -1) {
    edglog(fatal) << "* Error * Error in chmod for reduced part:\n\t" << local_path << std::endl; 
    edglog(fatal) << "*       * Execution will procede since directory can be created by client." << std::endl; 
    edglog(fatal) << "          IMPORTANT: Directory permissions will need to be manually changed." << std::endl; 
  } else {
    edglog(warning) << "Created: " << local_path << std::endl;
  }
  
  if ( cmdname == "DagSubmit" ) {
    jdl::DAGAd dagad(ad); 
    jdl::DAGAd::node_iterator node_b; 
    jdl::DAGAd::node_iterator node_e;
    boost::tie(node_b, node_e) = dagad.nodes();
 
    while ( node_b != node_e) {
      const jdl::DAGNodeInfo& node_info = (*node_b).second;
      const classad::ClassAd* ad = node_info.description_ad();
 
      if (!ad->EvaluateAttrString("edg_jobid", jobid) ) {
        edglog(fatal) << "Error while retrieving jobId from classad." << std::endl;
        cmd -> setParam("SDCreationError", client::NSE_MKDIR);
        cmd -> setParam("SDCreationMessage", "Error while retrieving jobId from classad. Node: " + (*node_b).first);
        return true; 
      }
 
      local_path.assign( root + "/" + wmsutils::jobid::get_reduced_part( wmsutils::jobid::JobId(jobid), 0 ));
 
      edglog(fatal) << "Creating DAG Node directories. NodeId: " << jobid << std::endl;

      if ( mkdir(local_path.c_str(), 0773) == -1) { 
        if  (errno == EEXIST ) { 
          edglog(severe) << local_path << " already exists." << std::endl; 
        } else { 
          edglog(fatal) << "* Error * Error in mkdir for reduced part:\n\t" << local_path << std::endl; 
          edglog(fatal) << "          Execution will proceede since directory can be created by client." << std::endl; 
          edglog(fatal) << "          IMPORTANT: Directory permissions will need to be manually changed." << std::endl; 
        } 
      } else if (chmod(local_path.c_str(), 0773) == -1) { 
        edglog(fatal) << "* Error * Error in chmod for reduced part:\n\t" << local_path << std::endl; 
        edglog(fatal) << "*       * Execution will procede since directory can be created by client." << std::endl; 
        edglog(fatal) << "          IMPORTANT: Directory permissions will need to be manually changed." << std::endl; 
      } else { 
        edglog(warning) << "Created: " << local_path << std::endl; 
      } 

 
      node_b++; 
    }
  }




  // end of iteration
  return true;
}


static bool createStagingDirectories(Command* cmd)
{ 
  edglog_fn("CFSI::crStagingDirectories");

  char *local_account = NULL;
  std::string jdl;
  assert( cmd -> getParam("jdl", jdl) );
  
  std::string jobid;
  classad::ClassAd ad;
  classad::ClassAdParser parser;
  if( !parser.ParseClassAd(jdl, ad) ) {
    cmd->setParam("SDCreationMessage","Unable to parse Jdl expression.");
    cmd->setParam("SDCreationError", (int)client::NSE_JDL_PARSING);
    return true;
  }
  ad.EvaluateAttrString("edg_jobid", jobid);

  edglog(info) << "Creating staging dirs - job: " << jobid << std::endl;

  std::string authentication_key;
  std::string key_value;
  // <<<<<<<<<<<<<<<<<<<<<<<<<
  //singleton_default<daemon::NetworkServer>::instance().configuration().authentication_key();
  // =========================
  authentication_key="CertificateSubject";
  // >>>>>>>>>>>>>>>>>>>>>>>
  if( ! ad.EvaluateAttrString(authentication_key, key_value) ) {
    cmd->setParam("SDCreationMessage","Cannot retrieve Authentication Key.");
    cmd->setParam("SDCreationError", (int)client::NSE_AUTHENTICATION_ERROR);
    
    return true;
  }
  	
  if (globus_gss_assist_gridmap((char*)key_value.c_str(), &local_account)) { 
    std::string msg( "No authentication mapping for:" + key_value );
    cmd->setParam("SDCreationMessage",msg);
    cmd->setParam("SDCreationError", int(client::NSE_AUTHENTICATION_ERROR));
  } else {
    // *** Use next two lines instead of those
    // struct group *local_group = getgrnam(local_account);
    // if (local_group) {

    struct passwd *local_uinfo = getpwnam(local_account);
    if (local_uinfo) {
      
      std::string local_path(singleton_default<ns::daemon::NetworkServer>::instance().configuration().sandbox_staging_path() + "/" + nsjobid::to_filename( wmsutils::jobid::JobId(jobid) ));
      std::string input_sandbox_local_path(local_path + "/input");
      std::string output_sandbox_local_path(local_path + "/output");

#ifndef WITH_GLOBUS_FTP_CLIENT_API	
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
	edglog(error) << "Warning: could not obtain PW information about UID " <<
	  input_sandbox_local_path << std::endl;
      }

#endif

      // user-proxy copy patch
      socket_pp::GSISocketAgent *agent = 
	static_cast<socket_pp::GSISocketAgent*>(&cmd -> agent());
      
      if(!copy_file( agent -> CredentialsFile(), local_path +"/user.proxy")){
	
	std::string error_msg( 
	     std::string("Unable to copy ") + agent -> CredentialsFile() +
	     std::string(" to ") + local_path + std::string("/user.proxy") + 
	     std::string(" (") + strerror( errno ) + std::string(")") );
	
	edglog(critical) << error_msg << std::endl;
	cmd->setParam("SDCreationMessage", error_msg);
	cmd->setParam("SDCreationError",(int)client::NSE_AUTHENTICATION_ERROR);
      } 
      else {
	cmd->setParam("X509UserProxy", local_path + "/user.proxy");	
	cmd->setParam("SDCreationMessage","No Error");
	cmd->setParam("SDCreationError", (int)client::NSE_NO_ERROR);
     } 
    } 
    else {
      std::string msg( "Could not find any UNIX user named: " + std::string( local_account ) );
      edglog(critical) << msg << std::endl;
      cmd->setParam("SDCreationMessage", msg);
      cmd->setParam("SDCreationError", (int)client::NSE_AUTHENTICATION_ERROR);
    }
  }
 
  if( local_account ) {
    free( local_account );
  }
  return true;
}

static bool createContext(Command* cmd) {

  edglog_fn("CFSI::crContext");

  if (edg_wll_InitContext( cmd->getLogContext() ) != 0) {
    // just signalize it, if needed
    edglog(fatal) << "Error while initializing the context." << std::endl;
    return false;
  }

  if (edg_wll_SetParam( *(cmd->getLogContext()), 
			EDG_WLL_PARAM_SOURCE, 
                        EDG_WLL_SOURCE_NETWORK_SERVER  ) != 0) {
    edglog(fatal) << "Error setting source parameter in logging context." << std::endl;
    return false;
  }
  
  std::string cmdname;
  assert( cmd -> getParam( "Command", cmdname ) );  
  edglog(info) << "Creating Context for " << cmdname << "." << std::endl;

  std::string sequence_code;
  std::string dg_jobid;

  // we initialize the context usign different sources depending on cmd.
  if ( cmdname == "JobSubmit" || cmdname == "DagSubmit" ) {
    // get the sequence code from jdl Classad
    // string sequence_code ......
    std::string jdl;
    assert( cmd -> getParam("jdl", jdl) );
  
    classad::ClassAd ad;
    classad::ClassAdParser parser;
    
    if( !parser.ParseClassAd(jdl, ad) ) {
      edglog(fatal) << "Error in parsing jdl getting SequenceCode" << std::endl;
      return false;
    }
    
    ad.EvaluateAttrString("LB_sequence_code", sequence_code);  
    ad.EvaluateAttrString("edg_jobid", dg_jobid);

  } else if ( ( cmdname == "JobCancel" ) || (cmdname == "JobPurge") )  {

    std::string path;
    assert (cmd -> getParam("JobPath", path));
    assert (cmd -> getParam("JobId", dg_jobid));

    fs::path seqfile( path + "/" + ".edg_wll_seq", fs::native); 
    std::ifstream seqfilestream( seqfile.native_file_string().c_str() );
    if( seqfilestream ) { 
      seqfilestream >> sequence_code;
    } 
    else { 
      edglog(severe) << path << "/.edg_wll_seq does not exist " << std::endl; 
      return false; 
    }
  }

  edglog(info) << "Setting LB parameters." << std::endl;
  std::string dname;
  cmd -> getParam("DistinguishedName", dname);

  if( !(edg_wlc_JobIdParse(dg_jobid.c_str(), cmd->getLogJobId())) ) {
#ifdef GLITE_WMS_HAVE_LBPROXY
    if ( !(edg_wll_SetLoggingJobProxy(*(cmd->getLogContext()), 
				      *(cmd->getLogJobId()), 
				      sequence_code.c_str(), 
				      dname.c_str(),
				      EDG_WLL_SEQ_NORMAL)) ) {
#else
    if ( !(edg_wll_SetLoggingJob(*(cmd->getLogContext()), 
			         *(cmd->getLogJobId()), 
			         sequence_code.c_str(), 
			         EDG_WLL_SEQ_NORMAL)) ) {
#endif
      if ( !(edg_wll_SetParamInt( *(cmd->getLogContext()), 
				  EDG_WLL_PARAM_SOURCE, 
				  EDG_WLL_SOURCE_NETWORK_SERVER)) ) {
	if ( edg_wll_SetParamString(*(cmd->getLogContext()), 
				    EDG_WLL_PARAM_INSTANCE, 
				    (boost::lexical_cast<std::string>(singleton_default<ns::daemon::NetworkServer>::instance().configuration().listening_port())).c_str() 
				    ) == 0 ) {
	  // Not needed. The context, when created, already refers to host proxy.
	  // const configuration::CommonConfiguration *comm = configuration::Configuration::instance()->common();
	  // if ( edg_wll_SetParam( *(cmd->getLogContext()), 
	  //			 EDG_WLL_PARAM_X509_PROXY, 
	  //			 comm->host_proxy_file().c_str() ) == 0)
	  return true;
	  // else edglog(fatal) << "Cannot set proxyfile path inside context." << std::endl;
	} else {
	  edglog(fatal) << "Error setting logging Instance: CreateContext." << std::endl; 
	  }
      } else {
	edglog(fatal) << "Error setting source parameter in logging context." << std::endl;
      }
    } else {
      edglog(fatal) << "Error setting logging Job: CreateContext." << std::endl;
    }
  } else {
    edglog(fatal) << "Error parsing JobId: CreateContext." << std::endl;    
  }

  return false;
  
}

bool listOutputFiles(Command *cmd) {
  edglog_fn("CFSI::listOutputFiles");
  edglog(info) << "Preparing to Purge." << std::endl;

  std::vector<std::string> v;
  std::string job;
  
  cmd -> getParam("JobId", job);
  fs::path p( 
    singleton_default<ns::daemon::NetworkServer>::instance().configuration().sandbox_staging_path(), 
    fs::native
  );
  p /= fs::path(nsjobid::to_filename( wmsutils::jobid::JobId(job) ), fs::native);
  p /= fs::path("output", fs::native);

  list_files( p, v);

  bool result = false;
  cmd -> getParam ("Command", job);
  if ( job == "GetOutputFilesList" ) {
    result = cmd -> setParam("OutputFilesList",v);
  } else if ( job == "GetOutputFilesListSize" ) {
    result = cmd -> setParam("OutputFilesListSize", (int)v.size());
  } else {
    edglog(error) << logger::setfunction("CFSI::listOutFiles()") << "Unknown Command Name." << std::endl;
  }

  return result;
} 

bool proxyRenewalCheck(Command *cmd) {
  edglog_fn("CFSI::ckProxyRen");
  bool proxyresult = false;
  cmd -> getParam( "ProxyRenewalDone", proxyresult );
  edglog(warning) << "Proxy renewal: " << proxyresult << std::endl;
  return proxyresult;
}

static void testAndLog( Command* cmd, int &code, bool &with_hp, int &lap)
{
  edglog_fn("CFSI::test&Log");
  const configuration::CommonConfiguration *conf = configuration::Configuration::instance()->common();
  int          ret;
  std::string  host_proxy;

  if( code ) {

    switch( code ) {
    case EINVAL:
      edglog(critical) << "Critical error in L&B calls: EINVAL." << std::endl;
      code = 0; // Don't retry...
      break;

    case EDG_WLL_ERROR_GSS:
      edglog(severe) << "Severe error in SSL layer while communicating with L&B daemons." << std::endl;

      if( with_hp ) {
        edglog(severe) << "The log with the host certificate has just been done. Giving up." << std::endl;
	code = 0; // Don't retry...
      }
      else {
	edglog(info) << "Retrying using host proxy certificate..." << std::endl;
	host_proxy = conf->host_proxy_file();

	if( host_proxy.length() == 0 ) {
	  edglog(warning) << "Host proxy file not set inside configuration file." << std::endl
			  << "Trying with a default NULL and hoping for the best." << std::endl;
	  ret = edg_wll_SetParam( *(cmd->getLogContext()), EDG_WLL_PARAM_X509_PROXY, NULL );
	}
	else {
	  edglog(info) << "Host proxy file found = \"" << host_proxy << "\"." << std::endl;
	  ret = edg_wll_SetParam( *(cmd->getLogContext()), EDG_WLL_PARAM_X509_PROXY, host_proxy.c_str() );
	}

	if( ret ) {
	  edglog(severe) << "Cannot set the host proxy inside the context. Giving up." << std::endl;
	  code = 0; // Don't retry.
	}
	else with_hp = true; // Set and retry (code is still != 0)
      }

      break;

    default:
      if( ++lap > 3 ) {
	edglog(error) << "L&B call retried " << lap << " times always failed." << std::endl
		   << "Ignoring." << std::endl;
	code = 0; // Don't retry anymore
      }
      else {
	edglog(warning) << "L&B call got a transient error. Waiting 60 seconds and trying again." << std::endl;
	edglog(info) << "Try n. " << lap << "/3" << std::endl;
	sleep( 10 );
      }

      break;
    }
  }
  else // The logging call worked fine, do nothing
    edglog(debug) << "L&B call succeeded." << std::endl;

  return;
}

static void reset_user_proxy( Command* cmd, const std::string &proxyfile )
{
  edglog_fn("CFSI::resetUserProxy");
  bool    erase = false;
  int     res;

  if( proxyfile.size() ) {
    fs::path pf( fs::normalize_path(proxyfile), fs::native );

    if( fs::exists(pf) ) {
      res = edg_wll_SetParam( *(cmd->getLogContext()), EDG_WLL_PARAM_X509_PROXY, proxyfile.c_str() );
      if( res ) edglog(severe) << "Cannot set proxyfile path inside context." << std::endl;
    }
    else erase = true;
  }
  else if( proxyfile.size() == 0 ) erase = true;

  if( erase ) {
    res = edg_wll_SetParam( *(cmd->getLogContext()), EDG_WLL_PARAM_X509_PROXY, NULL );
    if( res ) edglog(severe) << "Cannot reset proxyfile path inside context." << std::endl;;
  }

}


static bool LogRefusedJob(Command *cmd, char *reason, bool retry){
  
  int i=0;
  edglog_fn("CFSI::LogRefusedJobN");
  edglog(fatal) << "Logging Refused Job: " << reason << std::endl; 
  std::string hn( retrieveHostName() );
  bool logged = false;

  for (; i < 3 && !logged && retry; i++) {
#ifdef GLITE_WMS_HAVE_LBPROXY
    logged = !edg_wll_LogRefusedProxy (*(cmd->getLogContext()), 
				       EDG_WLL_SOURCE_USER_INTERFACE, 
				       hn.c_str(),
				       "",
				       reason);
#else
    logged = !edg_wll_LogRefused (*(cmd->getLogContext()), 
				  EDG_WLL_SOURCE_USER_INTERFACE, 
				  hn.c_str(),
				  "",
				  reason);
#endif
    if (!logged && (i<2) && retry) {
      edglog(info) << "Failed to log Refused Job. Sleeping 60 seconds before retry." << std::endl;
      sleep(60);
    }
  }

  if ((retry && (i>=3)) || (!retry && (i>0)) ) {
    edglog(severe) << "Error while logging Refused Job." << std::endl;
    return false;
  }
  
  edglog(debug) << "Logged." << std::endl;
  return true;
}

static bool LogRefusedJob(Command *cmd, char *reason, bool retry, bool test) { 
  
  if (!test) return LogRefusedJob(cmd, reason, retry);
  edglog_fn("CFSI::LogRefusedJobE");
  edglog(fatal) << "Logging Refused Job: " << reason << std::endl; 

  int res;
  bool with_hp = false;
  int lap = 0;
  std::string hn( retrieveHostName() );
  std::string user_proxy;
  cmd->getParam("X509UserProxy", user_proxy);
  reset_user_proxy(cmd, user_proxy);

  do {
#ifdef GLITE_WMS_HAVE_LBPROXY
    res = edg_wll_LogRefusedProxy (*(cmd->getLogContext()),
				   EDG_WLL_SOURCE_USER_INTERFACE,
				   hn.c_str(),
				   "", 
				   reason);
#else
    res = edg_wll_LogRefused (*(cmd->getLogContext()),
			      EDG_WLL_SOURCE_USER_INTERFACE,
			      hn.c_str(),
			      "", 
			      reason);
#endif
    
    testAndLog( cmd, res, with_hp, lap );
  } while( res != 0 );

  return true;
}


static bool LogAcceptedJob(Command *cmd, bool retry){

  edglog_fn("CFSI::LogAcceptedJobN");
  edglog(fatal) << "Logging Accepted Job." << std::endl; 
  int i=0;
  std::string hn( retrieveHostName() );
  bool logged = false;

  for (; i < 3 && !logged && retry; i++) {
#ifdef GLITE_WMS_HAVE_LBPROXY
    logged = !edg_wll_LogAcceptedProxy(*(cmd->getLogContext()), 
				       EDG_WLL_SOURCE_USER_INTERFACE, 
				       hn.c_str(),
				       "",
				       "");
#else
    logged = !edg_wll_LogAccepted(*(cmd->getLogContext()), 
				  EDG_WLL_SOURCE_USER_INTERFACE, 
				  hn.c_str(),
				  "",
				  "");
#endif
    if (!logged && (i<2) && retry) {
      edglog(info) << "Failed to log Accepted Job. Sleeping 60 seconds before retry." << std::endl;
      sleep(60);
    }
  }
  
  if ((retry && (i>=3)) || (!retry && (i>0)) ) {
    edglog(severe) << "Error while logging Accepted Job." << std::endl;
    return false;
  }
  
  edglog(debug) << "Logged." << std::endl;
  return true;
}

static bool LogAcceptedJob(Command *cmd, bool retry, bool test) { 
  
  if (!test) return LogAcceptedJob(cmd, retry);
  edglog_fn("CFSI::LogAcceptedJobE");
  edglog(fatal) << "Logging Accepted Job." << std::endl;

  int res;
  bool with_hp = false;
  int lap = 0;
  std::string hn( retrieveHostName() );
  std::string user_proxy;
  cmd->getParam("X509UserProxy", user_proxy);
  reset_user_proxy(cmd, user_proxy);

  do {
#ifdef GLITE_WMS_HAVE_LBPROXY
    res = edg_wll_LogAcceptedProxy (*(cmd->getLogContext()),
				    EDG_WLL_SOURCE_USER_INTERFACE,
				    hn.c_str(),
				    "", 
				    "");
#else
    res = edg_wll_LogAccepted (*(cmd->getLogContext()),
			       EDG_WLL_SOURCE_USER_INTERFACE,
			       hn.c_str(),
			       "", 
			       "");
#endif

    testAndLog( cmd, res, with_hp, lap );
  } while( res != 0 );

  return true;
}

/*
static bool LogEnqueuedJob(Command *cmd, char* file_queue,
                           bool mode, char* reason, bool retry){
  int i=0;
  edglog_fn("CFSI::LogEnqueuedJobN");
  edglog(fatal) << "Logging Enqueued Job." << std::endl; 
  bool logged = false;
  std::string jdl;
  cmd -> getParam("jdl", jdl);

  for (; i < 3 && !logged && retry; i++) {
      logged = !edg_wll_LogEnQueuedProxy (*(cmd->getLogContext()), 
                                          file_queue, 
			                  jdl.c_str(), 
			                  (mode ? "OK" : "FAIL"), 
			                  reason); 
      if (!logged && (i<2) && retry) {
        edglog(info) << "Failed to log Enqueued Job. Sleeping 60 seconds before retry." << std::endl;
	sleep(60);
      }
  }

  if ((retry && (i>=3)) || (!retry && (i>0)) ) {
    edglog(severe) << "Error while logging Enqueued Job." << std::endl;
    return false;
  }
  
  edglog(debug) << "Logged." << std::endl;
  edg_wll_FreeContext(*(cmd->getLogContext()));
  return true;
}

static bool LogEnqueuedJob(Command *cmd, char* file_queue,
                           bool mode, char* reason, bool retry, bool test) { 
  
  if (!test) return LogEnqueuedJob(cmd, file_queue, mode, reason, retry);
  edglog_fn("CFSI::LogEnqueuedJobE");
  edglog(fatal) << "Logging Enqueued Job." << std::endl;

  int res;
  bool with_hp = false;
  int lap = 0;
  std::string jdl;
  cmd -> getParam("jdl", jdl);
  std::string user_proxy;
  cmd->getParam("X509UserProxy", user_proxy);
  reset_user_proxy(cmd, user_proxy);

  do {
    res = edg_wll_LogEnQueuedProxy (*(cmd->getLogContext()), 
                                    file_queue, 
			            jdl.c_str(), 
			            (mode ? "OK" : "FAIL"), 
			            reason); 
    testAndLog( cmd, res, with_hp, lap );
  } while( res != 0 );

  return true;
}
*/

static bool LogCancelJob(Command *cmd, char* reason, bool retry){
 
  int i=0; 
  edglog_fn("CFSI::LogCancelJobN");
  edglog(fatal) << "Logging Cancel Request." << std::endl;
  bool logged = false;
  
  for (; i < 3 && !logged && retry; i++) {
#ifdef GLITE_WMS_HAVE_LBPROXY
    logged = !edg_wll_LogCancelREQProxy(*(cmd->getLogContext()),
					reason);
#else
    logged = !edg_wll_LogCancelREQ(*(cmd->getLogContext()),
				   reason);
#endif
    if (!logged && (i<2) && retry) {
      edglog(info) << "Failed to log Cancelled Job. Sleeping 60 seconds before retry." << std::endl;
      sleep(60);
    }
  }

  if ((retry && (i>=3)) || (!retry && (i>0)) ) {
    edglog(severe) << "Error while logging Cancelled Job." << std::endl;
    return false;
  }
  
  edglog(debug) << "Logged." << std::endl;
  return true;
} 

static bool LogCancelJob(Command *cmd, char* reason, bool retry, bool test) { 
  
  if (!test) return LogCancelJob(cmd, reason, retry);
  edglog_fn("CFSI::LogCancelJobE");
  edglog(fatal) << "Logging Cancel Request." << std::endl;

  int res;
  bool with_hp = false;
  int lap = 0;
  std::string user_proxy;
  cmd->getParam("X509UserProxy", user_proxy);
  reset_user_proxy(cmd, user_proxy);

  do {
#ifdef GLITE_WMS_HAVE_LBPROXY
    res = edg_wll_LogCancelREQProxy(*(cmd->getLogContext()),
			            reason);
#else
    res = edg_wll_LogCancelREQ(*(cmd->getLogContext()),
			       reason);
#endif
    testAndLog( cmd, res, with_hp, lap );
  } while( res != 0 );

  return true;
}

static bool LogPurgeJob(Command *cmd, bool retry){

  int i=0;
  edglog_fn("CFSI::LogPurgeJobN");
  edglog(fatal) << "Logging Purge Request." << std::endl;
  bool logged = false;

  for (; i < 10 && !logged && retry; i++) {
#ifdef GLITE_WMS_HAVE_LBPROXY
    logged = !edg_wll_LogClearUSERProxy(*(cmd->getLogContext()));
#else
    logged = !edg_wll_LogClearUSER(*(cmd->getLogContext()));
#endif
    if (!logged && (i<2) && retry) {
      edglog(info) << "Failed to log Purged Job. Sleeping 60 seconds before retry." << std::endl;
      sleep(60);
    }
  }
  
  if ((retry && (i>=3)) || (!retry && (i>0)) ) {
    edglog(severe) << "Error while logging Purged Job." << std::endl;
    return false;
  }
  
  edglog(debug) << "Logged." << std::endl;
  return true;
}

static bool LogPurgeJob(Command *cmd, bool retry, bool test) { 
  
  if (!test) return LogPurgeJob(cmd, retry);
  edglog_fn("CFSI::LogPurgeJobE");
  edglog(fatal) << "Logging Purge Request." << std::endl; 

  int res;
  bool with_hp = false;
  int lap = 0;
  std::string user_proxy;
  cmd->getParam("X509UserProxy", user_proxy);
  reset_user_proxy(cmd, user_proxy);

  do {
#ifdef GLITE_WMS_HAVE_LBPROXY
    res = edg_wll_LogClearUSERProxy(*(cmd->getLogContext()));
#else
    res = edg_wll_LogClearUSER(*(cmd->getLogContext()));
#endif
    testAndLog( cmd, res, with_hp, lap );
  } while( res != 0 );

  return true;
}


static bool LogAbortedJob(Command *cmd, char* reason, bool retry){

  int i=0;  
  edglog_fn("CFSI::LogAbortedJobN");
  edglog(fatal) << "Logging Aborted Job." << std::endl; 
  bool logged = false;

  for (; i < 10 && !logged && retry; i++) {
#ifdef GLITE_WMS_HAVE_LBPROXY
    logged = !edg_wll_LogAbortProxy(*(cmd->getLogContext()),
				    reason); 
#else
    logged = !edg_wll_LogAbort(*(cmd->getLogContext()),
			       reason); 
#endif
    if (!logged && (i<2) && retry) {
      edglog(info) << "Failed to log Purged Job. Sleeping 60 seconds before retry." << std::endl;
      sleep(60);
    }
  }

  if ((retry && (i>=3)) || (!retry && (i>0)) ) {
    edglog(severe) << "Error while logging Aborted Job." << std::endl;
    return false;
  }
  
  edglog(debug) << "Logged." << std::endl;
  return true;
}

static bool LogAbortedJob(Command *cmd, char* reason, bool retry, bool test) { 
  
  if (!test) return LogAbortedJob(cmd, reason, retry);
  edglog_fn("CFSI::LogAbortedJobE");
  edglog(fatal) << "Logging Aborted Job. Reason: " << std::string(reason)<< std::endl;

  int res;
  bool with_hp = false;
  int lap = 0;
  std::string user_proxy;
  cmd->getParam("X509UserProxy", user_proxy);
  reset_user_proxy(cmd, user_proxy);

  do {
#ifdef GLITE_WMS_HAVE_LBPROXY
    res = edg_wll_LogAbortProxy(*(cmd->getLogContext()),
			        reason);
#else
    res = edg_wll_LogAbort(*(cmd->getLogContext()),
			   reason);
#endif

    testAndLog( cmd, res, with_hp, lap );
  } while( res != 0 );

  return true;
}


static bool logCancel(Command *cmd){
  return LogCancelJob(cmd, 
		      "Logging Cancel Request.",
		      true,
		      false);
}

Command* CommandFactoryServerImpl::create(const std::string& cmdstr)
{
  std::string name;
  int minor = 0, major = 2, rev = 0;
  versions::Version ver;

  edglog_fn("CFSI::crFSM");
  edglog(info) << "Creating FSM." << std::endl;

  if (cmdstr.find(":") == std::string::npos) {
    name.assign(cmdstr);
  } else {
    try {
      static boost::regex ex("(.*):([0-9]+).([0-9]+).([0-9]+)");
      boost::smatch pieces;
      if ( boost::regex_match (cmdstr, pieces, ex) ) {
	name.assign(pieces[1].first, pieces[1].second);
	major=std::atoi(std::string(pieces[2].first, pieces[2].second).c_str());
	minor=std::atoi(std::string(pieces[3].first, pieces[3].second).c_str());
	rev=std::atoi(std::string(pieces[4].first, pieces[4].second).c_str());
      } else {
	edglog(fatal) << "Wrong Client Version: command string does not match regex: " << cmdstr << ".\n\tGuessing default version: " << major << "." << minor << "." << rev << std::endl;
      }
    } catch (boost::bad_expression& e) {
      edglog(fatal) << "Wrong Version: bad RegularExpression: " << "\n\t" << e.what() << "\n\tGuessing default version: " << major << "." << minor << "." << rev << std::endl;
    }
  }

  ver = versions::Version(major, minor, rev);

  Command* cmd = new Command;
  classad::ClassAdParser parser;
  if (cmd -> ad) {
    delete (cmd -> ad);
    cmd -> ad = NULL;
  }
  cmd -> ad = parser.ParseClassAd("[Arguments=[];]");
  if (cmd -> fsm) {
    delete (cmd -> fsm);
    cmd -> fsm = NULL;
  }
  cmd -> fsm = new fsm::state_machine_t;
  cmd -> serializeImpl = serializeServerImpl;
  cmd -> ad -> InsertAttr("Command", name);
  cmd -> ad -> InsertAttr("ServerVersion", SERVER_VERSION);
  cmd -> ad -> InsertAttr("ClientVersion", ver.asString());

  edglog(info) << "Command Received..: " << name << std::endl; 
  edglog(info) << "Client  Version...: " << ver.asString() << std::endl; 

  fsm::CommandState::shared_ptr state;
  
  if( name == "JobSubmit" ) {        
    state.reset( new fsm::ExecuteFunction(createContext) );
    cmd -> fsm -> push (state);
    state.reset(new fsm::ReceiveLong("SandboxSize") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(checkJobSize) );
    cmd -> fsm -> push(state);   
    state.reset(new fsm::SendBoolean("CheckSizePassed") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(evaluateCheckSize) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(checkUserQuota) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendBoolean("CheckQuotaPassed") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(evaluateCheckQuota) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(checkSpace) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendBoolean("CheckPassed") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(insertCertificateSubject) );
    cmd -> fsm -> push(state);
    state.reset( new fsm::ExecuteFunction(setJobPaths) );
    cmd -> fsm -> push(state);
    state.reset( new fsm::ExecuteFunction(createReducedPathDirs) );
    cmd -> fsm -> push(state);
#ifdef WITH_GLOBUS_FTP_CLIENT_API
    state.reset(new fsm::SendString("InputSandboxPath") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendString("OutputSandboxPath") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ReceiveBoolean("ClientCreateDirsPassed") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(evaluateClientCreateDirs) );
    cmd -> fsm -> push(state);
#endif
    state.reset(new fsm::ExecuteFunction(createStagingDirectories) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendInt("SDCreationError") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendString("SDCreationMessage") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(stagingDirectoryCreationPostControl));
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendBoolean("ProxyRenewalDone"));
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(proxyRenewalCheck));
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendString("InputSandboxPath") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ReceiveBoolean("TransferDone") );
    cmd -> fsm -> push(state);
    state.reset( new fsm::ForwardRequest() );
    cmd -> fsm -> push(state);
  } 
  else if( name =="GetMultiAttributeList" ) {
    state.reset( new fsm::ExecuteFunction(setMultiValuedAttributes) );
    cmd -> fsm -> push(state);
    state.reset( new fsm::SendVector("MultiAttributeList") );
    cmd -> fsm -> push(state);
  }
  else if (name == "JobCancel") {
    state.reset(new fsm::ReceiveString("JobId") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(setJobPaths) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(createContext) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(logCancel) );
    cmd -> fsm -> push(state);
    state.reset( new fsm::ForwardRequest() );
    cmd -> fsm -> push(state);    
  }
  else if (name == "GetSandboxRootPath") {
    state.reset(new fsm::ExecuteFunction(setSandboxRootPath) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendString("SandboxRootPath") );
    cmd -> fsm -> push(state);
  }
  else if (name == "GetQuotaManagementStatus") {
    state.reset(new fsm::ExecuteFunction(isQuotaManagementOn) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendBoolean("QuotaOn") );
    cmd -> fsm -> push(state);
  }
  else if (name == "GetMaxInputSandboxSize") {
    state.reset(new fsm::ExecuteFunction(setMaxInputSandboxSize) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendLong("MaxInputSandboxSize") );
    cmd -> fsm -> push(state);    
  }
  else if (name == "GetQuota") {
    state.reset(new fsm::ExecuteFunction(setUserQuota) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendLong("SoftLimit") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendLong("HardLimit") );
    cmd -> fsm -> push(state);
  }
  else if (name == "GetFreeQuota") {
    state.reset(new fsm::ExecuteFunction(setUserFreeQuota) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendLong("SoftLimit") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendLong("HardLimit") );
    cmd -> fsm -> push(state);
  }  
  else if (name == "JobPurge") {
    state.reset(new fsm::ReceiveString("JobId") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(setJobPaths) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(createContext) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(doPurge) );
    cmd -> fsm -> push(state);
  }
  else if (name == "ListJobMatch") {
    state.reset(new fsm::ReceiveString("jdl"));
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(insertCertificateSubject) );
    cmd -> fsm -> push(state);     
    state.reset(new fsm::ExecuteFunction(insertPipePath));
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(insertUserProxy) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ForwardRequest());
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(listjobmatchex)); 
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(removeUserProxy) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendVector("MatchResult"));
    cmd -> fsm -> push(state); 
  }
  else if (name == "GetOutputFilesList") {
    state.reset(new fsm::ReceiveString("JobId"));
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(listOutputFiles));
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendVector("OutputFilesList"));
    cmd -> fsm -> push(state);
  }
  else if (name == "GetOutputFilesListSize") {
    state.reset(new fsm::ReceiveString("JobId"));
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(listOutputFiles));
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendInt("OutputFilesListSize"));
    cmd -> fsm -> push(state);
  }
  else if( name == "DagSubmit" ) {        
    state.reset( new fsm::ExecuteFunction(createContext) );
    cmd -> fsm -> push (state);
    state.reset(new fsm::ReceiveLong("SandboxSize") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(checkJobSize) );
    cmd -> fsm -> push(state);   
    state.reset(new fsm::SendBoolean("CheckSizePassed") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(evaluateCheckSize) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(checkUserQuota) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendBoolean("CheckQuotaPassed") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(evaluateCheckQuota) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(checkSpace) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendBoolean("CheckPassed") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(insertCertificateSubject) );
    cmd -> fsm -> push(state);
#ifdef WITH_GLOBUS_FTP_CLIENT_API
    state.reset(new fsm::ExecuteFunction(setJobPaths) );
    cmd -> fsm -> push(state);
    state.reset( new fsm::ExecuteFunction(createReducedPathDirs) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(setSandboxRootPath) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendString("SandboxRootPath") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ReceiveBoolean("ClientCreateDirsPassed") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(evaluateClientCreateDirs) );
    cmd -> fsm -> push(state);
#endif
    state.reset(new fsm::ExecuteFunction(dag::createStagingDirectories) );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendInt("SDCreationError") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendString("SDCreationMessage") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(stagingDirectoryCreationPostControl));
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendBoolean("ProxyRenewalDone"));
    cmd -> fsm -> push(state);
    state.reset(new fsm::ExecuteFunction(proxyRenewalCheck));
    cmd -> fsm -> push(state);
    state.reset(new fsm::SendString("InputSandboxPath") );
    cmd -> fsm -> push(state);
    state.reset(new fsm::ReceiveBoolean("TransferDone") );
    cmd -> fsm -> push(state);
    state.reset( new fsm::ForwardRequest() );
    cmd -> fsm -> push(state);
  } 
  else {
    delete cmd;
    cmd = 0;
  }
  return cmd;
}

} // namespace glite
} // namespace wms
} // namespace manager
} // namespace ns
} // namespace commands
