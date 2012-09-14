/**
 * File: Version.h
 * Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
 * Copyright (c) 2000 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$
 
#ifndef _GLITE_WMS_MANAGER_NS_VERSIONS_VERSION_H_
#define _GLITE_WMS_MANAGER_NS_VERSIONS_VERSION_H_

#include <string>

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace versions{

  /**
   * Version object.
   * This class allows version management and check.
   * @author Marco Pappalardo
   */
  class Version {

  private:
    /** major attribute. */
    int major;
    /** minor attribute. */
    int minor;
    /** revision attribute. */
    int revision;

  public:
    /**
     * Constructor.
     * Creates a new Version object having 1.0.0 version tag.
     */
    Version();

    /**
     * Constructor.
     * Creates a new Version objects getting info from a string
     * having x.y.z form. Wrong strings will produce an error.
     * @param ver the version as string.
     */
    Version(const std::string& ver);

    /**
     * Constructor.
     * Creates a new Version object from passed major, minor nad revision values.
     * @param major the major value for version.
     * @param minor the minor value for version.
     * @param revision the revision value for version.
     */
    Version(int maj, int min, int rev);

    /**
     * Destructor.
     */
    virtual ~Version() {};  

    /**
     * Fills a string with a string representation of version.
     * @param ver a string to fill with version.
     */
    void getVersion(std::string& ver);
    
    /**
     * Returns a string representation of version.
     * @return a string representation of version.
     */
    std::string asString();

    /**
     * Operator comparing this obejct to given one, looking for equality.
     * @param v the version v to compare with this object.
     * @return true if this is equal to v parameter, false otherwise.
     */
    bool operator== (const Version& v);

    /**
     * Operator comparing this obejct to given one, looking to see
     * whether this object is greater than given one.
     * @param v the version v to compare with this object.
     * @return true if this is greater than v parameter, false otherwise.
     */
    bool operator>  (const Version& v);

    /**
     * Operator comparing this obejct to given one, looking to see
     * whether this object is less than given one.
     * @param v the version v to compare with this object.
     * @return true if this is less than v parameter, false otherwise.
     */
    bool operator<  (const Version& v);

    /**
     * Operator comparing this obejct to given one, looking to see
     * whether this object is greater than given one.
     * @param v the version v to compare with this object.
     * @return true if this is greater than v parameter, false otherwise.
     */
    bool operator>= (const Version& v) {
      return (*this==v)||(*this>v);
    }

    /**
     * Operator comparing this obejct to given one, looking to see
     * whether this object is less than given one.
     * @param v the version v to compare with this object.
     * @return true if this is less than v parameter, false otherwise.
     */
    bool operator<= (const Version& v) {
      return (*this==v)||(*this<v);
    }

    /**
     * Operator comparing this obejct to given one, looking for disequality.
     * @param v the version v to compare with this object.
     * @return true if this is different than v parameter, false otherwise.
     */
    bool operator!= (const Version& v) {
      return !(*this==v);
    }

    /**
     * Major increment.
     */
    void increment_major() {
      major++;
    }

    /**
     * Minor increment.
     */
    void increment_minor() {
      minor++;
    }

    /**
     * Revision increment.
     */
    void increment_revision() {
      revision++;
    }

  };



}}}}} // namespace glite::wms::manager::ns::versions


#endif
