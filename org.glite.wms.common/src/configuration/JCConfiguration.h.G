#ifndef GLITE_WMS_COMMON_CONFIGURATION_JCCONFIGURATION_H
#define GLITE_WMS_COMMON_CONFIGURATION_JCCONFIGURATION_H

#include "glite/wms/common/configuration/confbase.h"

namespace glite {
namespace wms {
namespace common {
namespace configuration {

class JCConfiguration : public confbase_c {
public:
  JCConfiguration( const classad::ClassAd *ad );

  ~JCConfiguration( void );

  /* Tmpl: partial include class JCConfiguration */
};

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_CONFIGURATION_JCCONFIGURATION_H */

// Local Variables:
// mode: c++
// End:
