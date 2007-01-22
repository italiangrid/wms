#ifndef EDG_WORKLOAD_JOBCONTROL_NAMESPACE_H_LOADED
#define EDG_WORKLOAD_JOBCONTROL_NAMESPACE_H_LOADED

#ifndef USING_COMMON_NAMESPACE
#define USING_COMMON_NAMESPACE using namespace glite::wms::common
#endif
#ifndef COMMON_SUBNAMESPACE_CLASS
#define COMMON_SUBNAMESPACE_CLASS( Namespace, Type )    \
namespace glite { namespace wms { namespace common {    \
  namespace Namespace {                                 \
    class Type;                                         \
  }}}}
#endif

#ifndef COMMON_SUBNAMESPACE_CLASS_J
#define COMMON_SUBNAMESPACE_CLASS_J( Namespace, Type )    \
namespace glite { namespace wms {                       \
  namespace Namespace {                                 \
    class Type;                                         \
  }}}
#endif

#define JOBCONTROL_NAMESPACE_BEGIN namespace glite { namespace wms { namespace jobsubmission

#define JOBCONTROL_NAMESPACE_END }}

#define USING_JOBCONTROL_NAMESPACE using namespace glite::wms::jobsubmission
#define USING_JOBCONTROL_NAMESPACE_ADD( last ) using namespace glite::wms::jobsubmission::##last

#define JOBCONTROL_NAMESPACE_CLASS( Type )                    \
namespace glite { namespace wms { namespace jobsubmission {   \
  class Type;                                                 \
}}}

#define JOBCONTROL_SUBNAMESPACE_CLASS( Namespace, Type )      \
namespace glite { namespace wms { namespace jobsubmission {   \
  namespace Namespace {                                       \
    class Type;                                               \
}}}}


//Temporary definitions
#define BUILD_USERNAME "user"
#define BUILD_HOSTNAME "test"

#endif /* EDG_WORKLOAD_JOBCONTROL_NAMESPACE_H_LOADED */

// Local Variables:
// mode: c++
// End:
