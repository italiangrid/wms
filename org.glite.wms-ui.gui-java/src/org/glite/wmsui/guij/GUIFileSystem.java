/*
 * GUIFileSystem.java 
 * 
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 * 
 */

package org.glite.wmsui.guij;

import java.awt.Component;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FilePermission;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Vector;
import javax.swing.JOptionPane;
import org.glite.jdl.Ad;
import org.glite.wmsui.apij.Api;

/**
 * Implementation of the GUIFileSystem class. This class provides constant
 * values, variable values and utility methods connected to general file
 * handling.
 * 
 * @ingroup gui
 * @brief This class provides some constant values and utility methods.
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class GUIFileSystem {
  static final String LOG4J_CONF_FILE_NAME = "glite_wmsui_gui_log4j.properties";

  // Logging level to use when application is unable to find Log4J configuration
  // file.
  //static final Level LOG4J_BASIC_CONF_LEVEL = Level.DEBUG;
  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final String EDG_LOCATION = "GLITE_LOCATION";

  static final String EDG_WL_LOCATION = "GLITE_WMS_LOCATION";

  // Environment variables will be checked in the specified order.
  static final String[] confFileLocationEnvironmentVars = { EDG_LOCATION,
      EDG_WL_LOCATION
  };

  // Alternative file locations will be checked in the specified order.
  // Write paths using initial and final file separators.
  static final String[] confFileAlternativeLocations = {
      File.separator + "opt" + File.separator + "glite" + File.separator
          + "etc" + File.separator,
      File.separator + "usr" + File.separator + "local" + File.separator
          + "etc" + File.separator, File.separator + "etc" + File.separator
  };

  static final String DEFAULT_ERROR_STORAGE_LOCATION = "/tmp";

  // GUI temporary file directory.
  static final String TEMPORARY_FILE_DIRECTORY = ".glite_wmsui_GUITempFiles";

  // GUI temporary copy file directory.
  static final String TEMPORARY_COPY_FILE_DIRECTORY = "GUITemporaryCopyFileDirectory";

  static final String USER_CONFIGURATION_FILE = "GUIUserPreferences.conf";

  static final String THREAD_CONFIGURATION_FILE = "GUIThread.log";

  // Monitor recovery file. Each user has a configuration file which keeps track
  // of the context.
  static final String JOBMONITOR_RECOVERY_FILE = "glite_wmsui_gui_jobmonitoring";

  // GUI configuration file.
  static final String GUI_CONF_VAR_FILE_NAME = "glite_wmsui_gui_var.conf";

  // VO specific configuration file (one for any VOs).
  static final String VO_CONF_FILE_NAME = "glite_wmsui.conf";

  // Condor dtd file used to read Job description file in xml format.
  // NOT NEEDED ANYMORE static final String CONDOR_DTD_FILE_NAME = "condor.dtd";
  // JDL Editor parsing error log file name.
  static final String ERROR_LOG_FILE_NAME = "glite_wmsui_jdle_error";

  // Version file.
  static final String VERSION_FILE = "version.txt";

  // The location of the files will be $home/X509_USER_FILE_DEFAULT,
  // $home/X509_USER_KEY_DEFAULT.
  static final String X509_USER_CERT_DEFAULT = ".globus/userfile.pem";

  static final String X509_USER_KEY_DEFAULT = ".globus/usercert.pem";

  static final String DEFAULT_JDL_EDITOR_SAVE_FILE_NAME = "job"; // job1, job2,

  // ...
  // File extensions.
  static final String JDL_FILE_EXTENSION = ".jdl";

  static final String ERROR_LOG_EXTENSION = ".log";

  static final String CE_ID_LIST_FILE_EXTENSION = ".lst";

  //static final String EDG_WL_GUI_CONFIG_VAR = "GLITE_WMS_GUI_CONFIG_VAR";
  //static final String EDG_WL_GUI_CONFIG_VO = "GLITE_WMS_GUI_CONFIG_VO";
  //!!! VO
  static final String VOMSES_FILE_NAME = "vomses";

  // VO
  // Initial configuration file name, used by JDLEditor.
  static String confFileName = "glite_wmsui_jdle_GLUE.xml";

  static final String EDG_XDAGMON_LOCATION = "GLITE_WMSUI_XDAGMON_LOCATION";

  static final String EDG_TOPOLOGY_LOCATION = "GLITE_WMSUI_TOPOLOGY_LOCATION";

  static final String EDG_REPOSITORY_LOCATION = "GLITE_WMSUI_REPOSITORY_LOCATION";

  //*** Vars
  static String guiConfVarFileName = "";

  static String oldConfFileName = "";

  static Vector confFileAttributesVector = new Vector();

  static Vector confFileFunctionsVector = new Vector();

  static Vector confFileReplicaCatalogVector = new Vector();

  static Vector confFileDataAccessProtocolVector = new Vector();

  static boolean isConfFileError = false;

  // Attributes type containing the attribute names read from configuration
  // file (attribute names used in Rank and Requirements simple panels).
  static java.util.jar.Attributes exprAttributesName;

  // END Vars
  /*****************************************************************************
   * METHODS
   ****************************************************************************/
  /*
   * METHODS TO GET FILE NAMES OR PATHS
   */
  static java.util.jar.Attributes getExprAttributesName() {
    return exprAttributesName;
  }

  static void parseConfigurationFile(Component component) {
    GUIConfigSAXParser configParser = new GUIConfigSAXParser();
    String configurationFile = "";
    try {
      configurationFile = GUIFileSystem.getConfigurationFile();
    } catch (IOException ioe) {
      if (isDebugging) {
        ioe.printStackTrace();
      }
      JOptionPane.showOptionDialog(component, ioe.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      JOptionPane.showOptionDialog(component, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
    }
    int result = configParser.parse(configurationFile);
    if (result == -1 && !isConfFileError) {
      JOptionPane.showOptionDialog(component,
          "Some problems occures opening configuration file"
              + "\nConfiguration values cannot be loaded",
          Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.WARNING_MESSAGE, null, null, null);
      isConfFileError = true;
    }
    exprAttributesName = configParser.getExprAttributesName();
    confFileAttributesVector = configParser.getAttribute();
    confFileFunctionsVector = configParser.getFunction();
    confFileReplicaCatalogVector = configParser.getReplicaCatalogVector();
    confFileDataAccessProtocolVector = configParser
        .getDataAccessProtocolVector();
  }

  static boolean getIsConfFileError() {
    return isConfFileError;
  }

  static Vector getConfigurationAttributes(Component component) {
    if (!oldConfFileName.equals(confFileName)
        || (confFileAttributesVector.size() == 0)) {
      parseConfigurationFile(component);
    }
    return confFileAttributesVector;
  }

  static Vector getConfigurationFunctions(Component component) {
    if (!oldConfFileName.equals(confFileName)
        || (confFileFunctionsVector.size() == 0)) {
      parseConfigurationFile(component);
    }
    return confFileFunctionsVector;
  }

  static Vector getConfigurationReplicaCatalog(Component component) {
    if (!oldConfFileName.equals(confFileName)
        || (confFileReplicaCatalogVector.size() == 0)) {
      parseConfigurationFile(component);
    }
    return confFileReplicaCatalogVector;
  }

  static Vector getConfigurationDataAccessProtocol(Component component) {
    if (!oldConfFileName.equals(confFileName)
        || (confFileDataAccessProtocolVector.size() == 0)) {
      parseConfigurationFile(component);
    }
    return confFileDataAccessProtocolVector;
  }

  /**
   * Return the location (path) of the configuration var file.
   */
  static String getConfFileLocation() {
    String location = "";
    if (File.separator.equals("/")) {
      for (int i = 0; i < confFileLocationEnvironmentVars.length; i++) {
        location = Api.getEnv(confFileLocationEnvironmentVars[i]);
        if ((location != null) && !location.trim().equals("")) {
          int locationLength = location.length();
          if (!location.substring(locationLength - 1, locationLength).equals(
              File.separator)) {
            location += File.separator;
          }
          location += "etc" + File.separator;
          if (new File(location + GUI_CONF_VAR_FILE_NAME).isFile()) {
            return location;
          } else {
            return "";
          }
        }
      }
      for (int i = 0; i < confFileAlternativeLocations.length; i++) {
        location = confFileAlternativeLocations[i].trim();
        int locationLength = location.length();
        if (!location.substring(locationLength - 1, locationLength).equals(
            File.separator)) {
          location += File.separator;
        }
        if (new File(location + GUI_CONF_VAR_FILE_NAME).isFile()) {
          return location;
        }
      }
    }
    return "";
  }

  static String getVersionFile() {
    return getConfFileLocation() + VERSION_FILE;
  }

  static String getLog4JConfFile() {
    return getConfFileLocation() + LOG4J_CONF_FILE_NAME;
  }

  static String getGUIConfVarFile() {
    String fileName = Api.getEnv(Utils.EDG_WL_GUI_CONFIG_VAR);
    if ((fileName != null) && !fileName.trim().equals("")) {
      if (new File(fileName).isFile()) {
        return fileName;
      }
    }
    return getDefaultGUIConfVarFile();
  }

  static String getDefaultGUIConfVarFile() {
    return getConfFileLocation() + GUI_CONF_VAR_FILE_NAME;
  }

  static String getGUIConfVOFile() {
    String fileName = Api.getEnv(Utils.EDG_WL_GUI_CONFIG_VO);
    if ((fileName != null) && !fileName.trim().equals("")) {
      if (new File(fileName).isFile()) {
        return fileName;
      }
    }
    String location = getConfFileLocation();
    return location + GUIGlobalVars.getVirtualOrganisation().toLowerCase()
        + File.separator + VO_CONF_FILE_NAME;
  }

  /**
   * Returns the name of the user temporary file directory as computed by
   * computeUserTempFileDirectory() method (this method computes the directory
   * name and stores it in the global variable
   * GUIGlobalVars.userTemporaryFileDirectory).
   */
  static String getUserTempFileDirectoryName() {
    return GUIGlobalVars.userTemporaryFileDirectory;
  }

  /**
   * Returns temporary file directory path. The path is relative. The absolute
   * path depends on what needed. For example you can add at the beginning the
   * user home path or the installation path.
   */
  static String getTemporaryFileDirectory() {
    return TEMPORARY_FILE_DIRECTORY + File.separator
        + getUserTempFileDirectoryName() + File.separator;
  }

  static String getJobTemporaryFileDirectory() {
    return getUserHomeDirectory() + File.separator + TEMPORARY_FILE_DIRECTORY
        + File.separator + getUserTempFileDirectoryName() + File.separator
        + getPreferencesDirectoryName() + File.separator;
  }

  static String getTemporaryCopyFileDirectory() {
    return getUserHomeDirectory() + File.separator
        + getTemporaryFileDirectory() + File.separator
        + TEMPORARY_COPY_FILE_DIRECTORY;
  }

  /**
   * Returns the preferences directory name depending on the type of preferences
   * actually selected from user; user or default preferences.
   */
  static String getPreferencesDirectoryName() {
    return "User";
  }

  /**
   * Returns the xml configuration file name with path used to configure JDL
   * Editor application. If the starting application is an applet or an internal
   * frame, it returns a blank string.
   */
  static String getConfigurationFile() throws IOException, Exception {
    if (Utils.applicationType == Utils.FRAME) {
      String location = getConfFileLocation();
      if (location != null) {
        return location + confFileName;
      } else {
        throw new Exception("Unable to find configuration file");
      }
    } else { // Utils.applicationType == APPLET or INTERNAL_FRAME
      return "";
    }
  }

  static String getUserPrefFile() {
    return getUserHomeDirectory() + File.separator
        + getTemporaryFileDirectory() + USER_CONFIGURATION_FILE;
  }

  /*
   * NEVER USED static void setConfFileName(String fileName) { oldConfFileName =
   * confFileName; confFileName = fileName; }
   */
  /*
   * NEVER USED static String getConfFileName() { return confFileName; }
   */
  /**
   * Returns default working directory checking for O.S. type.
   */
  static String getDefaultWorkingDirectory() {
    String workingDir = "";
    if (Utils.applicationType != Utils.APPLET) {
      String operatingSystem = System.getProperty("os.name");
      if (operatingSystem.equals("Linux") || operatingSystem.equals("Unix") || // Maybe
          // "Unix"
          // not
          // necessary.
          operatingSystem.equals("Solaris") || operatingSystem.equals("SunOS")
          || operatingSystem.equals("Digital Unix")) {
        workingDir = new String((new File("")).getAbsolutePath());
      } else {
        workingDir = new String((new File("")).getAbsolutePath()); // Maybe
        // "user.dir"
        // is better.
      }
    }
    return workingDir;
  }

  /*
   * NEVER USED static String getCurrentWorkingDirectory() { if
   * (Utils.applicationType != Utils.APPLET) { return (new
   * File("")).getAbsolutePath(); } else { return ""; } }
   */
  /**
   * Returns the condor .dtd file name added when you write a jdl file in a .xml
   * format ( (...) SYSTEM "file:./CONDOR_DTD_FILE_NAME" ).
   */
  /*
   * NOT NEEDED ANYMORE static String getCondorDTDFile() { if (applicationType !=
   * APPLET) { String location = Api.getEnv(EDG_WL_LOCATION); if ((location !=
   * null) && !location.trim().equals("")) { return location + File.separator +
   * "etc" + File.separator + CONDOR_DTD_FILE_NAME; } } return "./etc" +
   * File.separator + CONDOR_DTD_FILE_NAME; }
   */
  /**
   * Converts a string to a usable directory name. This method is used to get
   * the name of user temporary file directory from the proxy subject.
   */
  static String stringToDirectoryName(String inputString) {
    String outputString = "";
    char currentChar;
    for (int i = 0; i < inputString.length(); i++) {
      currentChar = inputString.charAt(i);
      switch (currentChar) {
        case ' ':
          outputString += '_';
        case '/':
          outputString += '0';
          continue;
        case '\\':
          outputString += '1';
          continue;
        case ':':
          outputString += '2';
          continue;
        case '*':
          outputString += '3';
          continue;
        case '?':
          outputString += '4';
          continue;
        case '<':
          outputString += '5';
          continue;
        case '>':
          outputString += '6';
          continue;
        case '|':
          outputString += '7';
          continue;
        case '"':
          outputString += '8';
          continue;
        case '\'':
          outputString += '9';
          continue;
      }
      outputString += currentChar;
    }
    return outputString;
  }

  static Vector getVirtualOrganisations() {
    Vector returnVector = new Vector();
    File voLocationFile = new File(getConfFileLocation());
    if (voLocationFile != null) {
      File[] files = voLocationFile.listFiles();
      if (files != null) {
        File voConfFile;
        for (int i = 0; i < files.length; i++) {
          if (files[i].isDirectory()) {
            voConfFile = new File(files[i] + File.separator + VO_CONF_FILE_NAME);
            if (voConfFile.isFile()) {
              returnVector.add(files[i].getName());
            }
          }
        }
      }
    }
    return returnVector;
  }

  static boolean isAbsolute(String path) {
    int length = path.length();
    if (length == 0) {
      return false;
    }
    if (path.charAt(0) == '\\') {
      if (path.indexOf("/") == -1) {
        return true;
      } else {
        return false;
      }
    }
    if (path.charAt(0) == '/') {
      if (path.indexOf("\\") == -1) {
        return true;
      } else {
        return false;
      }
    }
    if ((length >= 3) && Character.isLetter(path.charAt(0))
        && (path.charAt(1) == ':') && (path.charAt(2) == '\\')
        && (path.indexOf("/") == -1)) {
      return true;
    }
    return false;
  }

  static int getPrefixLength(String path) {
    if (path.length() == 0) {
      return 0;
    }
    return (path.charAt(0) == '/') ? 1 : 0;
  }

  static String getName(File fileName) {
    return getName(fileName.toString());
  }

  /**
   * //!!! TO CHANGE!!! Return the file name of a path (expressed in Unix-like
   * or Windows format). If path has no name return an empty string, if the path
   * has wrong format return blank String.
   */
  static String getName(String path) {
    int indexBackSlash = path.lastIndexOf("\\");
    int indexSlash = path.lastIndexOf("/");
    if ((path.lastIndexOf("\\") != -1) && (path.lastIndexOf("/") != -1)) {
      return null;
    } else {
      String name1 = "";
      String name2 = "";
      int prefixLength = getPrefixLength(path);
      if (indexBackSlash < prefixLength) {
        name1 = path.substring(prefixLength);
      } else {
        name1 = path.substring(indexBackSlash + 1);
      }
      prefixLength = getPrefixLength(path);
      if (indexSlash < prefixLength) {
        name2 = path.substring(prefixLength);
      } else {
        name2 = path.substring(indexSlash + 1);
      }
      return (name1.length() < name2.length()) ? name1 : name2;
    }
  }

  static String getNameNoExtension(String path) {
    String name = getName(path);
    String ext = getFileExtension(new File(name));
    int index = -1;
    if (!ext.equals("")) {
      index = name.indexOf(ext);
    }
    if (index != -1) {
      name = name.substring(0, index - 1);
    }
    return name;
  }

  /*
   * NEVER USED static boolean isCheckFileLocalOk(String attributeName, String
   * path, Component component) { if (path.substring(path.length() -
   * 1).equals(File.separator)) { JOptionPane.showOptionDialog(component, "- " +
   * attributeName + ": inserted path has no file name", Utils.ERROR_MSG_TXT,
   * JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null, null);
   * return false; } if (File.separator.equals("/")) { // Unix like System. if
   * ((path.indexOf("\\") != -1) || (path.indexOf(":") != -1) ||
   * (path.indexOf("*") != -1) || (path.indexOf("?") != -1) ||
   * (path.indexOf("\"") != -1) || (path.indexOf(" <") != -1) ||
   * (path.indexOf(">") != -1) || (path.indexOf("|") != -1) ||
   * (path.indexOf("'") != -1)) { JOptionPane.showOptionDialog(component, "- " +
   * attributeName + ": inserted path and file name cannot contain: " + "\\, :, *, ?,
   * \", <, >, |, '", Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
   * JOptionPane.ERROR_MESSAGE, null, null, null); return false; } } else { //
   * Windows System. int colonIndex = path.indexOf(":"); if (((colonIndex != -1) &&
   * (colonIndex != 1)) || (path.indexOf("/") != -1) || (path.indexOf("*") !=
   * -1) || (path.indexOf("?") != -1) || (path.indexOf("\"") != -1) ||
   * (path.indexOf(" <") != -1) || (path.indexOf(">") != -1) ||
   * (path.indexOf("|") != -1)) { JOptionPane.showOptionDialog(component, "- " +
   * attributeName + ": inserted path and file name cannot contain: " + "/, :
   * (only after drive letter), *, ?, \", <, >, |", Utils.ERROR_MSG_TXT,
   * JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null, null);
   * return false; } } return true; }
   */
  static String checkFileLocal(String attributeName, String path,
      Component component) {
    String errorMsg = "";
    if (path.substring(path.length() - 1).equals(File.separator)) {
      errorMsg += "- " + attributeName + ": inserted path has no file name\n";
    }
    if (File.separator.equals("/")) { // Unix like System.
      if ((path.indexOf("\\") != -1) || (path.indexOf(":") != -1)
          || (path.indexOf("*") != -1) || (path.indexOf("?") != -1)
          || (path.indexOf("\"") != -1) || (path.indexOf("<") != -1)
          || (path.indexOf(">") != -1) || (path.indexOf("|") != -1)
          || (path.indexOf("'") != -1)) {
        errorMsg += "- " + attributeName
            + ": inserted path and file name cannot contain: "
            + "\\, :, *, ?, \", <, >, |, '\n";
      }
    } else { // Windows System.
      int colonIndex = path.indexOf(":");
      if (((colonIndex != -1) && (colonIndex != 1))
          || (path.indexOf("/") != -1) || (path.indexOf("*") != -1)
          || (path.indexOf("?") != -1) || (path.indexOf("\"") != -1)
          || (path.indexOf("<") != -1) || (path.indexOf(">") != -1)
          || (path.indexOf("|") != -1)) {
        errorMsg += "- " + attributeName
            + ": inserted path and file name cannot contain: "
            + "/, : (only after drive letter), *, ?, \", <, >, |\n";
      }
    }
    return errorMsg;
  }

  /*
   * NEVER USED static boolean isCheckFileRemoteOk(String attributeName, String
   * path, Component component) { char last = path.charAt(path.length() - 1); if
   * ((last == '/') || (last == '\\')) { JOptionPane.showOptionDialog(component, "- " +
   * attributeName + ": inserted path has no file name", Utils.ERROR_MSG_TXT,
   * JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null, null);
   * return false; } //String oppositeSlash = (File.separator.equals("/")) ?
   * "\\" : "/"; if ((path.indexOf("*") != -1) || (path.indexOf("?") != -1) ||
   * (path.indexOf("\"") != -1) || (path.indexOf(" <") != -1) ||
   * (path.indexOf(">") != -1) || (path.indexOf("|") != -1)) {
   * JOptionPane.showOptionDialog(component, "- " + attributeName + ": inserted
   * remote path and file name cannot contain: " + "*, ?, \", <, >, |",
   * Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE,
   * null, null, null); return false; } return true; }
   */
  static String checkFileRemote(String attributeName, String path,
      Component component) {
    String errorMsg = "";
    char last = path.charAt(path.length() - 1);
    if ((last == '/') || (last == '\\')) {
      errorMsg += "- " + attributeName + ": inserted path has no file name\n";
    }
    if ((path.indexOf("*") != -1) || (path.indexOf("?") != -1)
        || (path.indexOf("\"") != -1) || (path.indexOf("<") != -1)
        || (path.indexOf(">") != -1) || (path.indexOf("|") != -1)) {
      errorMsg += "- " + attributeName
          + ": inserted remote path and file name cannot contain: "
          + "*, ?, \", <, >, |\n";
    }
    return errorMsg;
  }

  static Ad loadPrefFileAd() {
    File prefFile = new File(getUserPrefFile());
    Ad prefAd = new Ad();
    if (prefFile.isFile()) {
      try {
        prefAd.fromFile(prefFile.toString());
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
    }
    return prefAd;
  }

  static String getUserHomeDirectory() {
    if (Utils.applicationType != Utils.APPLET) {
      return System.getProperty("user.home");
    } else {
      return "";
    }
  }

  /**
   * Gets the extension of the input file. If the file name end with '.', the
   * extension is set to empty string.
   */
  public static String getFileExtension(File file) {
    String fileName = file.getName();
    String extension = "";
    int fileNameLength = fileName.length();
    char currentChar;
    if (fileName.charAt(fileNameLength - 1) == '.') {
      return ""; // File names ending with '.'
    }
    for (int i = fileNameLength - 2; i > 0; i--) {
      currentChar = fileName.charAt(i);
      if (currentChar == '.') {
        extension = fileName.substring(i + 1);
        return extension;
      }
    }
    return extension;
  }

  /*
   * NEVER USED public static String getExtension(File file) { String ext =
   * null; String name = file.getName(); int i = name.lastIndexOf('.'); if (i >
   * 0 && i < name.length() - 1) { ext = name.substring(i + 1).toLowerCase(); }
   * return ext; }
   */
  public static void copyFile(File inputFile, File outputFile)
      throws IOException {
    FileInputStream fileInputStream = new FileInputStream(inputFile);
    FileOutputStream fileOutputStream = new FileOutputStream(outputFile);
    byte[] buffer = new byte[1024];
    int i = 0;
    while ((i = fileInputStream.read(buffer)) != -1) {
      fileOutputStream.write(buffer, 0, i);
    }
    fileInputStream.close();
    fileOutputStream.close();
  }

  public static String readTextFile(File file) throws Exception {
    String result = "";
    if (!file.isFile()) {
      //return result;
      throw new Exception("File not found: " + file);
    }
    try {
      BufferedReader in = new BufferedReader(new FileReader(file));
      String line;
      while ((line = in.readLine()) != null) {
        result += line;
      }
      in.close();
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      throw e;
    }
    return result;
  }

  public static void saveTextFile(File file, String text) throws EOFException,
      IOException, Exception {
    saveTextFile(file.toString(), text, false);
  }

  public static void saveTextFile(String outputFile, String text)
      throws EOFException, IOException, Exception {
    saveTextFile(outputFile, text, false);
  }

  public static void saveTextFile(File file, String text,
      boolean checkPermission) throws SecurityException, EOFException,
      IOException, Exception {
    saveTextFile(file.toString(), text, checkPermission);
  }

  public static void saveTextFile(String outputFile, String text,
      boolean checkPermission) throws SecurityException, EOFException,
      IOException, Exception {
    if (checkPermission) {
      try {
        FilePermission perm = new FilePermission(outputFile, "write");
        java.security.AccessController.checkPermission(perm);
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        throw new SecurityException("No authorization to write file");
      }
    }
    try {
      DataOutputStream dos = new DataOutputStream(new BufferedOutputStream(
          new FileOutputStream(outputFile)));
      dos.writeBytes(text);
      dos.flush();
      dos.close();
    } catch (EOFException eofe) {
      if (isDebugging) {
        eofe.printStackTrace();
      }
      throw eofe;
    } catch (IOException ioe) {
      if (isDebugging) {
        ioe.printStackTrace();
      }
      throw ioe;
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
      throw ex;
    }
  }

  static void removeUserTempFileDirectory()
      throws java.io.FileNotFoundException, java.io.IOException {
    try {
      removeDirectoryTree(new File(getUserHomeDirectory() + File.separator
          + getTemporaryFileDirectory()));
    } catch (java.io.FileNotFoundException fnfe) {
      throw fnfe;
    } catch (java.io.IOException ioe) {
      throw ioe;
    }
  }

  static void removeUserSessionFiles() throws java.io.IOException {
    File temporaryFileDirectory = new File(getUserHomeDirectory()
        + File.separator + getTemporaryFileDirectory());
    if (temporaryFileDirectory.isDirectory()) {
      File[] files = temporaryFileDirectory.listFiles();
      for (int i = 0; i < files.length; i++) {
        if (files[i].isDirectory()) {
          removeDirectoryTree(files[i]);
        }
      }
    }
  }

  static void removeUserTempFileDirectory(boolean checkPrefFile)
      throws java.io.IOException {
    File temporaryFileDirectory = new File(getUserHomeDirectory()
        + File.separator + getTemporaryFileDirectory());
    if (temporaryFileDirectory.isDirectory()) {
      if (checkPrefFile) {
        File userPrefFile = new File(getUserPrefFile());
        if (!userPrefFile.isFile()) {
          removeDirectoryTree(temporaryFileDirectory);
        }
      } else {
        removeDirectoryTree(temporaryFileDirectory);
      }
    }
  }

  public static void removeDirectoryDescendant(File file)
      throws java.io.FileNotFoundException, java.io.IOException {
    if (file.exists()) {
      if (file.isDirectory()) {
        File[] files = file.listFiles();
        for (int i = 0; i < files.length; i++) {
          if (files[i].isDirectory()) {
            removeDirectoryTree(files[i]);
          } else if (!files[i].delete()) {
            throw new java.io.IOException("File deleting error");
          }
        }
        //if(!file.delete()) throw new java.io.IOException("File deleting
        // error");
      }
    } else {
      throw new java.io.FileNotFoundException("Input file not found");
    }
  }

  public static void removeDirectoryTree(File file)
      throws java.io.FileNotFoundException, java.io.IOException {
    if (file.exists()) {
      if (file.isDirectory()) {
        File[] files = file.listFiles();
        for (int i = 0; i < files.length; i++) {
          if (files[i].isDirectory()) {
            removeDirectoryTree(files[i]);
          } else if (!files[i].delete()) {
            throw new java.io.IOException("File deleting error");
          }
        }
        if (!file.delete()) {
          throw new java.io.IOException("File deleting error");
        }
      } else if (!file.delete()) {
        throw new java.io.IOException("File deleting error");
      }
    } else {
      throw new java.io.FileNotFoundException("Input file not found");
    }
  }

  /**
   * Reads the input file <cose>fileName</code> producing a Vector of lines
   * (String). The Vector is created removing all comment lines (i.e. line
   * beginning with '#' or '//').
   * 
   * @param fileName
   *          the name of the input file
   * @return a Vector containing the lines of the file
   */
  public static Vector readTextFileLines(String fileName) throws EOFException,
      FileNotFoundException, IOException {
    Vector lineVector = new Vector();
    String inputLine;
    int length = 0;
    int rowNumber = 0;
    try {
      try {
        BufferedReader inputFile = new BufferedReader(new InputStreamReader(
            new FileInputStream(fileName)));
        inputLine = inputFile.readLine().trim();
        while (inputLine != null) {
          inputLine = inputLine.trim();
          // Check for blank or comment line.
          String trimmedInputLine = inputLine.trim();
          length = trimmedInputLine.length();
          if ((length == 0)
              || (trimmedInputLine.charAt(0) == '#')
              || (trimmedInputLine.charAt(0) == '*')
              || ((length >= 2) && (trimmedInputLine.substring(0, 2)
                  .equals("//")))) {
            inputLine = inputFile.readLine();
            rowNumber++;
            continue;
          }
          lineVector.add(inputLine);
          inputLine = inputFile.readLine();
          rowNumber++;
        }
      } catch (EOFException eofe) {
        if (isDebugging) {
          eofe.printStackTrace();
        }
        throw eofe;
      }
    } catch (FileNotFoundException fnfe) {
      if (isDebugging) {
        fnfe.printStackTrace();
      }
      throw fnfe;
    } catch (IOException ioe) {
      if (isDebugging) {
        ioe.printStackTrace();
      }
      throw ioe;
    }
    return lineVector;
  }
}