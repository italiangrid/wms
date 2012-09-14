/*
 * NSClient.cpp
 * 
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/regex.hpp>
#include <classad_distribution.h>

extern "C" {
#include "globus_ftp_client.h"
}

#include <unistd.h>
#include <algorithm>
#include <map>
#include <queue>
#include <iterator>

#include "common.h"
#include "exceptions.h"
#include "NSClient.h"

#include "glite/wmsutils/tls/socket++/exceptions.h"
#include "glite/wmsutils/tls/socket++/GSISocketClient.h"
#include "glite/wmsutils/tls/socket++/GSISocketAgent.h"

#include "glite/wms/ns-common/CommandFactory.h"
#include "glite/wms/ns-common/CommandFactoryClientImpl.h"

#include "glite/wms/common/logger/edglog.h" 
#include "glite/wms/common/logger/manipulators.h" 
#include "glite/wms/common/utilities/edgstrstream.h"

#include "glite/jdl/ExpDagAd.h"
#include "glite/jdl/JDLAttributes.h"

#include "glite/wms/ns-common/const.h"
#include "logging.h"

namespace logger = glite::wms::common::logger; 
namespace excep = glite::wmsutils::exception;

namespace glite {
namespace socket_pp = wmsutils::tls::socket_pp;

namespace wms {

namespace utilities = common::utilities;

namespace manager {
namespace ns {
namespace client {


/**
 * Constructor.
 * @param h the host address.
 * @param p the port.
 * @param lvl the logging level.
 * @param auth_to authentication timeout for GSI connection.
 * @throws ConnectionException if connection error happens.
 */
NSClient::NSClient(const std::string& h, int p, logger::level_t lvl, int auth_to)
{
  // Open a logger on client side. Pay Attention:
  // This logger is not the same object that logs on the server side
  // If Java Client is used only on-screen log is used.
#ifndef WITH_UI_JCLIENT
  logger::threadsafe::edglog.open( "edglog.log", lvl );
#endif
  edglog_fn("NSC::NSClient");
  edglog(medium) << "Starting NS Client..." << std::endl;

  _initerror = false;
  auth_timeout = auth_to;
  connection = NULL; 

  char server_name[64];
  strcpy(server_name, h.c_str());
  std::string resolved_name;

  if ( !resolve_host(server_name, resolved_name) || resolved_name.empty() ) {
      edglog(critical) << "Error while creating NS Client, host: " << h << " port: " << p << "." << std::endl;
      edglog(critical) << "Failure while Resolving Hostname." << std::endl;
      throw ConnectionException(h);    
  }

  edglog(critical) << "Resolved Hostname: " << resolved_name << std::endl;
  connection = new socket_pp::GSISocketClient(resolved_name, p);

  // if connect does not work
  if (!connection) {
      edglog(critical) << "Error while creating NS Client, host: " << h << " port: " << p << "." << std::endl;
      edglog(critical) << "Resolved Hostname: " << resolved_name << std::endl;
      throw ConnectionException(h);
  } 
  
  char hostname[64];
  gethostname( hostname, 64);   

  if( !hostname_to_ip(hostname, this_ip) ) {
    this_ip = std::string(hostname);
  }

  globus_module_activate(GLOBUS_FTP_CLIENT_MODULE);
}

NSClient::~NSClient()
{
  if(connection) delete connection;
  globus_module_deactivate(GLOBUS_FTP_CLIENT_MODULE);
}

bool NSClient::connect() 
{
  edglog_fn("NSC::connect");
  bool result = false;	
  try {
    edglog(medium) << "Connecting to Server..." << std::endl;
    if( connection == NULL ) return false;
    connection -> set_auth_timeout(auth_timeout);
    result = connection -> Open();
  }
  catch(excep::Exception& e) {
    edglog(critical) << e.what() << std::endl;
    throw e;
  }
  return result;
}

bool NSClient::disconnect()
{
  edglog_fn("NSC::disconnect");
  edglog(medium) << logger::setfunction( "NSClient::disconnect()" ) << "Disconnecting from Server..." << std::endl;
  if( connection == NULL ) return false;
  return connection -> Close();
}

bool NSClient::IsInitDone() const { 
  return !_initerror; 
}

/**
 * Submit a job to broker.
 * @param s a jdl string representing a job.
 * @return true on successful submission, false otherwise.
 * @throws TimeoutException if timeouts expire.
 * @throws ConnectionException if error occurs while connecting.
 * @throws JDLParsingException if error occurs while parsing jdl string.
 * @throws SandboxIOException when error occurs while transferring sandbox.
 * @throws AuthenticationException for authentication failure.
 * @throws NotEnoughQuotaException for leaks of quota space.
 * @throws JobSizeException for job exceeding allowed job size. 
 */ 
bool NSClient::jobSubmit(const std::string& s)
{  
  edglog_fn("NSC::submit");
  edglog(info) << "Client Submit: ";

  connection -> DelegateCredentials(true);
  
  classad::ClassAdParser parser;
  boost::scoped_ptr<classad::ClassAd> ad( parser.ParseClassAd(s) );

  if( ad.get() == NULL ) {
    edglog(critical) << "Error while parsing Jdl string." << std::endl;
    throw JDLParsingException( s, 
			       std::string("NSClient::jobSubmit()"), 
			       std::string("Error while parsing Jdl string."));
  }
  
  std::string jdl(s);
  std::string type;
  ad.get()->EvaluateAttrString(_JDL_TYPE_ATTR_STR_, type);
  transform(type.begin(), type.end(), type.begin(), tolower);

  edglog(info) << "Job type: ";

  if ( type == _JOB_) {
    type.assign( _JOB_SUBMIT_ );
    edglog(info) << type << std::endl;

  } else if ( type == _DAG_) {
    type.assign( _DAG_SUBMIT_ );
    edglog(info) << type << std::endl; 
 
    jdl::ExpDagAd expdag(jdl); 
    expdag.inherit(jdl::JDL::INPUTSB); 
    jdl = expdag.toString();
    edglog(debug) << "DAG jdl after inheriting InputSandbox: " << jdl << std::endl;
  } else {
    edglog(critical) << "Error: Unrecognized JDL Type." << std::endl;
    throw JDLParsingException( s,
                               std::string("NSClient::jobSubmit()"),
                               std::string("Error: Unrecognized JDL Type."));
  }

  bool result = false;
  if ( connect() ) {
    edglog(medium) << "Connected." << std::endl;

    commands::CommandFactory<commands::CommandFactoryClientImpl> factory;
    boost::scoped_ptr<commands::Command> cmd( factory.create(type) ); 
    cmd -> setParam("jdl", jdl);
    cmd -> setParam("Host", connection->Host());
    result = runCommand(cmd.get());

    bool check;
    std::vector<std::string> untrasferred;
    if ( cmd -> getParam("UntransferredFiles", untrasferred) ) {

      utilities::edgstrstream ss;
      copy(untrasferred.begin(), 
	   untrasferred.end() , 
	   std::ostream_iterator<std::string>(ss, "\n\t"));
      std::string msg("One or more Input Sandbox files are missing: \n\t");
      SandboxIOException ex("NSClient::jobSubmit", msg + std::string(ss.str()));

      edglog(critical) << (msg + std::string(ss.str())) << std::endl;
      throw ex;
    } else {

      if (cmd -> getParam("ClientCreateDirsPassed", check) ) {
	
	if (!check) {
	  // The Globus_Ftp_Api hs not been able to create remote dirs
	  std::string msg("Server returned nonzero error code when attempting to create remote Sandbox Directories.");
	  SandboxIOException ex("NSClient::jobSubmit", msg);
	  edglog(critical) << msg << std::endl;
	  throw ex;
	}

	if ( cmd -> getParam("ProxyRenewalDone", check) ) {
	  if (!check) {
	    // The Globus_Ftp_Api hs not been able to create remote dirs
	    std::string msg("Error during Proxy Renewal registration.");
	    ProxyRenewalException ex("NSClient::jobSubmit", msg);
	    edglog(critical) << msg << std::endl;
	    throw ex;
	  }
	} 

      } else {

	if (cmd -> getParam("SDCreationError", check) ) {

	  std::cout << "Parameter SDCreationError found. Value: " << (check ? "TRUE" : "FALSE") << std::endl;
	  if (!check) {
	    std::string errmsg;
	    // The Globus_Ftp_Api hs not been able to create remote dirs
	    cmd -> getParam("SDCreationMessage", errmsg);
	    SandboxIOException ex("NSClient::jobSubmit", errmsg);
	    edglog(critical) << errmsg << std::endl;
	    throw ex;
	  }

	} else {

	  if ( cmd -> getParam("CheckQuotaPassed", check) ) {
	    if (!check) {
	      // Job has forbidden size: throw an exception
	      std::string msg("User Quota fully used.");
	      NotEnoughQuotaException ex( msg );
	      edglog(critical) << msg << std::endl;
	      throw ex;	
	    }
	  } else {
	    if ( cmd -> getParam("CheckSizePassed", check) ) {
	      if (!check) {
		// Job has forbidden size: throw an exception
		std::string msg("Job InputSandbox Size exceeds limits.");
		JobSizeException ex( msg );
		edglog(critical) << msg << std::endl;
		throw ex;	
	      }
	    }
	  }
	}
      }
    }
  }
  disconnect();
  return result;
}


/**
 * Submit a dag to broker.
 * @param s a jdl string representing a dag.
 * @return true on successful submission, false otherwise.
 * @throws TimeoutException if timeouts expire.
 * @throws ConnectionException if error occurs while connecting.
 * @throws JDLParsingException if error occurs while parsing jdl string.
 * @throws SandboxIOException when error occurs while transferring sandbox.
 * @throws AuthenticationException for authentication failure.
 * @throws NotEnoughQuotaException for leaks of quota space.
 * @throws JobSizeException for job exceeding allowed job size. 
 */ 
bool NSClient::dagSubmit(const std::string& s)
{  
  logger::threadsafe::edglog << logger::setfunction( "NSClient::dagSubmit()" ) << "Client DAG Submit. " << std::endl;

  connection -> DelegateCredentials(true);
  
  classad::ClassAdParser parser;
  boost::scoped_ptr<classad::ClassAd> ad( parser.ParseClassAd(s) );

  if( ad.get() == NULL ) {
    logger::threadsafe::edglog << "Error while parsing Jdl string." 
			       << std::endl;
    throw JDLParsingException( s, 
			       std::string("NSClient::dagSubmit()"), 
			       std::string("Error while parsing Jdl string."));
  }
  
  std::string jdl(s);   
    
  //Patch for ISB Inheritance   
  jdl::ExpDagAd expdag(jdl);   
  expdag.inherit(jdl::JDL::INPUTSB);   
  jdl = expdag.toString();   
  edglog(debug) << "DAG jdl after inheriting InputSandbox: " << jdl << std::endl; 

  bool result = false;
  if ( connect() ) {

    logger::threadsafe::edglog << logger::setfunction( "NSClient::dagSubmit()" ) << "Connected." << std::endl;

    commands::CommandFactory<commands::CommandFactoryClientImpl> factory;
    boost::scoped_ptr<commands::Command> cmd( factory.create("DagSubmit") ); 
    cmd -> setParam("jdl", s);
    cmd -> setParam("Host", connection->Host());
    result = runCommand(cmd.get());

    bool check;

    std::vector<std::string> untrasferred;
    if ( cmd -> getParam("UntransferredFiles", untrasferred) ) {

      utilities::edgstrstream ss;
      copy(untrasferred.begin(), 
	   untrasferred.end() , 
	   std::ostream_iterator<std::string>(ss, "\n\t"));
      std::string msg("One or more Input Sandbox files are missing: \n\t");
      SandboxIOException ex("NSClient::jobSubmit", msg + std::string(ss.str()));

      logger::threadsafe::edglog << logger::setfunction( "NSClient::jobSubmit()" ) << (msg + std::string(ss.str())) << std::endl;
      throw ex;
    } else {

      if (cmd -> getParam("ClientCreateDirsPassed", check) ) {
	if (!check) {
	  // The Globus_Ftp_Api hs not been able to create remote dirs
	  std::string msg("Server returned nonzero error code when attempting to create remote Sandbox Directories.");
	  SandboxIOException ex("NSClient::jobSubmit", msg);
	  logger::threadsafe::edglog << logger::setfunction( "NSClient::jobSubmit()" ) << msg << std::endl;
	  throw ex;
	}

	if ( cmd -> getParam("ProxyRenewalDone", check) ) {
	  if (!check) {
	    // The Globus_Ftp_Api hs not been able to create remote dirs
	    std::string msg("Error during Proxy Renewal registration. Job Aborted.");
	    ProxyRenewalException ex("NSClient::jobSubmit", msg);
	    logger::threadsafe::edglog << logger::setfunction( "NSClient::jobSubmit()" ) << msg << std::endl;
	    throw ex;
	  }
	} 
      } else {

	if ( cmd -> getParam("CheckQuotaPassed", check) ) {
	  if (!check) {
	    // Job has forbidden size: throw an exception
	    std::string msg("User Quota fully used.");
	    NotEnoughQuotaException ex( msg );
	    logger::threadsafe::edglog << logger::setfunction( "NSClient::jobSubmit()" ) << msg << std::endl;
	    throw ex;	
	  }
	} else {
	  if ( cmd -> getParam("CheckSizePassed", check) ) {
	    if (!check) {
	      // Job has forbidden size: throw an exception
	      std::string msg("Job InputSandbox Size exceeds limits.");
	      JobSizeException ex( msg );
	      logger::threadsafe::edglog << logger::setfunction( "NSClient::jobSubmit()" ) << msg << std::endl;
	      throw ex;	
	    }
	  }
	}
      }
    }
  }
  disconnect();
  return result;
}


/**
 * Lists all jobs matching a jdl string criteria.
 * @param jdl the jdl string.
 * @param l a pointer to the list which will receive all matching CEIds.
 * @return true on success, false otherwise.
 * @throws ConnectionException if connection error occurs.
 * @throws JDLParsingException if a parsing error occurs.
 * @throws AuthenticationException for authentication failure.
 * @throws NoSuitableResourceException if no suitable resource is found.
 * @throws MatchMakingException if mathcmaking reports errors.
 * @throws ListMatchException if II communication error occurs. 
 */
bool NSClient::listJobMatch(const std::string& jdl, std::vector<std::string>& l)
{
  edglog_fn("NSC::listJobMatch");
  edglog(info) << "Client listJobMatch. " << std::endl; 
  edglog(debug) << "Jdl: " << jdl << std::endl;

  connection -> DelegateCredentials(true);  
  classad::ClassAdParser parser;
  boost::scoped_ptr<classad::ClassAd> ad( parser.ParseClassAd(jdl) );
 
  if( ad.get() == NULL ) {
    edglog(fatal) << "Error while parsing Jdl string." << std::endl;
    throw JDLParsingException(jdl, 
			      std::string("NSClient::jobListMatch()"), 
			      std::string("Error while parsing Jdl string."));
  }

  if ( connect() ) {
    edglog(medium) << "Connected." << std::endl;
    commands::CommandFactory<commands::CommandFactoryClientImpl> factory;
    boost::scoped_ptr<commands::Command> cmd(factory.create("ListJobMatch"));
    cmd -> setParam("jdl", jdl);
    runCommand(cmd.get());
    disconnect();       

    if ( cmd -> getParam("MatchResult", l) ) {
      if (l.size() > 1 && l.front() == std::string(EDG_WL_NSMATCHMAKINGERROR)) {
	  edglog(critical) << "Error during MatchMaking:\n\t" << l[1] << std::endl;
	  throw ListMatchException(jdl, std::string("NSClient::jobListMatch()"), std::string(l[1]));	  
      }
      return true;
    } else {
      l.push_back(std::string(EDG_WL_NSMATCHMAKINGERROR));
      l.push_back(std::string("Unknown Error. No MatchResult: please check"));
      edglog(critical) << "Error during MatchMaking:\n\t" << l[1] << std::endl;
      throw ListMatchException(jdl, std::string("NSClient::jobListMatch()"), std::string(l[1]));	  
    }
  }

  return true; 
}

/**
 * Lists all jobs matching a jdl string criteria invoking WM based listmatch process.
 * @param jdl the jdl string.
 * @param l a pointer to the list which will receive all matching CEIds.
 * @return true on success, false otherwise.
 */
bool NSClient::listJobMatchEx(const std::string& jdl, std::vector<std::string>& l)
{
  edglog_fn("NSC::listJobMatch");
  edglog(info) << "Client listJobMatchEx. " << std::endl; 
  edglog(debug) << "Jdl: " << jdl << std::endl;

  connection -> DelegateCredentials(false);  
  classad::ClassAdParser parser;
  boost::scoped_ptr<classad::ClassAd> ad( parser.ParseClassAd(jdl) );
 
  if( ad.get() == NULL ) {
    edglog(fatal) << "Error while parsing Jdl string." << std::endl;
    throw JDLParsingException(jdl, 
			      std::string("NSClient::jobListMatch()"), 
			      std::string("Error while parsing Jdl string."));
  }

  if ( connect() ) {
    edglog(medium) << "Connected." << std::endl;
    commands::CommandFactory<commands::CommandFactoryClientImpl> factory;
    boost::scoped_ptr<commands::Command> cmd(factory.create("ListJobMatchEx"));
    cmd -> setParam("jdl", jdl);
    runCommand(cmd.get());
    disconnect();       

    if ( cmd -> getParam("MatchResult", l) ) {
      if (l.size() > 1 && l.front() == std::string(EDG_WL_NSMATCHMAKINGERROR)) {
	  edglog(critical) << "Error during MatchMaking:\n\t" << l[1] << std::endl;
	  throw ListMatchException(jdl, std::string("NSClient::jobListMatch()"), std::string(l[1]));	  
      }
      return true;
    } else {
      l.push_back(std::string(EDG_WL_NSMATCHMAKINGERROR));
      l.push_back(std::string("Unknown Error. No MatchResult: please check"));
      edglog(critical) << "Error during MatchMaking:\n\t" << l[1] << std::endl;
      throw ListMatchException(jdl, std::string("NSClient::jobListMatch()"), std::string(l[1]));	  
    }
  }

  return true; 
}

bool NSClient::listJobMatch(const std::string& jdl, std::vector< std::pair<std::string, double> >& l)
{
  edglog_fn("NSC::listJobMatchInt");
  std::vector<std::string> temp_list;
  if( !listJobMatch(jdl, temp_list) ) return false;

  edglog(debug) << "ListJobMatch: " << temp_list.size() << " record(s) found." << std::endl;

  for(std::vector<std::string>::const_iterator it = temp_list.begin();
		  it != temp_list.end(); it++) {
    try {  
      
      static boost::regex  expression( "(\\S+)\\s*=\\s*(\\S+)" );
      boost::smatch        pieces;
      std::string          ceid, rank;
      if( boost::regex_match( *it, pieces, expression) ) {
	
	ceid.assign  (pieces[1].first, pieces[1].second);
	rank.assign  (pieces[2].first, pieces[2].second);
	
	l.push_back( std::make_pair(ceid, std::atoi(rank.c_str())) );
	edglog(debug) << "/t Match: " << ceid << " Rank: " << rank << std::endl;   
      } else {   
	edglog(debug) << "/t!Match: " << ceid << " Rank: " << rank << std::endl; 
      }
    }
    catch( boost::bad_expression& e ) {
      edglog(critical) << e.what() << std::endl;
    }
  }
  return true;							
}

bool NSClient::listJobMatchEx(const std::string& jdl, std::vector< std::pair<std::string, double> >& l)
{
  edglog_fn("NSC::listJobMatchInt");
  std::vector<std::string> temp_list;
  if( !listJobMatchEx(jdl, temp_list) ) return false;

  edglog(debug) << "ListJobMatch: " << temp_list.size() << " record(s) found." << std::endl;

  for(std::vector<std::string>::const_iterator it = temp_list.begin();
		  it != temp_list.end(); it++) {
    try {  
      
      static boost::regex  expression( "(\\S+)\\s*=\\s*(\\S+)" );
      boost::smatch        pieces;
      std::string          ceid, rank;
      if( boost::regex_match( *it, pieces, expression) ) {
	
	ceid.assign  (pieces[1].first, pieces[1].second);
	rank.assign  (pieces[2].first, pieces[2].second);
	
	l.push_back( std::make_pair(ceid, std::atoi(rank.c_str())) );
	edglog(debug) << "/t Match: " << ceid << " Rank: " << rank << std::endl;   
      } else {   
	edglog(debug) << "/t!Match: " << ceid << " Rank: " << rank << std::endl; 
      }
    }
    catch( boost::bad_expression& e ) {
      edglog(critical) << e.what() << std::endl;
    }
  }
  return true;							
}

/**
 * Purges repository related to given job. All repository files and directories
 * related to jobid will be removed. Returns a boolean value explaining about
 * success or failure in execution.
 * @param dg_jobid the job identifier whose space has to be purged.
 * @return true
 */
bool NSClient::jobPurge(const std::string& dg_jobid)
{
  edglog_fn("NSC::jobPurge");
  edglog(info) << "Client jobPurge." << std::endl;

  connection -> DelegateCredentials(false);
  bool result = false;

  if ( connect() ) {
    edglog(info) << "Connected." <<std::endl;
    commands::CommandFactory<commands::CommandFactoryClientImpl> factory;
    boost::scoped_ptr<commands::Command> cmd( factory.create("JobPurge") );
    cmd -> setParam("JobId", dg_jobid);
    cmd -> setParam("Host", connection->Host());
    result &= runCommand(cmd.get());
  }
  disconnect();
  return result;
}

/**
 * Cancels all jobs in the given list. Deprecated. This method
 * will be soon removed from this object due to architecture change.
 * @param dg_job_list the list of job identifiers to cancel.
 * @return the number of expected notifications.
 * @throws TimeoutException if error occured (timeout expires).
 * @throws ConnectionException if connection error occurs.
 * @throws AuthenticationException for authentication failure.
 */
bool NSClient::jobCancel(const std::list<std::string>& dg_job_list)
{
  edglog_fn("NSC::jobCancel");
  edglog(info) << "Client jobCancel." << std::endl;

  connection -> DelegateCredentials(false);
  bool result = false;

  if ( connect() ) {
    edglog(info) << "Connected." << std::endl;
    
    for ( std::list<std::string>::const_iterator it = dg_job_list.begin(); it != dg_job_list.end(); it++) {
      commands::CommandFactory<commands::CommandFactoryClientImpl> factory;
      boost::scoped_ptr<commands::Command> cmd( factory.create("JobCancel") );
      cmd -> setParam("jobid", *it);
      cmd -> setParam("Host", connection->Host());
      result &= runCommand(cmd.get());
    }
  }
  disconnect();
  
  return result;
}

/**
 * Cancels all jobs in the given list and notifies the user.
 * Deprecated. This method will be soon removed due to architectural
 * changes.
 * @param dg_job_list the list of job identifiers to cancel.
 * @param user_contact the user mail recipient to send mail at.
 * @return the number of expected notifications.
 * @throws TimeoutException if error occured (timeout expires).
 * @throws ConnectionException if connection error occurs.
 * @throws AuthenticationException for authentication failure.
 */
int NSClient::jobCancel(const std::list<std::string>& dg_job_list, const std::string& user_contact)
{
  edglog_fn("NSC::jobCancel");
  edglog(critical) << "Client jobCancel with user contact Deprecated." << std::endl;

  connection -> DelegateCredentials(false);
  return 0;
}

/**
 * Cancels all jobs for given user certified string. Deprecated.
 * This method will soon be removed due to architectural changes.
 * @param ucs the user certified string.
 * @return the number of expected notifications.
 * @throws TimeoutException if data transmission errors occurs (timeout).
 * @throws ConnectionException if connection error occurs.
 * @throws AuthenticationException for authentication failure.
 */
int NSClient::jobCancelAll(const std::string& ucs)
{
  edglog_fn("NSC::jobCancelAll");
  edglog(critical) << "Client jobCancelAll. Deprecated." << std::endl;

  connection -> DelegateCredentials(false); 
  return 0;
}

/**
 * Cancels all jobs for given user certified string. Deprecated. This
 * method will soon be removed due to architectural changes.
 * @param ucs the user certified string.
 * @param user_contact the user mail recipient to send mail at.
 * @return the number of expected notifications.
 * @throws TimeoutException if data transmission errors occurs (timeout).
 * @throws ConnectionException if connection error occurs.
 * @throws AuthenticationException for authentication failure.
 */ 
int NSClient::jobCancelAll(const std::string& ucs, 
			   const std::string& user_contact)
{
  edglog_fn("NSC::jobCancelAll");
  edglog(critical) << "Client jobCancelAll with user contact. Deprecated." << std::endl;

  connection -> DelegateCredentials(false);
  return 0;
}

/**
 * Returns a multi-attribute list.
 * @param l the list of attributes.
 * @return true on success, false otherwise.
 * @throws ConnectionException on connection error.
 * @throws AuthenticationException for authentication failure.
 */
bool NSClient::getMultiattributeList(std::vector<std::string>&l)
{
  edglog_fn("NSC::getMAL");
  edglog(info) << "Client getMultiAttributeList." << std::endl;

  connection -> DelegateCredentials(false);
  if ( connect() ) {
    edglog(info) << "Connected." << std::endl;

    commands::CommandFactory<commands::CommandFactoryClientImpl> factory;
    boost::scoped_ptr<commands::Command> cmd( factory.create("GetMultiAttributeList") );
    runCommand(cmd.get());
    disconnect();	
    
    // Here we should log the attribute list returned.
    return cmd -> getParam("MultiAttributeList", l);
  }
  return false;
}


std::string NSClient::getSandboxRootPath()
{
  edglog_fn("NSC::getSRP");
  edglog(info) << "Client getSandboxRootPath." << std::endl;

  std::string path;
  connection -> DelegateCredentials(false);
  if ( connect() ) {
    edglog(info) << "Connected." << std::endl;

    commands::CommandFactory<commands::CommandFactoryClientImpl> factory;
    boost::scoped_ptr<commands::Command> cmd( factory.create("GetSandboxRootPath") );
    runCommand(cmd.get());
    disconnect();	
    cmd -> getParam("SandboxRootPath", path);

    edglog(info) << "Root Path: " << path << "." << std::endl;
  }
  return path;
}


bool NSClient::getOutputFilesList(const std::string& edg_jobid, std::vector<std::string>& l)
{
  edglog_fn("NSC:getOFL");
  edglog(info) << "Client OutputFilesList." << std::endl; 

  connection -> DelegateCredentials(false);  
 
  if ( connect() ) {
    edglog(info) << "Connected." << std::endl;
    commands::CommandFactory<commands::CommandFactoryClientImpl> factory;
    boost::scoped_ptr<commands::Command> cmd(factory.create("GetOutputFilesList"));
    cmd -> setParam("JobId", edg_jobid);
    runCommand(cmd.get());
    disconnect();       
    
    return cmd -> getParam("OutputFilesList", l);
  }

  return false; 
}


bool NSClient::getOutputFilesListSize(const std::string& edg_jobid, int& i)
{
  edglog_fn("NSC::getOFLSize");
  edglog(info) << "Client OutputFilesList." << std::endl; 

  connection -> DelegateCredentials(false);  
 
  if ( connect() ) {
    edglog(info) << "Connected." << std::endl;
    commands::CommandFactory<commands::CommandFactoryClientImpl> factory;
    boost::scoped_ptr<commands::Command> cmd(factory.create("GetOutputFilesListSize"));
    cmd -> setParam("JobId", edg_jobid);
    runCommand(cmd.get());
    disconnect();       
    
    return cmd -> getParam("OutputFilesListSize", i);
  }

  return false; 
}

bool NSClient::getQuota(std::pair<long, long>& quota)
{
  edglog_fn("NSC::getQ");
  edglog(info) << "Client getQuota." << std::endl;

  double soft = -1;
  double hard = -1;
  connection -> DelegateCredentials(false);
  if ( connect() ) {
    edglog(info) << "Connected." << std::endl;
    commands::CommandFactory<commands::CommandFactoryClientImpl> factory;
    boost::scoped_ptr<commands::Command> cmd( factory.create("GetQuota") );
    runCommand(cmd.get());
    disconnect();
    cmd -> getParam("SoftLimit", soft);
    cmd -> getParam("HardLimit", hard);
    edglog(info) << "Soft Limit: " << soft << ".\t" << "Hard Limit:" << hard << "." << std::endl;
  }    
  quota = std::make_pair( (long)soft, (long)hard );
  return ((soft != -1) && (hard != -1));
}

bool NSClient::getFreeQuota(std::pair<long, long>& quota)
{
  edglog_fn("NSC::getFQ");
  edglog(info) << "Client getFreeQuota." << std::endl;

  double soft = -1;
  double hard = -1;
  connection -> DelegateCredentials(false);
  if ( connect() ) {
    edglog(info) << "Connected." << std::endl;
    commands::CommandFactory<commands::CommandFactoryClientImpl> factory;
    boost::scoped_ptr<commands::Command> cmd( factory.create("GetQuota") );
    runCommand(cmd.get());
    disconnect();
    cmd -> getParam("SoftLimit", soft);
    cmd -> getParam("HardLimit", hard);
    edglog(info) << "Soft Limit: " << soft << ".\t" << "Hard Limit:" << hard << "." << std::endl;
    }
    quota = std::make_pair( (long)soft, (long)hard );
    return ((soft != -1) && (hard != -1) );
}

bool NSClient::getMaxInputSandboxSize(long& maxjobsize) {
  
  edglog_fn("NSC::getMISS");
  edglog(info) << "Client getMaxInputSandboxSize." << std::endl;
   
  connection -> DelegateCredentials(false);
  if ( connect() ) {
    edglog(info) << "Connected." << std::endl;
    commands::CommandFactory<commands::CommandFactoryClientImpl> factory;
    boost::scoped_ptr<commands::Command> cmd( factory.create("GetMaxInputSandboxSize") );
    runCommand(cmd.get());
    disconnect();
    double size = -1;
    cmd -> getParam("MaxInputSandboxSize", size);
    maxjobsize = (long)size;
    edglog(info) << "Max Input Sandbox Size: " << size << std::endl;
    return true;
  }
   
  maxjobsize = -1;
  return false;		
}

bool NSClient::getQuotaManagementStatus(bool& status) {
  edglog_fn("NSC::getQMS");
  edglog(info) << "Client getQuotaManagementStatus." << std::endl;

  connection -> DelegateCredentials(false);
  if ( connect() ) {
    edglog(info) << "Connected." << std::endl;
    commands::CommandFactory<commands::CommandFactoryClientImpl> factory;
    boost::scoped_ptr<commands::Command> cmd(factory.create("GetQuotaManagementStatus"));
    runCommand(cmd.get());
    disconnect();
    cmd -> getParam("QuotaOn", status);
    edglog(info) << "Quota Management: " << (status ? "On" : "Off") << std::endl;
    return true;
  }
  
  return false;
}

/**
 * Executes a the fsm related to a command.
 * @param cmd a pointer to the command.
 * @return true on successful run, false otherwise.
 */
bool NSClient::runCommand(commands::Command* cmd) {
  edglog_fn("NSC:runCommand");
  edglog(debug) << "Serializing Command ..." << std::endl;
  cmd -> serialize( connection->getAgent() );
  if ( cmd -> isDone() ) {
    edglog(fatal) << "Command seems to be broken." << std::endl;
    return false;
  }
#ifdef DEBUG_FSM
  try {
#endif
    while( cmd -> execute() && !cmd -> isDone() );
#ifdef DEBUG_FSM
  } catch (std::exception& e){
    edglog(debug) << "Exception caught:" << e.what() << std::endl;    
  }
#endif
  return cmd -> isDone();
}

/**
 * Returns the NS connection port.
 * @return the NS port, -1 otherwise.
 */
int NSClient::getPort() {
  if (connection) {
    return connection->port;
  }
  return -1;
}

/**
 * Returns the NS connection host.
 * @return the NS host, empty string otherwise.
 */
std::string NSClient::getHost() {
  if (connection) {
    return connection->host;
  }
  return std::string("");
}
   

} // namespace client
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

