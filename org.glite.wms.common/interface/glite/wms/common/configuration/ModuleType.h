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

#ifndef GLITE_WMS_COMMON_CONFIGURATION_MODULETYPE_H
#define GLITE_WMS_COMMON_CONFIGURATION_MODULETYPE_H

#include <vector>
#include <string>

namespace glite {
namespace wms {
namespace common {
namespace configuration {

class ModuleType {
public:
  enum module_type { unknown,
		     network_server,
		     workload_manager,
		     job_controller,
		     log_monitor,
		     wms_client,
                     workload_manager_proxy,
		     interface_cream_environment,
		     last_module
  };

  ModuleType( module_type type = unknown );
  ModuleType( const std::string &type );
  ModuleType( const char *type );

  ~ModuleType( void );

  inline module_type get_codetype( void ) const { return( this->mt_code ); }
  inline const std::string &get_stringtype( void ) const { return( this->mt_name ); }

  inline ModuleType &set_module( module_type type ) { this->setType( type ); return( *this ); }
  inline ModuleType &set_module( const std::string &type ) { this->setType( type ); return( *this ); }
  inline ModuleType &set_module( const char *type ) { this->setType( std::string(type) ); return( *this ); }

  static const std::string &module_name( module_type code );
  static module_type module_code( const std::string &name );

private:
  void setType( module_type type );
  void setType( const std::string &type );

  module_type     mt_code;
  std::string     mt_name;

  static std::vector<std::string>    mt_s_names;
};

} // Namespace configuration
} // common namespace end
} // wms namespace end
} // glite namespace end


#endif /* GLITE_WMS_COMMON_CONFIGURATION_MODULETYPE_H */

// Local Variables:
// mode: c++
// End:
