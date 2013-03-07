// File: lb.h
// Authors: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//          Marco Cecchi <marco.cecchi@cnaf.infn.it>
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

#ifndef GLITE_WMS_COMMON_UTILITIES_LB_H
#define GLITE_WMS_COMMON_UTILITIES_LB_H

#include <string>
#include <sys/errno.h>

#include "glite/wms/common/logger/logstream_ts.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/logger_utils.h"

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>

#include "glite/lb/producer.h"
#include "glite/lb/context.h"
#include "glite/lb/consumer.h"

namespace glite {

namespace wmsutils {
namespace jobid {

class JobId;

}}

namespace wms {
namespace common {
namespace utilities {

class LB_Unavailable: public std::exception { };

class BadContext: public std::exception
{
  int m_error;
public:
  BadContext(int e)
    : m_error(e) { }
  virtual ~BadContext() throw() { };
  virtual char const* what() const throw()
  {
    return std::string("Cannot get context (error"
      + boost::lexical_cast<std::string>(m_error)
      + ")").c_str();
  }
};

class Abort {
  std::string reason_;
public:
  Abort(std::string const& reason) : reason_(reason) { }
  std::string reason() const { return reason_; }
};

class Accepted
{
  std::string from_host_;
  std::string from_instance_;
  std::string jobid_;
public:
  Accepted(
    std::string const& from_host,
    std::string const& from_instance,
    std::string const& jobid
  )
    : from_host_(from_host), from_instance_(from_instance), jobid_(jobid) { }

  std::string from_host() const { return from_host_; }
  std::string from_instance() const { return from_instance_; }
  std::string jobid() const { return jobid_; }
};

class Cancel
{ 
  std::string status_;
  std::string reason_;
public:
  Cancel(std::string const& reason, std::string const& status)
    : status_(status), reason_(reason){ }

  std::string status() const { return status_; }
  std::string reason() const { return reason_; }
};

class CancelAbort
{
  std::string reason_;
public:
  CancelAbort(std::string const& reason)
    : reason_(reason) { }

  std::string reason() const { return reason_; }
};

class CancelDone
{
  std::string reason_;
public:
  CancelDone(std::string const& reason)
    : reason_(reason) { }

  std::string reason() const { return reason_; }
};

class CancelRefuse
{
  std::string reason_;
public:
  CancelRefuse(std::string const& reason)
    : reason_(reason) { }

  std::string reason() const { return reason_; }
};

class CancelRequest
{
  std::string reason_;
public:
  CancelRequest(std::string const& reason)
    : reason_(reason) { }

  std::string reason() const { return reason_; }
};

class ChangeACL
{
  // TODO edg_wll_LogChangeACL(edg_wll_Context context,
  //   const char * user_id,
  //   const int user_id_type,
  //   const int permission,
  //   const int permission_type,
  //   const int operation);
public:
};

class Checkpoint
{
  // TODO edg_wll_LogChkpt(edg_wll_Context context,
  //   const char * tag,
  //   const char * classad);
public:
};

class Clear
{
  std::string reason_;
public:
  Clear(std::string const& reason)
    : reason_(reason) { }

  std::string reason() const { return reason_; }
};

class ClearNoOutput { };

class ClearUser { };

class CurrentDescription
{
  std::string description_;
public:
  CurrentDescription(std::string const& description)
    : description_(description) { }

  std::string description() const { return description_; }
};

class DeepResubmission
{
  std::string reason_; // kind of "shallow resubmission is disabled"
  std::string tag_; // oftentimes empty
public:
  DeepResubmission(std::string const& reason, std::string const& tag)
    : reason_(reason), tag_(tag) { }

  std::string reason() const { return reason_; }
  std::string tag() const { return tag_; }
};

class Dequeued
{
  std::string queue_;
  std::string jobid_;
public:
  Dequeued(std::string const& queue, std::string const& jobid)
    : queue_(queue), jobid_(jobid) { }

  std::string queue() const { return queue_; }
  std::string jobid() const { return jobid_; }
};

class DoneCancelled
{
  std::string reason_;
public:
  DoneCancelled(std::string const& reason)
    : reason_(reason) { }

  std::string reason() const { return reason_; }
};

class DoneFailed
{
  std::string reason_;
  int exit_code_;
public:
  DoneFailed(std::string const& reason, int exit_code)
    : reason_(reason), exit_code_(exit_code) { }

  std::string reason() const { return reason_; }
  int exit_code() const { return exit_code_; }
};

class DoneOk
{
  std::string reason_;
public:
  DoneOk(std::string const& reason)
    : reason_(reason) { }

  std::string reason() const { return reason_; }
};

class Enqueued
{
  std::string queue_;
  std::string job_;
  std::string result_;
  std::string reason_;
public:
  Enqueued(
    std::string const& queue,
    std::string const& job,
    std::string const& result,
    std::string const& reason
  )
    : queue_(queue), job_(job), result_(result), reason_(reason) { }

  std::string queue() const { return queue_; }
  std::string job() const { return job_; }
  std::string result() const { return result_; }
  std::string reason() const { return reason_; }
};

class EnqueuedFailed
{
  std::string queue_;
  std::string job_;
  std::string reason_;
public:
  EnqueuedFailed(
    std::string const& queue,
    std::string const& job,
    std::string const& reason
  )
    : queue_(queue), job_(job), reason_(reason) { }

  std::string queue() const { return queue_; }
  std::string job() const { return job_; }
  std::string reason() const { return reason_; }
};

class EnqueuedOk
{
  std::string queue_;
  std::string job_;
  std::string reason_;
public:
  EnqueuedOk(
    std::string const& queue,
    std::string const& job,
    std::string const& reason
  )
    : queue_(queue), job_(job), reason_(reason) { }

  std::string queue() const { return queue_; }
  std::string job() const { return job_; }
  std::string reason() const { return reason_; }
};

class EnqueuedRefused
{
  std::string queue_;
  std::string job_;
  std::string reason_;
public:
  EnqueuedRefused(
    std::string const& queue,
    std::string const& job,
    std::string const& reason
  )
    : queue_(queue), job_(job), reason_(reason) { }

  std::string queue() const { return queue_; }
  std::string job() const { return job_; }
  std::string reason() const { return reason_; }
};

class EnqueuedStart
{
  std::string queue_;
  std::string job_;
  std::string reason_;
public:
  EnqueuedStart(
    std::string const& queue,
    std::string const& job,
    std::string const& reason
  )
    : queue_(queue), job_(job), reason_(reason) { }

  std::string queue() const { return queue_; }
  std::string job() const { return job_; }
  std::string reason() const { return reason_; }
};

class HelperCall
{
  std::string name_;
  std::string args_;
public:
  HelperCall(std::string const& name, std::string const& args)
    : name_(name), args_(args) { }
  std::string name() const { return name_; }
  std::string args() const { return args_; }
};

class HelperReturn
{
  std::string name_;
  std::string ret_val_;
public:
  HelperReturn(std::string const& name, std::string const& ret_val)
    : name_(name), ret_val_(ret_val) { }
  std::string name() const { return name_; }
  std::string ret_val() const { return ret_val_; }
};

class ClearTimeout { };

class Listener
{ 
  // TODO edg_wll_LogListener(edg_wll_Context context,
  //   const char * svc_name,
  //   const char * svc_host,
  //   const uint16_t svc_port);
};

class Match {
  std::string destination_;
public:
  Match(std::string const& destination)
    : destination_(destination) { }

  std::string destination() const { return destination_; }
};

class NoResubmission
{
  // TODO edg_wll_LogResubmissionWONTRESUB(edg_wll_Context context,
  //   const char * reason,
  //   const char * tag);
};

class Notification
{
  // TODO edg_wll_LogNotification(edg_wll_Context context,
  //   const edg_wll_NotifId notifId,
  //   const char * owner,
  //   const char * dest_host,
  //   const uint16_t dest_port,
  //   const char * jobstat);
};

class Pending
{
  std::string reason_;
public:
  Pending(std::string const& reason)
    : reason_(reason) { }

  std::string reason() const { return reason_; }
};

class Purge { };

class ReallyRunning
{
  std::string wn_seq_;
public:
  ReallyRunning(std::string const& wn_seq)
    : wn_seq_(wn_seq) { }

  std::string wn_seq() const { return wn_seq_; }
};

class Refused
{
  std::string from_host_;
  std::string from_instance_;
  std::string reason_;
public:
  Refused(
    std::string const& from_host,
    std::string const& from_instance,
    std::string const& reason
  )
    : from_host_(from_host), from_instance_(from_instance), reason_(reason) { }

  std::string reason() const { return reason_; }
};

class RegisterDAGJob { };

class RegisterJob { };

class RegisterPartitionableJob { };

class RegisterPartitionedJob { };

class RegisterSimpleJob { };

class ResourceUsage
{
  // TODO edg_wll_LogResourceUsage(edg_wll_Context context,
  //   const char * resource,
  //   const int quantity,
  //   const char * unit);
};

class Resume
{
  // TODO edg_wll_LogResume(edg_wll_Context context,
  //   const char * reason);
};

class Running { };

class ShallowResubmission
{
  std::string reason_;
  std::string token_file_;
public:
  ShallowResubmission(std::string const& reason, std::string const& token_file)
    : reason_(reason), token_file_(token_file) { }
  std::string reason() const { return reason_; }
  std::string token_file() const { return token_file_; }
};

class Suspend
{
  // TODO edg_wll_LogSuspend(edg_wll_Context context,
  //   const char * reason);
};

class Transfer
{
  // TODO edg_wll_LogTransfer(edg_wll_Context context,
  //   const edg_wll_Source destination,
  //   const char * dest_host,
  //   const char * dest_instance,
  //   const char * job,
  //   const char * result,
  //   const char * reason,
  //   const char * dest_jobid);
};

class TransferAccepted
{
  // TODO edg_wll_LogAccepted(edg_wll_Context context,
  //   const edg_wll_Source from,
  //   const char * from_host,
  //   const char * from_instance,
  //   const char * local_jobid);
};

class TransferFailed
{
  // TODO edg_wll_LogTransferFAIL(edg_wll_Context context,
  //   const edg_wll_Source destination,
  //   const char * dest_host,
  //   const char * dest_instance,
  //   const char * job,
  //   const char * reason,
  //   const char * dest_jobid);
};

class TransferOk
{
  // TODO edg_wll_LogTransferOK(edg_wll_Context context,
  //   const edg_wll_Source destination,
  //   const char * dest_host,
  //   const char * dest_instance,
  //   const char * job,
  //   const char * reason,
  //   const char * dest_jobid);
};

class TransferRefused
{
  // TODO edg_wll_LogRefused(edg_wll_Context context,
  //   const edg_wll_Source from,
  //   const char * from_host,
  //   const char * from_instance,
  //   const char * reason);
};

class TransferStart
{
  // TODO edg_wll_LogTransferSTART(edg_wll_Context context,
  //   const edg_wll_Source destination,
  //   const char * dest_host,
  //   const char * dest_instance,
  //   const char * job,
  //   const char * reason,
  //   const char * dest_jobid);
};

class UserTag
{
  // TODO edg_wll_LogUserTag(edg_wll_Context context,
  //   const char * name,
  //   const char * value);
};

class Context: boost::noncopyable
{
  edg_wll_Context m_context;
public:
  Context()
  {
    int errcode = edg_wll_InitContext(&m_context);
    if (errcode == ENOMEM) {
      throw std::bad_alloc();
    } else if (errcode) {
      Error("edg_wll_InitContext: unexpected error");
      throw BadContext(errcode);
    }
  }
  ~Context() { edg_wll_FreeContext(m_context); }
  edg_wll_Context c_context() const { return m_context; }
};

class LB: boost::noncopyable
{
  edg_wll_Source m_source;
  std::string m_instance;
  bool m_closed;
  boost::function<bool()> m_termination;
public:
  LB(
    edg_wll_Source const& source,
    std::string const& instance,
    boost::function<bool()> termination
  )
    : m_source(source),
      m_instance(instance),
      m_closed(false),
      m_termination(termination)
  { }
  virtual ~LB() { };

  virtual void set_context(
    Context const& context,
    wmsutils::jobid::JobId const& id,
    std::string const& x509_proxy,
    std::string const& sequence_code
  ) = 0;

