#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_SIZEFILE_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_SIZEFILE_H

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor {

class SizeField {
  friend std::ostream &operator<<( std::ostream &os, const SizeField &sf );
  friend std::istream &operator>>( std::istream &is, SizeField &sf );

public:
  SizeField( void );
  SizeField( long int position, unsigned int pending, bool last );
  ~SizeField( void );

  SizeField &reset( long int position, unsigned int pending, bool last );

  inline SizeField &pending( unsigned int pending ) { this->sf_pending = pending; return *this; }
  inline SizeField &position( long int position ) { this->sf_position = position; return *this; }
  inline SizeField &last( bool last ) { this->sf_last = last; return *this; }

  inline bool good( void ) const { return this->sf_good; }
  inline bool last( void ) const { return this->sf_last; }
  inline unsigned int pending( void ) const { return this->sf_pending; }
  inline long int position( void ) const { return this->sf_position; }

  static size_t dimension( void ) { return sf_s_long + sf_s_unsigned + 5; }

private:
  static size_t   sf_s_long, sf_s_unsigned;

  bool            sf_good, sf_last;
  unsigned int    sf_pending;
  long int        sf_position;
};

class SizeHeader {
  friend std::ostream &operator<<( std::ostream &os, const SizeHeader &sh );
  friend std::istream &operator>>( std::istream &is, SizeHeader &sh );

public:
  SizeHeader( const char *header = NULL );
  SizeHeader( const std::string &header );
  ~SizeHeader( void );

  SizeHeader &reset( const std::string &header );

  inline SizeHeader &reset( const char *header = NULL ) { return this->reset( header ? std::string(header) : std::string() ); }

  inline bool good( void ) const { return this->sh_good; }
  inline std::streamoff size( void ) const { return( this->sh_header.size() + 4 ); }
  inline const std::string &header( void ) const { return this->sh_header; }

private:
  bool          sh_good;
  std::string   sh_header;
};

class SizeFile {
public:
  SizeFile( const char *filename, bool create = false );

  void open( const char *filename, bool create = false );

  SizeFile &update_position( long int new_position );
  SizeFile &update_pending( unsigned int new_pending );
  SizeFile &update_last( bool new_last );
  SizeFile &update( long int new_position, unsigned int new_pending, bool new_last );
  SizeFile &set_last( bool new_last );
  SizeFile &set_completed( void );
  SizeFile &increment_pending( void );
  SizeFile &decrement_pending( void );
  SizeFile &update_header( const std::string &newheader );

  inline bool good( void ) const { return this->sf_good; }
  inline bool completed( void ) const
  { return( this->sf_good && this->sf_current.last() && (this->sf_current.pending() == 0) ); }
  inline const std::string &filename( void ) const { return this->sf_filename; }
  inline const SizeField &size_field( void ) const { return this->sf_current; }
  inline const SizeHeader &header( void ) const { return this->sf_header; }

private:
  void createDotFile( void );
  void openFile( bool create );
  void stashFile( void );
  void reopenFile( void );
  void newSizeFile( void );
  void dumpField( void );
  bool checkOldFormat( void );
  SizeField readLastField( void );
  SizeField readField( std::streamoff position );

  static const std::string   sf_s_defaultHeader;

  bool          sf_good;
  bool          sf_stashed;
  std::ios::pos_type sf_stash_pos;
  std::string   sf_filename;
  std::fstream  sf_stream;
  SizeHeader    sf_header;
  SizeField     sf_current;
};

}; // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_SIZEFILE_H */

// Local Variables:
// mode: c++
// End:
