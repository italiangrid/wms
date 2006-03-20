// $Id$

#include <iostream>
#include <fstream>
#include <classad_distribution.h>

#include "glite/wms/ism/ism.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"

namespace configuration = glite::wms::common::configuration;

namespace glite {
namespace wms {
namespace ism {

ism_type::value_type make_ism_entry(
  std::string const& id, // resource identifier
  int update_time, // update time
  ad_ptr const& ad, // resource descritpion
  update_function_type const& uf, // update function
  int expiry_time // expiry time with defualt 5*60
)
{
  return std::make_pair(
    id,
    boost::make_tuple(update_time, expiry_time, ad, uf)
  );
}

namespace {

ism_type* the_ism;
ism_mutex_type* the_ism_mutex;

}

void set_ism(ism_type& ism, ism_mutex_type& ism_mutex)
{
  the_ism = &ism;
  the_ism_mutex = &ism_mutex;
}

ism_mutex_type& get_ism_mutex(void)
{
  return *the_ism_mutex;
}

ism_type& get_ism(void)
{
  return *the_ism;
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
  ism_mutex_type::scoped_lock l(get_ism_mutex());
  time_t current_time = std::time(0);

  ism_type::iterator pos=get_ism().begin();
  ism_type::iterator const e=get_ism().end();

  for ( ; pos!=e; ) {

    bool inc_done = false;
    // Check the state of the ClassAd information
    if (boost::tuples::get<ad_ptr_entry>(pos->second) != NULL) {
      // If the ClassAd information is not NULL, go on with the updating
      int diff = current_time - boost::tuples::get<update_time_entry>(pos->second);
      // Check if .. is greater than expiry time
      if (diff > boost::tuples::get<expiry_time_entry>(pos->second)) {
        // Check if function object wrapper is not empty
        if (!boost::tuples::get<update_function_entry>(pos->second).empty()) {
          bool is_updated = update_ism_entry()(pos->second);
          if (!is_updated) {
            // if the update function returns false we remove the entry
            // only if it has been previously marked as invalid i.e. the entry's 
            // update time is less than 0
  	    if (boost::tuples::get<update_time_entry>(pos->second)<0) {
              get_ism().erase(pos++);
              inc_done = true;
            } else {
              boost::tuples::get<update_time_entry>(pos->second) = -1;
            }
	  }
          else {
            boost::tuples::get<update_time_entry>(pos->second) = current_time;
          }
        }
        else {
          // If the function object wrapper is empty, remove the entry
          get_ism().erase(pos++);
          inc_done = true;
        }
      }
    } else {
      // If the ClassAd information is NULL, remove the entry by ism
      get_ism().erase(pos++);
      inc_done = true;
    }
    if (!inc_done) ++pos;
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
  return (boost::tuples::get<expiry_time_entry>(entry)<=0);
}

//get IsmDump file
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
  ism_mutex_type::scoped_lock l(get_ism_mutex());
  std::ofstream             outf(get_ism_dump().c_str());

  for (ism_type::iterator pos=get_ism().begin();
       pos!= get_ism().end(); ++pos) {
    if (boost::tuples::get<2>(pos->second)) {
      classad::ClassAd          ad_ism_dump;

      ad_ism_dump.InsertAttr("id", pos->first);
      ad_ism_dump.InsertAttr("update_time",
        boost::tuples::get<update_time_entry>(pos->second));
      ad_ism_dump.InsertAttr("expiry_time",
        boost::tuples::get<expiry_time_entry>(pos->second));
      ad_ism_dump.Insert("info",
        boost::tuples::get<ad_ptr_entry>(pos->second).get()->Copy());
      outf << ad_ism_dump;
    }
  }
}

}
}
}
