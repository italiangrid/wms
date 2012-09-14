// File: lb_file.h
// Authors: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//          Marco Cecchi <marco.cecchi@cnaf.infn.it>
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

#ifndef GLITE_WMS_COMMON_UTILITIES_LB_FILE_H
#define GLITE_WMS_COMMON_UTILITIES_LB_FILE_H

#include "lb.h"
#include <iostream>
#include <fstream>
#include <boost/thread/mutex.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace glite {

namespace wmsutils {
namespace jobid {

class JobId;

}}

namespace wms {
namespace common {
namespace utilities {

class LB_file: public LB
{
private:
  boost::mutex::mutex f_mx;
  std::ofstream m_log_file;

  bool write(std::ostream& message) {
    boost::gregorian::date datetime(boost::gregorian::day_clock::local_day());

     boost::mutex::scoped_lock l(f_mx);
     return m_log_file << datetime << ": " << message << '\n';
  };
public:
  LB_file(
    edg_wll_Source const& source,
    std::string const& instance,
    boost::function<bool()> termination,
    std::string const& log_filename
  ) 
    : LB(source, instance, termination) {
    m_log_file.open(log_filename.c_str());
  }
  virtual ~LB_file() {
    m_log_file.flush();
  }

  virtual void set_context(
    Context const& context,
    wmsutils::jobid::JobId const& id,
    std::string const& x509_proxy,
    std::string const& sequence_code
  ) { }

  virtual void log(Context const& ctx, Abort const& e) { write(std::iostream()); }
  virtual void log(Context const& ctx, Accepted const& e) { }
  virtual void log(Context const& ctx, CancelAbort const& e) { }
  virtual void log(Context const& ctx, Cancel const& e) { }
  virtual void log(Context const& ctx, CancelDone const& e) { }
  virtual void log(Context const& ctx, CancelRefuse const& e) { }
  virtual void log(Context const& ctx, CancelRequest const& e) { }
  virtual void log(Context const& ctx, ChangeACL const& e) { }
  virtual void log(Context const& ctx, Checkpoint const& e) { }
  virtual void log(Context const& ctx, Clear const& e) { }
  virtual void log(Context const& ctx, ClearNoOutput const& e) { }
  virtual void log(Context const& ctx, ClearTimeout const& e) { }
  virtual void log(Context const& ctx, ClearUser const& e) { }
  virtual void log(Context const& ctx, CurrentDescription const& e) { }
  virtual void log(Context const& ctx, DeepResubmission const& e) { }
  virtual void log(Context const& ctx, Dequeued const& e) { }
  virtual void log(Context const& ctx, DoneCancelled const& e) { }
  virtual void log(Context const& ctx, DoneFailed const& e) { }
  virtual void log(Context const& ctx, DoneOk const& e) { }
  virtual void log(Context const& ctx, Enqueued const& e) { }
  virtual void log(Context const& ctx, EnqueuedFailed const& e) { }
  virtual void log(Context const& ctx, EnqueuedOk const& e) { }
  virtual void log(Context const& ctx, EnqueuedRefused const& e) { }
  virtual void log(Context const& ctx, EnqueuedStart const& e) { }
  virtual void log(Context const& ctx, HelperCall const& e) { }
  virtual void log(Context const& ctx, HelperReturn const& e) { }
  virtual void log(Context const& ctx, Listener const& e) { }
  virtual void log(Context const& ctx, Match const& e) { }
  virtual void log(Context const& ctx, NoResubmission const& e) { }
  virtual void log(Context const& ctx, Notification const& e) { }
  virtual void log(Context const& ctx, Pending const& e) { }
  virtual void log(Context const& ctx, Purge const& e) { }
  virtual void log(Context const& ctx, ReallyRunning const& e) { }
  virtual void log(Context const& ctx, Refused const& e) { }
  virtual void log(Context const& ctx, RegisterDAGJob const& e) { }
  virtual void log(Context const& ctx, RegisterJob const& e) { }
  virtual void log(Context const& ctx, RegisterPartitionableJob const& e) { }
  virtual void log(Context const& ctx, RegisterPartitionedJob const& e) { }
  virtual void log(Context const& ctx, RegisterSimpleJob const& e) { }
  virtual void log(Context const& ctx, ResourceUsage const& e) { }
  virtual void log(Context const& ctx, Resume const& e) { }
  virtual void log(Context const& ctx, Running const& e) { }
  virtual void log(Context const& ctx, ShallowResubmission const& e) { }
  virtual void log(Context const& ctx, Suspend const& e) { }
  virtual void log(Context const& ctx, Transfer const& e) { }
  virtual void log(Context const& ctx, TransferAccepted const& e) { }
  virtual void log(Context const& ctx, TransferFailed const& e) { }
  virtual void log(Context const& ctx, TransferOk const& e) { }
  virtual void log(Context const& ctx, TransferRefused const& e) { }
  virtual void log(Context const& ctx, TransferStart const& e) { }
  virtual void log(Context const& ctx, UserTag const& e) { }

  virtual bool is_retryable(int error) { return true; }
  virtual bool flush(Context const& ctx) { return true; }
};

}}}}
#endif
