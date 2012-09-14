/*
 * CommandFactoryClientImpl.cpp
 *
 * Copyright (c) 2001, 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */


// $Id$

#include <classad_distribution.h> 

#include "CommandState.h"
#include "SendString.h"
#include "SendBoolean.h"
#include "SendLong.h"
#include "ReceiveBoolean.h"
#include "ReceiveString.h"
#include "ReceiveLong.h"
#include "ReceiveVector.h"
#include "ReceiveInt.h"
#include "ExecuteFunction.h"

#include "CommandFactoryClientImpl.h"
#include "DagCommandFactoryClientImpl.h"
#include "Command.h"
#include "common.h"
#include "logging.h"
#include "../versions/client_ver.h"

#include "glite/wmsutils/tls/socket++/SocketAgent.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#include "exception_codes.h"
	
#ifdef WITH_GLOBUS_FTP_CLIENT_API 
#include "glite/wms/common/utilities/globus_ftp_utils.h" 
#include "gsimkdirex.h"
#endif 
 
namespace logger    = glite::wms::common::logger;
namespace socket_pp = glite::wmsutils::tls::socket_pp;
namespace utils = glite::wmsutils::classads;
namespace utilities = glite::wms::common::utilities;
namespace client    = glite::wms::manager::ns::client;

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

#ifdef WITH_GLOBUS_FTP_CLIENT_API
bool createRemoteDirs( Command* cmd) {
  edglog_fn("CFCI::crRemoteDirs");
  edglog(info) << " ...creating... " << std::endl;
  std::string to_host, isb_path, osb_path;
  cmd -> setParam("ClientCreateDirsPassed", false);

  edglog(debug) << utils::asString(cmd->asClassAd()) << std::endl; 

  if ( cmd -> getParam("Host", to_host)
       && 
       cmd -> getParam("InputSandboxPath", isb_path) 
       && 
       cmd -> getParam("OutputSandboxPath", osb_path) 
       ) {

      std::string dst(to_host+isb_path);
      size_t job_dir_pos  = dst.rfind('/');
      //Check for trailing slash.
      if (job_dir_pos==dst.length()-1) job_dir_pos = dst.rfind('/',job_dir_pos-1);
      std::string job_dir(dst.substr(0, job_dir_pos));

      edglog(debug) << "Job dir.: " << job_dir << std::endl;
      edglog(debug) << "Host....: " << to_host << std::endl;
      edglog(debug) << "ISB.....: " << isb_path << std::endl;
      edglog(debug) << "OSB.....: " << osb_path << std::endl;

      //Assuming that parent directories exist
      if (!(utilities::globus::mkdir(std::string("gsiftp://") + job_dir))) {
            edglog(critical) << "Cannot create job directory on NS: "<< job_dir << std::endl;
	    cmd -> setParam("SDCreationError", client::NSE_MKDIR);
	    cmd -> setParam("SDCreationMessage", "Cannot create directory on NS: " + job_dir);
	    return true;
      } else {
	edglog(info) << "Attempting to create directory"<< job_dir << std::endl;
	if (!gsimkdirex(job_dir)) {
	  edglog(critical) << "Cannot create directory on NS: "<< job_dir << std::endl;
	  cmd -> setParam("SDCreationError", client::NSE_MKDIR);
	  cmd -> setParam("SDCreationMessage", "Cannot create directory on NS: " + job_dir);
	  return true;
	}
      }
  
      if (!gsimkdirex(to_host + isb_path, job_dir)) {
            edglog(critical) << "Cannot create directory on NS: "<< to_host << isb_path << std::endl;
	    cmd -> setParam("SDCreationError", client::NSE_MKDIR);
            cmd -> setParam("SDCreationMessage", "Cannot create directory on NS: " + to_host + isb_path);
            return true;
      }
  
      if (!gsimkdirex(to_host + osb_path, job_dir)) {
            edglog(critical) << "Cannot create directory on NS: "<< to_host << osb_path << std::endl;
            cmd -> setParam("SDCreationError", client::NSE_MKDIR);
            cmd -> setParam("SDCreationMessage", "Cannot create directory on NS: " + to_host + osb_path);
	    return true;
      }
     
      cmd -> setParam("ClientCreateDirsPassed", true);
      edglog(info) << "Remote Dirs Creation Successful" << std::endl;

  } else {
    std::string errstr("Cannot perform globus::mkdir with\n\t Host: " + to_host + "\n\t ISB:" + isb_path + "\n\t OSB:" + osb_path);
    cmd -> setParam("SDCreationError", client::NSE_MKDIR);
    cmd -> setParam("SDCreationMessage", errstr );
    edglog(critical) << errstr << std::endl;
  }
  
  return true;
}

static bool evaluateRemoteDirsCreation(Command* cmd) {
  edglog_fn("CFCI::evRemoteDirsCr");
  edglog(info) << "Check Valid Job Size result." << std::endl;

  bool passed = false;

  if ( cmd->getParam("ClientCreateDirsPassed", passed) ) {
    passed ? edglog(severe) << "Remote Dirs Creation Successful." << std::endl :
      edglog(critical) << "Remote Dirs Creation failed: stopping Job." << std::endl;
    return passed;
  }

  edglog(critical) << "ClientRemoteDirsCreation param not found inside the Command." << std::endl;
  return false;

}

#endif

bool doSandboxTransfer( Command* cmd) {

  std::vector<std::string> files;
  std::vector<std::string> untransferredFiles;
  std::string to_host;
  std::string to_path;
  bool result = true;
  std::string jdl;
  edglog_fn("CFCI::doSandboxTransfer");
  edglog(info) << "Approaching Sandbox Transfer." << std::endl;

  if ( ! cmd -> getParam("jdl", jdl) ) {
    return false; 
  } 
  classad::ClassAdParser parser;
  classad::ClassAd* jdlad = parser.ParseClassAd( jdl );
  if (! jdlad ) {
    edglog(critical) << "Error while parsing ClassAd." << std::endl;
    return false; 
  } 
 
  if (!utils::EvaluateAttrListOrSingle(*jdlad,"InputSandbox",files)) {
    cmd -> setParam("TransferDone", result);
    edglog(critical) << "Input Sandbox Transfer done. No files to transfer." << std::endl;
    return true;
  }

  if ( cmd -> getParam("Host", to_host)
       && 
       cmd -> getParam("InputSandboxPath", to_path) ) {

    edglog(medium) << "Preparing for file transfer." << std::endl;

    std::string destinationURL="gsiftp://" + to_host + to_path;
#ifndef WITH_GLOBUS_FTP_CLIENT_API
    std::string command = "globus-url-copy";
#endif    

    std::string sourceURL= "file:";
    
    for(std::vector<std::string>::const_iterator it = files.begin();
	it != files.end(); it++ ) {
      
      std::string filename = (*it).substr((*it).rfind("/")+1); 
      edglog(warning) << "Transferring: " << (*it) << std::endl;

      char buffer[512]; 
      stripProtocol( (*it).c_str(), buffer );
      edglog(debug) << "Transferring file: " << buffer << std::endl;  

      bool transfer_ok = true;
      
#ifndef WITH_GLOBUS_FTP_CLIENT_API
      std::string cmd=command+" "+sourceURL+buffer+" "+destinationURL+"/"+filename; 
      edglog(debug) << "globus-url-copy cmdline: " << cmd << std::endl;
  
      if ( system( cmd.c_str() ) ) transfer_ok = false;
#else 
      if ( !utilities::globus::put(buffer, destinationURL + "/" + filename) ) transfer_ok = false;
#endif      
      if( !transfer_ok ) {
	      result = false;
	      untransferredFiles.push_back(filename);
	      edglog(critical) << filename << " untransferred." << std::endl;
      }       
    }

    cmd -> setParam("TransferDone", result);
    if (!result) {
      cmd->setParam("UntransferredFiles", untransferredFiles);
    }
    
    edglog(critical) << (result ? "Transfer Done." : "Error during File Transfer.") << std::endl;
    return result;
  }
  
  return false;
}

bool evaluateCheckQuotaResult(Command* cmd) {
  edglog_fn("CFCI::evalChkQuota");
  edglog(info) << "Checking User Quota result." << std::endl;
   
  bool quotaok = false;
  if ( cmd->getParam("checkQuotaPassed", quotaok) ) {
    edglog(debug) << "Check Quota Size = " << quotaok << std::endl;
    if ( quotaok ) {
      edglog(warning)  << "Quota Size OK!" << std::endl;
    } else {
      edglog(critical) << "Quota Size not enough." << std::endl;
    }
    return quotaok; 
  }
  
  edglog(critical) << "Check User Quota param not found inside the Command." << std::endl;
  return false;
}

bool evaluateCheckSizeResult(Command* cmd) {
  edglog_fn("CFCI::evCkSize");
  edglog(info) << "Checking valid Job Size result." << std::endl;
    
  bool sizeok = false;
  if ( cmd->getParam("checkSizePassed", sizeok) ) {
    edglog(debug) << "Check Job Size = " << sizeok << std::endl;
    if ( sizeok ) {
      edglog(warning) << "Job size OK!" << std::endl;
    } else {
      edglog(critical) << "Job Size not allowed." << std::endl;
    }
    return sizeok; 
  }
  
  edglog(critical) << "Check Job Size param not found inside the Command." << std::endl;
  return false;
}

