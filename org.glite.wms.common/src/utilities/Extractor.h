#ifndef __EXTRACTOR_H_LOADED
#define __EXTRACTOR_H_LOADED

#include <list>
#include <algorithm>

namespace glite {
namespace wms {
namespace common {
namespace utilities {

template <class Container>
class ForwardExtractor {
public:
  typedef typename Container::iterator        iterator;
  typedef typename Container::value_type      value_type;

  ForwardExtractor( void );
  ForwardExtractor( Container &cont );

  ~ForwardExtractor( void );

  iterator get_next( void );
  void erase( const iterator &it );
  void remove( const value_type &val );

  inline void clear( void ) { this->fe_container->clear(); this->fe_list.clear(); }
  inline void reset( Container &cont ) { this->fe_container = &cont; this->fe_list.clear(); }
  inline Container *operator->( void ) { return( this->fe_container ); }

private:
  typedef typename std::list<iterator>::iterator   ExtraIterator;

  Container               *fe_container;
  std::list<iterator>      fe_list;
};

template <class Container>
ForwardExtractor<Container>::ForwardExtractor<Container>( Container &cont ) : fe_container( &cont ), fe_list()
{}

template <class Container>
ForwardExtractor<Container>::ForwardExtractor<Container>( void ) : fe_container(), fe_list()
{}

template <class Container>
ForwardExtractor<Container>::~ForwardExtractor<Container>( void )
{}

template <class Container>
typename ForwardExtractor<Container>::iterator ForwardExtractor<Container>::get_next( void )
{
  iterator   next;

  if( this->fe_container->empty() ) next = this->fe_container->end();
  else if( this->fe_list.empty() ) {
    next = this->fe_container->begin();
    this->fe_list.push_back( next );
  }
  else {
    next = this->fe_list.back();
    ++next;

    if( next != this->fe_container->end() ) this->fe_list.push_back( next );
  }

  return( next );
}

template <class Container>
void ForwardExtractor<Container>::erase( const iterator &pos )
{
  ExtraIterator    where;

  where = std::find( this->fe_list.begin(), this->fe_list.end(), pos );

  if( where != this->fe_list.end() ) {
    this->fe_list.erase( where );
    this->fe_container->erase( pos );
  }

  return;
}

template <class Container>
void ForwardExtractor<Container>::remove( const value_type &val )
{
  ExtraIterator    where;

  where = std::find( this->fe_container->begin(), this->fe_container->end(), val );
  if( where != this->fe_container->end() ) this->erase( where );

  return;
}

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* __EXTRACTOR_H_LOADED */

// Local Variables:
// mode: c++
// End:
