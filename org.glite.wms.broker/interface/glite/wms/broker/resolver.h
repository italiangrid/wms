// File: resolver.h
// Author: Francesco Giacomini

// $Id$

#ifndef GLITE_WMS_BROKER_RESOLVER_H
#define GLITE_WMS_BROKER_RESOLVER_H

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace classad {
class ClassAd;
}

namespace glite {
namespace wms {
namespace broker {

typedef boost::shared_ptr<classad::ClassAd> ClassAdPtr;

typedef std::map<
  std::string,
  boost::function<ClassAdPtr(ClassAdPtr)>
> HelperRegistry;

class Resolver
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:

  Resolver(HelperRegistry const& registry);

  ClassAdPtr operator()(ClassAdPtr ad) const;

};

ClassAdPtr broker_helper_resolve(ClassAdPtr ad);
ClassAdPtr dagman_helper(ClassAdPtr ad);
ClassAdPtr job_adapter_helper(ClassAdPtr ad);

}}}

#endif
