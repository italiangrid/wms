/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

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
