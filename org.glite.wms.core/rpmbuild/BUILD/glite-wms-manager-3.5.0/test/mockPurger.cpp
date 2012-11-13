/*
  Copyright (c) 2004 on behalf of the EU EGEE Project:
  The European Organization for Nuclear Research (CERN),
  Istituto Nazionale di Fisica Nucleare (INFN), Italy
  Datamat Spa, Italy
  Centre National de la Recherche Scientifique (CNRS), France
  CS Systeme d'Information (CSSI), France
  Royal Institute of Technology, Center for Parallel Computers (KTH-PDC), Sweden
  Universiteit van Amsterdam (UvA), Netherlands
  University of Helsinki (UH.HIP), Finland
  University of Bergen (UiB), Norway
  Council for the Central Laboratory of the Research Councils (CCLRC), United Kingdom
  
  Mock Purger implementation
  
  Authors: Paolo Andreetto <paolo.andreetto@pd.infn.it>
  
  Version info: $Id$
  
 */

#include "purger.h" 

namespace glite {
namespace wms {
namespace purger {

bool glite::wms::purger::purgeStorageEx(const boost::filesystem::path &path, int purge_threshold, bool fake_rm){
  return true;
}

bool glite::wms::purger::purgeStorage(const glite::wmsutils::jobid::JobId &JobId, const std::string &sandboxdir){
  return true;
}

}}}
