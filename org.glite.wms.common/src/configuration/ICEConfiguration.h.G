#ifndef GLITE_WMS_COMMON_CONFIGURATION_ICECONFIGURATION_H
#define GLITE_WMS_COMMON_CONFIGURATION_ICECONFIGURATION_H

#include "glite/wms/common/configuration/confbase.h"

namespace glite {
namespace wms {
namespace common {
namespace configuration {

class ICEConfiguration : public confbase_c {
public:
  ICEConfiguration( const classad::ClassAd *ad );

  ~ICEConfiguration( void );

  /* Tmpl: partial include class ICEConfiguration */
};

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_CONFIGURATION_ICECONFIGURATION_H */

// Local Variables:
// mode: c++
// End:
