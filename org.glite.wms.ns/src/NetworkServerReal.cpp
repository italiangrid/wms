
/*
 * File: NetworkServerReal.cpp
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Author: Marco Pappalardo <marco.pappalardo@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/regex.hpp>
#include <boost/bind.hpp>
#include <classad_distribution.h>

#include "NetworkServerReal.h"
#include "NS2WMProxy.h"
#include "glite/wms/ns-common/NetworkServer.h"
#include "exceptions.h"
#include "glite/wms/ns-common/Command.h"
#include "glite/wms/ns-common/CommandFactory.h"
#include "glite/wms/ns-common/CommandFactoryServerImpl.h"
#include "glite/wms/ns-common/logging.h"
#include "glite/wms/common/task/Task.h"
#include "glite/wmsutils/exception/Exception.h"
#include "glite/wmsutils/tls/socket++/GSISocketAgent.h"
#include "glite/wmsutils/tls/socket++/GSISocketServer.h"
#include "glite/wmsutils/tls/socket++/exceptions.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/ModuleType.h"
#include "glite/wms/common/configuration/exceptions.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/process/process.h"
#include "glite/wms/common/process/user.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/common/logger/edglog.h" 
#include "glite/wms/common/logger/manipulators.h" 
#include "signal_handling.h"
#include "glite/wms/common/utilities/scope_guard.h"

namespace common        = glite::wms::common;
namespace task          = glite::wms::common::task;
namespace socket_pp     = glite::wmsutils::tls::socket_pp;
namespace process       = glite::wms::common::process;
namespace configuration = glite::wms::common::configuration;
namespace logger        = glite::wms::common::logger; 
namespace commands      = glite::wms::manager::ns::commands;
namespace fs            = boost::filesystem;

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace daemon {

class Listener: public task::PipeWriter< socket_pp::GSISocketAgent* >
{
public:
  void operator()()
  {
    socket_pp::GSISocketAgent* agent = 0;
    edglog_fn("Listener::run");
    try {
      while(true) {
        try {
	   agent =  UIserver -> Listen();
           if( agent ) {
             edglog(debug) << "Handling incoming connection on socket #" << agent->SocketDescriptor() << std::endl;
             edglog(debug) << "Request source is: " << agent -> PeerAddr() << ":" << agent -> PeerPort() << std::endl;
             // edglog(debug) << "Request source is: " << agent -> HostName() << ":" 
             //              << agent -> PeerPort() << std::endl;
             write_end().write( agent );
           }
        } 
	catch( glite::wmsutils::exception::Exception& e) {
	  edglog(fatal) << "Exception Caught: " << e.what() << std::endl;
	}
      }	
    }
    catch (task::SigPipe&) {
      if( agent ) UIserver -> KillAgent( agent );
      edglog(fatal) << "Pipe Closed." << std::endl;
    }
  }
  static GSISocketServerPtr UIserver;
};

GSISocketServerPtr Listener::UIserver;

namespace {
    boost::mutex auth_mutex;
}

class Manager: public task::PipeForwarder< socket_pp::GSISocketAgent*, classad::ClassAd* >
{

public:
  void operator()()
  {
    edglog_fn("Manager::run");
    try {
      while (true) {
              boost::scoped_ptr<commands::Command> cmd;
	      socket_pp::GSISocketAgent* agent = read_end().read();
    
              common::utilities::scope_guard kill_agent(
                boost::bind(&socket_pp::GSISocketServer::KillAgent, UIserver,  agent)
              );

	      std::string dname;
	      try {
		  // Locking a mutex to adrress a globus internal problem.
		  boost::mutex::scoped_lock l(auth_mutex);
		  // Addressing LCG Bug #2716
		  agent->SetTimeout(configuration::Configuration::instance() -> ns() -> connection_timeout());
		  if (!UIserver -> AuthenticateAgent(agent)) {
		    continue;
		  }
		  // Publishing DistinguishedName for LCG bug #3808
		  dname = agent -> CertificateSubject();
		  edglog(debug) << "Authenticated DN=\"" + dname + "\"" << std::endl;
		  // Resetting Timeouts
		  agent -> SetTimeout(-1);
	      } catch( socket_pp::Exception& e) {
		  // No need to log abort. The multi call fails on UI and no register is done.
		  edglog(fatal) << "Exception Caught during Agent Authentication." << e.what() << std::endl;
		  continue;
	      }

	      std::string cmdname;
	      agent -> Receive(cmdname);
	      
	      commands::CommandFactory<commands::CommandFactoryServerImpl> factory;
	      bool is_forwarded = false;
	      try {
		  cmd.reset(factory.create(cmdname));
		  cmd -> serialize( agent );
		  cmd -> setParam("DistinguishedName", dname);
		  assert( !cmd -> isDone() );
		  do {
			  
		    if( cmd -> state().forwardRequest() ) {
			    is_forwarded = true;
			    // The context may not be set in case of a match command
			    if (*cmd->getLogContext()) {
		    	    	char* seq_str = edg_wll_GetSequenceCode(*cmd->getLogContext());
			    	std::string seq_code(seq_str);		    
			    	free( seq_str );
			    	cmd->setParam("SeqCode", seq_code);	    
			    }
			    write_end().write(static_cast<classad::ClassAd*>(cmd->asClassAd().Copy()));
			    edglog(fatal) << "Command Forwarded." << std::endl;
		    }
		    
		  } while( cmd -> execute() && !cmd -> isDone() );
	      } 
	      catch (commands::bad&) {
	        edglog(fatal) << "Bad Command." << std::endl;
	      }
	      catch( socket_pp::Exception& e) {
	      	edglog(fatal) << "Exception Caught during client communication via socket: " 
		       << e.what() << std::endl;
		// Log abort so that the job won't remain in waiting status
		// if and only if the job has not been forwarded yet.
	      	edglog(fatal) << "Logging Aborted" << std::endl;
		if( !is_forwarded && (*cmd->getLogContext()) ) {
		  int retry = 10; // this should be taken from the configuration file...
#ifdef GLITE_WMS_HAVE_LBPROXY 
		  while( edg_wll_LogAbortProxy(*(cmd->getLogContext()), e.what()) && retry-- );
#else
		  while( edg_wll_LogAbort(*(cmd->getLogContext()), e.what()) && retry-- );
#endif
	        }
	      }
	      catch( glite::wmsutils::exception::Exception& e) {
		edglog(fatal) << "Exception Caught: " << e.what() << std::endl;
	      }
      }
    } 
    catch (task::Eof&) {
      edglog(fatal) << "Pipe Closed for reading." << std::endl;
    } 
    catch (task::SigPipe&) {
      edglog(fatal) << "Pipe Closed for writing." << std::endl;
    } 
    catch (commands::bad&) {
      edglog(fatal) << "Bad Command." << std::endl;
    } 
    catch( socket_pp::Exception& e) {
      edglog(fatal) << "Manager: " << e.what() << std::endl;
    } 
    catch( std::exception& e ) {
      edglog(fatal) << "Manager: " << e.what() << std::endl;
    } 
    catch (...) {
      edglog(fatal) << "Uncaught Exception: plese check." << std::endl;
    }
 
  }
  static GSISocketServerPtr UIserver;
};

GSISocketServerPtr Manager::UIserver;

class Dispatcher: public task::PipeReader< classad::ClassAd* >
{
public:
  void operator()()
  {
    try {
      edglog_fn("Dispatcher::run");
      while (true) {
	try {
          boost::scoped_ptr< classad::ClassAd > cmdAd( read_end().read() );
	  std::string cmdName;
	  try {
	    cmdName.assign(glite::wmsutils::classads::evaluate_attribute(*cmdAd, "Command"));
	    edglog(critical) << "Command to dispatch: " << cmdName << std::endl;
	  }		
	  catch(glite::wmsutils::classads::InvalidValue &e) {
	    edglog(fatal) << "Missing Command name." << std::endl;
	  }
	  
	  if ( cmdName == "JobSubmit") { 
	    singleton_default<NS2WMProxy>::instance().submit(cmdAd.get());
	  }
	  else if ( cmdName == "JobCancel" ) {
	    singleton_default<NS2WMProxy>::instance().cancel(cmdAd.get());
	  }
	  else if ( cmdName == "DagSubmit" ) {
	    singleton_default<NS2WMProxy>::instance().submit(cmdAd.get());
	  }
	  else if ( cmdName == "ListJobMatch" ) {
	    singleton_default<NS2WMProxy>::instance().match(cmdAd.get());
	  } else {
	    edglog(fatal) << "No forwarding procedure defined for this command." << std::endl;
	  }
	} 
	catch(utilities::Exception& e) {
	  edglog(fatal) << "Exception Caught:" << e.what() << std::endl;
	} 
	catch( std::exception& ex ) {
	  edglog(fatal) << "Exception Caught:" << ex.what() << std::endl;
	}
      }
    } catch (task::Eof&) {
      edglog(fatal) << "Pipe Closed." << std::endl;
    }
  }
};

NetworkServerReal::NetworkServerReal()
{
  daemonize_flag = true;
  privileges_flag = true;
}
 
NetworkServerReal::~NetworkServerReal()
{
}

void NetworkServerReal::init()
{ 
  edglog_fn("  NSR::init  ");

  try {
    if (privileges_flag) {
      drop_privileges();
    }
    if (daemonize_flag) {
      do_daemonize();
    }

    NSconf = configuration::Configuration::instance() -> ns();

    UIserver.reset( new socket_pp::GSISocketServer(NSconf->listening_port(), NSconf->backlog_size() ));
    UIserver -> set_auth_timeout(NSconf -> connection_timeout());
    UIserver -> Open();
    edglog(fatal) << "Listening on port: " << NSconf->listening_port() << std::endl;

    // *auth_mutex = PTHREAD_MUTEX_INITIALIZER;
    edglog(medium) << "Authentication Handshaking Enabling: Ok." << std::endl;

    // if (clearpipes_flag) {
       do_clearpipes();
    // }

    Listener::UIserver = UIserver;
    Manager::UIserver  = UIserver;

  } catch (CannotStartException e) {
    edglog(fatal) << "CannotStart Daemon: " << e.what() << std::endl;
    std::cout << "CannotStart Daemon: " << e.what() << std::endl;
    exit (-1);
  }
  catch( configuration::ModuleType &type ) {
    edglog(fatal) << "Unknown module type: " << type.get_stringtype() << std::endl;
    std::cout << "Unknown module type: " << type.get_stringtype() << std::endl;
    throw;
  }
  catch( configuration::InvalidExpression &error ) {
    std::cout << "Invalid expression: " << error << std::endl;
    edglog(fatal) << "Invalid expression: " << error << std::endl;
    throw;
  }
  catch( configuration::CannotOpenFile &file ) {
    edglog(fatal) << "Cannot open file: " << file << std::endl;
    std::cout << "Cannot open file: " << file << std::endl;
    throw;
  }
  catch( configuration::CannotConfigure &error ) {
    edglog(fatal) << "Cannot configure: " << error << std::endl;
    std::cout << "Cannot configure: " << error << std::endl;
    throw;
  }
  catch( socket_pp::IOException& e) {
    edglog(fatal) << "IOException caught: " << e.what() << std::endl;
    std::cout << "IOException caught: " << e.what() << std::endl;
    throw;
  } 	  
}

void NetworkServerReal::run()
{ 
  edglog_fn("NSR::run");
  if(UIserver.get()) {

    task::Pipe< socket_pp::GSISocketAgent*   > p1;
    task::Pipe< classad::ClassAd*            > p2;
    
    Listener   listener;
    Manager    manager;
    Dispatcher dispatcher;
    
    
    task::Task listeners   (listener,   p1,     1);
    
    try {
      task::Task managers    (manager, p1, p2, NSconf->master_threads());
      task::Task dispatchers (dispatcher, p2, NSconf->dispatcher_threads());
      while(!received_quit_signal()) {
        sleep(1);
      }
      exit(0);
    } catch ( configuration::InvalidExpression& error ) {
      edglog(fatal) << "Invalid Expression: " << error << std::endl;
    }
  }
  exit(-1);
}

void NetworkServerReal::shutdown()
{
/*
  do_shutdown.notify_one();
  edglog_fn("NSR::shutdown");
  edglog(fatal) << "Network Server Shutdown..." << std::endl;
*/
}