bool evaluateCreationResult(Command* cmd){
  edglog_fn("CFCI::evCreation");
  edglog(info) << "Evaluating staging directories creation result." << std::endl;

  int error_code = client::NSE_NO_ERROR;
 
  if ( cmd->getParam("SDCreationError", error_code) ) {
    if ( error_code == (int)client::NSE_NO_ERROR ) {
      edglog(warning) << "Creation OK!" << std::endl;
      return true;
    } else {
      edglog(critical) << "Creation FAILED!" << std::endl;
      return false;
    }

  } 
  
  edglog(critical) << "SDCreation Error param not found inside the Command." << std::endl;
  return false;
}

bool proxyRenewalCheck(Command* cmd){

  bool proxyrenewal;
  std::string proxyhost;
  std::string jdl;
  classad::ClassAd ad;
  classad::ClassAdParser parser;

  cmd -> getParam("jdl", jdl);
  edglog_fn("CFCI:ckProxyRenewal");
  edglog(info) << "Checking ProxyRenewal result." << std::endl;

  if( parser.ParseClassAd(jdl, ad) ) {

    if ( !ad.EvaluateAttrString("MyProxyServer", proxyhost) ) {
      edglog(info) << "No proxy renewal requested." << std::endl;
      return true;
    }

    if ( cmd->getParam("ProxyRenewalDone", proxyrenewal) ) {
      edglog(warning) << "ProxyRenewal result = " << proxyrenewal << std::endl;
      return proxyrenewal;
    } else {  
      edglog(critical) << "ProxyRenewal param not found inside the Command." << std::endl;
    }

  } else {
    edglog(critical) << "Error Parsing ClassAd." << std::endl;
  }
  return false;
}


static bool serializeClientImpl(socket_pp::SocketAgent* sck, Command* cmd)
{
  edglog_fn("CFCI::serializeClient");
  edglog(info) << "Asserting Client Version." << std::endl; 
  std::string cmdstr( cmd -> name() + ":" + cmd -> version() );
  edglog(medium) << "Command String Sent..: [ " << cmdstr << " ]" << std::endl; 

  return
    sck -> Send(cmdstr) &&
    sck -> Send( utils::asString(cmd -> asClassAd()) );
}

bool evaluateMatchMaking(Command* cmd) {
  edglog_fn("CFCI::evMatchMaking");
  edglog(info) << "Asserting Client Version." << std::endl; 
  bool check = false;
  cmd -> getParam("MatchMakingDone", check);
  edglog(medium) << "MatchMaking: " << check << std::endl;
  return check;
}

Command* CommandFactoryClientImpl::create(const std::string& name) 
{
  edglog_fn("CFCI::createFSM");
  edglog(info) << "Creating FSM." << std::endl;   
  Command* cmd = new Command;
  classad::ClassAdParser parser;
  if (cmd->ad != NULL) delete cmd->ad;
  cmd -> ad = parser.ParseClassAd("[Arguments=[];]");
  if (cmd->fsm != NULL) delete cmd->fsm;
  cmd -> fsm = new ns::fsm::state_machine_t;
  cmd -> serializeImpl = serializeClientImpl;
  cmd -> ad -> InsertAttr("Command", name);
  cmd -> ad -> InsertAttr("Version", std::string(CLIENT_VERSION));
  ns::fsm::CommandState::shared_ptr state;

  edglog(medium) << std::endl; // should log NEW_CMD == "\n"
  edglog(medium) << "Command Sent......: [ " << name << " ]" << std::endl;
  edglog(medium) << "Client  Version...: [ " << CLIENT_VERSION << " ]" << std::endl;
 
  if( name == "JobSubmit" ) {
    state.reset(new ns::fsm::ExecuteFunction(computeSandboxSize) );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::SendLong("SandboxSize") );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveBoolean("CheckSizePassed") );
    cmd -> fsm -> push(state);		    
    state.reset(new ns::fsm::ExecuteFunction(evaluateCheckSizeResult) );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveBoolean("CheckQuotaPassed") );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ExecuteFunction(evaluateCheckQuotaResult) );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveBoolean("CheckPassed") );
    cmd -> fsm -> push(state);
#ifdef WITH_GLOBUS_FTP_CLIENT_API
    state.reset(new ns::fsm::ReceiveString("InputSandboxPath") );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveString("OutputSandboxPath") );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ExecuteFunction(createRemoteDirs) );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::SendBoolean("ClientCreateDirsPassed") );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ExecuteFunction(evaluateRemoteDirsCreation) );
    cmd -> fsm -> push(state);
#endif
    state.reset(new ns::fsm::ReceiveInt("SDCreationError") );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveString("SDCreationMessage") );
    cmd -> fsm -> push(state);
    state.reset( new ns::fsm::ExecuteFunction(evaluateCreationResult) );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveBoolean("ProxyRenewalDone"));
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ExecuteFunction(proxyRenewalCheck));
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveString("InputSandboxPath") );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ExecuteFunction(doSandboxTransfer) );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::SendBoolean("TransferDone"));
    cmd -> fsm -> push(state);
  }
  else if (name == "GetMultiAttributeList") {
    state.reset(new ns::fsm::ReceiveVector("MultiAttributeList"));
    cmd -> fsm -> push(state);
  }
  else if ( name == "JobCancel" ) {  
    state.reset(new ns::fsm::SendString("JobId") );
    cmd -> fsm -> push(state);
  }
  else if ( name == "GetSandboxRootPath" ) {  
    state.reset(new ns::fsm::ReceiveString("SandboxRootPath") );
    cmd -> fsm -> push(state);
  }
  else if ( name == "GetQuotaManagementStatus" ) {
    state.reset(new ns::fsm::ReceiveBoolean("QuotaOn"));
    cmd -> fsm -> push(state);
  }
  else if ( name == "GetMaxInputSandboxSize" ) {
    state.reset(new ns::fsm::ReceiveLong("MaxInputSandboxSize"));
    cmd -> fsm -> push(state);
  }
  else if ( name == "GetQuota" ) {
    state.reset(new ns::fsm::ReceiveLong("SoftLimit"));
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveLong("HardLimit"));
    cmd -> fsm -> push(state);    
  }
  else if ( name == "GetFreeQuota" ) {
    state.reset(new ns::fsm::ReceiveLong("SoftLimit"));
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveLong("HardLimit"));
    cmd -> fsm -> push(state);
  }  
  else if ( name == "JobPurge" ) {
    state.reset(new ns::fsm::SendString("JobId") );
    cmd -> fsm -> push(state); 
  }
  else if (name == "ListJobMatch" || name == "ListJobMatchEx" ) {
    state.reset(new ns::fsm::SendString("jdl"));
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveVector("MatchResult"));
    cmd -> fsm -> push(state); 
  }
  else if (name == "GetOutputFilesList") {
    state.reset(new ns::fsm::SendString("JobId"));
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveVector("OutputFilesList"));
    cmd -> fsm -> push(state);
  }
  else if (name == "GetOutputFilesListSize") {
    state.reset(new ns::fsm::SendString("JobId"));
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveInt("OutputFilesListSize"));
    cmd -> fsm -> push(state);
  }
  else if ( name == "DagSubmit" ) {
    state.reset(new ns::fsm::ExecuteFunction(computeDagSandboxSize) );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::SendLong("SandboxSize") );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveBoolean("CheckSizePassed") );
    cmd -> fsm -> push(state);		    
    state.reset(new ns::fsm::ExecuteFunction(evaluateCheckSizeResult) );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveBoolean("CheckQuotaPassed") );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ExecuteFunction(evaluateCheckQuotaResult) );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveBoolean("CheckPassed") );
    cmd -> fsm -> push(state);
#ifdef WITH_GLOBUS_FTP_CLIENT_API
    state.reset(new ns::fsm::ReceiveString("SandboxRootPath") );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ExecuteFunction(dag::createRemoteDirs) );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::SendBoolean("ClientCreateDirsPassed") );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ExecuteFunction(evaluateRemoteDirsCreation) );
    cmd -> fsm -> push(state);
#endif
    state.reset(new ns::fsm::ReceiveInt("SDCreationError") );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveString("SDCreationMessage") );
    cmd -> fsm -> push(state);
    state.reset( new ns::fsm::ExecuteFunction(evaluateCreationResult) );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveBoolean("ProxyRenewalDone"));
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ExecuteFunction(proxyRenewalCheck));
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ReceiveString("InputSandboxPath") );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::ExecuteFunction(dag::doSandboxTransfer) );
    cmd -> fsm -> push(state);
    state.reset(new ns::fsm::SendBoolean("TransferDone"));
    cmd -> fsm -> push(state);
  }
  else {
    delete cmd;
    cmd = 0;
    edglog(critical) << "Unknown Command Request received." << std::endl;
  }
  return cmd;
}

} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite
