#ifndef GLITE_WMSUTILS_JOBID_JOBID_H
#define GLITE_WMSUTILS_JOBID_JOBID_H

/*
 * JobId.h
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 *
 */

#include <string>
#include <iosfwd>

#include "glite/wmsutils/jobid/cjobid.h"

typedef struct _edg_wlc_jobid_s* edg_wlc_jobid_t;

namespace glite { 
namespace wmsutils {
namespace jobid {

/**
 * Managing Identification, checking, retreiving info from a job
 * File name: JobId.h
 * The JobId class provides a representation of the Datagrid job identifier
 * (dg_jobId) and the methods for manipulating it.
 * We remind that the format of the dg_jobId is as follows:
 * <LB address>:<LB port>/<Unique String>
 *
 * @ingroup common
 * @version 0.1
 * @date 15 April 2002
 * @author Alessandro Maraschini <alessandro.maraschini@datamat.it>  */ 

class JobId {
public:
    /**@name Constructors/Destructor */
    //@{
    /** Instantiates an empty  JobId object */
    JobId() ;
    /**
     * Instantiates a JobId object from the passed dg_jobId in string format.
     * @param  job_id_string a string representig a classAd expression
     * @throws  WrongIdException When a string is passed in a wrong format
     */
    JobId(const std::string& job_id_string);
    JobId(const JobId&);
    JobId(const edg_wlc_JobId&);
    /**
     * Destructor
     * Destroy the Job Id instance
     */
    ~JobId() ;
  //@}

    /**@name Miscellaneous  */
    //@{
    /** Unsets the JobId instance. Clear all it's memebers */
    void clear() ;
    /**
     * Check wheater the jobId has been already created (true) or not (false)
     *@return  true (jobId created) or false (jobId not yet created)
     */
    bool isSet() { return ( m_JobId != 0 ) ; }
    /**
     * Set the JobId instance according to the LB and RB server addresses and the unique string passed as input parameters.
     * @param lb_server  Loggin and Bookkeeping server  address
     * @param port Loggin and Bookkeeping port ( dafault value is 9000 )
     * @param  unique A Unique identification ( automatically generatad by md5 protocol )
     * @throws  WrongIdException  When one parameter has been passed in a wrong format  */
    void setJobId(const std::string& lb_server, int port = 0, const std::string& unique = "");
  //@}
    /**@name Get Methods */
  //@{
    /** @return the LB address  into its string format
    * @throws  EmptyIdException  If the jobId has not been initialised yet */
    std::string getServer() const;
    /** @return the Unique string into its string format
    * @throws  EmptyIdException  If the jobId has not been initialised yet */
    std::string getUnique() const;
  //@}
    /** This method sets the JobId instance from the JobId in string format given
    * as input.
    * @param dg_JobId the string representing the job
    * @throws  WrongIdException  When a string is passed in a wrong format */
    void fromString ( const std::string& dg_JobId );
    /** Converts the jobId into a string
    @return the string representation of a JobId*/
    std::string toString() const;
    /** casting operator */
    operator const edg_wlc_JobId() const { return m_JobId; }
    /** Operator "=" create a deep copy of the JobId instance*/
    JobId & operator=(JobId const &);
    /** Operator "=" create a deep copy of the JobId instance*/
    JobId & operator=(const edg_wlc_JobId &);
    /** Retrieve the internal id reference 
    *@return the JobId internal reference used by some LB methods */
    edg_wlc_JobId  getId() const ;
private:
    // This Variable stores the Job unique identification String
    edg_wlc_JobId m_JobId;
    mutable char* m_pStr;
    mutable char* m_pBkserver;
    mutable char* m_pUnique;
    /** Operator "<"*/
    friend bool operator<(JobId const& lhs, JobId const& rhs);
    /** Operator "=="*/
    friend bool operator==(JobId const& lhs, JobId const& rhs);
};

inline bool operator<(JobId const& lhs, JobId const& rhs)
{
  return    strcmp ( lhs.m_pStr , rhs.m_pStr )  <0 ;
}

inline bool operator==(JobId const& lhs, JobId const& rhs)
{
return     strcmp ( lhs.m_pStr , rhs.m_pStr ) ==0 ;
}

std::ostream& operator<<(std::ostream& os, JobId const& id);

} // namespace jobid
} // namespace wmsutils
} // namespace glite

#endif // GLITE_WMSUTILS_JOBID_JOBID_H