void NetworkServerReal::drop_privileges()
{
  edglog_fn("  NSR::drop  ");
  process::User currentUser;
  const configuration::CommonConfiguration *commonconf = configuration::Configuration::instance()->common(); 
  
  if( currentUser.uid() == 0 ) { 
    edglog(fatal) << "Running as root, trying to lower permissions..." << std::endl;
 
    if( process::Process::self()->drop_privileges(commonconf->dguser().c_str()) ) {
       edglog(fatal) << "Failed: reason \"" << strerror(errno) << "\"" << std::endl;
       throw CannotStartException( "Cannot drop privileges, avoiding running as root." );
    }
  
    edglog(fatal) << "Running as user " << commonconf->dguser() << "..." << std::endl;
  } 
  else edglog(fatal) << "Already running in an unprivileged account..." << std::endl;
}

void NetworkServerReal::do_daemonize()
{ 
  edglog_fn("  NSR::daemon");
  process::Process::self()->make_daemon();
  edglog(fatal) << "Daemon Started..." << std::endl; 
} 

bool NetworkServerReal::do_clearpipes()
{
  const std::string lm_root_path = NSconf->list_match_root_path();
  const fs::path p = fs::path( lm_root_path , fs::native );
  std::string file_name_initials("0x");
  
  if ( !fs::exists( p ) ) {
    edglog(fatal) << "ListMatch Root Path directory does not exist!" << std::endl;
    edglog(fatal) << "Creating: " << lm_root_path << std::endl;
    try { 
      fs::create_directory( p );
      edglog(fatal) << "Creation Successful." << std::endl;
    } catch (fs::filesystem_error& e) {
      edglog(fatal) << "Creation Failed." << std::endl;
      return false;
    }
  } else {
    static boost::regex expression( "0x([[:xdigit:]]+).([[:digit:]]+)" );
    static boost::regex proxy_expression ( "user.proxy.0x([[:xdigit:]]+).([[:digit:]]+)" );
    fs::directory_iterator end_itr; // default construction yields past-the-end
    for ( fs::directory_iterator itr( p );
	  itr != end_itr;
	  ++itr )
      try {
	if ( !fs::is_directory( *itr ) )
	  {
	    std::string candidate_file( itr->leaf() );
	    if (boost::regex_match(candidate_file, expression)
		||
		boost::regex_match(candidate_file, proxy_expression)
		) {
	      remove(*itr);
	      edglog(fatal) << ">> File: " <<  candidate_file << " removed." << std::endl;
	    }
	  }
      } catch ( fs::filesystem_error& e ) {
	edglog(severe) << "Exception caught during listmatch pipe and proxy removal." << std::endl;
	edglog(severe) << e.what() << std::endl;
	edglog(fatal) << "File not removed." << std::endl;
      }
  }
  return true;
}


void NetworkServerReal::set_daemonize(bool flag) 
{
  daemonize_flag = flag;
}

void NetworkServerReal::set_privileges(bool flag) 
{
  privileges_flag = flag;
}

const configuration::NSConfiguration& NetworkServerReal::configuration()
{
 return *NSconf;
}

} // namespace daemon 
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite
