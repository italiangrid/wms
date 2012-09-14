#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_ABORTEDCONTAINER_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_ABORTEDCONTAINER_H

#include <map>
#include <string>

#include "glite/wms/common/utilities/FileList.h"
#include "glite/wms/common/utilities/FileListIterator.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor {

class AbortedContainer {
public:
  AbortedContainer( const char *filename );
  AbortedContainer( const std::string &filename );
  ~AbortedContainer( void );

  void compact( void );
  bool insert( const std::string &condorId );
  bool remove( const std::string &condorId );

  inline bool search( const std::string &condorId )
  { return( this->ac_pointers.find(condorId) != this->ac_pointers.end() ); }
  inline size_t inserted( void ) { return this->ac_inserted; }

private:
  void onConstruct( void );

  typedef glite::wms::common::utilities::FileList<std::string>   FileList;
  typedef std::map<std::string, FileList::iterator>                 Map;

  size_t     ac_inserted;
  FileList   ac_filelist;
  Map        ac_pointers;
};

}; // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_ABORTEDCONTAINER_H */

// Local Variables:
// mode: c++
// End:
