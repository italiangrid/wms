#include "ICEConfiguration.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace configuration {

ICEConfiguration::ICEConfiguration( const classad::ClassAd *ad ) : confbase_c( ad )
{}

ICEConfiguration::~ICEConfiguration( void )
{}

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace
