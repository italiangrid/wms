
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
#ifndef ICELBEVENT_H
#define ICELBEVENT_H

#include "CreamJob.h"
#include "glite/lb/producer.h"
#include <string>

namespace glite {
    namespace wms {
        namespace ice {
            namespace util {

                // Forward declaration
                class IceLBContext;

                /**
                 * This class represents the generic ICE LB event. All
                 * events must implement the execute() method, with
                 * the single ctx parameter being the current LB
                 * context. 
                 */
                class IceLBEvent {
                public:
                    virtual ~IceLBEvent( ) { };

                    /**
                     * Actually logs the event. 
                     *
                     * @param ctx a pointer to the current L&B
                     * context.  the caller is not transfering
                     * ownership of the ctx pointer; this method MUST
                     * NOT try to free the context.
                     *
                     * @return the LB exit code (0 means success)
                     */
                    virtual int execute( IceLBContext* ctx ) = 0;

                    /**
                     * Provides a description for the event being
                     * logged.  useful for debug purposes.
                     *
                     * @return the description of the event being
                     * logged
                     */
                    const std::string& describe( ) const {
                        return m_description;
                    };

                    /**
                     * Gets the job being logged
                     *
                     * @return the current job
                     */
                    CreamJob& getJob( ) {
                        return m_job; 
                    };

                    /**
                     * Gets the source logging the event
                     *
                     * @return the source logging the event
                     */
                    edg_wll_Source getSrc( void ) const {
                        return m_src;
                    }

                protected:
                    /**
                     * Constructor for an iceLBEvent. 
                     *
                     * @param j the job to be logged
                     * @param src the source logging the event
                     * @param dsc a textual description of the event being logged
                     */
                    IceLBEvent( const CreamJob& j, edg_wll_Source src, const std::string& dsc );

                    CreamJob m_job;
                    edg_wll_Source m_src;
                    std::string m_description;
                };

                /**
                 * A job is being transferred from ICE to CREAM.
                 */
                class cream_transfer_start_event : public IceLBEvent {
                public:
                    /**
                     * @param j the job being transferred.
                     */
                    cream_transfer_start_event( const CreamJob& j );
                    int execute( IceLBContext* ctx );
                };

                /**
                 * A job has been succesfully transferred from ICE to CREAM.
                 */
                class cream_transfer_ok_event : public IceLBEvent {
                public:
                    /**
                     * @param j the job succesfully transferred
                     */
                    cream_transfer_ok_event( const CreamJob& j );
                    int execute( IceLBContext* ctx );
                };


                /**
                 * Transfer of a job from ICE to CREAM failed due to some error
                 */
                class cream_transfer_fail_event : public IceLBEvent {
                public:
                    /**
                     * @param j the job whose transfer failed
                     * @param reason a string describing the failure reason 
                     */
                    cream_transfer_fail_event( const CreamJob& j, const std::string& reason );
                    int execute( IceLBContext* ctx );
                protected:
                    std::string m_reason;
                };

                /**
                 * A request has been dequeued from the WMS queue
                 */ 
                class wms_dequeued_event : public IceLBEvent {
                public:
                    wms_dequeued_event( const CreamJob& j, const std::string& qname );
                    int execute( IceLBContext* ctx );
                protected:
                    std::string m_qname;
                };


                class cream_refused_event : public IceLBEvent {
                public:
                    cream_refused_event( const CreamJob& j, const std::string& reason  );
                    int execute( IceLBContext* ctx );
                protected:
                    std::string m_reason;
                };


                class cream_cancel_request_event : public IceLBEvent {
                public:
                    cream_cancel_request_event( const CreamJob& j, const std::string& reason );
                    int execute( IceLBContext* ctx );
                protected:
                    std::string m_reason;
                };


                class cream_cancel_refuse_event : public IceLBEvent {
                public:
                    cream_cancel_refuse_event( const CreamJob& j, const std::string& reason );
                    int execute( IceLBContext* ctx );
                protected:
                    std::string m_reason;
                };


                class job_aborted_event : public IceLBEvent {
                public:
                    job_aborted_event( const CreamJob& j );
                    int execute( IceLBContext* ctx );
                };



                class job_running_event : public IceLBEvent {
                public:
                    job_running_event( const CreamJob& j );
                    int execute( IceLBContext* ctx );
                };


                class job_really_running_event : public IceLBEvent {
                public:
                    job_really_running_event( const CreamJob& j );
                    int execute( IceLBContext* ctx );
                };


                class job_cancelled_event : public IceLBEvent {
                public:
                    job_cancelled_event( const CreamJob& j );
                    int execute( IceLBContext* ctx );
                };


                class job_done_ok_event : public IceLBEvent {
                public:
                    job_done_ok_event( const CreamJob& j );
                    int execute( IceLBContext* ctx );
                };


                class job_done_failed_event : public IceLBEvent {
                public:
                    job_done_failed_event( const CreamJob& j );
                    int execute( IceLBContext* ctx );
                };


                class ns_enqueued_start_event : public IceLBEvent {
                public:
                    ns_enqueued_start_event( const CreamJob& j, const std::string& qname );
                    int execute( IceLBContext* ctx );
                protected:
                    std::string m_qname;
                };


                class ns_enqueued_fail_event : public IceLBEvent {
                public:
                    ns_enqueued_fail_event( const CreamJob& j, const std::string& qname, const std::string& reason );
                    int execute( IceLBContext* ctx );
                protected:
                    std::string m_qname;
                    std::string m_reason;
                };


                class ns_enqueued_ok_event : public IceLBEvent {
                public:
                    ns_enqueued_ok_event( const CreamJob& j, const std::string& qname );
                    int execute( IceLBContext* ctx );
                protected:
                    std::string m_qname;
                };


                class ice_resubmission_event : public IceLBEvent {
                public:
                    ice_resubmission_event( const CreamJob& j, const std::string& reason );
                    int execute( IceLBContext* ctx );
                protected:
                    std::string m_reason;
                };


                class job_suspended_event : public IceLBEvent {
                public:
                    job_suspended_event( const CreamJob& j );
                    int execute( IceLBContext* ctx );
                protected:
                    std::string m_reason;
                };

                
                class job_resumed_event : public IceLBEvent {
                public:
                    job_resumed_event( const CreamJob& j );
                    int execute( IceLBContext* ctx );
                protected:
                    std::string m_reason;
                };



            } // namespace util
        } // namespace ice
    } // namespace wns
}; // namespace glite

#endif
