// File: lb_server.cpp
// Authors: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//          Marco Cecchi <marco.cecchi@cnaf.infn.it>
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

#include "lb_server.h"
#include <boost/bind.hpp>
#include <openssl/pem.h>
#include <openssl/x509.h>

#include <sys/errno.h>

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"

namespace glite {
namespace wms {
namespace common {
namespace utilities {

namespace {

unsigned int const five_seconds = 5;

std::string
get_proxy_subject(std::string const& x509_proxy)
{
  static std::string const null_string;

  std::FILE* fd = std::fopen(x509_proxy.c_str(), "r");
  if (!fd) return null_string;
  boost::shared_ptr<std::FILE> fd_(fd, std::fclose);

  ::X509* const cert = ::PEM_read_X509(fd, 0, 0, 0);
  if (!cert) return null_string;
  boost::shared_ptr< ::X509> cert_(cert, ::X509_free);

  char* const s = ::X509_NAME_oneline(::X509_get_subject_name(cert), 0, 0);
  if (!s) return null_string;
  boost::shared_ptr<char> s_(s, ::free);

  return std::string(s);
}

void sleep_while(unsigned int seconds, boost::function<bool()> condition)
{
  for (unsigned int i = 0; i < seconds && condition(); ++i) {
    ::sleep(1);
  }
}

} // anonymous

LB_server::LB_server(
  edg_wll_Source const& source,
  std::string const& instance,
  boost::function<bool()> termination
)
  : LB(source, instance, termination)
{
}

void
LB_server::do_log(
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
      // TODO format_log_message(function_name, context)
    );
    if (is_retryable(error)) {
      // TODO Warning(message << " retrying in " << five_seconds << " seconds");
      sleep_while(five_seconds, this->termination());
    } else {
      // TODO Error(message << " LB is unavailable, giving up");
      close();
      throw LB_Unavailable();
    }
  }
}

void
LB_server::set_logging_job(
  edg_wll_Context context,
  wmsutils::jobid::JobId const& id,
  std::string const& sequence_code    
)
{
  int const flag = EDG_WLL_SEQ_NORMAL;

  while (!closed() && !termination()) {
    
    int const result = edg_wll_SetLoggingJob(
      context,
      id,
      sequence_code.c_str(),
      flag
    );

    if (!result) {
      return;
    }

    std::string message("edg_wll_SetLoggingJob failed:");
    if (is_retryable(result)) {
      Warning(message << " retrying in " << five_seconds << " seconds");
      sleep_while(five_seconds, this->termination());
    } else {
      Error(message << " LB is unavailable, giving up");
      close();
      throw LB_Unavailable();
      assert(false);
    }
  }

  if(!closed()) {
    close();
    throw LB_Unavailable();
  }
}

void LB_server::set_context(
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

  set_logging_job(c_context, id, sequence_code.c_str());
  if (!errcode) {
    throw BadContext(errcode);
  }
}

void LB_server::log(Context const& ctx, Abort const& e)
{
  do_log(
    boost::bind(edg_wll_LogAbort, _1, e.reason().c_str()),
    ctx,
    "edg_wll_LogAbort"
  );
}

void LB_server::log(Context const& ctx, Accepted const& e)
{
  do_log(
    boost::bind(edg_wll_LogAccepted,
      _1,
      source(),
      e.from_host().c_str(),
      e.from_instance().c_str(),
      e.jobid().c_str()
    ),
    ctx,
    "edg_wll_LogAccepted"
  );
}

void LB_server::log(Context const& ctx, Cancel const& e)
{
  do_log(
    boost::bind(edg_wll_LogCancel, _1, e.status().c_str(), e.reason().c_str()),
    ctx,
    "edg_wll_LogCancel"
  );
}

void LB_server::log(Context const& ctx, CancelAbort const& e)
{
  do_log(
    boost::bind(edg_wll_LogCancelABORT, _1, e.reason().c_str()),
    ctx,
    "edg_wll_LogCancelABORT"
  );
}

void LB_server::log(Context const& ctx, CancelDone const& e)
{
  do_log(
    boost::bind(edg_wll_LogCancelDONE, _1, e.reason().c_str()),
    ctx,
    "edg_wll_LogCancelDONE"
  );
}

void LB_server::log(Context const& ctx, CancelRefuse const& e)
{
  do_log(
    boost::bind(edg_wll_LogCancelREFUSE, _1, e.reason().c_str()),
    ctx,
    "edg_wll_LogCancelREFUSE"
  );
}

void LB_server::log(Context const& ctx, CancelRequest const& e)
{
  do_log(
    boost::bind(edg_wll_LogCancelREQ, _1, e.reason().c_str()),
    ctx,
    "edg_wll_LogCancelREQ"
  );
}

void LB_server::log(Context const& ctx, Dequeued const& e)
{
  do_log(
    boost::bind(edg_wll_LogDeQueued, _1, e.queue().c_str(), e.jobid().c_str()),
    ctx,
    "edg_wll_LogDeQueued"
  );
}

void LB_server::log(Context const& ctx, Pending const& e)
{
  do_log(
    boost::bind(edg_wll_LogPending, _1, e.reason().c_str()),
    ctx,
    "edg_wll_LogPending"
  );
}

void LB_server::log(Context const& ctx, ShallowResubmission const& e)
{
  do_log(
    boost::bind(
      edg_wll_LogResubmissionSHALLOW,
      _1,
      e.reason().c_str(),
      e.token_file().c_str()
    ),
    ctx,
    "edg_wll_LogResubmissionSHALLOW"
  );
}

void LB_server::log(Context const& ctx, DeepResubmission const& e)
{
  do_log(
    boost::bind(edg_wll_LogResubmissionWILLRESUB,
      _1,
      e.reason().c_str(),
      e.tag().c_str()
    ),
    ctx,
    "edg_wll_LogResubmissionWILLRESUB"
  );
}

void LB_server::log(Context const& ctx, Match const& e)
{
  do_log(
    boost::bind(edg_wll_LogMatch, _1, e.destination().c_str()),
    ctx,
    "edg_wll_LogMatch"
  );
}

void LB_server::log(Context const& ctx, EnqueuedStart const& e)
{
  do_log(
    boost::bind(edg_wll_LogEnQueuedSTART,
      _1,
      e.queue().c_str(),
      e.job().c_str(),
      e.reason().c_str()
    ),
    ctx,
    "edg_wll_LogEnQueuedSTART"
  );
}

void LB_server::log(Context const& ctx, EnqueuedOk const& e)
{
  do_log(
    boost::bind(edg_wll_LogEnQueuedOK,
      _1,
      e.queue().c_str(),
      e.job().c_str(),
      e.reason().c_str()
    ),
    ctx,
    "edg_wll_LogEnQueuedOK"
  );
}

void LB_server::log(Context const& ctx, EnqueuedFailed const& e)
{
  do_log(
    boost::bind(edg_wll_LogEnQueuedFAIL,
      _1,
      e.queue().c_str(),
      e.job().c_str(),
      e.reason().c_str()
    ),
    ctx,
    "edg_wll_LogEnQueuedFAIL"
  );
}

void LB_server::log(Context const& ctx, HelperCall const& e)
{
  do_log(
    boost::bind(edg_wll_LogHelperCallCALLED,
      _1,
      e.name().c_str(),
      e.args().c_str()
    ),
    ctx,
    "edg_wll_LogHelperCallCALLED"
  );
}

void LB_server::log(Context const& ctx, HelperReturn const& e)
{
  do_log(
    boost::bind(edg_wll_LogHelperReturnCALLED,
      _1,
      e.name().c_str(),
      e.ret_val().c_str()
    ),
    ctx,
    "edg_wll_LogHelperReturnCALLED"
  );
}

void LB_server::log(Context const& ctx, ChangeACL const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, Checkpoint const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, Clear const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, ClearNoOutput const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, ClearUser const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, CurrentDescription const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, DoneCancelled const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, DoneFailed const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, DoneOk const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, Enqueued const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, EnqueuedRefused const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, ClearTimeout const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, Listener const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, NoResubmission const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, Notification const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, Refused const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, Purge const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, ReallyRunning const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, RegisterDAGJob const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, RegisterJob const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, RegisterPartitionableJob const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, RegisterPartitionedJob const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, RegisterSimpleJob const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, ResourceUsage const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, Resume const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, Running const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, Suspend const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, Transfer const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, TransferAccepted const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, TransferFailed const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, TransferOk const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, TransferRefused const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, TransferStart const& e)
{
  // TODO
}

void LB_server::log(Context const& ctx, UserTag const& e)
{
  // TODO
}

bool LB_server::is_retryable(int error)
{
  return error == ETIMEDOUT
    || error == ENOTCONN
    || error == ECONNREFUSED
    || error == EAGAIN;
}

bool LB_server::flush(Context const& context)
{
  struct timeval* timeout = 0;
  return edg_wll_LogFlush(context.c_context(), timeout);
}

}}}}
