/***************************************************************************
 *  filename  : bin.h
 *  authors   : Alessio Gianelle <gianelle@pd.infn.it>
 *  Copyright (c) 2003 CERN and INFN on behalf of the EU DataGrid.
 *  For license conditions see LICENSE file or
 *  http://www.edg.org/license.html
 ***************************************************************************/

// $Id$

#ifndef GLITE_WMS_PARTITIONER_BIN_H__
#define GLITE_WMS_PARTITIONER_BIN_H__

#include <vector>

namespace glite {
namespace wmsui {
namespace partitioner {

  /** 
 * \brief Provides the Bin class.
 *
 * This class simulate the container in which we store the jobs.
 *
 **/
  
  class Bin {

  public:
    /** \name Constructor/Destructor */
    /// @{
    /** Construct the object inserting an element.  
     * \param capacity The maximum capacity of the bin.
     * \param name The "name" of the first element. 
     * \param weight The weight of the inserted element. */
     
    Bin( int capacity, int name, int weight ): b_capacity ( capacity ), b_weight ( weight )
      { this->b_element.push_back( name ); } 
      
    /// @}
    
    /** \name Miscellaneous methods. */
    /// @{
    /** Insert an element .
     * \param name The "name" of the element. 
     * \param weight The weight of the element. */
    inline void insert( int name, int weight ) {
      this->b_element.push_back( name );
      this->b_weight += weight;
    }
    
    /** Return the amount of free space.
     * \retval An integer equal to the free space of the bin.*/
    inline int freespace( void ) { return ( this->b_capacity - this->b_weight ); }
    
    /** Return the content of the bin.
     * \retval A vector of integer with the "name" of the elements stored
     * inside the bin. */
    inline std::vector<int> getelement( void ) { return this->b_element; }
    
    /// @}

  private:
    
    int               b_capacity;  /**< The capacity of the bin. */
    int               b_weight;    /**< The total weight if the bin. */
    std::vector<int>  b_element;   /**< The "name" of the elements stored in the bin. */
    
  };

} // namespace partitioner
} // namespace wms
} // namespace glite

#endif //  GLITE_WMS_PARTITIONER_BIN_H__
