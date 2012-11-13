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

// File: RBMinimizeAccessCostImpl.h
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id: 

#ifndef _GLITE_WMS_BROKER_RBMINIMIZEACCESSCOSTIMPL_H_
#define _GLITE_WMS_BROKER_RBMINIMIZEACCESSCOSTIMPL_H_

#include "ResourceBroker.h"
#include "glite/wms/brokerinfo/brokerinfo.h"
#include "glite/wms/brokerinfo/brokerinfoGlueImpl.h"

namespace brokerinfo  = glite::wms::brokerinfo;
namespace matchmaking = glite::wms::matchmaking;
namespace glite {
namespace wms {
namespace broker {

class RBMinimizeAccessCostImpl : public ResourceBrokerImpl
{
public:
  RBMinimizeAccessCostImpl(brokerinfo::BrokerInfo<brokerinfo::brokerinfoGlueImpl> *);
  ~RBMinimizeAccessCostImpl();
  matchmaking::match_table_t* findSuitableCEs(const classad::ClassAd* requestAd);
private:
  brokerinfo::BrokerInfo<brokerinfo::brokerinfoGlueImpl> *BI;
};

} // namespace broker
} // namespace wms
} // namespace glite

#endif
