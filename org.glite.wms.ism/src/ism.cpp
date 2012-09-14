/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// $Id$

#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>

#include <boost/lexical_cast.hpp>
#include <classad_distribution.h>
#include <boost/shared_ptr.hpp>

#include "glite/wms/ism/ism.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/logger/logger_utils.h"

namespace configuration = glite::wms::common::configuration;

namespace {
  int s_active_side = -1;
  boost::shared_ptr<void> s_matching_threads[2];
}

namespace glite {
namespace wms {
namespace ism {

void switch_active_side()
{
  ism_mutex_type::scoped_lock l(get_ism_mutex(ism::ce));
  if (s_active_side == -1) { // first time
    s_active_side = 0;
  } else {
    s_active_side = (s_active_side + 1) % 2;
  }

  Debug("switched active side to ISM " << s_active_side);
}

int dark_side()
{
  ism_mutex_type::scoped_lock l(get_ism_mutex(ism::ce));
  if (s_active_side == -1) { // first time
    return 0;
  } else {
    return (s_active_side + 1) % 2;
  }
}

int active_side()
{
  ism_mutex_type::scoped_lock l(get_ism_mutex(ism::ce));
  if (s_active_side == -1) { // first time
    return 1;
  } else {
    return s_active_side;
  }
}

std::pair<boost::shared_ptr<void>, int> match_on_active_side()
{
  ism_mutex_type::scoped_lock l(get_ism_mutex(ism::ce));
  if (!s_matching_threads[s_active_side]) {
    s_matching_threads[s_active_side].reset(static_cast<int*>(0));
  }
  return std::make_pair(
    s_matching_threads[-1 == s_active_side ? 1 : s_active_side],
    s_active_side);
}

namespace {
  ism_mutex_type* the_ism_mutex[2];
  ism_type* the_ism1[2];
  ism_type* the_ism2[2];
}

ism_type::value_type make_ism_entry(
  std::string const& id, // resource identifier
  int update_time, // update time
  ad_ptr const& ad, // resource descritpion
  update_function_type const& uf, // update function
  int expiry_time // expiry time with default 5*60
)
{
  boost::shared_ptr<boost::mutex> mt(new boost::mutex);
  return std::make_pair(
    id,
    boost::make_tuple(update_time, expiry_time, ad, uf, mt)
  );
}

void set_ism(
  ism_type* ism1,
  ism_type* ism2,
  ism_mutex_type* ism_mutex,
  size_t the_ism_index
)
{
  the_ism1[the_ism_index] = ism1 + the_ism_index;
  the_ism2[the_ism_index] = ism2 + the_ism_index;
  the_ism_mutex[the_ism_index] = ism_mutex + the_ism_index;
}

ism_mutex_type& get_ism_mutex(size_t the_ism_index)
{
  return *the_ism_mutex[the_ism_index];
}

ism_type& get_ism(size_t the_ism_index, int face)
{
  if (face == 0) {
    return *the_ism1[the_ism_index];
  } else {
    return *the_ism2[the_ism_index];
  }
}

ism_type& get_ism(size_t the_ism_index)
{
  ism_mutex_type::scoped_lock l(get_ism_mutex(ism::ce));
  if (s_active_side == 0) {
    return *the_ism1[the_ism_index];
  } else {
    return *the_ism2[the_ism_index];
  }
}

int matching_threads(int side)
{
  return s_matching_threads[side].use_count() - 1;
}

std::ostream&
operator<<(std::ostream& os, ism_type::value_type const& value)
{
  return os << '[' << value.first << "]\n"
    << boost::tuples::get<update_time_entry>(value.second) << '\n'
    << boost::tuples::get<expiry_time_entry>(value.second) << '\n'
    << *boost::tuples::get<ad_ptr_entry>(value.second) << '\n'
    << "[END]";
}

void call_update_ism_entries::operator()()
{
  Debug("ISM updater start");
  _(ce);
  _(se);
  Debug("ISM updater end");
}

void call_update_ism_entries::_(size_t the_ism_index)
{
  time_t current_time = std::time(0);
  ism_type::iterator const e = get_ism(the_ism_index).end();

  for (
    ism_type::iterator pos = get_ism(the_ism_index).begin();
    pos != e;
    ++pos
  ) {
    boost::mutex::scoped_lock l(*boost::tuples::get<4>(pos->second));

    // Check the state of the ClassAd information
    if (boost::tuples::get<ad_ptr_entry>(pos->second)) {
      // If the ClassAd information is not NULL, go on with the updating
      int diff = current_time - boost::tuples::get<update_time_entry>(pos->second);
      // Check if it is expired
      if (diff > boost::tuples::get<expiry_time_entry>(pos->second)) {
        // Check if function object wrapper is not empty
        if (!boost::tuples::get<update_function_entry>(pos->second).empty()) {
          if (!update_ism_entry()(pos->second)) {
            // if the update function returns false we remove the entry
            // only if it has been previously marked as invalid i.e. the entry's 
            // update time is less than 0
            boost::tuples::get<expiry_time_entry>(pos->second) = -1;
          } else {
            // the entry has been updated by the updated function
            boost::tuples::get<update_time_entry>(pos->second) = current_time;
          }
        } else {
          // the function object wrapper is empty
          boost::tuples::get<expiry_time_entry>(pos->second) = -1;
        }
      }
    } else {
      // If the ClassAd information is NULL, remove the entry by ism
      boost::tuples::get<update_time_entry>(pos->second) = -1;
    }

  }
}

bool update_ism_entry::operator()(ism_entry_type entry) 
{
  return boost::tuples::get<update_function_entry>(entry)(
  	boost::tuples::get<expiry_time_entry>(entry), 
	  boost::tuples::get<ad_ptr_entry>(entry));
}

// Returns whether the entry has expired, or not
bool is_expired_ism_entry(const ism_entry_type& entry)
{
  boost::xtime ct;
  boost::xtime_get(&ct, boost::TIME_UTC);
  int diff = ct.sec - boost::tuples::get<update_time_entry>(entry);
  return (diff > boost::tuples::get<expiry_time_entry>(entry));
}

bool is_void_ism_entry(const ism_entry_type& entry)
{
  return (boost::tuples::get<expiry_time_entry>(entry) <= 0);
}

std::string get_ism_dump(void)
{
  configuration::Configuration const* const config = configuration::Configuration::instance();
  assert(config);

  configuration::WMConfiguration const* const wm_config = config->wm();
  assert(wm_config);

  return wm_config->ism_dump();
}

void call_dump_ism_entries::operator()()
{
  Debug("ISM dump start");
  std::string const dump(get_ism_dump());
  std::string const tmp_dump(dump + ".tmp");

  _(ce, std::iostream::ios_base::trunc, tmp_dump);
  _(se, std::iostream::ios_base::app, tmp_dump);

  int res = std::rename(tmp_dump.c_str(), dump.c_str());
  if (res) {
    Warning("Cannot rename ISM dump file (error "
      + boost::lexical_cast<std::string>(res) + ')'
    );
  }
  Debug("ISM dump end");
}

void call_dump_ism_entries::_(
  size_t the_ism_index,
  std::iostream::ios_base::openmode open_mode,
  std::string const& filename)
{
  std::ofstream outf(filename.c_str(), open_mode);
  for (
    ism_type::iterator pos = get_ism(the_ism_index).begin();
    pos!= get_ism(the_ism_index).end();
    ++pos
  ) {
    boost::mutex::scoped_lock l(*boost::tuples::get<4>(pos->second));
    if (boost::tuples::get<2>(pos->second)) {
      classad::ClassAd ad_ism_dump;
      ad_ism_dump.InsertAttr("id", pos->first);
      ad_ism_dump.InsertAttr(
        "update_time",
        boost::tuples::get<update_time_entry>(pos->second)
      );
      ad_ism_dump.InsertAttr(
        "expiry_time",
        boost::tuples::get<expiry_time_entry>(pos->second)
      );
      ad_ism_dump.Insert(
        "info",
        boost::tuples::get<ad_ptr_entry>(pos->second).get()->Copy()
      );
      outf << ad_ism_dump;
    }

    l.unlock();
  }
}

}}}
