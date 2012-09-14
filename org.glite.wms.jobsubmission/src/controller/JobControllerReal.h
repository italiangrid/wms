#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERREAL_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERREAL_H

// File: JobControllerReal.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//         Rosario Peluso <Rosario.Peluso@pd.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <memory>

#include "common/RamContainer.h"
#include "common/EventLogger.h"
#include "JobControllerImpl.h"

namespace jccommon { class EventLogger; }

JOBCONTROL_NAMESPACE_BEGIN {
namespace controller {

class JobControllerReal: public JobControllerImpl {
public:
  JobControllerReal(boost::shared_ptr<jccommon::EventLogger> ctx);
  virtual ~JobControllerReal() { }

  virtual int msubmit(std::vector<classad::ClassAd*>&);
  virtual int submit(classad::ClassAd *ad);
  virtual bool cancel(const glite::wmsutils::jobid::JobId &id, const char *logfile );
  virtual bool cancel(int condorid, const char *logfile );

  static const int jcr_s_threshold = 25;
private:
  JobControllerReal(jccommon::EventLogger& rhs);

  void readRepository();

  int jcr_threshold;
  boost::shared_ptr<jccommon::RamContainer> jcr_repository;
  boost::shared_ptr<jccommon::EventLogger> jcr_logger;
};

}; // namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERREAL_H */

// Local Variables:
// mode: c++
// End:
