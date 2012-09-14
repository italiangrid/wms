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

#ifndef GLITE_WMS_ICE_ICEDB_TRANSATION_H
#define GLITE_WMS_ICE_ICEDB_TRANSATION_H

#include <string>
#include "sqlite/sqlite3.h"
#include "AbsDbOperation.h"

namespace log4cpp {
    class Category;
};

namespace glite {
namespace wms {
namespace ice {
namespace db {

    class AbsDbOperation; // Forward declaration

    /**
     * This class represents a transaction (sequence of operations) to
     * be performed on the job database. The user instantiates a
     * Transaction object and invokes one or multiple times the
     * execute() method, to execute individual operations on the
     * transation. By default, SQLite creates a single transaction for
     * each individual operation invoked. If a transaction involves a
     * sequence of operations, the user must explicitly call the
     * begin() method first, then invoke the execute() method for each
     * step of the transation. By default, the transaction is
     * committed when the Transation object is destroyed. If the user
     * invokes the abort() method, the transaction is aborted when the
     * Transaction object is destroyed.
     */
    class Transaction {
    public:
        
      Transaction( const bool read_only, const bool create_check = false );					    
      ~Transaction( );

        /**
         * Begin a transaction. Other threads might read or write to
         * the database while the transaction is active. Note that it
         * is possible to call the execute() method without calling
         * begin() before. Calling begin() is only useful to guarantee
         * consistency of a sequence of operations (i.e., multiple
         * invocations of the execute() method for the same
         * transaction).
         */        
        Transaction& Begin();

        /**
         * Starts an exclusive transaction. No other threads can read or
         * write to the database while this transaction is active.
         */ 
        Transaction& Begin_exclusive();

        /**
         * Abort the current transaction. Calling this method is only
         * useful if one has called begin() or begin_exclusive()
         * before.
         */ 
	void Abort();

        /**
         * Commit the current transaction. Calling this method is only
         * useful if one has called begin() or begin_exclusive()
         * before. If the user did not call begin() or
         * begin_exclusive(), then the transaction is automatically
         * committed after each invocation of the execute() method.
         */ 
	void Commit();

        /**
         * Executes operation op as part of the current transaction;
         * the caller owns the pointer to op. If op is null, this
         * operation does nothing.
         */
        Transaction& execute( AbsDbOperation* op ) throw( DbOperationException& );

    protected:

        log4cpp::Category* m_log_dev;
        static sqlite3 *s_db;
        bool m_begin; ///< true iff the user called the begin() method
        bool m_commit; ///< false iff the user called abort(); true otherwise
        
        // Forbid copying a Transaction
        Transaction(const Transaction&) { };

        /**
         * Create an empty DB. This method is invoked by the costructor
         * if the DB does not exist.
         */
        void create_db( const bool, const bool ); // FIXME: Raise exception?

    };

} // namespace db
} // namespace ice
} // namespace wms
} // namespace glite

#endif
