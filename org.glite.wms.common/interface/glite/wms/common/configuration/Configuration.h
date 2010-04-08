/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org\partners for details on the
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

#ifndef GLITE_WMS_COMMON_CONFIGURATION_CONFIGURATION_H
#define GLITE_WMS_COMMON_CONFIGURATION_CONFIGURATION_H

#include <string>
#include <memory>

#include "ModuleType.h"

namespace classad { class ClassAd; }

namespace glite {
namespace wms {
namespace common {
namespace configuration {

// Forward declarations
class JCConfiguration;
class LMConfiguration;
class NSConfiguration;
class WMConfiguration;
class WMCConfiguration;
class WMPConfiguration;
class ICEConfiguration;
class CommonConfiguration;

class Configuration {
public:
  Configuration( const std::string &filename, const ModuleType &type );
  Configuration( const ModuleType &type );

  ~Configuration( void );

  inline ModuleType::module_type get_module( void ) const { return this->c_mtype.get_codetype(); }

  inline const JCConfiguration  *jc( void ) const { return this->c_jc.get(); }
  inline const LMConfiguration  *lm( void ) const { return this->c_lm.get(); }
  inline const NSConfiguration  *ns( void ) const { return this->c_ns.get(); }
  inline const WMConfiguration  *wm( void ) const { return this->c_wm.get(); }
  inline const WMCConfiguration *wc( void ) const { return this->c_wc.get(); }
  inline const WMPConfiguration *wp( void ) const { return this->c_wp.get(); }
  inline const ICEConfiguration *ice( void ) const { return this->c_ice.get(); }
  inline const CommonConfiguration *common( void ) const { return this->c_common.get(); }

  inline static const Configuration *instance( void ) { return c_s_instance; }

  classad::ClassAd *get_classad( void );

private:
  void createConfiguration( const std::string &filename );
  void loadFile( const char *filename );

  std::auto_ptr<JCConfiguration>       c_jc;
  std::auto_ptr<LMConfiguration>       c_lm;
  std::auto_ptr<NSConfiguration>       c_ns;
  std::auto_ptr<WMConfiguration>       c_wm;
  std::auto_ptr<WMCConfiguration>      c_wc;
  std::auto_ptr<WMPConfiguration>      c_wp;
  std::auto_ptr<ICEConfiguration>      c_ice;
  std::auto_ptr<CommonConfiguration>   c_common;
  std::auto_ptr<classad::ClassAd>      c_read;
  ModuleType                           c_mtype;

  static const Configuration  *c_s_instance;
  static const char           *c_s_paths[];

  Configuration( const Configuration &c ); // Not implemented
  Configuration &operator=( const Configuration &c ); // Not implemented
};

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_CONFIGURATION_CONFIGURATION_H */

// Local Variables:
// mode: c++
// End:
