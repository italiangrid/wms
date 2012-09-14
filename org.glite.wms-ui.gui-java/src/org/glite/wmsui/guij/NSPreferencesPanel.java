/*
 * NSPreferencesPanel.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.awt.AWTEvent;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.util.Iterator;
import java.util.Map;
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Ad;
import condor.classad.Constant;
import condor.classad.ListExpr;

/**
 * Implementation of the NSPreferencesPanel class.
 * This class implements the main part of the Job Submitter application
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class NSPreferencesPanel extends JPanel {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int NS_NAME_COLUMN_INDEX = 0;

  static final String NS_NAME_TABLE_HEADER = "NS Name";

  static final int NS_ADDRESS_COLUMN_INDEX = 1;

  static final String NS_ADDRESS_TABLE_HEADER = "NS Address";

  static final int JDLE_SCHEMA_COLUMN_INDEX = 2;

  static final String JDLE_SCHEMA_TABLE_HEADER = "Information Service Schema";

  JPanel jPanelNS = new JPanel();

  JTextField jTextFieldNSAddress = new JTextField();

  JTextField jTextFieldNSPort = new JTextField();

  JLabel jLabelNSAddress = new JLabel();

  JLabel jLabelNSPort = new JLabel();

  JScrollPane jScrollPaneNSTable = new JScrollPane();

  JTable jTableNS;

  JobTableModel jobTableModel;

  Vector vectorHeader = new Vector();

  JButton jButtonAdd = new JButton();

  JButton jButtonRemove = new JButton();

  JButton jButtonAll = new JButton();

  JButton jButtonNone = new JButton();

  JButton jButtonClear = new JButton();

  JButton jButtonClearTable = new JButton();

  JButton jButtonEdit = new JButton();

  JobSubmitter jobSubmitterJFrame;

  JTextField jTextFieldNSName = new JTextField(8);

  JComboBox jComboBoxJDLESchema = new JComboBox();

  JLabel jLabelNSName = new JLabel();

  JLabel jLabelJDLESchema = new JLabel();

  /**
   * Constructor.
   */
  public NSPreferencesPanel(Component component) {
    //super((JobSubmitter) component);
    if (component instanceof JobSubmitter) {
      this.jobSubmitterJFrame = (JobSubmitter) component;
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
    vectorHeader.addElement(NS_NAME_TABLE_HEADER);
    vectorHeader.addElement(NS_ADDRESS_TABLE_HEADER);
    vectorHeader.addElement(JDLE_SCHEMA_TABLE_HEADER);
    for (int i = 0; i < Utils.jdleSchemaArray.length; i++) {
      jComboBoxJDLESchema.addItem(Utils.jdleSchemaArray[i]);
    }
    jobTableModel = new JobTableModel(vectorHeader, 0);
    jTableNS = new JTable(jobTableModel);
    jTableNS.getTableHeader().setReorderingAllowed(false);
    setSize(new Dimension(550, 460));
    setLayout(null);
    jPanelNS.setBorder(new TitledBorder(new EtchedBorder(),
        " NS Configuration ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelNS.setBounds(new Rectangle(5, 5, 527, 305));
    jPanelNS.setLayout(null);
    jTextFieldNSAddress.setBounds(new Rectangle(88, 51, 338, 21));
    jTextFieldNSPort.setBounds(new Rectangle(464, 51, 53, 21));
    jTextFieldNSPort.setText(Utils.LB_DEFAULT_PORT);
    jLabelNSAddress.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelNSAddress.setText("NS Address");
    jLabelNSAddress.setBounds(new Rectangle(13, 51, 72, 21));
    jLabelNSPort.setBounds(new Rectangle(427, 51, 34, 21));
    jLabelNSPort.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelNSPort.setText("Port");
    jScrollPaneNSTable.getViewport().setBackground(Color.white);
    jScrollPaneNSTable.setBorder(BorderFactory.createEtchedBorder());
    jScrollPaneNSTable.setBounds(new Rectangle(12, 124, 505, 131));
    jButtonAdd.setText("Add");
    jButtonAdd.setBounds(new Rectangle(434, 80, 83, 26));
    jButtonAdd.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonAddEvent(e);
      }
    });
    jButtonRemove.setBounds(new Rectangle(434, 265, 83, 26));
    jButtonRemove.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonRemoveEvent(e);
      }
    });
    jButtonRemove.setText("Remove");
    jButtonAll.setText("All");
    jButtonAll.setBounds(new Rectangle(12, 265, 83, 26));
    jButtonAll.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonAllEvent(e);
      }
    });
    jButtonNone.setBounds(new Rectangle(101, 265, 83, 26));
    jButtonNone.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonNoneEvent(e);
      }
    });
    jButtonNone.setText("None");
    jButtonClear.setBounds(new Rectangle(255, 80, 83, 26));
    jButtonClear.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonClearEvent(e);
      }
    });
    jButtonClear.setText("Clear");
    jButtonClearTable.setText("Clear");
    jButtonClearTable.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonClearTableEvent(e);
      }
    });
    jButtonClearTable.setBounds(new Rectangle(344, 265, 83, 26));
    jButtonEdit.setText("Replace");
    jButtonEdit.setBounds(new Rectangle(345, 80, 83, 26));
    jButtonEdit.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonReplaceEvent(e);
      }
    });
    jTextFieldNSName.setBounds(new Rectangle(88, 22, 146, 21));
    jTextFieldNSName.addKeyListener(new java.awt.event.KeyAdapter() {
      public void keyPressed(KeyEvent e) {
        jTextFieldNSNameKeyPressed(e);
      }
    });
    jComboBoxJDLESchema.setBounds(new Rectangle(417, 22, 100, 21));
    jLabelNSName.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelNSName.setText("NS Name");
    jLabelNSName.setBounds(new Rectangle(17, 22, 68, 21));
    jLabelJDLESchema.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelJDLESchema.setText("Information Service Schema");
    jLabelJDLESchema.setBounds(new Rectangle(235, 22, 174, 21));
    jPanelNS.add(jTextFieldNSName, null);
    jPanelNS.add(jLabelJDLESchema, null);
    jPanelNS.add(jComboBoxJDLESchema, null);
    jPanelNS.add(jLabelNSPort, null);
    jPanelNS.add(jTextFieldNSPort, null);
    jPanelNS.add(jTextFieldNSAddress, null);
    jPanelNS.add(jButtonAdd, null);
    jPanelNS.add(jButtonEdit, null);
    jPanelNS.add(jButtonClear, null);
    jPanelNS.add(jScrollPaneNSTable, null);
    jPanelNS.add(jButtonRemove, null);
    jPanelNS.add(jButtonAll, null);
    jPanelNS.add(jButtonClearTable, null);
    jPanelNS.add(jButtonNone, null);
    jPanelNS.add(jLabelNSAddress, null);
    jPanelNS.add(jLabelNSName, null);
    jScrollPaneNSTable.getViewport().add(jTableNS, null);
    this.add(jPanelNS, null);
    jTableNS.addMouseListener(new MouseAdapter() {
      public void mouseClicked(MouseEvent me) {
        if (me.getClickCount() == 2) {
          Point point = me.getPoint();
          int row = jTableNS.rowAtPoint(point);
          int column = jTableNS.columnAtPoint(point);
          String nsName = jobTableModel.getValueAt(row, NS_NAME_COLUMN_INDEX)
              .toString().trim();
          String addressPort = jobTableModel.getValueAt(row,
              NS_ADDRESS_COLUMN_INDEX).toString().trim();
          String nsSchema = jobTableModel.getValueAt(row,
              JDLE_SCHEMA_COLUMN_INDEX).toString().trim();
          int index = addressPort.lastIndexOf(":");
          String address = addressPort;
          String port = Utils.NS_DEFAULT_PORT;
          if (index != -1) {
            address = addressPort.substring(0, index);
            port = addressPort.substring(index + 1);
          }
          jTextFieldNSName.setText(nsName);
          jTextFieldNSAddress.setText(address);
          jTextFieldNSPort.setText(port);
          jComboBoxJDLESchema.setSelectedItem(nsSchema);
        }
      }
    });
    jTextFieldNSName.grabFocus();
  }

  void jButtonClearEvent(ActionEvent e) {
    jTextFieldNSName.setText("");
    if (jComboBoxJDLESchema.getItemCount() != 0) {
      jComboBoxJDLESchema.setSelectedIndex(0);
    }
    jTextFieldNSAddress.setText("");
    jTextFieldNSPort.setText("");
    jTextFieldNSAddress.grabFocus();
  }

  void jButtonAddEvent(ActionEvent e) {
    String insertedNSName = jTextFieldNSName.getText().trim();
    String insertedNSAddress = jTextFieldNSAddress.getText().trim();
    String insertedNSPort = jTextFieldNSPort.getText().trim();
    String insertedNSSchema = jComboBoxJDLESchema.getSelectedItem().toString();
    if (insertedNSName.equals("")) {
      JOptionPane.showOptionDialog(NSPreferencesPanel.this,
          "NS Name field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldNSName.setText("");
      jTextFieldNSName.grabFocus();
      return;
    }
    if (insertedNSAddress.equals("")) {
      JOptionPane.showOptionDialog(NSPreferencesPanel.this,
          "NS Address field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldNSAddress.setText("");
      jTextFieldNSAddress.grabFocus();
      return;
    }
    if (insertedNSPort.equals("")) {
      insertedNSPort = Utils.LB_DEFAULT_PORT.trim();
      if (insertedNSPort.equals("")) {
        JOptionPane.showOptionDialog(NSPreferencesPanel.this,
            "NS Port cannot be blank", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        jTextFieldNSPort.grabFocus();
        jTextFieldNSPort.selectAll();
        return;
      }
    } else if ((Utils.getValueType(insertedNSPort) != Utils.INTEGER)
        || (Integer.parseInt(insertedNSPort, 10) < 1024)) {
      JOptionPane.showOptionDialog(NSPreferencesPanel.this,
          "NS Port must be an int value greater than 1023",
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      jTextFieldNSPort.grabFocus();
      jTextFieldNSPort.selectAll();
      return;
    }
    String addressPort = insertedNSAddress + ":" + insertedNSPort;
    if (jobTableModel.isElementPresentInColumnCi(insertedNSName,
        NS_NAME_COLUMN_INDEX) == true) {
      JOptionPane.showOptionDialog(NSPreferencesPanel.this,
          "Inserted NS Name is already present", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldNSName.grabFocus();
      jTextFieldNSName.selectAll();
      return;
    }
    Vector rowElement = new Vector();
    rowElement.addElement(insertedNSName);
    rowElement.addElement(insertedNSAddress + ":" + insertedNSPort);
    rowElement.addElement(insertedNSSchema);
    jobTableModel.addRow(rowElement);
    LBPreferencesPanel lbPreferencesPanel = jobSubmitterJFrame
        .getJobSubmitterPreferencesReference().getLBPreferencesPanelReference();
    lbPreferencesPanel.addNSAddress(insertedNSAddress + ":" + insertedNSPort);
    jTextFieldNSName.selectAll();
    jTextFieldNSName.grabFocus();
  }

  void jButtonReplaceEvent(ActionEvent ae) {
    if (jTableNS.getSelectedRowCount() == 0) {
      JOptionPane.showOptionDialog(NSPreferencesPanel.this,
          "Please first select from table a NS to replace",
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
      return;
    } else if (jTableNS.getSelectedRowCount() != 1) {
      JOptionPane.showOptionDialog(NSPreferencesPanel.this,
          "Please select from table a single NS to replace",
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
      return;
    }
    int selectedRow = jTableNS.getSelectedRow();
    String insertedNSName = jTextFieldNSName.getText().trim();
    String insertedNSAddress = jTextFieldNSAddress.getText().trim();
    String insertedNSPort = jTextFieldNSPort.getText().trim();
    String insertedNSSchema = jComboBoxJDLESchema.getSelectedItem().toString();
    String addressPort = insertedNSAddress + ":" + insertedNSPort;
    if (insertedNSName.equals("")) {
      JOptionPane.showOptionDialog(NSPreferencesPanel.this,
          "NS Name field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldNSName.setText("");
      jTextFieldNSName.grabFocus();
      return;
    }
    if (insertedNSAddress.equals("")) {
      JOptionPane.showOptionDialog(NSPreferencesPanel.this,
          "NS Address field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldNSAddress.setText("");
      jTextFieldNSAddress.grabFocus();
      return;
    }
    if (insertedNSPort.equals("")) {
      insertedNSPort = Utils.NS_DEFAULT_PORT;
    } else if (Utils.getValueType(insertedNSPort) != Utils.INTEGER) {
      JOptionPane.showOptionDialog(NSPreferencesPanel.this,
          "NS Port must be an int value", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldNSPort.grabFocus();
      jTextFieldNSPort.selectAll();
      return;
    }
    String selectedNSName = jobTableModel.getValueAt(selectedRow,
        NS_NAME_COLUMN_INDEX).toString().trim();
    String selectedNSAddress = jobTableModel.getValueAt(selectedRow,
        NS_ADDRESS_COLUMN_INDEX).toString().trim();
    String selectedNSSchema = jobTableModel.getValueAt(selectedRow,
        JDLE_SCHEMA_COLUMN_INDEX).toString().trim();
    if ((!selectedNSName.equals(insertedNSName))
        && (jobTableModel.isElementPresentInColumnCi(insertedNSName,
            NS_NAME_COLUMN_INDEX) == true)) {
      JOptionPane.showOptionDialog(NSPreferencesPanel.this,
          "Inserted NS Name is already present", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldNSName.grabFocus();
      jTextFieldNSName.selectAll();
      return;
    }
    NSPanel nsPanel = null;
    int tabIndex = jobSubmitterJFrame.jTabbedPaneRB.indexOfTab(selectedNSName);
    if (tabIndex != -1) {
      nsPanel = (NSPanel) jobSubmitterJFrame.jTabbedPaneRB
          .getComponentAt(tabIndex);
    }
    if ((nsPanel != null) && nsPanel.hasSubmittedJobs()) {
      if (!addressPort.equals(selectedNSAddress)
          || !insertedNSSchema.equals(selectedNSSchema)) {
        JOptionPane.showOptionDialog(NSPreferencesPanel.this, "Selected NS \""
            + selectedNSName
            + "\" has submitted job(s)\nYou can replace NS Name only",
            Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE, null, null, null);
        return;
      }
    }
    //!!!NetworkServer ns = (NetworkServer) GUIGlobalVars.nsMap.get(selectedNSName);
    if (!insertedNSName.equals(selectedNSName)) {
      //!!! Null check
      if (nsPanel != null) {
        nsPanel.setName(insertedNSName);
        //!!!ns.setName(insertedNSName);
      }
      File directory = new File(GUIFileSystem.getJobTemporaryFileDirectory()
          + File.separator + selectedNSName);
      directory.renameTo(new File(GUIFileSystem.getJobTemporaryFileDirectory()
          + File.separator + insertedNSName));
      jobTableModel.setValueAt(insertedNSName, selectedRow,
          NS_NAME_COLUMN_INDEX);
      Vector openedEditorVector = GUIGlobalVars
          .getNSPanelOpenedEditorKeyVector(selectedNSName);
      JDLEditor editor;
      String title;
      String nsName;
      String keyJobName;
      int index = -1;
      for (int i = 0; i < openedEditorVector.size(); i++) {
        editor = (JDLEditor) GUIGlobalVars.openedEditorHashMap
            .get(openedEditorVector.get(i));
        title = editor.getTitle();
        index = title.lastIndexOf("-");
        keyJobName = title.substring(index + 1).trim();
        editor.setTitle("JDL Editor - " + insertedNSName + " - " + keyJobName);
        editor.rBName = insertedNSName;
        GUIGlobalVars.openedEditorHashMap.remove(selectedNSName + " - "
            + keyJobName);
        GUIGlobalVars.openedEditorHashMap.put(insertedNSName + " - "
            + keyJobName, editor);
      }
    }
    //!!!ns.setAddress(insertedNSAddress);
    //!!!ns.setJDLESchema(insertedNSSchema);
    //!!!GUIGlobalVars.nsMap.remove(selectedNSName);
    //!!!GUIGlobalVars.nsMap.put(insertedNSName, ns);
    jobTableModel.setValueAt(insertedNSAddress + ":" + insertedNSPort,
        selectedRow, NS_ADDRESS_COLUMN_INDEX);
    jobTableModel.setValueAt(insertedNSSchema, selectedRow,
        JDLE_SCHEMA_COLUMN_INDEX);
    LBPreferencesPanel lbPreferencesPanel = jobSubmitterJFrame
        .getJobSubmitterPreferencesReference().getLBPreferencesPanelReference();
    lbPreferencesPanel.replaceNSAddress(selectedNSAddress, insertedNSAddress
        + ":" + insertedNSPort);
    jTextFieldNSName.selectAll();
    jTextFieldNSName.grabFocus();
  }

  private void removeRows(int[] selectedRow) {
    NSPanel rbPanel = null;
    int selectedRowCount = selectedRow.length;
    LBPreferencesPanel lbPreferencesPanel = jobSubmitterJFrame
        .getJobSubmitterPreferencesReference().getLBPreferencesPanelReference();
    String currentNSName = "";
    String currentNSAddress = "";
    for (int i = selectedRowCount - 1; i >= 0; i--) {
      currentNSName = jobTableModel.getValueAt(selectedRow[i],
          NS_NAME_COLUMN_INDEX).toString().trim();
      currentNSAddress = jobTableModel.getValueAt(selectedRow[i],
          NS_ADDRESS_COLUMN_INDEX).toString().trim();
      int choice = 0;
      int index = jobSubmitterJFrame.jTabbedPaneRB.indexOfTab(currentNSName);
      if (index != -1) {
        rbPanel = (NSPanel) jobSubmitterJFrame.jTabbedPaneRB
            .getComponentAt(index);
        if (rbPanel.hasSubmittedJobs() || rbPanel.hasJobs()) {
          String submitted = "";
          if (rbPanel.hasSubmittedJobs()) {
            submitted = " submitted";
          }
          choice = JOptionPane.showOptionDialog(NSPreferencesPanel.this, "NS '"
              + currentNSName + "' contains" + submitted
              + " job(s)\nRemove anyway?", Utils.WARNING_MSG_TXT,
              JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null,
              null, null);
        }
      }
      if (choice == 0) {
        jobTableModel.removeRow(selectedRow[i]);
        lbPreferencesPanel.removeNSAddress(currentNSAddress);
        if (jobTableModel.getRowCount() != 0) {
          int selectableRow = selectedRow[selectedRowCount - 1] + 1
              - selectedRowCount; // Next.
          if (selectableRow > jobTableModel.getRowCount() - 1) {
            selectableRow--; // Prev. (selectedRow[selectedRowCount - 1] - selectedRowCount).
          }
          jTableNS.setRowSelectionInterval(selectableRow, selectableRow);
        }
      }
    }
  }

  void jButtonRemoveEvent(ActionEvent e) {
    int[] selectedRows = jTableNS.getSelectedRows();
    if (selectedRows.length != 0) {
      removeRows(selectedRows);
    } else {
      JOptionPane.showOptionDialog(NSPreferencesPanel.this,
          Utils.SELECT_AN_ITEM, Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
          null, null);
    }
  }

  void jButtonClearTableEvent(ActionEvent e) {
    int choice = JOptionPane.showOptionDialog(NSPreferencesPanel.this,
        "Do you really want to clear NS table?", "Confirm Clear",
        JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null,
        null);
    if (choice == 0) {
      //!!! selectAll?
      jTableNS.selectAll();
      removeRows(jTableNS.getSelectedRows());
      LBPreferencesPanel lbPreferencesPanel = jobSubmitterJFrame
          .getJobSubmitterPreferencesReference()
          .getLBPreferencesPanelReference();
      lbPreferencesPanel.removeAllNSAddress();
    }
  }

  void jButtonAllEvent(ActionEvent e) {
    jTableNS.selectAll();
  }

  void jButtonNoneEvent(ActionEvent e) {
    jTableNS.clearSelection();
  }

  void loadDefaultPreferences() {
    Vector nsAddressesVector = GUIGlobalVars.getNSVector();
    jobTableModel.removeAllRows();
    if (nsAddressesVector.size() != 0) {
      for (int i = 0; i < nsAddressesVector.size(); i++) {
        Vector rowToAdd = new Vector();
        rowToAdd.add(Utils.DEFAULT_NS_NAME + Integer.toString(i + 1));
        rowToAdd.add(nsAddressesVector.get(i).toString());
        rowToAdd.add(Utils.DEFAULT_INFORMATION_SCHEMA);
        jobTableModel.addRow(rowToAdd);
      }
    }
  }

  static Ad setNSLBMapAttribute(Ad ad, Map map, JobTableModel jobTableModel)
      throws Exception {
    Vector vectorToSave = new Vector();
    Vector nsVector = new Vector();
    Vector lbVector;
    String nsAddress;
    String list;
    for (int j = 0; j < jobTableModel.getRowCount(); j++) {
      list = "";
      nsAddress = jobTableModel.getValueAt(j, NS_ADDRESS_COLUMN_INDEX)
          .toString();
      nsVector.add(Constant.getInstance(nsAddress));
      lbVector = (Vector) map.get(nsAddress);
      if (lbVector.size() >= 1) {
        list += "{";
        if (!lbVector.get(0).toString().equals("")) {
          list += "\"" + lbVector.get(0).toString() + "\"";
        }
        for (int i = 1; i < lbVector.size(); i++) {
          if (!lbVector.get(i).toString().equals("")) {
            list += ",\"" + lbVector.get(i).toString() + "\"";
          }
        }
        list += "}";
        vectorToSave.add(list);
      }
    }
    try {
      if (ad.hasAttribute(Utils.PREF_FILE_NS_ADDRESS)) {
        ad.delAttribute(Utils.PREF_FILE_NS_ADDRESS);
      }
      if (nsVector.size() != 0) {
        ad.setAttribute(Utils.PREF_FILE_NS_ADDRESS, new ListExpr(nsVector));
      }
      if (ad.hasAttribute(Utils.PREF_FILE_LB_ADDRESS)) {
        ad.delAttribute(Utils.PREF_FILE_LB_ADDRESS);
      }
      if (vectorToSave.size() != 0) {
        ad.setAttribute(Utils.PREF_FILE_LB_ADDRESS, new ListExpr(vectorToSave));
      }
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      throw e;
    }
    return ad;
  }

  void loadPreferencesFromFile() {
    Ad userConfAd = new Ad();
    Ad userConfJobSubmitterAd = new Ad();
    Vector nsNameVector = new Vector();
    Vector nsAddressVector = new Vector();
    Vector nsJDLESchemaVector = new Vector();
    File userConfFile = new File(GUIFileSystem.getUserPrefFile());
    jobTableModel.removeAllRows();
    if (userConfFile.isFile()) {
      try {
        userConfAd.fromFile(userConfFile.toString());
        if (userConfAd.hasAttribute(Utils.PREF_FILE_JOB_SUBMITTER)) {
          userConfJobSubmitterAd = userConfAd
              .getAd(Utils.PREF_FILE_JOB_SUBMITTER);
          if (userConfJobSubmitterAd.hasAttribute(Utils.PREF_FILE_NS_NAME)) {
            nsNameVector = userConfJobSubmitterAd
                .getStringValue(Utils.PREF_FILE_NS_NAME);
            nsAddressVector = userConfJobSubmitterAd
                .getStringValue(Utils.PREF_FILE_NS_ADDRESS);
            nsJDLESchemaVector = userConfJobSubmitterAd
                .getStringValue(Utils.PREF_FILE_JDLE_SCHEMA);
            for (int i = 0; i < nsNameVector.size(); i++) {
              Vector rowToAdd = new Vector();
              rowToAdd.add(nsNameVector.get(i));
              rowToAdd.add(nsAddressVector.get(i));
              rowToAdd.add(nsJDLESchemaVector.get(i));
              jobTableModel.addRow(rowToAdd);
            }
          }
        }
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
    }
  }

  /*
   static Vector getConfFileNetworkServers() {
   Ad userConfAd = new Ad();
   Vector nsVector = new Vector();
   File userConfFile = new File(GUIFileSystem.getUserPrefFile());
   if(userConfFile.isFile()) {
   try {
   userConfAd.fromFile(userConfFile.toString());
   } catch (Exception ex) {
   if (isDebugging) ex.printStackTrace();
   }
   try {
   if (userConfAd.hasAttribute(Utils.PREF_FILE_JOB_SUBMITTER)) {
   Ad userConfJobSubmitterAd = userConfAd.getAd(Utils.PREF_FILE_JOB_SUBMITTER);
   if (userConfJobSubmitterAd.hasAttribute(Utils.PREF_FILE_NS_NAME)) {
   Vector nsNameVector = userConfJobSubmitterAd.getStringValue(Utils.PREF_FILE_NS_NAME);
   Vector nsAddressVector = userConfJobSubmitterAd.getStringValue(Utils.PREF_FILE_NS_ADDRESS);
   Vector nsJDLESchemaVector = userConfJobSubmitterAd.getStringValue(Utils.PREF_FILE_JDLE_SCHEMA);
   for (int i = 0; i < nsNameVector.size(); i++) {
   nsVector.add(new NetworkServer(nsNameVector.get(i).toString(),
   nsAddressVector.get(i).toString(),
   nsJDLESchemaVector.get(i).toString()));
   }
   }
   }
   }
   catch (Exception e) {
   if (isDebugging) e.printStackTrace();
   }
   }
   return nsVector;
   }
   static Vector getConfFileNetworkServerNames() {
   Vector nsNameVector = new Vector();
   Vector nsVector = getConfFileNetworkServers();
   for(int i = 0; i < nsVector.size(); i++) {
   nsNameVector.add(((NetworkServer) nsVector.get(i)).getName());
   }
   return nsNameVector;
   }
   */
  int jButtonApplyEvent(ActionEvent e) {
    String title = "<html><font color=\"#602080\">"
        + JobSubmitterPreferences.NS_PANEL_NAME + ":" + "</font>";
    JDialog jDialog = (JDialog) jobSubmitterJFrame
        .getJobSubmitterPreferencesReference();
    if (jobTableModel.getRowCount() == 0) {
      JOptionPane.showOptionDialog(jDialog, title
          + "\nPlease provide at least a Network Server", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      return Utils.FAILED;
    }
    for (int i = 0; i < jobSubmitterJFrame.jTabbedPaneRB.getTabCount(); i++) {
      if (((NSPanel) jobSubmitterJFrame.jTabbedPaneRB.getComponentAt(i))
          .hasSubmittingJobs()) {
        JOptionPane
            .showOptionDialog(
                jDialog,
                title
                    + "\nOne or more jobs are in submitting phase"
                    + "\nCannot apply preferences, all job submissions must be completed",
                Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                JOptionPane.WARNING_MESSAGE, null, null, null);
        return Utils.FAILED;
      }
    }
    Iterator iterator = GUIGlobalVars.tempNSLBMap.keySet().iterator();
    while (iterator.hasNext()) {
      if (((Vector) GUIGlobalVars.tempNSLBMap.get(iterator.next())).size() == 0) {
        return Utils.FAILED;
      }
    }
    Ad userConfAd = GUIFileSystem.loadPrefFileAd();
    Ad userConfJobSubmitterAd = new Ad();
    File userConfFile = new File(GUIFileSystem.getUserPrefFile());
    if (userConfFile.isFile()) {
      try {
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
    Vector nsApplyVector = new Vector();
    Vector nsNameVector = new Vector();
    Vector nsToRemove = new Vector();
    Iterator nsIterator = GUIGlobalVars.nsMap.keySet().iterator();
    while (nsIterator.hasNext()) {
      nsToRemove.add(nsIterator.next().toString());
    }
    logger.debug("nsToRemove Vector: " + nsToRemove);
    String nsName;
    String nsAddress;
    String nsSchema;
    NetworkServer ns;
    GUIGlobalVars.nsMap.clear();
    for (int i = 0; i < jobTableModel.getRowCount(); i++) {
      nsName = jobTableModel.getValueAt(i, NS_NAME_COLUMN_INDEX).toString()
          .trim();
      nsAddress = jobTableModel.getValueAt(i, NS_ADDRESS_COLUMN_INDEX)
          .toString().trim();
      nsSchema = jobTableModel.getValueAt(i, JDLE_SCHEMA_COLUMN_INDEX)
          .toString().trim();
      ns = new NetworkServer(nsName, nsAddress, nsSchema);
      GUIGlobalVars.nsMap.put(nsName, ns);
      nsToRemove.remove(nsName);
      guiNSNameVector.add(Constant.getInstance(nsName));
      guiNSAddressVector.add(Constant.getInstance(nsAddress));
      guiNSJDLESchemaVector.add(Constant.getInstance(nsSchema));
      nsApplyVector.add(new NetworkServer(jobTableModel.getValueAt(i,
          NS_NAME_COLUMN_INDEX).toString(), jobTableModel.getValueAt(i,
          NS_ADDRESS_COLUMN_INDEX).toString(), jobTableModel.getValueAt(i,
          JDLE_SCHEMA_COLUMN_INDEX).toString()));
      nsNameVector.add(jobTableModel.getValueAt(i, NS_NAME_COLUMN_INDEX)
          .toString());
    }
    logger.debug("nsToRemove Vector (after): " + nsToRemove);
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
    logger.debug("userConfFile: " + userConfFile);
    try {
      GUIFileSystem
          .saveTextFile(userConfFile, userConfAd.toString(true, false));
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
      JOptionPane.showOptionDialog(jDialog, "Unable to save preferences file",
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
    }
    for (int i = 0; i < nsToRemove.size(); i++) {
      try {
        GUIFileSystem.removeDirectoryTree(new File(GUIFileSystem
            .getJobTemporaryFileDirectory()
            + nsToRemove.get(i).toString()));
      } catch (Exception ex) {
        // Do nothing. Unable to remove panel directory and jobs.
      }
    }
    jobSubmitterJFrame.setNSTabbedPanePanels(nsApplyVector);
    jobSubmitterJFrame.setNSMenuItems(nsNameVector);
    if (jobSubmitterJFrame.jTabbedPaneRB.getTabCount() == 1) {
      jobSubmitterJFrame.jMenuMoveTo.setEnabled(false);
      jobSubmitterJFrame.jMenuCopyTo.setEnabled(false);
    } else {
      jobSubmitterJFrame.jMenuMoveTo.setEnabled(true);
      jobSubmitterJFrame.jMenuCopyTo.setEnabled(true);
    }
    //jobSubmitterJFrame.jobSubmitterPreferences.reloadOldWorkingSession = true;
    return Utils.SUCCESS;
  }

  void jTextFieldNSNameKeyPressed(KeyEvent e) {
    String nsName = jTextFieldNSName.getText();
    if (nsName.length() > GraphicUtils.NS_NAME_MAX_CHAR_NUMBER) {
      jTextFieldNSName.setText(nsName.substring(0,
          GraphicUtils.NS_NAME_MAX_CHAR_NUMBER));
      Toolkit.getDefaultToolkit().beep();
    }
  }
}