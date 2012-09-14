/*
 * File: NS2WMProxy.h
 * Author: Marco Pappalardo
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$
#ifndef _GLITE_WMS_MANAGER_NS_DAEMON_NS2WMPROXY_H_
#define _GLITE_WMS_MANAGER_NS_DAEMON_NS2WMPROXY_H_

#include <string>
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>
#include "glite/wms/common/utilities/FileList.h"
#include "glite/wms/common/utilities/FileListLock.h"
#include "glite/wmsutils/jobid/cjobid.h"
#include "glite/lb/producer.h"

namespace classad {
  class ClassAd;
}

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {
  class Command;
}
namespace daemon {

namespace commands=glite::wms::manager::ns::commands;

typedef boost::scoped_ptr< glite::wms::common::utilities::FileList<std::string> > FileListPtr;
typedef boost::scoped_ptr< glite::wms::common::utilities::FileListMutex> FileListMutexPtr;

class NS2WMProxy
{
  FileListPtr m_filelist;
  FileListMutexPtr m_mutex;

public:
  /*
   * Contructor.
   */
  NS2WMProxy();
  /*
   * Destructor.
   */
  virtual ~NS2WMProxy();
  
  /*
   * Initialized the NS to WM Proxy using the given FileList.
   * @param filename the pathname of FileList file.
   */
  void init(const std::string &filename);

  /*
   * Forwards a submit command to the WM through the FileList.
   * @param cmd the Command object related to the edg_job_submit as provided by NS.
   */
  virtual void submit(classad::ClassAd*);
  /*
   * Forwards a cancel command to the WM through the FileList.
   * @param cmd the Command object related to the edg_job_cancel as provided by NS.
   */  
  virtual void cancel(classad::ClassAd*);
  /*
   * Forwards a match command to the WM through the FileList.
   * @param cmd the Command object related to the edg_job_list_match as provided by NS.
   */  
  virtual void match(classad::ClassAd*);

private:
  /*
   * Stores the sequence code for Purger activities, in case of submit.
   * @param cmd the Command classad object related to the edg_job_submit as provided by NS.
   */
  void storeSequenceCode(classad::ClassAd*, edg_wll_Context*);
  /*
   * Stores the user id for Purger quota activities.
   * @param cmd the Command classad object related to the edg_job_submit as provided by NS.
   */
  void storeQuotaId(classad::ClassAd*);
  /*                                                            
   * Stores sequence codes for DAG Nodes.                       
   * @param jdlad the dag classad.                              
   * @param seqfile the sequence code file of the node.         
   * @param to_root_path the sandbox root path.                 
   */ 
  void NS2WMProxy::storeDAGSequenceCode(classad::ClassAd*, std::string, std::string); 
  /*
   * Converts NS inner protocol-based info into conversation protocol common
   * to NS and WM.
   * @param the Command object related to the command as provided by NS.
   * @return the converted common-protocol-based string.
   */
  std::string convertProtocol(classad::ClassAd*); 
  /*                                                            
   * Normalizes Dag nodes ISB deleting pathname preceeding filename.
   * It takes a reference to a classad and changes are applied to it.
   * @param ad the dag classad                                  
   */ 
  classad::ClassAd* normalizeNodesISB(classad::ClassAd*); 

};

} // namespace daemon
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif

// Local Variables:
// mode: c++
// End:






