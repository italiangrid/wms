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

// Authors:
//          Marco Cecchi <marco.cecchi@cnaf.infn.it>
//          Salvatore Monforte <salvatore.monforte@ct.infn.it>
//          Elisabetta Ronchieri <elisabetta.ronchieri@cnaf.infn.it>

#ifndef GLITE_WMS_ISM_ISM_H
#define GLITE_WMS_ISM_ISM_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/flyweight.hpp>
#include <boost/unordered_map.hpp>

namespace classad {
   class ClassAd;
}

namespace glite {
namespace wms {
namespace ism {

enum {
  update_time_entry = 0,
  expiry_time_entry,
  keyvalue_info_entry,
  update_function_entry,
  mutex_entry
};

enum {
  ce = 0,
  se
};

// resource description
typedef boost::shared_ptr<classad::ClassAd> ad_ptr;

struct flyweight_hash
{
   std::size_t operator()(boost::flyweight<std::string> const& e) const
   {
      boost::hash<std::string> hasher;
      std::string const& str = e.get();
      return hasher(str);
   }
};

// update function
typedef boost::function<bool(
  int&,
  boost::unordered_map<
    boost::flyweight<std::string>,
    boost::flyweight<std::string>,
    flyweight_hash
  >
)> update_function_type;

typedef boost::tuple<
  int, // update time
  int, // expiry  "
  boost::unordered_map<
    boost::flyweight<std::string>,
    boost::flyweight<std::string>,
    flyweight_hash
  >, // ad description in terms of key/value pairs. this is ready to be used
     // in a flyweight pattern or any distributed cache
  update_function_type, // update function
  boost::shared_ptr<boost::mutex::mutex> // one different mutex for each entry
                                         // mutex is non-copyable
> ism_entry_type;

// 1. resource identifier
// 2. ism entry type
typedef boost::unordered_map<
  boost::flyweight<std::string>,
  ism_entry_type,
  flyweight_hash
  //, , __gnu_cxx::malloc_allocator<ism_entry_type>
> ism_type;

// type specification for the mutex in ism
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
  boost::unordered_map<
    boost::flyweight<std::string>,
    boost::flyweight<std::string>,
    flyweight_hash  
  > const& ad_info,
  int expiry_time = 300,
  update_function_type const& uf = update_function_type()
);

std::ostream& operator<<(std::ostream& os, ism_type::value_type const& value);

struct call_update_ism_entries
{
  void operator()();
};

struct update_ism_entry 
{
  bool operator()(ism_entry_type entry);
};

bool is_expired_ism_entry(const ism_entry_type& entry);

std::string get_ism_dump(void);

struct call_dump_ism_entries
{
  void operator()();
};

} // ism
} // wms
} // glite

#endif // GLITE_WMS_ISM_ISM_H
