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

// Author: Marco Cecchi

#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/exception.hpp>

#include <classad_distribution.h>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/WMPConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/common/utilities/wm_commands.h"
#include "glite/wms/common/utilities/jobdir.h"
#include "glite/wms/helper/exceptions.h"

#include "glite/jobid/JobId.h"

#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAttributes.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"

#include "glite/wms/purger/purger.h"
#include "glite/security/proxyrenewal/renewal.h"
#include "glite/wms/common/utilities/manipulation.h"

#include "glite/lb/statistics.h"

#include "events.h"
#include "replanner.h"
#include "lb_utils.h"
#include "submission_utils.h"

namespace configuration = glite::wms::common::configuration;
namespace utilities = glite::wms::common::utilities;
namespace wmsutils = glite::wmsutils;
namespace bfs = boost::filesystem;

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {

void do_nothing() {
}

void unrecoverable(
  ContextPtr lb_context,
  std::string jobid,
  std::string msg)
{
  Error(jobid << " failed (" << msg << ')');
  log_abort(lb_context, msg);
  glite_jobid_t Job_Id;
  glite_jobid_parse(jobid.c_str(), &Job_Id);
}

class Replan
{
  boost::shared_ptr<ReplannerState> m_state;
public:
  Replan(boost::shared_ptr<ReplannerState> state) : m_state(state) { }
  void operator()(std::string const& jobid) {

    classad::ClassAd job_ad;
    try {
      job_ad = *wmsutils::classads::parse_classad(
        get_original_jdl(m_state->m_lb_context, jobid) // for queries we can use a generic context
      );
    } catch (CannotRetrieveJDL const& e) {
      Error("cannot retrieve jdl for job:" << jobid);
      return;
    } catch (...) {
      Error("unknown exception while retrieving jdl for job:" << jobid);
      return;
    }

    bool no_throw = false;
    if (jdl::get_enable_wms_feedback(job_ad, no_throw)) {
      // CREAM queues only as by WMSRequirements
      if (!shallow_resubmission_is_enabled(job_ad)) {
        Error("cannot replan " << jobid << " with shallow resubmission disabled");
        return;
      }

      ContextPtr lb_context;
      try {
        lb_context = create_context(
          jobid,
          jdl::get_x509_user_proxy(job_ad),
          ""
        );
      } catch (CannotCreateLBContext const& e) {
        Error(e.what());
        return;
      }
      std::string const sequence_code(get_lb_sequence_code(lb_context));
      jdl::set_lb_sequence_code(job_ad, sequence_code);

      std::string const ceid = get_last_matched_ceid(lb_context, jobid);
      Debug("last submission was to " << ceid);
      static boost::regex const cream_ceid(".+/cream-.+");
      bool const is_cream_ce = boost::regex_match(ceid, cream_ceid);
      if (is_cream_ce) {

        Info("replanning job " << jobid);

        bfs::directory_iterator end;
        bfs::path base_path(
          (*configuration::Configuration::instance()).ns()->sandbox_staging_path()
        );
        base_path /= bfs::path(utilities::get_reduced_part(jobid), bfs::native);
        base_path /= bfs::path(utilities::to_filename(jobid), bfs::native);
        static boost::regex const filter(
          (*configuration::Configuration::instance()).wm()->token_file()
          + "_.*" 
        );

        std::string token_file;
        try {
          bool found = false;
          for (bfs::directory_iterator it(base_path); it != end; ++it) {
            if (boost::regex_match(it->leaf(), filter)) {
              if (!found) {
                token_file = it->string();
                found = true;
              } else {
                unrecoverable(lb_context, jobid, it->string() + ": two shallow resubmit tokens are present at the same time!");
              }
            }
          }
        } catch (bfs::filesystem_error const& e) {
          Error(e.what());
          return;
        }

        if (token_file.empty()) {
          Info(
            "couldn't find any token for job " << jobid
            << ", which might have gone Running"
          );
          return;
        }

        std::string token_num_str(
          token_file,
          token_file.rfind('_') + 1,
          token_file.size() - token_file.rfind('_') - 1
        );
        int token_num = -1;
        try {
          token_num = boost::lexical_cast<float>(token_num_str);
        } catch(boost::bad_lexical_cast const&) {
          Error("cannot extract token number from " << token_file);
          return;
        }

        Debug("present token number for job " << jobid << " is " << token_num);
        if (token_num >= 0) {

          int max_replannings(
            configuration::Configuration::instance()->wm()->max_replans_count()
          );

          std::string token_file_plus_one(
            token_file.substr(0, token_file.rfind('_') + 1)
            + boost::lexical_cast<std::string>(token_num + 1)
          );

          int res = ::rename(token_file.c_str(), token_file_plus_one.c_str());
          if (res && errno != ENOENT) {
            Error(
              "cannot rename token " +
              token_file + " to " + token_file_plus_one +
              " (" + boost::lexical_cast<std::string>(errno) +
              "), replanning skipped for job" + jobid
            );
          } else if (errno == ENOENT) {
            Debug(jobid << " is really running, replanning not needed");
          } else {

            if (token_num >= max_replannings) {
              ::unlink(token_file.c_str()); // so that the job will stop anyway
              unrecoverable(lb_context, jobid, "hit max number of replans");
            }
            bool result = false;
            jdl::remove_replans_count(job_ad, result);
            // pass the new token number on to submit_request, which 
            // will infer that this is a reschedule
            jdl::set_replans_count(job_ad, token_num + 1, result);
            std::string wm_input((*configuration::Configuration::instance()).wm()->input());

            boost::scoped_ptr<glite::wms::common::utilities::JobDir> jobdir(
              new glite::wms::common::utilities::JobDir(
                boost::filesystem::path(
                  wm_input,
                  boost::filesystem::native
                )
              )
            );
              
            log_resubmission_shallow(lb_context, "job will be replanned");
            log_enqueued_start(lb_context, wm_input); // exits the scheduled state,
            // now safe because the request has been successfully written;
            // if the log fails, the possible race condition between
            // subsequent replans is handled in SubmitProcessor
            std::string request;
            try {
              classad::ClassAdUnParser unparser;
              boost::scoped_ptr<classad::ClassAd> command(
                new classad::ClassAd(utilities::submit_command_create(job_ad))
              );
              unparser.Unparse(request, command.get());
              jobdir->deliver(request);
              Info(
                "created replanning request for job " << jobid
                << " with token " << token_file_plus_one
              );
            } catch (utilities::JobDirError const& e) {
              Error("replan request creation failed (" << e.what() <<
                ") for job " << jobid << ", retrying at the next replanning"
              );
              log_enqueued_fail(lb_context, wm_input, request, "cannot write in WM input");
              return;
            }

            log_enqueued_ok(lb_context, wm_input, request);
          }
        }
      } else {
        Debug("feedback is not enabled for non cream job " << jobid);
      }
    } else {
        Debug("feedback was not enabled in JDL for job " << jobid);
    }
  } // operator()
};

} // anonymous namespace

Replanner::Replanner(boost::shared_ptr<ReplannerState> state)
  : m_state(state)
{
  edg_wll_Context context;
  int errcode = edg_wll_InitContext(&context);

  ContextPtr result(context, edg_wll_FreeContext);

  errcode |= edg_wll_SetParam(
    context,
    EDG_WLL_PARAM_X509_PROXY,
    get_host_x509_proxy().c_str()
  );

  if (errcode) {
    Error("cannot create LB context (" << errcode << ')');
  } else {
    m_state->m_lb_context = result;
  }
}

void Replanner::operator()()
{
  int const grace_period(
    (*configuration::Configuration::instance()).wm()->replan_grace_period()
  );
  int const replan_rate(grace_period);

  Info("replanner in action");

  //edg_wll_Context ctx;
  //edg_wll_InitContext(&ctx);

  //std::string lbserver_fqdn(
  //  (*configuration::Configuration::instance()).wp()->lbserver().front()
  //);
  //std::string lbserver_host;
  //int lbserver_port = 9000;
  //static boost::regex const has_port("(.*):([[:digit:]]*).*");
  //boost::smatch pieces;
  //if (boost::regex_match(lbserver_fqdn, pieces, has_port)) {
  //  lbserver_host.assign(pieces[1].first, pieces[1].second);
  //  lbserver_port = boost::lexical_cast<int>(
  //    std::string().assign(pieces[2].first, pieces[2].second)
  //  );
  //}
  //std::string lbserver;
  //if (lbserver_host.empty()) {
  //  lbserver = lbserver_fqdn;
  //} else {
  //  lbserver = lbserver_host;
  //}
  //edg_wll_SetParam(
  //  ctx,
  //  EDG_WLL_PARAM_QUERY_SERVER,
  //  lbserver.c_str()
  //);
  //edg_wll_SetParam(ctx, EDG_WLL_PARAM_QUERY_SERVER_PORT, lbserver_port);

  //edg_wll_QueryRec group[2];
  //group[0].attr = EDG_WLL_QUERY_ATTR_DESTINATION;
  //group[0].op = EDG_WLL_QUERY_OP_EQUAL;
  //group[0].value.c = "ALL";

  //group[1].attr = EDG_WLL_QUERY_ATTR_UNDEF;

/*
  time_t now(::time(0));
  time_t to(now);
  time_t last_day = 86400; // statistics over the last day: with weighted mean
                           // more recent entries will count more, so this has
                           // not to be necessarily a configuration parameter
                           // TODO: parametrize
  time_t from(now - last_day);

  float* durations = 0;
  float* dispersions = 0;
  char** groups = 0;
  int from_res, to_res;
  if (
    false
    edg_wll_StateDurationsFromTo(
      ctx,
      group,
      EDG_WLL_JOB_SUBMITTED, // TODO transition SCHEDULED -> RUNNING not implemented yet, lb server side
      EDG_WLL_JOB_RUNNING,
      0,
      &from,
      &to,
      &durations,
      &dispersions,
      &groups,
      &from_res,
      &to_res)
  ) {
    char *et,*ed;
    edg_wll_Error(ctx,&et,&ed);
    Error(
      "edg_wll_StateDurationsFromTo(" <<
      lbserver
      << ',' << lbserver_port << "): " << et << ',' << ed
    );
  } else {
    for (int i = 0; groups[i]; ++i) {
      Debug(
        "Average duration at " << groups[i] << ": " << durations[i] << '\n' <<
        "Dispersion index: " << dispersions[i] << '\n'
      );
      free(groups[i]);
    }
    free(groups);
    free(durations);
    free(dispersions);
  }
*/

  if (m_state->m_lb_context) {
    try {
      std::vector<std::string> stuck_jobs_ids = get_scheduled_jobs(
        m_state->m_lb_context, grace_period
      );
      if (stuck_jobs_ids.empty()) {
        Debug("no jobs in scheduled state for more than " << grace_period << " seconds for replanning");
      }
      Replan replan(m_state);
      for_each(stuck_jobs_ids.begin(), stuck_jobs_ids.end(), replan);
    } catch (std::exception const& e) {
      Error("replanner: " << e.what());
    } catch (...) {
      Error("replanner: caught unknown exception");
    }

    m_state->m_events.schedule_at(
      *this,
      std::time(0) + replan_rate,
      replanner_priority
    );
  } else {
    Error("replanner: invalid L&B context, exiting");
  }
}

}}}} // glite::wms::manager::server
