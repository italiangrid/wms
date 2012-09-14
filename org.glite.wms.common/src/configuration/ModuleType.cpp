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

#include "glite/wms/common/configuration/ModuleType.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace configuration {

static char      *names[] = { "Unknown Module", "NetworkServer", "WorkloadManager", "JobController", "LogMonitor", "WorkloadManagerProxy", "ICE" };

vector<string>    ModuleType::mt_s_names( names, names + (int) ModuleType::last_module );

/*
  Private methods
*/

void ModuleType::setType( module_type type )
{
  int    itype = (int) type;

  if( (itype <= (int) unknown) || (itype >= (int) last_module) ) type = unknown;

  this->mt_code = type;
  this->mt_name.assign( mt_s_names[(int) type] );

  return;
}

void ModuleType::setType( const string &type )
{
  int      i;

  for( i = (((int) unknown) + 1); i < (int) last_module; i++ )
    if( type == mt_s_names[i] )
      break;

  if( i == (int) last_module ) i = (int) unknown;

  this->mt_code = (module_type) i;
  this->mt_name.assign( mt_s_names[i] );

  return;
}

ModuleType::ModuleType( module_type type ) : mt_code( unknown ), mt_name()
{ this->setType( type ); }

ModuleType::ModuleType( const std::string &type ) : mt_code( unknown ), mt_name()
{ this->setType( type ); }

ModuleType::ModuleType( const char *type ) : mt_code( unknown ), mt_name()
{ this->setType( string(type) ); }

ModuleType::~ModuleType( void )
{}

/*
  Static methods
*/

const string &ModuleType::module_name( module_type code )
{
  int     type = (int) code;

  if( (type <= (int) unknown) || (type >= (int) last_module) ) code = unknown;

  return( mt_s_names[(int) code] );
}

ModuleType::module_type ModuleType::module_code( const string &type )
{
  int      i;

  for( i = (((int) unknown) + 1); i < (int) last_module; i++ )
    if( type == mt_s_names[i] )
      break;

  if( i == (int) last_module ) i = (int) unknown;

  return( (module_type) i );
}

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace
