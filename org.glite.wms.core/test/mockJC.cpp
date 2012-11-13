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
  
  Mock JobController implementation
  
  Authors: Paolo Andreetto <paolo.andreetto@pd.infn.it>
  
  Version info: $Id$
  
 */

#include "JobController.h"

using namespace glite::wms::jobsubmission::controller;

JobController::JobController( edg_wll_Context *cont ){
}

JobController::~JobController( ){
}

int JobController::submit( const classad::ClassAd *ad ){
  return 0;
}

bool JobController::cancel( const glite::wmsutils::jobid::JobId &id, const char *logfile, bool force ){
  return true;
}

bool JobController::cancel( int condorid, const char *logfile, bool force ){
  return true;
}
