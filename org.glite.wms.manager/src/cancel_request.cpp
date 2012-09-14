// File: request.cpp
// Authors: Marco Cecchi
//          Francesco Giacomini
//          Salvatore Monforte
// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/utilities/wm_commands.h"
#include "glite/wms/common/utilities/input_reader.h"
#include "glite/wms/common/logger/logger_utils.h"

#include "glite/wmsutils/exception/Exception.h"
#include "glite/jobid/JobId.h"

#include "glite/wms/purger/purger.h"
#include "glite/security/proxyrenewal/renewal.h"

#include "glite/lb/producer.h"

#include "cancel_request.h"
#include "lb_utils.h"
#include "submission_utils.h"
#include "listmatch.h"

namespace jobid = glite::jobid;
namespace jdl = glite::jdl;
namespace exception = glite::wmsutils::exception;
namespace utilities = glite::wms::common::utilities;
namespace configuration = glite::wms::common::configuration;
namespace fs = boost::filesystem;

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {
  bool have_lbproxy()
  {
    configuration::Configuration const& config(
      *configuration::Configuration::instance()
    );
    return config.common()->lbproxy();
  }
}

CancelProcessor::CancelProcessor(boost::shared_ptr<CancelState> state)
  : m_state(state)
{ }

void CancelProcessor::operator()()
{
  Debug("considering cancel of " << m_state->m_id.toString());

  JobStatusPtr status;
  try { // get status for whatever kind of job
    status = job_status(m_state->m_id, m_state->m_lb_context, 0);
  } catch (CannotCreateLBContext const& e) {
    Error(std::string(e.what()) + ": ignoring cancel request");
    return;
  }

  if (status && (EDG_WLL_STAT_COLLECTION == status->jobtype || EDG_WLL_STAT_DAG == status->jobtype)) {

    JobStatusPtr collection_status(
      job_status(m_state->m_id, children | child_status )
    );
    if (!collection_status) {
      Error(m_state->m_id.toString() << ": unable to retrieve collection status for processing cancel");
      return;
    }
    char** i = collection_status->children;
    edg_wll_JobStat *j = collection_status->children_states;

    bool incomplete_processing = false;
    bool exit_while = false;
    if (collection_status->children) {
      while (
        !exit_while &&
        i < collection_status->children + collection_status->children_num
        && *i)
      {
        fs::path const cancel_wm_file(get_cancel_token(jobid::JobId(*i)));
        try {
          create_token(cancel_wm_file);
        } catch (CannotCreateToken&) {
          Error(m_state->m_id.toString()<< "Cannot create token for cancel");
        }

        if (j && j->state != EDG_WLL_JOB_UNDEF) {
          LB_Events events(get_cancel_events(m_state->m_lb_context, jobid::JobId(*i)));

          if (is_last_cancelled_by_wm(events)) {
            // this node has already been processed by this cancel
          } else if(EDG_WLL_JOB_DONE == j->state && is_last_done(events, EDG_WLL_DONE_OK)) {
            // this node does not need to be cancelled
          } else if (
            !(EDG_WLL_JOB_DONE == j->state && is_last_done(events, EDG_WLL_DONE_FAILED))
            && is_last_enqueued_by_wm(events, EDG_WLL_ENQUEUED_OK))
          {
            if (EDG_WLL_JOB_ABORTED != j->state) {
              try {
                change_logging_job(m_state->m_lb_context, "", jobid::JobId(*i));
                log_cancelled(m_state->m_lb_context);
                change_logging_job(m_state->m_lb_context, "", m_state->m_id);
                m_state->m_wm.cancel(jobid::JobId(*i), m_state->m_lb_context);
              } catch (...) {
              }
            } else {
              Info("cannot cancel aborted job " << m_state->m_id.toString());
            }
          } else {
            if (EDG_WLL_STAT_DAG == status->jobtype) {
              if (EDG_WLL_JOB_ABORTED != j->state) {
                try {
                  change_logging_job(m_state->m_lb_context, "", jobid::JobId(*i));
                  log_cancelled(m_state->m_lb_context);
                  change_logging_job(m_state->m_lb_context, "", m_state->m_id);
                  m_state->m_wm.cancel(jobid::JobId(*i), m_state->m_lb_context);
                } catch (...) {
                }
              } else {
                Info("cannot cancel aborted job " << m_state->m_id.toString());
              }
            } else {
              incomplete_processing = true;
            }
          }
        } else {
          Error(m_state->m_id.toString() << ": unable to retrieve children information from jobstatus");
          exit_while = true;
        }
        ++i;
        ++j;
      }
    }

    if (incomplete_processing) {
      // some node is still under wm processing
      m_state->m_events.schedule_at(
        *this,
        std::time(0) + match_retry_period(),
        cancel_priority
      );
    } else {
      // being a collection an 'overjob', purging can be done
      // right here, as it should, without worrying about
      // possible race conditions
      log_cancelled(m_state->m_lb_context);
      log_done_cancelled(m_state->m_lb_context);

      boost::function<int(edg_wll_Context)> log_cancel_done = have_lbproxy() ? 
        boost::bind(edg_wll_LogCancelDONEProxy,_1,"edg_wll_LogCancelDONEProxy") : 
        boost::bind(edg_wll_LogCancelDONE,_1,"edg_wll_LogCancelDONE");

      purger::Purger(have_lbproxy()).
        force_dag_node_removal().
        log_using( log_cancel_done )(m_state->m_id);

      return;
    }
  } else { // single job or node
    
    fs::path const cancel_token(get_cancel_token(m_state->m_id));
    if (!fs::exists(cancel_token)) { 
    Debug("cancel token not found for " << m_state->m_id.toString());
      return; // submit request has got the message
              // or else, if the job was purged, another reason
              // not to worry anymore
    }

    LB_Events events(get_cancel_events(m_state->m_lb_context, m_state->m_id));
    if(is_done(status) && is_last_done(events, EDG_WLL_DONE_OK)) {
       // this node does not need to be cancelled
    } else if (
      !(is_done(status) && is_last_done(events, EDG_WLL_DONE_FAILED))
      && is_last_enqueued_by_wm(events, EDG_WLL_ENQUEUED_OK)
    ) {
      if (!is_aborted(status)) {
        try {

          log_cancelled(m_state->m_lb_context);
          m_state->m_wm.cancel(m_state->m_id, m_state->m_lb_context);
        } catch (...) {

          Info("failed cancel for " << m_state->m_id.toString());
          // this will cause the job to be aborted rather than
          // cancelled, so the user knows there's something wrong with this job
          // filesystem exceptions are caught by WMReal

          UnregisterProxy(m_state->m_id)();

          boost::function<int(edg_wll_Context)> log_abort = have_lbproxy() ? 
            boost::bind(edg_wll_LogAbortProxy,_1,"edg_wll_LogAbortProxy") : 
            boost::bind(edg_wll_LogAbort,_1,"edg_wll_LogAbort");

          purger::Purger(have_lbproxy()).
            force_dag_node_removal().
            log_using( log_abort )(m_state->m_id);
        }

        Info("forwarded cancel request for job " << m_state->m_id.toString());
      } else {
        Info("cannot cancel aborted job " << m_state->m_id.toString());
      }
    } else { // non terminal state which happens to fall "behind" this cancel
      Info("failed cancel for " << m_state->m_id.toString());
      UnregisterProxy(m_state->m_id)();

      boost::function<int(edg_wll_Context)> log_abort = have_lbproxy() ? 
        boost::bind(edg_wll_LogAbortProxy,_1,"edg_wll_LogAbortProxy") : 
        boost::bind(edg_wll_LogAbort,_1,"edg_wll_LogAbort");

      purger::Purger(have_lbproxy()).
        force_dag_node_removal().
        log_using( log_abort )(m_state->m_id);

      // can be restored once wmp produces 'atomic' requests
      //Debug("cancel: rescheduling request for " << m_state->m_id.toString());
      //m_state->m_events.schedule_at(
      //  *this,
      //  std::time(0) + match_retry_period(),
      //  cancel_priority
      //);
    }
  }
}

}}}}
