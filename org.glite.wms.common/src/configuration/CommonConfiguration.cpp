#include "CommonConfiguration.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace configuration {

CommonConfiguration::CommonConfiguration( const classad::ClassAd *ad ) : confbase_c( ad )
{}

CommonConfiguration::~CommonConfiguration( void )
{}

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace
