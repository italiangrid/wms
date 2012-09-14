#ifndef EDG_WORKLOAD_JOBCONTROL_COMMON_RAMCONTAINER_H
#define EDG_WORKLOAD_JOBCONTROL_COMMON_RAMCONTAINER_H

#include <vector>

#include "PointerId.h"

namespace classad { class ClassAd; }

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

class IdContainer;

class RamContainer {
public:
  RamContainer( void );
  RamContainer( IdContainer &ic );
  ~RamContainer( void );

  inline int inserted( void ) const { return this->rc_inserted; }

  void copy( IdContainer &ic );

  std::string condor_id( const std::string &edgId );
  std::string edg_id( const std::string &condorId );

  void insert( const std::string &edgId, const std::string &condorId );
  bool remove_by_condor_id( const std::string &condorid );

private:
  RamContainer( const RamContainer &rc ); // Not implemented
  RamContainer &operator=( const RamContainer &rc ); // Not implemented

  void internalCopy(IdContainer & ic);

  int                     rc_inserted;
  std::list<PointerId>    rc_pointers;
  std::vector<EdgId>      rc_edgs;
  std::vector<CondorId>   rc_condors;
};

}; // Namespace jccommon

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_COMMON_RAMCONTAINER_H */

// Local Variables:
// mode: c++
// End:
