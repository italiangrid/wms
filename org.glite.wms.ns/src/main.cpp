
/*
 * File: main.cpp
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// Id: $
 
#include "glite/wms/ns-common/NetworkServer.h"
#include "NS2WMProxy.h"
#include "glite/wms/ns-common/logging.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/ModuleType.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/LineParserExceptions.h"
#include "glite/wmsutils/classads/classad_utils.h"
//#include "glite/wms/classad_plugin/classad_plugin_loader.h"

#include <globus_common.h>

#include <boost/scoped_ptr.hpp>

using namespace glite::wms::manager::ns::daemon;
using namespace glite::wms::common::utilities;
using namespace glite::wmsutils::classads;

namespace configuration = glite::wms::common::configuration;
namespace logger        = glite::wms::common::logger;

LineOption  options[] = {
	{ 'c', 1, "conf_file", "\t use conf_file as configuration file." },
	{ 'd', no_argument, "\t disable daemonize.", "\t run as daemon." },
	{ 'r', no_argument, "\t disable privileges drop.", "\t run as root process." }
};
//no_daemon
//as_root
// to be added

int main(int argc, char* argv[])
{
  std::vector<LineOption>        optvec( options, options + sizeof(options)/sizeof(LineOption) );
  LineParser                     options( optvec, 0 );
  std::string conf_file;
  bool     daemon_flag = true;
  bool       drop_flag = true;

  //FIXME: should be removed if/when the broker_helper DL is used.
  // glite::wms::classad_plugin::classad_plugin_loader init;
  
  try {
  
        options.parse( argc, argv );
        conf_file.assign( options.is_present('c') ? options['c'].getStringValue() : "glite_wms.conf" );
	if (options.is_present('d')) {
	  daemon_flag = false;
	}
	if (options.is_present('r')) {
	  drop_flag = false;
	}
        std::auto_ptr<configuration::Configuration> conf;
        conf.reset(new configuration::Configuration( conf_file, "NetworkServer"));

	logger::threadsafe::edglog.open( 
	      configuration::Configuration::instance()->ns()->log_file(),
     	      static_cast<logger::level_t>(configuration::Configuration::instance()->ns()->log_level()) );


	edglog_fn("   NS::main  ");
	edglog(fatal) << "--------------------------------------" << std::endl;
	edglog(fatal) << "Starting Network Server..." << std::endl;
	
	logger::threadsafe::edglog.activate_log_rotation (
		configuration::Configuration::instance()->ns()->log_file_max_size(),
		configuration::Configuration::instance()->ns()->log_rotation_base_file(),
	        configuration::Configuration::instance()->ns()->log_rotation_max_file_number() );

	if (globus_module_activate(GLOBUS_COMMON_MODULE) != GLOBUS_SUCCESS)   {
	    edglog_fn("NS::main");
	    edglog(fatal) << "Failed to initialize Globus common module" << std::endl;
	    exit( -1 );
	}

	singleton_default<NetworkServer>::instance().set_daemonize(daemon_flag);	
	singleton_default<NetworkServer>::instance().set_privileges(drop_flag);
	singleton_default<NetworkServer>::instance().init();
	singleton_default<NS2WMProxy>::instance().init(configuration::Configuration::instance() -> wm() -> input() );
	edglog(fatal) << "Initialization Done. Running Server..." << std::endl;
	edglog(fatal) << "--------------------------------------" << std::endl;
	singleton_default<NetworkServer>::instance().run();
  } 
  catch( LineParsingError &error ) {
    std::cerr << error << std::endl;
    exit( error.return_code() );
  }
  catch(std::exception& e) {
    edglog(fatal) << "Exception caught: " << e.what() << std::endl;
  } 
  catch( ... ) {
    edglog(fatal) << "Uncaught exception...." << std::endl;	 
  }
  
}
