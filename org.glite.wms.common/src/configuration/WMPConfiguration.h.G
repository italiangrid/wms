#ifndef GLITE_WMS_COMMON_CONFIGURATION_WMPCONFIGURATION_H
#define GLITE_WMS_COMMON_CONFIGURATION_WMPCONFIGURATION_H

#include "glite/wms/common/configuration/confbase.h"

namespace glite {
namespace wms {
namespace common {
namespace configuration {

class WMPConfiguration : public confbase_c {
public:
  WMPConfiguration( const classad::ClassAd *ad );

  ~WMPConfiguration( void );

  /* Tmpl: partial include class WMPConfiguration */
};

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_CONFIGURATION_WMPCONFIGURATION_H */

// Local Variables:
// mode: c++
// End:
