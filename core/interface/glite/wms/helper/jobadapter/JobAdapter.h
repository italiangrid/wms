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

/***************************************************************************
 *  filename  : JobAdapter.h
 *  authors   : Elisabetta Ronchieri <elisabetta.ronchieri@cnaf.infn.it>
 *  Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
 *  For license conditions see LICENSE file or
 *  http://www.edg.org/license.html
 ***************************************************************************/

// $Id: JobAdapter.h,v 1.2.28.4.2.1.4.2 2010/04/08 13:52:15 mcecchi Exp $

#ifndef GLITE_WMS_HELPER_JOBADAPTER_JOBADAPTER_H
#define GLITE_WMS_HELPER_JOBADAPTER_JOBADAPTER_H

#ifndef GLITE_WMS_X_BOOST_UTILITY_HPP
#define GLITE_WMS_X_BOOST_UTILITY_HPP
#include <boost/utility.hpp>
#endif
#include <boost/shared_ptr.hpp>

namespace classad {
class ClassAd;
}

namespace glite {
namespace wms {
namespace helper {
namespace jobadapter {

void
replace(std::string& where, const std::string& what, const std::string& with);

class JobAdapter: boost::noncopyable
{
public:
  JobAdapter(
    const classad::ClassAd* ad,
    boost::shared_ptr<std::string> jw_template
  );
  ~JobAdapter();
  classad::ClassAd* resolve(void);
private: 
  const classad::ClassAd* m_ad;
  boost::shared_ptr<std::string> m_jw_template;
};

} // namespace jobadapter
} // namespace helper
} // namespace wms
} // namespace glite

#endif // GLITE_WMS_HELPER_JOBADAPTER_JOBADAPTER_H
