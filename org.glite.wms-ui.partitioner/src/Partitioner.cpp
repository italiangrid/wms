/***************************************************************************
 *  filename  : Partitioner.cpp
 *  authors   : Alessio Gianelle <gianelle@pd.infn.it>
 *  Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
 *  For license conditions see LICENSE file or
 *  http://www.edg.org/license.html
 ***************************************************************************/

// $Id$

#include <stdlib.h>

#include "glite/wmsui/partitioner/Partitioner.h"

#include "glite/wms/jdl/DAGAdManipulation.h"
#include "glite/wms/jdl/jdl_attributes.h"
#include "glite/wms/jdl/JDLAttributes.h"
#include "glite/wms/jdl/JobAdManipulation.h"
#include "glite/wms/jdl/ManipulationExceptions.h"

#include "glite/wmsutils/exception/Exception.h"
#include "glite/wmsutils/exception/exception_codes.h"

#include <boost/lexical_cast.hpp>

namespace reqad = glite::wms::jdl;
namespace utils = glite::wmsutils::exception;

namespace glite {
namespace wmsui {
namespace partitioner {

Partitioner::Partitioner( const classad::ClassAd* ad, std::vector<std::string> id ) : p_prejob( NULL ), 
										      p_postjob ( NULL ), 
										      p_id ( id ) {
  
  int nstep, cstep;
  std::vector<int> w;
  std::vector<std::string> lstep;
  classad::ClassAd *old = static_cast<classad::ClassAd*>(ad->Copy());
  
  // set the "dag" part
  this->p_dag = new reqad::DAGAd();
  
  try { // the edg_jobid is required
    set_edg_jobid( *this->p_dag, reqad::get_edg_jobid( *old ) );
    reqad::remove_edg_jobid( *old );
  } catch ( reqad::CannotGetAttribute ) { 
    throw utils::Exception(__FILE__, __LINE__, "Partitioner::Partitioner( ... )", utils::WMS_FATAL_ERROR, "NO_EdgJobId");
  }

  try { // the InputSandBox is optional
    std::vector<std::string> isb;
    reqad::get_input_sandbox( *old, isb );
    set_input_sandbox( *this->p_dag, isb );
    reqad::remove_input_sandbox( *old );
  } catch( reqad::CannotGetAttribute ) {}
  
  try { // the MyProxyServer is optional
    set_my_proxy_server( *this->p_dag, reqad::get_my_proxy_server( *old ) );
    reqad::remove_my_proxy_server( *old );
  }  catch( reqad::CannotGetAttribute ) {}
  
  try { // The VO is optional
    set_virtual_organisation( *this->p_dag, reqad::get_virtual_organisation( *old ) );
    reqad::remove_virtual_organisation( *old );
  }  catch( reqad::CannotGetAttribute ) {}

  // set the prejob
  try { // the prejob is optional
		classad::ClassAd *pre = reqad::get_prejob( *old );
		this->p_prejob = new reqad::JobAd( *pre );
		delete pre;
    reqad::remove_prejob( *old );
    this->p_prejob->setAttribute( reqad::JDL::JOBID, this->p_id.back() );
    this->p_id.pop_back(); // delete the last jobid
  } catch( reqad::CannotGetAttribute ) {}
  
  // set the postjob
  try { // the postjob is optional
		classad::ClassAd *post = reqad::get_postjob( *old );
		this->p_postjob = new reqad::JobAd( *post );
		delete post;
    reqad::remove_postjob( *old );
    this->p_postjob->setAttribute( reqad::JDL::JOBID, this->p_id.back() );
    this->p_id.pop_back(); // delete the last jobid
    // remove if set the CHKPT_STEPS attribute
    if ( this->p_postjob->hasAttribute( reqad::JDL::CHKPT_STEPS ) )
      this->p_postjob->delAttribute( reqad::JDL::CHKPT_STEPS );
    // set the correct CHKPT_STEPS attribute (using the edg-jobids of the subjobs)
    for ( int i = 0; i < (int)this->p_id.size(); i++ ) 
      this->p_postjob->addAttribute( reqad::JDL::CHKPT_STEPS, this->p_id[i] );
    // remove if set the JOBTYPE attribute
    if ( this->p_postjob->hasAttribute( reqad::JDL::JOBTYPE ) )
      this->p_postjob->delAttribute( reqad::JDL::JOBTYPE );
    // the post-job must be chkpt
    this->p_postjob->setAttribute( reqad::JDL::JOBTYPE, std::string(JDL_JOBTYPE_CHECKPOINTABLE) );
  } catch ( reqad::CannotGetAttribute ) {}

  // set the Partition attribute (so the real decomposition)
  try { // the Current_Step is optional
    cstep = reqad::get_current_step( *old );
    reqad::remove_current_step( *old );
  } catch ( reqad::CannotGetAttribute  ) {
    cstep = 0;
  }
  
  try { // the Step_Weight is optional
    reqad::get_step_weight( *old, w );
    reqad::remove_step_weight( *old );
  } catch ( reqad::CannotGetAttribute ) {}

  try { // the Job_Steps is required
    nstep = reqad::get_job_steps( *old );
    reqad::remove_job_steps( *old );
    this->p_subdivision = new Partition( nstep, cstep, this->p_id.size() ); 
  } catch ( reqad::CannotGetAttribute ) {
    try { // the stepper is a list of strings
      reqad::get_job_steps( *old, lstep );
      reqad::remove_job_steps( *old );
      if ( w.empty() )
	this->p_subdivision = new Partition( lstep, this->p_id.size() );
      else  // the weight attribute is set
	this->p_subdivision = new Partition( lstep, w, this->p_id.size());
    } catch ( reqad::CannotGetAttribute ) {
      throw utils::Exception(__FILE__, __LINE__, "Partitioner::Partitioner( ... )",  utils::WMS_FATAL_ERROR, "NO_JobSteps");
    }
  }
  // set the subjob (this part of jdl is common for all the subjobs)
  this->p_subjob = new reqad::JobAd( *old );
  if ( this->p_subjob->hasAttribute( reqad::JDL::JOBTYPE ) )
    this->p_subjob->delAttribute( reqad::JDL::JOBTYPE );
  // subjobs must be chkpt
  this->p_subjob->setAttribute( reqad::JDL::JOBTYPE, std::string(JDL_JOBTYPE_CHECKPOINTABLE) );
  
}


  Partitioner::~Partitioner( void ) {
    delete this->p_subdivision;
    delete this->p_subjob;
    delete this->p_dag;
    delete this->p_prejob;
    delete this->p_postjob;
  }
  
  reqad::JobAd Partitioner::setSubJob( std::string ji, int n ) {
  
    reqad::JobAd ret = *this->p_subjob; 
    
    ret.setAttribute( reqad::JDL::JOBID, ji );
    ret.setAttribute( reqad::JDL::CHKPT_CURRENTSTEP, this->p_subdivision->getStepsSet(n)->getCurrentIndex() );
    
    if ( this->p_subdivision->getStepsSet( n )->isInt() ) {
      ret.setAttribute( reqad::JDL::CHKPT_STEPS, this->p_subdivision->getStepsSet(n)->getLastIndex() );
    } else {
      std::vector<std::string>  ltmp = this->p_subdivision->getStepsSet(n)->getLabelList();
      for (int i = 0; i < (int)ltmp.size(); i++)
	ret.addAttribute( reqad::JDL::CHKPT_STEPS, ltmp[i] );
    }
    
    return ret;
  }

  reqad::DAGAd* Partitioner::createDag( void ) {

    std::string            name, ss;
    classad::ClassAdParser parser;
    
    // Postjob
    if ( this->p_postjob ) {
      ss = this->p_postjob->toString();
      reqad::DAGNodeInfo* node = new reqad::DAGNodeInfo( *parser.ParseClassAd( ss ) , "edg-jdl");
      this->p_dag->add_node( "postjob", *node );
      free( node );
    }

    // Prejob
    if ( this->p_prejob ) {
      ss = this->p_prejob->toString();
      reqad::DAGNodeInfo* node = new reqad::DAGNodeInfo( *parser.ParseClassAd( ss ) , "edg-jdl");
      this->p_dag->add_node( "prejob", *node );
      free( node );
    }
      
    // Subjobs
    for ( int i = 0; i < (int)this->p_id.size(); i++ ) {
      ss = this->setSubJob(this->p_id[i], i).toString();
      reqad::DAGNodeInfo* node = new reqad::DAGNodeInfo( *parser.ParseClassAd( ss ) , "edg-jdl");
      name = "subjob"; 
      name.append( boost::lexical_cast<std::string>(i) );
      this->p_dag->add_node( name, *node );
      if ( this->p_prejob ) {
	this->p_dag->add_dependency( "prejob", name );
      }
      if ( this->p_postjob ) {
	this->p_dag->add_dependency( name, "postjob" );
      }
      free( node );
    }
    
    
    return this->p_dag;;
    
  }
  
 
} // namespace partitioner
} // namespace wms
} // namespace glite
