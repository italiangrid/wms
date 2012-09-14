// File: jobstate.cpp
// Author: Alessio Gianelle <gianelle@pd.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <cstdlib>
#include <boost/lexical_cast.hpp>

#include "glite/wms/checkpointing/jobstate.h"
#include "glite/wms/checkpointing/stepper.h"
#include "glite/wms/checkpointing/ChkptException.h"

// JobId library
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

// LB library
#include "glite/lb/consumer.h"
#include "glite/lb/producer.h"

using namespace std;
using namespace classad;
//using namespace glite::wmsutils::tls::socket_pp;
using namespace glite::wmsutils::jobid;

namespace glite {
namespace wms {
namespace checkpointing {
  
// Constructors-destructor
// Default one
JobState::JobState( state_type type ) : js_stepper ( NULL ), js_pairs ( NULL )
{  
 
  if ( type ) { // a not empty state is required   
    
    ClassAd       *state_ad;
    ClassAdParser  parser;

    const char   *jobid = this->createContext();
    const string  state = getStateFromLB( jobid );
    if ( state.size() != 0 ) {
      state_ad = parser.ParseClassAd( state );
      initialize( state_ad );
    } // no state found return an empty object
  }
}

// From classAd. 
JobState::JobState( ClassAd *state_ad ) : js_stepper ( NULL ), js_pairs ( NULL ) 
{
  initialize( state_ad );
}

// From string.
JobState::JobState( const string state_str ) : js_stepper ( NULL ), js_pairs ( NULL ) 
{
  ClassAd       *state_ad;
  ClassAdParser  parser;
  state_ad = parser.ParseClassAd( state_str.c_str() );
  initialize( state_ad );
}

const string read_file( istream *ifs )
{
  streamsize       read;
  const streamsize bufsiz = 1024;
  char             buffer[bufsiz];
  string           output;

  if ( !ifs->good() ) throw SEException(__FILE__, __LINE__, "read_file", "Unable to open file");

  while( ifs->good() && !ifs->eof() ) {
    read = ifs->read( buffer, bufsiz ).gcount();
    output.append( buffer, read );
  }
 return output;
}

// From file. 
JobState::JobState( istream *infile ) : js_stepper ( NULL ), js_pairs ( NULL ) 
{
  ClassAd       *state_ad;
  ClassAdParser  parser;
  const string   state_str = read_file( infile );

  state_ad = parser.ParseClassAd( state_str.c_str() );
  initialize( state_ad );
}

JobState::JobState( const JobState &cjs ) : js_stateId( cjs.js_stateId ), js_ctx ( cjs.js_ctx ), 
  js_stepper( NULL ), js_pairs( NULL )
{
  if ( cjs.js_stepper )
    this->js_stepper = new StepsSet( *cjs.js_stepper );
  if ( cjs.js_pairs ) 
    this->js_pairs = static_cast<classad::ClassAd*>(cjs.js_pairs->Copy());
}

// Create and initialize an LB context
const char *JobState::createContext( void )
{
  glite::wmsutils::jobid::JobId   jobid;
  int            error;
  
  // define a context using the values stored in the environment variables. 
  const char *job = getenv( "GLITE_WMS_JOBID" ); // the edg jobID

  if ( !job ) 
    throw SEException(__FILE__, __LINE__, "JobState::createContext", "GLITE_WMS_JOBID");
  try {
    jobid.fromString(job);
  } catch (glite::wmsutils::jobid::WrongIdException) {
    throw SEException(__FILE__, __LINE__, "JobState::createContext", "GLITE_WMS_JOBID");
  }
  const char *sc = getenv( "GLITE_WMS_SEQUENCE_CODE" ); // the sequence code
  if ( !sc ) 
    throw SEException(__FILE__, __LINE__, "JobState::createContext", "GLITE_WMS_SEQUENCE_CODE");
 
  this->js_ctx.reset( new edg_wll_Context );

  // initialize SSL libraries
/* sslutils removed */
//  error = edg_wlc_SSLInitialization();
//  if( error )
//    throw LFException(__FILE__, __LINE__, "JobState::createContext", "edg_wlc_SSLInitialization", error);

  // create the context
  error = edg_wll_InitContext( this->js_ctx.get() );

  if ( error ) 
    throw LFException(__FILE__, __LINE__, "JobState::createContext", "edg_wll_InitContext", error);
  error = edg_wll_SetParam( *this->js_ctx, EDG_WLL_PARAM_SOURCE, EDG_WLL_SOURCE_APPLICATION );
  if ( error ) 
    throw LFException(__FILE__, __LINE__, "JobState::createContext", "edg_wll_SetParam(Source)", error);
  error = edg_wll_SetParam( *this->js_ctx, EDG_WLL_PARAM_QUERY_SERVER, jobid.getServer().c_str());
  if ( error ) 
    throw LFException(__FILE__, __LINE__, "JobState::createContext", "edg_wll_SetParam(QueryServer)", error);
  error = edg_wll_SetLoggingJob( *this->js_ctx, jobid, sc, EDG_WLL_SEQ_NORMAL );
  if ( error )  
    throw LFException(__FILE__, __LINE__, "JobState::createContext", "edg_wll_SetLoggingJob", error);

  return job;
}

// free the object
void JobState::removeall( void )
{
  if ( ( this->js_ctx.unique() ) && ( this->js_ctx.get() ) )
    edg_wll_FreeContext( *this->js_ctx );
  delete this->js_pairs;
  if ( js_stepper ) delete this->js_stepper;
}

JobState::~JobState()
{
  this->removeall();
}

JobState &JobState::operator=( const JobState &that )
{
  if( this != &that ) {
    this->removeall();

    if ( that.js_stepper ) this->js_stepper = new StepsSet( *that.js_stepper );
    else this->js_stepper = NULL;
    if ( that.js_pairs ) this->js_pairs = static_cast<classad::ClassAd*>(that.js_pairs->Copy());
    else this->js_pairs = NULL;
    this->js_stateId.assign( that.js_stateId );
    this->js_ctx = that.js_ctx;
  }
  return( *this );
}

void JobState::setId( const string& stateid)
{
  this->js_stateId = stateid;
  return;
}

// Initialize a JobState object from a ClassAD 
void JobState::initialize( ClassAd *cstate ) 
{
  string      id;
  ExprTree   *tree;
  ClassAd    *pub;

  if ( cstate->EvaluateAttrString("StateId", id) )
    this->setId( id ); 

  pub = (ClassAd *) cstate->Lookup( "UserData" );
  if ( !pub ) 
    throw SEException( __FILE__, __LINE__, "JobState::initialize", "UserData");

  tree = cstate->Lookup( "JobSteps" );
  if ( tree ) { // the iterator is set
    int             cstep, istep;
    string          sstep;
    vector<string>  vect;
    Value           val;
    const ExprList *el;
    cstate->EvaluateExpr(tree, val);

    // setting the iterator js_stepper
    if ( val.IsIntegerValue(istep) ) {
      if ( !cstate->EvaluateAttrInt("CurrentStep", cstep) )
	cstep = 1; // current step is not set => use a default value
      if ( this->js_stepper ) this->js_stepper->initialize(istep, cstep); // a load call
      else this->js_stepper = new StepsSet(istep, cstep); // first call
    } 
    else if ( val.IsStringValue( sstep ) ) {
      vect.push_back( sstep );
      if ( !cstate->EvaluateAttrInt("CurrentStep", cstep) )
	cstep = 0; // current step is not set => use a default one
      
      if ( this->js_stepper ) this->js_stepper->initialize(vect, cstep); // a load call
      else this->js_stepper = new StepsSet(vect, cstep); // first call
    }
    else if ( val.IsListValue(el) ) { // it is a list of labels
	EvalState *es = NULL;
	for ( ExprList::const_iterator elIter = el->begin(); 
	      elIter != el->end(); elIter++ ) {
	  (*elIter)->Evaluate(*es, val);
	  if ( val.IsStringValue( sstep ) ) 
	    vect.push_back( sstep ); 
	  else 
	    throw SEException( __FILE__, __LINE__, "JobState::initialize", "JobSteps");
	}
	if ( !cstate->EvaluateAttrInt("CurrentStep", cstep) )
	  cstep = 0; // current step is not set => use a default one
      
	if ( this->js_stepper ) this->js_stepper->initialize(vect, cstep); // a load call
	else this->js_stepper = new StepsSet(vect, cstep); // first call
    
    } else // the JobStep attribute is wrong typed
      throw SEException( __FILE__, __LINE__, "JobState::initialize", "JobSteps"); 
  }
  
  // All goes well so define the js_pairs
  if ( this->js_pairs ) this->js_pairs->Update( *pub );
  else this->js_pairs = static_cast<classad::ClassAd*>(pub->Copy());
  return;
}

void JobState::isEmpty( int line, const char *func )
{
  if ( this->js_stateId.length() == 0 ) 
    throw ESException(__FILE__, line, func, CHKPT_NoStateId);

  if ( !this->js_pairs ) 
    throw ESException(__FILE__, line, func, CHKPT_EmptyState);
  
  return;
}

// save value methods (set a new pairs <name, value> )
int JobState::saveValue( const string& name, int value ) 
{
  int    res = 0;

  this->isEmpty( __LINE__, "JobState::saveValue(int)" );
  if ( !this->js_pairs->InsertAttr(name, value) )
    res = CHKPT_SyntaxError;
  return res;
}

int JobState::saveValue( const string& name, bool value ) 
{
  int    res = 0;

  this->isEmpty( __LINE__, "JobState::saveValue(bool)" );
  if ( !this->js_pairs->InsertAttr(name, value) )
    res = CHKPT_SyntaxError;
  return res;
}

int JobState::saveValue( const string& name, double value ) 
{
  int    res = 0;
  
  this->isEmpty( __LINE__, "JobState::saveValue(double)" );
  if ( !this->js_pairs->InsertAttr(name, value) )
    res =  CHKPT_SyntaxError;
  return res;
}

int JobState::saveValue( const string& name, const char* value) 
{
  return this->saveValue(name, (std::string)value);
}

int JobState::saveValue( const string& name, const string& value ) 
{
  int    res = 0;
  
  this->isEmpty( __LINE__, "JobState::saveValue(string)" );
  if ( !this->js_pairs->InsertAttr(name, value) )
    res = CHKPT_SyntaxError;
  return res;
}

// append methods
int JobState::addValue( ExprTree* trees, const Value& val, const string& name ) 
{
  Value             valSource;
  const ExprList    *el;
  vector<ExprTree*> vect;
 
  //ExprTree trees has to be copied in order to avoid memory-conflicts
  ExprTree *tree = trees->Copy();
  
  // Converts the Source tree Expression to a Value
  if ( !this->js_pairs->EvaluateExpr( tree, valSource ) ) return CHKPT_SyntaxError;
  
  // Check The Source Value and retrieve/create the Vector
  if ( valSource.IsListValue( el ) ) // It's a list ->retrieve vector components
    el->GetComponents( vect );  
  else   // It's a single value => append it to an empty vector
    vect.push_back( tree );        

  // val is the new value that needs to be inserted in the list.
  // Set the new Value to a Literal (ExpTree son) and appends it.
  Literal   *lit = Literal::MakeLiteral( val );
  if ( lit != NULL ) vect.push_back( (ExprTree*) lit );
  else return CHKPT_SyntaxError;
  
  // Re-Generate the new ExprTreelist from the vector
  ExprList* newEl;
  newEl = ExprList::MakeExprList( vect );
  newEl->SetParentScope( (const ClassAd *) this->js_pairs ) ;
  if ( !this->js_pairs->Insert( name , newEl ) ) return CHKPT_SyntaxError;
    
  return 0;
}

int JobState::appendValue( const string& name, int value ) 
{
  this->isEmpty( __LINE__, "JobState::appendValue(int)" );
 
  ExprTree*  tree = this->js_pairs->Lookup( name );
  if ( tree == NULL ) // The attribute doesn't exist, it should be set
    return saveValue( name, value );
  else if ( getType( tree ) == "Int" ) { // The attribute already exist. 
    // so a list should be made
    Value      val;
    val.SetIntegerValue( value );
    return addValue( tree, val, name );
  } else return CHKPT_WrongType;
}

int JobState::appendValue( const string& name, bool value ) 
{
 this->isEmpty( __LINE__, "JobState::appendValue(bool)" );
 
 ExprTree*  tree = this->js_pairs->Lookup( name );
 if ( tree == NULL ) // The attribute doesn't exist, it should be set
   return saveValue( name, value );
 else if ( getType( tree ) == "Boolean" ) { // The attribute already exist. 
   // so a list should be made
   Value      val;	
   val.SetBooleanValue( value );
   return addValue( tree, val, name );
 } else return CHKPT_WrongType;
}

int JobState::appendValue( const string& name, double value ) 
{
  this->isEmpty( __LINE__, "JobState::appendValue(double)" );
  
  ExprTree*  tree = this->js_pairs->Lookup( name );
  if ( tree == NULL ) // The attribute doesn't exist, it should be set
    return saveValue( name, value );
  else if ( getType( tree ) == "Double" ) { // The attribute already exist. 
    // so a list should be made
    Value      val;
    val.SetRealValue( value );
    return addValue( tree, val, name );
  } else return CHKPT_WrongType;
};

int JobState::appendValue( const string& name, const char* value)
{
  return this->appendValue(name, (std::string)value);
}

int JobState::appendValue( const string& name, const string& value)
{
  this->isEmpty( __LINE__, "JobState::appendValue(string)" );
  
  ExprTree*  tree = this->js_pairs->Lookup( name );
  if ( tree == NULL ) // The attribute doesn't exist, it should be set
    return saveValue( name, value );
  else if ( getType( tree ) == "String" ) { // The attribute already exist. 
    // so a list should be made
    Value      val;
    val.SetStringValue( value );
    return addValue( tree, val, name );
  } else return CHKPT_WrongType;
}

// get value methods (return 1-size vector if the attribute has a single value)
Value JobState::getUnTypedValue(const string& name) 
{
  this->isEmpty( __LINE__, "JobState::getUnTypedValue" );
  
  ExprTree  *tree;
  Value     val;
  
  tree = this->js_pairs->Lookup( name.c_str() ) ;
  if ( tree == NULL )
    throw ULException(__FILE__, __LINE__, 
		      "JobState::getUnTypedValue", name);
  else
      this->js_pairs->EvaluateAttr( name, val );
  return val;
}

vector<int> JobState::getIntValue (const string& name) 
{
  int            i;
  vector<int>    vect;
  const ExprList *el;
  
  //Retrieve the Value type and Check
  Value val = getUnTypedValue( name );
  if (val.IsListValue( el )) {      // It's a List, Iterate over values
    EvalState *es = NULL;
    for ( ExprList::const_iterator elIter = el->begin(); 
	  elIter != el->end(); elIter++ ) {
      (*elIter)->Evaluate(*es, val);
      if ( val.IsIntegerValue( i ) ) 
	vect.push_back( i ); 
      else 
	throw WTException(__FILE__, __LINE__, 
			  "JobState::getStringValue", name, "String");
    }
  } else { // It's a single value => cast it and create a 1-size vector 
    if ( val.IsIntegerValue( i ) )   
      vect.push_back( i );
    else throw WTException(__FILE__, __LINE__, 
			   "JobState::getIntValue", name, "Int");
  }
  return vect;
}

vector<bool> JobState::getBoolValue (const string& name) 
{
  bool           b;
  vector<bool>   vect;
  const ExprList *el;

  //Retrieve the Value type and Check
  Value val = getUnTypedValue( name );
  if ( val.IsListValue( el ) ) {      // It's a List, Iterate over values
    EvalState *es = NULL;
    for ( ExprList::const_iterator elIter = el->begin(); 
	  elIter != el->end(); elIter++ ) {
      (*elIter)->Evaluate(*es, val);
      if ( val.IsBooleanValue( b ) ) 
	vect.push_back( b ); 
      else 
	throw WTException(__FILE__, __LINE__, 
			  "JobState::getStringValue", name, "String");
    }
  } else { // It's a single value => cast it and create a 1-size vector 
    if ( val.IsBooleanValue( b ) )    
      vect.push_back( b );
    else throw WTException(__FILE__, __LINE__, 
			   "JobState::getBoolValue", name, "Bool");
    }
  return vect;
}

vector<double> JobState::getDoubleValue (const string& name) 
{
  double         d;
  vector<double> vect;
  const ExprList *el;

  //Retrieve the Value type and Check
  Value val = getUnTypedValue( name );
  if ( val.IsListValue( el ) ) {      // It's a List, Iterate over values
    EvalState *es = NULL;
    for ( ExprList::const_iterator elIter = el->begin(); 
	  elIter != el->end(); elIter++ ) {
      (*elIter)->Evaluate(*es, val);
      if ( val.IsRealValue( d ) ) 
	vect.push_back( d ); 
      else 
	throw WTException(__FILE__, __LINE__, 
			  "JobState::getStringValue", name, "String");
    }
  } else { // It's a single value => cast it and create a 1-size vector 
    if ( val.IsRealValue( d ) )  
      vect.push_back( d );
    else throw WTException(__FILE__, __LINE__, 
			   "JobState::getDoubleValue", name, "Double");
  }
  return vect;
}

vector<string> JobState::getStringValue (const string& name) 
{
  string         s;
  vector<string> vect;
  const ExprList *el;

  //Retrieve the Value type and Check
  Value val = getUnTypedValue( name );
  if ( val.IsListValue( el ) ) {      // It's a List, Iterate over values
    EvalState *es = NULL;
    for ( ExprList::const_iterator elIter = el->begin(); 
	  elIter != el->end(); elIter++ ) {
      (*elIter)->Evaluate(*es, val);
      if ( val.IsStringValue( s ) ) 
	vect.push_back( s ); 
      else 
	throw WTException(__FILE__, __LINE__, 
			  "JobState::getStringValue", name, "String");
    }
  } else { // It's a string => cast it and create a 1-size vector 
    if ( val.IsStringValue( s ) )  
      vect.push_back( s );
    else throw WTException(__FILE__, __LINE__, "JobState::getStringValue", name, "String");
  }
  return vect;
}

// Check-Type Methods
bool JobState::isIntValue( const string& name ) 
{
  this->isEmpty( __LINE__, "JobState::isIntValue" );
  
  ExprTree *tree = this->js_pairs->Lookup( name );    
  if ( tree != NULL ) {
    return ( getType( tree ) == "Int" );    
  } else throw ULException(__FILE__, __LINE__, "JobState::isIntValue", name);
}

bool JobState::isBoolValue( const string& name ) 
{
  this->isEmpty( __LINE__, "JobState::isBoolValue" );

  ExprTree *tree = this->js_pairs->Lookup( name );
  if ( tree != NULL ) {
    return ( getType( tree ) == "Boolean" );
  } else throw ULException(__FILE__, __LINE__, "JobState::isBoolValue", name);
}

bool JobState::isDoubleValue( const string& name )
{
  this->isEmpty( __LINE__, "JobState::isDoubleValue" );

  ExprTree *tree = this->js_pairs->Lookup( name );
  if ( tree != NULL ) {
    return ( getType( tree ) == "Double" );
  } else throw ULException(__FILE__, __LINE__, "JobState::isDoubleValue", name);
}

bool JobState::isStringValue( const string& name ) 
{
  this->isEmpty( __LINE__, "JobState::isStringValue" );

  ExprTree *tree = this->js_pairs->Lookup( name );
  if ( tree != NULL ) {
    return ( getType( tree ) == "String" );
  } else throw ULException(__FILE__, __LINE__, "JobState::isStringValue", name);
}

bool JobState::isListValue( const string& name ) 
{
  this->isEmpty( __LINE__, "JobState::isListValue" );

  Value val;
  ExprTree *tree = this->js_pairs->Lookup( name );
  
  if ( tree != NULL ) {
    this->js_pairs->EvaluateExpr( tree, val );
    return ( val.GetType() == Value::LIST_VALUE );
  } else throw ULException(__FILE__, __LINE__, "JobState::isListValue", name);
}

// Misc
void JobState::clearPairs( void ) 
{
  this->isEmpty( __LINE__, "JobState::clearPairs" );
  this->js_pairs->Clear(); // Call superclass method
  return;
}

int JobState::checkState( void )
{
 
  if ( this->js_stateId.length() == 0 ) 
    return CHKPT_NoStateId;
  
  if ( !this->js_pairs )
    return CHKPT_EmptyState;

  if ( !this->js_stepper )
    return CHKPT_NoIterator;
  else {
    int curr = this->js_stepper->getCurrentIndex();
    int last = this->js_stepper->getLastIndex();
    if ( ( curr < 0 ) || ( curr > last ) )
      return CHKPT_OutOfSet;
  }

  return 0;
}


ClassAd JobState::toClassAd( void )
{
  ClassAd          total;

  this->isEmpty( __LINE__, "JobState::toClassAd" );

  total.InsertAttr( "StateId", this->js_stateId );
  
  if ( this->js_stepper ) { // the stepper is defined
    int last = this->js_stepper->getLastIndex();
    int curr = this->js_stepper->getCurrentIndex();
    if ( this->js_stepper->isLabel() ) { // convert it in a Expr List
      curr += 1; // remap the current index
      vector<string> ms = js_stepper->getLabelList();
      Value val;
      Literal   *lit;
      vector<ExprTree*> vect;
      for (int i = 0; i <= last; i++) {
	val.SetStringValue( ms[i] );
	lit = Literal::MakeLiteral( val );
	vect.push_back( (ExprTree*) lit );
      }
      ExprList* newEl = ExprList::MakeExprList( vect );
      total.Insert( "JobSteps", newEl );
    } else total.InsertAttr( "JobSteps", last ); 
  
    total.InsertAttr( "CurrentStep", curr );
  }

  total.Insert( "UserData", this->js_pairs->Copy() );
  
  return total;
}

string JobState::toString( void )
{
  string           state_str;
  ClassAd          state_ad = this->toClassAd();
  ClassAdUnParser  unparser;
  
  unparser.Unparse( state_str, state_ad.Copy() );
  
  return state_str;
}

void JobState::toFile( const string& filename )
{
  string   state_str = this->toString();  
  ofstream fout (filename.c_str());

  fout << state_str;
  return;
}

// Save and Load state methods
int JobState::saveState( void )
{
  const char *job;
  string     state_str;

  // create the ctx if it is not set
  if ( !this->js_ctx.get() )
    job = this->createContext();
  else 
    job = getenv( "GLITE_WMS_JOBID" );

  if ( !job ) return CHKPT_SyntaxError;
  
  if ( this->js_stateId.compare( job ) )
    return CHKPT_NotAuth; 

  state_str = this->toString();
  
  // define the TAG value (for LB logging)
  int    is = 0;
  string cs;
  try {
    Step st = this->getCurrentStep();
    if ( st.isLabel() ) cs = st.getLabel();
    else is = st.getInteger();
  } catch (ESException) {
    cs = "Default checkpointing without JobSteps";
  }
  
  int error = 0;
  int rep = 3; // if we have a connection problem try "rep" times
  while ( rep ) {
    if ( cs.size() == 0 ) {
      error = edg_wll_LogEventSync( *this->js_ctx, EDG_WLL_EVENT_CHKPT, 
				    EDG_WLL_FORMAT_CHKPT, boost::lexical_cast<string>(is).c_str(), state_str.c_str() );
    } else
      error = edg_wll_LogEventSync( *this->js_ctx, EDG_WLL_EVENT_CHKPT, 
				    EDG_WLL_FORMAT_CHKPT, cs.c_str(), state_str.c_str() );
    switch ( error ) {
    case EAGAIN:
    case ECONNREFUSED: 
	  case ENOTCONN:
		case EDG_WLL_ERROR_GSS: // connection problem
      rep--;
			sleep( 10 );
      error = CHKPT_ConnProb;
      break;
    case EPERM:
    case EINVAL:
    case EDG_WLL_ERROR_NOJOBID: // internal error
      rep = 0;
      error = CHKPT_SaveFailed; 
      break;
    case ENOSPC:
    case ENOMEM: // LB error
      rep = 0;
      break;
    default:
      rep = 0;
    }
  }
  return error;
}

JobState JobState::loadState( const string& stateid, int num )
{
  if( !this->js_ctx.get() )
    this->createContext();

  string st = getStateFromLB( stateid.c_str(), num );
  
  if ( st.size() != 0 ) { 
    JobState js( st );
    return js;
  } else { // return an empty object
    JobState js;
    return js;
  }
}

// get next step 
Step JobState::getNextStep( void ) 
{
  if ( this->js_stepper ) {
    Step    *res = NULL;
    if ( this->js_stepper->isLabel() ) 
      res = new Step( this->js_stepper->getNextLabel() );
    else 
      res = new Step( this->js_stepper->getNextInt() );
    return *res;
  } else throw ESException(__FILE__, __LINE__, "JobState::getNextStep", CHKPT_NoIterator);
}
// get current step
Step JobState::getCurrentStep( void )  
{
  if ( this->js_stepper ) {
    Step    *res = NULL;
    if ( this->js_stepper->isLabel() ) 
      res = new Step( this->js_stepper->getCurrentLabel() );
    else 
      res = new Step( this->js_stepper->getCurrentInt() );
    return *res;
  } else throw ESException(__FILE__, __LINE__, "JobState::getCurrentStep", CHKPT_NoIterator);
}

const string JobState::getType( ExprTree *tree )
{
  Value             val;
  const ExprList    *el;
  vector<ExprTree*> vect;
  this->js_pairs->EvaluateExpr( tree, val );
  if ( val.IsListValue(el) ) {
    el->GetComponents( vect );
    this->js_pairs->EvaluateExpr( vect[0], val );
  }
  if ( val.GetType() == Value::INTEGER_VALUE )
    return "Int";
  else if ( val.GetType() == Value::BOOLEAN_VALUE )
    return "Boolean";
  else if (val.GetType() == Value::REAL_VALUE  )
    return "Double";
  else if ( val.GetType() == Value::STRING_VALUE )
    return "String";
  else return "Undefined";
}

// Compare two edg_wll_Event using the timestamp member
int cmp_by_timestamp(const void* first, const void* second) {
  timeval f = ((edg_wll_Event *) first)->any.timestamp;
  timeval s = ((edg_wll_Event *) second)->any.timestamp;
  if (( f.tv_sec > s.tv_sec ) ||
      (( f.tv_sec == s.tv_sec ) && ( f.tv_usec > s.tv_usec )))
    return 1;
  if (( f.tv_sec < s.tv_sec ) ||
      (( f.tv_sec == s.tv_sec ) && ( f.tv_usec < s.tv_usec )))
    return -1;
  return 0;
}

// Query LB to retrieve the last State stored by the job with edg_jobid = jobid_str
string JobState::getStateFromLB( const char *jobid_str, int num ) 
{
  int                 i, cnt, error;
  string              state;
  edg_wlc_JobId       jobid;
  edg_wll_Event       *events = NULL;
  edg_wll_QueryRec    jc[2];
  edg_wll_QueryRec    ec[2];
  
  // parse the jobID string
  error = edg_wlc_JobIdParse(jobid_str, &jobid);
  if ( error )  
    throw LFException( __FILE__, __LINE__, 
		       "JobState::getStateFromLB", "edg_wlc_JobIdParse", error);
  
  memset(jc,0,sizeof jc);
  memset(ec,0,sizeof ec);
  
  // job condition: JOBID = jobid
  jc[0].attr = EDG_WLL_QUERY_ATTR_JOBID;
  jc[0].op = EDG_WLL_QUERY_OP_EQUAL;
  jc[0].value.j = jobid;
  
  // event condition: Event type = CHKPT
  ec[0].attr = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  ec[0].op = EDG_WLL_QUERY_OP_EQUAL;
  ec[0].value.i = EDG_WLL_EVENT_CHKPT;

  error = edg_wll_QueryEvents( *this->js_ctx, jc, ec, &events );
  
  if ( error == ENOENT )  // no events founded
    return string();
  
  if ( error )  // query failed
    throw LFException( __FILE__, __LINE__, 
		       "JobState::getStateFromLB", "edg_wll_QueryEvents", error);
  
  for ( cnt=0; events[cnt].type; cnt++ ); // counts the number of events
  
  if ( !cnt ) // no events found
    return string();
 
  // sort the events vector using the timestamp 
  qsort(events, cnt, sizeof(edg_wll_Event), &cmp_by_timestamp); 

  // the last state in the array is the most recent
  if ( num < cnt )
    state.assign( events[cnt-1-num].chkpt.classad ? events[cnt-1-num].chkpt.classad : "" );
  else
    return string(); // Event not found

  for ( i = 0; i < cnt; i++) 
    edg_wll_FreeEvent( events+i );
  free(events);
  
  edg_wlc_JobIdFree( jobid );

  return state;
}  

} // checkpointing
} // wms
} // glite

