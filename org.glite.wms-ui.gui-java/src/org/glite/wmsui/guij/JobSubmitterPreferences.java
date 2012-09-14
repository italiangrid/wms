/*
 * JobSubmitterPreferences.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.awt.AWTEvent;
import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.WindowEvent;
import java.io.File;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Ad;
import condor.classad.Constant;
import condor.classad.ListExpr;

/**
 * Implementation of the JobSubmitterPreferences class.
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class JobSubmitterPreferences extends JDialog {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final String NS_PANEL_NAME = "Network Server";

  static final String LB_PANEL_NAME = "Logging & Bookkeeping";

  static final String JDL_DEFAULTS_PANEL_NAME = "JDL Defaults";

  static final String LOGGING_PANEL_NAME = "Logging";

  static final int NS_PANEL_INDEX = 0;

  static final int LB_PANEL_INDEX = 1;

  static final int MISCELLANEOUS_PANEL_INDEX = 2;

  static final int LOGGING_PANEL_INDEX = 3;

  JPanel jPanelButton = new JPanel();

  JButton jButtonCancel = new JButton();

  JButton jButtonApply = new JButton();

  JButton jButtonOk = new JButton();

  JButton jButtonLoadDefault = new JButton();

  JButton jButtonLoadUser = new JButton();

  JTabbedPane jTabbedPane = new JTabbedPane();

  JobSubmitter jobSubmitterJFrame;

  NSPreferencesPanel jobSubmitterPreferencesPanel;

  LBPreferencesPanel jobMonitorPreferencesPanel;

  JDLDefaultsPreferencesPanel jdlDefaultsPreferencesPanel;

  LoggingPreferencesPanel loggingPreferencesPanel;

  JLabel jLabelFill = new JLabel();

  //boolean reloadOldWorkingSession = false;
  boolean isUserPreferences = false;

  /**
   * Constructor.
   */
  public JobSubmitterPreferences(Component component) {
    super((JobSubmitter) component);
    jobSubmitterJFrame = (JobSubmitter) component;
    jobSubmitterPreferencesPanel = new NSPreferencesPanel(jobSubmitterJFrame);
    jobMonitorPreferencesPanel = new LBPreferencesPanel(jobSubmitterJFrame);
    jdlDefaultsPreferencesPanel = new JDLDefaultsPreferencesPanel(
        jobSubmitterJFrame);
    loggingPreferencesPanel = new LoggingPreferencesPanel(jobSubmitterJFrame);
    jobMonitorPreferencesPanel.setMiscellaneousVisible(false);
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit();
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
  }

  private void jbInit() throws Exception {
    isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG) ? true
        : false;
    // Initialize tabbed pane panels.
    Object[][] element = { { NS_PANEL_NAME, jobSubmitterPreferencesPanel
    }, { LB_PANEL_NAME, jobMonitorPreferencesPanel
    }, { JDL_DEFAULTS_PANEL_NAME, jdlDefaultsPreferencesPanel
    }, { LOGGING_PANEL_NAME, loggingPreferencesPanel
    }
    };
    for (int i = 0; i < element.length; i++) {
      jTabbedPane.addTab((String) element[i][0], (JPanel) element[i][1]);
    }
    jButtonCancel.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonCancelEvent(null);
      }
    });
    jButtonCancel.setText("Cancel");
    jButtonApply.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonApplyEvent(e);
      }
    });
    jButtonApply.setText("Apply ");
    jButtonOk.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonOkEvent(null);
      }
    });
    jButtonOk.setText("  Ok  ");
    jButtonLoadDefault.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonLoadDefaultEvent(null);
      }
    });
    jButtonLoadDefault.setText("Default");
    jButtonLoadUser.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonLoadUserEvent(null);
      }
    });
    jButtonLoadUser.setText("  User  ");
    jLabelFill.setPreferredSize(new Dimension(210, 20));
    jPanelButton.add(jButtonCancel, null);
    jPanelButton.add(jButtonLoadDefault, null);
    jPanelButton.add(jLabelFill, null);
    jPanelButton.add(jButtonOk, null);
    jPanelButton.add(jButtonApply, null);
    jPanelButton.setBorder(BorderFactory.createRaisedBevelBorder());
    jTabbedPane.addChangeListener(new ChangeListener() {
      public void stateChanged(ChangeEvent e) {
        if (jTabbedPane.getTitleAt(jTabbedPane.getSelectedIndex()).equals(
            LB_PANEL_NAME)) {
          int selectedRow = jobSubmitterPreferencesPanel.jTableNS
              .getSelectedRow();
          if (selectedRow != -1) {
            jobMonitorPreferencesPanel.jComboBoxNS
                .setSelectedItem(jobSubmitterPreferencesPanel.jTableNS
                    .getValueAt(selectedRow,
                        NSPreferencesPanel.NS_ADDRESS_COLUMN_INDEX).toString());
          }
        }
      }
    });
    this.getContentPane().setLayout(new BorderLayout());
    this.getContentPane().add(jTabbedPane, BorderLayout.CENTER);
    this.getContentPane().add(jPanelButton, BorderLayout.SOUTH);
    setSize(new Dimension(550, 410));
    setTitle("Job Submitter - Preferences");
    setResizable(false);
    jobSubmitterPreferencesPanel.jTextFieldNSName.grabFocus();
    jButtonLoadUserEvent(null);
  }

  void jButtonCancelEvent(ActionEvent e) {
    dispose();
  }

  protected void processWindowEvent(WindowEvent e) {
    super.processWindowEvent(e);
    this.setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
    if (e.getID() == WindowEvent.WINDOW_CLOSING) {
      if (jButtonCancel.isVisible()) {
        jButtonCancelEvent(null);
      } else {
        int choice = JOptionPane.showOptionDialog(JobSubmitterPreferences.this,
            "Do you really want to exit?", "Confirm Exit",
            JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null,
            null, null);
        if (choice == 0) {
          System.exit(0);
        }
      }
    }
  }

  void jButtonOkEvent(ActionEvent e) {
    int result = jButtonApplyEvent(e);
    if (result != Utils.FAILED) {
      dispose();
      /*
       if (reloadOldWorkingSession) {
       jobSubmitterJFrame.loadOldWorkingSession();
       }*/
    }
  }

  int jButtonApplyEvent(ActionEvent e) {
    int result = Utils.SUCCESS;
    int choice = 0;
    if (GUIGlobalVars.openedEditorHashMap.size() != 0) {
      choice = JOptionPane.showOptionDialog(JobSubmitterPreferences.this,
          "Apply preferences?\nAll opened editors will be closed",
          "Confirm Apply Preferences", JOptionPane.YES_NO_OPTION,
          JOptionPane.QUESTION_MESSAGE, null, null, null);
      if (choice != 0) {
        return Utils.FAILED;
      }
      GraphicUtils.closeAllEditorFrames();
    }
    if (jobSubmitterPreferencesPanel.jButtonApplyEvent(null) == Utils.FAILED) {
      result = Utils.FAILED;
    } else {
      jobSubmitterJFrame.loadOldWorkingSession();
    }
    if (jobMonitorPreferencesPanel.jButtonApplyEvent(null) == Utils.FAILED) {
      result = Utils.FAILED;
    }
    if (jdlDefaultsPreferencesPanel.jButtonApplyEvent(null) == Utils.FAILED) {
      result = Utils.FAILED;
    }
    if (loggingPreferencesPanel.jButtonApplyEvent(null) == Utils.FAILED) {
      result = Utils.FAILED;
    }
    return result;
  }

  static Map yieldNSLBMap(Vector nsVector, Vector lbVector) throws Exception {
    Map nsLBMap = new HashMap();
    if (nsVector.size() != 0) {
      if (lbVector.size() != 0) {
        if (lbVector.get(0) instanceof String) {
          for (int i = 1; i < lbVector.size(); i++) {
            if (!(lbVector.get(i) instanceof String)) {
              throw new Exception("Unable to parse configuration file, " + "'"
                  + Utils.CONF_FILE_LBADDRESSES + "' attribute");
            }
          }
        } else { // Vector.
          for (int i = 1; i < lbVector.size(); i++) {
            if (lbVector.get(i) instanceof String) {
              throw new Exception("Unable to parse configuration file, " + "'"
                  + Utils.CONF_FILE_LBADDRESSES + "' attribute");
            }
          }
        }
        if (lbVector.get(0) instanceof String) { // Attribute is a simple list.
          for (int i = 0; i < nsVector.size(); i++) {
            nsLBMap.put(nsVector.get(i).toString(), lbVector.clone());
          }
        } else { // Attribute is a list of lists.
          Vector vectorElement;
          int lbVectorSize = lbVector.size();
          String nsItem;
          for (int i = 0; i < nsVector.size(); i++) {
            nsItem = nsVector.get(i).toString().trim();
            if (!nsLBMap.containsKey(nsItem)) {
              if (i < lbVectorSize) {
                vectorElement = (Vector) lbVector.get(i);
              } else {
                vectorElement = new Vector();
              }
              nsLBMap.put(nsItem, vectorElement);
            }
          }
        }
      }
    }
    return nsLBMap;
  }

  static Map loadNSLBMap(File userConfFile) throws Exception {
    Ad userConfAd = new Ad();
    Ad userConfJobSubmitterAd = new Ad();
    Map nsLBMap = new HashMap();
    logger.info("loading NS-LB Map, file:" + userConfFile);
    if (userConfFile.isFile()) {
      Vector lbVector = new Vector();
      Vector nsVector = new Vector();
      try {
        userConfAd.fromFile(userConfFile.toString());
        if (userConfAd.hasAttribute(Utils.PREF_FILE_JOB_SUBMITTER)) {
          userConfJobSubmitterAd = userConfAd
              .getAd(Utils.PREF_FILE_JOB_SUBMITTER);
          if (userConfJobSubmitterAd.hasAttribute(Utils.PREF_FILE_NS_ADDRESS)) {
            nsVector = userConfJobSubmitterAd
                .getStringValue(Utils.PREF_FILE_NS_ADDRESS);
          }
          if (nsVector.size() != 0) {
            if (userConfJobSubmitterAd.hasAttribute(Utils.PREF_FILE_LB_ADDRESS)) {
              lbVector = userConfJobSubmitterAd
                  .getStringValue(Utils.PREF_FILE_LB_ADDRESS);
              if (lbVector.size() != 0) {
                if (lbVector.get(0) instanceof String) {
                  for (int i = 1; i < lbVector.size(); i++) {
                    if (!(lbVector.get(i) instanceof String)) {
                      throw new Exception("Unable to parse configuration file "
                          + userConfFile + "\n'" + Utils.CONF_FILE_LBADDRESSES
                          + "' attribute");
                    }
                  }
                } else { // Vector.
                  for (int i = 1; i < lbVector.size(); i++) {
                    if (lbVector.get(i) instanceof String) {
                      throw new Exception("Unable to parse configuration file "
                          + userConfFile + "\n'" + Utils.CONF_FILE_LBADDRESSES
                          + "' attribute");
                    }
                  }
                }
                if (lbVector.get(0) instanceof String) { // Attribute is a simple list.
                  for (int i = 0; i < nsVector.size(); i++) {
                    nsLBMap.put(nsVector.get(i).toString(), lbVector.clone());
                  }
                } else { // Attribute is a list of lists.
                  Vector vectorElement = new Vector();
                  int lbVectorSize = lbVector.size();
                  String nsItem;
                  for (int i = 0; i < nsVector.size(); i++) {
                    nsItem = nsVector.get(i).toString().trim();
                    if (!nsLBMap.containsKey(nsItem)) {
                      if (i < lbVectorSize) {
                        vectorElement = (Vector) lbVector.get(i);
                      } else {
                        vectorElement = new Vector();
                      }
                      nsLBMap.put(nsItem, vectorElement);
                    }
                  }
                }
              }
            }
          }
        }
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        throw ex;
      }
    }
    return nsLBMap;
  }

  static Vector loadPrefFileLB() throws Exception {
    Ad userConfAd = new Ad();
    Ad userConfJobSubmitterAd = new Ad();
    Vector lbVector = new Vector();
    File userConfFile = new File(GUIFileSystem.getUserPrefFile());
    logger.info("loadPrefFileLB() file:" + userConfFile);
    if (userConfFile.isFile()) {
      try {
        userConfAd.fromFile(userConfFile.toString());
        if (userConfAd.hasAttribute(Utils.PREF_FILE_JOB_SUBMITTER)) {
          userConfJobSubmitterAd = userConfAd
              .getAd(Utils.PREF_FILE_JOB_SUBMITTER);
          if (userConfJobSubmitterAd.hasAttribute(Utils.PREF_FILE_LB_ADDRESS)) {
            lbVector = userConfJobSubmitterAd
                .getStringValue(Utils.PREF_FILE_LB_ADDRESS);
          }
        }
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        throw ex;
      }
    }
    return lbVector;
  }

  static void saveNSLBMap(Map map) throws Exception {
    Ad userConfAd = new Ad();
    Ad userPrefJobSubmitterAd = new Ad();
    File userConfFile = new File(GUIFileSystem.getUserPrefFile());
    if (userConfFile.isFile()) {
      try {
        userConfAd.fromFile(userConfFile.toString());
        if (userConfAd.hasAttribute(Utils.PREF_FILE_JOB_SUBMITTER)) {
          userPrefJobSubmitterAd = userConfAd
              .getAd(Utils.PREF_FILE_JOB_SUBMITTER);
          userConfAd.delAttribute(Utils.PREF_FILE_JOB_SUBMITTER);
        }
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        throw ex;
      }
    }
    try {
      userPrefJobSubmitterAd = LBPreferencesPanel.setNSLBMapAttribute(
          userPrefJobSubmitterAd, map);
      userConfAd.setAttribute(Utils.PREF_FILE_JOB_SUBMITTER,
          userPrefJobSubmitterAd);
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
      throw ex;
    }
    try {
      GUIFileSystem.saveTextFile(userConfFile, userConfAd.toString());
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
      throw ex;
    }
  }

  static void savePrefFileLB(Vector lbVector) throws Exception {
    Ad userConfAd = new Ad();
    Ad userPrefJobSubmitterAd = new Ad();
    File userConfFile = new File(GUIFileSystem.getUserPrefFile());
    if (userConfFile.isFile()) {
      try {
        userConfAd.fromFile(userConfFile.toString());
        if (userConfAd.hasAttribute(Utils.PREF_FILE_JOB_SUBMITTER)) {
          userPrefJobSubmitterAd = userConfAd
              .getAd(Utils.PREF_FILE_JOB_SUBMITTER);
          userConfAd.delAttribute(Utils.PREF_FILE_JOB_SUBMITTER);
        }
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        throw ex;
      }
    }
    Vector lbAddressVector = new Vector();
    String lbAddress;
    for (int i = 0; i < lbVector.size(); i++) {
      lbAddress = lbVector.get(i).toString().trim();
      lbAddressVector.add(Constant.getInstance(lbAddress));
    }
    try {
      if (userPrefJobSubmitterAd.hasAttribute(Utils.PREF_FILE_LB_ADDRESS)) {
        userPrefJobSubmitterAd.delAttribute(Utils.PREF_FILE_LB_ADDRESS);
      }
      userPrefJobSubmitterAd.setAttribute(Utils.PREF_FILE_LB_ADDRESS,
          new ListExpr(lbAddressVector));
      userConfAd.setAttribute(Utils.PREF_FILE_JOB_SUBMITTER,
          userPrefJobSubmitterAd);
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
      throw ex;
    }
    try {
      GUIFileSystem.saveTextFile(userConfFile, userConfAd.toString());
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
      throw ex;
    }
  }

  static Vector loadPrefFileNS() {
    Ad userConfAd = new Ad();
    Vector nsVector = new Vector();
    File userConfFile = new File(GUIFileSystem.getUserPrefFile());
    logger.debug("loadPrefFileNS() file: " + GUIFileSystem.getUserPrefFile());
    if (userConfFile.isFile()) {
      try {
        userConfAd.fromFile(userConfFile.toString());
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
      try {
        if (userConfAd.hasAttribute(Utils.PREF_FILE_JOB_SUBMITTER)) {
          Ad userConfJobSubmitterAd = userConfAd
              .getAd(Utils.PREF_FILE_JOB_SUBMITTER);
          if (userConfJobSubmitterAd.hasAttribute(Utils.PREF_FILE_NS_NAME)) {
            Vector nsNameVector = userConfJobSubmitterAd
                .getStringValue(Utils.PREF_FILE_NS_NAME);
            Vector nsAddressVector = userConfJobSubmitterAd
                .getStringValue(Utils.PREF_FILE_NS_ADDRESS);
            Vector nsJDLESchemaVector = userConfJobSubmitterAd
                .getStringValue(Utils.PREF_FILE_JDLE_SCHEMA);
            for (int i = 0; i < nsNameVector.size(); i++) {
              nsVector.add(new NetworkServer(nsNameVector.get(i).toString(),
                  nsAddressVector.get(i).toString(), nsJDLESchemaVector.get(i)
                      .toString()));
            }
          }
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
    }
    return nsVector;
  }

  static void savePrefFileNS(Vector nsNameVector, Vector nsAddressVector,
      Vector nsSchemaVector) throws Exception {
    Ad userConfAd = new Ad();
    Ad userConfJobSubmitterAd = new Ad();
    File userConfFile = new File(GUIFileSystem.getUserPrefFile());
    if (userConfFile.isFile()) {
      try {
        userConfAd.fromFile(userConfFile.toString());
        if (userConfAd.hasAttribute(Utils.PREF_FILE_JOB_SUBMITTER)) {
          userConfJobSubmitterAd = userConfAd
              .getAd(Utils.PREF_FILE_JOB_SUBMITTER);
          userConfAd.delAttribute(Utils.PREF_FILE_JOB_SUBMITTER);
        }
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
    }
    Vector guiNSNameVector = new Vector();
    Vector guiNSAddressVector = new Vector();
    Vector guiNSJDLESchemaVector = new Vector();
    for (int i = 0; i < nsNameVector.size(); i++) {
      guiNSNameVector.add(Constant.getInstance(nsNameVector.get(i).toString()));
      guiNSAddressVector.add(Constant.getInstance(nsAddressVector.get(i)
          .toString()));
      guiNSJDLESchemaVector.add(Constant.getInstance(nsSchemaVector.get(i)
          .toString()));
    }
    try {
      if (userConfJobSubmitterAd.hasAttribute(Utils.PREF_FILE_NS_NAME)) {
        userConfJobSubmitterAd.delAttribute(Utils.PREF_FILE_NS_NAME);
      }
      if (userConfJobSubmitterAd.hasAttribute(Utils.PREF_FILE_NS_ADDRESS)) {
        userConfJobSubmitterAd.delAttribute(Utils.PREF_FILE_NS_ADDRESS);
      }
      if (userConfJobSubmitterAd.hasAttribute(Utils.PREF_FILE_JDLE_SCHEMA)) {
        userConfJobSubmitterAd.delAttribute(Utils.PREF_FILE_JDLE_SCHEMA);
      }
      userConfJobSubmitterAd.setAttribute(Utils.PREF_FILE_NS_NAME,
          new ListExpr(guiNSNameVector));
      userConfJobSubmitterAd.setAttribute(Utils.PREF_FILE_NS_ADDRESS,
          new ListExpr(guiNSAddressVector));
      userConfJobSubmitterAd.setAttribute(Utils.PREF_FILE_JDLE_SCHEMA,
          new ListExpr(guiNSJDLESchemaVector));
      userConfAd.setAttribute(Utils.PREF_FILE_JOB_SUBMITTER,
          userConfJobSubmitterAd);
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
    }
    try {
      GUIFileSystem.saveTextFile(userConfFile, userConfAd.toString());
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
      throw ex;
    }
  }

  static Vector loadPrefFileNSNames() {
    Vector nsNameVector = new Vector();
    Vector nsVector = loadPrefFileNS();
    for (int i = 0; i < nsVector.size(); i++) {
      nsNameVector.add(((NetworkServer) nsVector.get(i)).getName());
    }
    return nsNameVector;
  }

  void setTabbedPanelsFromPreferences(String preferences) {
    if (preferences.equals(Utils.USER_PREFERENCES)) {
      Vector nsVectorConfFile = JobSubmitterPreferences.loadPrefFileNS();
      jobSubmitterJFrame.setNSTabbedPanePanels(nsVectorConfFile);
      jobSubmitterJFrame.setNSMenuItems(JobSubmitterPreferences
          .loadPrefFileNSNames());
      jobSubmitterPreferencesPanel.loadPreferencesFromFile();
    } else if (preferences.equals(Utils.DEFAULT_PREFERENCES)) {
      Vector nsAddressesVector = GUIGlobalVars.getNSVector();
      if (nsAddressesVector.size() != 0) {
        Vector nsVector = new Vector();
        Vector nsNameVector = new Vector();
        Vector nsSchemaVector = new Vector();
        String nsName;
        for (int i = 0; i < nsAddressesVector.size(); i++) {
          nsName = Utils.DEFAULT_NS_NAME + Integer.toString(i + 1);
          nsNameVector.add(nsName);
          nsVector.add(new NetworkServer(nsName, nsAddressesVector.get(i)
              .toString(), Utils.DEFAULT_INFORMATION_SCHEMA));
          nsSchemaVector.add(Utils.DEFAULT_INFORMATION_SCHEMA);
        }
        jobSubmitterJFrame.setNSTabbedPanePanels(nsVector);
        jobSubmitterJFrame.setNSMenuItems(nsNameVector);
        jobSubmitterPreferencesPanel.loadDefaultPreferences();
      }
    }
  }

  void jButtonLoadDefaultEvent(ActionEvent ae) {
    int choice = JOptionPane.showOptionDialog(JobSubmitterPreferences.this,
        "Load Default Preferences?", "Confirm Load Default Preferences",
        JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null,
        null);
    if (choice == 0) {
      jobSubmitterPreferencesPanel.loadDefaultPreferences();
      jobMonitorPreferencesPanel.loadDefaultPreferences();
      jdlDefaultsPreferencesPanel.loadDefaultPreferences();
      loggingPreferencesPanel.loadDefaultPreferences();
    }
  }

  void jButtonLoadUserEvent(ActionEvent ae) {
    jobSubmitterPreferencesPanel.loadPreferencesFromFile();
    //jobMonitorPreferencesPanel.loadPreferencesFromFile();
    try {
      jobMonitorPreferencesPanel.loadPreferencesFromFile();
    } catch (Exception e) {
      e.printStackTrace();
      JOptionPane.showOptionDialog(JobSubmitterPreferences.this,
          e.getMessage(), Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
    }
    jdlDefaultsPreferencesPanel.loadPreferencesFromFile();
    loggingPreferencesPanel.loadPreferencesFromFile();
  }

  static String initializeUserConfiguration() {
    Ad userPrefAd = new Ad();
    Ad defaultPrefAd = new Ad();
    Ad voSpecificPrefAd = new Ad();
    String warningMsg = "";
    try {
      logger.debug("Reading getUserPrefFile: "
          + GUIFileSystem.getUserPrefFile());
      userPrefAd.fromFile(GUIFileSystem.getUserPrefFile());
      logger.debug("userPrefAd: " + userPrefAd);
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
    }
    try {
      logger.debug("Reading getGUIConfVarFile: "
          + GUIFileSystem.getGUIConfVarFile());
      defaultPrefAd.fromFile(GUIFileSystem.getGUIConfVarFile());
      logger.debug("defaultPrefFile: " + defaultPrefAd);
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
    }
    try {
      logger.debug("Reading getGUIConfVOFile: "
          + GUIFileSystem.getGUIConfVOFile());
      voSpecificPrefAd.fromFile(GUIFileSystem.getGUIConfVOFile());
      logger.debug("voSpecificPrefFile: " + voSpecificPrefAd);
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
    }
    //if ((userPrefAd.size() == 0) && (defaultPrefFile.size() == 0)) {
    //return;
    //}
    try {
      Vector returnVector = JDLDefaultsPreferencesPanel
          .getRequirementsRankVector(userPrefAd, voSpecificPrefAd,
              defaultPrefAd);
      GUIGlobalVars.srrDefaultVector = (Vector) returnVector.get(0);
      JDLDefaultsPreferencesPanel.setSchemaAttributes(
          GUIGlobalVars.srrDefaultVector, userPrefAd);
      warningMsg += returnVector.get(1).toString();
      if (userPrefAd.hasAttribute(Utils.CONF_FILE_HLRLOCATION)) {
        GUIGlobalVars.setHLRLocation(userPrefAd.getStringValue(
            Utils.CONF_FILE_HLRLOCATION).get(0).toString());
      } else if (voSpecificPrefAd.hasAttribute(Utils.CONF_FILE_HLRLOCATION)) {
        String hlrLocation = voSpecificPrefAd.getStringValue(
            Utils.CONF_FILE_HLRLOCATION).get(0).toString();
        GUIGlobalVars.setHLRLocation(hlrLocation);
        userPrefAd.setAttribute(Utils.CONF_FILE_HLRLOCATION, hlrLocation);
      }
      if (userPrefAd.hasAttribute(Utils.CONF_FILE_MYPROXYSERVER)) {
        GUIGlobalVars.setMyProxyServer(userPrefAd.getStringValue(
            Utils.CONF_FILE_MYPROXYSERVER).get(0).toString());
      } else if (voSpecificPrefAd.hasAttribute(Utils.CONF_FILE_MYPROXYSERVER)) {
        String myProxyServer = voSpecificPrefAd.getStringValue(
            Utils.CONF_FILE_MYPROXYSERVER).get(0).toString();
        GUIGlobalVars.setMyProxyServer(myProxyServer);
        userPrefAd.setAttribute(Utils.CONF_FILE_MYPROXYSERVER, myProxyServer);
      }
      if (userPrefAd.hasAttribute(Utils.GUI_CONF_VAR_RETRY_COUNT)) {
        GUIGlobalVars.setGUIConfVarRetryCount(((Integer) userPrefAd
            .getIntValue(Utils.GUI_CONF_VAR_RETRY_COUNT).get(0)).intValue());
      } else if (defaultPrefAd.hasAttribute(Utils.GUI_CONF_VAR_RETRY_COUNT)) {
        int retryCount = ((Integer) defaultPrefAd.getIntValue(
            Utils.GUI_CONF_VAR_RETRY_COUNT).get(0)).intValue();
        GUIGlobalVars.setGUIConfVarRetryCount(retryCount);
        userPrefAd.setAttribute(Utils.GUI_CONF_VAR_RETRY_COUNT, retryCount);
      }
      // LoggingPreferencesPanel
      if (userPrefAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_DESTINATION)) {
        GUIGlobalVars.setGUIConfVarLoggingDestination(userPrefAd
            .getStringValue(Utils.GUI_CONF_VAR_LOGGING_DESTINATION).get(0)
            .toString());
      } else if (voSpecificPrefAd
          .hasAttribute(Utils.GUI_CONF_VAR_LOGGING_DESTINATION)) {
        String loggingDestination = voSpecificPrefAd.getStringValue(
            Utils.GUI_CONF_VAR_LOGGING_DESTINATION).get(0).toString();
        GUIGlobalVars.setGUIConfVarLoggingDestination(loggingDestination);
        userPrefAd.setAttribute(Utils.GUI_CONF_VAR_LOGGING_DESTINATION,
            loggingDestination);
      }
      if (userPrefAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_TIMEOUT)) {
        GUIGlobalVars
            .setGUIConfVarLoggingTimeout(((Integer) userPrefAd.getIntValue(
                Utils.GUI_CONF_VAR_LOGGING_TIMEOUT).get(0)).intValue());
      } else if (defaultPrefAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_TIMEOUT)) {
        int loggingTimeout = ((Integer) defaultPrefAd.getIntValue(
            Utils.GUI_CONF_VAR_LOGGING_TIMEOUT).get(0)).intValue();
        GUIGlobalVars.setGUIConfVarLoggingTimeout(loggingTimeout);
        userPrefAd.setAttribute(Utils.GUI_CONF_VAR_LOGGING_TIMEOUT,
            loggingTimeout);
      }
      if (userPrefAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT)) {
        GUIGlobalVars.setGUIConfVarLoggingSyncTimeout(((Integer) userPrefAd
            .getIntValue(Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT).get(0))
            .intValue());
      } else if (defaultPrefAd
          .hasAttribute(Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT)) {
        int loggingTimeout = ((Integer) defaultPrefAd.getIntValue(
            Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT).get(0)).intValue();
        GUIGlobalVars.setGUIConfVarLoggingSyncTimeout(loggingTimeout);
        userPrefAd.setAttribute(Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT,
            loggingTimeout);
      }
      if (userPrefAd.hasAttribute(Utils.GUI_CONF_VAR_ERROR_STORAGE)) {
        GUIGlobalVars.setGUIConfVarErrorStorage(userPrefAd.getStringValue(
            Utils.GUI_CONF_VAR_ERROR_STORAGE).get(0).toString());
      } else if (defaultPrefAd.hasAttribute(Utils.GUI_CONF_VAR_ERROR_STORAGE)) {
        String errorStorage = defaultPrefAd.getStringValue(
            Utils.GUI_CONF_VAR_ERROR_STORAGE).get(0).toString();
        GUIGlobalVars.setGUIConfVarErrorStorage(errorStorage);
        userPrefAd.setAttribute(Utils.GUI_CONF_VAR_ERROR_STORAGE, errorStorage);
      } else {
        userPrefAd.setAttribute(Utils.GUI_CONF_VAR_ERROR_STORAGE, GUIGlobalVars
            .getGUIConfVarErrorStorage());
      }
      GUIFileSystem.saveTextFile(GUIFileSystem.getUserPrefFile(), userPrefAd
          .toString(true, true));
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    return warningMsg;
  }

  void setCancelButtonVisible(boolean bool) {
    jButtonCancel.setVisible(bool);
    jButtonLoadDefault.setVisible(bool);
    jLabelFill.setVisible(bool);
  }

  LBPreferencesPanel getLBPreferencesPanelReference() {
    return jobMonitorPreferencesPanel;
  }

  NSPreferencesPanel getNSPreferencesPanelReference() {
    return jobSubmitterPreferencesPanel;
  }
}