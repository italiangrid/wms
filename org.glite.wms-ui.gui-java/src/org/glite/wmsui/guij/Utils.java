/*
 * Utils.java 
 * 
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 * 
 */

package org.glite.wmsui.guij;

import java.io.BufferedReader;
import java.io.EOFException;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Calendar;
import java.util.Date;
import java.util.Enumeration;
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Vector;
import javax.naming.directory.InvalidAttributeValueException;
import javax.swing.JComponent;
import javax.swing.JOptionPane;
import javax.swing.JTextField;
import org.glite.jdl.Jdl;
import org.glite.jdl.JobAd;
import org.glite.jdl.JobAdException;
import org.glite.wmsui.apij.Api;

/**
 * Implementation of the Utils class. This class provides constant values and
 * utility methods.
 * 
 * @ingroup gui
 * @brief This class provides some constant values and utility methods.
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class Utils {
  //static Logger logger =
  // Logger.getLogger(CredentialInformation.class.getName());
  //static final String LOG4J_CONF_FILE_NAME =
  // "glite_wmsui_gui_log4j.properties";
  // Logging level to use when application is unable to find Log4J configuration
  // file.
  //static final Level LOG4J_BASIC_CONF_LEVEL = Level.DEBUG;
  // Set this constant to true, to debug all classes of the package.
  // (if true, you will print the stack trace of all cought exceptions)
  static final boolean GLOBAL_DEBUG = false;

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || GLOBAL_DEBUG;

  static final String COPYRIGHT = "Copyright (c) Members of the EGEE Collaboration. 2004.\n"
      + "See http://public.eu-egee.org/partners/ for details on the copyright holders.\n"
      + "For license conditions see the license file or http://www.eu-egee.org/license.html\n"
      + "\nDeveloped by Datamat S.p.A.";

  // Tabbed pane panel names. Do not change order, add at the end!!
  static final String[] GUI_PANEL_NAMES = { "Type", "Files", "Environment",
      "Input Data", "Output Data", "Requirements", "Rank", "Unknown", "Tags",
      "Partitionable"
  };

  // Used to create UserContact list
  // NOT NEEDED ANYMORE static final char EMAIL_SEPARATOR = ',';
  static final String JOBTYPE_LIST_SEPARATOR = "/";

  static final int ROW_BEFORE_NO_JDL_FILE_ERROR = Integer.MAX_VALUE;

  // Job types array.
  static final String[] JOB_TYPES = {
      Jdl.JOBTYPE_NORMAL,
      Jdl.JOBTYPE_INTERACTIVE, 
      Jdl.JOBTYPE_CHECKPOINTABLE,
      Jdl.JOBTYPE_PARTITIONABLE,
      Jdl.JOBTYPE_MPICH,
      Jdl.JOBTYPE_CHECKPOINTABLE + JOBTYPE_LIST_SEPARATOR
          + Jdl.JOBTYPE_INTERACTIVE,
      Jdl.JOBTYPE_CHECKPOINTABLE + JOBTYPE_LIST_SEPARATOR + Jdl.JOBTYPE_MPICH
  };

  // Attribute different from these ones is inserted inside Unknown panel.
  // (these are the attributes GUI can 'handle').
  static final String[] guiAttributesArray = { Jdl.TYPE, Jdl.JOBTYPE,
      Jdl.DEFAULT_RANK, Jdl.CEID, Jdl.STR_USER_PROXY, // API attributes
                                                      // 'automatically'
                                                      // inserted in a jdl file.
      //!!! STR_USER_PROXY will be a user attribute.
      Jdl.PRE_JOB, Jdl.POST_JOB,
      Jdl.NODENUMB, Jdl.CHKPT_STEPS, Jdl.CHKPT_CURRENTSTEP, Jdl.SHPORT,
      Jdl.VIRTUAL_ORGANISATION, Jdl.EXECUTABLE, Jdl.ARGUMENTS, Jdl.STDINPUT,
      Jdl.STDOUTPUT, Jdl.STDERROR, Jdl.INPUTSB, Jdl.OUTPUTSB, Jdl.ENVIRONMENT,
      Jdl.MYPROXY, /* Jdl.USER_CONTACT, */Jdl.HLR_LOCATION, Jdl.RETRYCOUNT,
      Jdl.INPUTDATA, /* Jdl.REPLICA_CATALOG, */Jdl.DATA_ACCESS, Jdl.OUTPUTDATA,
      Jdl.OUTPUT_SE, Jdl.REQUIREMENTS, Jdl.RANK, Jdl.FUZZY_RANK, Jdl.RANK_MPI
  };

  static final String[] jobTypeAttributeArray = { Jdl.TYPE, Jdl.JOBTYPE,
      Jdl.NODENUMB, Jdl.CHKPT_STEPS, Jdl.CHKPT_CURRENTSTEP, Jdl.SHPORT
  };
  
  static final String[] partitionableAttributeArray = { Jdl.PRE_JOB,
      Jdl.POST_JOB
  };
  
  static final String[] jobDefinition1AttributeArray = {
      Jdl.VIRTUAL_ORGANISATION, Jdl.EXECUTABLE, Jdl.ARGUMENTS, Jdl.STDINPUT,
      Jdl.STDOUTPUT, Jdl.STDERROR, Jdl.INPUTSB, Jdl.OUTPUTSB,
  };

  static final String[] jobDefinition2AttributeArray = { Jdl.ENVIRONMENT,
      Jdl.MYPROXY, /* Jdl.USER_CONTACT, */Jdl.HLR_LOCATION, Jdl.RETRYCOUNT
  };

  static final String[] jobInputDataAttributeArray = { Jdl.INPUTDATA, /* Jdl.REPLICA_CATALOG, */
      Jdl.DATA_ACCESS
  };

  static final String[] requirementsAttributeArray = { Jdl.REQUIREMENTS
  };

  static final String[] rankAttributeArray = { Jdl.RANK, Jdl.FUZZY_RANK
  };

  static final String[] jobOutputDataAttributeArray = { Jdl.OUTPUTDATA,
      Jdl.OUTPUT_SE
  };

  static final String[] tagsAttributeArray = { Jdl.USER_TAGS
  };

  static final int VALUE = -4;

  static final int LIST = -3;

  static final int INFINITE = -2;

  static final int UNKNOWN = -1;

  static final int INTEGER = 0;

  static final int FLOAT = 1;

  static final int BOOLEAN = 2;

  static final int STRING = 3;

  static final int EXPR = 4;

  static final int ABS_TIME = 5;

  static final int REL_TIME = 6;

  static final int MEMORY = 7;

  static final int SECONDS = 8;

  static final String[] TYPE_NAMES = { "Integer", "Float", "Boolean", "String",
      "Expr", "AbsTime", "RelTime", "Memory", "Seconds"
  };

  // Job Id attribute added to the jdl file when the corresponding job is
  // submitted.
  static final String GUI_JOB_ID_ATTR_NAME = "GUIJobId";

  // Submission Time attribute added to the jdl file when the corresponding
  // job is submitted.
  static final String GUI_SUBMISSION_TIME_ATTR_NAME = "GUISubmissionTime";

  // Checkpoint State File attribute added to the jdl file when user select
  // a chekpointstate for a checkpointable job. Note: if user select a CE Id
  // for a job the attribute name will be Jdl.CEID.
  static final String GUI_CHECKPOINT_STATE_FILE = "GUICheckpointStateFile";

  static final String[] guiTemporarlyAddedAttributeArray = {
      GUI_JOB_ID_ATTR_NAME, GUI_SUBMISSION_TIME_ATTR_NAME,
      GUI_CHECKPOINT_STATE_FILE
  };

  // Operative Systems.
  static final int UNKNOWN_SO = -1;

  static final int UNIX_LIKE = 0;

  static final int WINDOWS = 1;

  static final int NO_SORTING = -1;

  // Return values.
  static final int FAILED = -1;

  static final int SUCCESS = 0;

  // Application type.
  static final int UNKNOWN_APPLICATION = -1;

  static final int FRAME = 0;

  static final int APPLET = 1;

  static final int INTERNAL_FRAME = 2;

  // Job Monitor update modes.
  static final int LB_MODE = 0;

  static final int RGMA_MODE = 1;

  // R-GMA mode update rate.
  static final int RGMA_UPDATE_RATE = 8000;

  static final int GUI_THREAD_INITIAL_PORT = 5000;

  static final int GUI_THREAD_RETRY_COUNT = 250; // Port from 5000 to 5250.

  //*** Information service schemas.
  // Default schema file.
  static final String CONF_FILE_NAME = "glite_wmsui_jdle_GLUE.xml";

  static final String GLUE_CONF_FILE_NAME = "glite_wmsui_jdle_GLUE.xml";

  static final String EDG_CONF_FILE_NAME = "glite_wmsui_jdle_EDG.xml";

  static final String[] jdleSchemaArray = { "Glue", "EDG"
  };

  static final String[][] jdleSchemaNameConfFile = {
      { "GLUE", GLUE_CONF_FILE_NAME
      }, { "EDG", EDG_CONF_FILE_NAME
      }
  };

  static final String DEFAULT_INFORMATION_SCHEMA = "Glue";

  // END
  static final String DEFAULT_NS_NAME = "NS"; // NS1, NS2, ...

  // Job Submitter Strings.
  static final String NOT_SUBMITTED_TEXT = "NOT SUBMITTED";

  static final String SUBMITTED_TEXT = "SUBMITTED";

  //static final String SUBMITTING_TEXT = "Submitting...";
  static final String SUBMITTING_TEXT = "<html><b>" + "- Submitting... -"
      + "</b>";

  //static final String LISTMATCHING_TEXT = "Searching CE...";
  static final String LISTMATCHING_TEXT = "<html><b>" + "- Searching CE... -"
      + "</b>";

  // Job Monitor Strings
  static final String UNABLE_TO_GET_STATUS = "- Status Error -";

  static final String COLLECTING_STATE = "- Collecting... -";

  static final String STATE_FAILED = " (Failed)";

  static final String STATE_CANCELLED = " (Cancelled)";

  static final String STATE_CANCELLING = " (Cancel Req.)";

  static final String STATE_EXIT_CODE_NOT_ZERO = " (ExitCode != 0)";

  static final String UNAVAILABLE_SUBMISSION_TIME = "<html><b>"
      + "- Unavailable -" + "</b>";

  static final String LB_DEFAULT_PORT = "";

  static final String NS_DEFAULT_PORT = "";

  // Error and Warning messages.
  static final int MESSAGE_LINES_PER_JOPTIONPANE = 10;

  static final int WARNING_MSG = JOptionPane.WARNING_MESSAGE;

  static final int ERROR_MSG = JOptionPane.ERROR_MESSAGE;

  static final String WARNING_MSG_TXT = "Warning Message";

  static final String ERROR_MSG_TXT = "Error Message";

  static final String INFORMATION_MSG_TXT = "Information Message";

  static final String SELECT_AN_ITEM = "Please first select a list item";

  static final String FATAL_ERROR = "Fatal Error: ";

  static final String UNESPECTED_ERROR = "Unexpected Error: ";

  static final String WRONG_COMPONENT_ARGUMENT_TYPE = "Wrong component argument type";

  static final String GUI_SOCKET_HANDSHAKE_MSG = "GuiSocketHandshakeMessage";

  static final String USER_PREFERENCES = "User";

  static final String DEFAULT_PREFERENCES = "Default";

  static final String[] preferencesTypes = { "User", "Default"
  };

  // Min, Default and Max value of numeric attributes (used for initial
  // settings and to check values inserted from user).
  static final int NODENUMBER_MIN_VAL = 2;

  static final int NODENUMBER_DEF_VAL = 2;

  static final int NODENUMBER_MAX_VAL = Integer.MAX_VALUE;

  static final int RETRYCOUNT_MIN_VAL = 0;

  static final int RETRYCOUNT_DEF_VAL = 0;

  static final int RETRYCOUNT_MAX_VAL = Integer.MAX_VALUE;

  static final int SHADOWPORT_MIN_VAL = 1024;

  static final int SHADOWPORT_DEF_VAL = 6000;

  static final int SHADOWPORT_MAX_VAL = Integer.MAX_VALUE; //!!! Check this
                                                           // value.

  static final int JOBSTEPS_MIN_VAL = 2;

  static final int JOBSTEPS_DEF_VAL = 2;

  static final int JOBSTEPS_MAX_VAL = Integer.MAX_VALUE;

  static final int CURRENTSTEP_MIN_VAL = 1; // If JobSteps is an integer value.

  static final int CURRENTSTEP_DEF_VAL = 1;

  // CURRENTSTEP_MAX_VAL not present. Max value is equal to JobSteps user
  // selected value.
  static final int SPEC_INT_2000_MIN_VAL = 0;

  static final int SPEC_INT_2000_DEF_VAL = 0;

  static final int SPEC_INT_2000_MAX_VAL = Integer.MAX_VALUE;

  static final float SPEC_FLOAT_2000_MIN_VAL = 0.0f;

  static final float SPEC_FLOAT_2000_DEF_VAL = 0.0f;

  static final float SPEC_FLOAT_2000_MAX_VAL = Float.MAX_VALUE;

  static final int MIN_NUMBER_OF_FREE_CPUS_MIN_VAL = 0;

  static final int MIN_NUMBER_OF_FREE_CPUS_DEF_VAL = 0;

  static final int MIN_NUMBER_OF_FREE_CPUS_MAX_VAL = Integer.MAX_VALUE;

  static final float MIN_MAIN_MEMORY_MIN_VAL = 0.0f;

  static final float MIN_MAIN_MEMORY_DEF_VAL = 0.0f;

  static final float MIN_MAIN_MEMORY_MAX_VAL = Float.MAX_VALUE;

  static final float MAX_AVAILABLE_CPU_TIME_MIN_VAL = 0.0f;

  static final float MAX_AVAILABLE_CPU_TIME_DEF_VAL = 0.0f;

  static final float MAX_AVAILABLE_CPU_TIME_MAX_VAL = Float.MAX_VALUE;

  static final float TIME_TO_TRAVERSE_QUEUE_MIN_VAL = 0.2f;

  static final float TIME_TO_TRAVERSE_QUEUE_DEF_VAL = 0.2f;

  static final float TIME_TO_TRAVERSE_QUEUE_MAX_VAL = Float.MAX_VALUE;

  static final int PROXY_LIFETIME_MIN_VAL = 1;

  static final int PROXY_LIFETIME_DEF_VAL = 12;

  static final int PROXY_LIFETIME_MAX_VAL = Integer.MAX_VALUE;

  static final int UPDATE_RATE_DEF_VAL = 5;

  static final int UPDATE_RATE_MIN_VAL = 1;

  static final int UPDATE_RATE_MAX_VAL = Integer.MAX_VALUE;

  static final float MIN_SPACE_ON_SE_DEF_VAL = 0.0f;

  static final float MIN_SPACE_ON_SE_MIN_VAL = 0.0f;

  static final float MIN_SPACE_ON_SE_MAX_VAL = Float.MAX_VALUE;

  // User preferences file LB logging attribute
  static final int LOGGING_TIMEOUT_DEF_VAL = 30;

  static final int LOGGING_TIMEOUT_MIN_VAL = 1;

  static final int LOGGING_TIMEOUT_MAX_VAL = Integer.MAX_VALUE;

  static final int LOGGING_SYNC_TIMEOUT_DEF_VAL = 30;

  static final int LOGGING_SYNC_TIMEOUT_MIN_VAL = 1;

  static final int LOGGING_SYNC_TIMEOUT_MAX_VAL = Integer.MAX_VALUE;

  // Retrieve checkpoint state
  static final int CHECKPOINT_STATE_DEF_VAL = 0;

  static final int CHECKPOINT_STATE_MIN_VAL = 0;

  static final int CHECKPOINT_STATE_MAX_VAL = Integer.MAX_VALUE;

  // NS logger level
  static final int NS_LOGGER_LEVEL_DEF_VAL = 0;

  static final int NS_LOGGER_LEVEL_MIN_VAL = 0;

  static final int NS_LOGGER_LEVEL_MAX_VAL = 6;

  // MultipleJobPanel max number of simultaneously monitored job number.
  static final int MAX_MONITORED_JOB_NUMBER_DEF_VAL = 50;

  static final int MAX_MONITORED_JOB_NUMBER_MIN_VAL = 1;

  static final int MAX_MONITORED_JOB_NUMBER_MAX_VAL = 200;

  // Icon files.
  static final String ICON_FILE_OPEN = "images/file_open.gif";

  static final String ICON_FILE_SAVE = "images/file_save.gif";

  static final String ICON_FILE_NEW = "images/file_new.gif";

  static final String ICON_CUT = "images/cut.gif";

  static final String ICON_COPY = "images/copy.gif";

  static final String ICON_PASTE = "images/paste.gif";

  static final String ICON_DATAGRID_LOGO = "images/glite_small.jpg";

  // Names of VO specific configuration attributes used in VO_CONF_FILE_NAME.
  static final String CONF_FILE_VIRTUAL_ORGANISATION = "VirtualOrganisation";

  static final String CONF_FILE_HLRLOCATION = "HLRLocation";

  static final String CONF_FILE_NSADDRESSES = "NSAddresses";

  static final String CONF_FILE_LBADDRESSES = "LBAddresses";

  static final String CONF_FILE_MYPROXYSERVER = "MyProxyServer";

  static final String PREF_FILE_JOB_MONITOR = "GUIJobMonitor";

  static final String PREF_FILE_JOB_SUBMITTER = "GUIJobSubmitter";

  // Logging & Bookeeping panel.
  static final String PREF_FILE_LB_ADDRESS = "GUILBAddress";

  //static final String PREF_FILE_LB_PORT = "GUILBPort";
  static final String PREF_FILE_UPDATE_RATE = "GUIUpdateRate";

  static final String PREF_FILE_UPDATE_MODE = "GUIUpdateMode";

  // Network Server panel.
  static final String PREF_FILE_NS_NAME = "GUINSName";

  static final String PREF_FILE_NS_ADDRESS = "GUINSAddress";

  static final String PREF_FILE_JDLE_SCHEMA = "GUIJDLESchema";

  // Names of configuration attributes used in GUI_CONF_VAR_FILE_NAME.
  //static final String GUI_CONF_VAR_REQUIREMENTS = "Requirements";
  //static final String GUI_CONF_VAR_RANK = "Rank";
  static final String GUI_CONF_VAR_RANKMPI = "rankMPI";

  static final String GUI_CONF_VAR_RETRY_COUNT = "RetryCount";

  static final String GUI_CONF_VAR_VERSION = "Version";

  static final String GUI_CONF_VAR_VO = "VirtualOrganisation";

  static final String GUI_CONF_VAR_ERROR_STORAGE = "ErrorStorage";

  static final String GUI_CONF_VAR_LOGGING_TIMEOUT = "LoggingTimeout";

  static final String GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT = "LoggingSyncTimeout";

  static final String GUI_CONF_VAR_LOGGING_DESTINATION = "LoggingDestination";

  static final String GUI_CONF_VAR_JDLE_DEFAULT_SCHEMA = "JDLEDefaultSchema";

  static final String GUI_CONF_VAR_MAX_MONITORED_JOB_NUMBER = "MaxMonitoredJobNumber";

  static final String GUI_CONF_VAR_NS_LOGGER_LEVEL = "NSLoggerLevel";

  // Names of attributes used in OutputData attribute (OutputData is of Ad
  // type).
  static final String OUTPUT_FILE = "OutputFile";

  static final String STORAGE_ELEMENT = "StorageElement";

  static final String LOGICAL_FILE_NAME = "LogicalFileName";

  static final String EDG_WL_LOG_TIMEOUT = "EDG_WL_LOG_TIMEOUT";

  static final String EDG_WL_LOG_SYNC_TIMEOUT = "EDG_WL_LOG_SYNC_TIMEOUT";

  static final String EDG_WL_LOG_DESTINATION = "EDG_WL_LOG_DESTINATION";

  static final String EDG_WL_GUI_CONFIG_VAR = "GLITE_WMSUI_GUI_CONFIG_VAR";

  static final String EDG_WL_GUI_CONFIG_VO = "GLITE_WMSUI_GUI_CONFIG_VO";

  //!!! DAG
  // Names of query Ad in user preferences file.
  static final String USER_QUERIES = "GUIUserQueries";

  static final String QUERY_NAME = "QueryName";

  static final String ALL_LB_FLAG = "AllLBFlag";

  static final String LB_ADDRESS = "LBAddress";

  static final String OWNED_JOBS_ONLY_FLAG = "OwnedJobsOnlyFlag";

  static final String USER_TAGS = "UserTags";

  static final String FROM_DATE = "FromDate";

  static final String TO_DATE = "ToDate";

  static final String INCLUDE_STATE_ARRAY = "IncludeStateArray";

  // NOT CONSTANTS
  static int applicationType = UNKNOWN_APPLICATION;

  // Initial configuration file name, used by JDLEditor.
  static String confFileName = "glite_wmsui_jdle_GLUE.xml";

  // Set from Job Submitter in order to use user specified schema when you
  // open a JDL Editor.
  static String confFileSchema = "GLUE";

  /*****************************************************************************
   * METHODS
   ****************************************************************************/
  static void setApplicationType(int type) {
    applicationType = type;
  }

  /**
   * Substitutes single occurence of backslash with a double one. Added because
   * classad parse method interprets slashes as a special formatting chars. (if
   * it finds '\', it takes the next char and try to format: e.g. if '\' is
   * followed by 'n' you have \n -> "new line")
   */
  static String addBackSlashes(String inputExpClassad) {
    int index = inputExpClassad.indexOf("\\");
    String toReplace = "\\\\";
    while (index != -1) {
      inputExpClassad = inputExpClassad.substring(0, index) + toReplace
          + inputExpClassad.substring(index + 1);
      index = inputExpClassad.indexOf("\\", index + 2);
    }
    return inputExpClassad;
  }

  static boolean verify(JComponent jComponent, int type) {
    float minValue = Float.MIN_VALUE;
    double maxValue = Double.MAX_VALUE;
    boolean result = verify(jComponent, type, minValue, maxValue);
    return result;
  }

  static boolean verify(JComponent jComponent, int type, float minValue) {
    double maxValue = Double.MAX_VALUE;
    boolean result = verify(jComponent, type, minValue, maxValue);
    return result;
  }

  static boolean verify(JComponent jComponent, int type, float minValue,
      double maxValue) {
    JTextField jTextField = null;
    if (jComponent instanceof JTextField) {
      jTextField = (JTextField) jComponent;
    } else {
      System.exit(-1);
    }
    boolean result = true;
    String value = jTextField.getText().trim();
    switch (type) {
      case INTEGER:
        if (value.equals("")) {
          result = false;
        } else if (value.substring(0, 1).equals("-")) {
          result = false;
        } else {
          try {
            int parsedValue = Integer.parseInt(value, 10);
            if ((parsedValue < minValue) || (parsedValue > maxValue)) {
              result = false;
            }
          } catch (NumberFormatException e) {
            result = false;
            jTextField.selectAll();
          }
        }
      break;
      case FLOAT:
        if (value.equals("")) {
          result = false;
        } else if (value.substring(0, 1).equals("-")) {
          result = false;
        } else {
          try {
            float parsedValue = Float.parseFloat(value);
            if ((parsedValue < minValue) || (parsedValue > maxValue)) {
              result = false;
            }
          } catch (NumberFormatException e) {
            result = false;
            jTextField.selectAll();
          }
        }
      break;
    }
    return result;
  }

  static void upButtonEvent(JComponent jComponent, int type,
      String defaultValue, float minValue, double maxValue) {
    JTextField jTextField = null;
    if (jComponent instanceof JTextField) {
      jTextField = (JTextField) jComponent;
    } else {
      System.exit(-1);
    }
    if (maxValue == Utils.INFINITE) {
      maxValue = Double.MAX_VALUE;
    }
    jTextField.grabFocus();
    boolean result = verify(jTextField, type, minValue, maxValue);
    switch (type) {
      case Utils.INTEGER:
        if (result) {
          String value = jTextField.getText().trim();
          Integer intValue = null;
          try {
            int newValue = Integer.parseInt(value, 10);
            if (newValue < maxValue) {
              newValue++;
              intValue = new Integer(newValue);
              jTextField.setText(intValue.toString());
            }
          } catch (NumberFormatException ex) {
            // Do nothing.
          }
        } else {
          jTextField.setText(defaultValue);
          jTextField.selectAll();
        }
      break;
      case Utils.FLOAT:
        if (result) {
          String value = jTextField.getText().trim();
          Float floatValue = null;
          try {
            float newValue = Float.parseFloat(value);
            newValue = newValue * 10;
            newValue += 2;
            newValue = newValue / 10;
            if (newValue > maxValue) {
              newValue = newValue * 10;
              newValue -= 2;
              newValue = newValue / 10;
            }
            floatValue = new Float(newValue);
            jTextField.setText(floatValue.toString());
          } catch (NumberFormatException ex) {
            // Do nothing.
          }
        } else {
          jTextField.setText(defaultValue);
          jTextField.selectAll();
        }
      break;
    }
  }

  static void downButtonEvent(JComponent jComponent, int type,
      String defaultValue, float minValue, double maxValue) {
    JTextField jTextField = null;
    if (jComponent instanceof JTextField) {
      jTextField = (JTextField) jComponent;
    } else {
      System.exit(-1);
    }
    if (maxValue == Utils.INFINITE) {
      maxValue = Double.MAX_VALUE;
    }
    jTextField.grabFocus();
    boolean result = verify(jTextField, type, minValue, maxValue);
    switch (type) {
      case Utils.INTEGER:
        if (result) {
          String value = jTextField.getText().trim();
          Integer intValue = null;
          try {
            int newValue = Integer.parseInt(value, 10);
            if (newValue > minValue) {
              newValue--;
            }
            intValue = new Integer(newValue);
            jTextField.setText(intValue.toString());
          } catch (NumberFormatException ex) {
            // Do nothing.
          }
        } else {
          jTextField.setText(defaultValue);
          jTextField.selectAll();
        }
      break;
      case Utils.FLOAT:
        if (result) {
          String value = jTextField.getText().trim();
          Float floatValue = null;
          try {
            float newValue = Float.parseFloat(value);
            newValue *= 10;
            newValue -= 2;
            newValue /= 10;
            if (newValue < minValue) {
              newValue *= 10;
              newValue += 2;
              newValue /= 10;
            }
            floatValue = new Float(newValue);
            jTextField.setText(floatValue.toString());
          } catch (NumberFormatException ex) {
            // Do nothing.
          }
        } else {
          jTextField.setText(defaultValue);
          jTextField.selectAll();
        }
      break;
    }
  }

  static String checkAttributeAdd(String attribute, String value) {
    JobAd jobAdCheck = new JobAd();
    String errorMsg = "";
    try {
      jobAdCheck.addAttribute(attribute, value);
    } catch (IllegalArgumentException iae) {
      return errorMsg += "- " + iae.getMessage() + "\n";
    } catch (InvalidAttributeValueException iave) {
      return errorMsg += "- " + iave.getMessage() + "\n";
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
    }
    return errorMsg;
  }

  public static int getOperatingSystem() {
    String operatingSystem = System.getProperty("os.name");
    if (operatingSystem.equals("Linux") || operatingSystem.equals("Unix") // Maybe
                                                                          // "Unix"
                                                                          // is
                                                                          // not
                                                                          // necessary.
        || operatingSystem.equals("Solaris") || operatingSystem.equals("SunOS")
        || operatingSystem.equals("Digital Unix")) {
      return UNIX_LIKE;
    } else if (operatingSystem.equals("Windows")
        || operatingSystem.equals("Windows 2000")
        || operatingSystem.equals("Windows Me")
        || operatingSystem.equals("Windows 98")
        || operatingSystem.equals("Windows NT")
        || operatingSystem.equals("Windows 95")) {
      return WINDOWS;
    }
    return UNKNOWN_SO;
  }

  public static int getValueType(String value) {
    value = value.trim();
    if (isInteger(value)) {
      return Utils.INTEGER;
    } else if (isFloat(value)) {
      return Utils.FLOAT;
    } else if (isBoolean(value)) {
      return Utils.BOOLEAN;
    } else {
      return Utils.STRING;
    }
  }

  static boolean isFloat(String value) {
    try {
      Float.parseFloat(value);
    } catch (NumberFormatException e) {
      return false;
    }
    return true;
  }

  static boolean isInteger(String value) {
    try {
      Integer.parseInt(value, 10);
    } catch (NumberFormatException e) {
      return false;
    }
    return true;
  }

  static boolean isBoolean(String value) {
    value = value.trim().toUpperCase();
    if (value.equals("TRUE") || value.equals("FALSE")) {
      return true;
    }
    return false;
  }

  /**
   * Swaps parameters order of a two parameters function
   */
  static String swapFunctionParameters(String function) {
    function = function.trim();
    String functionName = function.substring(0, function.indexOf("(")).trim();
    String firstParameter = function.substring(function.indexOf("(") + 1,
        function.indexOf(",")).trim();
    String secondParameter = function.substring(function.indexOf(",") + 1,
        function.indexOf(")")).trim();
    return functionName + "(" + secondParameter + ", " + firstParameter + ")";
  }

  /**
   * Converts a String in the format: "[0123456789].[0123456789] s" into a human
   * readable format
   */
  static String toTime(String eventTime) {
    int end = eventTime.indexOf(" ");
    if (end == -1) {
      return "";
    }
    eventTime = eventTime.substring(0, end);
    double time;
    try {
      // Time returned from LB is divided by 1000.
      time = java.lang.Double.parseDouble(eventTime) * 1000;
    } catch (java.lang.NumberFormatException exc) {
      return "";
    }
    Calendar cl = new GregorianCalendar();
    cl.setTimeInMillis((long) time);
    return cl.getTime().toString();
  }

  static Date toDate(String eventTime) {
    int end = eventTime.indexOf(" ");
    if (end == -1) {
      // Unable to parse the time-string
      return null;
    }
    eventTime = eventTime.substring(0, end);
    double time;
    try {
      // Time returned from LB is divided by 1000.
      time = java.lang.Double.parseDouble(eventTime) * 1000;
    } catch (java.lang.NumberFormatException exc) {
      return null;
    }
    return new Date((long) time);
  }

  public static String secondsToTime(int seconds) {
    int hours = seconds / 3600;
    seconds = seconds % 3600;
    int minutes = seconds / 60;
    seconds = seconds % 60;
    String hoursTxt = Integer.toString(hours);
    hoursTxt = (hoursTxt.length() == 1) ? "0" + hoursTxt : hoursTxt;
    String minutesTxt = Integer.toString(minutes);
    minutesTxt = (minutesTxt.length() == 1) ? "0" + minutesTxt : minutesTxt;
    String secondsTxt = Integer.toString(seconds);
    secondsTxt = (secondsTxt.length() == 1) ? "0" + secondsTxt : secondsTxt;
    return hoursTxt + ":" + minutesTxt + ":" + secondsTxt;
  }

  static boolean isInVectorCi(String element, Vector vector) {
    element = element.toUpperCase();
    String vectorElement = "";
    Enumeration enumeration = vector.elements();
    for (; enumeration.hasMoreElements();) {
      vectorElement = enumeration.nextElement().toString().toUpperCase();
      if (vectorElement.equals(element)) {
        return true;
      }
    }
    return false;
  }

  /**
   * Returns the input <cose>path</code> after resolving all the environment
   * variables.
   * 
   * @param path
   *          the input path to resolve
   * @return the resolved path
   */
  public static String solveEnviron(String path) throws JobAdException {
    int lastIndex = 0;
    int sepIndex = -1;
    int varNameLength = 0;
    int varValueLength = 0;
    String varName = "";
    String varValue = "";
    int newIndex = path.indexOf("$");
    while (newIndex != -1) {
      sepIndex = path.indexOf("/", newIndex);
      if (sepIndex == -1) {
        sepIndex = path.length();
      }
      varName = path.substring(newIndex + 1, sepIndex);
      varNameLength = varName.length();
      varValue = Api.getEnv(varName);
      if (varValue != null) {
        varValueLength = varValue.length();
        path = path.substring(0, newIndex) + varValue
            + path.substring(newIndex + 1 + varNameLength);
        lastIndex = newIndex + varValueLength;
        newIndex = path.indexOf("$", lastIndex);
      } else {
        throw new JobAdException("Environment variable '" + varName
            + "' has not been set");
      }
    }
    return path;
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

  static Map cloneMap(Map map) {
    Map returnMap = new HashMap();
    Iterator iterator = map.keySet().iterator();
    Object key;
    while (iterator.hasNext()) {
      key = iterator.next();
      returnMap.put(key, map.get(key));
    }
    return returnMap;
  }

  static void setJDLESchemaFile(String schema) {
    schema = schema.trim().toUpperCase();
    for (int i = 0; i < jdleSchemaNameConfFile.length; i++) {
      if (schema.equals(jdleSchemaNameConfFile[i][0])) {
        confFileSchema = schema.toUpperCase();
        confFileName = jdleSchemaNameConfFile[i][1];
      }
    }
  }
}