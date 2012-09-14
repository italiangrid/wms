/*
 * JDLDefaultsPreferencesPanel.java
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
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.SwingConstants;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.plaf.basic.BasicArrowButton;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Ad;
import org.glite.jdl.Jdl;
import org.glite.jdl.JobAd;

/**
 * Implementation of the JDLDefaultsPreferencesPanel class.
 * This class implements the main part of the Job Submitter application
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class JDLDefaultsPreferencesPanel extends JPanel {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int SCHEMA_COLUMN_INDEX = 0;

  static final String SCHEMA_TABLE_HEADER = "Information Service Schema";

  static final int REQUIREMENTS_COLUMN_INDEX = 1;

  static final String REQUIREMENTS_TABLE_HEADER = "Requirements";

  static final int RANK_COLUMN_INDEX = 2;

  static final String RANK_TABLE_HEADER = "Rank";

  private Vector srrDefaultVector = new Vector();

  private String lastSelectedSchema = "";

  JobMonitor jobMonitorJFrame;

  JobSubmitter jobSubmitterJFrame;

  JLabel jLabelSchema = new JLabel();

  JComboBox jComboBoxSchema = new JComboBox();

  JLabel jLabelRank = new JLabel();

  JTextPane jTextPaneRank = new JTextPane();

  JLabel jLabelRequirements = new JLabel();

  JTextPane jTextPaneRequirements = new JTextPane();

  JPanel jPanelRankRequirementsDefault = new JPanel();

  JTextField jTextFieldRetryCount = new JTextField();

  BasicArrowButton downRetryCount = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton upRetryCount = new BasicArrowButton(BasicArrowButton.NORTH);

  JLabel jLabelHLRLocation = new JLabel();

  JTextField jTextFieldHLRLocation = new JTextField();

  JLabel jLabelMyProxyServer = new JLabel();

  JTextField jTextFieldMyProxyServer = new JTextField();

  JCheckBox jCheckBoxRetryCount = new JCheckBox();

  JScrollPane jScrollPaneRequirements = new JScrollPane();

  JScrollPane jScrollPaneRank = new JScrollPane();

  JTextPane jTextPaneRankMPI = new JTextPane();

  JScrollPane jScrollPaneRankMPI = new JScrollPane();

  JLabel jLabelRankMPI = new JLabel();

  /**
   * Constructor.
   */
  public JDLDefaultsPreferencesPanel(Component component) {
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
    for (int i = 0; i < Utils.jdleSchemaArray.length; i++) {
      jComboBoxSchema.addItem(Utils.jdleSchemaArray[i]);
    }
    this.lastSelectedSchema = Utils.jdleSchemaArray[0];
    jTextFieldRetryCount.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldRetryCount, Utils.INTEGER,
            Integer.toString(Utils.RETRYCOUNT_DEF_VAL),
            Utils.RETRYCOUNT_MIN_VAL, Utils.RETRYCOUNT_MAX_VAL);
      }

      public void focusGained(FocusEvent e) {
      }
    });
    downRetryCount.setBounds(new Rectangle(138, 265, 16, 16));
    downRetryCount.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldRetryCount, Utils.INTEGER, Integer
            .toString(Utils.RETRYCOUNT_DEF_VAL), Utils.RETRYCOUNT_MIN_VAL,
            Utils.RETRYCOUNT_MAX_VAL);
      }
    });
    upRetryCount.setBounds(new Rectangle(138, 250, 16, 16));
    upRetryCount.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldRetryCount, Utils.INTEGER, Integer
            .toString(Utils.RETRYCOUNT_DEF_VAL), Utils.RETRYCOUNT_MIN_VAL,
            Utils.RETRYCOUNT_MAX_VAL);
      }
    });
    jTextFieldRetryCount.setBounds(new Rectangle(107, 251, 28, 30));
    jTextFieldRetryCount.setText(Integer.toString(Utils.RETRYCOUNT_DEF_VAL));
    jTextFieldRetryCount.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelHLRLocation.setText("HLRLocation");
    jLabelHLRLocation.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelHLRLocation.setBounds(new Rectangle(11, 193, 93, 22));
    jTextFieldHLRLocation.setText("");
    jTextFieldHLRLocation.setBounds(new Rectangle(107, 193, 408, 22));
    jLabelMyProxyServer.setText("MyProxyServer");
    jLabelMyProxyServer.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelMyProxyServer.setBounds(new Rectangle(8, 221, 96, 22));
    jTextFieldMyProxyServer.setText("");
    jTextFieldMyProxyServer.setBounds(new Rectangle(107, 221, 408, 22));
    jCheckBoxRetryCount.setHorizontalAlignment(SwingConstants.CENTER);
    jCheckBoxRetryCount.setText("RetryCount");
    jCheckBoxRetryCount.setBounds(new Rectangle(8, 258, 98, 17));
    jCheckBoxRetryCount.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jCheckBoxRetryCountEvent(e);
      }
    });
    jComboBoxSchema.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jComboBoxSchemaEvent(e);
      }
    });
    jScrollPaneRequirements.setBounds(new Rectangle(107, 55, 408, 34));
    jScrollPaneRank.setBounds(new Rectangle(107, 96, 408, 34));
    jScrollPaneRankMPI.setBounds(new Rectangle(107, 137, 408, 34));
    jTextPaneRankMPI.setText("");
    jLabelRankMPI.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelRankMPI.setText("rankMPI");
    jLabelRankMPI.setBounds(new Rectangle(14, 137, 87, 18));
    jLabelRank.setFont(new java.awt.Font("Dialog", 1, 12));
    jLabelSchema.setFont(new java.awt.Font("Dialog", 1, 12));
    jLabelRequirements.setFont(new java.awt.Font("Dialog", 1, 12));
    this.add(jPanelRankRequirementsDefault, null);
    jPanelRankRequirementsDefault.add(jComboBoxSchema, null);
    jPanelRankRequirementsDefault.add(jLabelRankMPI, null);
    jPanelRankRequirementsDefault.add(jLabelRank, null);
    jPanelRankRequirementsDefault.add(jLabelRequirements, null);
    jPanelRankRequirementsDefault.add(jLabelSchema, null);
    jPanelRankRequirementsDefault.add(jScrollPaneRequirements, null);
    jPanelRankRequirementsDefault.add(jScrollPaneRank, null);
    jPanelRankRequirementsDefault.add(jScrollPaneRankMPI, null);
    jPanelRankRequirementsDefault.add(jTextFieldHLRLocation, null);
    jPanelRankRequirementsDefault.add(jCheckBoxRetryCount, null);
    jPanelRankRequirementsDefault.add(jTextFieldRetryCount, null);
    jPanelRankRequirementsDefault.add(downRetryCount, null);
    jPanelRankRequirementsDefault.add(upRetryCount, null);
    jPanelRankRequirementsDefault.add(jTextFieldMyProxyServer, null);
    jPanelRankRequirementsDefault.add(jLabelMyProxyServer, null);
    jPanelRankRequirementsDefault.add(jLabelHLRLocation, null);
    jScrollPaneRankMPI.getViewport().add(jTextPaneRankMPI, null);
    jScrollPaneRank.getViewport().add(jTextPaneRank, null);
    jScrollPaneRequirements.getViewport().add(jTextPaneRequirements, null);
    jLabelSchema.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelSchema.setText("Schema");
    jLabelSchema.setBounds(new Rectangle(45, 23, 56, 19));
    setSize(new Dimension(553, 462));
    setLayout(null);
    jComboBoxSchema.setBounds(new Rectangle(107, 23, 147, 22));
    jLabelRank.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelRank.setText("rank");
    jLabelRank.setBounds(new Rectangle(26, 96, 75, 19));
    jTextPaneRank.setText("");
    jLabelRequirements.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelRequirements.setText("requirements");
    jLabelRequirements.setBounds(new Rectangle(12, 55, 89, 18));
    jTextPaneRequirements.setText("");
    jPanelRankRequirementsDefault.setBorder(BorderFactory.createEtchedBorder());
    jPanelRankRequirementsDefault.setBorder(new TitledBorder(
        new EtchedBorder(), " JDL Defaults ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelRankRequirementsDefault.setBounds(new Rectangle(5, 5, 527, 293));
    jPanelRankRequirementsDefault.setLayout(null);
    setRetryCountEnabled(false);
    //loadDefaultPreferences();
  }

  int jButtonApplyEvent(ActionEvent ae) {
    String selectedSchema = jComboBoxSchema.getSelectedItem().toString().trim();
    String insertedRequirements = jTextPaneRequirements.getText().trim();
    String insertedRank = jTextPaneRank.getText().trim();
    String insertedRankMPI = jTextPaneRankMPI.getText().trim();
    JDialog jDialog = (JDialog) jobSubmitterJFrame
        .getJobSubmitterPreferencesReference();
    String title = "<html><font color=\"#602080\">"
        + JobSubmitterPreferences.JDL_DEFAULTS_PANEL_NAME + ":" + "</font>";
    if (!checkRequirementsRank(insertedRequirements, insertedRank,
        insertedRankMPI, jDialog)) {
      return Utils.FAILED;
    }
    // Add last element (remove it if present before adding).
    logger.debug("selectedSchema: " + selectedSchema);
    for (int i = 0; i < this.srrDefaultVector.size(); i++) {
      logger.debug("srrDefaultVector Schema: "
          + ((SchemaRankRequirementsDefault) this.srrDefaultVector.get(i))
              .getSchema());
      if (((SchemaRankRequirementsDefault) this.srrDefaultVector.get(i))
          .getSchema().equals(selectedSchema)) {
        this.srrDefaultVector.remove(i);
      }
    }
    this.srrDefaultVector.add(new SchemaRankRequirementsDefault(selectedSchema,
        insertedRequirements, insertedRank, insertedRankMPI));
    GUIGlobalVars.srrDefaultVector.add(new SchemaRankRequirementsDefault(
        selectedSchema, insertedRequirements, insertedRank, insertedRankMPI));
    JobAd checkJobAd = new JobAd();
    String errorMsg = "";
    String hlrLocation = jTextFieldHLRLocation.getText().trim();
    try {
      //if (!hlrLocation.equals("")) {
      checkJobAd.setAttribute(Jdl.HLR_LOCATION, hlrLocation);
      //}
    } catch (Exception e) {
      errorMsg += e.getMessage();
    }
    String myProxyServer = jTextFieldMyProxyServer.getText().trim();
    try {
      //if (!myProxyServer.equals("")) {
      checkJobAd.setAttribute(Jdl.MYPROXY, myProxyServer);
      //}
    } catch (Exception e) {
      errorMsg += e.getMessage();
    }
    int retryCount = Integer
        .parseInt(jTextFieldRetryCount.getText().trim(), 10);
    if (jCheckBoxRetryCount.isSelected()) {
      try {
        checkJobAd.setAttribute(Jdl.RETRYCOUNT, retryCount);
      } catch (Exception e) {
        errorMsg += e.getMessage();
      }
    }
    errorMsg = errorMsg.trim();
    if (!errorMsg.equals("")) {
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
      SchemaRankRequirementsDefault schemaRankRequirementsDefault;
      String schema;
      String requirements;
      String rank;
      String rankMPI;
      for (int i = 0; i < this.srrDefaultVector.size(); i++) {
        schemaRankRequirementsDefault = (SchemaRankRequirementsDefault) this.srrDefaultVector
            .get(i);
        schema = schemaRankRequirementsDefault.getSchema();
        requirements = schemaRankRequirementsDefault.getRequirements().trim();
        rank = schemaRankRequirementsDefault.getRank().trim();
        rankMPI = schemaRankRequirementsDefault.getRankMPI().trim();
        if (confAd.hasAttribute(schema)) {
          confAd.delAttribute(schema);
        }
        JobAd schemaAd = new JobAd();
        if (!requirements.equals("")) {
          schemaAd.setAttributeExpr(Jdl.REQUIREMENTS, requirements);
        }
        if (!rank.equals("")) {
          schemaAd.setAttributeExpr(Jdl.RANK, rank);
        }
        if (!rankMPI.equals("")) {
          schemaAd.setAttributeExpr(Utils.GUI_CONF_VAR_RANKMPI, rankMPI);
        }
        if (schemaAd.size() != 0) {
          confAd.setAttribute(schema, schemaAd);
        }
      }
      if (confAd.hasAttribute(Utils.CONF_FILE_HLRLOCATION)) {
        confAd.delAttribute(Utils.CONF_FILE_HLRLOCATION);
      }
      //if (!hlrLocation.equals("")) {
      confAd.setAttribute(Utils.CONF_FILE_HLRLOCATION, hlrLocation);
      //}
      GUIGlobalVars.setHLRLocation(hlrLocation);
      if (confAd.hasAttribute(Utils.CONF_FILE_MYPROXYSERVER)) {
        confAd.delAttribute(Utils.CONF_FILE_MYPROXYSERVER);
      }
      //if (!myProxyServer.equals("")) {
      confAd.setAttribute(Utils.CONF_FILE_MYPROXYSERVER, myProxyServer);
      //}
      GUIGlobalVars.setMyProxyServer(myProxyServer);
      if (confAd.hasAttribute(Utils.GUI_CONF_VAR_RETRY_COUNT)) {
        confAd.delAttribute(Utils.GUI_CONF_VAR_RETRY_COUNT);
      }
      if (jCheckBoxRetryCount.isSelected()) {
        confAd.setAttribute(Utils.GUI_CONF_VAR_RETRY_COUNT, retryCount);
        GUIGlobalVars.setGUIConfVarRetryCount(retryCount);
      } else {
        GUIGlobalVars.setGUIConfVarRetryCount(GUIGlobalVars.NO_RETRY_COUNT);
      }
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    try {
      GUIFileSystem.saveTextFile(userConfFile, confAd.toString(true, true));
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
      JOptionPane.showOptionDialog(jDialog, title
          + "\nUnable to save preferences settings to file:" + "\n"
          + ex.getMessage(), Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return Utils.FAILED;
    }
    return Utils.SUCCESS;
  }

  static Ad setSchemaAttributes(Vector srrDefaultVector, Ad ad) {
    try {
      SchemaRankRequirementsDefault schemaRankRequirementsDefault;
      String schema;
      String requirements;
      String rank;
      String rankMPI;
      for (int i = 0; i < srrDefaultVector.size(); i++) {
        schemaRankRequirementsDefault = (SchemaRankRequirementsDefault) srrDefaultVector
            .get(i);
        schema = schemaRankRequirementsDefault.getSchema();
        requirements = schemaRankRequirementsDefault.getRequirements().trim();
        rank = schemaRankRequirementsDefault.getRank().trim();
        rankMPI = schemaRankRequirementsDefault.getRankMPI().trim();
        if (ad.hasAttribute(schema)) {
          ad.delAttribute(schema);
        }
        JobAd schemaAd = new JobAd();
        if (!requirements.equals("")) {
          schemaAd.setAttributeExpr(Jdl.REQUIREMENTS, requirements);
        }
        if (!rank.equals("")) {
          schemaAd.setAttributeExpr(Jdl.RANK, rank);
        }
        if (!rankMPI.equals("")) {
          schemaAd.setAttributeExpr(Utils.GUI_CONF_VAR_RANKMPI, rankMPI);
        }
        if (schemaAd.size() != 0) {
          ad.setAttribute(schema, schemaAd);
        }
      }
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    return ad;
  }

  void loadPreferencesFromFile() {
    resetAttributeValues();
    Ad userPrefAd = new Ad();
    Ad voConfAd = new Ad();
    Ad guiConfAd = new Ad();
    //Vector nsVector = new Vector();
    File userPrefFile = new File(GUIFileSystem.getUserPrefFile());
    File voConfFile = new File(GUIFileSystem.getGUIConfVOFile());
    File guiConfFile = new File(GUIFileSystem.getGUIConfVarFile());
    if (userPrefFile.isFile()) {
      try {
        userPrefAd.fromFile(userPrefFile.toString());
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
      try {
        if (userPrefAd.hasAttribute(Utils.CONF_FILE_HLRLOCATION)) {
          jTextFieldHLRLocation.setText(userPrefAd.getStringValue(
              Utils.CONF_FILE_HLRLOCATION).get(0).toString().trim());
        }
        if (userPrefAd.hasAttribute(Utils.CONF_FILE_MYPROXYSERVER)) {
          jTextFieldMyProxyServer.setText(userPrefAd.getStringValue(
              Utils.CONF_FILE_MYPROXYSERVER).get(0).toString().trim());
        }
        if (userPrefAd.hasAttribute(Utils.GUI_CONF_VAR_RETRY_COUNT)) {
          setRetryCountEnabled(true);
          jCheckBoxRetryCount.setSelected(true);
          jTextFieldRetryCount.setText(((Integer) userPrefAd.getIntValue(
              Utils.GUI_CONF_VAR_RETRY_COUNT).get(0)).toString());
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
      logger.debug("----- voConfFile: " + voConfFile);
      if (voConfFile.isFile()) {
        try {
          voConfAd.fromFile(voConfFile.toString());
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
        }
      }
      if (guiConfFile.isFile()) {
        try {
          guiConfAd.fromFile(guiConfFile.toString());
          if (!userPrefAd.hasAttribute(Utils.GUI_CONF_VAR_RETRY_COUNT)
              && guiConfAd.hasAttribute(Utils.GUI_CONF_VAR_RETRY_COUNT)) {
            setRetryCountEnabled(true);
            jCheckBoxRetryCount.setSelected(true);
            jTextFieldRetryCount.setText(((Integer) guiConfAd.getIntValue(
                Utils.GUI_CONF_VAR_RETRY_COUNT).get(0)).toString());
          }
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
        }
      }
      loadRequirementsRankFromAd(userPrefAd, voConfAd, guiConfAd);
    }
  }

  boolean checkRequirementsRank(String insertedRequirements,
      String insertedRank, String insertedRankMPI, JDialog jDialog) {
    String title = "<html><font color=\"#602080\">"
        + JobSubmitterPreferences.JDL_DEFAULTS_PANEL_NAME + ":" + "</font>";
    if (!insertedRankMPI.equals("")
        && (insertedRequirements.equals("") || insertedRank.equals(""))) {
      if (insertedRequirements.equals("")) {
        JOptionPane.showOptionDialog(jDialog, title
            + "\nrequirements field cannot be blank", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        jTextPaneRequirements.setText("");
        jTextPaneRequirements.grabFocus();
        jComboBoxSchema.setSelectedItem(this.lastSelectedSchema);
        return false;
      } else {
        JOptionPane.showOptionDialog(jDialog, title
            + "\nrank field cannot be blank", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        jTextPaneRank.setText("");
        jTextPaneRank.grabFocus();
        jComboBoxSchema.setSelectedItem(this.lastSelectedSchema);
        return false;
      }
    } else if (!insertedRequirements.equals("") && insertedRank.equals("")) {
      JOptionPane.showOptionDialog(jDialog, title
          + "\nrank field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextPaneRank.setText("");
      jTextPaneRank.grabFocus();
      jComboBoxSchema.setSelectedItem(this.lastSelectedSchema);
      return false;
    } else if (insertedRequirements.equals("") && !insertedRank.equals("")) {
      JOptionPane.showOptionDialog(jDialog, title
          + "\nrequirements field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextPaneRequirements.setText("");
      jTextPaneRequirements.grabFocus();
      jComboBoxSchema.setSelectedItem(this.lastSelectedSchema);
      return false;
    } else if (!insertedRequirements.equals("") && !insertedRank.equals("")) {
      JobAd checkJobAd = new JobAd();
      try {
        checkJobAd.setAttributeExpr(Jdl.REQUIREMENTS, insertedRequirements);
      } catch (Exception e) {
        JOptionPane.showOptionDialog(jDialog, title + "\n" + e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        jTextPaneRequirements.selectAll();
        jTextPaneRequirements.grabFocus();
        jComboBoxSchema.setSelectedItem(this.lastSelectedSchema);
        return false;
      }
      try {
        checkJobAd.setAttributeExpr(Jdl.RANK, insertedRank);
      } catch (Exception e) {
        JOptionPane.showOptionDialog(jDialog, title + "\n" + e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        jTextPaneRank.selectAll();
        jTextPaneRank.grabFocus();
        jComboBoxSchema.setSelectedItem(this.lastSelectedSchema);
        return false;
      }
      try {
        checkJobAd
            .setAttributeExpr(Utils.GUI_CONF_VAR_RANKMPI, insertedRankMPI);
      } catch (Exception e) {
        JOptionPane.showOptionDialog(jDialog, title + "\n" + e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        jTextPaneRankMPI.selectAll();
        jTextPaneRankMPI.grabFocus();
        jComboBoxSchema.setSelectedItem(this.lastSelectedSchema);
        return false;
      }
    }
    return true;
  }

  static boolean isCheckRequirementsRankOk(String insertedRequirements,
      String insertedRank, String insertedRankMPI) {
    String title = "<html><font color=\"#602080\">"
        + JobSubmitterPreferences.JDL_DEFAULTS_PANEL_NAME + ":" + "</font>";
    if (!insertedRankMPI.equals("")
        && (insertedRequirements.equals("") || insertedRank.equals(""))) {
      if (insertedRequirements.equals("")) {
        return false;
      } else {
        return false;
      }
    } else if (!insertedRequirements.equals("") && insertedRank.equals("")) {
      return false;
    } else if (insertedRequirements.equals("") && !insertedRank.equals("")) {
      return false;
    } else if (!insertedRequirements.equals("") && !insertedRank.equals("")) {
      JobAd checkJobAd = new JobAd();
      try {
        checkJobAd.setAttributeExpr(Jdl.REQUIREMENTS, insertedRequirements);
      } catch (Exception e) {
        return false;
      }
      try {
        checkJobAd.setAttributeExpr(Jdl.RANK, insertedRank);
      } catch (Exception e) {
        return false;
      }
      try {
        checkJobAd
            .setAttributeExpr(Utils.GUI_CONF_VAR_RANKMPI, insertedRankMPI);
      } catch (Exception e) {
        return false;
      }
    }
    return true;
  }

  void jComboBoxSchemaEvent(ActionEvent ae) {
    String selectedSchema = jComboBoxSchema.getSelectedItem().toString().trim();
    if (!selectedSchema.equals(this.lastSelectedSchema)) {
      String insertedRequirements = jTextPaneRequirements.getText().trim();
      String insertedRank = jTextPaneRank.getText().trim();
      String insertedRankMPI = jTextPaneRankMPI.getText().trim();
      JDialog jDialog = (JDialog) jobSubmitterJFrame
          .getJobSubmitterPreferencesReference();
      if (!checkRequirementsRank(insertedRequirements, insertedRank,
          insertedRankMPI, jDialog)) {
        return;
      }
      SchemaRankRequirementsDefault schemaRankRequirementsDefault = new SchemaRankRequirementsDefault();
      String requirements = "";
      String rank = "";
      String rankMPI = "";
      boolean isPresent = false;
      for (int i = 0; i < srrDefaultVector.size(); i++) {
        schemaRankRequirementsDefault = (SchemaRankRequirementsDefault) this.srrDefaultVector
            .get(i);
        if (selectedSchema.equals(schemaRankRequirementsDefault.getSchema())) {
          requirements = schemaRankRequirementsDefault.getRequirements();
          rank = schemaRankRequirementsDefault.getRank();
          rankMPI = schemaRankRequirementsDefault.getRankMPI();
          break;
        }
      }
      for (int i = 0; i < srrDefaultVector.size(); i++) {
        schemaRankRequirementsDefault = (SchemaRankRequirementsDefault) this.srrDefaultVector
            .get(i);
        if (this.lastSelectedSchema.equals(schemaRankRequirementsDefault
            .getSchema())) {
          if (!insertedRequirements.equals("") && !insertedRank.equals("")) {
            schemaRankRequirementsDefault.setRequirements(insertedRequirements);
            schemaRankRequirementsDefault.setRank(insertedRank);
            schemaRankRequirementsDefault.setRankMPI(insertedRankMPI);
          } else {
            this.srrDefaultVector.remove(schemaRankRequirementsDefault);
          }
          isPresent = true;
          break;
        }
      }
      if (!isPresent && !insertedRequirements.equals("")
          && !insertedRank.equals("")) {
        this.srrDefaultVector.add(new SchemaRankRequirementsDefault(
            this.lastSelectedSchema, insertedRequirements, insertedRank,
            insertedRankMPI));
      }
      jTextPaneRequirements.setText(requirements);
      jTextPaneRank.setText(rank);
      jTextPaneRankMPI.setText(rankMPI);
      this.lastSelectedSchema = selectedSchema;
      logger.info("\nVector:\n" + this.srrDefaultVector);
    }
  }

  void setRetryCountEnabled(boolean bool) {
    jTextFieldRetryCount.setEnabled(bool);
    upRetryCount.setEnabled(bool);
    downRetryCount.setEnabled(bool);
  }

  void jCheckBoxRetryCountEvent(ActionEvent e) {
    setRetryCountEnabled(jCheckBoxRetryCount.isSelected());
  }

  void resetAttributeValues() {
    this.srrDefaultVector.clear();
    jTextPaneRequirements.setText("");
    jTextPaneRank.setText("");
    jTextPaneRankMPI.setText("");
    jTextFieldHLRLocation.setText("");
    jTextFieldMyProxyServer.setText("");
    jTextFieldRetryCount.setText(Integer.toString(Utils.RETRYCOUNT_DEF_VAL));
    jCheckBoxRetryCount.setSelected(false);
    setRetryCountEnabled(false);
  }

  void loadDefaultPreferences() {
    resetAttributeValues();
    this.srrDefaultVector.clear();
    Ad guiConfAd = new Ad();
    Ad voConfAd = new Ad();
    File guiConfFile = new File(GUIFileSystem.getGUIConfVarFile());
    File voConfFile = new File(GUIGlobalVars.envVOConfFile);
    //File voConfFile = new File(Utils.getGUIConfVOFile());
    if (guiConfFile.isFile()) {
      try {
        guiConfAd.fromFile(guiConfFile.toString());
        if (guiConfAd.hasAttribute(Utils.GUI_CONF_VAR_RETRY_COUNT)) {
          setRetryCountEnabled(true);
          jCheckBoxRetryCount.setSelected(true);
          jTextFieldRetryCount.setText(((Integer) guiConfAd.getIntValue(
              Utils.GUI_CONF_VAR_RETRY_COUNT).get(0)).toString());
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
    }
    if (voConfFile.isFile()) {
      try {
        voConfAd.fromFile(voConfFile.toString());
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
      try {
        if (voConfAd.hasAttribute(Utils.CONF_FILE_HLRLOCATION)) {
          jTextFieldHLRLocation.setText(voConfAd.getStringValue(
              Utils.CONF_FILE_HLRLOCATION).get(0).toString().trim());
        }
        if (voConfAd.hasAttribute(Utils.CONF_FILE_MYPROXYSERVER)) {
          jTextFieldMyProxyServer.setText(voConfAd.getStringValue(
              Utils.CONF_FILE_MYPROXYSERVER).get(0).toString().trim());
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
    }
    loadRequirementsRankFromAd(null, voConfAd, guiConfAd);
  }

  static Vector getRequirementsRankVector(Ad userPrefAd, Ad voConfAd,
      Ad guiConfAd) {
    Vector srrDefaultVector = new Vector();
    String errorMsg = "";
    if (userPrefAd != null) {
      for (int i = 0; i < Utils.jdleSchemaArray.length; i++) {
        if (userPrefAd.hasAttribute(Utils.jdleSchemaArray[i])) {
          Ad schemaAd = new Ad();
          try {
            schemaAd = userPrefAd.getAd(Utils.jdleSchemaArray[i]);
          } catch (Exception ex) {
            // Schema not present. Do nothing.
            continue;
          }
          Object requirements = schemaAd.lookup(Jdl.REQUIREMENTS);
          Object rank = schemaAd.lookup(Jdl.RANK);
          Object rankMPI = schemaAd.lookup(Utils.GUI_CONF_VAR_RANKMPI);
          if ((requirements == null)
              || (rank == null)
              || (rankMPI == null)
              || !JDLDefaultsPreferencesPanel.isCheckRequirementsRankOk(
                  requirements.toString().trim(), rank.toString().trim(),
                  rankMPI.toString().trim())) {
            continue;
          }
          srrDefaultVector.add(new SchemaRankRequirementsDefault(
              Utils.jdleSchemaArray[i], requirements.toString().trim(), rank
                  .toString().trim(), rankMPI.toString().trim()));
        }
      }
    }
    if (voConfAd != null) {
      Object requirements = voConfAd.lookup(Jdl.REQUIREMENTS);
      Object rank = voConfAd.lookup(Jdl.RANK);
      Object rankMPI = voConfAd.lookup(Utils.GUI_CONF_VAR_RANKMPI);
      if ((requirements == null)
          || (rank == null)
          || (rankMPI == null)
          || !JDLDefaultsPreferencesPanel.isCheckRequirementsRankOk(
              requirements.toString().trim(), rank.toString().trim(), rankMPI
                  .toString().trim())) {
        if (!((requirements == null) && (rank == null) && (rankMPI == null))) {
          errorMsg += "- Unable to get default values for "
              + Utils.DEFAULT_INFORMATION_SCHEMA + " schema "
              + "inside VO configuration file";
        }
      } else {
        if (!SchemaRankRequirementsDefault.containsSchema(srrDefaultVector,
            Utils.DEFAULT_INFORMATION_SCHEMA)) {
          srrDefaultVector.add(new SchemaRankRequirementsDefault(
              Utils.DEFAULT_INFORMATION_SCHEMA, requirements.toString().trim(),
              rank.toString().trim(), rankMPI.toString().trim()));
        }
      }
    }
    if (guiConfAd != null) {
      for (int i = 0; i < Utils.jdleSchemaArray.length; i++) {
        if (!SchemaRankRequirementsDefault.containsSchema(srrDefaultVector,
            Utils.jdleSchemaArray[i])
            && guiConfAd.hasAttribute(Utils.jdleSchemaArray[i])) {
          Ad schemaAd = new Ad();
          try {
            schemaAd = guiConfAd.getAd(Utils.jdleSchemaArray[i]);
          } catch (Exception ex) {
            // Schema not present. Do nothing.
            continue;
          }
          logger.debug("----- schemaAd: " + schemaAd);
          Object requirements = schemaAd.lookup(Jdl.REQUIREMENTS);
          Object rank = schemaAd.lookup(Jdl.RANK);
          Object rankMPI = schemaAd.lookup(Utils.GUI_CONF_VAR_RANKMPI);
          if ((requirements == null)
              || (rank == null)
              || (rankMPI == null)
              || !JDLDefaultsPreferencesPanel.isCheckRequirementsRankOk(
                  requirements.toString().trim(), rank.toString().trim(),
                  rankMPI.toString().trim())) {
            errorMsg += "- Unable to get default values for "
                + Utils.jdleSchemaArray[i] + " schema "
                + "inside GUI configuration file";
            continue;
          }
          srrDefaultVector.add(new SchemaRankRequirementsDefault(
              Utils.jdleSchemaArray[i], requirements.toString().trim(), rank
                  .toString().trim(), rankMPI.toString().trim()));
        }
      }
    }
    logger.debug("srrDefaultVector: " + srrDefaultVector + "\nerrorMsg: "
        + errorMsg);
    Vector returnVector = new Vector();
    returnVector.add(srrDefaultVector);
    returnVector.add(errorMsg.trim());
    return returnVector;
  }

  void loadRequirementsRankFromAd(Ad userPrefAd, Ad voConfAd, Ad guiConfAd) {
    Vector returnVector = getRequirementsRankVector(userPrefAd, voConfAd,
        guiConfAd);
    this.srrDefaultVector = (Vector) returnVector.get(0);
    String errorMsg = returnVector.get(1).toString();
    if (!errorMsg.equals("")) {
      JOptionPane.showOptionDialog(JDLDefaultsPreferencesPanel.this,
          "Some problems occurs reading Requirement, "
              + "Rank and RankMPI values from configuration files:\n"
              + errorMsg, Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.WARNING_MESSAGE, null, null, null);
    }
    logger.debug("this.srrDefaultVector: " + this.srrDefaultVector);
    SchemaRankRequirementsDefault srrDefault;
    String selectedSchema = jComboBoxSchema.getSelectedItem().toString();
    for (int i = 0; i < this.srrDefaultVector.size(); i++) {
      srrDefault = (SchemaRankRequirementsDefault) this.srrDefaultVector.get(i);
      if ((srrDefault).getSchema().equals(selectedSchema)) {
        jTextPaneRequirements.setText(srrDefault.getRequirements());
        jTextPaneRank.setText(srrDefault.getRank());
        jTextPaneRankMPI.setText(srrDefault.getRankMPI());
        break;
      }
    }
  }
}

class SchemaRankRequirementsDefault {
  String schema;

  String requirements;

  String rank;

  String rankMPI;

  public SchemaRankRequirementsDefault() {
    this("", "", "", "");
  }

  public SchemaRankRequirementsDefault(String schema, String requirements,
      String rank, String rankMPI) {
    if (schema == null) {
      schema = "";
    }
    if (requirements == null) {
      requirements = "";
    }
    if (rank == null) {
      rank = "";
    }
    if (rankMPI == null) {
      rankMPI = "";
    }
    this.schema = schema;
    this.requirements = requirements;
    this.rank = rank;
    this.rankMPI = rankMPI;
  }

  void setSchema(String schema) {
    if (schema == null) {
      this.schema = "";
    } else {
      this.schema = schema;
    }
  }

  String getSchema() {
    return this.schema;
  }

  void setRank(String rank) {
    if (rank == null) {
      this.rank = "";
    } else {
      this.rank = rank;
    }
  }

  String getRank() {
    return this.rank;
  }

  void setRankMPI(String rankMPI) {
    if (rank == null) {
      this.rankMPI = "";
    } else {
      this.rankMPI = rankMPI;
    }
  }

  String getRankMPI() {
    return this.rankMPI;
  }

  void setRequirements(String requirements) {
    if (requirements == null) {
      this.requirements = "";
    } else {
      this.requirements = requirements;
    }
  }

  String getRequirements() {
    return this.requirements;
  }

  static boolean containsSchema(Vector schemaVector, String schema) {
    SchemaRankRequirementsDefault schemaInstance;
    for (int i = 0; i < schemaVector.size(); i++) {
      schemaInstance = (SchemaRankRequirementsDefault) schemaVector.get(i);
      if (schemaInstance.getSchema().equals(schema)) {
        return true;
      }
    }
    return false;
  }
}