/* **************************************************************************
 *  filename  : JobId.cpp
 *  author    : Alessandro Maraschini <alessandro.maraschini@datamat.it>
 *  copyright : (C) 2002 by DATAMAT
 ***************************************************************************/

#include "glite/wmsutils/jobid/JobId.h"

#include <iostream>

#include "glite/wmsutils/jobid/JobIdExceptions.h"

namespace glite {
namespace wmsutils {
namespace jobid {

using namespace std ;

/******************************************************************
 Constructor / Destructor
 *******************************************************************/
JobId::JobId() : m_JobId( 0 ), m_pStr( 0 ), m_pBkserver( 0 ), m_pUnique( 0 )
{
}

JobId::JobId(const std::string& job_id_string )
    : m_JobId( 0 ), m_pStr( 0 ), m_pBkserver( 0 ), m_pUnique( 0 )
{
    fromString( job_id_string ) ;
}

JobId::JobId(const JobId &old)
{
	edg_wlc_JobIdDup(old.m_JobId,&m_JobId);
	m_pStr = old.m_pStr ? strdup(old.m_pStr) : 0;
	m_pBkserver = old.m_pBkserver ? strdup(old.m_pBkserver) : 0;
	m_pUnique = old.m_pUnique ? strdup(old.m_pUnique) : 0;
}

JobId & JobId::operator=(JobId const &old)
{
	clear();
	edg_wlc_JobIdDup(old.m_JobId,&m_JobId);
	m_pStr = old.m_pStr ? strdup(old.m_pStr) : 0;
	m_pBkserver = old.m_pBkserver ? strdup(old.m_pBkserver) : 0;
	m_pUnique = old.m_pUnique ? strdup(old.m_pUnique) : 0;

	return *this;
}


JobId::JobId(const edg_wlc_JobId &old)
	: m_pStr(0), m_pBkserver(0), m_pUnique(0)
{
	edg_wlc_JobIdDup(old,&m_JobId);
}


JobId & JobId::operator=(const edg_wlc_JobId &old)
{
	clear();
	edg_wlc_JobIdDup(old,&m_JobId);
	m_pStr = 0;
	m_pBkserver = 0;
	m_pUnique = 0;
	return(*this);
}

JobId::~JobId()
{
    clear();
}

/******************************************************************
 method : clear
 unsets the JobId instance.
 *******************************************************************/
void JobId::clear()
{
    if ( m_JobId )
    {
	edg_wlc_JobIdFree( m_JobId );
	m_JobId = 0;
	if (m_pStr)
            free(m_pStr);
	if (m_pBkserver)
            free(m_pBkserver);
	if (m_pUnique)
	    free(m_pUnique);
        m_pStr = m_pBkserver = m_pUnique = NULL;
    }
}


/******************************************************************
 method :    setJobId
 sets the JobId instance according to the LB and RB
 server addresses and the unique string passed as input parameters.
 *******************************************************************/
void JobId::setJobId(const string& bkserver, int port, const string& unique)
{
    int code = edg_wlc_JobIdRecreate(bkserver.c_str(), port, unique.size() ? unique.c_str() : NULL, &m_JobId) ;
    if (  code != 0 )
	throw WrongIdException(__FILE__ , __LINE__ ,  "setJobId(const string& bkserver, int port, const string& unique)" , code  )  ;
}


/******************************************************************
 Protected method :    fromString
 sets the JobId instance from the dg_jobId in string format given as input.
 *******************************************************************/
void JobId::fromString (const string& dg_JobId)
{
    clear();
    int code = edg_wlc_JobIdParse(dg_JobId.c_str(), &m_JobId)  ;
    if  ( code != 0 )
	throw WrongIdException(__FILE__ , __LINE__ , "fromString (const string& dg_JobId)" , code )  ;
}

/******************************************************************
 method :    ToString
 converts the JobId instance into its string format.
 and put it in the dg_jobId output variable
 *******************************************************************/
std::string JobId::toString() const
{
    if ( m_JobId && !m_pStr )
	m_pStr = edg_wlc_JobIdUnparse(m_JobId) ;
    if ( !m_pStr )
	throw EmptyIdException (__FILE__ , __LINE__ ,"toString()" ,ENOENT , "JobId")  ;
    return m_pStr;
}

/******************************************************************
 method :   getServer
 return a string containing the LB server address,
 *******************************************************************/
std::string JobId::getServer() const
{
    if ( m_JobId && !m_pBkserver )
	m_pBkserver = edg_wlc_JobIdGetServer( m_JobId ) ;

    if ( !m_pBkserver )
	throw EmptyIdException (__FILE__ , __LINE__ , "getServer()",  ENOENT , "LB server Address")  ;

    return m_pBkserver;
}

/******************************************************************
 method :   getUnique
 return a string containing unique jobid string
 *******************************************************************/
std::string JobId::getUnique() const
{
    if ( m_JobId && !m_pUnique )
	m_pUnique = edg_wlc_JobIdGetUnique( m_JobId ) ;

    if ( !m_pUnique )
	throw EmptyIdException (__FILE__ , __LINE__ , "getUnique()" ,  ENOENT , "Unique")  ;

    return m_pUnique;
}
/******************************************************************
 method :   getId
 return the c JobId struct representing this instance
 *******************************************************************/
edg_wlc_JobId  JobId::getId() const
{
	edg_wlc_JobId out ;
	if ( edg_wlc_JobIdDup(m_JobId,  &out)  )
		throw EmptyIdException (__FILE__ , __LINE__ , "getId()" ,  ENOENT , "JobId")  ;
	return out ;
}

std::ostream&
operator<<(std::ostream& os, JobId const& id)
{
  return os << id.toString();
}

} // namespace jobid
} // namespace wmsutils
} // namespace glite 
