#ifndef GLITE_WMS_COMMON_CONFIGURATION_CONFBASE_H
#define GLITE_WMS_COMMON_CONFIGURATION_CONFBASE_H

#include <vector>
#include <string>

namespace classad { class ClassAd; class ExprTree;}

namespace glite {
namespace wms {
namespace common {
namespace configuration {

class confbase_c {
public:
  virtual ~confbase_c( void );

  inline const classad::ClassAd *get_classad( void ) const
  { return this->cb_ad; }

protected:
  confbase_c( const classad::ClassAd *ad );

  bool getBool( const char *name, bool def ) const;
  int getInt( const char *name, int def ) const;
  double getDouble( const char *name, double def ) const;
  std::string getString( const char *name, const std::string &def ) const;
  std::string getAndParseString( const char *name, const std::string &def ) const;
  std::string getAndParseFileName( const char *name, const std::string &def ) const;
  std::vector<std::string> getVector( const char *name ) const;
  classad::ExprTree* getExpression(const char *name) const;
  classad::ClassAd* getClassAd(const char *name) const;  
  const classad::ClassAd        *cb_ad;

private:
  confbase_c( const confbase_c &cb ); // Not implemented
  confbase_c &operator=( const confbase_c &cb ); // Not implemented
};

} // Namespace configuration
} // common namespace end
} // wms namespace end
} // glite namespace end


#endif /* GLITE_WMS_COMMON_CONFIGURATION_CONFBASE_H */

// Local Variables:
// mode: c++
// End:
