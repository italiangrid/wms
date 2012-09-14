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

#ifndef GLITE_WMS_ICE_ABS_DB_OPERATION_H
#define GLITE_WMS_ICE_ABS_DB_OPERATION_H

#include "sqlite/sqlite3.h" // for sqlite3
#include <exception>
#include <string>

//#include "boost/thread/recursive_mutex.hpp"

namespace glite {
namespace wms {
namespace ice {
namespace db {

    /**
     * Exception which can be raised during the execution of an
     * operation of the DB.
     */
    class DbOperationException : public std::exception { 
    protected:
        std::string m_cause;
    public:
        DbOperationException( const std::string& cause ) throw() : m_cause(cause) { };
        virtual ~DbOperationException() throw() { };
        const char* what() const throw() { return m_cause.c_str(); };
    };

    class DbLockedException : public DbOperationException {
    public:
        DbLockedException( const std::string& cause ) throw() : DbOperationException( cause ) { };
    };

    /**
     * This class represents an abstract operation to be executed on
     * the database.
     */
    class AbsDbOperation { 
    public:
        typedef int (*sqlite_callback_t)(void*,int,char**,char**);    

        virtual ~AbsDbOperation() { };
        virtual void execute( sqlite3* db ) throw( DbOperationException& ) = 0;
    protected:

	//static boost::recursive_mutex  s_mutex;

        AbsDbOperation( const std::string& caller) : m_caller(caller) { };
        void do_query( sqlite3* db, const std::string& sqlcmd, sqlite_callback_t callback = 0, void* param = 0 ) throw( DbOperationException& );
	
	const std::string m_caller;
	
	std::string get_caller( void ) const { return m_caller; }
    };

} // namespace db
} // namespace ice
} // namespace wms
} // namespace glite

#endif
