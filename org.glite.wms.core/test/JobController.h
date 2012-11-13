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
  
  Mock JobController header file
  
  Authors: Paolo Andreetto <paolo.andreetto@pd.infn.it>
  
  Version info: $Id$
  
 */

#include <glite/wmsutils/jobid/JobId.h>
#include "glite/lb/producer.h"

namespace classad { class ClassAd; }

namespace glite { 
  namespace wms { 
    namespace jobsubmission { 
      namespace controller {

class JobController
{
public:
  JobController( edg_wll_Context *cont = NULL );
  ~JobController( void );
                                                                                                                            
  int submit( const classad::ClassAd *ad );
  bool cancel( const glite::wmsutils::jobid::JobId &id, const char *logfile = NULL, bool force = false );
  bool cancel( int condorid, const char *logfile = NULL, bool force = true );                                                                                                                            
};

      }
    }
  }
}
