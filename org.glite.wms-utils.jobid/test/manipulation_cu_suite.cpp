#include "manipulation_cu_suite.h"


using namespace CppUnit;
using namespace std;
using namespace glite::wmsutils::jobid;


void Manipulation_test::setUp()
{}

void Manipulation_test::tearDown()
{}

void Manipulation_test::to_fromfile_case()
{
	JobId element;
	
	string lbserver="grid012g.cnaf.infn.it";
	int port=6000;
	string unique ="qaKyEoV3G144rmoyXeW6QA";
	element.setJobId(lbserver, port, unique);
	
	string filename=to_filename(element);
	
	JobId newelement = from_filename(filename);
	
	string reduced = get_reduced_part(element, 7);

	string newreduced = get_reduced_part(newelement, 7);
	
	CPPUNIT_ASSERT(reduced==newreduced);
}

