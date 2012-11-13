/*
  Copyright (c) 2004 on behalf of the EU EGEE Project:
  The European Organization for Nuclear Research (CERN),
  Istituto Nazionale di Fisica Nucleare (INFN), Italy
  Datamat Spa, Italy
  Centre National de la Recherche Scientifique (CNRS), France
  CS Systeme d'Information (CSSI), France
  Royal Institute of Technology, Center for Parallel Computers (KTH-PDC), Sweden
  Universiteit van Amsterdam (UvA), Netherlands
  University of Helsinki (UH.HIP), Finland
  University of Bergen (UiB), Norway
  Council for the Central Laboratory of the Research Councils (CCLRC), United Kingdom
  
  DispatcherFromFileList suite
  
  Authors: Paolo Andreetto <paolo.andreetto@pd.infn.it>
  
  Version info: $Id$
  Release: $Name$
  
 */

#include "DispatcherFromFileList_cu_suite.h"

using namespace CppUnit;

/* 
    **********************************************************

      Mock functions and objects

    ********************************************************** 
*/

namespace glite {
namespace wms {
namespace manager {
namespace server {

int signal_counter=0;

void signal_handling_init(){
  signal_counter=2;
}

bool received_quit_signal(){
  signal_counter--;
  return !signal_counter;
}

} // server
} // manager
} // wms
} // glite



/* 
    **********************************************************

      Unit suite methods

    ********************************************************** 
*/

void
DispatcherFromFileList_test::setUp()
{
  std::string file("dispatch.list");

  signal_handling_init();

  boost::shared_ptr<DispatcherFromFileList::extractor_type> extractor(new DispatcherFromFileList::extractor_type(file));
  if (!extractor || !(dispatcher = new DispatcherFromFileList(extractor))) {
    cerr << "cannot build FileList extractor" << endl;
    exit(1);
  }

}

void
DispatcherFromFileList_test::tearDown()
{
  if(dispatcher) delete dispatcher;
}

void
DispatcherFromFileList_test::test_simple_run()
{
  glite::wms::common::task::PipeWriteEnd<boost::shared_ptr<Request> > pipe;
  dispatcher->run(pipe);

}


