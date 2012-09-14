/***************************************************************************
    filename  : glite_wmsui_LbWrapper.i
    begin     : Wen Mar 12
    author    : Fabrizio Pacini
    email     : fpacini@datamat.it
    copyright : (C) 2001 by Datamat
 ***************************************************************************/
//
// $Id$
//

%module glite_wmsui_LbWrapper

%{
#include "glite/lb/JobStatus.h"
%}
%include "std_string.i"
%include "std_vector.i"
%include "std_list.i"
%include "typemaps.i"


%typemap(python,argout) std::string& outString {
  PyObject *o;
  o = PyList_New(2);
  PyList_SetItem(o,0,$result);
  PyList_SetItem(o,1,Py_BuildValue("s#",$1->c_str(),$1->size()));
  delete $1;
  $result = o;
}

%typemap(in,numinputs=0) std::string& outString {
 $1 = new std::string;
}


namespace std {
   %template(StringVector) vector<string>;
   %template(IntVector) vector<int>;   
}

namespace std {
   %template(StringStatus) vector<glite::lb::JobStatus> ;
}


%inline %{

using namespace std;
typedef std::string String;

#include "LbWrapper.h"

%}

%include "LbWrapper.h"
