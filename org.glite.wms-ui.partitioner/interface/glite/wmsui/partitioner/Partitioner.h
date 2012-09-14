/***************************************************************************
 *  filename  : Partitioner.h
 *  authors   : Alessio Gianelle <gianelle@pd.infn.it>
 *  Copyright (c) 2003 CERN and INFN on behalf of the EU DataGrid.
 *  For license conditions see LICENSE file or
 *  http://www.edg.org/license.html
 ***************************************************************************/

// $Id$

/**
 * \file
 * \brief Provides the Partitioner Helper. 
 *
 * \version 0.1
 * \date 21 March 2003
 * \author Alessio Gianelle <gianelle@pd.infn.it>
*/

#ifndef GLITE_WMS_PARTITIONER_PARTITIONER_H
#define GLITE_WMS_PARTITIONER_PARTITIONER_H

// glite include
#include "glite/wmsui/partitioner/Partition.h"
#include "glite/wms/jdl/JobAd.h"
#include "glite/wms/jdl/DAGAd.h"

// classAd include files
#include "classad_distribution.h"

namespace reqad = glite::wms::jdl;

namespace glite {
namespace wmsui {
namespace partitioner {

class Partitioner {

 public:
  /**
   * Constructor.
   * \param ad the original ClassAd of the incoming job.
   * \param id a vector with the edg-jobids of all the subjobs.
   * \throw wmsutils::exception::Exception if the StepsSet or the edg_jobid are not set.
   * \warning the edg-jobid of the dag itself and the StepsSet attribute must be set in the classad (ad).
   */
  Partitioner( const classad::ClassAd* ad, std::vector<std::string> id);
  /**
   * Destructor.
   */
  ~Partitioner();
  
   /**
   * Create the dag.
   * \retval a DAGAd with the created DAG.
   */
  reqad::DAGAd *createDag( void );

 private:

  /**
   * Define the Subjob.
   * \param ji The edg-jobid of the subjob
   * \param index This index is used to individuate the right subrange.
   * \retval A JobAd with the classAd for the submission of the subjobs.  
   */
  reqad::JobAd setSubJob( std::string ji, int index );
 
  Partition                *p_subdivision; /**< Partition of the initial JobSteps.*/
  reqad::JobAd             *p_prejob;      /**< The PreJob definition if it exists. */
  reqad::JobAd             *p_postjob;     /**< The PostJob (or aggregator) definition if it exists. */
  reqad::JobAd             *p_subjob;      /**< The SubJob's generic definition. */
  reqad::DAGAd             *p_dag;         /**< The dag. */
  std::vector<std::string>  p_id;          /**< A list with the edg-jobid of all the jobs. */
  
};
 
} // namespace partitioner
} // namespace wms
} // namespace glite

#endif // GLITE_WMS_PLANNING_PARTITIONER_PARTITIONER_H
