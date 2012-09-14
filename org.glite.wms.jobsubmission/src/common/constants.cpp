#include "jobcontrol_namespace.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

const char   *generic_events_string[] = {
  "Null event",
  "Job cancelled from queue",
  "Force remove job",
  "Cannot cancel job from queue",
  "Job cancelled by the user"
};

} /* End of jccommon namespace */

} JOBCONTROL_NAMESPACE_END;
