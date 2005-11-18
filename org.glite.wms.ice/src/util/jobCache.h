
#ifndef __JOBCACHE_H__
#define __JOBCACHE_H__

#include <string>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <ostream>
#include "jnlFile_ex.h"
#include "jobCacheOperation.h"
#include "glite/ce/cream-client-api-c/job_statuses.h"
#include "jnlFileManager.h"
#include "ClassadSyntax_ex.h"
#include "elementNotFound_ex.h"
#include "creamJob.h" 
#include "boost/thread/recursive_mutex.hpp"

// #define OPERATION_SEPARATOR ":"
#define MAX_OPERATION_COUNTER 10

#define DEFAULT_JNLFILE "/tmp/jobCachePersistFile"
#define DEFAULT_SNAPFILE "/tmp/jobCachePersistFile.snapshot"

namespace api = glite::ce::cream_client_api;

namespace glite {
  namespace wms {
    namespace ice {
      namespace util {
              
	//______________________________________________________
	class jobCache {

	private:
	  static jobCache *_instance;
	  static std::string jnlFile;
	  static std::string snapFile;

          boost::recursive_mutex jobCacheMutex; //< Lock used to guarantee mutual exclusion while accessing this data structure from concurrent threads. This will be removed when the locking mechanism based on the lock() and unlock() methods will be used.

          boost::recursive_mutex theMutex;

          /**
           * This class represents a sort of "double-keyed hash
           * table", that is, a hash table where each element can be
           * indexed by two different (both unique) keys. Elements
           * stored in this table are of type CreamJob; a
           * jobCacheTable provides methods to put a single job, and
           * retrieve a job with a given Cream jobID or a given Grid
           * jobID. Both these keys are strings.
           *
           * This class should be used by methods of the jobCache
           * class <em>only</em>. The implemeentation (and even only
           * the existence) of the jobCacheTable should be kept opaque
           * to the user.
           */
          class jobCacheTable {
          protected:
              std::list< CreamJob > _jobs; //< Jobs in the jobCacheTable are stored in a list
              std::map< std::string, std::list< CreamJob >::iterator > _cidMap; //< This hash table associates Cream jobIDs with job positions in the list _jobs
              std::map< std::string, std::list< CreamJob >::iterator > _gidMap; //< This hash table associates Grid jobIDs with job positions in the list _jobs

              // Some useful typedefs, for internal use only...
              typedef std::list< CreamJob > _jobsType;
              typedef std::map< std::string, _jobsType::iterator > _cidMapType;
              typedef std::map< std::string, _jobsType::iterator > _gidMapType;

          public:

              // Typedefs
              typedef _jobsType::iterator iterator; //< The standard (modifying) iterator used for containers
              typedef _jobsType::const_iterator const_iterator; //< The standard (non-modifying) iterator used for containers

              jobCacheTable();
              virtual ~jobCacheTable() { };             
              
              /**
               * Inserts a job into the in-memory hash table. If the job
               * is already in the table, the old job is overwritten by the new
               * one. This method <em>does not</em> log anything on the journal
               *
               * @param c The job to insert
               */
              void putJob( const CreamJob& c );
              
              /**
               * Removes a job from the in-memory hash table. If the job
               * is not in the table, this method does nothing. This method
               * <em>does not</em> log anything on the journal.
               *
               * @param c The job to remove. If the job does not exist,
               * nothing is done.
               */
              void delJob( const CreamJob& c );
              
              /**
               * Looks up for a job with a given Grid ID (gid). Returns
               * an iterator pointint to the requested job in the
               * _jobs data structure.
               *
               * @param gid the Grid JobID to look for 
               *
               * @return an iterator to the requested job in the _jobs
               * list; returns an iterator pointing to the end of list
               * (_jobs.end()) if the job was not found
               */
              jobCacheTable::iterator findJobByGID( const std::string& gid );
                            
              /**
               * Looks up for a job with a given Cream ID (cid). Returns
               * an iterator pointint to the requested job in the
               * _jobs data structure.
               *
               * @param cid the Cream JobID to look for 
               *
               * @return an iterator to the requested job in the _jobs
               * list; returns an iterator pointing to the end of list
               * (_jobs.end()) if the job was not found
               */
              jobCacheTable::iterator findJobByCID( const std::string& cid );

              /**
               * Returns an iterator to the first job in the jobCacheTable
               *
               * @return an iterator to the first job
               */
              jobCacheTable::iterator begin( void );

              /**
               * Returns an iterator to the end of the jobCacheTable
               *
               * @return an iterator to the end of the jobCacheTable
               */
              jobCacheTable::iterator end( void );

              /**
               * Returns a const_iterator to the first job in the jobCacheTable
               *
               * @return a const_iterator to the first job
               */
              jobCacheTable::const_iterator begin( void ) const;

              /**
               * Returns a const_iterator to the end of the jobCacheTable
               *
               * @return a const_iterator to the end of the jobCacheTable
               */
              jobCacheTable::const_iterator end( void ) const;

          };

          jobCacheTable _jobs; //< The in-core data structure holding the set of jobs

	  int operation_counter; //< Number of operations logged on the journal

	  void loadJournal(void) 
	    throw(jnlFile_ex&, ClassadSyntax_ex&, jnlFileReadOnly_ex&);

	  void loadSnapshot(void) 
	    throw(jnlFile_ex&, ClassadSyntax_ex&);

	  std::ifstream is;
	  std::ofstream os;
	  glite::wms::ice::util::jnlFileManager* jnlMgr;

	  CreamJob unparse(const std::string&) throw(ClassadSyntax_ex&);
	  void toString(const CreamJob&, std::string&);

	protected:
	  jobCache(const std::string&, const std::string&) 
	    throw(jnlFile_ex&, ClassadSyntax_ex&);

          /**
           * Logs an operation to the journal manager. The operation
           * count is increased; if the maximum number of operations
           * is reached, the journal file is truncated, and the cache
           * content is stored on disk.
           *
           * @param op the operation to log
           * @param param the parameters to that operation
           */
          void logOperation( const operation& op, const std::string& param );

	public:

	  static jobCache* getInstance() throw(jnlFile_ex&, ClassadSyntax_ex&);
	  static void setJournalFile(const std::string& jnl) { jnlFile = jnl; }
	  static void setSnapshotFile(const std::string& snap) { snapFile = snap; }

	  virtual ~jobCache();

          // Modifiers

          /**
           * Inserts a job on the jobCache; it is possible to insert a
           * job which is already in the cache: in this case the old
           * job is replaced by the new one. All operations are logged
           * on the journal file.
           *
           * @param c the job to insert into the cache. Job c may
           * already be in the cache; in this case the old job is
           * overwritten
           */
	  void put(const CreamJob& c ) throw(jnlFile_ex&, jnlFileReadOnly_ex&);

          //
          // ALL the following methods should be considered OBSOLETE
          //
	  void updateStatusByCreamJobID(const std::string& cid, const api::job_statuses::job_status&) throw(elementNotFound_ex&);

	  void updateStatusByGridJobID(const std::string& gid, const api::job_statuses::job_status&) throw(elementNotFound_ex&);

	  void remove_by_grid_jobid(const std::string&) 
	    throw (jnlFile_ex&, jnlFileReadOnly_ex&);

	  void remove_by_cream_jobid(const std::string&) 
	    throw (jnlFile_ex&, jnlFileReadOnly_ex&, elementNotFound_ex&);

	  bool isFinished_by_grid_jobid(const std::string&) throw(elementNotFound_ex&);
	  bool isFinished_by_cream_jobid(const std::string&) throw(elementNotFound_ex&);

	  std::string get_grid_jobid_by_cream_jobid(const std::string&) throw(elementNotFound_ex&);
	  std::string get_cream_jobid_by_grid_jobid(const std::string&) throw(elementNotFound_ex&);
	  CreamJob getJobByCreamJobID(const std::string&) throw(elementNotFound_ex&);
	  CreamJob getJobByGridJobID(const std::string&) throw(elementNotFound_ex&);
	  api::job_statuses::job_status 
	  getStatus_by_cream_jobid(const std::string&) throw(elementNotFound_ex&);
	  api::job_statuses::job_status
	  getStatus_by_grid_jobid(const std::string&) throw(elementNotFound_ex&);
          //
          // end list of OBSOLETE methods
          //

	  void getActiveCreamJobIDs(std::vector<std::string>& target) ;  

        protected:	  
	  void dump(void) throw(jnlFile_ex&);
	  void print(std::ostream&);
	};
      }
    }
  }
}

#endif
