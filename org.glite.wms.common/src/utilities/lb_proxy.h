// File: lb_proxy.h
// Authors: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//          Marco Cecchi <marco.cecchi@cnaf.infn.it>
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

#ifndef GLITE_WMS_COMMON_UTILITIES_LB_PROXY_H
#define GLITE_WMS_COMMON_UTILITIES_LB_PROXY_H

#include "lb.h"

namespace glite {

namespace wmsutils {
namespace jobid {

class JobId;

}}

namespace wms {
namespace common {
namespace utilities {

class LB_proxy : public LB
{
  void do_log(
    boost::function<int(edg_wll_Context)> log,
    Context const& context,
    std::string const& function_name
  );
  void set_logging_job(
    edg_wll_Context context,
    wmsutils::jobid::JobId const& id,
    std::string const& x509_proxy,
    std::string const& sequence_code    
  );

public:
  LB_proxy(
    edg_wll_Source const& source,
    std::string const& instance,
    boost::function<bool()> termination
  );
  virtual ~LB_proxy() { }

  virtual void set_context(
    Context const& context,
    wmsutils::jobid::JobId const& id,
    std::string const& x509_proxy,
    std::string const& sequence_code
  );

  virtual void log(Context const& ctx, Abort const& e);
  virtual void log(Context const& ctx, Accepted const& e);
  virtual void log(Context const& ctx, Cancel const& e);
  virtual void log(Context const& ctx, CancelAbort const& e);
  virtual void log(Context const& ctx, CancelDone const& e);
  virtual void log(Context const& ctx, CancelRefuse const& e);
  virtual void log(Context const& ctx, CancelRequest const& e);
  virtual void log(Context const& ctx, ChangeACL const& e);
  virtual void log(Context const& ctx, Checkpoint const& e);
  virtual void log(Context const& ctx, Clear const& e);
  virtual void log(Context const& ctx, ClearNoOutput const& e);
  virtual void log(Context const& ctx, ClearTimeout const& e);
  virtual void log(Context const& ctx, ClearUser const& e);
  virtual void log(Context const& ctx, CurrentDescription const& e);
  virtual void log(Context const& ctx, DeepResubmission const& e);
  virtual void log(Context const& ctx, Dequeued const& e);
  virtual void log(Context const& ctx, DoneCancelled const& e);
  virtual void log(Context const& ctx, DoneFailed const& e);
  virtual void log(Context const& ctx, DoneOk const& e);
  virtual void log(Context const& ctx, Enqueued const& e);
  virtual void log(Context const& ctx, EnqueuedFailed const& e);
  virtual void log(Context const& ctx, EnqueuedOk const& e);
  virtual void log(Context const& ctx, EnqueuedRefused const& e);
  virtual void log(Context const& ctx, EnqueuedStart const& e);
  virtual void log(Context const& ctx, HelperCall const& e);
  virtual void log(Context const& ctx, HelperReturn const& e);
  virtual void log(Context const& ctx, Listener const& e);
  virtual void log(Context const& ctx, Match const& e);
  virtual void log(Context const& ctx, NoResubmission const& e);
  virtual void log(Context const& ctx, Notification const& e);
  virtual void log(Context const& ctx, Pending const& e);
  virtual void log(Context const& ctx, Purge const& e);
  virtual void log(Context const& ctx, ReallyRunning const& e);
  virtual void log(Context const& ctx, Refused const& e);
  virtual void log(Context const& ctx, RegisterDAGJob const& e);
  virtual void log(Context const& ctx, RegisterJob const& e);
  virtual void log(Context const& ctx, RegisterPartitionableJob const& e);
  virtual void log(Context const& ctx, RegisterPartitionedJob const& e);
  virtual void log(Context const& ctx, RegisterSimpleJob const& e);
  virtual void log(Context const& ctx, ResourceUsage const& e);
  virtual void log(Context const& ctx, Resume const& e);
  virtual void log(Context const& ctx, Running const& e);
  virtual void log(Context const& ctx, ShallowResubmission const& e);
  virtual void log(Context const& ctx, Suspend const& e);
  virtual void log(Context const& ctx, Transfer const& e);
  virtual void log(Context const& ctx, TransferAccepted const& e);
  virtual void log(Context const& ctx, TransferFailed const& e);
  virtual void log(Context const& ctx, TransferOk const& e);
  virtual void log(Context const& ctx, TransferRefused const& e);
  virtual void log(Context const& ctx, TransferStart const& e);
  virtual void log(Context const& ctx, UserTag const& e);

  virtual bool is_retryable(int error);
  virtual bool flush(Context const& ctx);
};

}}}}
#endif
