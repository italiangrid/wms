/*
 * DagCommandFactoryClientImpl.cpp
 * 
 * Copyright (c) 2001, 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/jdl/DAGAd.h"

#include "Command.h"
#include "common.h"
#include "common_dag.h"
#include "DagCommandFactoryClientImpl.h"
#include "logging.h"
#include "exception_codes.h"
#include "JobId.h"

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"


#ifdef WITH_GLOBUS_FTP_CLIENT_API
#include "glite/wms/common/utilities/globus_ftp_utils.h"
#include "gsimkdirex.h"
#endif
#include "glite/wmsutils/classads/classad_utils.h"

#include <vector>
#include <string>
#include <classad_distribution.h>
#include <iostream>
#include <boost/tuple/tuple.hpp>

namespace utils = glite::wmsutils::classads;
namespace utilities = glite::wms::common::utilities; 
namespace logger    = glite::wms::common::logger;
namespace requestad = glite::jdl;
namespace commands  = glite::wms::manager::ns::commands;
namespace nsjobid   = glite::wms::ns::jobid;

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

  /**
   * Compute the sandboxsize for the given job.
   * WARNING: the size value is not inizialized so it can be used for sums into
   * cicles.
   * @param ad a classad containing the job whose sandbox size must be calculated.
   * @param size the off_t var to fill with the sandbox size.
   * return true on success, false otherwise.
   */

  namespace 
  {
    struct computeNodeSandboxSize 
    {
      computeNodeSandboxSize(off_t& offt, bool& r) : size(offt), result(r)
      {
	size = 0;
	result = true;
      }

      void operator()(const requestad::DAGAd::node_value_type& nvt) 
      {
	std::string jobId;
	std::vector<std::string> files;
	
	const requestad::DAGNodeInfo& node_info = nvt.second;
	const classad::ClassAd* ad = node_info.description_ad();
	// const classad::ClassAd* ad = nvt.second.description_ad();
	
#if DEBUG
	off_t oldsize=size;
#endif

	if (!ad->EvaluateAttrString("edg_jobid", jobId) ) {
	  edglog(fatal) << "Error while retrieving jobId from classad." << std::endl;
	  result = false;
	}

	if (!utils::EvaluateAttrListOrSingle(*ad, "InputSandbox", files)) {
#if DEBUG
	  edglog(debug) << "Job: " << jobId << ". Rilevati: " << files.size() << " files" << std::endl;
#endif
	}
	
	if (!files.size()) {
#if DEBUG
	  edglog(debug) << "Job: " << jobId << ". Empty Input Sandbox. " << std::endl;
#endif     
	}
	
	for_each( files.begin(), files.end(), computeFileSize(size, result) );
	
#if DEBUG
	edglog(debug) << "Job: " << JobId << ". Sandbox Size: " << (size - oldsize) << std::endl;
#endif
      }
    
      off_t& size;
      bool& result;

    };
      
  }
  
  bool computeDagSandboxSize (Command* cmd) { 
 
    std::string jdl;
    if ( ! cmd -> getParam("jdl", jdl) ) {
      return false; 
    } 
    
    classad::ClassAdParser parser;
    classad::ClassAd* jdlad = parser.ParseClassAd( jdl );
    if (! jdlad ) { 
      return false;
    } 
    
    requestad::DAGAd dagad(*jdlad);
    requestad::DAGAd::node_iterator node_b;
    requestad::DAGAd::node_iterator node_e;
    boost::tie(node_b, node_e) = dagad.nodes();
   
    off_t total_size = 0;
    bool result = true;

    std::for_each(node_b, node_e, computeNodeSandboxSize(total_size, result));
    
    cmd -> setParam("SandboxSize", (double)total_size );
    return result; 
  } 

  namespace dag {

#ifdef WITH_GLOBUS_FTP_CLIENT_API

    bool createJobDirs(commands::Command* cmd, 
		       const std::string host, 
		       const std::string root) {
      
      std::string isb_path( root + "/input" );
      std::string osb_path( root + "/output" );
      cmd -> setParam("OutputSandboxPath", osb_path);
      cmd -> setParam("InputSandboxPath",  isb_path);
      edglog_fn("DagCFCI:crJobDirs");
      edglog(medium) << "OSB: " << osb_path << std::endl;
      edglog(medium) << "ISB: " << isb_path << std::endl;

      std::string dst( host + isb_path );
      size_t job_dir_pos  = dst.rfind('/');
      //Check for trailing slash.
      if (job_dir_pos==dst.length()-1) job_dir_pos = dst.rfind('/',job_dir_pos-1);
      std::string job_dir(dst.substr(0, job_dir_pos));
	
      if (!(utilities::globus::mkdir(std::string("gsiftp://") + job_dir))) {
	edglog(critical) << "Cannot create directory on NS: "<< job_dir << std::endl;
	cmd -> setParam("SDCreationError", client::NSE_MKDIR);
	cmd -> setParam("SDCreationMessage", "Cannot create directory on NS: " + job_dir);
	return false;
      } else {
        edglog(info) << "Attempting to create directory"<< job_dir << std::endl; 
        if (!gsimkdirex(job_dir)) {
          edglog(critical) << "Cannot create directory on NS: "<< job_dir << std::endl;
          cmd -> setParam("SDCreationError", client::NSE_MKDIR);
          cmd -> setParam("SDCreationMessage", "Cannot create directory on NS: " + job_dir);
          return false; 
        } 
      }
  
      if (!gsimkdirex(host + isb_path, job_dir)) {
	edglog(critical) << "Cannot create directory on NS: "<< host << isb_path << std::endl;
	cmd -> setParam("SDCreationError", client::NSE_MKDIR);
	cmd -> setParam("SDCreationMessage", "Cannot create directory on NS: " + host + isb_path);
	return false;
      }
      
      if (!gsimkdirex(host + osb_path, job_dir)) {
	edglog(critical) << "Cannot create directory on NS: "<< host << osb_path << std::endl;
	cmd -> setParam("SDCreationError", client::NSE_MKDIR);
	cmd -> setParam("SDCreationMessage", "Cannot create directory on NS: " + host + osb_path);
	return false;
      }
      
      return true;
    }


    bool createRemoteDirs(commands::Command* cmd) {
      edglog_fn("DagFCI:crRemoteDirs");
      edglog(info) << "Creating... " << std::endl;

      std::string to_host, root, path, errstr;
      cmd -> setParam("ClientCreateDirsPassed", false);

      edglog(debug) << utils::asString(cmd->asClassAd())  << std::endl;
       
      if ( cmd -> getParam("Host", to_host)
	   &&
	   cmd -> getParam("SandboxRootPath", root) ) {

	std::string jdl;
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
	requestad::DAGAd dagad(*jdlad);
	std::string id( utils::unparse_expression(*dagad.get_generic("edg_jobid")) );

	path.assign(root + "/" + nsjobid::to_filename( wmsutils::jobid::JobId(id) ) );

	if ( createJobDirs( cmd, to_host, path) ) {
	  requestad::DAGAd::node_iterator node_b;
	  requestad::DAGAd::node_iterator node_e;
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
            edglog(debug) << "Attemping to handle paths for: " << id << std::endl;	    
	    path.assign( root + "/" + nsjobid::to_filename( wmsutils::jobid::JobId(id) ) );
            edglog(debug) << path << std::endl;	    

	    if ( !createJobDirs( cmd, to_host, path) ) {
	      errstr.assign("Error creating Node directories at\n\t Host: " + to_host + "\n\t JobId:" + id);
	      edglog(fatal) << errstr << std::endl;	  	      
	      break;
	    }
	    
	    node_b++;
	  }

	  cmd -> setParam("ClientCreateDirsPassed", true);
	  edglog(info) << "Remote Dirs Creation Successful" << std::endl;
	} else {
	  errstr.assign("Error creating DAG directories at\n\t Host: " + to_host + "\n\t DAGId:" + id);
	  edglog(fatal) << errstr << std::endl;	  
	}
	
      } else {
	errstr.assign("Cannot perform globus::mkdir with\n\t Host: " + to_host + ". Missing Host or Jdl.");
	cmd -> setParam("SDCreationError", client::NSE_MKDIR);
	cmd -> setParam("SDCreationMessage", errstr );
	edglog(fatal) << errstr << std::endl;
      }
  
      return true;
    }
#endif


    /*
     * Takes care of sandbox transfer for a given job.
     * If a file is not transferred it inserts into untransferred
     * vector a string given by node_name::filename, if node_name is not
     * empty; otherwise it puts the sole filename.
     * @param node_name the name of the job or dag node.
     * @param to_host the host to send the files to.
     * @param to_path the path to store the files in the host.
     * @param files the vector containing filenames.
     * @param untransferred the vector to fill with untransferred filenames.
     */
    bool transferJobSandbox(std::string node_name,
			    std::string to_host,
			    std::string to_path,
			    std::vector<std::string> files, 
			    std::vector<std::string>& untransferred) {

      bool result = true;
      std::string destinationURL="gsiftp://" + to_host + to_path;
#ifndef WITH_GLOBUS_FTP_CLIENT_API
      std::string command = "globus-url-copy";
#endif    

      std::string sourceURL= "file:";
    
      for(std::vector<std::string>::const_iterator it = files.begin();
	  it != files.end(); it++ ) {
	    
	std::string filename = (*it).substr((*it).rfind("/")+1); 
	edglog(info) << "Transferring: " << (*it) << std::endl;
	    
	char buffer[512]; 
	stripProtocol( (*it).c_str(), buffer );
	edglog(debug) << "Transferring file: " << buffer << std::endl;

	bool transfer_ok = true;
      
#ifndef WITH_GLOBUS_FTP_CLIENT_API
	std::string cmd=command+" "+sourceURL+buffer+" "+destinationURL+"/"+filename; 
	edglog(debug) << "globus-url-copy cmdline: " << cmd << std::endl;
  
	if ( system( cmd.c_str() ) ) transfer_ok = false;
#else 
	edglog(debug) << "Globus Ftp put to: " <<  std::string(destinationURL + "/" + filename) << std::endl;
	if ( !utilities::globus::put(buffer, destinationURL + "/" + filename) ) transfer_ok = false;
#endif      
	if( !transfer_ok ) {
	  result = false;
	  untransferred.push_back((node_name.empty() ? "" : node_name + "::") + filename);
	  edglog(fatal) << filename << " untransferred." << std::endl;
	}       
      }
      
      return result;
    }


    bool doSandboxTransfer( commands::Command* cmd ) {

      edglog_fn("DAG:dST");
      std::vector<std::string> files;
      std::vector<std::string> untransferredFiles;
      std::string to_host, to_root_path, to_path, errstr, jdl;
      bool result = true;
      
      if ( ! cmd -> getParam("jdl", jdl) ) {
	return false; 
      } 

      classad::ClassAdParser parser;
      classad::ClassAd* jdlad = parser.ParseClassAd( jdl );
      if (! jdlad ) {
	edglog(fatal) << "Error while parsing ClassAd." << std::endl;
	return false; 
      } 
      
      requestad::DAGAd dagad(*jdlad);
      std::string id( utils::unparse_expression(*dagad.get_generic("edg_jobid")) );
      
      if ( !( cmd -> getParam("Host", to_host) )
	   ||  
	   !( cmd -> getParam("SandboxRootPath", to_root_path) ) ) {
	edglog(fatal) << "**** ERROR ****" << std::endl;
        edglog(fatal) << "* Unset values found retrieving info for ISBs transfer:" << std::endl;
	edglog(fatal) << "* Host: " << to_host << "\n* RootPath: " << to_path << "\n***************" << std::endl;
      }


      if (!utils::EvaluateAttrListOrSingle(*jdlad,"InputSandbox",files)) {
	cmd -> setParam("TransferDone", result);
	edglog(info) << "Input Sandbox Transfer done. No files." << std::endl;
      }
      else {
	
	to_path.assign( std::string(to_root_path) + "/" + nsjobid::to_filename( wmsutils::jobid::JobId(id) ) +
			"/input" );
	edglog(fatal) << "Preparing for file transfer." << std::endl;

	edglog(debug) << utils::asString(cmd->asClassAd()) << std::endl;
	edglog(fatal) << "Host: " << to_host << std::endl;
	edglog(fatal) << "Path: " << to_path << std::endl;
	
	result = transferJobSandbox("DAG::" + id, to_host, to_path, files, untransferredFiles);
      }

      // Let's Transfer nodes Sandboxes
      requestad::DAGAd::node_iterator node_b;
      requestad::DAGAd::node_iterator node_e;
      boost::tie(node_b, node_e) = dagad.nodes();

      while ( node_b != node_e) {
	const requestad::DAGNodeInfo& node_info = (*node_b).second;
	const classad::ClassAd* ad = node_info.description_ad();

	if (!ad->EvaluateAttrString("edg_jobid", id) ) {
	  edglog(fatal) << "Error while retrieving jobId from classad." << std::endl;
	}    	    

	std::vector<std::string> nodefiles;

	if (!utils::EvaluateAttrListOrSingle(*ad, "InputSandbox", nodefiles)) {
	  cmd -> setParam("TransferDone", result);
	  edglog(medium) << "Input Sandbox Transfer done. No files." << std::endl;
	} else {
	  to_path.assign( std::string(to_root_path) + "/" + nsjobid::to_filename( wmsutils::jobid::JobId(id) ) +
			  "/input" );
	  if ( !transferJobSandbox( (*node_b).first, to_host, to_path, nodefiles, untransferredFiles ) ) {
	    errstr.assign("Error transferring Node sandbox at\n\t Host: " + to_host + "\n\t JobId:" + id);
	    edglog(fatal) << errstr << std::endl;
	    result = false;
	  }
	}
	    
	node_b++;
      }

      cmd -> setParam("TransferDone", result);
      if (!result) {
	cmd->setParam("UntransferredFiles", untransferredFiles);
	edglog(fatal) << "Error during File Transfer." << std::endl;
      } else {
	edglog(info) << "Transfer Done." << std::endl;
      }
      return result;
    }
      

} // namespace dag
} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite 
