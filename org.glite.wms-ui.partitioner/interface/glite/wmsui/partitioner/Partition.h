/***************************************************************************
 *  filename  : Partition.h
 *  authors   : Alessio Gianelle <gianelle@pd.infn.it>
 *  Copyright (c) 2003 CERN and INFN on behalf of the EU DataGrid.
 *  For license conditions see LICENSE file or
 *  http://www.edg.org/license.html
 ***************************************************************************/

// $Id$

/**
 * \file
 * \brief Provides the Partition object. 
 *
 * \version 0.1
 * \date 21 March 2003
 * \author Alessio Gianelle <gianelle@pd.infn.it>
*/

#ifndef GLITE_WMS_PARTITIONER_PARTITION_H
#define GLITE_WMS_PARTITIONER_PARTITION_H

#include <string>
#include <vector>

// glite include
#include <glite/wms/checkpointing/stepper.h>

namespace chkpt = glite::wms::checkpointing;

namespace glite {
namespace wmsui {
namespace partitioner {

/** 
 * \brief Provides the Partition class.
 *
 * This class is the core of the job partitioning service. It is responsible
 * for the decomposition of the initial JobSteps range into n sublists. A lot
 * of strategies should be used considering the hints of the user (the 
 * StepWeight attribute) and the number of suitable CEs (at the moment of the 
 * partinioning).     
 *
 **/
  
  class Partition {

  public:

    /** \name Constructor/Destructor */
    /// @{
    /** Construct the object from a "numeric" Stepper 
     * \param last is the max value of the range
     * \param first id the min value of the range 
     * \param el the number of sublists
     * \warning For "numeric" stepper the user could not provide the "StepWeight" value. */
    Partition( int last, int first, int el );  
    /** Construct the object from a "label" Stepper
     * \param llabel is a vector of strings contains the labels
     * \param weight is a vector of integers with the "weights" of the labels
     * \param el the number of sublists
     * \warning The two vectors must have the same number of elements. */
    Partition( const std::vector<std::string>& llabel, const std::vector<int>& w, int el );
    /** Construct the object from a "label" Stepper
     * \param llabel is a vector of strings contains the labels
     * \param el the number of sublists
     * \warning The "weights" vector is optional  */
    Partition( const std::vector<std::string>& llabel, int el );

    /** Desctructor. */
    ~Partition();
    
     /// @}

     /** \name Miscellaneous methods. */
    /// @{
    /** Returns the number of sublists created 
     * \retval The value of the member "element" */
    inline int getElement( void ) { return this->element; }
    /** Returns the "index"th sublist created as a StepsSet object
     * \param index an integer with the index of the required subrange.
     * \retval A StepsSet object initialized with the index-th subrange computed. */
    inline chkpt::StepsSet* getStepsSet( int index ) { return this->decomposition[index]; }
    /** Returns the total number of steps
     *  \retval A integer equal to the total number of steps */
    inline int getTotSteps( void ) { 
      this->total->reset(); // CurrentIndex should be changed!
      return this->total->getLastIndex() - this->total->getCurrentIndex() + 1; 
    }
    /** Decompose the initial Stepper in sub-set of steps each one with the
     *  same number of elements. 
     *	\warning It sets the decomposition members */
    void decompose( void );
     /** Decompose the initial Stepper in sub-set of steps using the weights vector.
	 It calls an external function to optimize the decomposition.
     *	\warning It sets the decomposition members */
   void wdecompose( void );
        
    /// @}

  private:
    int                           element;       /**< The number of sublists. */
    chkpt::StepsSet              *total;         /**< Initial list of steps. */
    std::vector<int>              weights;       /**< The weights of the steps. */
    std::vector<chkpt::StepsSet*> decomposition; /**< The computed partition. */
  };
 
} // namespace partitioner
} // namespace wms
} // namespace glite

#endif // GLITE_WMS_PLANNING_PARTITIONER_PARTITION_H
