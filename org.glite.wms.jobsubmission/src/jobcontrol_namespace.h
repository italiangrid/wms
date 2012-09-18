/* Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
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
