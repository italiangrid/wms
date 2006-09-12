#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERREAL_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERREAL_H

// File: JobControllerReal.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//         Rosario Peluso <Rosario.Peluso@pd.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <memory>

#include "../common/EventLogger.h"
#include "JobControllerImpl.h"

typedef  struct _edg_wll_Context  *edg_wll_Context;

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon { class RamContainer; }

namespace controller {

class JobControllerReal: public JobControllerImpl {
public:
  JobControllerReal( edg_wll_Context *cont );
  virtual ~JobControllerReal( void );

  virtual int submit( const classad::ClassAd *ad );
  virtual bool cancel( const glite::wmsutils::jobid::JobId &id, const char *logfile );
  virtual bool cancel( int condorid, const char *logfile );
  virtual size_t queue_size( void );

private:
  JobControllerReal( const JobControllerReal &rhs );
  JobControllerReal &operator=( const JobControllerReal &rhs );

  void readRepository( void );

  int                                      jcr_threshold;
  std::auto_ptr<jccommon::RamContainer>    jcr_repository;
  jccommon::EventLogger                    jcr_logger;

  static const int        jcr_s_threshold = 10;
};

}; // namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERREAL_H */

// Local Variables:
// mode: c++
// End:
