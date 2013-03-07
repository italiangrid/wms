#ifndef GLITE_WMS_COMMON_CONFIGURATION_COMMONCONFIGURATION_H
#define GLITE_WMS_COMMON_CONFIGURATION_COMMONCONFIGURATION_H

#include "glite/wms/common/configuration/confbase.h"

namespace glite {
namespace wms {
namespace common {
namespace configuration {

class CommonConfiguration : public confbase_c {
public:
  CommonConfiguration( const classad::ClassAd *ad );

  ~CommonConfiguration( void );

  /* Tmpl: partial include class CommonConfiguration */
};

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_CONFIGURATION_COMMONCONFIGURATION_H */

// Local Variables:
// mode: c++
// End:
