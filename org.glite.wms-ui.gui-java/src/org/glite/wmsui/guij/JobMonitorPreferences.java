/*
 * JobMonitorPreferences.java
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
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Ad;
import condor.classad.Constant;
import condor.classad.ListExpr;

/**
 * Implementation of the JobMonitorPreferences class.
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class JobMonitorPreferences extends JDialog {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final String LB_PANEL_NAME = "Logging & Bookkeeping";

  static final String LB_QUERY_PANEL_NAME = "User Queries";

  JPanel jPanelButton = new JPanel();

  JButton jButtonCancel = new JButton();

  JButton jButtonApply = new JButton();

  JButton jButtonOk = new JButton();

  JButton jButtonLoadDefault = new JButton();

  JButton jButtonLoadUser = new JButton();

  JobMonitor jobMonitorJFrame;

  LBPreferencesPanel jobMonitorPreferencesPanel;

  QueryPreferencesPanel queryPreferencesPanel;

  JLabel jLabelFill = new JLabel();

  boolean reloadOldWorkingSession = false;

  boolean isUserPreferences = false;

  JTabbedPane jTabbedPane = new JTabbedPane();

  /**
   * Constructor.
   */
  public JobMonitorPreferences(Component component) {
    super((JobMonitor) component);
    jobMonitorJFrame = (JobMonitor) component;
    jobMonitorPreferencesPanel = new LBPreferencesPanel(jobMonitorJFrame);
    queryPreferencesPanel = new QueryPreferencesPanel(this, jobMonitorJFrame);
    jobMonitorPreferencesPanel.setNSComboVisible(false);
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit();
    } catch (Exception e) {
      if (isDebugging)
        e.printStackTrace();
    }
  }

  private void jbInit() throws Exception {
    isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG) ? true
        : false;
    // Initialize tabbed pane panels.
    Object[][] element = { { LB_PANEL_NAME, jobMonitorPreferencesPanel
    }, { LB_QUERY_PANEL_NAME, queryPreferencesPanel
    }
    };
    for (int i = 0; i < element.length; i++) {
      jTabbedPane.addTab((String) element[i][0], (JPanel) element[i][1]);
    }
    jButtonCancel.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonCancelEvent(e);
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
        jButtonOkEvent(e);
      }
    });
    jButtonOk.setText("  Ok  ");
    jButtonLoadDefault.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonLoadDefaultEvent(e);
      }
    });
    jButtonLoadDefault.setText("Default");
    jButtonLoadUser.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonLoadUserEvent(e);
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
    this.getContentPane().setLayout(new BorderLayout());
    this.getContentPane().add(jTabbedPane, BorderLayout.CENTER);
    this.getContentPane().add(jPanelButton, BorderLayout.SOUTH);
    setSize(new Dimension(550, 410));
    setTitle("Job Monitor - Preferences");
    setResizable(false);
    jButtonLoadUserEvent(null);
  }

  protected void processWindowEvent(WindowEvent e) {
    super.processWindowEvent(e);
    this.setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
    if (e.getID() == WindowEvent.WINDOW_CLOSING) {
      this.dispose();
    }
  }

  void jButtonCancelEvent(ActionEvent e) {
    dispose();
  }

  void jButtonOkEvent(ActionEvent e) {
    jButtonApplyEvent(null);
    dispose();
  }

  int jButtonApplyEvent(ActionEvent e) {
    int result = Utils.SUCCESS;
    if (jobMonitorPreferencesPanel.jButtonApplyEvent(null) == Utils.FAILED) {
      result = Utils.FAILED;
    }
    setQueryPanelLBAvailability();
    queryPreferencesPanel.jButtonApplyEvent(null);
    return result;
  }

  void setQueryPanelLBAvailability() {
    String lbAddress = "";
    int index = -1;
    Vector menuLBVector = jobMonitorJFrame.getLBMenuItems();
    for (int i = 0; i < queryPreferencesPanel.jTableQueries.getRowCount(); i++) {
      lbAddress = queryPreferencesPanel.jobTableModel.getValueAt(i,
          QueryPreferencesPanel.LB_SERVER_COLUMN_INDEX).toString();
      if (lbAddress.equals(QueryPanel.ALL_LB_SERVERS)) {
        continue;
      }
      index = lbAddress.indexOf(QueryPreferencesPanel.NOT_AVAILABLE_LB);
      if (index != -1) {
        lbAddress = lbAddress.substring(0, index).trim();
      }
      if (!menuLBVector.contains(lbAddress)) {
        lbAddress += QueryPreferencesPanel.NOT_AVAILABLE_LB;
      }
      queryPreferencesPanel.jobTableModel.setValueAt(lbAddress, i,
          QueryPreferencesPanel.LB_SERVER_COLUMN_INDEX);
    }
  }

  static int loadPrefFileUpdateRate() {
    int updateRate = Utils.UPDATE_RATE_DEF_VAL;
    Ad userConfAd = new Ad();
    Ad userConfJobMonitorAd = new Ad();
    File userConfFile = new File(GUIFileSystem.getUserPrefFile());
    if (userConfFile.isFile()) {
      try {
        userConfAd.fromFile(userConfFile.toString());
        if (userConfAd.hasAttribute(Utils.PREF_FILE_JOB_MONITOR)) {
          userConfJobMonitorAd = userConfAd.getAd(Utils.PREF_FILE_JOB_MONITOR);
          if (userConfJobMonitorAd.hasAttribute(Utils.PREF_FILE_UPDATE_RATE)) {
            updateRate = ((Integer) userConfJobMonitorAd.getIntValue(
                Utils.PREF_FILE_UPDATE_RATE).get(0)).intValue();
          }
        }
      } catch (Exception ex) {
        if (isDebugging)
          ex.printStackTrace();
      }
    }
    return updateRate;
  }

  static Vector loadPrefFileLB() {
    Ad userConfAd = new Ad();
    Ad userConfJobMonitorAd = new Ad();
    Vector lbVector = new Vector();
    File userConfFile = new File(GUIFileSystem.getUserPrefFile());
    logger.debug("loadPrefFileLB() file: " + userConfFile);
    if (userConfFile.isFile()) {
      try {
        userConfAd.fromFile(userConfFile.toString());
        if (userConfAd.hasAttribute(Utils.PREF_FILE_JOB_MONITOR)) {
          userConfJobMonitorAd = userConfAd.getAd(Utils.PREF_FILE_JOB_MONITOR);
          if (userConfJobMonitorAd.hasAttribute(Utils.PREF_FILE_LB_ADDRESS)) {
            Vector lbAddressVector = userConfJobMonitorAd
                .getStringValue(Utils.PREF_FILE_LB_ADDRESS);
            for (int i = 0; i < lbAddressVector.size(); i++) {
              lbVector.add(lbAddressVector.get(i));
            }
          }
        }
      } catch (Exception ex) {
        if (isDebugging)
          ex.printStackTrace();
      }
    }
    return lbVector;
  }

  static void savePrefFileUpdateRate(Integer updateRate) throws Exception {
    try {
      savePrefFile(null, updateRate);
    } catch (Exception e) {
      if (isDebugging)
        e.printStackTrace();
      throw e;
    }
  }

  static void savePrefFileLB(Vector lbVector) throws Exception {
    try {
      savePrefFile(lbVector, null);
    } catch (Exception e) {
      if (isDebugging)
        e.printStackTrace();
      throw e;
    }
  }

  static void savePrefFile(Vector lbVector, int updateRate) throws Exception {
    try {
      savePrefFile(lbVector, new Integer(updateRate));
    } catch (Exception e) {
      if (isDebugging)
        e.printStackTrace();
      throw e;
    }
  }

  static void savePrefFile(Vector lbVector, Integer updateRate)
      throws Exception {
    Ad userConfAd = new Ad();
    Ad userConfJobMonitorAd = new Ad();
    File userConfFile = new File(GUIFileSystem.getUserPrefFile());
    if (userConfFile.isFile()) {
      try {
        userConfAd.fromFile(userConfFile.toString());
        if (userConfAd.hasAttribute(Utils.PREF_FILE_JOB_MONITOR)) {
          userConfJobMonitorAd = userConfAd.getAd(Utils.PREF_FILE_JOB_MONITOR);
          userConfAd.delAttribute(Utils.PREF_FILE_JOB_MONITOR);
        }
      } catch (Exception ex) {
        if (isDebugging)
          ex.printStackTrace();
      }
    }
    // Add LB(s) in preferences file.
    try {
      if (userConfJobMonitorAd.hasAttribute(Utils.PREF_FILE_LB_ADDRESS)) {
        userConfJobMonitorAd.delAttribute(Utils.PREF_FILE_LB_ADDRESS);
      }
    } catch (Exception e) {
      if (isDebugging)
        e.printStackTrace();
    }
    if ((lbVector != null) && (lbVector.size() != 0)) {
      Vector lbToSaveVector = new Vector();
      String lbAddress;
      for (int i = 0; i < lbVector.size(); i++) {
        lbAddress = lbVector.get(i).toString().trim();
        lbToSaveVector.add(Constant.getInstance(lbAddress));
      }
      try {
        userConfJobMonitorAd.setAttribute(Utils.PREF_FILE_LB_ADDRESS,
            new ListExpr(lbToSaveVector));
      } catch (Exception ex) {
        if (isDebugging)
          ex.printStackTrace();
      }
    }
    // Add update rate in preferences file.
    if (updateRate != null) {
      try {
        if (userConfJobMonitorAd.hasAttribute(Utils.PREF_FILE_UPDATE_RATE)) {
          userConfJobMonitorAd.delAttribute(Utils.PREF_FILE_UPDATE_RATE);
        }
        userConfJobMonitorAd.setAttribute(Utils.PREF_FILE_UPDATE_RATE,
            updateRate.intValue());
      } catch (Exception ex) {
        if (isDebugging)
          ex.printStackTrace();
      }
    }
    try {
      userConfAd
          .setAttribute(Utils.PREF_FILE_JOB_MONITOR, userConfJobMonitorAd);
    } catch (Exception ex) {
      if (isDebugging)
        ex.printStackTrace();
    }
    try {
      GUIFileSystem.saveTextFile(userConfFile, userConfAd.toString(true, true));
    } catch (Exception e) {
      if (isDebugging)
        e.printStackTrace();
      throw e;
    }
  }

  void jButtonLoadDefaultEvent(ActionEvent ae) {
    int choice = JOptionPane.showOptionDialog(JobMonitorPreferences.this,
        "Load Default Preferences?", "Confirm Load Default Preferences",
        JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null,
        null);
    if (choice == 0) {
      jobMonitorPreferencesPanel.loadDefaultPreferences();
    }
  }

  void jButtonLoadUserEvent(ActionEvent ae) {
    jobMonitorPreferencesPanel.loadPreferencesFromFile();
    queryPreferencesPanel.loadPreferencesFromFile();
  }

  void setUserPreferencesContext(boolean bool) {
    isUserPreferences = bool;
    if (bool) {
      jobMonitorJFrame.selectedPreferences = Utils.USER_PREFERENCES;
    }
  }

  static void initializeUserConfiguration() {
    Ad userPrefAd = new Ad();
    try {
      userPrefAd.fromFile(GUIFileSystem.getUserPrefFile());
    } catch (Exception ex) {
      if (isDebugging)
        ex.printStackTrace();
    }
    if (userPrefAd.size() == 0) {
      return;
    }
    try {
      if (userPrefAd.hasAttribute(Utils.PREF_FILE_JOB_MONITOR)) {
        Ad monitorAd = (Ad) userPrefAd.getAdValue(Utils.PREF_FILE_JOB_MONITOR)
            .get(0);
        logger.debug("initializeUserConfiguration() monitorAd: " + monitorAd);
        if (monitorAd.hasAttribute(Utils.PREF_FILE_UPDATE_RATE)) {
          GUIGlobalVars.setJobMonitorUpdateRate(((Integer) monitorAd
              .getIntValue(Utils.PREF_FILE_UPDATE_RATE).get(0)).intValue());
          logger.debug("initializeUserConfiguration pref file update rate: "
              + ((Integer) monitorAd.getIntValue(Utils.PREF_FILE_UPDATE_RATE)
                  .get(0)).intValue());
        }
      }
    } catch (Exception e) {
      if (isDebugging)
        e.printStackTrace();
    }
  }
}