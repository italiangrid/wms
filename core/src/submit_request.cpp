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
#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/exception.hpp>

#include "glite/jdl/DAGAd.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/utilities/wm_commands.h"
#include "glite/wms/common/utilities/input_reader.h"
#include "glite/wms/common/utilities/manipulation.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/helper/exceptions.h"

#include "glite/wmsutils/exception/Exception.h"
#include "glite/jobid/JobId.h"

#include "glite/wms/purger/purger.h"
#include "glite/security/proxyrenewal/renewal.h"

#include "glite/lb/producer.h"

#include "submit_request.h"
#include "lb_utils.h"
#include "submission_utils.h"
#include "recovery.h"

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

  static int const EXIT_RETRY_NODE = 98;
  static int const EXIT_ABORT_NODE = 99;

  struct null_deleter { void operator()(void const *) const { } };

  void queue_new_submit_request(
    std::string name,
    jobid::JobId const jobid, 
    classad::ClassAd& job_jdl,
    boost::shared_ptr<SubmitState> state
  ) {
    jdl::set_lb_sequence_code(job_jdl, "");
    RemoveNodeFromCollection cleanup(name, state);
    boost::shared_ptr<classad::ClassAd> fake_command_ad(
      new classad::ClassAd(
        utilities::submit_command_create(job_jdl)
      )
    );

    ContextPtr lb_context
      = create_context(jobid, state->m_x509_proxy, "");

    boost::shared_ptr<SubmitState> new_state(
      new SubmitState(
        fake_command_ad,
        "jobsubmit",
        jobid,
        "",
        state->m_x509_proxy,
        lb_context,
        cleanup,
        state->m_wm,
        state->m_events,
        state->m_jw_template
      )
    );
    state->m_events.schedule(
      SubmitProcessor(new_state),
      submit_priority
    );
  }

  class log_abort_for_child {
    ContextPtr m_context;
    std::string const m_reason;
  public:
    log_abort_for_child(ContextPtr context, std::string const& reason)
      : m_context(context), m_reason(reason) { }
    void operator()(char* id, edg_wll_JobStat& s) {
      if (is_submitted(JobStatusPtr(&s, null_deleter())) ||
        is_waiting(JobStatusPtr(&s, null_deleter()))) {

        jobid::JobId const _id(id);
        change_logging_job(m_context, EDG_WLL_SEQ_ABORT, _id);
        log_abort(m_context, m_reason); 
      }
    }
  };

  void unrecoverable_collection(boost::shared_ptr<SubmitState> state, std::string msg)
  {
    JobStatusPtr cs(
      job_status(state->m_id, state->m_lb_context, children)
    );
    if (!cs) {
      Error(
        state->m_id.toString() << ": unable to retrieve collection status"
      );
      return;
    }
    if (!cs->children) {
      Error(state->m_id.toString() << ": unable to retrieve children information from jobstatus");
      return ;
    }

    char** i = cs->children;
    edg_wll_JobStat *j = cs->children_states;
    log_abort_for_child lafc(state->m_lb_context, msg);
    while (i < cs->children + cs->children_num && *i) {
      if (j && j->state != EDG_WLL_JOB_UNDEF) {
        lafc(*i, *j);
      } else {
        Error(state->m_id.toString() << ": unable to retrieve children information from jobstatus");
        *i = 0;
        continue;
      }
      ++i;
      ++j;
    }

    change_logging_job(state->m_lb_context, EDG_WLL_SEQ_ABORT, state->m_id);
  }

  void unrecoverable(
    boost::shared_ptr<SubmitState> state,
    std::string msg)
  {
    if (job_type(*state->m_ad) == collection) {
      unrecoverable_collection(state, state->m_id.toString() + " failed (" + msg + ')');
    }
    Error(state->m_id.toString() << " failed (" << msg << ')');
    log_abort(state->m_lb_context, msg);
    fs::path const cancel_token(get_cancel_token(state->m_id));
    ::unlink(cancel_token.native_file_string().c_str());
  }

  bool have_lbproxy()
  {
    configuration::Configuration const& config(
      *configuration::Configuration::instance()
    );
    return config.common()->lbproxy();
  }

  int get_expiry_period()
  {
    configuration::WMConfiguration const& wm_config
      = *configuration::Configuration::instance()->wm();
    return wm_config.expiry_period();
  }

  boost::tuple<
    int,
    int,
    std::vector<std::pair<std::string, int> >,
    std::vector<std::string>
  > retrieve_lb_info(
    jobid::JobId const& jobid,
    ContextPtr context
  )
  {
    // this can currently happen only in case of a resubmission
    // for a resubmission we need a few information stored in the LB:
    // - previous deep and shallow resubmissions
    // - previous matches
    // - original jdl

    LB_Events events(get_interesting_events(context, jobid));
    if (events.empty()) {
      Warning("Cannot retrieve interesting events for " << jobid.toString());
    }

    std::vector<std::pair<std::string, int> > const previous_matches = get_previous_matches(events);

    // in principle this warning should happen only if the req is a resubmit
    if (previous_matches.empty()) {
      Warning("cannot retrieve previous matches for " << jobid.toString());
    }

    std::vector<std::string> previous_matches_simple;
    for (std::vector<std::pair<std::string, int> >::const_iterator it = previous_matches.begin();
         it != previous_matches.end(); ++it) {
      previous_matches_simple.push_back(it->first);
    }

    boost::tuple<int, int> retries = get_retry_counts(events);
    return boost::make_tuple<
      int,
      int,
      std::vector<std::pair<std::string, int> >,
      std::vector<std::string>
    >(
      retries.get<0>(),
      retries.get<1>(),
      previous_matches,
      previous_matches_simple
    );
  }

  bool is_request_expired(
    boost::shared_ptr<classad::ClassAd> jdl,
    std::time_t first_processed)
  {
    bool exists = false;
    int expiry_time = 0;
    if (jdl) {
      expiry_time = jdl::get_expiry_time(*jdl, exists);
    }
    if (!exists) {
      return std::time(0) - first_processed > get_expiry_period();
    } else {
      return expiry_time < std::time(0);
    }
  }

  bool shallow_resubmission_is_enabled(boost::shared_ptr<classad::ClassAd> jdl)
  {
    return server::shallow_resubmission_is_enabled(*jdl);
  }
}

SubmitProcessor::SubmitProcessor(boost::shared_ptr<SubmitState> state)
  : m_state(state)
{ }

void SubmitProcessor::postpone(
  std::string const& reason,
  int match_retry_period
)
{
  if (!reason.empty()) {
    Info("postponing " << m_state->m_id.toString() << " (" << reason << ')');
    log_pending(m_state->m_lb_context, reason);
  }
  m_state->m_events.schedule_at(
    *this,
    std::time(0) + match_retry_period,
    submit_priority
  );
}

void SubmitProcessor::operator()()
{
  Debug("considering (re)submit of " << m_state->m_id.toString());

  bool proxy_expired = true;
  try {
    proxy_expired = is_proxy_expired(m_state->m_id);
  } catch(MissingProxy const& e) {
    std::string const error_msg(
      "X509 proxy not found or I/O error (" + std::string(e.what()) + ')'
    );
    unrecoverable(m_state, error_msg);
    return;
  } catch(InvalidProxy const& e) {
    std::string const error_msg(
      "invalid X509 proxy (" + std::string(e.what()) + ')'
    );
    unrecoverable(m_state, error_msg);
    return;
  }

  if (proxy_expired) {
    unrecoverable(m_state, "proxy expired");
    return;
  }

  if (is_request_expired(m_state->m_ad, m_state->m_created_at)) {
    unrecoverable(m_state, "request expired");
    return;
  }

  job_type_t job_type_ = job_type(*m_state->m_ad);
  try {
    if (job_type_ == dag) {

      static std::string const na = "notapplicable";
      if (SubmitState::START == m_state->m_last_checkpointing) {
        log_match(m_state->m_lb_context, "dagman");
        // the wm now manages the DAG in its entirety, 
        // so the LB state machine must be someway tricked
        log_enqueued_start(m_state->m_lb_context, na);
        log_enqueued_ok(m_state->m_lb_context, na, na);
        //log_dequeued(m_state->m_lb_context, na);
        log_transfer_start(m_state->m_lb_context, "L&B");
        log_transfer_ok(
          m_state->m_lb_context,
          "L&B",
          na,
          "DAG managed by WM"
        );
        log_running(m_state->m_lb_context, "localhost");
      }

      jdl::DAGAd dagad(*m_state->m_ad);
      jdl::DAGAd::Graph_t dag = dagad.graph();
      jdl::DAGAd::VertexIterator v_i, v_end;
      // - the recovery mechanism is by design included in the processing of the request,
      // unlike what happens with other job types
      // - starting from LB 2.1, querying the LBProxy will also return terminated jobs
      // allowing to check for 'green nodes'
      // in this first phase, similar to what happens with the WM recovery, an assessment
      // is done about each node's status
      for (boost::tie(v_i, v_end) = boost::vertices(dag); v_i != v_end; ++v_i) {

        // find classad by graph vertex name
        std::string node_name = get(boost::vertex_name, dag)[*v_i];
        jdl::DAGAd::node_iterator classad_node_it = dagad.find(node_name);
        jdl::DAGNodeInfo const& classad_info = classad_node_it->second;
        classad::ClassAd node_jdl(
          *classad_info.description_ad()
        );
        jobid::JobId const node_id(jdl::get_edg_jobid(node_jdl));
        JobStatusPtr status(
          job_status(node_id, m_state->m_lb_context)
        );

        Debug(node_name << " id " << node_id.toString() << " has status " << status->state);
        // let's see if the job has reached a terminal state (green or red)
        if (is_aborted(status) || is_cancelled(status)) {
          get(boost::vertex_color, dag)[*v_i] = boost::red_color;
        } else if (is_done(status)) {

          try {
            LB_Events events(
              get_recovery_events(m_state->m_lb_context, node_id)
            );
            if (is_last_done(events, EDG_WLL_DONE_OK)) {
              get(boost::vertex_color, dag)[*v_i] = boost::green_color;
            } else {
              // done/failed is not a terminal state
              get(boost::vertex_color, dag)[*v_i] = boost::gray_color;
            }
          } catch (LB_QueryFailed const& e) {
            std::string const error_msg(" unable to retrieve recovery events");
            Error(m_state->m_id.toString() << error_msg);
            return;
          } catch (LB_QueryTemporarilyFailed const& e) {
            std::string const error_msg(
              " unable to retrieve recovery events (transient error)"
            );
            Error(m_state->m_id.toString() << error_msg);
            m_state->m_events.schedule_at(
              *this,
              std::time(0) + match_retry_period(),
              submit_priority
              );
          }
        } else if (is_submitted(status)) {
          get(boost::vertex_color, dag)[*v_i] = boost::white_color; // not yet started
        } else { // not in terminal state
          get(boost::vertex_color, dag)[*v_i] = boost::gray_color;
        }
      } // end 'recovery'

      // second phase, plan submission of pending nodes or terminate
      // with some status (ok/aborted)
      bool completed_dag = true;
      for (boost::tie(v_i, v_end) = boost::vertices(dag); v_i != v_end; ++v_i) {
        if (get(boost::vertex_color, dag)[*v_i] == boost::gray_color) {

          completed_dag = false;
        } else if (get(boost::vertex_color, dag)[*v_i] == boost::white_color) {
          // still unprocessed request
          completed_dag = false;
          jdl::DAGAd::InEdgeIterator in_i, in_end;
          boost::tie(in_i, in_end) = boost::in_edges(*v_i, dag);
          if (in_i == in_end) {
            // job can be run straightaway
            std::string node_name = get(boost::vertex_name, dag)[*v_i];
            jdl::DAGAd::node_iterator classad_node_it = dagad.find(node_name);
            jdl::DAGNodeInfo const& classad_info = classad_node_it->second;
            classad::ClassAd node_jdl(
              *classad_info.description_ad()
            );
            jobid::JobId const node_id(jdl::get_edg_jobid(node_jdl));
            try {
              Debug("queueing new submit for " << node_name << " as it has no parents");
              queue_new_submit_request(node_name, node_id, node_jdl, m_state);
            } catch (CannotCreateLBContext const& e) {
              Error(e.what() << ": cannot create request for " << node_id.toString());
              get(boost::vertex_color, dag)[*v_i] = boost::red_color;
              continue;
            }
            get(boost::vertex_color, dag)[*v_i] = boost::gray_color;
          } else { // node has in-edges
            bool preceding_jobs_are_green = true;
            for (
              boost::tie(in_i, in_end) = boost::in_edges(*v_i, dag);
              in_i != in_end;
              ++in_i
            ) {
              jdl::DAGAd::Vertex src = boost::source(*in_i, dag);
              if (get(boost::vertex_color, dag)[src] == boost::red_color) {
                get(boost::vertex_color, dag)[*v_i] = boost::red_color;

                std::string node_name = get(boost::vertex_name, dag)[*v_i];
                jdl::DAGAd::node_iterator classad_node_it = dagad.find(node_name);
                jdl::DAGNodeInfo const& classad_info = classad_node_it->second;
                classad::ClassAd node_jdl(
                  *classad_info.description_ad()
                );
                jdl::set_lb_sequence_code(node_jdl, "");
                RemoveNodeFromCollection cleanup(node_name, m_state);
                boost::shared_ptr<classad::ClassAd> fake_command_ad(
                  new classad::ClassAd(
                    utilities::submit_command_create(node_jdl)
                  )
                );
                jobid::JobId const node_id(jdl::get_edg_jobid(node_jdl));
                ContextPtr lb_context
                  = create_context(node_id, m_state->m_x509_proxy, "");

                boost::shared_ptr<SubmitState> new_state(
                  new SubmitState(
                    fake_command_ad,
                    "jobsubmit",
                    node_id,
                    "",
                    m_state->m_x509_proxy,
                    lb_context,
                    cleanup,
                    m_state->m_wm,
                    m_state->m_events,
                    m_state->m_jw_template
                  )
                );

                unrecoverable(new_state, "parents have aborted");
                preceding_jobs_are_green = false;
                break;
              } else if (get(boost::vertex_color, dag)[src] == boost::white_color
                || get(boost::vertex_color, dag)[src] == boost::gray_color
              ) {
                preceding_jobs_are_green = false;
              }
            }
            if (preceding_jobs_are_green) {
              std::string node_name = get(boost::vertex_name, dag)[*v_i];
              jdl::DAGAd::node_iterator classad_node_it = dagad.find(node_name);
              jdl::DAGNodeInfo const& classad_info = classad_node_it->second;
              classad::ClassAd node_jdl(
                *classad_info.description_ad()
              );
              jobid::JobId const node_id(jdl::get_edg_jobid(node_jdl));
              try {
                Debug("queueing new submit for " << node_name << " as it has terminal parents");
                queue_new_submit_request(node_name, node_id, node_jdl, m_state);
              } catch (CannotCreateLBContext const& e) {
                Error(e.what() << ": cannot create request for " << node_id.toString());
                get(boost::vertex_color, dag)[*v_i] = boost::red_color;
              }
            }
          }
        }
      }

      if (!completed_dag) {
        m_state->m_events.schedule_at(
          *this,
          std::time(0) + match_retry_period(),
          submit_priority
          );
        m_state->m_last_checkpointing = SubmitState::PENDING;
      } else {
        bool allgreen = true;
        for (boost::tie(v_i, v_end) = boost::vertices(dag); v_i != v_end; ++v_i) {
          if (get(boost::vertex_color, dag)[*v_i] != boost::green_color) {
            allgreen = false;
            break;
          }
        }
        if (allgreen) {
          log_done_ok(m_state->m_lb_context, "DAG successfully completed", 0);
        } else {
          log_abort(m_state->m_lb_context, "DAG completed with failed jobs");
        }
      }
    } else if (job_type_ == collection) {

      if (SubmitState::START == m_state->m_last_checkpointing) {
        Debug(m_state->m_id.toString() << ", started delivering collection");
        m_state->m_wm.submit_collection(
          *m_state->m_ad,
          m_state->m_lb_context,
          m_state->m_collection_pending,
          m_state->m_jw_template
        );
      }
      if (m_state->m_collection_pending.empty()) {

        Info(m_state->m_id.toString() << ", collection delivered");
      } else {

        if (SubmitState::START == m_state->m_last_checkpointing) {
          Info(m_state->m_id.toString() << ", considering pending collection");
          // manage all pending jobs as normal jobs
          jdl::DAGAd dagad(*m_state->m_ad);
          jdl::DAGAd::node_iterator first_node, last_node;
          boost::tie(first_node, last_node) = dagad.nodes();
          std::set<std::string>::iterator first =
            m_state->m_collection_pending.begin();
          std::set<std::string>::iterator const last =
            m_state->m_collection_pending.end();
          for ( ; first != last; ++first) {

            std::string node_name = *first;
            jdl::DAGAdNodeIterator it = dagad.find(node_name);
            assert(it != last_node);
            jdl::DAGNodeInfo const& node_info = it->second;
            classad::ClassAd node_jdl(
              *node_info.description_ad()
            );
            jobid::JobId const node_id(jdl::get_edg_jobid(node_jdl));
            try {
              queue_new_submit_request(node_name, node_id, node_jdl, m_state);
            } catch (CannotCreateLBContext const& e) {
              Error(e.what() << ": cannot create request for " << node_id.toString());
            }
          } // for each child job

          m_state->m_last_checkpointing = SubmitState::PENDING;
          m_state->m_events.schedule_at(
            *this,
            std::time(0) + match_retry_period(),
            submit_priority
          );
        }
      }
    } else if (job_type_ == single) {

      if (SubmitState::LIMBO == m_state->m_last_checkpointing) {
        try {
          JobStatusPtr status(job_status(m_state->m_id));
          ContextPtr context(m_state->m_lb_context);
          LB_Events events(get_recovery_events(context, m_state->m_id));
    
          if (in_limbo(status, events)) {
            postpone(" waiting in limbo", match_retry_period());
          } else {
            m_state->m_last_checkpointing = SubmitState::START;
            Debug(m_state->m_id.toString() << " exited from limbo!");
          }
        } catch(LB_QueryFailed&) {
          std::string const error_msg(" unable to retrieve recovery events");
          Error(m_state->m_id.toString() << error_msg);
          return;
        }
      }

      int deep_count, shallow_count;
      std::vector<std::pair<std::string, int> > previous_matches;
      std::vector<std::string> previous_matches_simple;
      if (m_state->m_command == "jobresubmit") {

        boost::tie(
          deep_count,
          shallow_count,
          previous_matches,
          previous_matches_simple
        )
          = retrieve_lb_info(m_state->m_id, m_state->m_lb_context);
      
        std::string const job_ad_str(
          get_original_jdl(m_state->m_lb_context, m_state->m_id)
        );
        if (job_ad_str.empty()) {
          throw CannotRetrieveJDL();
        }

        std::auto_ptr<classad::ClassAd> job_ad(
          glite::wmsutils::classads::parse_classad(job_ad_str)
        );

        m_state->m_ad = job_ad;
        job_ad.release();

        jdl::set_edg_previous_matches(*m_state->m_ad, previous_matches_simple);
        jdl::set_edg_previous_matches_ex(*m_state->m_ad, previous_matches);
      }

      fs::path const cancel_token(get_cancel_token(m_state->m_id));
      if (fs::exists(cancel_token)) {
        // this check also addresses a resubmit 
        // coming in after the job has been cancelled
        log_cancelled(m_state->m_lb_context);
        log_done_cancelled(m_state->m_lb_context);
        purger::Purger thePurger(have_lbproxy());
        thePurger.force_dag_node_removal();
        thePurger(m_state->m_id);

        return;
      }

      fs::directory_iterator end;
      fs::path base_path(
        (*configuration::Configuration::instance()).ns()->sandbox_staging_path()
      );
      base_path /= fs::path(utilities::get_reduced_part(m_state->m_id), fs::native);
      base_path /= fs::path(utilities::to_filename(m_state->m_id), fs::native);
      std::string last_token_file;
      boost::regex const filter(
        (*configuration::Configuration::instance()).wm()->token_file()
        + "_.*"
      );
      bool token_found = false;
      try {
        for (fs::directory_iterator it(base_path); it != end; ++it) {
          if (boost::regex_match(it->leaf(), filter)) {
            if (!token_found) {
              last_token_file = it->string();
              token_found = true;
            } else {
              unrecoverable(
                m_state,
                it->string() +
                ": at least two shallow resubmit tokens are present at the same time!"
              );
            }
          }
        }
      } catch (fs::filesystem_error const& e) {
        Error(e.what());
        return;
      }

      int replans_count = 0;
      if (token_found && !last_token_file.empty()) {
        replans_count = int(boost::lexical_cast<float>(
          last_token_file.substr(
            last_token_file.rfind('_') + 1
          ).c_str()
        ));
        bool nothrow = false;
        jdl::set_replans_count(*m_state->m_ad, replans_count, nothrow);
        Debug("found token number " << replans_count << " for job " << m_state->m_id.toString());
      }

      fs::path const token_file(get_reallyrunning_token(m_state->m_id, replans_count));
      fs::path const token_tmp_file(
        token_file.native_file_string() + '.', // so as to work always with a lexical_cast to float
        fs::native
      );
      std::string token_file_str(token_file.native_file_string());
      std::string token_tmp_file_str(token_tmp_file.native_file_string());
      if (m_state->m_command == "jobresubmit") {
        if (shallow_resubmission_is_enabled(m_state->m_ad)) {

          int res = ::rename(token_file_str.c_str(), token_tmp_file_str.c_str());
          if (res && errno != ENOENT) {
            Error("cannot rename token "
              + token_file.native_file_string()
              + " (error " + boost::lexical_cast<std::string>(errno) + ')'
            );
            throw CannotCreateToken();
          }

          if (fs::exists(token_tmp_file)) {
            server::check_shallow_count(*m_state->m_ad, shallow_count);
            log_resubmission_shallow(
              m_state->m_lb_context,
              token_file_str
            );
          } else {
            server::check_deep_count(*m_state->m_ad, deep_count);
            log_resubmission_deep(
              m_state->m_lb_context,
              token_file_str
            );
            create_token(token_tmp_file);
          }
        } else { // shallow not enabled

          server::check_deep_count(*m_state->m_ad, deep_count);
          log_resubmission_deep(m_state->m_lb_context);
        }
      }

      bool is_replan = false;
      if (m_state->m_command == "jobsubmit") {
        bool has_replans_count = false;
        int jdl_replans_count = jdl::get_replans_count(*m_state->m_ad, has_replans_count);
        if (!has_replans_count) {
          jdl_replans_count = 0;
        }
        if (jdl_replans_count > 0 && replans_count == jdl_replans_count) {
          Debug("this is a reschedule for job " << m_state->m_id.toString());
          is_replan = true;
        } else if (jdl_replans_count > 0 && replans_count > jdl_replans_count) {
          // another reschedule is on the way (on very rare race conditions)
          return; // present request can be safely dropped
        } else {
          if (shallow_resubmission_is_enabled(m_state->m_ad)) {
            if (!fs::exists(token_tmp_file)) { // as a 'reallyrunning' token,
                                           // it must be created anyhow

              create_token(token_tmp_file);
            }
          }
        }
      }

      if (
        !is_replan // the token has already been managed by the replanner
        && shallow_resubmission_is_enabled(m_state->m_ad)
      ) {
        if (::rename(token_tmp_file_str.c_str(), token_file_str.c_str()))
        {
          // ENOENT would be an error as well, unlike the previous case
          Error("cannot rename temporary token "
            + token_tmp_file_str
            + " (error " + boost::lexical_cast<std::string>(errno) + ')'
          );
          throw CannotCreateToken();
        }
      }

      std::string const jobid(m_state->m_id.toString());
      configuration::Configuration const& config(
        *configuration::Configuration::instance()
      );
      std::string maradona(config.ns()->sandbox_staging_path());
      maradona += "/";
      maradona += utilities::get_reduced_part(jobid);
      maradona += "/";
      maradona += utilities::to_filename(jobid);
      maradona += "/Maradona.output";
      ::unlink(maradona.c_str());

      m_state->m_wm.submit(
        *m_state->m_ad,
        m_state->m_lb_context,
        m_state->m_jw_template,
        is_replan
      );

      Info(m_state->m_id.toString() << " delivered");
    } else {
      Error("Unknown job type for job: " << m_state->m_id.toString());
    }
  } catch (std::invalid_argument const& e) {
    unrecoverable(m_state, e.what());
  } catch (jdl::ManipulationException const& e) {
    unrecoverable(m_state, e.what());
  } catch (exception::Exception const& e) {
    unrecoverable(m_state, e.what());
  } catch (LB_QueryFailed const& e) {
    unrecoverable(m_state, " failed (LB query failed)");
  } catch (LB_QueryTemporarilyFailed const& e) {
    postpone("LB query temporarily failed", match_retry_period());
  } catch (HitMaxRetryCount& e) {
    unrecoverable(
      m_state,
      "hit max retry count ("
        + boost::lexical_cast<std::string>(e.count()) + ')'
    );
  } catch (HitJobRetryCount& e) {
    unrecoverable(
      m_state,
      "hit job retry count ("
        + boost::lexical_cast<std::string>(e.count()) + ')'
    );
  } catch (HitMaxShallowCount& e) {
    unrecoverable(
      m_state,
      "hit shallow max retry count ("
        + boost::lexical_cast<std::string>(e.count()) + ')'
    );
  } catch (HitJobShallowCount& e) {
    unrecoverable(
      m_state,
      "hit job shallow retry count ("
        + boost::lexical_cast<std::string>(e.count()) + ')'
    );
  } catch (CannotRetrieveJDL const& e) {
    postpone("cannot retrieve jdl", match_retry_period());
  } catch (helper::HelperError const& e) {
    postpone(e.what(), match_retry_period());
  } catch (CannotCreateToken&) {
    postpone("cannot create token for shallow resubmission", match_retry_period());
  } catch (CannotDeliverFatal& e) {
    unrecoverable(m_state, " cannot deliver request");
  } catch (CannotCreateLBContext const& e) {
    unrecoverable(m_state, std::string(e.what()) + ": ignoring request");
  } catch (std::exception const&e) {
    unrecoverable(m_state, e.what());
  } catch (...) {
    unrecoverable(m_state, "unknown exception caught handling submit request");
  }
}

}}}}
