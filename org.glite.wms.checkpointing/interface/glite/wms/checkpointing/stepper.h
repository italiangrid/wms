// File: stepper.h
// Author: Alessio Gianelle <gianelle@pd.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

/**
 * \file
 * \brief Provides the StepsSet object. 
 *
 * \version 0.1
 * \date 18 December 2002
 * \author Alessio Gianelle <gianelle@pd.infn.it>
*/

#ifndef GLITE_WMS_CHECKPOINTING_STEPPER_H
#define GLITE_WMS_CHECKPOINTING_STEPPER_H

#include <vector>
#include <string>

namespace glite {
namespace wms {
namespace checkpointing {

  /**
   *  \brief Provides the iterator.
   *  There could be two types of iterators: a numeric stepper and a string stepper.
   * 
   *  The numeric iterator is defined through a numeric range which is
   *  described through two number: the last (the biggest) number, and
   *  the current step. In this case the iterator represents all the
   *  naturals numbers between the current step and the last one.
   *  Otherwise an iterator could be described through a list of
   *  string, if this is the case every steps is identified through a
   *  string label.  */
  
  class StepsSet {
    
  public:
    /** Provides the type of the iterator. */
    enum iterator_type { 
      integer,    /**< Identify a numeric iterator. */
      label       /**< Identify a string iterator. */
    }; 
    
    /** \name Constructors Destructor */ 
    /// @{
    /** Instantiates a StepsSet object with a string type iterator.
	\param llabel - A vector of strings that define all the names of the labels.
	\param cstep - The index of the \a current step. \b Default value is 0, which
        means the first label. */
    StepsSet( const std::vector<std::string>& llabel, int cstep = 0 );   
    /** Instantiates a StepsSet object with a numeric type iterator.
	\param last  - An int with the last step. 
	\param cstep - The index of the \a current step. \b Default value is 0. */
    StepsSet( int last, int cstep = 0);
    /// @}

    /** \name Initialize methods */
    /// @{
    /** Initialize a StepsSet object from a vector of strings. 
	\param llabel - A vector of strings that define all the names of the labels.
	\param cstep - The index of the \a current step. */
    void initialize( const std::vector<std::string>& llabel, int cstep );
    /** Initialize a StepsSet object as a numeric type iterator.
	\param last - An int with the last step. 
	\param step - The index of the \a current step. */
    void initialize( int last, int step );
    /// @}

    /** \name Get Methods */
    /// @{
    /** Gets the value of the next numeric step. 
	\return An int with the value of the next step.
	\throw  EoSException - When the range is finished. */
    int getNextInt( void ); 
    /** Gets the value of the next label (string) step. 
	\return A string with the value of the next step.
	\throw  EoSException - When the range is finished. */
    const std::string getNextLabel( void );
    /** Gets the value of the current numeric step. 
	\return An int with the value of the current step. 
	\throw  EoSException - When the range is finished. */
    int  getCurrentInt( void );
    /** Gets the value of the current label (string) step. 
	\return A string with the value of the current step.
	\throw  EoSException - When the range is finished. */
    const std::string getCurrentLabel(void );
    /// @}

    /** \name Check-Type methods */
    /// @{
    /** Check if the iterator is a numeric one.
	\return @a true if the iterator is numeric; @a false otherwise. */ 
    inline bool isInt( void ) { return ( this->ss_ittype == integer ); }
    /** Check if the iterator is a list of strings labels.
	\return @a true if the iterator is a list of strings labels; @a false otherwise. */ 
    inline bool isLabel( void ) { return ( this->ss_ittype == label ); }
    /// @}

    /** \name Miscellaneous methods */
    /// @{
    /** Clean the list of labels */
    inline void clear( void ) { this->ss_steps.clear(); } 
    /** Get the <em>numeric index</em> of the current label.
	\return An int with the index of the current label. */
    inline int  getCurrentIndex( void ) { return this->ss_current; }
    /** Get the last index.
	\return An int with the last index. */
    inline int getLastIndex( void ) { return this->ss_last; }
    /** Get the vector of labels.
	\return A vector if strings with the list of labels. */
    inline std::vector<std::string> getLabelList( void ) { return this->ss_steps; }
    /** Reset the iterator setting it equal to the first index. */
    inline void reset( void ) { this->ss_current = this->ss_first; } 
    /// @}
	
  private:
    int                      ss_first;    /**< The first index of the iterator. */
    int                      ss_last;     /**< The last index of the iterator. */
    int                      ss_current;  /**< The numeric index of the current step (It is equal to the label in a numeric iterator). */
    iterator_type            ss_ittype;   /**< The type of the iterator. */
    std::vector<std::string> ss_steps;    /**< The vector with the list of the strings labels (Not used in a numeric iterator). */
    
  };
  
} // checkpointing
} // wms
} // glite

#endif // GLITE_WMS_CHECKPOINTING_STEPPER_H

//  Local Variables:
//  mode: c++
//  End:
