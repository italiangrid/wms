/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

//
// File: wmp2wm.h
// Author: Marco Pappalardo
// Author: Giuseppe Avellino <egee@datamat.it>
//

#ifndef GLITE_WMS_WMPROXY_SERVER_WMP2WM_H
#define GLITE_WMS_WMPROXY_SERVER_WMP2WM_H

#include <string>

// Boost
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>

// Jobdir
#include "glite/wms/common/utilities/jobdir.h"

// Eventlogger
#include "eventlogger/eventlogger.h"

namespace classad
{
class ClassAd;
}

namespace glite
{
namespace wms
{
namespace wmproxy
{
namespace commands
{
class Command;
}

namespace server
{


/**
 * WMP2WM class
 *
 * @version 1.0
 * @date 2004
 * @author Giuseppe Avellino <egee@datamat.it>
*/
class WMP2WM
{

public:
   /**
    * Contructor
    */
   WMP2WM();

   /**
    *  Destructor
    */
   virtual ~WMP2WM();

   /**
   * Initialized the WMP to WM Proxy using the given Jobdir.
   * @param filename the pathname of JobDir.
   * @param wmpeventlogger
   */
   void init(const std::string& filename,
             glite::wms::wmproxy::eventlogger::WMPEventLogger *wmpeventlogger);
   /**
   * Forwards a submit command to the WM through the FileList.
   * @param cmdAd the Command object related to the edg_job_submit as
   * provided by WMP.
   */
   virtual void submit(const std::string& jdl, const std::string& jdlpath = "");
   /**
   * Forwards a cancel command to the WM through the FileList.
   * @param cmdAd the Command object related to the edg_job_cancel as
   * provided by WMP.
   */
   virtual void cancel(const std::string& jobid, const std::string& seq_code);
   /**
   * Forwards a match command to the WM through the FileList.
   * @param cmdAd the Command object related to the edg_job_list_match
   * as provided by WMP.
   */
   virtual void match(const std::string& jdl, const std::string& file,
                      const std::string& proxy, void * result);
private:

   /**
   * Stores the sequence code for Purger activities, in case of submit.
   * @param cmdAd the Command classad object related to the edg_job_submit
   * as provided by WMP.
   */
   void storeSequenceCode(classad::ClassAd* cmdAd);

   /**
   * Stores sequence codes for DAG Nodes.
   * @param classad the dag classad.
   * @param seqfile the sequence code file of the node.
   * @param to_root_path the sandbox root path.
   */
   void storeDAGSequenceCode(classad::ClassAd* classad, std::string seqfile,
                             std::string to_root_path);
   /**
   * Converts WMP inner protocol-based info into conversation protocol common
   * to WMP and WM.
   * @param cmdAd the Command object related to the command as provided by WMP.
   * @return the converted common-protocol-based string.
   */
   std::string convertProtocol(classad::ClassAd* cmdAd);
   // Event logger instance
   glite::wms::wmproxy::eventlogger::WMPEventLogger * wmpeventlogger;
   boost::shared_ptr<glite::wms::common::utilities::JobDir> m_jobdir;
};

} // namespace server
} // namespace wmproxy
} // namespace wms
} // namespace glite

#endif // GLITE_WMS_WMPROXY_SERVER_WMP2WM_H





