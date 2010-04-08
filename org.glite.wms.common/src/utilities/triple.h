/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#ifndef _triple_h_
#define _triple_h_

/*
 * triple.h
 *
 * Copyright (c) EU DataGrid 2002.
 * For license description see http://www.eu-datagrid.org/license.html
 */

// Id:$

namespace glite {
namespace wms {
namespace common { 
namespace utilities {
/**
 * A triple template.
 *
 * @version
 * @date September 16 2002
 * @author Salvatore Monforte
 */
template <class _T1, class _T2, class _T3>
struct triple {
  typedef _T1 first_type;
  typedef _T2 second_type;
  typedef _T3 third_type;
  _T1 first;
  _T2 second;
  _T3 third;

  triple() : first(_T1()), second(_T2()), third(_T3()) {}
  triple(const _T1& __a, const _T2& __b, const _T3& __c) : first(__a), second(__b), third(__c) {}
};

/**
 * Redefines == operator for triples.
 * Returns true if given x triple equals y triple.
 * 
 * @param __x a triple.
 * @param __y a triple.
 * @returns true if given x triple equals y triple, false otherwise.
 */
template <class _T1, class _T2, class _T3>
inline bool operator==(const triple<_T1, _T2, _T3>& __x, const triple<_T1, _T2, _T3>& __y)
{
  return __x.first == __y.first && __x.second == __y.second && __x.third == __y.third; 
}

/**
 * Redefines < operator for triples.
 * Returns true if given x triple is less than y triple.
 * 
 * @param __x a triple.
 * @param __y a triple.
 * @returns true if given x triple is less than y triple, false otherwise.
 */
template <class _T1, class _T2, class _T3>
inline bool operator<(const triple<_T1, _T2, _T3>& __x, const triple<_T1, _T2, _T3>& __y)
{ 
  return __x.first < __y.first || 
    (!(__y.first  < __x.first)  && __x.second < __y.second) ||
    (!(__y.second < __x.second) && __x.third  < __y.third); 
}

/**
 * Makes a triple from given parameters.
 * 
 * @param __x an object to be used as x item.
 * @param __y an object to be used as y item.
 * @param __z an object to be used as z item.
 * @returns a triple made by the three given objects.
 */
template <class _T1, class _T2, class _T3>
inline triple<_T1, _T2, _T3> make_triple(const _T1& __x, const _T2& __y, const _T3& __z)
{
  return triple<_T1, _T2, _T3>(__x, __y, __z);
}

} // namespace utilities
} // namespace common
} // namespace wms
} // namespace glite

#endif 

