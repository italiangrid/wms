#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERIMPL_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERIMPL_H

// File: JobControllerImpl.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//         Rosario Peluso <Rosario.Peluso@pd.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <glite/wmsutils/jobid/JobId.h>

COMMON_SUBNAMESPACE_CLASS_J(jobid, JobId );

namespace classad { class ClassAd; }

JOBCONTROL_NAMESPACE_BEGIN {

/**
 *  Controller namespace.
 *  This namespace contains the objects and the functions used
 *  for the interface between the WM and the JC and between the
 *  JC and CondorG.
 */
namespace controller {

/**
 *  Base JobController class.
 *  Pure virtual class defining a generic interface to the
 *  JobController facilities.
 */
class JobControllerImpl {
public:
  /**
   *  Empty constructor.
   */
  JobControllerImpl( void ) {}
  /**
   *  Empty virtual destructor.
   */
  virtual ~JobControllerImpl( void ) {}

  /**
   *  Submit a job.
   *  This method allows the user to submit a job to the controller or
   *  from the controller to CondorG.
   *  \param ad The ClassAd containing all the informations about the job.
   *  \return Implementation dependent integer. Don't cope on it.
   */
  virtual int submit( const classad::ClassAd *ad ) = 0;
  /**
   *  Remove a job.
   *  Remove a job from the queue using its JOB ID.
   *  \param id The JOB ID of the job.
   *  \param logfile The file where condor is logging for such job.
   *  \return \a true if everything went well, \a false otherwise
   */
  virtual bool cancel( const glite::wmsutils::jobid::JobId &id, const char *logfile ) = 0;
  /**
   *  Remove a job.
   *  Remove a job from the queue using its condor ID.
   *  \param id The Condor ID of the job.
   *  \param logfile The file where condor is logging for such job.
   *  \return Implementation dependent integer. Don't cope on it.
   */
  virtual bool cancel( int condorid, const char *logfile ) = 0;
  /**
   *  Ask for the queue size.
   *  \return The number of request still in the queue.
   */
  virtual size_t queue_size( void ) = 0;

private:
  JobControllerImpl( const JobControllerImpl &rhs ); // Not implemented
  JobControllerImpl &operator=( const JobControllerImpl &rhs ); // Not implemented
};

}; // namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERIMPL_H */

// Local Variables:
// mode: c++
// End:
