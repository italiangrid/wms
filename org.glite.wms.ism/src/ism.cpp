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

// $Id: ism.cpp,v 1.12.2.3.2.13.2.1.2.4.2.1 2011/09/30 12:46:15 mcecchi Exp $

#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <classad_distribution.h>

#include "glite/wms/ism/ism.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/logger/logger_utils.h"

namespace configuration = glite::wms::common::configuration;

namespace glite {
namespace wms {
namespace ism {

namespace {

int s_active_side = -1;
boost::shared_ptr<void> s_matching_threads[2];

void update(size_t the_ism_index)
{
  ism_mutex_type::scoped_lock l(get_ism_mutex(ce)); // must NOT be the_ism_index
  ism_type& active_side(get_ism(the_ism_index));
  ism_type::iterator it = active_side.begin();
  ism_type::iterator const e = active_side.end();

  for ( ; it != e; ++it) {

    boost::mutex::scoped_lock l(*boost::tuples::get<mutex_entry>(it->second));
    std::time_t const current_time(std::time(0));
    // check the state of the ClassAd information
    if (!boost::tuples::get<keyvalue_info_entry>(it->second).empty()) {
      // If the ClassAd information is not NULL, go on with the updating
      int diff = current_time - boost::tuples::get<update_time_entry>(it->second);
      // check if it is expired
      if (diff > boost::tuples::get<expiry_time_entry>(it->second)) {
        // check if function object wrapper is not empty
        if (!boost::tuples::get<update_function_entry>(it->second).empty()) {
          if (!update_ism_entry()(it->second)) {
            // if the update function returns false we remove the entry
            // only if it has been previously marked as invalid i.e. the entry's 
            // update time is less than 0
            boost::tuples::get<expiry_time_entry>(it->second) = -1;
          } else { // the entry has been updated by the updated function
            boost::tuples::get<update_time_entry>(it->second) = current_time;
          }
        } else { // the function object wrapper is empty
          boost::tuples::get<expiry_time_entry>(it->second) = -1;
        }
      }
    } else { // if the ClassAd information is NULL, remove the entry by ism
      boost::tuples::get<update_time_entry>(it->second) = -1;
    }
  }
}

void dump(
  size_t the_ism_index,
  std::iostream::ios_base::openmode open_mode,
  std::string const& filename)
{
  std::ofstream outf(filename.c_str(), open_mode);

  ism_mutex_type::scoped_lock l(get_ism_mutex(the_ism_index));
  ism_type& active_side(get_ism(the_ism_index));
  ism_type::iterator it = active_side.begin();
  ism_type::iterator const e = active_side.end();

  for ( ; it != e; ++it) {

    outf << "id = " << it->first << '\n';
    boost::unordered_map<
      boost::flyweight<std::string>,
      boost::flyweight<std::string>,
      flyweight_hash
    > keyvalue_info(boost::tuples::get<keyvalue_info_entry>(it->second));
    for (
      boost::unordered_map<
        boost::flyweight<std::string>,
        boost::flyweight<std::string>,
        flyweight_hash>::iterator iter(keyvalue_info.begin());
      iter != keyvalue_info.end();
      ++iter
    ) {
      outf << iter->first << " = " << keyvalue_info[iter->first] << '\n';
    }
    outf << "update_time = " << boost::tuples::get<update_time_entry>(it->second) << '\n';
    outf << "expiry_time = " << boost::tuples::get<expiry_time_entry>(it->second) << "\n\n";
  }
}

}

void switch_active_side()
{
  ism_mutex_type::scoped_lock l(get_ism_mutex(ce)); // must NOT be the_ism_index
  if (s_active_side == -1) { // first time
    s_active_side = 0;
  } else {
    s_active_side = (s_active_side + 1) % 2;
  }

  Debug("switched active side to ISM " << s_active_side);
}

int dark_side()
{
  ism_mutex_type::scoped_lock l(get_ism_mutex(ce)); // must NOT be the_ism_index
  if (s_active_side == -1) { // first time
    return 0;
  } else {
    return (s_active_side + 1) % 2;
  }
}

int active_side()
{
  ism_mutex_type::scoped_lock l(get_ism_mutex(ce)); // must NOT be the_ism_index
  if (s_active_side == -1) { // first time
    return 1;
  } else {
    return s_active_side;
  }
}

std::pair<boost::shared_ptr<void>, int> match_on_active_side()
{
  ism_mutex_type::scoped_lock l(get_ism_mutex(ce)); // must NOT be the_ism_index
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
  boost::unordered_map<
    boost::flyweight<std::string>,
    boost::flyweight<std::string>,
    flyweight_hash
  > const& ad_info,
  int expiry_time, // expiry time with default 5 * 60
  update_function_type const& uf // update function
)
{
  boost::shared_ptr<boost::mutex> mt(new boost::mutex);
  return std::make_pair(
    boost::flyweight<std::string>(id),
    boost::make_tuple(update_time, expiry_time, ad_info, uf, mt)
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
  ism_mutex_type::scoped_lock l(get_ism_mutex(ce)); // must NOT be the_ism_index
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
  std::string keyvalue_info_str;
  boost::unordered_map<
    boost::flyweight<std::string>,
    boost::flyweight<std::string>,
    flyweight_hash
  > keyvalue_info(boost::tuples::get<keyvalue_info_entry>(value.second));
  for (
    boost::unordered_map<
      boost::flyweight<std::string>,
      boost::flyweight<std::string>,
      flyweight_hash>::iterator iter(keyvalue_info.begin());
    iter != keyvalue_info.end();
    ++iter
  ) {
    keyvalue_info_str += std::string(value.first) + std::string(" = ") +
      std::string(keyvalue_info[iter->first]) + '\n';
  }

  return os << '[' << value.first << "]\n"
    << boost::tuples::get<update_time_entry>(value.second) << '\n'
    << boost::tuples::get<expiry_time_entry>(value.second) << '\n'
    << keyvalue_info_str << '\n'
    << "[END]";
}

void call_update_ism_entries::operator()()
{
  Debug("ISM updater start");
  update(ce);
  update(se);
  Debug("ISM updater end");
}

bool update_ism_entry::operator()(ism_entry_type entry) 
{
  return boost::tuples::get<update_function_entry>(entry)(
    boost::tuples::get<expiry_time_entry>(entry), 
    boost::tuples::get<keyvalue_info_entry>(entry)
  );
}

// returns whether the entry has expired, or not
bool is_expired_ism_entry(const ism_entry_type& entry)
{
  boost::xtime ct;
  boost::xtime_get(&ct, boost::TIME_UTC);
  int diff = ct.sec - boost::tuples::get<update_time_entry>(entry);
  return (diff > boost::tuples::get<expiry_time_entry>(entry));
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
  std::string const dump_file(get_ism_dump());
  std::string const tmp_dump_file(dump_file + ".tmp");

  dump(ce, std::iostream::ios_base::trunc, tmp_dump_file);
  dump(se, std::iostream::ios_base::app, tmp_dump_file);

  int res = std::rename(tmp_dump_file.c_str(), dump_file.c_str());
  if (res) {
    Warning("Cannot rename ISM dump file " + tmp_dump_file + " to " + dump_file + " (error "
      + boost::lexical_cast<std::string>(res) + ')'
    );
  }
  Debug("ISM dump end");
}

}}}
