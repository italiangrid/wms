#ifndef GLITE_WMS_COMMON_CONFIGURATION_WMC_CONFIGURATION_H
#define GLITE_WMS_COMMON_CONFIGURATION_WMC_CONFIGURATION_H

#ifndef GLITE_WMS_COMMON_CONFIGURATION_CONFBASE_H
#include "glite/wms/common/configuration/confbase.h"
#endif

namespace glite {
namespace wms {
namespace common {
namespace configuration {

class WMCConfiguration : public confbase_c {
public:
  WMCConfiguration( const classad::ClassAd *ad );

  ~WMCConfiguration( void );

  /* Tmpl: partial include class WMCConfiguration */
};

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_CONFIGURATION_WMC_CONFIGURATION_H */

// Local Variables:
// mode: c++
// End:
