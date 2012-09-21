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

// File: RequestStateMachine.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id: RequestStateMachine.cpp,v 1.4.2.2.8.2.4.2 2010/04/08 13:52:15 mcecchi Exp $

#include <dlfcn.h>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <classad_distribution.h>
#include "RequestStateMachine.h"
#include "Helper.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wmsutils/classads/classad_utils.h"

namespace utils = glite::wmsutils::classads;

namespace glite {
namespace wms {
namespace helper {

typedef RequestStateMachine::state_type state_type;
typedef RequestStateMachine::expression_type expression_type;
typedef RequestStateMachine::initial_states_type initial_states_type;
typedef RequestStateMachine::transition_table_type transition_table_type;
typedef RequestStateMachine::final_states_type final_states_type;

namespace {

const std::string state_machine_reqs("[requirements="
                                     "islist(other.initial_states)"
                                     "&& islist(other.transition_table)"
                                     "&& islist(other.final_states)]");

void init_initial_states(initial_states_type& states,
                         classad::ExprList const* l)
{
  states.insert(std::make_pair(state_type("SimpleLogicalJob"),
                               expression_type("other.type==\"job\"")));
  states.insert(std::make_pair(state_type("DAGJob"),
                               expression_type("other.type==\"dag\"")));
}

class CannotLoadDynamicLibrary {};

class DynamicLibrary
{
  void* m_handle;

public:
  DynamicLibrary(std::string& name)
    : m_handle(dlopen(name.c_str(), RTLD_GLOBAL))
  {
    if (!m_handle) {
      throw CannotLoadDynamicLibrary();
    }
  }
  ~DynamicLibrary()
  {
    dlclose(m_handle);
  }  
};

std::vector<boost::shared_ptr<DynamicLibrary> > f_shared_libs;

void load_helpers()
{
  // the following map will be created out of the configuration file
  typedef std::map<std::string, std::string> Helper2LibMap;
  Helper2LibMap helper2lib;
  helper2lib.insert(std::make_pair("BrokerHelper", "libbroker_helper.so"));
  helper2lib.insert(std::make_pair("JobAdapterHelper", "libjobadapter_helper.so"));
  helper2lib.insert(std::make_pair("DAGManHelper", "libdagman_helper.so"));

  for (Helper2LibMap::iterator h = helper2lib.begin();
       h != helper2lib.end();
       ++h)
    try {
      boost::shared_ptr<DynamicLibrary> l(new DynamicLibrary(h->second));
      f_shared_libs.push_back(l);
    } catch (CannotLoadDynamicLibrary&) {
      Error("Cannot load helper library " << h->first);
    }
}

void unload_helpers()
{
  f_shared_libs.clear();
}

void init_transition_table(transition_table_type& table,
                           classad::ExprList const* l)
{
  table.insert(
    std::make_pair(
      state_type("SimpleLogicalJob"),
      boost::make_tuple(
        std::string("BrokerHelper"),
        state_type("SimplePhysicalJob")
      )
    )
  );
  table.insert(
    std::make_pair(
      state_type("SimplePhysicalJob"),
      boost::make_tuple(
        std::string("JobAdapterHelper"),
        state_type("AdaptedJob")
      )
    )
  );
  table.insert(
    std::make_pair(
      state_type("DAGJob"),
      boost::make_tuple(
        std::string("DAGManHelper"),
        state_type("AdaptedJob")
      )
    )
  );
}

void init_final_states(final_states_type& states,
                       classad::ExprList const* l)
{
  states.insert(state_type("AdaptedJob"));
}

bool state_is_final(state_type const& state,
                    final_states_type const& final_states)
{
  return final_states.find(state) != final_states.end();
}

}

RequestStateMachine::RequestStateMachine(classad::ClassAd const* config)
{
  init_initial_states(m_initial_states, 0);
  init_transition_table(m_transition_table, 0);
  //  load_helpers();
  init_final_states(m_final_states, 0);
}

RequestStateMachine::~RequestStateMachine()
{
  //  unload_helpers();
}

class MatchAd: public std::unary_function<initial_states_type::value_type, bool>
{
  classad::ClassAd const* m_ad;
public:
  MatchAd(classad::ClassAd const* ad): m_ad(ad) {}
  result_type operator()(argument_type const& v) const
  {
    expression_type expr = "[requirements=" + v.second + "]";
    boost::scoped_ptr<classad::ClassAd> rhs(utils::parse_classad(expr));
    
    return utils::left_matches_right(*m_ad, *rhs);
  }
};

void
RequestStateMachine::start(classad::ClassAd const* ad)
{
  initial_states_type::const_iterator it = find_if(m_initial_states.begin(),
                                                   m_initial_states.end(),
                                                   MatchAd(ad));
  if (it == m_initial_states.end()) {
    throw NoValidState();
  }
  m_current_state = it->first;
}

classad::ClassAd*
RequestStateMachine::next_step(
  classad::ClassAd const* ad,
  boost::shared_ptr<std::string> jw_template
)
{
  assert(ad != 0);

  classad::ClassAd* result = 0;

  transition_table_type::const_iterator it = m_transition_table.find(m_current_state);
  if (it == m_transition_table.end()) {
    throw NoValidState();
  }

  std::string helper;
  state_type next_state;
  boost::tie(helper, next_state) = it->second;

  result = Helper(helper).resolve(ad, jw_template);
  m_current_state = next_state;

  assert(result != 0);

  return result;
}

RequestStateMachine::operator void*() const
{
  return state_is_final(m_current_state, m_final_states) ? (void*)0 : (void*)(-1);
}

int
RequestStateMachine::operator!() const
{
  return state_is_final(m_current_state, m_final_states);
}

}}} // glite::wms::helper
