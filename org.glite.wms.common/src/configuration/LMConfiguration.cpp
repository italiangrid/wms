#include "LMConfiguration.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace configuration {

LMConfiguration::LMConfiguration( const classad::ClassAd *ad ) : confbase_c( ad )
{}

LMConfiguration::~LMConfiguration( void )
{}

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace
