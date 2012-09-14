#!/usr/bin/perl
# LICENSE:
#Copyright (c) Members of the EGEE Collaboration. 2010. 
#See http://www.eu-egee.org/partners/ for details on the copyright
#holders.  
#
#Licensed under the Apache License, Version 2.0 (the "License"); 
#you may not use this file except in compliance with the License. 
#You may obtain a copy of the License at 
#
#   http://www.apache.org/licenses/LICENSE-2.0 
#
#Unless required by applicable law or agreed to in writing, software 
#distributed under the License is distributed on an "AS IS" BASIS, 
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
#implied. 
#See the License for the specific language governing permissions and 
#limitations under the License.
#
#END LICENSE 

#
# This script takes the template CreamJob.txt and generates the source header
# file CreamJob.h containing the CreamJob class with proper ctors, and setter/getter
# method. This is a convenient way to do, because the members of CreamJobs are many and
# can change in the future and the impact on all the ICE's code could be very difficult
# to handle without an automatic generator of code.
#

@_data_methods = `cat CreamJob.txt`;

$inside_method = 0;
%custom_method = {};

foreach (@_data_methods) {
  chomp;
  if ( ($_ !~ m/^METHOD:/) && ($inside_method==0) ) {next;}
  $inside_method = 1;
  if( $_ =~ m/ENDMETHOD:/) {
    $inside_method = 0;
    next;
  }
  if( $inside_method == 1) {
    if( $_ =~ m/^METHOD:/ ) {
      print "\n";
      $_ =~ s/^METHOD://;
      @pieces = split( ",", $_ );
      $custom_method{$pieces[1]} = 1;
    }
  }
  next;
}

@_data_members = `cat CreamJob.txt`;

print <<CODE;
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

#ifndef GLITE_WMS_ICE_UTIL_CREAMJOB_H
#define GLITE_WMS_ICE_UTIL_CREAMJOB_H

/**
 *
 * ICE and WMS Headers
 *
 */


#include "ice/IceCore.h"
#include "IceUtils.h"
#include "Url.h"
#include "IceConfManager.h"
#include "ClassadSyntax_ex.h"
#include "classad_distribution.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"

/**
 *
 * Cream Client API C++ Headers
 *
 */
#include "gssapi.h"

#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/job_statuses.h"
#include "glite/ce/cream-client-api-c/VOMSWrapper.h"
#include "glite/ce/cream-client-api-c/certUtil.h"
#include "glite/ce/cream-client-api-c/CEUrl.h"

/**
 *
 * Boost's headers
 *
 */
#include <boost/thread/recursive_mutex.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/regex.hpp>

/**
 *
 * System and STL C++ Headers
 *
 */
#include <exception>
#include <vector>
#include <string>
#include <ctime>
#include <set>

namespace api_util = glite::ce::cream_client_api::util;
namespace api      = glite::ce::cream_client_api;
namespace api_util = glite::ce::cream_client_api::util;
namespace fs       = boost::filesystem;

using namespace std;
using namespace glite::wms::ice::util;


CODE

print "\nnamespace glite {\n  namespace wms {\n    namespace ice {\n      namespace util {\n        \n";

print "\t\tclass CreamJob {\n\n";

print "\t\tprotected:\n";

foreach (@_data_members) {
  chomp;
  if ($_ !~ m/^MEMBER:/) {next;}
  $_ =~ s/^MEMBER://;
  if ($_ =~ m/#PRIMARYKEY#/) {
    $line = $_;
    $line =~ s/#PRIMARYKEY#//;
    #print "$line\n";
    push @data_members, $line;
    @pieces = split(",", $line);
    $primary_key{$pieces[2]} = "primary key";
    next;
  }
  push @data_members, $_;
  next;
}


$num_of_members = scalar @data_members;
%defaults = {};

foreach (@data_members) {
  chomp;
  #$_ =~ s/blobstring/std::string/g;
  @pieces = split(",", $_);
  #print "\t\t  " . $pieces[1] . "  m_" . $pieces[2] . ";\n";
  printf "\t\t  %30s  m_%s;\n", $pieces[1], $pieces[2];
  printf "\t\t  %30s  m_changed_%s;\n", "bool", $pieces[2];
  push @members, $pieces[2];
  push @sql_types, $pieces[0];
  $defaults{$pieces[2]} = $pieces[3];
}

printf "\n\t\t  %30s  m_new;\n", "bool";

print "\n\t\t public:\n";
print "\t\t  static boost::recursive_mutex s_classad_mutex;\n";
print "\t\t  static boost::recursive_mutex s_reschedule_mutex;\n";
print "\t\t  static int num_of_members( void ) { return $num_of_members; }";
print "\n\n\t\tpublic:\n";
print "\n\t\t /**\n";
print "\t\t  *\n";
print "\t\t  * SETTER methods: set the Job's data members\n";
print "\t\t  *\n";
print "\t\t  */\n";


foreach (@data_members) {
  chomp;
  #$_ =~ s/blobstring/std::string/g;
  @pieces = split(",", $_);
  if($custom_method{"set_$pieces[2]"} == 1) { next; }
  print "\t\t  ". "virtual void  set_" . $pieces[2] . "( const " . $pieces[1] . "& _" . $pieces[2] . ") { m_" . $pieces[2] . " = _" . $pieces[2] . "; m_changed_$pieces[2]=true; }\n";
}


print "\n\t\t /**\n";
print "\t\t  *\n";
print "\t\t  * GETTER methods: return the value of the Job's members\n";
print "\t\t  *\n";
print "\t\t  */\n";
foreach (@data_members) {
  chomp;
  $_ =~ s/blobstring/std::string/g;
  @pieces = split(",", $_);
  if($custom_method{"$pieces[2]"} == 1) { next; }
  print "\t\t  " . "virtual " . $pieces[1]. "  " . $pieces[2] . "( void ) const { return m_" . $pieces[2] . "; }\n";
}


print "\n\t\t /**\n";
print "\t\t  *\n";
print "\t\t  * Database field name GETTER methods: return the names of the database column\n";
print "\t\t  *\n";
print "\t\t  */\n";
foreach (@data_members) {
  chomp;
  @pieces = split(",", $_);
  print "\t\t  " . "static std::string " . $pieces[2] . "_field( void ) { return \"" . $pieces[2] . "\"; }\n";
}

##
#
#
# STANDARD CTOR
#
#
##

print "\n\t\t  CreamJob(\n\t  ";

foreach (@data_members) {
  chomp;
  @pieces = split(",", $_);
  $line = "  const " . $pieces[1]. "& " . $pieces[2];
  push @ctor_fieds, $line;
}

print "\t\t" . join(",\n\t\t\t", @ctor_fieds);
print "\n\t\t\t) : ";

foreach ( @members ) {
  push @inits, "m_$_( $_ )";
  push @inits, "m_changed_$_( true )";
}

print "" . join( ",", @inits );
#print " { }\n\n";
print  " , m_new(true) { }\n\n";


##
#
#
# EMPTY CTOR
#
#
##

print "\t\t  CreamJob() : ";

@inits = ();
foreach ( @members ) {
  
  push @inits, "m_$_( " . $defaults{$_} . " )";
  push @inits, "m_changed_$_( true )";
}

print join(",", @inits ) . ", m_new(true) {}\n\n";

##
#
#
# RESET '_CHANGED' FIELD needed when the job is retrieved from database
#
#
##

print "\t\t  void reset_change_flags( void ) {\n\t\t    ";
@inits = ();
foreach ( @members ) {
  push @inits, "m_changed_$_ = false";
}
print join("; \n\t\t    ", @inits ) ;
#. ";\n\t\t  }\n\n";
print ";\n\t\t    m_new = false;";
print "\n\t\t  }\n\n";


@inits = ();
print "\t\t  bool is_to_update( void ) const {\n";
foreach ( @members ) {
  push @inits, "m_changed_$_";
}
print "\t\t    return " . join(" || ", @inits);
print ";\n\t\t  }\n\n";






