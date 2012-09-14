/***************************************************************************
 *  filename  : BinList.cpp
 *  authors   : Alessio Gianelle <gianelle@pd.infn.it>
 *  Copyright (c) 2003 CERN and INFN on behalf of the EU DataGrid.
 *  For license conditions see LICENSE file or
 *  http://www.edg.org/license.html
 ***************************************************************************/

// $Id$

#include "Binlist.h"
#include <cstdlib>
#include <ctime>

namespace glite {
namespace wmsui {
namespace partitioner {

  BinList::BinList( int capacity ) : bl_capacity( capacity ) {};

  BinList::~BinList() {
    for ( int i = 0; i < (int)this->bl_element.size(); i++ )
      delete this->bl_element[i];
  }

  void BinList::insert( int name, int weight ) {
    this->bl_element.push_back( new Bin( this->bl_capacity, name, weight ) );
  }
    
  
  void BinList::insertFF( int name, int weight ) {
    bool done = 0;
    int i = 0;

    while ( ( !done ) && ( i < (int)this->bl_element.size() ) ) {
      if ( this->bl_element[i]->freespace() >= weight ) {
	this->bl_element[i]->insert( name, weight );
	done = 1;
      }
      else i++;
    }
    if ( !done )
      this->bl_element.push_back( new Bin( this->bl_capacity, name, weight ) );
  }
  
  void BinList::insertBF( int name, int weight ) {
    int tmp, i;
    int best = -1;
    int fsbest = this->bl_capacity;

    for ( i = 0; i < (int)this->bl_element.size(); i++ ) {
      tmp = this->bl_element[i]->freespace() - weight;
      if ( ( tmp >= 0 ) && ( tmp < fsbest ) ) {
	best = i; 
	fsbest = tmp;
      }
    }

    if ( best >= 0 )
      this->bl_element[best]->insert( name, weight );
    else
      this->bl_element.push_back( new Bin( this->bl_capacity, name, weight ) );
  }

  void BinList::insertWF( int name, int weight ) {
    int tmp, i;
    int worst = -1;
    int fsworst = -1;

    for ( i = 0; i < (int)this->bl_element.size(); i++ ) {
      tmp = this->bl_element[i]->freespace() - weight;
      if ( ( tmp >= 0 ) && ( tmp > fsworst ) ) {
	worst = i; 
	fsworst = tmp;
      }
    }

    if ( worst >= 0 )
      this->bl_element[worst]->insert( name, weight );
    else
      this->bl_element.push_back( new Bin( this->bl_capacity, name, weight ) );
  }

  void BinList::insertAWF( int name, int weight ) {
    int tmp, i;
    int aworst = -1, worst = -1;
    int fsaworst = -1, fsworst = -1;

    for ( i = 0; i < (int)this->bl_element.size(); i++ ) {
      tmp = this->bl_element[i]->freespace() - weight;
      if ( ( tmp >= 0 ) && ( tmp > fsaworst ) ) {
	if ( tmp > fsworst ) { 
	  aworst = worst; worst = i; 
	  fsaworst = fsworst; fsworst = tmp;
	} else {
	  aworst = i;
	  fsaworst = tmp;
	}
      }
    }

    if ( aworst >= 0 )
      this->bl_element[aworst]->insert( name, weight );
    else if ( worst >= 0 )
      this->bl_element[worst]->insert( name, weight );
    else
      this->bl_element.push_back( new Bin( this->bl_capacity, name, weight ) );

  }

  void BinList::insertRF( int name, int weight ) {
    int i;
    std::vector<int> list;
    
    srand((unsigned)time(NULL));
    
    for ( i = 0; i < (int)this->bl_element.size(); i++ ) 
      if ( this->bl_element[i]->freespace() >= weight )
	list.push_back(i);
   
    if ( (int)list.size() > 0 )
      this->bl_element[ list [ (int)(list.size()*rand()/(RAND_MAX+1.0)) ] ]->insert( name, weight );
    else 
      this->bl_element.push_back( new Bin( this->bl_capacity, name, weight ) );

  }

  void BinList::reset( int capacity ) {
    for ( int i = 0; i < (int)this->bl_element.size(); i++ )
      delete this->bl_element[i];
    this->bl_element.clear();
    this->bl_capacity = capacity;

  }

} // namespace partitioner
} // namespace wms
} // namespace glite
