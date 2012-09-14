/*
 * glite-wms-jdl-common.cpp
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 */




#include "glite/wmsutils/exception/Exception.h"
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/jdl/JobAd.h"
#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/ExpDagAd.h"
#include "glite/jdl/Ad.h"
#include "glite/jdl/jdl_attributes.h"
#include "glite-wms-client-common.h"

// DagReplacing Example:
#include "glite/jdl/DAGAd.h"
#include "glite/wmsutils/classads/classad_utils.h"
using namespace classad;
using namespace glite::wmsutils::classads ;
using namespace std ;
using namespace glite::jdl ;

// Static Variables:
std::string jdlFile="";
bool WCDM = true ;
const string RANDOM= "aHHG678GkF50gJd590HFkjF5kf9FgIQWEIUY6798ee97KYiIIU99OIKIyIiTWQIUYR8iIErQEI7KiuYQW";

// GENERAL Methods
void setWCDM(bool value){WCDM=value;}

string sep(){ string sep="*********************************************" ; return  sep + "\n" +sep +"\n" ; }

void setJdlFile(const std::string& jf){jdlFile=jf;};

std::string getJdlFile(){return jdlFile;}

void title(const string &title){ if (WCDM) cout << sep()<< title << endl << sep(); }

void toBCopiedFileList(const std::string &wmp_uri, const std::string &isb_uri,
	const std::vector <std::string> &paths, std::vector <std::pair<std::string, std::string> > to_bcopied){
	toBcopied("InputSandbox", paths, to_bcopied, wmp_uri, isb_uri);
	for (unsigned int i = 0 ; i< to_bcopied.size() ; i++){
		if (WCDM) cout << "\t-SRC:  " << to_bcopied[i].first  <<"\n\t-DEST: " << to_bcopied[i].second << "\n\t   - - -"<< endl ;
	}
}



const string endline = "\n";
// EXTERNAL method
string printExtracted(ExtractedAd *extractedAd, unsigned int offs){
	assert (extractedAd!=NULL);
	if (!extractedAd->hasFiles()){
		return "";
	}
	string offset ="*   ";
	string msg ="";
	for (unsigned int i = 0;i<=offs;i++){ offset +="*   "; }
	if (extractedAd->getNodeName()!=""){
		msg += offset +"NodeName = " + extractedAd->getNodeName() + endline ;
	}
	if (extractedAd->getJobId()!=""){
		msg += offset +"JobId    = " + extractedAd->getJobId() + endline ;
	}
	if (extractedAd->hasFiles()){
		msg += offset + "Files:" + endline ;
	}
	std::vector<FileAd> files = extractedAd->getFiles();
	for (unsigned int i = 0;i<files.size();i++){
		msg += offset + " - "+files[i].file ;
		msg +=" ( " +boost::lexical_cast<string>(files[i].size) +" K)" +endline;
	}
	std::vector<ExtractedAd*> children = extractedAd->getChildren();
	if (children.size()){
		msg += offset +"Children = " + endline ;
		for (unsigned int i = 0;i<children.size();i++){
			msg+= printExtracted(children[i],offs+1);
		}
	}
	if(extractedAd->getTotalSize()){
		msg += offset +"Tot. Size = " +
		boost::lexical_cast<string>(extractedAd->getTotalSize()) + endline ;
	}
	return msg;
}


std::string printWarnings(std::vector<std::string> warnings){
	string msg ="";
	if( warnings.size() ){
		msg+="Warning(s) found: ";
		for (unsigned int i=0;i<warnings.size();i++){
			msg+="\n" +warnings[i];
		}
	}
	return msg;
}
void printWarnings(Ad *ad){
	if (ad->hasWarnings() ){
		title(printWarnings(ad->getWarnings()));
	}
}

/********
DAGAD client Side
********/
void dagClientSide(ExpDagAd &dagad){
	std::vector <std::string> jdls ;
	std::vector <std::pair<std::string, std::string> > to_bcopied ;
	title ("dagClientSide ON CLIENT SIDE.........." );
	dagad.setLocalAccess(true);
	if (WCDM) cout << dagad.toString()<< endl ;
	title("get Submission strings...");
	jdls = dagad.getSubmissionStrings ();
	for (unsigned int i = 0 ; i<jdls.size() ; i++){
		if (WCDM) cout << "JDL " << i << endl << jdls[i] << endl ;
	}
	title("Done\nSubmission String now:");
	if (WCDM) cout << dagad.toString(ExpDagAd::MULTI_LINES)  << endl ;

	if ( dagad.gettoBretrieved()){
		title ("Extracted files...\n" +printExtracted(dagad.getExtractedAd()));
	}else {
		title("There are NO files to be retrieved");
	}
	if (dagad.hasWarnings() ){
		title(printWarnings(dagad.getWarnings()));
	}
}

