// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

#include <classad_distribution.h>

#include <boost/lexical_cast.hpp>

#include "glite/jdl/PrivateAdManipulation.h"

#include "pointer_id.h"
#include "constants.h"
#include "files.h"

using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

const char   *PointerId::pi_s_CondorId = "CondorId", *PointerId::pi_s_EdgId = "EdgId";
const char   *PointerId::pi_s_SequenceCode = "SequenceCode", *PointerId::pi_s_CondorStatus = "CondorStatus";
const char   *PointerId::pi_s_LastStatus = "LastStatus", *PointerId::pi_s_RetryCount = "RetryCount";

PointerId::PointerId( void ) : pi_position(), pi_condorId(), pi_edgId()
{}

PointerId::PointerId( FileList::iterator &position ) : pi_condorStatus( 0 ),
						       pi_lastStatus( static_cast<int>(undefined_status) ),
						       pi_retryCount( 0 ),
						       pi_position( position ),
						       pi_condorId(), pi_edgId(), pi_sequenceCode()
{
  classad::ClassAd       value( *position );

  value.EvaluateAttrString( pi_s_CondorId, this->pi_condorId );
  value.EvaluateAttrString( pi_s_EdgId, this->pi_edgId );
  value.EvaluateAttrString( pi_s_SequenceCode, this->pi_sequenceCode );
  value.EvaluateAttrNumber( pi_s_CondorStatus, this->pi_condorStatus );
  value.EvaluateAttrNumber( pi_s_LastStatus, this->pi_lastStatus );
  value.EvaluateAttrNumber( pi_s_RetryCount, this->pi_retryCount );
}

PointerId::PointerId( FileList::iterator &position, const string &condorId,
		      const string &edgId, const string &seqcode ) : pi_condorStatus( 0 ),
								     pi_lastStatus( static_cast<int>(undefined_status) ),
								     pi_retryCount( 0 ),
								     pi_position( position ),
								     pi_condorId( condorId ), pi_edgId( edgId ),
								     pi_sequenceCode( seqcode )
{}

PointerId::PointerId( const string &condorId, const string &edgId ) : pi_condorStatus( 0 ),
								      pi_lastStatus( static_cast<int>(undefined_status) ),
								      pi_retryCount( 0 ),
								      pi_position(), pi_condorId( condorId ),
								      pi_edgId( edgId ), pi_sequenceCode()
{}

PointerId::~PointerId( void ) {}

PointerId &PointerId::reset( FileList::iterator &position )
{
  classad::ClassAd     value;

  this->pi_position = position;
  value.Update( *position );

  value.EvaluateAttrString( pi_s_CondorId, this->pi_condorId );
  value.EvaluateAttrString( pi_s_EdgId, this->pi_edgId );
  value.EvaluateAttrString( pi_s_SequenceCode, this->pi_sequenceCode );
  value.EvaluateAttrNumber( pi_s_CondorStatus, this->pi_condorStatus );
  value.EvaluateAttrNumber( pi_s_LastStatus, this->pi_lastStatus );
  value.EvaluateAttrNumber( pi_s_RetryCount, this->pi_retryCount );

  return *this;
}

PointerId &PointerId::reset( const std::string &condorId, const std::string &edgId )
{
  this->pi_position.reset();

  this->pi_condorId.assign( condorId );
  this->pi_edgId.assign( edgId );

  return *this;
}

PointerId &PointerId::reset( FileList::iterator &position, const string &seqcode )
{
  this->pi_position = position;
  this->pi_sequenceCode.assign( seqcode );

  return *this;
}

const classad::ClassAd &PointerId::job_ad( void )
{
  if( !this->pi_jobAd ) {
    classad::ClassAd          *tmp;
    jccommon::Files            files( this->pi_edgId );
    string                     adfile( files.classad_file().native_file_string() );
    ifstream                   ifs( adfile.c_str() );
    classad::ClassAdParser     parser;

    tmp = parser.ParseClassAd( ifs );

    this->pi_jobAd.reset( tmp ? tmp : new classad::ClassAd() );
  }

  return *this->pi_jobAd;
}

string PointerId::proxy_file( void )
{
  bool                       good;
  const classad::ClassAd    &jobad = this->job_ad();
  string                     file( glite::jdl::get_x509_user_proxy(jobad, good) );

  if( !good ) file.erase();

  return file;
}

EdgId::EdgId( list<PointerId>::iterator &position ) : ei_position( position ), ei_edgId( position->edg_id() )
{}

EdgId::EdgId( const EdgId &id ) : ei_position( id.ei_position ), ei_edgId( id.ei_edgId )
{}

EdgId::~EdgId( void ) {}

EdgId &EdgId::operator=( const EdgId &that )
{
  if( this != &that ) {
    this->ei_position = that.ei_position;
    this->ei_edgId = that.ei_edgId;
  }

  return *this;
}

CondorId::CondorId( list<PointerId>::iterator &position ) : ci_position( position ), ci_condorId( position->condor_id() )
{}

CondorId::CondorId( const CondorId &id ) : ci_position( id.ci_position ), ci_condorId( id.ci_condorId )
{}

CondorId::~CondorId( void ) {}

CondorId &CondorId::operator=( const CondorId &that )
{
  if( this != &that ) {
    this->ci_position = that.ci_position;
    this->ci_condorId = that.ci_condorId;
  }

  return *this;
}

} // namespace common
} JOBCONTROL_NAMESPACE_END
