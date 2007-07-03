#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERFAKE_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERFAKE_H

#include "jobcontrol_namespace.h"
#include "JobControllerImpl.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon { class RamContainer; }

namespace controller {

class JobControllerFake : public JobControllerImpl {
public:
  JobControllerFake( void );
  virtual ~JobControllerFake( void );

  virtual int submit( const classad::ClassAd *ad );
  virtual bool cancel( const glite::wmsutils::jobid::JobId &id, const char *logfile );
  virtual bool cancel( int condorid, const char *logfile );
  virtual size_t queue_size( void );

private:
  JobControllerFake( const JobControllerFake & ); // Not implemented
  JobControllerFake &operator=( const JobControllerFake & ); // Not implemented
};

}; // namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERFAKE_H */

// Local Variables:
// mode: c++
// End:
