/***************************************************************************
 *  filename  : catalog_access_utils.h
 *  authors   : Enzo Martelli <enzo.martelli@ct.infn.it>
 *  Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
 *  For license conditions see LICENSE file or
 *  http://www.edg.org/license.html
 ***************************************************************************/

#ifndef GLITE_WMS_RLS_CATALOG_ACCESS_UTILS_H
#define GLITE_WMS_RLS_CATALOG_ACCESS_UTILS_H

#include<boost/shared_ptr.hpp>

#include<string>
#include<vector>
#include<map>

namespace classad
{
class ClassAd;
}

namespace glite {
namespace wms {
namespace rls {

typedef
std::map<
  std::string,             // lfn
  std::vector<std::string> // sfns
> filemapping;

boost::shared_ptr<filemapping>
resolve_filemapping_info(
   const classad::ClassAd& requestAd
);

}
}
}


#endif
