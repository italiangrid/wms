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
