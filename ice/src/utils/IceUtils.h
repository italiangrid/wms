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

#ifndef GLITE_WMS_ICE_UTIL_ICEUTILS_HH
#define GLITE_WMS_ICE_UTIL_ICEUTILS_HH

#include <string>
#include <map>
#include <list>
#include <set>
#include <stdexcept>
#include <ctime>
#include <utility>
#include <sys/types.h>
#include <sys/socket.h>
#include "classad_distribution.h"

#include "ClassadSyntax_ex.h"
#include "JobRequest_ex.h"

#include <algorithm>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/lexical_cast.hpp>

namespace glite {
  namespace wms {
    namespace ice {
      namespace util {
	
        struct URL {
	    std::string proto;
	    std::string hostname;
	    int         tcpport;
	    std::string path;
	};
	
	class CreamJob;
	class Request;

	class IceUtils {

	  static std::string             s_tmpname;
	  static boost::recursive_mutex  s_mutex_tmpname;
	  static boost::recursive_mutex  s_mutex_myname;
	  static std::string             s_myname;

	public:
	  inline static std::string get_tmp_name( void ) {
	    boost::recursive_mutex::scoped_lock M( s_mutex_tmpname );
	    if( s_tmpname.empty() ) {
	      char *ptr = ::tmpnam( 0 );
	      std::string tmp( "START_GLITEWMSICE_SQL_STRING_TAG_" );
	      if(ptr)
		tmp += boost::lexical_cast<std::string>( (long long)ptr );//to_string( (long long) ptr );
	      
	      struct timeval T;
	      gettimeofday( &T, 0 );
	      
	      tmp += "_";
	      tmp += boost::lexical_cast<std::string>( T.tv_sec ); //to_string(T.tv_sec);
	      tmp += "_";
	      tmp += boost::lexical_cast<std::string>( T.tv_usec ); //to_string( T.tv_usec );
	      
	      s_tmpname = tmp;
	      
	      boost::replace_all( s_tmpname, "'", "_" );
	    }
	    
	    return s_tmpname;
	  }
	  
	  static int fetch_jobs_callback(void *param, int argc, char **argv, char **azColName);
	  
	  //static bool is_rescheduled_job( const CreamJob& );
	  static bool exists_subsequent_token( const std::string&, std::string& );
	  static bool ignore_job( const std::string& CID, CreamJob& tmp_job, std::string& reason );
	  
	  static std::pair<bool, time_t> is_good_proxy( const std::string& proxyfile ) throw();

	  static std::pair<bool, time_t> is_valid_proxy( const std::string& proxyfile ) throw();

	  static std::string compute_sha1_digest( const std::string& proxyfile ) throw( std::runtime_error& );
	  static std::string bin_to_string( unsigned char* buf, size_t len );

	  static void cream_jdl_helper( const std::string& oldJdl,
			              std::string& newjdl ) 
	    throw( ClassadSyntax_ex& );

	  static int update_isb_list( classad::ClassAd* );
	  static int update_osb_list( classad::ClassAd* jdl );

	  //
	  // Utility function: Computes a SHA1 hash of the input string. The
	  // resulting hash is made of 40 printable characters, each
	  // character in the range [0-9A-F].
	  //
	  // @input name an input string; 
	  //
	  // @return a string of 40 printable characters, each in the range [0-9A-F]
	  //
	  static std::string compressed_string( const std::string& name );

	  
  
	  /**
	   * Utility function to return the hostname
	   *
	   * @return the host name
	   */
	  static std::string get_host_name( void ) throw ( std::runtime_error& );
  
	  static std::string get_url( void ) throw ( std::runtime_error& );
  
	  /**
	   * Converts a time_t value to a string. This function
	   * uses the thread-safe function ctime_r.
	   *
	   * @param tval the time_t value to convert
	   * @result the textual representation of timestamp tval
	   */ 
	  static std::string time_t_to_string( time_t tval );
  
	  //	std::string int_to_string( const int val );
  
	  /**
	   * This function outputs a string containing the CREAM and grid
	   * jobid for a given job. This function should be used whenever
	   * any information related to a job needs to be logged into the
	   * log files.
	   *
	   * @param job the job whose cream/grid id should be returned
	   *
	   * @return the string containing cream and grid job id, in
	   * human-readable form
	   */
  
	  static std::string canonizeString( const std::string& ) throw();
 /* 
	  static std::string to_string( long int );
	  static std::string to_string( unsigned long int );
	  static std::string to_string( long long );
	  static std::string to_string( unsigned long long );
	  static std::string to_string( short int );
	  static std::string to_string( unsigned short int );
	  static std::string to_string( float );
	  static std::string to_string( bool );
	  static std::string to_string( const std::string& str ) { return str; }
*/
	  static std::string join( const std::list<std::string>& array, const std::string& sep );

	  static std::string withSQLDelimiters( const std::string& value ) {
	    std::string delimitedString = get_tmp_name();
	    delimitedString += value;
	    delimitedString += get_tmp_name();
	    return delimitedString;
	  }
	  

	  
	  static bool parse_url( const std::string& url, URL& target, std::string& error );

        }; // class utilities

	class pathName {
	public:
	  typedef enum { invalid=-1, absolute, uri, relative } pathType_t;
	  
	  pathName( const std::string& p );
	  virtual ~pathName( ) { }
	  // accessors
	  pathType_t getPathType( void ) const { return m_pathType; }
	  const std::string& getFullName( void ) const { return m_fullName; }
	  const std::string& getPathName( void ) const { return m_pathName; }
	  const std::string& getFileName( void ) const { return m_fileName; }
	  
	protected:
	  
	  //log4cpp::Category* m_log_dev;
	  const std::string m_fullName;
	  pathType_t m_pathType;
	  std::string m_pathName;
	  std::string m_fileName;
	};
	
// 	struct ltstring {
// 	  bool operator()( const std::pair<std::string, std::string>& s1, 
// 			   const std::pair<std::string, std::string>& s2) const
// 	  {
// 	    if ( s1.first.compare(s2.first) < 0 ) return true;
// 	    else {
// 	      if(s2.first.compare(s1.first) < 0 ) return false;
// 	      else {
// 		if( s1.second.compare(s2.second) < 0 ) return true;
// 		else return false;
// 	      }
// 	    }
// 	    
// 	  }
// 	};

      } // namespace util
    } // namespace ice
  } // namespace wms
} // namespace glite

#endif
