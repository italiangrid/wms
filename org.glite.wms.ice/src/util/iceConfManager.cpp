
#include "iceConfManager.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"

using namespace std;
namespace conf = glite::wms::common::configuration;

glite::wms::ice::util::iceConfManager* glite::wms::ice::util::iceConfManager::instance = NULL;
string glite::wms::ice::util::iceConfManager::conf_file = "";
bool glite::wms::ice::util::iceConfManager::initialized = false;
boost::recursive_mutex glite::wms::ice::util::iceConfManager::mutex;

//______________________________________________________________________________
glite::wms::ice::util::iceConfManager* 
glite::wms::ice::util::iceConfManager::getInstance( )
  throw (glite::wms::ice::util::ConfigurationManager_ex&)
{
  boost::recursive_mutex::scoped_lock M( mutex );
  if(!initialized) 
    throw glite::wms::ice::util::ConfigurationManager_ex("ConfigurationManager non initialized: must set the configuration filename before use");
  if(!instance)
    instance = new iceConfManager();
  return instance;
}

//______________________________________________________________________________
glite::wms::ice::util::iceConfManager::iceConfManager()
  throw (glite::wms::ice::util::ConfigurationManager_ex&)
  : HostProxyFile( "" ),
    WM_Input_FileList( "" ),
    ICE_Input_FileList( "" ),
    CachePersistFile( "" ),
    LogFile( "" ),
    ListenerPort( 0 )
{
  //conf::ICEConfiguration const* ice_config;
  //conf::WMConfiguration const* wm_config;
  conf::Configuration* config;
  try {
    config = 
      new conf::Configuration(conf_file, 
			      conf::ModuleType::interface_cream_environment);
    //ice_config = config->ice();
    //wm_config  = config->wm();
  } catch(exception& ex) {
    throw glite::wms::ice::util::ConfigurationManager_ex( ex.what() );
  }
  
  HostProxyFile     = config->common()->host_proxy_file();
  WM_Input_FileList = config->wm()->input();
  ICE_Input_FileList= config->ice()->input();
  CachePersistFile  = config->ice()->cache_persist_file();
  ListenerPort      = config->ice()->listener_port();
  LogFile           = config->ice()->logfile();
}

//______________________________________________________________________________
void glite::wms::ice::util::iceConfManager::init(const string& filename)
{
  boost::recursive_mutex::scoped_lock M( mutex );
  if(initialized) return;
  conf_file = filename;
  initialized = true;
}
