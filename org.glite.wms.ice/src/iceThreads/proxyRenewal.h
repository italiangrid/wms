/*
 * Copyright (c) 2004 on behalf of the EU EGEE Project:
 * The European Organization for Nuclear Research (CERN),
 * Istituto Nazionale di Fisica Nucleare (INFN), Italy
 * Datamat Spa, Italy
 * Centre National de la Recherche Scientifique (CNRS), France
 * CS Systeme d'Information (CSSI), France
 * Royal Institute of Technology, Center for Parallel Computers (KTH-PDC), Sweden
 * Universiteit van Amsterdam (UvA), Netherlands
 * University of Helsinki (UH.HIP), Finland
 * University of Bergen (UiB), Norway
 * Council for the Central Laboratory of the Research Councils (CCLRC), United Kingdom
 *
 * ICE Proxy renewal thread
 *
 * Authors: Alvise Dorigo <alvise.dorigo@pd.infn.it>
 *          Moreno Marzolla <moreno.marzolla@pd.infn.it>
 */

#ifndef GLITE_WMS_ICE_UTIL_PROXYRENEWAL_H
#define GLITE_WMS_ICE_UTIL_PROXYRENEWAL_H

#include "iceThread.h"
#include "creamJob.h"

namespace log4cpp {    
  class Category;
};

namespace glite {
  namespace wms {
    namespace ice {
      namespace util {


          class proxyRenewal : public iceThread {
          protected:

              log4cpp::Category* m_log_dev;
              time_t m_delay;

          public: 
              proxyRenewal();
              virtual void body( void );
          };

      } 
    }
  }
}

#endif
