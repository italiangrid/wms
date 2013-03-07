// File: recovery.cpp
// Author: Francesco Giacomini
// Author: Salvatore Monforte
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

#include <algorithm>

#include <boost/regex.hpp>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/common/utilities/scope_guard.h"
#include "glite/wms/common/utilities/input_reader.h"
#include "glite/wms/common/utilities/wm_commands.h"

#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/jobid/JobId.h"

#include "glite/jdl/DAGAd.h"
#include "glite/jdl/DAGAdManipulation.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"

#include "submit_request.h"
#include "cancel_request.h"
#include "events.h"
#include "dispatcher_utils.h"
#include "submission_utils.h"
#include "lb_utils.h"
#include "input_reader.h"

namespace utilities = glite::wms::common::utilities;
namespace jobid = glite::jobid;
namespace ca = glite::wmsutils::classads;
namespace jdl = glite::jdl;

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {

struct FullRequest
{
  std::string command;
  utilities::InputItemPtr request;
  ClassAdPtr ad;
  FullRequest(std::string const& c, utilities::InputItemPtr const& r, ClassAdPtr a)
    : command(c), request(r), ad(a) { } 
};

struct RequestToRecover
{
  std::string id;
  FullRequest request;
  RequestToRecover(std::string const& i, FullRequest const& r) :
    id(i), request(r) { }
};

typedef std::vector<RequestToRecover> RequestsToRecover;

class Ignore
{
  std::string m_id;
public:
  Ignore(std::string const& id)
    : m_id(id)
  { }
  void operator()(FullRequest const& v) const
  {
    Debug("ignoring " << v.command << " request for " << m_id);
    v.request->remove_from_input();
  }
};

class id_equals
{
  std::string m_id;
public:
  id_equals(std::string const& id)
    : m_id(id)
  { }
  bool operator()(RequestsToRecover::value_type const& v) const
  {
    std::string const& id = v.id;
    return m_id == id;
  }
};

bool is_cancel(FullRequest const& v)
{
  std::string const& command = v.command;
  return command == "jobcancel";
}

RequestsToRecover*
insert_request(
  RequestsToRecover* id_to_requests,
  utilities::InputItemPtr& request
)
{
  boost::function<void()> cleanup(
    boost::bind(&utilities::InputItem::remove_from_input, request)
  );
  // if the request is not valid, cleanup automatically
  utilities::scope_guard cleanup_guard(cleanup);

  std::string const command_ad_str(request->value());

  try {

    ClassAdPtr command_ad(ca::parse_classad(command_ad_str));
    std::string command = utilities::command_get_command(*command_ad);
    jobid::JobId id;
    std::string sequence_code;
    std::string x509_proxy;
    boost::tie(id, sequence_code, x509_proxy)
    = check_request(*command_ad, command);

    id_to_requests->push_back(
      RequestToRecover(
        id.toString(),
        FullRequest(command, request, command_ad)
      )
    );

    cleanup_guard.dismiss();

  } catch (ca::ClassAdError& e) {
    Info(e.what());
  }

  return id_to_requests;
}

struct null_deleter
{
  void operator()(void const *) const { }
};

std::pair<std::string, JobStatusPtr> 
make_pair_id_status(char* id, edg_wll_JobStat& s) 
{
  if(id && &s) {
    return std::make_pair(
      std::string(id), JobStatusPtr(&s, null_deleter())
    );
  }
  return std::make_pair(std::string(), JobStatusPtr());
}

struct RecoverableNodeInfo
{
  std::string name;
  ClassAdPtr ad;
  RecoverableNodeInfo(
    std::string const& n, ClassAdPtr a
  ) : name(n), ad(a) { }
};

class select_recoverable_nodes
{
  std::string const& m_id;
  ContextPtr m_lb_context;
  std::map<std::string, JobStatusPtr> m_children_states;
  
public:
  select_recoverable_nodes(
    std::string const& id,
    ContextPtr lb_context,
    JobStatusPtr collection_status
  ) : m_id(id), m_lb_context(lb_context)

  {

    if (!collection_status->children) {
      Error(m_id << ": unable to retrieve children information from jobstatus");
      return ;
    } 

    char** i = collection_status->children;
    edg_wll_JobStat *j = collection_status->children_states;

    while (i < collection_status->children + collection_status->children_num && *i) {
      if (j && j->state != EDG_WLL_JOB_UNDEF) {
        m_children_states.insert(
          m_children_states.begin(),
          make_pair_id_status(*i, *j)
        );
      } else {
        Error(m_id << ": unable to retrieve children information from jobstatus");
        break;
      }
      ++i;
      ++j;
    }
  }

  std::vector<RecoverableNodeInfo>* operator()(
    std::vector<RecoverableNodeInfo>* recoverable_nodes,
    jdl::DAGAd::node_iterator::value_type const& node     
  ) 
  {
    try {
      classad::ClassAd const& jdl = *node.second.description_ad();
      std::string const node_id = jdl::get_edg_jobid(jdl);
      jobid::JobId jobid(node_id);
      std::map<std::string, JobStatusPtr>::const_iterator it(
        m_children_states.find(node_id)
      );

      if (it == m_children_states.end() || !it->second) {
        Debug(node_id << ": cannot retrieve status (job events have been probably purged)");
        return recoverable_nodes;
      }

      JobStatusPtr status = it->second;
      LB_Events events(get_recovery_events(m_lb_context, jobid));

      if (is_last_done(events, EDG_WLL_DONE_FAILED)) {
        Debug(node_id << ": not recoverable node (DONE FAILED)");
        return recoverable_nodes;
      }
      if (is_waiting(status)) {

        Debug(node_id << ": recoverable node (WAITING)");
        recoverable_nodes->push_back(
          RecoverableNodeInfo(
            node.first,
            ClassAdPtr(static_cast<classad::ClassAd*>(jdl.Copy()))
          )
        );
        return recoverable_nodes;
      }
      
      if (events.empty() && is_submitted(status)) {
        Debug(node_id << ": recoverable node (SUBMITTED)");
        recoverable_nodes->push_back(
          RecoverableNodeInfo(
            node.first,
            ClassAdPtr(static_cast<classad::ClassAd*>(jdl.Copy()))
          )
        );
      } else if (in_limbo(status, events)) {

        Debug(node_id << ": node will wait in limbo");
      } else if(is_ready(status)) {

        Debug(node_id << ": recoverable node (READY)");

        recoverable_nodes->push_back(
          RecoverableNodeInfo(
            node.first,
            ClassAdPtr(static_cast<classad::ClassAd*>(jdl.Copy()))
          )
        );
      } else { // whatever job has definitely passed WM in th chain

        Debug(node_id << ": not recoverable");
      }
    } catch(LB_QueryTemporarilyFailed&) {

      Debug(m_id << ": unable to retrieve recovery events!");
    } catch(LB_QueryFailed&) {

      Debug(m_id << ": unable to retrieve recovery events!");
    } catch(...) {

      Debug(m_id << ": unexpected fatal error");
    }

    return recoverable_nodes;
  }
};

struct create_submit_request
{
  Events& m_events;
  WMReal const& m_wm;
  boost::shared_ptr<std::string> m_jw_template;
  boost::shared_ptr<SubmitState> m_request;

  create_submit_request(
    Events& e,
    WMReal const& w,
    boost::shared_ptr<std::string> j,
    boost::shared_ptr<SubmitState> r
  )
    : m_events(e), m_wm(w), m_jw_template(j), m_request(r) { }

  boost::shared_ptr<SubmitState>
  operator()(RecoverableNodeInfo const& rni) {
    classad::ClassAd const& jdl = *rni.ad;
    std::string const node_id = jdl::get_edg_jobid(jdl); 
    boost::shared_ptr<classad::ClassAd> fake_command_ad_ptr(
      new classad::ClassAd(utilities::submit_command_create(jdl))
    );
    RemoveNodeFromCollection cleanup(rni.name, m_request);
    boost::shared_ptr<SubmitState> request;
    try {
      ContextPtr lb_context = create_context(
        node_id,
        m_request->m_x509_proxy,
        ""
      );
      request.reset(
        new SubmitState(
          fake_command_ad_ptr,
          "jobsubmit", 
          node_id,
         "",
          m_request->m_x509_proxy,
          lb_context,
          cleanup,
          m_wm,
          m_events,
          m_jw_template
        ) 
      );
      m_request->m_collection_pending.insert(rni.name);
    } catch (CannotCreateLBContext const& e) {
      Info(e.what() << ": ignoring request for " << node_id);
    }
    return request;
  }
};

bool is_not_submitted(edg_wll_JobStat status)
{
  return status.state != EDG_WLL_JOB_SUBMITTED;
}

bool is_every_children_submitted(JobStatusPtr s)
{
  return std::find_if(
    s->children_states,
    s->children_states + s->children_num,
    is_not_submitted
  ) == s->children_states + s->children_num;
}

}

void request_recovery(
  std::string const& id,
  FullRequest const& req,
  Events& events,
  WMReal const& wm,
  boost::shared_ptr<std::string> jw_template
)
{

  std::string const& command = req.command;
  jobid::JobId jobid;
  std::string sequence_code;
  std::string x509_proxy;
  boost::tie(
    jobid, sequence_code, x509_proxy
  ) = check_request(*req.ad, command);

  bool submit = true;
  bool limbo = false;

  ContextPtr lb_context;
  if (command == "match") {
    Info("match request");
    submit = false;
    Ignore ignore(id);
    ignore(req);
  } else {
    try {
      lb_context = create_context(id, x509_proxy, sequence_code);
    } catch (CannotCreateLBContext& e) {
      Error(e.what());
      return;
    }

    JobStatusPtr status(job_status(jobid, lb_context, 0));
    bool is_collection(
      status && 
      status->jobtype == EDG_WLL_STAT_COLLECTION
    );

    if (command == "jobsubmit") {

      if (is_collection) {

        Info("recovering submit request for collection");
     
        classad::ClassAd const* jdl = ca::evaluate_expression(
          *req.ad,
          "arguments.ad"
        );
        jdl::DAGAd collection(*jdl);  
        JobStatusPtr collection_status(
          job_status(jobid, children | child_status )
        );
        if (!collection_status) {
        
          std::string const id(jobid.toString());
          Warning(id << ": unable to retrieve collection status!");
          Ignore ignore(id);
          ignore(req);
          return;
        }

        if ( // parent node
          is_submitted(status)
          || (
            is_waiting(status) &&
            is_every_children_submitted(collection_status)
          )
        ) {
          boost::function<void()> cleanup(
            boost::bind(&utilities::InputItem::remove_from_input, req.request)
          );
          boost::shared_ptr<SubmitState> state;
          state.reset(
            new SubmitState(
              req.ad,
              command, 
              id,
              sequence_code,
              x509_proxy,
              lb_context,
              cleanup,
              wm,
              events,
              jw_template
            ) 
          );

          events.schedule(SubmitProcessor(state), submit_priority);
          return;
        }

        // collection needs to be recovered node by node
        jdl::DAGAd::node_iterator node_b;
        jdl::DAGAd::node_iterator node_e;
       
        boost::tie(node_b, node_e) = collection.nodes();
        std::vector<RecoverableNodeInfo> recoverable_nodes;
        std::accumulate(
          node_b,
          node_e,
          &recoverable_nodes,
          select_recoverable_nodes(id, lb_context, collection_status)
        );
      
        boost::function<void()> cleanup(
          boost::bind(&utilities::InputItem::remove_from_input, req.request)
        );
        boost::shared_ptr<SubmitState> parent_request_state(
          new SubmitState(
            req.ad,
            command,
            id,
            sequence_code,
            x509_proxy,
            lb_context,
            cleanup,
            wm,
            events,
            jw_template,
            SubmitState::PENDING
          )
        );
        // reschedule parent node...
        events.schedule(SubmitProcessor(parent_request_state), submit_priority);
        // ...and its recoverable children
        std::vector<RecoverableNodeInfo>::const_iterator it_end = recoverable_nodes.end();
        create_submit_request node_sr(events, wm, jw_template, parent_request_state);
        for (
          std::vector<RecoverableNodeInfo>::const_iterator it
            = recoverable_nodes.begin();
          it != it_end;
          ++it)
        {
          events.schedule(
            SubmitProcessor(node_sr(*it)),
            submit_priority
          );
        }

        return;
      } else { // single job submit

        Info("recovering submit request");
     
        try {
          LB_Events events(get_recovery_events(lb_context, jobid));
          if (is_last_done(events, EDG_WLL_DONE_FAILED)) {
            Debug("not recovering submit request, resubmit on the way");
            return; // a resubmit is already present or on the way
          }
          if (!is_waiting(status)) {
            limbo = !lb_context ? true : in_limbo(status, events);
          }
        } catch (LB_QueryFailed&) {
          Debug(id << ": unable to retrieve recovery events!");
          limbo = true;
        } catch (LB_QueryTemporarilyFailed&) {
          Debug(id << ": unable to retrieve recovery events!");
          limbo = true;
        }
      }  
    } else if (command == "jobresubmit") {

      Info("recovering resubmit request");
  
      if (!is_waiting(status)) {
        try { // need to make sure the job is still in charge of the WM
         LB_Events events(get_recovery_events(lb_context, jobid));
          limbo = !lb_context ? true : in_limbo(status, events);
        } catch(LB_QueryFailed&) {
          Debug(id << ": unable to retrieve recovery events!");
          limbo = true;
        } catch(LB_QueryTemporarilyFailed&) {
          Debug(id << ": unable to retrieve recovery events!");
          limbo = true;
        }
      } // else if (is_waiting(status)) { submit = true; } 
    } else if (command == "jobcancel") {

      Info("recovering cancel request");
      boost::function<void()> cleanup(
        boost::bind(&utilities::InputItem::remove_from_input, req.request)
      );
      std::string sequence_code =
        utilities::cancel_command_get_lb_sequence_code(*req.ad);
      try {
        ContextPtr lb_context = create_context(id, x509_proxy, sequence_code);
        boost::shared_ptr<CancelState> state(
          new CancelState(
            id,
            lb_context,
            cleanup,
            wm,
            events
          )
        );
        events.schedule(CancelProcessor(state), cancel_priority);
      } catch (CannotCreateLBContext const& e) {
        Info(e.what() << ": ignoring cancel request for " << id);
      }
      submit = false;
      // do not ignore request
    } else {

      Warning("unrecognized command:" << command);
      submit = false;
      Ignore ignore(id);
      ignore(req);
    }
  } // if (command != "match")

  if (submit) {
    boost::function<void()> cleanup(
      boost::bind(&utilities::InputItem::remove_from_input, req.request)
    ); 
    try {
      SubmitState::checkpoint starting_point = SubmitState::START;
      if (limbo) {
        starting_point = SubmitState::LIMBO;
      }
      boost::shared_ptr<SubmitState> state(
        new SubmitState(
          req.ad,
          command,
          id,
          sequence_code,
          x509_proxy,
          lb_context,
          cleanup,
          wm,
          events,
          jw_template,
          starting_point
        )
      );
      events.schedule(SubmitProcessor(state), submit_priority);
    } catch (CannotCreateLBContext const& e) {
      Info(e.what() << ": ignoring submit request for " << id);
    }
  }
}

class recover
{
  Events& m_events;
  WMReal const& m_wm;
  boost::shared_ptr<std::string> m_jw_template;
public:
  recover(
    Events& events,
    WMReal const& wm,
    boost::shared_ptr<std::string> jw_template
  )
    : m_events(events),
      m_wm(wm),
      m_jw_template(jw_template)
  { }

  void operator()(RequestsToRecover::value_type const& id_requests) const
  {
    std::string const& id = id_requests.id;
    FullRequest const& request_to_recover = id_requests.request;
    Info("recovering " << id);

    request_recovery(
      id,
      request_to_recover,
      m_events,
      m_wm,
      m_jw_template
    );
  }
};

void
recovery(
  boost::shared_ptr<utilities::InputReader> input,
  Events& events_queue,
  WMReal const& wm,
  boost::shared_ptr<std::string> jw_template
) try {
  utilities::InputReader::InputItems requests(input->read());
  RequestsToRecover requests_to_recover;
  std::accumulate( 
    requests.begin(),
    requests.end(),
    &requests_to_recover,
    insert_request 
  );

  std::for_each(
    requests_to_recover.begin(), requests_to_recover.end(),
    recover(events_queue, wm, jw_template)
  );
} catch (std::exception const& e) {
  Error("Recovery: " << e.what() << ". Exiting...");
} catch (...) {
  Error("Recovery: caught unknown exception. Exiting...");
}

}}}} // glite::wms::manager::server
