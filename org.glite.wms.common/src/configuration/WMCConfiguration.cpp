#include "WMCConfiguration.h"

namespace glite {
namespace wms {
namespace common {
namespace configuration {

WMCConfiguration::WMCConfiguration( const classad::ClassAd *ad ) : confbase_c( ad )
{}

WMCConfiguration::~WMCConfiguration( void )
{}

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace

