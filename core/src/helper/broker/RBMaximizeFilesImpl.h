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

// File: RBMaximizeFilesImpl.h
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id: RBMaximizeFilesImpl.h,v 1.1.2.1 2012/09/11 10:19:36 mcecchi Exp $

#ifndef _GLITE_WMS_BROKER_RBMAXIMIZEFILESIMPL_H_
#define _GLITE_WMS_BROKER_RBMAXIMIZEFILESIMPL_H_

#include "glite/wms/broker/ResourceBroker.h"
#include "glite/wms/helper/brokerinfo/brokerinfo.h"
#include "glite/wms/helper/brokerinfo/brokerinfoISMImpl.h"

namespace glite {
namespace wms {
namespace broker {	

namespace brokerinfo  = wms::brokerinfo;
namespace matchmaking = wms::matchmaking;

class RBMaximizeFilesImpl : public ResourceBrokerImpl
{
public:
   RBMaximizeFilesImpl(brokerinfo::BrokerInfo<brokerinfo::brokerinfoISMImpl> *, bool do_prefetch=false);
  ~RBMaximizeFilesImpl();
  matchmaking::match_table_t* findSuitableCEs(const classad::ClassAd* requestAd);
private:
  brokerinfo::BrokerInfo<brokerinfo::brokerinfoISMImpl>* BI;
  bool m_prefetch;
};

} // namespace broker
} // namespace wms
} // namespace glite

#endif
