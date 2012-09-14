// File: process_submit.cpp
// Author: Francesco Giacomini

// $Id$

#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <boost/regex.hpp>
#include <boost/scoped_ptr.hpp>
#include <classad_distribution.h>
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/wms/broker/match.h"
#include "glite/wmsutils/classads/classad_utils.h"

namespace jdl = glite::jdl;
namespace cu = glite::wmsutils::classads;

namespace glite {
namespace wms {
namespace manager {

namespace {
void process_submit_impl(classad::ClassAd& ad);
void process_submit_impl(classad::ClassAd& ad, std::string const& ce_id);
}

void process_submit(classad::ClassAd& ad)
{
  bool submit_to_exists = false;
  std::string const ce_id = jdl::get_submit_to(ad, submit_to_exists);
  if (submit_to_exists) {
    process_submit_impl(ad, ce_id);
  } else {
    process_submit_impl(ad);
  }
}

namespace {

void process_submit_impl(classad::ClassAd& ad)
{
  std::vector<std::string> previous_matches;
  {
    bool no_throw;
    jdl::get_edg_previous_matches(ad, previous_matches, no_throw);
  }

  broker::MatchTable matches;
  broker::DataInfo data_info;
  match(ad, matches, data_info, previous_matches);

  assert(!matches.empty());

  broker::MatchTable::const_iterator best_match_it;

  bool fuzzy_rank_exists = false;
  if (jdl::get_fuzzy_rank(ad, fuzzy_rank_exists) && fuzzy_rank_exists) {
    bool fuzzy_factor_exists = false;
    double fuzzy_factor = jdl::get_fuzzy_factor(ad, fuzzy_factor_exists);
    if (!fuzzy_factor_exists) {
      fuzzy_factor = 1;
    }
    best_match_it = broker::FuzzySelector(fuzzy_factor)(matches);
  } else {
    best_match_it = broker::MaxRankSelector()(matches);
  }

  classad::ClassAd const& ce_ad = *best_match_it->ce_ad;

  boost::scoped_ptr<classad::ClassAd> brokerinfo(
    create_brokerinfo(ad, ce_ad, data_info)
  );

  // TODO: can the get function throw?
  std::string const brokerinfo_file(
    jdl::get_input_sandbox_path(ad) + "/.BrokerInfo"
  );
  std::string const tmp_file(brokerinfo_file + ".tmp");
  std::ofstream brokerinfo_os(tmp_file.c_str());
  brokerinfo_os << *brokerinfo << std::endl;

  assert(brokerinfo_os);
  brokerinfo_os.close();

  int e = ::rename(tmp_file.c_str(), brokerinfo_file.c_str());
  assert(e == 0);

  bool no_throw;
  std::vector<std::string> isb;
  jdl::get_input_sandbox(ad, isb, no_throw);
  bool isb_base_uri_exists = false;
  std::string const isb_base_uri(
    jdl::get_wmpinput_sandbox_base_uri(ad, isb_base_uri_exists)
  );
  if (isb_base_uri_exists) {
    isb.push_back(isb_base_uri + "/input/.BrokerInfo");
  } else {
    isb.push_back(".BrokerInfo");
  }

  jdl::set_input_sandbox(ad, isb);
  // TODO: can evaluate throw?
  std::string const ce_id(cu::evaluate_attribute(ce_ad, "GlueCEUniqueID"));
  jdl::set_ce_id(ad, ce_id);

  // TODO: flatten requirements

  jdl::set_globus_resource_contact_string(
    ad,
    jdl::get_globus_resource_contact_string(ce_ad)
  );
  jdl::set_queue_name(ad, jdl::get_queue_name(ce_ad));
  jdl::set_lrms_type(ad, jdl::get_lrms_type(ce_ad));
  jdl::set_ce_id(ad, jdl::get_ce_id(ce_ad));
}

void process_submit_impl(classad::ClassAd& ad, std::string const& ce_id)
{
  static boost::regex const re("(.+/[^\\-]+-([^\\-]+))-(.+)");
  boost::smatch pieces;
  if (boost::regex_match(ce_id, pieces, re)) {
    std::string const contact_string(pieces[1].first, pieces[1].second);
    std::string const lrms_type(pieces[2].first, pieces[2].second);
    std::string const queue_name(pieces[3].first, pieces[3].second);

    jdl::set_globus_resource_contact_string(ad, contact_string);
    jdl::set_queue_name(ad, queue_name);
    bool lrms_type_exists = false;
    jdl::get_lrms_type(ad, lrms_type_exists);
    if (!lrms_type_exists) {
      jdl::set_lrms_type(ad, lrms_type);
    }
    jdl::set_ce_id(ad, ce_id);
  } else {
    assert(!"invalid SubmitTo");
  }
}

}

}}}
