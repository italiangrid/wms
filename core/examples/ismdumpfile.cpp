/* Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
#include <iostream>
#include <fstream>
#include <string>

#include <boost/shared_ptr.hpp>

#include <classad_distribution.h>

/*#include "glite/wms/jdl/JobAdManipulation.h"
##include "glite/wms/jdl/PrivateAdManipulation.h"
#include "glite/wms/jdl/ManipulationExceptions.h"
#include "glite/wms/jdl/JDLAttributes.h"
#include "glite/wms/jdl/PrivateAttributes.h"

namespace jdl = glite::wms::jdl;
*/
int
main(int argc, char* argv[])
{
  std::ifstream inf;
  std::string   outputfile;

  // we read the ClassAds file.
  if (argc == 3) {
    inf.open(argv[1]);
    outputfile = argv[2];
  } else {
    std::cout << "Usage: "
         << argv[0]
         << " CLASSAD_CE_INFO_FILE"
	 << " CLASSAD_OUTPUT_FILE"
         << std::endl;
    return 1;
  }

  std::string input_ad;
  while (!inf.eof()) {
    std::string line;
    inf >> line;
    input_ad += line;
  }

  inf.close(); 

  // we define a ClassAds.
  classad::ClassAdParser parser;
  classad::ClassAd* ad = parser.ParseClassAd(input_ad.c_str());

  std::cout << "start parser: " << input_ad << std::endl;

  if (ad == 0)
  {
    std::cout << "Bad input classad file" << std::endl;
    return 1;
  }

  std::ofstream outf(outputfile.c_str());

  for (int i = 0; i<=3; i++) {
    classad::ClassAd ad1;

    ad1.InsertAttr("id", "ranatan.cnaf.infn.it:2190");

    ad1.InsertAttr("update_time", i);

    ad1.InsertAttr("expiry_time", 120+i);

    ad1.Insert("info", ad->Copy());

    outf << ad1;
  }

  delete ad;
}

  


