/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the
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

/*
 * File: brokerinfo.h
 * Author: Monforte Salvatore
 */

// $Id: brokerinfo.h,v 1.1.2.1 2012/09/11 10:19:39 mcecchi Exp $

#ifndef _GLITE_WMS_BROKERINFO_BROKERINFO_H_
#define _GLITE_WMS_BROKERINFO_BROKERINFO_H_

#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <string>
#include <set>
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
> FileMapping;

struct StorageInfo
{
  typedef std::set<std::pair<std::string, int> > Protocols;
  typedef std::vector<FileMapping::const_iterator> Links;
  typedef std::vector<std::pair<std::string, std::string> > CE_Mounts;

  Protocols protocols;
  Links links;
  CE_Mounts ce_mounts;
};

typedef std::map<std::string, StorageInfo> StorageMapping;

struct DataInfo
{
  boost::shared_ptr<FileMapping> fm;
  boost::shared_ptr<StorageMapping> sm;
  DataInfo(
    boost::shared_ptr<FileMapping> f,
    boost::shared_ptr<StorageMapping> s
  ) : fm(f), sm(s) {}
};

classad::ClassAd*
create_brokerinfo(
  classad::ClassAd const& jdl_ad,
  classad::ClassAd const& ce_ad,
  DataInfo const& data_info
);

} // namespace brokerinfo
} // namespace wms
} // namespace glite

#endif
