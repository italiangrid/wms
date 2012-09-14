// JDL:
#include "glite/jdl/ExpDagAd.h"
#include "glite/jdl/JobAd.h"
#include "glite/jdl/NodeAd.h"
#include "glite/jdl/collectionad.h"
#include "glite/jdl/extractfiles.h"

// CPP UNIT:
#include "cppunit/TestRunner.h"
#include "cppunit/TestResult.h"
#include "cppunit/TestResultCollector.h"
#include "cppunit/TextOutputter.h"
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/extensions/HelperMacros.h"
//BOOST
#include <boost/lexical_cast.hpp>

#define DEF_REQUIREMENTS "other.GlueCEPolicyMaxCPUTime >= 6"
#define DEF_RANK "other.GlueCEStateFreeCPUs"
#define DEF_EXECUTABLE "/bin/date"
#define DEF_VO "egee"
#define DEF_ARGUMENTS "--some-arg"

#define DEF_NODES_NUMBER 3



void setJdlFile(const std::string& jdlFile);
std::string getJdlFile();

// debug mode
void setWCDM(bool value);
std::string sep();
void title(const std::string &title);
void toBCopiedFileList(const std::string &wmp_uri, const std::string &isb_uri,
	const std::vector <std::string> &paths, std::vector <std::pair<std::string, std::string> > to_bcopied);
std::string printExtracted(glite::jdl::ExtractedAd *extractedAd, unsigned int offs=0);
std::string printWarnings(std::vector<std::string>);
// void inherit( Ad &jab)
/********
DAGAD
********/
void dagClientSide(glite::jdl::ExpDagAd &dagad);
void dagServerSide(glite::jdl::ExpDagAd &dagad);
std::vector< std::string> insertJobIds (glite::jdl::ExpDagAd &dagad);


/********
JOBAd
********/
glite::jdl::Ad createAd();
glite::jdl::JobAd createJobAd();
glite::jdl::ExpDagAd createDagAd();
glite::jdl::CollectionAd createCollectionAd();

void jobClientSide(glite::jdl::JobAd &dagad);
void jobServerSide(glite::jdl::JobAd &dagad);





// endline

