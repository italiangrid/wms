#ifndef GLITE_WMS_COMMON_CONFIGURATION_NSCONFIGURATION_H
#define GLITE_WMS_COMMON_CONFIGURATION_NSCONFIGURATION_H

#include "glite/wms/common/configuration/confbase.h"

namespace glite {
namespace wms {
namespace common {
namespace configuration {

class NSConfiguration : public confbase_c {
public:
  NSConfiguration( const classad::ClassAd *ad );

  ~NSConfiguration( void );

  /* Tmpl: partial include class NSConfiguration */
};

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_CONFIGURATION_NSCONFIGURATION_H */

// Local Variables:
// mode: c++
// End:
