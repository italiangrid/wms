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

// File: RBSimpleISMImpl.h
// Author: Salvatore Monforte
// Copyright (c) 2002 EU DataGrid.

// $Id: RBSimpleISMImpl.h,v 1.1.2.2 2012/09/12 10:02:12 mcecchi Exp $

#ifndef GLITE_WMS_BROKER_RBSIMPLEISMIMPL_H
#define GLITE_WMS_BROKER_RBSIMPLEISMIMPL_H

#include "ResourceBroker.h"

namespace glite {
namespace wms {
namespace broker {

struct RBSimpleISMImpl : ResourceBroker::Impl
{
  boost::tuple<
    boost::shared_ptr<matchmaking::matchtable>,
    boost::shared_ptr<brokerinfo::FileMapping>,
    boost::shared_ptr<brokerinfo::StorageMapping>
  > 
  findSuitableCEs(classad::ClassAd const*);
};

}}}

#endif
