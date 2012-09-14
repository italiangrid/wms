// File: resolver.cpp
// Author: Francesco Giacomini

// $Id$

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <classad_distribution.h>
#include "glite/wms/broker/resolver.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include <cassert>

namespace utils = glite::wmsutils::classads;

namespace glite {
namespace wms {
namespace broker {

namespace {

typedef std::string State;
typedef std::string Expression;
typedef std::map<State, Expression> InitialStates;
typedef std::map<
  State,
  boost::tuple<std::string, State>
> TransitionTable;
typedef std::set<State> FinalStates;

std::string const state_machine_reqs(
  "[requirements="
  "islist(other.initial_states)"
  "&& islist(other.transition_table)"
  "&& islist(other.final_states)]"
);

void init_initial_states(InitialStates& states)
{
  states.insert(
    std::make_pair(
      State("SimpleLogicalJob"),
      Expression("other.type==\"job\"")
    )
  );
  states.insert(
    std::make_pair(
      State("DAGJob"),
      Expression("other.type==\"dag\"")
    )
  );
}

void init_transition_table(TransitionTable& table)
{
  table.insert(
    std::make_pair(
      State("SimpleLogicalJob"),
      boost::make_tuple(
        std::string("BrokerHelper"),
        State("SimplePhysicalJob")
      )
    )
  );
  table.insert(
    std::make_pair(
      State("SimplePhysicalJob"),
      boost::make_tuple(
        std::string("JobAdapterHelper"),
        State("AdaptedJob")
      )
    )
  );
  table.insert(
    std::make_pair(
      State("DAGJob"),
      boost::make_tuple(
        std::string("DAGManHelper"),
        State("AdaptedJob")
      )
    )
  );
}

void init_final_states(FinalStates& states)
{
  states.insert(State("AdaptedJob"));
}

bool state_is_final(State const& state, FinalStates const& final_states)
{
  return final_states.find(state) != final_states.end();
}

}

struct Resolver::Impl
{
  HelperRegistry const* registry;
  InitialStates initial_states;
  FinalStates final_states;
  TransitionTable transition_table;
};

Resolver::Resolver(HelperRegistry const& registry)
  : m_impl(new Impl)
{
  m_impl->registry = &registry;
  
  init_initial_states(m_impl->initial_states);
  init_transition_table(m_impl->transition_table);
  init_final_states(m_impl->final_states);
}

class MatchAd: public std::unary_function<InitialStates::value_type, bool>
{
  boost::shared_ptr<classad::ClassAd> m_ad;
public:
  MatchAd(boost::shared_ptr<classad::ClassAd> ad): m_ad(ad) {}
  result_type operator()(argument_type const& v) const
  {
    Expression expr = "[requirements=" + v.second + "]";
    boost::scoped_ptr<classad::ClassAd> rhs(utils::parse_classad(expr));
    
    return utils::left_matches_right(*m_ad, *rhs);
  }
};

ClassAdPtr Resolver::operator()(ClassAdPtr ad) const
{
  ClassAdPtr result;

  InitialStates::const_iterator it(
    std::find_if(
      m_impl->initial_states.begin(),
      m_impl->initial_states.end(),
      MatchAd(ad)
    )
  );
  if (it == m_impl->initial_states.end()) {
    assert(!"no valid initial state");
  }
  State current_state = it->first;

  while (!state_is_final(current_state, m_impl->final_states)) {
    
    TransitionTable::const_iterator it(
      m_impl->transition_table.find(current_state)
    );

    if (it == m_impl->transition_table.end()) {
      assert(!"no valid state");
    }

    std::string helper;
    State next_state;
    boost::tie(helper, next_state) = it->second;

    HelperRegistry::const_iterator hit = m_impl->registry->find(helper);
    if (hit == m_impl->registry->end()) {
      assert(!"no helper");
    }
    boost::shared_ptr<classad::ClassAd> result = hit->second(ad);
    current_state = next_state;
  }

  return result;
}

}}}
