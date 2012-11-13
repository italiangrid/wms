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
  
  DispatcherFromFileList main
  
  Authors: Paolo Andreetto <paolo.andreetto@pd.infn.it>
  
  Version info: $Id$
  Release: $Name$
  
 */

#include <iostream>
#include <fstream>
#include "DispatcherFromFileList_cu_suite.h"
#include "config.h"

using namespace CppUnit;

#define REPORT_FILE REPORT_DIR "/DispatcherFromFileList_cu.xml"

int main (int argc , char** argv)
{
  std::ofstream xml(REPORT_FILE,ios::app);
  if(!xml) {
    std::cerr << "Cannot open " << REPORT_FILE << endl;
    return 1;
  }

  CppUnit::TestResult controller;
  CppUnit::TestResultCollector result;
  controller.addListener( &result );
  TestRunner runner;
  runner.addTest( DispatcherFromFileList_test::suite());
  runner.run(controller);
  CppUnit::XmlOutputter outputter( &result, xml );
  CppUnit::TextOutputter outputter2( &result, std::cerr );
  outputter.write();
  outputter2.write();
	
  return result.wasSuccessful() ? 0 : 1 ;
}


