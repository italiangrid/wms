/*
 * File: BrokerInfo.h
 * Author: Monforte Salvatore
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$

#ifndef _GLITE_WMS_BROKERINFO_BROKERINFO_H_
#define _GLITE_WMS_BROKERINFO_BROKERINFO_H_

#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <string>
#include <vector>
#include <utility>

namespace classad
{
  class ClassAd;
}

namespace glite {
namespace wms {
namespace brokerinfo {

typedef
std::map<
  std::string,             // lfn
  std::vector<std::string> // sfns
> filemapping;

typedef
std::map<
  std::string, // se
  boost::tuple<
    std::vector<std::pair<std::string,int> >,         // protocols and ports
    std::vector<filemapping::const_iterator>,         // link to supplied data files
    std::vector<std::pair<std::string, std::string> > // close computing elements and mount points
  >
> storagemapping;

extern "C++" 
{
 classad::ClassAd*
 make_brokerinfo_ad(
   boost::shared_ptr<filemapping>,
   boost::shared_ptr<storagemapping>,
   classad::ClassAd const&
 );

 boost::shared_ptr<storagemapping>
 resolve_storagemapping_info(
   boost::shared_ptr<filemapping> fm
 );

 boost::shared_ptr<filemapping>
 resolve_filemapping_info(
   const classad::ClassAd& requestAd
 );

 std::vector<storagemapping::const_iterator>
 select_compatible_storage(
   storagemapping const& sm,
   std::vector<std::string>const& dap
 );
 
 size_t
 count_unique_logical_files(
   std::vector<storagemapping::const_iterator>::const_iterator first,
   std::vector<storagemapping::const_iterator>::const_iterator last
 );
}

}; // namespace brokerinfo
}; // namespace wms
}; // namespace glite

#endif
