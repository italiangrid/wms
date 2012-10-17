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

// File: ISM.h
// Authors: Elisabetta Ronchieri <elisabetta.ronchieri@cnaf.infn.it>
//          Marco Cecchi <marco.cecchi@cnaf.infn.it>
//
// $Id: ism.h,v 1.10.2.2.2.5.2.1.2.3.2.1 2011/09/30 12:46:14 mcecchi Exp $

#ifndef GLITE_WMS_ISM_ISM_H
#define GLITE_WMS_ISM_ISM_H

#include <map>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/xtime.hpp>
//#include <boost/flyweight.hpp>

namespace classad {
class ClassAd;
}

namespace glite {
namespace wms {
namespace ism {

enum {
  id_entry = 0,
  update_time_entry,
  expiry_time_entry,
  ad_info_entry,
  update_function_entry,
  mutex_entry
};

enum {
  ce = 0,
  se
};

typedef boost::function<bool(
  int&, boost::shared_ptr<std::map<std::string, std::string> >)
> update_function_type;

typedef boost::tuple<
  std::string, // resource identifier
  int, // update time
  int, // expiry "
  boost::shared_ptr<
    std::map<std::string, std::string>
  >, // key/value resource description, READY TO BE 'MEMCACHED'
  update_function_type, // update function
  boost::shared_ptr<boost::mutex::mutex> // one different mutex for each entry, needed for updating the live side
> ism_entry_type;

typedef std::vector<ism_entry_type> ism_type; // as simple as that...
typedef boost::recursive_mutex ism_mutex_type;

void set_ism(
  ism_type* the_ism1,
  ism_type* the_ism2,
  ism_mutex_type* the_ism_mutex, // one is enough
  size_t the_ism_index
);
ism_type& get_ism(size_t the_ism_index);
ism_type& get_ism(size_t the_ism_index, int side);
ism_mutex_type& get_ism_mutex(size_t the_ism_index);
void switch_active_side();
int dark_side();
int active_side();
std::pair<boost::shared_ptr<void>, int> match_on_active_side();
int matching_threads(int side);

ism_type::value_type make_ism_entry(
  std::string const& id,
  int update_time,
  boost::shared_ptr<
    std::map<std::string, std::string>
  > ad_info,
  int expiry_time = 300,
  update_function_type const& uf = update_function_type()
);

std::ostream& operator<<(std::ostream& os, ism_type::value_type const& value);

class call_update_ism_entries
{
  void _(size_t);
public:
  void operator()();
};

struct update_ism_entry 
{
  bool operator()(ism_entry_type entry);
};

bool is_expired_ism_entry(const ism_entry_type& entry);

// Returns whether the ism entry is assigned an expiry_time less or equal to 0, or not
bool is_void_ism_entry(const ism_entry_type& entry);

} // ism
} // wms
} // glite

#endif // GLITE_WMS_ISM_ISM_H