string int2RndString(int i, int j=0){
	const int random_i =( (i +57 +j)*7 + (i+39+j)/11 + ((17+j)*(i+4)) )%(RANDOM.size() -5);
	return RANDOM.substr(random_i,4) ;
}


vector<string> insertJobIds (glite::jdl::ExpDagAd &dagad){
		title("\nInserting jobids into dagad...");
		std::vector <std::string>  jobids ;
		dagad.setAttribute (ExpDagAd::EDG_JOBID,"https://gundam.cnaf.infn.it:9000/DAGroootJobId");
		const string JOBID ="https://insertingJobidServer:9000/AbCd-";
		string jobid ="";
		for (unsigned int i = 0 ; i< dagad.size (); i++){
			jobid = JOBID + int2RndString(i, dagad.size());
			jobids.push_back(jobid);
			if (WCDM) cout << i <<") " << jobid<< endl;
		}
		dagad.setJobIds(jobids);
		return jobids;
}
/********
DAGAD WMPROXY Side Emulation
********/
void dagServerSide(ExpDagAd &dagad){
	dagad.getSubmissionStrings();
	title("dagServerSide ON SERVER SIDE:.. " + dagad.toString());
	Ad ad (dagad.toString());
	if (!ad.hasAttribute("WMPInputSandboxBaseURI")) {
		ad.setAttribute("WMPInputSandboxBaseURI", "wmp://InputBaseURI:1234/AutomaticSet");
	}
	if (!ad.hasAttribute("edg_jobid")) {
		ad.setAttribute("edg_jobid", "https://insertingDAGAD.it:9000/gadDagGad");
	}
	ExpDagAd dagadServer (ad.toString());
	dagadServer.setLocalAccess(false);
	// Insert JobIds:
	vector<string> jobids  = insertJobIds(dagadServer);
	// ITERATE over its children
	vector<string>::iterator iter = jobids.begin();
	vector<string>::iterator const end = jobids.end();
	string jobidstring, seed, osbURI;
	int strSize=0;
	int rnd =1;
	// Emulating SERVER SIDE:
	for (; iter != end; ++iter, rnd++) {
		jobidstring = *iter;
		glite::wmsutils::jobid::JobId subjobid(jobidstring);
		strSize=iter->size();
		seed = subjobid.getUnique();
		NodeAd nodead = dagadServer.getNode(subjobid);
		// Adding ISB/OSB attributes (when needed)
		if (nodead.hasAttribute(JDL::INPUTSB)){
			nodead.setAttribute("InputSandboxPath", "wmpFtp:///node/IsbPathCmn/"+seed);
		}
		if (nodead.hasAttribute(JDL::OUTPUTSB)) {
			nodead.setAttribute("OutputSandboxPath", "wmpFtp:///node/OsbPathCmn/"+seed);
			osbURI="wmpFtp:///node/OsbDestURI/"+seed;
			if (nodead.hasAttribute(JDL::OSB_BASE_DEST_URI)){
				osbURI=nodead.getString(JDL::OSB_BASE_DEST_URI);
			}
			vector<string> osbs= nodead.getStringValue(JDL::OUTPUTSB);
			vector<string>::iterator iter = osbs.begin();
			vector<string>::iterator const end = osbs.end();
			for (; iter != end; ++iter) {
				nodead.addAttribute(JDL::OSB_DEST_URI,osbURI+"/"+*iter);
			}
		}
		// Other attributes  (Mandatory)
		nodead.setAttribute("UserProxy", "DN:UprxCmn/User=MaraskCmn");
		nodead.setAttribute(JDL::WMPISB_BASE_URI, "wmpiftp://nodedestURICmn:1234/"+ seed);
		dagadServer.replaceNode(subjobid, nodead);
	}
	title("DagAd after SERVER adding attributes: "  + dagadServer.toString());
	if (WCDM) cout <<"get Submission strings..."<< endl ;
	std::vector <std::string> jdls = dagadServer.getSubmissionStrings ();
	title("Done\nTo String now: " + dagadServer.toString(ExpDagAd::MULTI_LINES));
	if (dagadServer.hasWarnings() ){
		title(printWarnings(dagadServer.getWarnings()));
	}
}


/************************************
* Ad / JobAd / Exp / CollectionAd Creation
*************************************/

glite::jdl::Ad intJad ;


void setAdValues ( glite::jdl::Ad *jad ,
			std::string requirements = DEF_REQUIREMENTS ,
			std::string rank = DEF_RANK ,
			std::string executable = DEF_EXECUTABLE ,
			std::string vo = DEF_VO ) {
		jad->setAttributeExpr( glite::jdl::JDL::REQUIREMENTS, requirements);
		jad->setAttributeExpr( glite::jdl::JDL::RANK, rank);
		jad->setAttribute( glite::jdl::JDL::EXECUTABLE, executable);
		jad->setAttribute( glite::jdl::JDL::VIRTUAL_ORGANISATION, vo);
}

