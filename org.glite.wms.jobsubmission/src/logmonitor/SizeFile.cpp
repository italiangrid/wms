#include <unistd.h>

#include <cmath>

#include <iostream>
#include <iomanip>
#include <string>

#include <boost/filesystem/path.hpp>

#include "glite/wms/common/utilities/streamdescriptor.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "../jobcontrol_namespace.h"

#include "SizeFile.h"

USING_COMMON_NAMESPACE;
using namespace std;
namespace fs = boost::filesystem;

RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor {

namespace {

int integer_size( size_t size, int basen = 10 )
{
  int         bitnumber = (int) (log((double)(UCHAR_MAX + 1)) / log(2.0)), answer;
  double      partial = size / sizeof(unsigned char), total = (bitnumber * partial), largest, rapporto;

  largest = pow( 2, total ) - 1;
  rapporto = log( largest ) / log( (double) basen );
  answer = (int) rapporto;

  if( ((double) answer) != rapporto ) answer += 1;

  return answer;
}

}; // Anonymous namespace

size_t  SizeField::sf_s_long = integer_size( sizeof(long int) ), SizeField::sf_s_unsigned = integer_size( sizeof(unsigned int) );
const string SizeFile::sf_s_defaultHeader( "Condor log size file, DAG enabled.\n"
					   "This header is made to contain the dagId.\n"
					   "In this moment the DAG Id is unknown." );

ostream &operator<<( ostream &os, const SizeField &sf )
{
  os << setfill('0') << setw(SizeField::sf_s_long) << sf.sf_position << ' '
     << setfill('0') << setw(SizeField::sf_s_unsigned) << sf.sf_pending << ' '
     << sf.sf_last << " |";

  return os;
}

istream &operator>>( istream &is, SizeField &sf )
{
  bool            good, last;
  char            c;
  unsigned int    pending;
  long int        position;

  is >> position >> pending >> last >> c;
  good = is.good() && !is.bad() && (c == '|');

  if( good ) {
    sf.sf_position = position;
    sf.sf_pending = pending;
    sf.sf_last = last;
  }

  sf.sf_good = good;

  return is;
}

SizeField::SizeField( void ) : sf_good( false ), sf_last( false ), sf_pending( 0 ), sf_position( 0 )
{}

SizeField::SizeField( long int position, unsigned int pending, bool last ) : sf_good( true ), sf_last( last ),
									     sf_pending( pending ), sf_position( position )
{}

SizeField::~SizeField( void ) {}

SizeField &SizeField::reset( long int position, unsigned int pending, bool last )
{
  this->sf_position = position;
  this->sf_pending = pending;
  this->sf_last = last;
  this->sf_good = true;

  return *this;
}

ostream &operator<<( ostream &os, const SizeHeader &sh )
{
  os << sh.sh_header << "\n...";

  return os;
}

istream &operator>>( istream &is, SizeHeader &sh )
{
  bool            last;
  string          buffer, header;

  do {
    getline( is, buffer );

    last = (buffer == "...");
    if( is.good() && !last ) {
      header.append( buffer );
      header.append( 1, '\n' );
    }
    else if( last )
      header.erase( header.end() - 1 );

  } while( is.good() && !last );

  if( (sh.sh_good = last) )
    sh.sh_header.assign( header );
  else
    sh.sh_header.clear();

  return is;
}

SizeHeader::SizeHeader( const char *header ) : sh_good( header != NULL ), sh_header( header ? header : "" )
{}

SizeHeader::SizeHeader( const string &header ) : sh_good( header.size() != 0 ), sh_header( header )
{}

SizeHeader::~SizeHeader( void ) {}

SizeHeader &SizeHeader::reset( const string &header )
{
  this->sh_good = (header.size() != 0);
  this->sh_header.assign( header );

  return *this;
}

void SizeFile::createDotFile( void )
{
  fs::path      condorfile( this->sf_filename, fs::native );
  fs::path      dotfile( condorfile.branch_path() );
  string                       name( condorfile.native_file_string() );

  if( !condorfile.empty() ) {
    name.insert( name.begin(), '.' );
    name.append( ".size" );

    dotfile /= name;

    this->sf_filename.assign( dotfile.native_file_string() );
  }
  else this->sf_filename.clear();

  return;
}

void SizeFile::newSizeFile( void )
{
  this->sf_stream.clear(); this->sf_stream.close();
  this->sf_stream.open( this->sf_filename.c_str(), ios::out );

  if( !this->sf_header.good() )
    this->sf_header.reset( sf_s_defaultHeader );

  this->sf_stream << this->sf_header << endl << this->sf_current << endl;

  this->sf_stream.close();
  this->sf_stream.open( this->sf_filename.c_str(), ios::out | ios::in );
  this->sf_stream.seekp( 0, ios::end );

  return;
}

SizeField SizeFile::readField( streamoff position )
{
  SizeField     field;

  this->sf_stream.seekg( position );
  this->sf_stream >> field;

  return field;
}

SizeField SizeFile::readLastField( void )
{
  streamoff       position = (SizeField::dimension() + 1);
  SizeField       field;

  this->sf_stream.seekg( -position, ios::end );
  this->sf_stream >> field;

  return field;
}

bool SizeFile::checkOldFormat( void )
{
  bool               last, good = false;
  long int           position;
  unsigned int       pending;
  string             dagid, buffer;
  SizeField          field;

  this->sf_stream.clear(); this->sf_stream.seekg( 0 );
  this->sf_stream >> position >> pending >> last;

  if( this->sf_stream.good() ) this->sf_stream >> dagid;

  if( this->sf_stream.good() || this->sf_stream.eof() ) {
    this->sf_stream.clear();
    field.reset( position, pending, last );

    if( (good = field.good()) && (dagid.length() != 0) ) {
      buffer.assign( "Restored from old file\nDagId = " );
      buffer.append( dagid );
      buffer.append( "\n###########" );

      this->sf_header.reset( buffer );
    }

    if( good ) this->sf_current = field;
  }

  return good;
}

void SizeFile::openFile( bool create )
{
  int                   field_number;
  streamoff             position, filesize, field_storage_size;
  logger::StatePusher   pusher( elog::cedglog, "SizeFile::openFile()" );

  if( this->sf_filename.size() != 0 ) {
    utilities::create_file( this->sf_filename.c_str() );

    if( create ) {
      this->sf_header.reset( sf_s_defaultHeader );
      this->sf_current.reset( 0, 0, 0 );

      this->newSizeFile();

      if( !this->sf_stream.good() || this->sf_stream.bad() ) {
	elog::cedglog << logger::setlevel( logger::severe ) << "Cannot open size file \"" << this->sf_filename << "\"." << endl;

	this->sf_good = false;
      }
    }
    else {
      this->sf_stream.open( this->sf_filename.c_str(), ios::in | ios::out );

      if( this->sf_stream.good() ) {
	this->sf_stream.seekg( 0, ios::end );
	filesize = this->sf_stream.tellg();

	if( filesize != 0 ) { // File contains something
	  elog::cedglog << logger::setlevel( logger::info ) << "Size file is not empty. Checking header." << endl;

	  this->sf_stream.seekg( 0 ); // Rewind the file !!!
	  this->sf_stream >> this->sf_header;

	  if( this->sf_header.good() && !this->sf_stream.eof() ) { // Good header
	    elog::cedglog << logger::setlevel( logger::debug ) << "The header of the size file seems to be good..." << endl
			  << "Trying to read the last status..." << endl;

	    this->sf_current = this->readLastField();
	    if( this->sf_current.good() && this->sf_stream.good() ) { // Last field successfully read
	      elog::cedglog << logger::setlevel( logger::info ) << "Last field successfully read, winding size file." << endl
			    << logger::setlevel( logger::debug ) << "Position = " << this->sf_current.position() << endl
			    << "Pending jobs = " << this->sf_current.pending() << endl
			    << "Last job submitted = " << this->sf_current.last() << endl;

	      this->sf_stream.seekp( 0, ios::end );
	    }
	    else { // Try to read the last but one field
	      if( !this->sf_stream.good() || this->sf_stream.bad() ) this->sf_stream.clear();

	      field_storage_size = filesize - this->sf_header.size();
	      field_number = field_storage_size / (SizeField::dimension() + 1);

	      if( field_number != 0 ) { // There seems to be enough space to store some fields
		while( field_number > 0 ) { // Try to find the "first" good field
		  this->sf_current = this->readField( this->sf_header.size() + (field_number * (SizeField::dimension() + 1)) );

		  if( this->sf_current.good() && this->sf_stream.good() ) {
		    elog::cedglog << logger::setlevel( logger::info ) << "Badly closed file successfully recovered, winding it." << endl
				  << logger::setlevel( logger::debug ) << "Position = " << this->sf_current.position() << endl
				  << "Pending jobs = " << this->sf_current.pending() << endl
				  << "Last job submitted = " << this->sf_current.last() << endl;

		    position = this->sf_stream.tellg();
		    this->sf_stream.seekp( position ); // Put the write pointer at the last good known field.

		    break;
		  }
		  else if( !this->sf_stream.good() || this->sf_stream.bad() )
		    this->sf_stream.clear();

		  field_number -= 1;
		}

		if( field_number == 0 ) {
		  elog::cedglog << logger::setlevel( logger::error ) << "Cannot find any good size field inside the file." << endl
				<< logger::setlevel( logger::warning ) << "(Re)Starting with a zero sized file." << endl;

		  this->sf_current.reset( 0, 0, 0 );
		  this->newSizeFile();
		}
	      }
	      else { // File seems to be too short
		elog::cedglog << logger::setlevel( logger::error ) << "Size file is too short (" 
			      << filesize << "/" << (this->sf_header.size() + SizeField::dimension() + 1) << ")" << endl
			      << logger::setlevel( logger::warning ) << "(Re)Starting with a zero sized file." << endl;

		this->sf_current.reset( 0, 0, 0 );
		this->newSizeFile();
	      }
	    }
	  }
	  else if( this->sf_stream.eof() ) { // Header corrupted or old file...
	    elog::cedglog << logger::setlevel( logger::error ) << "Size file header is not good."
			  << logger::setlevel( logger::info ) << "Trying to understand if the file is in the old format." << endl;

	    if( this->checkOldFormat() ) { // File in the old format !!! Good...
	      elog::cedglog << logger::setlevel( logger::info ) << "The file was in the old format... Well !!!" << endl
			    << logger::setlevel( logger::debug ) << "Position = " << this->sf_current.position() << endl
			    << "Pending jobs = " << this->sf_current.pending() << endl
			    << "Last job submitted = " << this->sf_current.last() << endl
			    << logger::setlevel( logger::info ) << "Reverting to the new format..." << endl;

	      this->newSizeFile();
	    }
	    else {
	      elog::cedglog << logger::setlevel( logger::error ) << "The file wasn't even in the old format." << endl
			    << logger::setlevel( logger::warning ) << "(Re)Starting with a zero sized file." << endl;

	      this->sf_current.reset( 0, 0, 0 );
	      this->newSizeFile();
	    }
	  }
	  else { // Yuck !!! I/O errors...
	    elog::cedglog << logger::setlevel( logger::severe ) << "Input/Output errors while reading size file." << endl;

	    this->sf_good = false;
	  }
	}
	else { // File is empty
	  elog::cedglog << logger::setlevel( logger::info ) << "Size file is empty. Writing new one." << endl;

	  this->sf_header.reset( sf_s_defaultHeader );
	  this->sf_current.reset( 0, 0, 0 );
	  this->sf_stream.seekp( 0 );
	  this->sf_stream << this->sf_header << endl << this->sf_current << endl;
	}
      }
      else { // Yuck !!! I/O errors...
	elog::cedglog << logger::setlevel( logger::severe ) << "Input/Output errors while reading size file." << endl;

	this->sf_good = false;
      }
    }
  }
  else { // Empty file name ???
    elog::cedglog << logger::setlevel( logger::severe ) << "Filename is empty: what I have to do ???" << endl;

    this->sf_good = false;
  }

  return;
}

void SizeFile::dumpField( void )
{
  if( this->sf_stream.good() )
    this->sf_stream << this->sf_current << endl;

  this->sf_good = this->sf_stream.good();

  return;
}

SizeFile::SizeFile( const char *filename, bool create ) : sf_good( true ), sf_filename( filename ? filename : "" ), sf_stream(),
							  sf_header(), sf_current()
{
  this->createDotFile();
  this->openFile( create );
}

void SizeFile::open( const char *filename, bool create )
{
  this->sf_good = true;
  this->sf_filename.assign( filename ? filename : "" );
  this->sf_stream.clear(); this->sf_stream.close();

  this->createDotFile();
  this->openFile( create );
}

SizeFile &SizeFile::update_position( long int new_position )
{
  if( this->sf_good ) {
    this->sf_current.position( new_position );
    this->dumpField();
  }

  return *this;
}

SizeFile &SizeFile::update_pending( unsigned int new_pending )
{
  if( this->sf_good ) {
    this->sf_current.pending( new_pending );
    this->dumpField();
  }

  return *this;
}

SizeFile &SizeFile::update_last( bool new_last )
{
  if( this->sf_good ) {
    this->sf_current.last( new_last );
    this->dumpField();
  }

  return *this;
}

SizeFile &SizeFile::update( long int new_position, unsigned int new_pending, bool new_last )
{
  if( this->sf_good ) {
    this->sf_current.reset( new_position, new_pending, new_last );
    this->dumpField();
  }

  return *this;
}

SizeFile &SizeFile::set_last( bool new_last )
{
  if( this->sf_good ) this->sf_current.last( new_last );

  return *this;
}

SizeFile &SizeFile::set_completed( void )
{
  if( this->sf_good ) this->sf_current.pending( 0 ).last( true );

  return *this;
}

SizeFile &SizeFile::increment_pending( void )
{
  unsigned int    old;

  if( this->sf_good ) {
    old = this->sf_current.pending() + 1;
    this->sf_current.pending( old );
  }

  return *this;
}

SizeFile &SizeFile::decrement_pending( void )
{
  unsigned int    old;

  if( this->sf_good ) {
    old = this->sf_current.pending();

    if( old > 0 ) {
      old -= 1;
      this->sf_current.pending( old );
    }
    else this->sf_good = false;
  }

  return *this;
}

SizeFile &SizeFile::update_header( const std::string &newheader )
{
  int                  fd;
  string::size_type    space;
  string               buffer;

  if( this->sf_good ) {
    if( this->sf_header.header().length() <= newheader.length() ) {
      space = newheader.length() - this->sf_header.header().length();

      buffer.assign( newheader );
      if( space > 1 ) {
	buffer.append( 1, '\n' );
	buffer.append( space - 1, '#' );
      }
      else if( space == 1 ) buffer.append( 1, ' ' );

      this->sf_header.reset( buffer );

      /*
	This is dangerous, too...
	But it should be faster than the next one...
      */
      if( this->sf_stream.good() ) {
	this->sf_stream.seekp( 0 );
	this->sf_stream << this->sf_header;

	this->sf_stream.seekp( 0, ios::end );

	this->sf_good = this->sf_stream.good();
      }
      else this->sf_good = false;
    }
    else {
      /*
	Dangerous operation, must rewind all the file and truncate it...
	Brrrrr...
	This restores back an old problem with I/O interrupted operations.
      */

      this->sf_header.reset( newheader );
      this->sf_stream.seekg( 0 ); this->sf_stream.seekp( 0 );

      fd = utilities::streamdescriptor( this->sf_stream );
      if( ftruncate(fd, 0) ) this->sf_good = false;
      else {
	this->sf_stream << this->sf_header << endl << this->sf_current << endl;

	this->sf_good = this->sf_stream.good();
      }
    }
  }

  return *this;
}

}; // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END;
