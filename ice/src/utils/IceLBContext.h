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

//
// This file is heavily based on org.glite.wms.jobsubmission/src/common/EventLogger.h
//
#ifndef ICELBCONTEXT_H
#define ICELBCONTEXT_H

#include <exception>

#include "glite/lb/context.h"
#include "CreamJob.h"

// Forward declaration
// namespace log4cpp {
//     class Category;
// };

typedef  struct _edg_wll_Context  *edg_wll_Context;

namespace glite {
    namespace wms {
        namespace ice {
            namespace util {

                class IceLBException : public std::exception {
                public:
                    IceLBException( const char *reason );
                    IceLBException( const std::string &reason );
                    virtual ~IceLBException( void ) throw();
                    
                    virtual const char *what( void ) const throw();
                    
                private:
                    std::string     m_le_reason;
                };


                class IceLBContext {
                public:

                    IceLBContext( );

                    virtual ~IceLBContext( void );

                    static void set_lb_retries( unsigned int r ) { s_el_s_retries = r; return; }
                    static void set_lb_interval( unsigned int sec ) { s_el_s_sleep = sec; return; }
                    static void set_lb_destination( const char* dest ) { el_s_destination = dest; return; }

                    /**
                     * @obsolete{Registers a new job (this would
                     * normally be done by the UI).}
                     */

                    inline void startLogging( void ) { this->m_el_count = 0; this->m_el_hostProxy = false; }

                    void setLoggingJob( const CreamJob& theJob, edg_wll_Source src, const bool use_cancel_seq_code  ) throw( IceLBException& );

                    void testCode( int &code, bool retry = false );
                    std::string getLoggingError( const char *preamble );

                    edg_wll_Context* el_context;

                    static const char *el_s_notLogged, *el_s_unavailable, *el_s_OK, *el_s_failed, *el_s_succesfully;
                    static const char *el_s_destination; 

                    std::string el_s_localhost_name;

                protected:
		
		    static std::string  s_localHostName;
		
                    bool                m_el_hostProxy;
                    unsigned int        m_el_count;
                    //log4cpp::Category  *m_log_dev;

                    static unsigned int s_el_s_retries, 
		                        s_el_s_sleep;
                };

            } // namespace util
        } // namespace ice
    } // namespace wns
}; // namespace glite

#endif
