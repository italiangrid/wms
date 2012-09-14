/*
 * LoggingPreferencesPanel.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.awt.AWTEvent;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.FocusEvent;
import java.io.File;
import java.net.URL;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.plaf.basic.BasicArrowButton;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Ad;

/**
 * Implementation of the LoggingPreferencesPanel class.
 * This class implements the main part of the Job Submitter application
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class LoggingPreferencesPanel extends JPanel {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  JobMonitor jobMonitorJFrame;

  JobSubmitter jobSubmitterJFrame;

  JPanel jPanelLogging = new JPanel();

  JTextField jTextFieldLoggingTimeout = new JTextField();

  BasicArrowButton upLoggingTimeout = new BasicArrowButton(
      BasicArrowButton.NORTH);

  BasicArrowButton downLoggingTimeout = new BasicArrowButton(
      BasicArrowButton.SOUTH);

  JTextField jTextFieldLoggingSyncTimeout = new JTextField();

  BasicArrowButton downLoggingSyncTimeout = new BasicArrowButton(
      BasicArrowButton.SOUTH);

  BasicArrowButton upLoggingSyncTimeout = new BasicArrowButton(
      BasicArrowButton.NORTH);

  JLabel jLabelLoggingDestination = new JLabel();

  JTextField jTextFieldLoggingDestination = new JTextField();

  JCheckBox jCheckBoxRetryCount = new JCheckBox();

  JCheckBox jCheckBoxLoggingTimeout = new JCheckBox();

  JCheckBox jCheckBoxLoggingSyncTimeout = new JCheckBox();

  JTextField jTextFieldErrorStorage = new JTextField();

  JLabel jLabelErrorStorage = new JLabel();

  JButton jButtonErrorStorageFile = new JButton();

  JPanel jPanelErrorStorage = new JPanel();

  JLabel jLabelSec1 = new JLabel();

  JLabel jLabelSec2 = new JLabel();

  /**
   * Constructor.
   */
  public LoggingPreferencesPanel(Component component) {
    //super((JobMonitor) component);
    if (component instanceof JobMonitor) {
      jobMonitorJFrame = (JobMonitor) component;
    } else if (component instanceof JobSubmitter) {
      jobSubmitterJFrame = (JobSubmitter) component;
    } else {
      System.exit(-1);
    }
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
    jTextFieldLoggingTimeout.setText(Integer
        .toString(Utils.LOGGING_TIMEOUT_DEF_VAL));
    jTextFieldLoggingTimeout.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldLoggingTimeout.setBounds(new Rectangle(149, 50, 28, 30));
    jTextFieldLoggingTimeout
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            GraphicUtils.jTextFieldFocusLost(jTextFieldLoggingTimeout,
                Utils.INTEGER, Integer.toString(Utils.LOGGING_TIMEOUT_DEF_VAL),
                Utils.LOGGING_TIMEOUT_MIN_VAL, Utils.LOGGING_TIMEOUT_MAX_VAL);
          }

          public void focusGained(FocusEvent e) {
          }
        });
    upLoggingTimeout.setBounds(new Rectangle(179, 49, 16, 16));
    upLoggingTimeout.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldLoggingTimeout, Utils.INTEGER, Integer
            .toString(Utils.LOGGING_TIMEOUT_DEF_VAL),
            Utils.LOGGING_TIMEOUT_MIN_VAL, Utils.LOGGING_TIMEOUT_MAX_VAL);
      }
    });
    downLoggingTimeout.setBounds(new Rectangle(179, 64, 16, 16));
    downLoggingTimeout.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldLoggingTimeout, Utils.INTEGER, Integer
            .toString(Utils.LOGGING_TIMEOUT_DEF_VAL),
            Utils.LOGGING_TIMEOUT_MIN_VAL, Utils.LOGGING_TIMEOUT_MAX_VAL);
      }
    });
    jTextFieldLoggingSyncTimeout
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            GraphicUtils.jTextFieldFocusLost(jTextFieldLoggingTimeout,
                Utils.INTEGER, Integer
                    .toString(Utils.LOGGING_SYNC_TIMEOUT_DEF_VAL),
                Utils.LOGGING_SYNC_TIMEOUT_MIN_VAL,
                Utils.LOGGING_SYNC_TIMEOUT_MAX_VAL);
          }

          public void focusGained(FocusEvent e) {
          }
        });
    downLoggingSyncTimeout.setBounds(new Rectangle(458, 64, 16, 16));
    downLoggingSyncTimeout
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            Utils.downButtonEvent(jTextFieldLoggingSyncTimeout, Utils.INTEGER,
                Integer.toString(Utils.LOGGING_SYNC_TIMEOUT_DEF_VAL),
                Utils.LOGGING_SYNC_TIMEOUT_MIN_VAL,
                Utils.LOGGING_SYNC_TIMEOUT_MAX_VAL);
          }
        });
    upLoggingSyncTimeout.setBounds(new Rectangle(458, 49, 16, 16));
    upLoggingSyncTimeout.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldLoggingSyncTimeout, Utils.INTEGER,
            Integer.toString(Utils.LOGGING_SYNC_TIMEOUT_DEF_VAL),
            Utils.LOGGING_SYNC_TIMEOUT_MIN_VAL,
            Utils.LOGGING_SYNC_TIMEOUT_MAX_VAL);
      }
    });
    jTextFieldLoggingSyncTimeout.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldLoggingSyncTimeout.setBounds(new Rectangle(428, 50, 28, 30));
    jTextFieldLoggingSyncTimeout.setText(Integer
        .toString(Utils.LOGGING_SYNC_TIMEOUT_DEF_VAL));
    jLabelLoggingDestination.setBounds(new Rectangle(14, 22, 129, 22));
    jLabelLoggingDestination.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelLoggingDestination.setText("Logging Destination");
    jTextFieldLoggingDestination.setBounds(new Rectangle(149, 22, 367, 22));
    jTextFieldLoggingDestination.setText("");
    jTextFieldErrorStorage.setBounds(new Rectangle(93, 23, 405, 22));
    jTextFieldErrorStorage.setText(GUIGlobalVars.getGUIConfVarErrorStorage());
    jLabelErrorStorage.setBounds(new Rectangle(7, 23, 82, 22));
    jLabelErrorStorage.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelErrorStorage.setText("ErrorStorage");
    jButtonErrorStorageFile.setBounds(new Rectangle(496, 23, 22, 22));
    jButtonErrorStorageFile
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jButtonErrorStorageFileEvent(e);
          }
        });
    URL url = LoggingPreferencesPanel.class.getResource(Utils.ICON_FILE_OPEN);
    if (url != null) {
      jButtonErrorStorageFile.setIcon(new ImageIcon(url));
    } else {
      jButtonErrorStorageFile.setText("...");
    }
    jPanelLogging.setBorder(new TitledBorder(new EtchedBorder(),
        " LB Logging ", 0, 0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelLogging.setBounds(new Rectangle(5, 5, 527, 95));
    jPanelLogging.setLayout(null);
    jCheckBoxLoggingTimeout.setHorizontalAlignment(SwingConstants.CENTER);
    jCheckBoxLoggingTimeout.setText("Logging Timeout");
    jCheckBoxLoggingTimeout.setBounds(new Rectangle(6, 56, 136, 19));
    jCheckBoxLoggingTimeout
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jCheckBoxLoggingTimeoutEvent(e);
          }
        });
    jCheckBoxLoggingSyncTimeout.setHorizontalAlignment(SwingConstants.CENTER);
    jCheckBoxLoggingSyncTimeout.setText("Logging Sync Timeout");
    jCheckBoxLoggingSyncTimeout.setBounds(new Rectangle(252, 56, 173, 19));
    jCheckBoxLoggingSyncTimeout
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jCheckBoxLoggingSyncTimeoutEvent(e);
          }
        });
    jPanelErrorStorage.setBorder(new TitledBorder(new EtchedBorder(),
        " JDL Editor Parsing Error File Path ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelErrorStorage.setBounds(new Rectangle(5, 103, 527, 61));
    jPanelErrorStorage.setLayout(null);
    jLabelSec1.setText("sec.");
    jLabelSec1.setBounds(new Rectangle(198, 58, 30, 15));
    jLabelSec2.setText("sec.");
    jLabelSec2.setBounds(new Rectangle(478, 58, 30, 15));
    jPanelLogging.add(jCheckBoxRetryCount, null);
    jPanelErrorStorage.add(jTextFieldErrorStorage, null);
    jPanelErrorStorage.add(jButtonErrorStorageFile, null);
    jPanelErrorStorage.add(jLabelErrorStorage, null);
    setSize(new Dimension(553, 462));
    setLayout(null);
    jPanelLogging.add(jCheckBoxRetryCount, null);
    this.add(jPanelLogging, null);
    jPanelLogging.add(jLabelLoggingDestination, null);
    jPanelLogging.add(jTextFieldLoggingDestination, null);
    jPanelLogging.add(jCheckBoxLoggingTimeout, null);
    jPanelLogging.add(jTextFieldLoggingTimeout, null);
    jPanelLogging.add(downLoggingTimeout, null);
    jPanelLogging.add(upLoggingTimeout, null);
    jPanelLogging.add(jLabelSec1, null);
    jPanelLogging.add(downLoggingSyncTimeout, null);
    jPanelLogging.add(jCheckBoxLoggingSyncTimeout, null);
    jPanelLogging.add(jTextFieldLoggingSyncTimeout, null);
    jPanelLogging.add(upLoggingSyncTimeout, null);
    jPanelLogging.add(jLabelSec2, null);
    this.add(jPanelErrorStorage, null);
    setLoggingTimeoutEnabled(false);
    setLoggingSyncTimeoutEnabled(false);
    //loadDefaultPreferences();
  }

  /*
   void jButtonApplyEvent(ActionEvent ae) {
   String jComboSelection =
   jobSubmitterJFrame.jobSubmitterPreferences.jComboBoxPreferences.getSelectedItem().toString().trim();
   if (jComboSelection.equals(Utils.USER_PREFERENCES)) {
   String uiConfFileName = Utils.getGUIConfVarFile();
   File confFile = new File(uiConfFileName);
   Ad confAd = new Ad();
   if (confFile.isFile()) {
   try {
   confAd.fromFile(uiConfFileName);
   } catch (Exception e) {
   if (isDebugging) e.printStackTrace();
   }
   }
   int loggingTimeOut = Integer.parseInt(jTextFieldLoggingTimeout.getText().trim(), 10);
   int loggingSyncTimeOut = Integer.parseInt(jTextFieldLoggingSyncTimeout.getText().trim(), 10);
   String loggingDestination = jTextFieldLoggingDestination.getText().trim();
   try {
   if (confAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_TIMEOUT)) {
   confAd.delAttribute(Utils.GUI_CONF_VAR_LOGGING_TIMEOUT);
   }
   confAd.setAttribute(Utils.GUI_CONF_VAR_LOGGING_TIMEOUT, loggingTimeOut);
   if (confAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT)) {
   confAd.delAttribute(Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT);
   }
   confAd.setAttribute(Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT, loggingSyncTimeOut);
   if (confAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_DESTINATION)) {
   confAd.delAttribute(Utils.GUI_CONF_VAR_LOGGING_DESTINATION);
   }
   confAd.setAttribute(Utils.GUI_CONF_VAR_LOGGING_DESTINATION, loggingDestination);
   GUIFileSystem.saveTextFile(uiConfFileName, confAd.toString());
   } catch (Exception e) {
   if (isDebugging) e.printStackTrace();
   }
   }
   }
   */
  void jCheckBoxLoggingTimeoutEvent(ActionEvent e) {
    setLoggingTimeoutEnabled(jCheckBoxLoggingTimeout.isSelected());
  }

  void setLoggingTimeoutEnabled(boolean bool) {
    jTextFieldLoggingTimeout.setEnabled(bool);
    upLoggingTimeout.setEnabled(bool);
    downLoggingTimeout.setEnabled(bool);
  }

  void jCheckBoxLoggingSyncTimeoutEvent(ActionEvent e) {
    setLoggingSyncTimeoutEnabled(jCheckBoxLoggingSyncTimeout.isSelected());
  }

  void setLoggingSyncTimeoutEnabled(boolean bool) {
    jTextFieldLoggingSyncTimeout.setEnabled(bool);
    upLoggingSyncTimeout.setEnabled(bool);
    downLoggingSyncTimeout.setEnabled(bool);
  }

  int jButtonApplyEvent(ActionEvent ae) {
    String errorStorage = jTextFieldErrorStorage.getText().trim();
    File errorStorageFile = new File(errorStorage);
    String title = "<html><font color=\"#602080\">"
        + JobSubmitterPreferences.LOGGING_PANEL_NAME + ":" + "</font>";
    JDialog jDialog = (JDialog) jobSubmitterJFrame
        .getJobSubmitterPreferencesReference();
    if (errorStorageFile.isDirectory()) {
      if (!errorStorageFile.canWrite()) {
        JOptionPane.showOptionDialog(jDialog, title
            + "\nUnable to set ErrorStorage to provided value"
            + "\nno authorization to write in: " + errorStorage,
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        jTextFieldErrorStorage.selectAll();
        jTextFieldErrorStorage.grabFocus();
        return Utils.FAILED;
      }
    } else {
      String message = "";
      if (errorStorage.equals("")) {
        message = "\nErrorStorage field cannot be blank";
      } else {
        message = "\nUnable to set ErrorStorage to provided value"
            + "\nunable to find directory: " + errorStorage;
      }
      JOptionPane.showOptionDialog(jDialog, title + message,
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      jTextFieldErrorStorage.selectAll();
      jTextFieldErrorStorage.grabFocus();
      return Utils.FAILED;
    }
    File userConfFile = new File(GUIFileSystem.getUserPrefFile());
    Ad confAd = new Ad();
    if (userConfFile.isFile()) {
      try {
        confAd.fromFile(userConfFile.toString());
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
    }
    try {
      if (confAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_TIMEOUT)) {
        confAd.delAttribute(Utils.GUI_CONF_VAR_LOGGING_TIMEOUT);
      }
      if (jCheckBoxLoggingTimeout.isSelected()) {
        int loggingTimeout = Integer.parseInt(jTextFieldLoggingTimeout
            .getText(), 10);
        confAd.setAttribute(Utils.GUI_CONF_VAR_LOGGING_TIMEOUT, loggingTimeout);
        GUIGlobalVars.setGUIConfVarLoggingTimeout(loggingTimeout);
      }
      if (confAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT)) {
        confAd.delAttribute(Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT);
      }
      if (jCheckBoxLoggingSyncTimeout.isSelected()) {
        int loggingSyncTimeout = Integer.parseInt(jTextFieldLoggingSyncTimeout
            .getText(), 10);
        confAd.setAttribute(Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT,
            loggingSyncTimeout);
        GUIGlobalVars.setGUIConfVarLoggingTimeout(loggingSyncTimeout);
      }
      if (confAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_DESTINATION)) {
        confAd.delAttribute(Utils.GUI_CONF_VAR_LOGGING_DESTINATION);
      }
      String loggingDestination = jTextFieldLoggingDestination.getText().trim();
      //if (!loggingDestination.equals("")) {
      confAd.setAttribute(Utils.GUI_CONF_VAR_LOGGING_DESTINATION,
          loggingDestination);
      //}
      GUIGlobalVars.setGUIConfVarLoggingDestination(loggingDestination);
      if (confAd.hasAttribute(Utils.GUI_CONF_VAR_ERROR_STORAGE)) {
        confAd.delAttribute(Utils.GUI_CONF_VAR_ERROR_STORAGE);
      }
      if (!errorStorage.equals("")) {
        confAd.setAttribute(Utils.GUI_CONF_VAR_ERROR_STORAGE, errorStorage);
      }
      GUIGlobalVars.setGUIConfVarErrorStorage(errorStorage);
      GUIFileSystem.saveTextFile(userConfFile, confAd.toString(true, false));
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    return Utils.SUCCESS;
  }

  void loadPreferencesFromFile() {
    resetAttributeValues();
    Ad userConfAd = new Ad();
    File userConfFile = new File(GUIFileSystem.getUserPrefFile());
    if (userConfFile.isFile()) {
      try {
        userConfAd.fromFile(userConfFile.toString());
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
      try {
        if (userConfAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_TIMEOUT)) {
          setLoggingTimeoutEnabled(true);
          jCheckBoxLoggingTimeout.setSelected(true);
          jTextFieldLoggingTimeout.setText(((Integer) userConfAd.getIntValue(
              Utils.GUI_CONF_VAR_LOGGING_TIMEOUT).get(0)).toString());
        }
        if (userConfAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT)) {
          setLoggingSyncTimeoutEnabled(true);
          jCheckBoxLoggingSyncTimeout.setSelected(true);
          jTextFieldLoggingSyncTimeout.setText(((Integer) userConfAd
              .getIntValue(Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT).get(0))
              .toString());
        }
        if (userConfAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_DESTINATION)) {
          jTextFieldLoggingDestination.setText(userConfAd.getStringValue(
              Utils.GUI_CONF_VAR_LOGGING_DESTINATION).get(0).toString().trim());
        }
        if (userConfAd.hasAttribute(Utils.GUI_CONF_VAR_ERROR_STORAGE)) {
          jTextFieldErrorStorage.setText(userConfAd.getStringValue(
              Utils.GUI_CONF_VAR_ERROR_STORAGE).get(0).toString().trim());
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
    }
  }

  void loadDefaultPreferences() {
    resetAttributeValues();
    Ad defPrefAd = new Ad();
    String uiPrefFileName = GUIFileSystem.getGUIConfVOFile();
    File defPrefFile = new File(uiPrefFileName);
    if (defPrefFile.isFile()) {
      try {
        defPrefAd.fromFile(uiPrefFileName.toString());
        if (defPrefAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_DESTINATION)) {
          jTextFieldLoggingDestination.setText(defPrefAd.getStringValue(
              Utils.GUI_CONF_VAR_LOGGING_DESTINATION).get(0).toString().trim());
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
    }
    defPrefAd = new Ad();
    uiPrefFileName = GUIFileSystem.getGUIConfVarFile();
    defPrefFile = new File(uiPrefFileName);
    if (defPrefFile.isFile()) {
      try {
        defPrefAd.fromFile(uiPrefFileName.toString());
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
      try {
        if (defPrefAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_TIMEOUT)) {
          setLoggingTimeoutEnabled(true);
          jCheckBoxLoggingTimeout.setSelected(true);
          jTextFieldLoggingTimeout.setText(((Integer) defPrefAd.getIntValue(
              Utils.GUI_CONF_VAR_LOGGING_TIMEOUT).get(0)).toString());
        }
        if (defPrefAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT)) {
          setLoggingSyncTimeoutEnabled(true);
          jCheckBoxLoggingSyncTimeout.setSelected(true);
          jTextFieldLoggingSyncTimeout.setText(((Integer) defPrefAd
              .getIntValue(Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT).get(0))
              .toString());
        }
        if (defPrefAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_DESTINATION)) {
          //jTextFieldLoggingDestination.setText(defPrefAd.getStringValue(
          //    Utils.GUI_CONF_VAR_LOGGING_DESTINATION).get(0).toString().trim());
          JOptionPane.showOptionDialog(
              jobSubmitterJFrame.jobSubmitterPreferences,
              "Deprecated attribute " + "'"
                  + Utils.GUI_CONF_VAR_LOGGING_DESTINATION
                  + "' found in GUI configuration file", Utils.WARNING_MSG_TXT,
              JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null,
              null, null);
        }
        if (defPrefAd.hasAttribute(Utils.GUI_CONF_VAR_ERROR_STORAGE)) {
          jTextFieldErrorStorage.setText(defPrefAd.getStringValue(
              Utils.GUI_CONF_VAR_ERROR_STORAGE).get(0).toString().trim());
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
    }
  }

  void resetAttributeValues() {
    jTextFieldLoggingDestination.setText("");
    jTextFieldLoggingTimeout.setText(Integer
        .toString(Utils.LOGGING_TIMEOUT_DEF_VAL));
    jCheckBoxLoggingTimeout.setSelected(false);
    setLoggingTimeoutEnabled(false);
    jTextFieldLoggingSyncTimeout.setText(Integer
        .toString(Utils.LOGGING_SYNC_TIMEOUT_DEF_VAL));
    jCheckBoxLoggingSyncTimeout.setSelected(false);
    setLoggingSyncTimeoutEnabled(false);
    jTextFieldErrorStorage.setText(GUIGlobalVars.getGUIConfVarErrorStorage());
  }

  void jButtonErrorStorageFileEvent(ActionEvent e) {
    JFileChooser fileChooser = new JFileChooser();
    fileChooser.setDialogTitle("JDL Editor Parsing Error File Path Selection");
    fileChooser.setCurrentDirectory(new File(GUIGlobalVars
        .getFileChooserWorkingDirectory()));
    fileChooser.setApproveButtonToolTipText("Select File Path");
    fileChooser.setApproveButtonText("Select");
    //fileChooser.setFileHidingEnabled(false);
    fileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
    int choice = fileChooser.showOpenDialog(LoggingPreferencesPanel.this);
    if (choice != JFileChooser.APPROVE_OPTION) {
      return;
    } else if (!fileChooser.getSelectedFile().isDirectory()) {
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      JOptionPane.showOptionDialog(LoggingPreferencesPanel.this,
          "Unable to find directory: " + selectedFile, Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      return;
    } else {
      GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
          .getCurrentDirectory().toString());
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      jTextFieldErrorStorage.setText(selectedFile);
    }
  }
}