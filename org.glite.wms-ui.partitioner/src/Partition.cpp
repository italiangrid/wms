
/***************************************************************************
 *  filename  : Partition.cpp
 *  authors   : Alessio Gianelle <gianelle@pd.infn.it>
 *  Copyright (c) 2003 CERN and INFN on behalf of the EU DataGrid.
 *  For license conditions see LICENSE file or
 *  http://www.edg.org/license.html
 ***************************************************************************/

// $Id$
#include <cmath>
#include <map>
#include "glite/wmsui/partitioner/Partition.h"
#include "Binlist.h"
#include "glite/wms/checkpointing/ChkptException.h"

namespace chkpt = glite::wms::checkpointing; 

namespace glite {  
namespace wmsui {
namespace partitioner {

// Constructors-destructor

  Partition::Partition( int last, int first, int el ) : element ( el ), weights() {
    this->total = new chkpt::StepsSet( last, first );
    if ( this->getTotSteps() < this->element ) this->element = this->getTotSteps();
    this->decompose();
  }

  // uhm... Could an "integer stepper" have a vector of "weights" ?
	
  Partition::Partition( const std::vector<std::string>& llabel, const std::vector<int>& w, int el ) 
    : element( el ), weights( w ) {
    
    this->total = new chkpt::StepsSet( llabel );
    if ( this->getTotSteps() < this->element ) this->element = this->getTotSteps();
    this->wdecompose(); // called the right function
  }
  
  Partition::Partition( const std::vector<std::string>& llabel, int el ) 
    : element( el ), weights() {
    
    this->total = new chkpt::StepsSet( llabel );
    if ( this->getTotSteps() < this->element ) this->element = this->getTotSteps();
    this->decompose();
  }
  
  Partition::~Partition() {
    
    delete this->total;
    if ( this->decomposition.size() != 0 )
      for ( int i = 0; i < this->element; i++ )
	delete this->decomposition[i];
  }


  void Partition::wdecompose( void ) {
    bool done = 0;
    int i, c, totw = 0; 
    std::vector<std::string> ll  = this->total->getLabelList();
    std::map<int, std::vector<std::string> > res;
    std::multimap<int, int> prob; // use a multimap so the element are automatically sorted.
    std::multimap<int, int>::reverse_iterator iter; // we need a decreasing order.

    // Compute the starting capacity for the bins (the best one!).
    int maxw = this->weights[0];
    for (i = 0; i < this->getTotSteps(); i++) {
      prob.insert( std::multimap<int, int>::value_type( this->weights[i], i ) );
      totw += this->weights[i];
      if ( this->weights[i] > maxw ) maxw = this->weights[i];
    }
    // Use the average weight.
    c = static_cast<int>(ceil( static_cast<double>( totw ) / this->element ));
    // If an element is weighter than the average, use its weight!
    if ( c < maxw ) c = maxw; 

    BinList bl( c );

    // If we use as starting capacity the max weight, probably the algorithm use
    // a number of bins lesser than the requested CEs. So forced the algorithm to 
    // use in any case a number of bin equal to "this->element".
    if ( c == maxw ) {
      for( iter = prob.rbegin(), i = 0; i < this->element; ++iter, i++ ) 
	bl.insert(iter->second, iter->first);
      prob.erase( iter.base(), prob.end() );
    }

    // The "real" algorithm (use the FirstFit rule).
    while ( !done ) {
      for( iter = prob.rbegin(); iter != prob.rend(); ++iter ) 
	bl.insertFF( iter->second, iter->first);
      if ( bl.getnumber() == this->element ) done = 1;
      else bl.reset( ++c );
    }
    
    // Set this->decomposition value.
    for ( i=0; i < this->element; i++)
      for (int j=0; j < (int)bl.getbin(i)->getelement().size(); j++ ) 
	res[ i ].push_back( ll[ bl.getbin(i)->getelement()[j] ] ); 
  
    for ( i=0; i < this->element; i++ ) 
      this->decomposition.push_back( new chkpt::StepsSet( res[ i ] ) );
   
  }
 
  void Partition::decompose( void ) {
   
    int m = static_cast<int>(ceil( static_cast<double>( this->getTotSteps() ) / this->element )); // elements per subjob
    int f = this->total->getCurrentIndex();; // index of the first element of the range
    int tot = this->getTotSteps(); // partial total
    
    std::vector<std::string> tmp;
    std::vector<std::string> ll = this->total->getLabelList();
    
    for ( int i = 0; i < this->element; i++ ) {
      if ( this->total->isInt() ) 
	this->decomposition.push_back( new chkpt::StepsSet( f + m - 1, f ) );
      else {
	tmp.assign( &ll[f], &ll[f + m] );
	this->decomposition.push_back( new chkpt::StepsSet( tmp ) );
      }
      f += m; tot -= m;
      m = static_cast<int>(ceil( static_cast<double>( tot ) / (  this->element - i - 1 ) ));
    }
  }

 
} // namespace partitioner
} // namespace wms
} // namespace glite