glite::jdl::Ad createAd(){
	glite::jdl::Ad jad;
	if (jdlFile==""){
		title("Creating Default Ad....");
		setAdValues( &jad );
	}else{
		title("Creating Ad From File: " +jdlFile);
		jad.fromFile(jdlFile);
	}
	return jad;
}
glite::jdl::JobAd createJobAd(){
	JobAd jobad (*(createAd().ad()));
	if (jobad.hasAttribute("defaultRank")){jobad.setDefaultRank (jobad.lookUp("defaultRank"));}
	if (jobad.hasAttribute("defaultRequirements")){jobad.setDefaultReq (jobad.lookUp("defaultRequirements"));}
	return jobad;
}

glite::jdl::CollectionAd createCollectionAd(){
	CollectionAd collect;
	if (jdlFile==""){
		collect.setAttribute(JDL::TYPE,JDL_TYPE_COLLECTION);
		collect.setAttribute( glite::jdl::JDL::VIRTUAL_ORGANISATION, DEF_VO);
		Ad templateAd= createAd();
		for (unsigned int i=0;i<DEF_NODES_NUMBER;i++){
			collect.addNode(templateAd);
		}
	}else{
		title("Creating CollectionAd From File: " +jdlFile);
		collect.fromFile(jdlFile);
	}

	if (collect.hasAttribute("defaultRank")){collect.setDefaultRank (collect.lookUp("defaultRank"));}
	if (collect.hasAttribute("defaultRequirements")){
		collect.setDefaultReq (collect.lookUp("defaultRequirements"));
	}
	return collect;
}


glite::jdl::ExpDagAd createDagAd(){
	if (jdlFile==""){
		title("Creating Default Dagad....");
		string jdl=
		string("[ nodes = [  dependencies = {  };  Node_0 = [  description =[ Executable = \"/usr/bin/ls\";  ]; node_type = 	\"edg-jdl\" ];")+
		string("Node_1 = [ description =[ Executable = \"/usr/bin/ls\";  ]; node_type = \"edg-jdl\"];];")+
		string("VirtualOrganisation = \"EGEE\"; Type = \"dag\";  requirements = other.something ; rank = 3; ]");
		return ExpDagAd (jdl);
	}else{
		title("Creating Dagad From File: " +jdlFile);
		ifstream is( jdlFile.c_str());
		ExpDagAd dagad(is);
		string msg="";
		if (dagad.hasAttribute("defaultRank")){
			msg+="inserting def rank...\n";
			dagad.setDefaultRank ("defaultDagLevelRank");
			msg+= "INSERTED:" + dagad.getDefaultRank () +"\n";
		}
		if (dagad.hasAttribute("defaultRequirements")){
			msg+="inserting def requirements\n";
			dagad.setDefaultReq ("defaultDagLevelReq==true");
			msg+= "INSERTED:" + dagad.getDefaultReq () +"\n";
		}
		if (msg!=""){ title (msg);}
		return dagad;
	}
	return ExpDagAd("ERROR");
}

/********
JOBAd
********/
void manipulateServerSide(glite::jdl::Ad &jobadServer){
	if (WCDM) cout << jobadServer.toLines() << endl;
	// insert Server mandatory attributes...
	if (!jobadServer.hasAttribute(JDL::JOBID)){
		jobadServer.setAttribute(JDL::JOBID,"https://gundam.cnaf.infn.it:9000/zx5ND75DsRQjgLnodeA");
	}
	if(!jobadServer.hasAttribute(JDL::WMPISB_BASE_URI)){
		jobadServer.setAttribute(JDL::WMPISB_BASE_URI,"https://gundam.cnaf.infn.it:9000/zx5ND75DsRQjgLnodeA");
	}
}

void jobClientSide(glite::jdl::JobAd &jab){
	title("jobClientSide  ON CLIENT SIDE......................");
	jab.setLocalAccess(true);
	jab.setDefaultReq ("other.GlueCEStateStatus == \"Production\"") ;
	jab.setDefaultRank ("-other.GlueCEStateEstimatedResponseTime");
	if (WCDM) cout << jab.toLines() << endl;
	title("Submission...");
	if (WCDM) cout << jab.toSubmissionString() << endl;
	title ("Extracted files...\n" +printExtracted(jab.getExtractedAd()));
	printWarnings(&jab);
}
void jobServerSide(glite::jdl::JobAd &jab){
	title("jobServerSide ON SEVER SIDE......................");
	JobAd jobadServer(jab.toString());
	jobadServer.setLocalAccess(false);
	manipulateServerSide(jobadServer);
	title("jobServerSide Submission...");
	if (WCDM) cout << jobadServer.toSubmissionString() <<endl;
	printWarnings(&jab);	
}
