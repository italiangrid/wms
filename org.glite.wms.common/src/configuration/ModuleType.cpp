#include "glite/wms/common/configuration/ModuleType.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace configuration {

static char      *names[] = { "Unknown Module", "NetworkServer", "WorkloadManager", "JobController", "LogMonitor", "WmsClient", "WorkloadManagerProxy" };

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
