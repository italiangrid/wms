
/* LICENSE:
Copyright (c) Members of the EGEE Collaboration. 2010. 
See http://www.eu-egee.org/partners/ for details on the copyright
holders.  

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

   http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. 
See the License for the specific language governing permissions and 
limitations under the License.

END LICENSE */
#ifndef ICELBEVENTFACTORY_H
#define ICELBEVENTFACTORY_H

#include "CreamJob.h"

namespace glite {
    namespace wms {
        namespace ice {
            namespace util {

                class IceLBEvent; // forward declaration

                /**
                 * This class is a factory used to build a logging
                 * event corresponding to a job status change.
                 */
                class iceLBEventFactory {
                public:
                    virtual ~iceLBEventFactory( ) { };

                    /**
                     * Factory method used to create an iceLBEvent
                     * object which corresponds to the last (most
                     * recent) status change for job j. The caller
                     * owns the pointer which is returned; thus, the
                     * caller is responsible for freeing the pointer
                     * when necessary.
                     *
                     * @param j the job whose more recent status is to
                     * be logged 
                     *
                     * @return a logging event corresponding to the
                     * most recent status of job j; if the current job
                     * status for j does not correspond to any valid
                     * LB event, then the null pointer is returned.
                     * The caller owns the returned pointer, and is
                     * thus responsible for relinquishing it.
                     */
                    static IceLBEvent* mkEvent( const CreamJob& j, const bool force_donefailed = false );

                protected:
                    iceLBEventFactory( ) { };
                };

            } // namespace util
        } // namespace ice
    } // namespace wms
} // namespace glite

#endif