  virtual void log(Context const& ctx, Abort const& e) = 0;
  virtual void log(Context const& ctx, Accepted const& e) = 0;
  virtual void log(Context const& ctx, Cancel const& e) = 0;
  virtual void log(Context const& ctx, CancelAbort const& e) = 0;
  virtual void log(Context const& ctx, CancelDone const& e) = 0;
  virtual void log(Context const& ctx, CancelRefuse const& e) = 0;
  virtual void log(Context const& ctx, CancelRequest const& e) = 0;
  virtual void log(Context const& ctx, ChangeACL const& e) = 0;
  virtual void log(Context const& ctx, Checkpoint const& e) = 0;
  virtual void log(Context const& ctx, Clear const& e) = 0;
  virtual void log(Context const& ctx, ClearNoOutput const& e) = 0;
  virtual void log(Context const& ctx, ClearTimeout const& e) = 0;
  virtual void log(Context const& ctx, ClearUser const& e) = 0;
  virtual void log(Context const& ctx, CurrentDescription const& e) = 0;
  virtual void log(Context const& ctx, DeepResubmission const& e) = 0;
  virtual void log(Context const& ctx, Dequeued const& e) = 0;
  virtual void log(Context const& ctx, DoneCancelled const& e) = 0;
  virtual void log(Context const& ctx, DoneFailed const& e) = 0;
  virtual void log(Context const& ctx, DoneOk const& e) = 0;
  virtual void log(Context const& ctx, Enqueued const& e) = 0;
  virtual void log(Context const& ctx, EnqueuedFailed const& e) = 0;
  virtual void log(Context const& ctx, EnqueuedOk const& e) = 0;
  virtual void log(Context const& ctx, EnqueuedRefused const& e) = 0;
  virtual void log(Context const& ctx, EnqueuedStart const& e) = 0;
  virtual void log(Context const& ctx, HelperCall const& e) = 0;
  virtual void log(Context const& ctx, HelperReturn const& e) = 0;
  virtual void log(Context const& ctx, Listener const& e) = 0;
  virtual void log(Context const& ctx, Match const& e) = 0;
  virtual void log(Context const& ctx, NoResubmission const& e) = 0;
  virtual void log(Context const& ctx, Notification const& e) = 0;
  virtual void log(Context const& ctx, Pending const& e) = 0;
  virtual void log(Context const& ctx, Purge const& e) = 0;
  virtual void log(Context const& ctx, ReallyRunning const& e) = 0;
  virtual void log(Context const& ctx, Refused const& e) = 0;
  virtual void log(Context const& ctx, RegisterDAGJob const& e) = 0;
  virtual void log(Context const& ctx, RegisterJob const& e) = 0;
  virtual void log(Context const& ctx, RegisterPartitionableJob const& e) = 0;
  virtual void log(Context const& ctx, RegisterPartitionedJob const& e) = 0;
  virtual void log(Context const& ctx, RegisterSimpleJob const& e) = 0;
  virtual void log(Context const& ctx, ResourceUsage const& e) = 0;
  virtual void log(Context const& ctx, Resume const& e) = 0;
  virtual void log(Context const& ctx, Running const& e) = 0;
  virtual void log(Context const& ctx, ShallowResubmission const& e) = 0;
  virtual void log(Context const& ctx, Suspend const& e) = 0;
  virtual void log(Context const& ctx, Transfer const& e) = 0;
  virtual void log(Context const& ctx, TransferAccepted const& e) = 0;
  virtual void log(Context const& ctx, TransferFailed const& e) = 0;
  virtual void log(Context const& ctx, TransferOk const& e) = 0;
  virtual void log(Context const& ctx, TransferRefused const& e) = 0;
  virtual void log(Context const& ctx, TransferStart const& e) = 0;
  virtual void log(Context const& ctx, UserTag const& e) = 0;

  virtual bool is_retryable(int error) = 0;
  virtual bool flush(Context const& ctx) = 0;

  boost::function<bool()> termination() { return m_termination; }
  void close() { m_closed = true; }
  bool closed() { return m_closed; }
  std::string instance() { return m_instance; }
  edg_wll_Source source() { return m_source; }
};

}}}}
#endif
