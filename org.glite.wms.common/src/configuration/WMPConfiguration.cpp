#include "WMPConfiguration.h"

namespace glite {
namespace wms {
namespace common {
namespace configuration {

WMPConfiguration::WMPConfiguration( const classad::ClassAd *ad ) : confbase_c( ad )
{}

WMPConfiguration::~WMPConfiguration( void )
{}

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace

