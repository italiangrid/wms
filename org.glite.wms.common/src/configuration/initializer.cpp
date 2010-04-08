/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <memory>
#include <string>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/initializer.h"
#include "glite/wms/common/configuration/exceptions.h"

using namespace std;
using namespace glite::wms::common;

static bool                                  edg_wlcc_removeConfig = false;
static const configuration::Configuration   *edg_wlcc_mainConfig = NULL;
static string                                edg_wlcc_errorMessage;

extern "C" {

config_error_t edg_wlcc_ConfigurationInitialize( const char *filename, const char *modulename )
{
  config_error_t err = edg_wlcc_OK;

  if( edg_wlcc_mainConfig != NULL ) {
    err = edg_wlcc_exists;
    edg_wlcc_errorMessage.assign( "Configuration already initialized." );
  }
  else {
    try {
      edg_wlcc_mainConfig = configuration::Configuration::instance();
      edg_wlcc_removeConfig = ( edg_wlcc_mainConfig == NULL );

      if( edg_wlcc_removeConfig )
	edg_wlcc_mainConfig = new configuration::Configuration( filename, modulename );
    }
    catch( configuration::CannotConfigure &error ) {
      edg_wlcc_errorMessage.assign( error.reason() );

      err = edg_wlcc_error;
    }
  }

  return err;
}

void edg_wlcc_ConfigurationFree( void )
{
  if( edg_wlcc_removeConfig ) {
    delete edg_wlcc_mainConfig;

    edg_wlcc_mainConfig = NULL;
    edg_wlcc_removeConfig = false;
  }
}

const char *edg_wlcc_ConfigurationError( void )
{
  return edg_wlcc_errorMessage.c_str();
}

void edg_wlcc_SetConfigurationError( const char *error )
{
  edg_wlcc_errorMessage.assign( error );
}

} /* extern "C" */
