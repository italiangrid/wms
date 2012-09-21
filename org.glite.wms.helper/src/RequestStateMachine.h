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

// File: RequestStateMachine.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id: RequestStateMachine.h,v 1.1.1.1.28.4.2.2.4.2 2010/04/08 13:52:15 mcecchi Exp $

#ifndef GLITE_WMS_HELPER_REQUESTSTATEMACHINE_H
#define GLITE_WMS_HELPER_REQUESTSTATEMACHINE_H

#include <map>
#include <set>
#include <string>

#include <boost/utility.hpp>
#include <boost/tuple/tuple.hpp>

namespace classad {
class ClassAd;
}

namespace glite {
namespace wms {
namespace helper {

class NoValidState: public std::exception
{
  char const* what() const throw()
  {
    return "No valid state transition for the job";
  }
};

class RequestStateMachine: boost::noncopyable
{
public:
  typedef std::string state_type;
  typedef std::string expression_type;
  typedef std::map<state_type, expression_type> initial_states_type;
  typedef std::map<state_type, boost::tuple<std::string, state_type> > transition_table_type;
  typedef std::set<state_type> final_states_type;

  struct AdNotValid {};

private:
  initial_states_type m_initial_states;
  transition_table_type m_transition_table;
  final_states_type m_final_states;
  state_type m_current_state;

public:
  // throws ADNotValid if state_machine_ad does not represent a valid state machine
  RequestStateMachine(classad::ClassAd const* state_machine_ad);
  ~RequestStateMachine();

  void start(classad::ClassAd const* ad);
  classad::ClassAd* next_step(
    classad::ClassAd const* ad,
    boost::shared_ptr<std::string> jw_template
  );
  operator void*() const;
  int operator!() const;

};

}}} // glite::wms::helper

#endif

// Local Variables:
// mode: c++
// End:
