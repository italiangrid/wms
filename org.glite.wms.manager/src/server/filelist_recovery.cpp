// File: filelist_recovery.cpp
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

#include "filelist_recovery.h"
#include <algorithm>
#include <boost/regex.hpp>
#include "lb_utils.h"
#include "glite/wms/common/utilities/scope_guard.h"
#include "glite/wms/common/utilities/wm_commands.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wms/common/logger/logger_utils.h"

namespace utilities = glite::wms::common::utilities;
namespace jobid = glite::wmsutils::jobid;
namespace ca = glite::wmsutils::classads;

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {

typedef std::vector<
  boost::tuple<
    std::string,                // command
    extractor_type::iterator,   // iterator
    ClassAdPtr                  // already parsed command classad
  >
> requests_for_id_type;         // requests for a specific id

typedef boost::tuple<
  std::string,                  // jobid
  requests_for_id_type
> id_requests_type;

typedef std::vector<id_requests_type> id_to_requests_type;

class id_equals
{
  std::string m_id;
public:
  id_equals(std::string const& id)
    : m_id(id)
  {
  }
  bool operator()(id_to_requests_type::value_type const& v) const
  {
    std::string const& id = v.get<0>();
    return m_id == id;
  }
};

bool is_cancel(requests_for_id_type::value_type const& v)
{
  std::string const& command = v.get<0>();
  return command == "jobcancel";
}

void catalog_requests_by_id(
  ExtractorPtr extractor,
  requests_type const& requests,
  id_to_requests_type& id_to_requests
)
{
  requests_type::const_iterator it = requests.begin();
  requests_type::const_iterator const end = requests.end();
  for ( ; it != end; ++it) {

    extractor_type::iterator const request_it = *it;
    boost::function<void()> cleanup(FLCleanUp(extractor, request_it));
    // if the request is not valid, cleanup automatically
    utilities::scope_guard cleanup_guard(cleanup);

    std::string const command_ad_str(*request_it);

    try {

      ClassAdPtr command_ad(ca::parse_classad(command_ad_str));

      std::string command;
      jobid::JobId id;
      std::string sequence_code;
      std::string x509_proxy;
      if (utilities::command_is_valid(*command_ad)) {
        boost::tie(command, id, sequence_code, x509_proxy)
          = parse_request(*command_ad);
      } else {
        Info("Invalid command: "
          << *command_ad <<
          " (doesn't match requirements...)"
        );
        continue;
      }

      id_to_requests_type::iterator ite(
        std::find_if(
          id_to_requests.begin(),
          id_to_requests.end(),
          id_equals(id.toString())
        )
      );

      if (ite != id_to_requests.end()) {
        ite->get<1>().push_back(
          boost::make_tuple(command, request_it, command_ad)
        );
      } else {
        id_to_requests.push_back(
          boost::make_tuple(
            id.toString(),
            requests_for_id_type()
          )
        );
        id_to_requests.back().get<1>().push_back(
          boost::make_tuple(command, request_it, command_ad)
        );
      }

      cleanup_guard.dismiss();

    } catch (ca::ClassAdError& e) {
      Info(e.what());
    } catch (InvalidRequest& e) {
      Info(e.str());
    }
  }
}

class clean_ignore
{
  ExtractorPtr m_extractor;
  std::string m_id;
public:
  clean_ignore(ExtractorPtr extractor, std::string const& id)
    : m_extractor(extractor), m_id(id)
  {
  }
  void operator()(requests_for_id_type::value_type const& v) const
  {
    Debug("ignoring " << v.get<0>() << " request for " << m_id);
    FLCleanUp(m_extractor, v.get<1>())();
  }
};

void single_request_recovery(
  id_requests_type const& id_requests,
  ExtractorPtr extractor,
  TaskQueue& tq
)
{
  std::string const& id = id_requests.get<0>();
  requests_for_id_type const& requests_for_id = id_requests.get<1>();
  assert(requests_for_id.size() == 1);
  requests_for_id_type::value_type const& req = requests_for_id.front();
  std::string const& command = req.get<0>();
 
  JobStatusPtr status(job_status(jobid::JobId(id)));

  bool valid = true;
  if (command == "match" && !status) {
    Info("matching");
  } else if (command == "jobsubmit" && is_waiting(status)) {
    Info("submitting");
  } else if (command == "jobcancel" && !is_cancelled(status)) {
    Info("cancelling");
  } else if (command == "jobresubmit" && is_waiting(status)) {
    Info("resubmitting");
  } else {
    assert(false && "invalid command");
    valid = false;
  }

  if (valid) {
    extractor_type::iterator const& request_it = req.get<1>();
    boost::function<void()> cleanup(FLCleanUp(extractor, request_it));
    classad::ClassAd& command_ad(*req.get<2>());
    RequestPtr request(new Request(command_ad, command, id, cleanup));
    tq.insert(std::make_pair(id, request));
  } else {
    clean_ignore(extractor, id)(req);
  }
}

std::string summary(requests_for_id_type const& requests_for_id)
{
  std::string result;

  requests_for_id_type::const_iterator b = requests_for_id.begin();
  requests_for_id_type::const_iterator const e = requests_for_id.end();
  for ( ; b != e; ++b) {
    std::string const& command = b->get<0>();
    result += toupper(command[0]);
  }

  return result;
}

void multiple_request_recovery(
  id_requests_type const& id_requests,
  ExtractorPtr extractor,
  TaskQueue& tq
)
{
  std::string const& id = id_requests.get<0>();
  requests_for_id_type const& requests_for_id = id_requests.get<1>();
  assert(requests_for_id.size() > 1);

  JobStatusPtr status(job_status(jobid::JobId(id)));

  std::string summary(summary(requests_for_id));
  std::string status_summary(" (status ");
  if (status) {
    status_summary += boost::lexical_cast<std::string>(status->state) + ')';
  } else {
    status_summary += "not available)";
  }
  Info("multiple requests [" << summary << "] for " << id << status_summary);

  // invalid patterns
  boost::regex const nonmatch_match_nonmatch_re("[^M]*M[^M]*");
  boost::regex const nonsubmit_submit_re("[^S]+S.*");
  boost::regex const more_submits_re("S.*S.*");

  // possible patterns
  boost::regex const more_matches_re("M+");
  boost::regex const a_cancel_re(".*C.*");
  boost::regex const no_cancel_re("[^C]*");

  if (boost::regex_match(summary, nonmatch_match_nonmatch_re)
      || boost::regex_match(summary, nonsubmit_submit_re)
      || boost::regex_match(summary, more_submits_re)
     ) {

    Info("invalid pattern; ignoring all requests");
    std::for_each(
      requests_for_id.begin(),
      requests_for_id.end(),
      clean_ignore(extractor, id)
    );

  } else if (boost::regex_match(summary, more_matches_re) && !status) {

    Info("matching");

    requests_for_id_type::value_type const& match_req(requests_for_id.back());
    
    extractor_type::iterator request_it = match_req.get<1>();
    boost::function<void()> cleanup(FLCleanUp(extractor, request_it));
    classad::ClassAd& command_ad(*match_req.get<2>());
    RequestPtr request(new Request(command_ad, "match", id, cleanup));
    tq.insert(std::make_pair(id, request));
    
    std::for_each(
      requests_for_id.begin(),
      boost::prior(requests_for_id.end()),
      clean_ignore(extractor, id)
    );

  } else if (boost::regex_match(summary, a_cancel_re)
             && !is_cancelled(status)) {

    Info("cancelling");

    // find the request corresponding to a cancel
    requests_for_id_type::const_iterator const cancel_it(
      std::find_if(requests_for_id.begin(), requests_for_id.end(), is_cancel)
    );
    assert(cancel_it != requests_for_id.end());
    extractor_type::iterator request_it = cancel_it->get<1>();
    boost::function<void()> cleanup(FLCleanUp(extractor, request_it));
    classad::ClassAd& command_ad(*cancel_it->get<2>());
    RequestPtr request(new Request(command_ad, "jobcancel", id, cleanup));
    tq.insert(std::make_pair(id, request));

    std::for_each(
      requests_for_id.begin(),
      cancel_it,
      clean_ignore(extractor, id)
    );
    std::for_each(
      boost::next(cancel_it),
      requests_for_id.end(),
      clean_ignore(extractor, id)
    );

  } else if (boost::regex_match(summary, no_cancel_re)
             && is_waiting(status)) {

    // take the last request, which must be a resubmit
    Info("resubmitting");

    requests_for_id_type::value_type const& last_resubmit(
      requests_for_id.back()
    );
    extractor_type::iterator request_it = last_resubmit.get<1>();
    boost::function<void()> cleanup(FLCleanUp(extractor, request_it));
    classad::ClassAd& command_ad(*last_resubmit.get<2>());
    RequestPtr request(new Request(command_ad, "jobresubmit", id, cleanup));
    tq.insert(std::make_pair(id, request));

    std::for_each(
      requests_for_id.begin(),
      boost::prior(requests_for_id.end()),
      clean_ignore(extractor, id)
    );

  } else {

    Info("invalid pattern; ignoring all requests");
    std::for_each(
      requests_for_id.begin(),
      requests_for_id.end(),
      clean_ignore(extractor, id)
    );

  }
}

class recover
{
  ExtractorPtr m_extractor;
  TaskQueue* m_tq;
public:
  recover(ExtractorPtr extractor, TaskQueue& tq)
    : m_extractor(extractor), m_tq(&tq)
  {
  }
  void operator()(id_to_requests_type::value_type const& id_requests) const
  {
    std::string const& id = id_requests.get<0>();
    requests_for_id_type const& requests_for_id = id_requests.get<1>();
    assert(!requests_for_id.empty());
    Info("recovering " << id);
    if (requests_for_id.size() == 1) {
      single_request_recovery(id_requests, m_extractor, *m_tq);
    } else {
      multiple_request_recovery(id_requests, m_extractor, *m_tq);
    }
  }


};


}

void
recovery(ExtractorPtr extractor, TaskQueue& tq)
{
  requests_type requests(extractor->get_all_available());
  id_to_requests_type id_to_requests;
  catalog_requests_by_id(extractor, requests, id_to_requests);

  std::for_each(
    id_to_requests.begin(),
    id_to_requests.end(),
    recover(extractor, tq)
  );
}

}}}} // glite::wms::manager::server
