#ifndef GLITE_WMS_COMMON_CONFIGURATION_LMCONFIGURATION_H
#define GLITE_WMS_COMMON_CONFIGURATION_LMCONFIGURATION_H

#include "glite/wms/common/configuration/confbase.h"

namespace glite {
namespace wms {
namespace common {
namespace configuration {

class LMConfiguration : public confbase_c {
public:
  LMConfiguration( const classad::ClassAd *ad );

  ~LMConfiguration( void );

  /* Tmpl: partial include class LMConfiguration */
};

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_CONFIGURATION_LMCONFIGURATION_H */

// Local Variables:
// mode: c++
// End:
