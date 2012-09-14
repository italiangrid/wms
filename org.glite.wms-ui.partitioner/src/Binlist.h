/***************************************************************************
 *  filename  : binlist.h
 *  authors   : Alessio Gianelle <gianelle@pd.infn.it>
 *  Copyright (c) 2003 CERN and INFN on behalf of the EU DataGrid.
 *  For license conditions see LICENSE file or
 *  http://www.edg.org/license.html
 ***************************************************************************/

// $Id$

#ifndef GLITE_WMS_PARTITIONER_BINLIST_H__
#define GLITE_WMS_PARTITIONER_BINLIST_H__

#include "Bin.h"

namespace glite {
namespace wmsui {
namespace partitioner {

  /** 
 * \brief Provides the BinList class.
 *
 * This class is used to implement the Bin Packing algorithm (1-dimension).
 *
 **/
  
  class BinList {

  public:
    /** \name Constructor/Destructor */
    /// @{
    /** Default Constructor. 
     * \param capacity The capacity of the bins. */ 
    BinList( int capacity );
      
    /** Desctructor. */
    ~BinList();
    
    /// @}
    
    /** \name Insert methods. */
    /// @{
    /** Insert an element in the right Bin or create a new ones .
     * \param name The "name" of the element.
     * \param weight The weight of the element. 
     * \warning Insert the element creating a new Bin. */
    void insert( int name, int weight );
    /** \warning Insert the element using the FirstFit algorithm */
    void insertFF( int name, int weight ); 
    /** \warning Insert the element using the BestFit algorithm  */
    void insertBF( int name, int weight ); 
    /** \warning Insert the element using the WorstFit algorithm  */
    void insertWF( int name, int weight );
    /** \warning Insert the element using the AlmostWorstFit algorithm  */
    void insertAWF( int name, int weight );
    /** \warning Insert the element using the RandomFit algorithm  */
    void insertRF( int name, int weight );
    /// @}

    
    /** \name Miscellaneous methods. */
    /// @{

    /** Return the capacity of the Bins in the list. 
     * \retval the max capacity of the Bins in the list */
    inline int getcapacity( void ) { return this->bl_capacity; }

   /** Return the number of the Bins in the list. 
     * \retval the number of the Bins in the list */
    inline int getnumber( void ) { return this->bl_element.size(); }

    /** Return a Bin of the list.
     * \param index The index of the Bin.
     * \retval The requested Bin or a NULL pointer. */
    inline Bin* getbin( int index ) { 
      if ( index < this->getnumber() ) 
	return this->bl_element[ index ]; 
      else return NULL;
    }

    /** Reinitialize the List.
     * \param capacity The capacity of the bins. 
     * \warning It deletes all the old bins. */
    void reset( int capacity );

    /// @}

  private:
    
    int                bl_capacity; /**< Capacity of teh bins. */
    std::vector<Bin*>  bl_element;  /**< Pointers to the bins stored in the list. */
    
  };


} // namespace partitioner
} // namespace wmsui
} // namespace glite

#endif //  GLITE_WMS_PARTITIONER_BINLIST_H__
