// File: lb_proxy.cpp
// Authors: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//          Marco Cecchi <marco.cecchi@cnaf.infn.it>
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <sys/errno.h>

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"

#include "lb_proxy.h"
#include "lb_utils.h"

namespace glite {
namespace wms {
namespace common {
namespace utilities {

LB_proxy::LB_proxy(
  edg_wll_Source const& source,
  std::string const& instance,
  boost::function<bool()> termination
)
  : LB(source, instance, termination)
{
}

void
LB_proxy::do_log(
  boost::function<int(edg_wll_Context)> log,
  Context const& context,
  std::string const& function_name
)
{
  while (!closed() && !termination()()) {
    int error = log(context.c_context());
    if (!error) {
      return; // successful
    }
    std::string message(
      format_log_message(function_name, context)
    );
    if (is_retryable(error)) {
      Warning(message << " retrying in " << five_seconds << " seconds");
      sleep_while(five_seconds, this->termination());
    } else {
      Error(message << " LB is unavailable, giving up");
      close();
      throw LB_Unavailable();
    }
  }
}

void
LB_proxy::set_logging_job(
  edg_wll_Context context,
  wmsutils::jobid::JobId const& id,
  std::string const& x509_proxy,
  std::string const& sequence_code    
)
{
  std::string const user_dn = get_proxy_subject(x509_proxy);
  int const flag = EDG_WLL_SEQ_NORMAL;

  while (!closed() && !termination()) {

    int const result = edg_wll_SetLoggingJobProxy(
      context,
      id,
      sequence_code.c_str(),
      user_dn.c_str(),
      flag
    );

    if (!result) {
      return;
    }

    std::string message("edg_wll_SetLoggingJobProxy failed:");
    if (is_retryable(result)) {
      Warning(message << " retrying in " << five_seconds << " seconds");
      sleep_while(five_seconds, this->termination());
    } else {
      Error(message << " LB is unavailable, giving up");
      close();
      throw LB_Unavailable();
    }
  }

  if(!closed()) {
    close();
    throw LB_Unavailable();
  }
}

void LB_proxy::set_context(
  Context const& context,
  wmsutils::jobid::JobId const& id,
  std::string const& x509_proxy,
  std::string const& sequence_code
)
{
  edg_wll_Context c_context = context.c_context();

  int errcode = edg_wll_SetParam(
    c_context,
    EDG_WLL_PARAM_SOURCE,
    source()
  );
  errcode |= edg_wll_SetParam(
    c_context,
    EDG_WLL_PARAM_INSTANCE,
    instance().c_str()
  );
  errcode |= edg_wll_SetParam(
    c_context,
    EDG_WLL_PARAM_X509_PROXY,
    x509_proxy.c_str()
  );
  if (errcode) {
    throw BadContext(errcode);
  }

  set_logging_job(c_context, id, x509_proxy, sequence_code);
}

void LB_proxy::log(Context const& ctx, Abort const& e)
{
  do_log(
    boost::bind(edg_wll_LogAbortProxy, _1, e.reason().c_str()),
    ctx,
    "edg_wll_LogAbortProxy"
  );
}

void LB_proxy::log(Context const& ctx, Accepted const& e)
{
  do_log(
    boost::bind(edg_wll_LogAcceptedProxy,
      _1,
      source(),
      e.from_host().c_str(),
      e.from_instance().c_str(),
      e.jobid().c_str()
    ),
    ctx,
    "edg_wll_LogAcceptedProxy"
  );
}

void LB_proxy::log(Context const& ctx, Cancel const& e)
{
  do_log(
    boost::bind(edg_wll_LogCancelProxy, _1, e.status().c_str(), e.reason().c_str()),
    ctx,
    "edg_wll_LogCancelProxy"
  );
}

void LB_proxy::log(Context const& ctx, CancelAbort const& e)
{
  do_log(
    boost::bind(edg_wll_LogCancelABORTProxy, _1, e.reason().c_str()),
    ctx,
    "edg_wll_LogCancelABORTProxy"
  );
}

void LB_proxy::log(Context const& ctx, CancelDone const& e)
{
  do_log(
    boost::bind(edg_wll_LogCancelDONEProxy, _1, e.reason().c_str()),
    ctx,
    "edg_wll_LogCancelDONEProxy"
  );
}

void LB_proxy::log(Context const& ctx, CancelRefuse const& e)
{
  do_log(
    boost::bind(edg_wll_LogCancelREFUSEProxy, _1, e.reason().c_str()),
    ctx,
    "edg_wll_LogCancelREFUSEProxy"
  );
}

void LB_proxy::log(Context const& ctx, CancelRequest const& e)
{
  do_log(
    boost::bind(edg_wll_LogCancelREQProxy, _1, e.reason().c_str()),
    ctx,
    "edg_wll_LogCancelREQProxy"
  );
}

void LB_proxy::log(Context const& ctx, Dequeued const& e)
{
  do_log(
    boost::bind(edg_wll_LogDeQueuedProxy, _1, e.queue().c_str(), e.jobid().c_str()),
    ctx,
    "edg_wll_LogDeQueuedProxy"
  );
}

void LB_proxy::log(Context const& ctx, Pending const& e)
{
  do_log(
    boost::bind(edg_wll_LogPendingProxy, _1, e.reason().c_str()),
    ctx,
    "edg_wll_LogPendingProxy"
  );
}

void LB_proxy::log(Context const& ctx, ShallowResubmission const& e)
{
  do_log(
    boost::bind(
      edg_wll_LogResubmissionSHALLOWProxy,
      _1,
      e.reason().c_str(),
      e.token_file().c_str()
    ),
    ctx,
    "edg_wll_LogResubmissionSHALLOWProxy"
  );
}

void LB_proxy::log(Context const& ctx, DeepResubmission const& e)
{
  do_log(
    boost::bind(edg_wll_LogResubmissionWILLRESUBProxy,
      _1,
      e.reason().c_str(),
      e.tag().c_str()
    ),
    ctx,
    "edg_wll_LogResubmissionWILLRESUBProxy"
  );
}

void LB_proxy::log(Context const& ctx, Match const& e)
{
  do_log(
    boost::bind(edg_wll_LogMatchProxy, _1, e.destination().c_str()),
    ctx,
    "edg_wll_LogMatchProxy"
  );
}

void LB_proxy::log(Context const& ctx, EnqueuedStart const& e)
{
  do_log(
    boost::bind(edg_wll_LogEnQueuedSTARTProxy,
      _1,
      e.queue().c_str(),
      e.job().c_str(),
      e.reason().c_str()
    ),
    ctx,
    "edg_wll_LogEnQueuedSTARTProxy"
  );
}

void LB_proxy::log(Context const& ctx, EnqueuedOk const& e)
{
  do_log(
    boost::bind(edg_wll_LogEnQueuedOKProxy,
      _1,
      e.queue().c_str(),
      e.job().c_str(),
      e.reason().c_str()
    ),
    ctx,
    "edg_wll_LogEnQueuedOKProxy"
  );
}

void LB_proxy::log(Context const& ctx, EnqueuedFailed const& e)
{
  do_log(
    boost::bind(edg_wll_LogEnQueuedFAILProxy,
      _1,
      e.queue().c_str(),
      e.job().c_str(),
      e.reason().c_str()
    ),
    ctx,
    "edg_wll_LogEnQueuedFAILProxy"
  );
}

void LB_proxy::log(Context const& ctx, HelperCall const& e)
{
  do_log(
    boost::bind(edg_wll_LogHelperCallCALLEDProxy,
      _1,
      e.name().c_str(),
      e.args().c_str()
    ),
    ctx,
    "edg_wll_LogHelperCallCALLEDProxy"
  );
}

void LB_proxy::log(Context const& ctx, HelperReturn const& e)
{
  do_log(
    boost::bind(edg_wll_LogHelperReturnCALLEDProxy,
      _1,
      e.name().c_str(),
      e.ret_val().c_str()
    ),
    ctx,
    "edg_wll_LogHelperReturnCALLEDProxy"
  );
}

void LB_proxy::log(Context const& ctx, ChangeACL const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, Checkpoint const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, Clear const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, ClearNoOutput const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, ClearUser const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, CurrentDescription const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, DoneCancelled const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, DoneFailed const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, DoneOk const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, Enqueued const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, EnqueuedRefused const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, ClearTimeout const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, Listener const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, NoResubmission const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, Notification const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, Refused const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, Purge const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, ReallyRunning const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, RegisterDAGJob const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, RegisterJob const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, RegisterPartitionableJob const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, RegisterPartitionedJob const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, RegisterSimpleJob const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, ResourceUsage const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, Resume const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, Running const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, Suspend const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, Transfer const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, TransferAccepted const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, TransferFailed const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, TransferOk const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, TransferRefused const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, TransferStart const& e)
{
  // TODO
}

void LB_proxy::log(Context const& ctx, UserTag const& e)
{
  // TODO
}

bool LB_proxy::is_retryable(int error)
{
  return error == ETIMEDOUT
    || error == ENOTCONN
    || error == ECONNREFUSED
    || error == EAGAIN;
}

bool LB_proxy::flush(Context const& context)
{
  struct timeval* timeout = 0;
  return edg_wll_LogFlush(context.c_context(), timeout);
}

}}}}
