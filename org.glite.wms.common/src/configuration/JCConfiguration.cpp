#include "JCConfiguration.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace configuration {

JCConfiguration::JCConfiguration( const classad::ClassAd *ad ) : confbase_c( ad )
{}

JCConfiguration::~JCConfiguration( void )
{}

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace
