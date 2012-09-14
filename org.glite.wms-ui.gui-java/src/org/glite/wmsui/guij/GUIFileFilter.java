/*
 * GUIFileFilter.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.io.File;
import java.util.Vector;

class GUIFileFilter extends javax.swing.filechooser.FileFilter {
  // Accept all directories and all .jdl, .xml, no extension files.
  String description = null;

  Vector extensionVector = new Vector();

  static boolean showOnlyDirectory = false;

  public GUIFileFilter(String description, String[] extensions) {
    this.description = description;
    setExtensions(extensions);
  }

  public boolean accept(File file) {
    if (file.isDirectory()) {
      return true;
    }
    if (showOnlyDirectory) {
      return false;
    }
    if (extensionVector.size() == 0) {
      return true;
    }
    String extension = GUIFileSystem.getFileExtension(file);
    if (extension != null) {
      extension = extension.toUpperCase();
      if (extensionVector.contains(extension)) {
        return true;
      } else {
        return false;
      }
    }
    return false;
  }

  // Return the description of this filter.
  public String getDescription() {
    return description;
  }

  // Set the description of this filter.
  public void setDescription(String description) {
    this.description = description;
  }

  // Set extensions of the files to display.
  public void setExtensions(String[] extensions) {
    this.extensionVector.clear();
    if (extensions == null) {
      return;
    }
    for (int i = 0; i < extensions.length; i++) {
      this.extensionVector.add(extensions[i]);
    }
  }

  static public void setShowOnlyDirectory(boolean bool) {
    showOnlyDirectory = bool;
  }

  public boolean getShowOnlyDirectory() {
    return showOnlyDirectory;
  }
}