print "\t\t  static std::string get_query_fields( void ) { \n";

print "\t\t  \t\t\t return \"" . (join ",", @members) . "\"; }\n";

print "\n\n\t\t  std::string get_query_values( void ) const { \n\n";
print "\t\t    string sql;\n";
@values = ();
foreach ( @members ) {

  print "\t\t    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_$_) );\n";
  print "\t\t    sql += \",\";\n";

  push @values, " IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_$_) ) ";
}
print "\t\t    sql = sql.substr(0, sql.length() -1 );\n";
print "\t\t    return sql;\n";
print "\t\t  }\n";

print "\n\n\t\t  static std::string get_createdb_query( void ) { \n";


for($i = 0; $i < scalar @data_members; $i++) {
  $line = $data_members[$i];
  chomp $line;
  @pieces = split(",", $line);
  push @dbmembers, $pieces[2] . " " . $sql_types[$i] . " " . $primary_key{$pieces[2]} . " not null" ;
}


print "\t\t  \t\t\t return \"" . join( ",",  @dbmembers );
print "\"; }\n";


##
#
#
# SQL to update selectively
#
#
##
print "\n\n\t\t  void update_database( std::string& target ) const {\n";
print "\t\t    std::string _sql;\n";

for($i = 0; $i < scalar @data_members; $i++) {
  $line = $data_members[$i];
  chomp $line;
  @pieces = split(",", $line);
  $member = $pieces[2];
  print "\t\t    if(m_changed_$pieces[2]) {\n";
  print "\t\t      _sql += this->$pieces[2]_field( ); \n";
  print "\t\t      _sql += \"=\";\n";
  print "\t\t      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->$pieces[2]( )) );\n";
  print "\t\t      _sql += \",\";\n\n";
  print "\t\t    }\n";
}
print "\n\t\t    if( _sql.empty() ) {return;}\n\n";
print "\t\t    if( boost::ends_with( _sql, \",\") ) { _sql = _sql.substr(0, _sql.length() -1 ); }\n\n";
print "\t\t    _sql += \" WHERE \";\n";
print "\t\t    _sql += this->grid_jobid_field();\n";
print "\t\t    _sql += \"=\";\n";
print "\t\t    _sql += IceUtils::withSQLDelimiters( this->grid_jobid( ) );\n";
print "\t\t    _sql += \";\";\n";
print "\t\t    target = string(\"UPDATE jobs SET \") + _sql;\n";
print "\t\t  }\n";


##
#
#
# CUSTOM METHOD RETRIEVED FROM TXT
#
#
##

$inside_method = 0;
@_data_methods = `cat CreamJob.txt`;

foreach (@_data_methods) {
  chomp;
  if ( ($_ !~ m/^METHOD:/) && ($inside_method==0) ) {next;}
  $inside_method = 1;
  if( $_ =~ m/ENDMETHOD:/) {
    $inside_method = 0;
    next;
  }
  if( $inside_method == 1) {
    if( $_ =~ m/^METHOD:/ ) {
      print "\n";
      $_ =~ s/^METHOD://;
      @pieces = split( ",", $_ );
      $custom_method{$pieces[1]} = 1;
      $_ =~ s/,/ /; ## first replace
      $_ =~ s/,/ /; ## second and last replace
    }
    print "\t\t  $_\n";
  }
  next;
}

print "\n\t\t  virtual ~CreamJob( void ) {\n";
#for($i = 0; $i < scalar @data_members; $i++) {
#  $line = $data_members[$i];
#  chomp $line;
#  @pieces = split(",", $line);
#  $member = $pieces[2];
#  print "\t\t    m_changed_" . $member . " = false;\n";
#}
print"\t\t  }\n";

print "\n\n\t\t}; // class CreamJob";

print "\n        } // namespace util\n      } // namespace ice\n    } // namespace wms\n}; // namespace glite\n";

print "\n#endif\n";
