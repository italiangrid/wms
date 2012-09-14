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

// File: CopyHelper.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id$

#ifndef GLITE_WMS_HELPER_COPYHELPER_H
#define GLITE_WMS_HELPER_COPYHELPER_H

#ifndef GLITE_WMS_HELPER_HELPERIMPL_H
#include "glite/wms/helper/HelperImpl.h"
#endif

namespace glite {
namespace wms {
namespace helper {

class CopyHelper: public HelperImpl
{
public:
  CopyHelper();
  ~CopyHelper();

  virtual helper_type type() const;
  virtual filename_type resolve(filename_type const& input_file) const;
  virtual classad::ClassAd* resolve(classad::ClassAd const* input_ad) const;
};

} // namespace helper
} // namespace wms
} // namespace glite

#endif // GLITE_WMS_HELPER_COPYHELPER_H
