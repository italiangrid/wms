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

// File: HelperFactory.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id$

#include <fstream>
#include <utility>
#include <classad_distribution.h>
#include <boost/utility.hpp>
#include <map>

#include "glite/wms/helper/HelperFactory.h"
#include "glite/wms/helper/HelperImpl.h"


using namespace std;
using namespace classad;

namespace glite {
namespace wms {
namespace helper {

HelperFactory* HelperFactory::s_instance = 0;

class HelperFactory::Impl: boost::noncopyable
{
  typedef std::map<std::string, product_creator_type> factory_type;
  factory_type m_factory;

public:

  bool register_helper(std::string const& id, product_creator_type creator);
  bool unregister_helper(std::string const& id);
  product_type* create_helper(std::string const& id);
  std::vector<std::string> list() const;
};

bool
HelperFactory::Impl::register_helper(std::string const& id, product_creator_type creator)
{
  return m_factory.insert(std::make_pair(id, creator)).second;
}

bool
HelperFactory::Impl::unregister_helper(std::string const& id)
{
  return m_factory.erase(id) == 1;
}

HelperFactory::product_type*
HelperFactory::Impl::create_helper(std::string const& id)
{
   factory_type::iterator i = m_factory.find(id);
   if (i == m_factory.end())
   {
      throw NoCreateHelperException();

   }
   return (i->second)();
}

std::vector<std::string>
HelperFactory::Impl::list() const
{
  std::vector<std::string> result;

  for (factory_type::const_iterator it = m_factory.begin(); it != m_factory.end(); ++it) {
    result.push_back(it->first);
  }

  return result;
}

HelperFactory*
HelperFactory::instance()
{
  if (s_instance == 0) {
    s_instance = new HelperFactory;
  }

  return s_instance;
}

HelperFactory::HelperFactory()
  : m_impl(new Impl)
{
}

HelperFactory::~HelperFactory()
{
  delete m_impl;
}

bool
HelperFactory::register_helper(std::string const& id, product_creator_type creator)
{
  return m_impl->register_helper(id, creator);
}

bool
HelperFactory::unregister_helper(std::string const& id)
{
  return m_impl->unregister_helper(id);
}

HelperFactory::product_type*
HelperFactory::create_helper(std::string const& id)
{
  return m_impl->create_helper(id);
}

std::vector<std::string>
HelperFactory::list() const
{
  return m_impl->list();
}


} // namespace helper
} // namespace wms
} // namespace glite
