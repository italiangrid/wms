/* LICENSE:
Copyright (c) Members of the EGEE Collaboration. 2010. 
See http://www.eu-egee.org/partners/ for details on the copyright
holders.  

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

   http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. 
See the License for the specific language governing permissions and 
limitations under the License.

END LICENSE */


#include "IceConfManager.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"

namespace conf_ns = glite::wms::common::configuration;
using namespace glite::wms::ice::util;
using namespace std;

IceConfManager* IceConfManager::s_instance = 0;
string IceConfManager::s_conf_file;
bool IceConfManager::s_initialized = false;

//____________________________________________________________________________
IceConfManager* IceConfManager::instance( )
    throw ( ConfigurationManager_ex&)
{
    if( !s_initialized ) {
        throw ConfigurationManager_ex("ConfigurationManager non initialized: must set the configuration filename before use");
    }
    if( !s_instance ) {
        s_instance = new IceConfManager( );
    }
    return s_instance;
}

//____________________________________________________________________________
IceConfManager::IceConfManager( )
    throw ( ConfigurationManager_ex& )
{
    conf_ns::Configuration* config = 0;
    try {
        config = new conf_ns::Configuration(s_conf_file, conf_ns::ModuleType::interface_cream_environment);
    } catch(exception& ex) {
        throw ConfigurationManager_ex( ex.what() );
    }
    m_configuration.reset( config );
}

IceConfManager::~IceConfManager( )
{

}

//____________________________________________________________________________
void IceConfManager::init( const string& filename )
{
    if ( !s_initialized ) {
        s_conf_file = filename;
        s_initialized = true;
    }
}
