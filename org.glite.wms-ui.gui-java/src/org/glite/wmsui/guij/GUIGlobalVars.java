/*
 * GUIGlobalVars.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.io.File;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Vector;
import org.glite.jdl.Jdl;
import org.glite.jdl.JobAd;
import org.glite.wmsui.apij.Api;

/**
 * Implementation of the GUIGlobalVars class.
 * This class contains the global variables used by the GUI.
 *
 * @ingroup gui
 * @brief Utility class used to store global variables values.
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class GUIGlobalVars {
  static final int NO_RETRY_COUNT = -1;

  //*** Certificate and Proxy context.
  static String proxyFilePath = "";

  static String certFilePath = "";

  static String certKeyPath = "";

  static String trustedCertDir = "";

  static String proxySubject = "";

  static boolean hasVOMSExtension = false;

  //*** END
  // Working Virtual Organisation
  static private String virtualOrganisation = "";

  //*** Current requirements, rank and rankMPI values.
  // Variable used to mantain Requirements attribute default value.
  //static private String guiConfVarRequirements = "true"; //Jdl.REQUIREMENTS_DEFAULT;
  static private String guiConfVarRequirements = "";

  // Variable used to mantain Rank attribute default value.
  // If no value is provided in conf file, API default will be used.
  //static private String guiConfVarRank = Jdl.RANK_DEFAULT;
  static private String guiConfVarRank = "";

  // Variable used to mantain RankMPI attribute default value.
  static private String guiConfVarRankMPI = "";

  static Vector srrDefaultVector;

  //*** END
  //*** Job Submitter preferences working values.
  // Job Submitter preferences Network Server panel.
  static private Vector nsVector = new Vector();

  static Map nsLBMap = new HashMap();

  static Map tempNSLBMap = new HashMap();

  static Map defaultNSLBMap = new HashMap();

  // Job Monitor LB Vector.
  static private Vector lbVector = new Vector();

  // Job Submitter preferences JDL Defaults panel.
  static private String hlrLocation = "";

  static private String myProxyServer = "";

  static private int guiConfVarRetryCount = NO_RETRY_COUNT;

  // Job Submitter preferences Logging panel.
  static private String guiConfVarErrorStorage = "/tmp";

  static private int guiConfVarLoggingTimeout = Utils.LOGGING_TIMEOUT_DEF_VAL;

  static private int guiConfVarLoggingSyncTimeout = Utils.LOGGING_SYNC_TIMEOUT_DEF_VAL;

  static private String guiConfVarLoggingDestination = "";

  // These values are present in the preferences file but user can't set
  // the values directly from GUI.
  static private int nsLoggerLevel = Utils.NS_LOGGER_LEVEL_DEF_VAL;

  //*** END
  // Application Version. The value is read from a file containing only this value.
  static private String guiConfVarVersion = "Unavailable";

  //*** Hash Maps.
  // Map used to store NetworkServer objects. The key is the name of the Job Submitter
  // tabbed pane panel the value is the NetworkServer.
  static Map nsMap = new HashMap();

  // Keep track of current opened editors, the key has the form "<NSPanel> - <keyJobName>",
  // the value is the reference to the editor instance.
  static Map openedEditorHashMap = new HashMap();

  static Map openedListenerFrameMap = new HashMap();

  static Map openedCheckpointStateMap = new HashMap();

  static Map openedListmatchMap = new HashMap();

  //*** END
  //*** Copy and Paste functionalities.
  // Keep track of copied name(s) of NSPanel job(s).
  static Vector selectedJobNameCopyVector = new Vector();

  // Keep track of the RBPanel name from where the job name are copied.
  static String selectedRBPanelCopy = "";

  //*** END
  // FileChooser instances use this default directory.
  static String fileChooserWorkingDirectory = "";

  // User temporary file directory to save context files in.
  static String userTemporaryFileDirectory = "";

  // Stores true if user decided to restore previous context.
  static boolean restorePreviousSession = false;

  // Path of configuration file as set in Environment variable.
  static String envVOConfFile = "";

  // JobAd containing the attribute names an values to insert in JobAd authomatically.
  // i.e. default values.
  static JobAd jobAdAttributeToAdd = new JobAd();

  // Default update rate.
  static private int jobMonitorUpdateRate = Utils.UPDATE_RATE_DEF_VAL;

  // Update mode.
  static private volatile int updateMode = Utils.LB_MODE;

  // METHODS
  static protected void setUpdateMode(int mode) {
    if ((mode == Utils.LB_MODE) || (mode == Utils.RGMA_MODE)) {
      updateMode = mode;
    } else {
      updateMode = Utils.LB_MODE;
    }
  }

  static protected int getUpdateMode() {
    return updateMode;
  }

  static protected String getHLRLocation() {
    return hlrLocation;
  }

  static protected void setHLRLocation(String location) {
    if (location != null) {
      hlrLocation = location.trim();
    }
  }

  static protected void setMyProxyServer(String myProxyServer) {
    if (myProxyServer != null) {
      GUIGlobalVars.myProxyServer = myProxyServer.trim();
    }
  }

  static protected String getMyProxyServer() {
    return GUIGlobalVars.myProxyServer;
  }

  static protected void setLBVector(Vector vector) {
    lbVector = (Vector) vector.clone();
  }

  static protected Vector getLBVector() {
    return lbVector;
  }

  static protected void setNSVector(Vector vector) {
    nsVector = (Vector) vector.clone();
  }

  static protected Map getNSLBMap() {
    return nsLBMap;
  }

  /*static protected void setNSLBMap(Map nsLBMap) {
   nsLBMap = nsLBMap;
   }*/
  static protected Vector getNSVector() {
    return nsVector;
  }

  static protected String getFileChooserWorkingDirectory() {
    return fileChooserWorkingDirectory;
  }

  static protected void setFileChooserWorkingDirectory(String directory) {
    if ((new File(directory)).isDirectory()) {
      fileChooserWorkingDirectory = directory;
    }
  }

  /**
   * Returns all openedEditorHashMap keys of the JDLEditor opened from the
   * specified Network Server panel.
   * openedEditorHashMap key is in the form "<panel name> - <job name>"
   */
  static protected Vector getNSPanelOpenedEditorKeyVector(String nsName) {
    Vector returnVector = new Vector();
    Iterator keyIterator = openedEditorHashMap.keySet().iterator();
    String key;
    String currentNSName;
    int lastIndex = -1;
    while (keyIterator.hasNext()) {
      key = keyIterator.next().toString().trim();
      lastIndex = key.lastIndexOf("-");
      if (lastIndex != -1) {
        currentNSName = key.substring(0, lastIndex).trim();
        if (currentNSName.equals(nsName)) {
          returnVector.add(key);
        }
      }
    }
    return returnVector;
  }

  static void setJobMonitorUpdateRate(int minutes) {
    if (minutes < 0) {
      return;
    }
    jobMonitorUpdateRate = minutes;
  }

  static int getJobMonitorUpdateRate() {
    return jobMonitorUpdateRate;
  }

  static void setVirtualOrganisation(String vo) {
    virtualOrganisation = vo.trim();
  }

  static String getVirtualOrganisation() {
    return virtualOrganisation;
  }

  static void setGUIConfVarRequirements(String expr) {
    if ((expr != null) && !expr.trim().equals("")) {
      JobAd jobAd = new JobAd();
      try {
        jobAd.setAttributeExpr(Jdl.REQUIREMENTS, expr);
        guiConfVarRequirements = expr.trim();
      } catch (Exception e) {
        // throw exc
      }
    }
  }

  static String getGUIConfVarRequirements() {
    return guiConfVarRequirements;
  }

  static void setGUIConfVarRank(String expr) {
    if ((expr != null) && !expr.trim().equals("")) {
      JobAd jobAd = new JobAd();
      try {
        jobAd.setAttributeExpr(Jdl.RANK, expr);
        guiConfVarRank = expr.trim();
      } catch (Exception e) {
        // throw exc
      }
    }
  }

  static String getGUIConfVarRank() {
    return guiConfVarRank;
  }

  static void setGUIConfVarRankMPI(String expr) {
    if ((expr != null) && !expr.trim().equals("")) {
      JobAd jobAd = new JobAd();
      try {
        jobAd.setAttributeExpr(Jdl.RANK_MPI, expr);
        guiConfVarRankMPI = expr.trim();
      } catch (Exception e) {
        // throw exc
      }
    }
  }

  static String getGUIConfVarRankMPI() {
    return guiConfVarRankMPI;
  }

  static void setGUIConfVarRetryCount(int value) {
    if (value == NO_RETRY_COUNT) {
      guiConfVarRetryCount = NO_RETRY_COUNT;
    }
    if ((Utils.RETRYCOUNT_MIN_VAL <= value)
        && (value <= Utils.RETRYCOUNT_MAX_VAL)) {
      guiConfVarRetryCount = value;
    }
  }

  static int getGUIConfVarRetryCount() {
    return guiConfVarRetryCount;
  }

  static void setGUIConfVarVersion(String version) {
    if ((version != null) && !version.trim().equals("")) {
      guiConfVarVersion = version.trim();
    }
  }

  static String getGUIConfVarVersion() {
    return guiConfVarVersion;
  }

  static void setGUIConfVarLoggingTimeout(int value) {
    if ((0 <= value) && (value <= Integer.MAX_VALUE)) {
      guiConfVarLoggingTimeout = value;
    }
  }

  static int getGUIConfVarLoggingTimeout() {
    return guiConfVarLoggingTimeout;
  }

  static void setGUIConfVarLoggingSyncTimeout(int value) {
    if ((0 <= value) && (value <= Integer.MAX_VALUE)) {
      guiConfVarLoggingSyncTimeout = value;
    }
  }

  static int getGUIConfVarLoggingSyncTimeout() {
    return guiConfVarLoggingSyncTimeout;
  }

  static void setGUIConfVarLoggingDestination(String loggingDestination) {
    if (loggingDestination != null) {
      loggingDestination = loggingDestination.trim();
      if (!loggingDestination.equals("")) {
        guiConfVarLoggingDestination = loggingDestination;
        Api.setEnv(Utils.EDG_WL_LOG_DESTINATION, guiConfVarLoggingDestination);
      } else {
        Api.unsetEnv(Utils.EDG_WL_LOG_DESTINATION);
      }
    }
  }

  static String getGUIConfVarLoggingDestination() {
    return guiConfVarLoggingDestination;
  }

  static int getGUIConfVarNSLoggerLevel() {
    return nsLoggerLevel;
  }

  static void setGUIConfVarNSLoggerLevel(int value) {
    if ((Utils.NS_LOGGER_LEVEL_MIN_VAL <= value)
        && (value <= Utils.NS_LOGGER_LEVEL_MAX_VAL)) {
      nsLoggerLevel = value;
    } else {
      nsLoggerLevel = Utils.NS_LOGGER_LEVEL_DEF_VAL;
    }
  }

  static void setGUIConfVarErrorStorage(String errorStorage) {
    if ((errorStorage != null) && !errorStorage.trim().equals("")) {
      File errorStorageFile = new File(errorStorage);
      if (errorStorageFile.isDirectory()) {
        guiConfVarErrorStorage = errorStorageFile.toString() + File.separator;
      }
    }
  }

  static String getGUIConfVarErrorStorage() {
    return guiConfVarErrorStorage;
  }
}