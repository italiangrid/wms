#include "NSConfiguration.h"

namespace glite {
namespace wms {
namespace common {
namespace configuration {

NSConfiguration::NSConfiguration( const classad::ClassAd *ad ) : confbase_c( ad )
{}

NSConfiguration::~NSConfiguration( void )
{}

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace

