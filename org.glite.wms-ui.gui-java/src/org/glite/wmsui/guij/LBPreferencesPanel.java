/*
 * LBPreferencesPanel.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.awt.AWTEvent;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.FocusEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.plaf.basic.BasicArrowButton;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Ad;
import condor.classad.Constant;
import condor.classad.ListExpr;

/**
 * Implementation of the LBPreferencesPanel class.
 * This class implements the main part of the Job Submitter application
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class LBPreferencesPanel extends JPanel {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int LB_ADDRESS_COLUMN_INDEX = 0;

  static final String LB_ADDRESS_TABLE_HEADER = "Address";

  static final int LB_PORT_COLUMN_INDEX = 1;

  static final String LB_PORT_TABLE_HEADER = "Port";

  static final String LB_DEFAULT_PORT = "9000";

  String lastSelectedNS = "";

  JPanel jPanelLB = new JPanel();

  JTextField jTextFieldLBAddress = new JTextField();

  JTextField jTextFieldLBPort = new JTextField();

  JLabel jLabelLBAddress = new JLabel();

  JLabel jLabelLBPort = new JLabel();

  JScrollPane jScrollPaneLBTable = new JScrollPane();

  JTable jTableLB;

  JobTableModel jobTableModel;

  Vector vectorHeader = new Vector();

  JButton jButtonAdd = new JButton();

  JButton jButtonRemove = new JButton();

  JButton jButtonAll = new JButton();

  JButton jButtonNone = new JButton();

  JButton jButtonClear = new JButton();

  JButton jButtonClearTable = new JButton();

  JPanel jPanelMiscellaneous = new JPanel();

  JTextField jTextFieldUpdateRate = new JTextField();

  //JLabel jLabelUpdateRate = new JLabel();
  JLabel jLabelMin = new JLabel();

  JobMonitorInterface jint;

  JButton jButtonEdit = new JButton();

  BasicArrowButton upUpdateRate = new BasicArrowButton(BasicArrowButton.NORTH);

  BasicArrowButton downUpdateRate = new BasicArrowButton(BasicArrowButton.SOUTH);

  JobMonitor jobMonitorJFrame;

  JobSubmitter jobSubmitterJFrame;

  JComboBox jComboBoxNS = new JComboBox();

  JLabel jLabelNS = new JLabel();

  JTextField jTextFieldNS = new JTextField();

  JLabel jLabelNSAddress = new JLabel();

  JPanel jPanelNS = new JPanel();

  JRadioButton jRadioButtonLBUpdate = new JRadioButton();

  JRadioButton jRadioButtonRGMAUpdate = new JRadioButton();

  ButtonGroup buttonGroupUpdate = new ButtonGroup();

  /**
   * Constructor.
   */
  public LBPreferencesPanel(Component component) {
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
    vectorHeader.addElement(LB_ADDRESS_TABLE_HEADER);
    vectorHeader.addElement(LB_PORT_TABLE_HEADER);
    jobTableModel = new JobTableModel(vectorHeader, 0);
    jTableLB = new JTable(jobTableModel);
    jTableLB.getTableHeader().setReorderingAllowed(false);
    jTextFieldLBPort.setText(Utils.LB_DEFAULT_PORT);
    jLabelLBAddress.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelLBAddress.setText("LB Address");
    jLabelLBPort.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelLBPort.setText("LB Port");
    jScrollPaneLBTable.getViewport().setBackground(Color.white);
    jScrollPaneLBTable.setBorder(BorderFactory.createEtchedBorder());
    jButtonAdd.setText("  Add  ");
    jButtonAdd.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonAddEvent(e);
      }
    });
    jButtonRemove.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonRemoveEvent(e);
      }
    });
    jButtonRemove.setText("Remove");
    jButtonAll.setText("   All   ");
    jButtonAll.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonAllEvent(e);
      }
    });
    jButtonNone.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonNoneEvent(e);
      }
    });
    jButtonNone.setText("  None  ");
    jButtonClear.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonClearEvent(e);
      }
    });
    jButtonClear.setText(" Clear ");
    jButtonClearTable.setText(" Clear ");
    jButtonClearTable.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonClearTableEvent(e);
      }
    });
    jTextFieldUpdateRate.setPreferredSize(new Dimension(35, 25));
    jTextFieldUpdateRate.setHorizontalAlignment(SwingConstants.RIGHT);
    //jTextFieldUpdateRate.setBounds(new Rectangle(135, 18, 33, 27));
    jTextFieldUpdateRate.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldUpdateRate, Utils.INTEGER,
            Integer.toString(Utils.UPDATE_RATE_DEF_VAL),
            Utils.UPDATE_RATE_MIN_VAL, Utils.UPDATE_RATE_MAX_VAL);
      }

      public void focusGained(FocusEvent e) {
      }
    });
    jTextFieldUpdateRate.setText(Integer.toString(Utils.UPDATE_RATE_DEF_VAL));
    jLabelMin.setText("Min.");
    jButtonEdit.setText("Replace");
    jButtonEdit.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonReplaceEvent(e);
      }
    });
    upUpdateRate.setBounds(new Rectangle(168, 15, 16, 16));
    upUpdateRate.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldUpdateRate, Utils.INTEGER, Integer
            .toString(Utils.UPDATE_RATE_DEF_VAL), Utils.UPDATE_RATE_MIN_VAL,
            Utils.UPDATE_RATE_MAX_VAL);
      }
    });
    downUpdateRate.setBounds(new Rectangle(168, 31, 16, 16));
    downUpdateRate.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldUpdateRate, Utils.INTEGER, Integer
            .toString(Utils.UPDATE_RATE_DEF_VAL), Utils.UPDATE_RATE_MIN_VAL,
            Utils.UPDATE_RATE_MAX_VAL);
      }
    });
    jRadioButtonLBUpdate.setText("LB Update Rate");
    jRadioButtonLBUpdate.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jRadioButtonLBUpdateEvent(e);
      }
    });
    jRadioButtonRGMAUpdate.setText("R-GMA");
    jRadioButtonRGMAUpdate
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jRadioButtonRGMAUpdateEvent(e);
          }
        });
    jScrollPaneLBTable.getViewport().add(jTableLB, null);
    buttonGroupUpdate.add(jRadioButtonLBUpdate);
    buttonGroupUpdate.add(jRadioButtonRGMAUpdate);
    jRadioButtonLBUpdate.setSelected(true);
    setLBUpdateEnabled(true);
    jComboBoxNS.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jComboBoxNSActionPerformed(e);
      }
    });
    jComboBoxNS.setMinimumSize(new Dimension(200, 25));
    jLabelNS.setHorizontalAlignment(SwingConstants.RIGHT);
    //jLabelNS.setText("NS Name");
    jLabelNS.setText("NS Address");
    jTextFieldNS.setText("");
    jTextFieldNS.setPreferredSize(new Dimension(180, 20));
    jTextFieldLBPort.setText(Utils.LB_DEFAULT_PORT);
    jTextFieldLBPort.setPreferredSize(new Dimension(60, 20));
    jTextFieldLBAddress.setPreferredSize(new Dimension(325, 20));
    //jLabelUpdateRate.setBounds(new Rectangle(16, 22, 84, 18));
    //jLabelUpdateRate.setHorizontalAlignment(SwingConstants.RIGHT);
    //jLabelUpdateRate.setText("Update Rate");
    jLabelNSAddress.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelNSAddress.setText("NS Address");
    this.setLayout(new BorderLayout());
    jPanelNS.add(jLabelNS);
    jPanelNS.add(jComboBoxNS);
    ((FlowLayout) jPanelNS.getLayout()).setAlignment(FlowLayout.LEFT);
    //jPanelNS.add(jLabelNSAddress);
    //jPanelNS.add(jTextFieldNS);
    JPanel jPanelLB = new JPanel();
    ((FlowLayout) jPanelLB.getLayout()).setAlignment(FlowLayout.LEFT);
    jPanelLB.add(jLabelLBAddress);
    jPanelLB.add(jTextFieldLBAddress);
    jPanelLB.add(jLabelLBPort);
    jPanelLB.add(jTextFieldLBPort);
    JPanel jPanelButton = new JPanel();
    ((FlowLayout) jPanelButton.getLayout()).setAlignment(FlowLayout.RIGHT);
    jPanelButton.add(jButtonClear, null);
    jPanelButton.add(jButtonEdit, null);
    jPanelButton.add(jButtonAdd, null);
    JPanel jPanelButton2 = new JPanel();
    jPanelButton2.setBorder(GraphicUtils.SPACING_BORDER);
    jPanelButton2.setLayout(new BoxLayout(jPanelButton2, BoxLayout.X_AXIS));
    jPanelButton2.add(jButtonAll, null);
    jPanelButton2.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelButton2.add(jButtonNone, null);
    jPanelButton2.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelButton2.add(Box.createGlue());
    jPanelButton2.add(jButtonRemove, null);
    jPanelButton2.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelButton2.add(jButtonClearTable, null);
    JPanel jPanelNorth = new JPanel();
    JPanel jPanelCenter = new JPanel();
    JPanel jPanelSouth = new JPanel();
    jPanelNorth.setLayout(new BoxLayout(jPanelNorth, BoxLayout.Y_AXIS));
    jPanelNorth.add(jPanelNS);
    jPanelNorth.add(jPanelLB);
    jPanelNorth.add(jPanelButton);
    JPanel jPanelLBConf = new JPanel();
    jPanelLBConf.setLayout(new BorderLayout());
    jPanelLBConf.setBorder(new TitledBorder(new EtchedBorder(),
        " LB Configuration ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelLBConf.add(jPanelNorth, BorderLayout.NORTH);
    jPanelLBConf.add(jScrollPaneLBTable, BorderLayout.CENTER);
    jPanelLBConf.add(jPanelButton2, BorderLayout.SOUTH);
    jScrollPaneLBTable.getViewport().add(jTableLB, null);
    jPanelMiscellaneous
        .setBorder(new TitledBorder(new EtchedBorder(), " Status Update ", 0,
            0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    //jPanelMiscellaneous.setMaximumSize(new Dimension(100, 210));
    JPanel jPanelUpDown = new JPanel();
    jPanelUpDown.setLayout(new BoxLayout(jPanelUpDown, BoxLayout.Y_AXIS));
    jPanelUpDown.add(upUpdateRate);
    jPanelUpDown.add(downUpdateRate);
    ((FlowLayout) jPanelMiscellaneous.getLayout())
        .setAlignment(FlowLayout.LEFT);
    jPanelMiscellaneous.add(jRadioButtonLBUpdate, null);
    //jPanelMiscellaneous.add(jLabelUpdateRate, null);
    jPanelMiscellaneous.add(jTextFieldUpdateRate, null);
    jPanelMiscellaneous.add(jPanelUpDown, null);
    jPanelMiscellaneous.add(jLabelMin, null);
    jPanelMiscellaneous.add(jRadioButtonRGMAUpdate, null);
    JPanel jPanelUpdateRate = new JPanel();
    FlowLayout flowLayout = (FlowLayout) jPanelUpdateRate.getLayout();
    flowLayout.setHgap(0);
    flowLayout.setVgap(0);
    flowLayout.setAlignment(FlowLayout.LEFT);
    jPanelUpdateRate.add(jPanelMiscellaneous);
    this.add(jPanelUpdateRate, BorderLayout.SOUTH);
    this.add(jPanelLBConf, BorderLayout.CENTER);
    //this.add(jPanelLBConf);
    //this.add(jPanelMiscellaneous);
    jTableLB.addMouseListener(new MouseAdapter() {
      public void mouseClicked(MouseEvent me) {
        if (me.getClickCount() == 2) {
          Point point = me.getPoint();
          int row = jTableLB.rowAtPoint(point);
          int column = jTableLB.columnAtPoint(point);
          String lbAddress = jobTableModel.getValueAt(row,
              LB_ADDRESS_COLUMN_INDEX).toString().trim();
          String lbPort = jobTableModel.getValueAt(row, LB_PORT_COLUMN_INDEX)
              .toString().trim();
          jTextFieldLBAddress.setText(lbAddress);
          jTextFieldLBPort.setText(lbPort);
        }
      }
    });
    jTextFieldLBAddress.grabFocus();
  }

  void jButtonClearEvent(ActionEvent e) {
    jTextFieldLBAddress.setText("");
    jTextFieldLBPort.setText(Utils.LB_DEFAULT_PORT);
    jTextFieldLBAddress.grabFocus();
  }

  void jButtonAddEvent(ActionEvent e) {
    String insertedAddress = jTextFieldLBAddress.getText().trim();
    if (!insertedAddress.equals("")) {
      String insertedPort = jTextFieldLBPort.getText().trim();
      if (insertedPort.equals("")) {
        insertedPort = Utils.LB_DEFAULT_PORT;
        if (insertedPort.equals("")) {
          jTextFieldLBPort.grabFocus();
          JOptionPane.showOptionDialog(LBPreferencesPanel.this,
              "LB Port cannot be blank", Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
              null, null);
          jTextFieldLBPort.selectAll();
          return;
        }
      } else if (Utils.getValueType(insertedPort) != Utils.INTEGER
          || (Integer.parseInt(insertedPort, 10) < 1024)) {
        jTextFieldLBPort.grabFocus();
        JOptionPane.showOptionDialog(LBPreferencesPanel.this,
            "LB Port must be an int value greater than 1023",
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        jTextFieldLBPort.selectAll();
        return;
      }
      if (jobTableModel.isRowPresent(insertedAddress + insertedPort) == true) {
        JOptionPane.showOptionDialog(LBPreferencesPanel.this,
            "Inserted LB Address and Port are already present",
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        return;
      }
      Vector rowElement = new Vector();
      rowElement.addElement(insertedAddress);
      rowElement.addElement(insertedPort);
      jobTableModel.addRow(rowElement);
      jTextFieldLBAddress.selectAll();
      if (jobSubmitterJFrame != null) {
        String nsAddress = jComboBoxNS.getSelectedItem().toString();
        GUIGlobalVars.tempNSLBMap.remove(nsAddress);
        Vector lbVector = new Vector();
        for (int i = 0; i < jTableLB.getRowCount(); i++) {
          lbVector.add(jobTableModel.getValueAt(i, LB_ADDRESS_COLUMN_INDEX)
              .toString()
              + ":"
              + jobTableModel.getValueAt(i, LB_PORT_COLUMN_INDEX).toString());
        }
        GUIGlobalVars.tempNSLBMap.put(nsAddress, lbVector);
      }
    } else {
      jTextFieldLBAddress.grabFocus();
      JOptionPane.showOptionDialog(LBPreferencesPanel.this,
          "LB Address field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldLBAddress.setText("");
      return;
    }
    jTextFieldLBAddress.grabFocus();
  }

  void jButtonReplaceEvent(ActionEvent ae) {
    if (jTableLB.getSelectedRowCount() == 0) {
      JOptionPane.showOptionDialog(LBPreferencesPanel.this,
          "Please first select from table a LB to replace",
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
      return;
    } else if (jTableLB.getSelectedRowCount() != 1) {
      JOptionPane.showOptionDialog(LBPreferencesPanel.this,
          "Please select from table a single LB to replace",
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
      return;
    }
    int selectedRow = jTableLB.getSelectedRow();
    String insertedLBAddress = jTextFieldLBAddress.getText().trim();
    String insertedLBPort = jTextFieldLBPort.getText().trim();
    if (insertedLBAddress.equals("")) {
      jTextFieldLBAddress.grabFocus();
      JOptionPane.showOptionDialog(LBPreferencesPanel.this,
          "LB Address field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldLBAddress.setText("");
      return;
    }
    if (insertedLBPort.equals("")) {
      insertedLBPort = Utils.LB_DEFAULT_PORT;
    } else if (Utils.getValueType(insertedLBPort) != Utils.INTEGER) {
      jTextFieldLBPort.grabFocus();
      JOptionPane.showOptionDialog(LBPreferencesPanel.this,
          "LB Port must be an int value", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldLBPort.selectAll();
      return;
    }
    String selectedLBAddress = jobTableModel.getValueAt(selectedRow,
        LB_ADDRESS_COLUMN_INDEX).toString().trim();
    String selectedLBPort = jobTableModel.getValueAt(selectedRow,
        LB_PORT_COLUMN_INDEX).toString().trim();
    if (!selectedLBAddress.equals(insertedLBAddress)
        && (jobTableModel.isRowPresent(insertedLBAddress + insertedLBPort) == true)) {
      JOptionPane.showOptionDialog(LBPreferencesPanel.this,
          "Inserted LB Address and Port are already present",
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return;
    }
    jobTableModel.setValueAt(insertedLBAddress, selectedRow,
        LB_ADDRESS_COLUMN_INDEX);
    jobTableModel.setValueAt(insertedLBPort, selectedRow, LB_PORT_COLUMN_INDEX);
    if (jobSubmitterJFrame != null) {
      String nsAddress = jComboBoxNS.getSelectedItem().toString();
      GUIGlobalVars.tempNSLBMap.remove(nsAddress);
      Vector lbVector = new Vector();
      for (int i = 0; i < jTableLB.getRowCount(); i++) {
        lbVector.add(jobTableModel.getValueAt(i, LB_ADDRESS_COLUMN_INDEX)
            .toString()
            + ":"
            + jobTableModel.getValueAt(i, LB_PORT_COLUMN_INDEX).toString());
      }
      GUIGlobalVars.tempNSLBMap.put(nsAddress, lbVector);
    }
    jTextFieldLBAddress.selectAll();
    jTextFieldLBAddress.grabFocus();
  }

  void jButtonRemoveEvent(ActionEvent e) {
    int[] selectedRow = jTableLB.getSelectedRows();
    int selectedRowCount = selectedRow.length;
    if (selectedRowCount != 0) {
      for (int i = selectedRowCount - 1; i >= 0; i--) {
        jobTableModel.removeRow(selectedRow[i]);
        if (jobTableModel.getRowCount() != 0) {
          int selectableRow = selectedRow[selectedRowCount - 1] + 1
              - selectedRowCount; // Next.
          if (selectableRow > jobTableModel.getRowCount() - 1) {
            selectableRow--; // Prev. (selectedRow[selectedRowCount - 1] - selectedRowCount).
          }
          jTableLB.setRowSelectionInterval(selectableRow, selectableRow);
        }
      }
      if (jobSubmitterJFrame != null) {
        String nsAddress = jComboBoxNS.getSelectedItem().toString();
        GUIGlobalVars.tempNSLBMap.remove(nsAddress);
        Vector lbVector = new Vector();
        for (int i = 0; i < jTableLB.getRowCount(); i++) {
          lbVector.add(jobTableModel.getValueAt(i, LB_ADDRESS_COLUMN_INDEX)
              .toString()
              + ":"
              + jobTableModel.getValueAt(i, LB_PORT_COLUMN_INDEX).toString());
        }
        GUIGlobalVars.tempNSLBMap.put(nsAddress, lbVector);
      }
    } else {
      JOptionPane.showOptionDialog(LBPreferencesPanel.this,
          Utils.SELECT_AN_ITEM, Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
          null, null);
    }
  }

  void jButtonClearTableEvent(ActionEvent e) {
    int choice = JOptionPane.showOptionDialog(LBPreferencesPanel.this,
        "Do you really want to clear LB table?", "Confirm Clear",
        JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null,
        null);
    if (choice == 0) {
      jobTableModel.removeAllRows();
      if (jobSubmitterJFrame != null) {
        String nsAddress = jComboBoxNS.getSelectedItem().toString();
        GUIGlobalVars.tempNSLBMap.remove(nsAddress);
        GUIGlobalVars.tempNSLBMap.put(nsAddress, new Vector());
      }
    }
  }

  void jButtonAllEvent(ActionEvent e) {
    jTableLB.selectAll();
  }

  void jButtonNoneEvent(ActionEvent e) {
    jTableLB.clearSelection();
  }

  Ad setNSLBVectorAttribute(Ad ad, Vector lbAddressVector) throws Exception {
    Vector vectorToSave = new Vector();
    Vector lbVector = new Vector();
    String list;
    for (int j = 0; j < lbAddressVector.size(); j++) {
      list = "";
      lbVector = (Vector) lbAddressVector.get(j);
      ListExpr listExpr = new ListExpr();
      String lbAddress;
      for (int i = 0; i < lbVector.size(); i++) {
        lbAddress = lbVector.get(i).toString().trim();
        if (!lbAddress.equals("")) {
          listExpr.add(Constant.getInstance(lbAddress));
        }
      }
      vectorToSave.add(listExpr);
    }
    try {
      if (ad.hasAttribute(Utils.PREF_FILE_LB_ADDRESS)) {
        ad.delAttribute(Utils.PREF_FILE_LB_ADDRESS);
      }
      ad.setAttribute(Utils.PREF_FILE_LB_ADDRESS, new ListExpr(vectorToSave));
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      throw e;
    }
    return ad;
  }

  static Ad setNSLBMapAttribute(Ad ad, Map map) throws Exception {
    Iterator iterator = map.keySet().iterator();
    Vector vectorToSave = new Vector();
    Vector nsVector = new Vector();
    Vector lbVector;
    String nsAddress;
    String list;
    while (iterator.hasNext()) {
      list = "";
      nsAddress = iterator.next().toString();
      nsVector.add(Constant.getInstance(nsAddress));
      lbVector = (Vector) map.get(nsAddress);
      ListExpr listExpr = new ListExpr();
      String lbAddress;
      for (int i = 0; i < lbVector.size(); i++) {
        lbAddress = lbVector.get(i).toString().trim();
        if (!lbAddress.equals("")) {
          listExpr.add(Constant.getInstance(lbAddress));
        }
      }
      vectorToSave.add(listExpr);
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
    logger.debug("---------->>> ad: " + ad);
    return ad;
  }

  int jButtonApplyEvent(ActionEvent e) {
    String address = "";
    String port = "";
    Vector lbToSaveVector = new Vector();
    Vector lbVector = new Vector();
    File userConfFile = new File(GUIFileSystem.getUserPrefFile());
    String title = "<html><font color=\"#602080\">"
        + JobSubmitterPreferences.LB_PANEL_NAME + ":" + "</font>";
    JDialog jDialog = (jobSubmitterJFrame != null) ? (JDialog) jobSubmitterJFrame
        .getJobSubmitterPreferencesReference()
        : (JDialog) jobMonitorJFrame.getJobMonitorPreferencesReference();
    if (jobSubmitterJFrame != null) {
      NSPreferencesPanel nsPreferencesPanel = jobSubmitterJFrame.jobSubmitterPreferences
          .getNSPreferencesPanelReference();
      Vector lbAddressVector = new Vector();
      String nsAddress;
      for (int i = 0; i < nsPreferencesPanel.jobTableModel.getRowCount(); i++) {
        nsAddress = nsPreferencesPanel.jobTableModel.getValueAt(i,
            NSPreferencesPanel.NS_ADDRESS_COLUMN_INDEX).toString();
        lbAddressVector = (Vector) GUIGlobalVars.tempNSLBMap.get(nsAddress);
        if (lbAddressVector.size() == 0) {
          title = "<html><font color=\"#602080\">"
              + JobSubmitterPreferences.NS_PANEL_NAME + ", "
              + JobSubmitterPreferences.LB_PANEL_NAME + ":" + "</font>";
          JOptionPane.showOptionDialog(jDialog, title
              + "\nPlease provide at least a Logging"
              + " & Bookkeeping server for Network Server:" + "\n'" + nsAddress
              + "'", Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
          NSPreferencesPanel nsPreferencesPanelReference = jobSubmitterJFrame
              .getJobSubmitterPreferencesReference()
              .getNSPreferencesPanelReference();
          int index = nsPreferencesPanelReference.jobTableModel
              .getIndexOfElementInColumn(nsAddress,
                  NSPreferencesPanel.NS_ADDRESS_COLUMN_INDEX);
          if (index != -1) {
            nsPreferencesPanelReference.jTableNS.setRowSelectionInterval(index,
                index);
          }
          return Utils.FAILED;
        }
      }
      NSPreferencesPanel nsPreferencesPanelReference = jobSubmitterJFrame
          .getJobSubmitterPreferencesReference()
          .getNSPreferencesPanelReference();
      GUIGlobalVars.nsLBMap.clear();
      Vector nsLBVectorToSave = new Vector();
      String tempNSAddress;
      for (int i = 0; i < nsPreferencesPanelReference.jobTableModel
          .getRowCount(); i++) {
        tempNSAddress = nsPreferencesPanelReference.jobTableModel.getValueAt(i,
            NSPreferencesPanel.NS_ADDRESS_COLUMN_INDEX).toString();
        Vector tempLBVector = (Vector) GUIGlobalVars.tempNSLBMap
            .get(tempNSAddress);
        if (!GUIGlobalVars.nsLBMap.containsKey(tempNSAddress)) {
          GUIGlobalVars.nsLBMap.put(tempNSAddress, tempLBVector);
        }
        nsLBVectorToSave.add(tempLBVector);
      }
      Ad userConfAd = GUIFileSystem.loadPrefFileAd();
      Ad userConfJobSubmitterAd = new Ad();
      try {
        if (userConfAd.hasAttribute(Utils.PREF_FILE_JOB_SUBMITTER)) {
          userConfJobSubmitterAd = userConfAd
              .getAd(Utils.PREF_FILE_JOB_SUBMITTER);
          userConfAd.delAttribute(Utils.PREF_FILE_JOB_SUBMITTER);
        }
        if (GUIGlobalVars.tempNSLBMap.size() != 0) {
          userConfJobSubmitterAd = setNSLBVectorAttribute(
              userConfJobSubmitterAd, nsLBVectorToSave);
        }
        logger.debug("userConfJobSubmitterAd:" + userConfJobSubmitterAd);
        userConfAd.setAttribute(Utils.PREF_FILE_JOB_SUBMITTER,
            userConfJobSubmitterAd);
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
      try {
        GUIFileSystem.saveTextFile(userConfFile, userConfAd
            .toString(true, true));
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        JOptionPane.showOptionDialog(jDialog, title
            + "\nUnable to save preferences settings to file",
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
      logger.debug("User Conf Ad: " + userConfAd);
    } else if (jobMonitorJFrame != null) {
      int updateRate = Integer.parseInt(jTextFieldUpdateRate.getText(), 10);
      logger.debug("updateRate: " + updateRate + " GUIGlobal: "
          + GUIGlobalVars.getJobMonitorUpdateRate());
      if (updateRate != GUIGlobalVars.getJobMonitorUpdateRate()) {
        GUIGlobalVars.setJobMonitorUpdateRate(updateRate);
        logger.debug("updateRate setting... ");
        if ((jobMonitorJFrame != null)
            && (jobMonitorJFrame.multipleJobPanel != null)
            && (jobMonitorJFrame.multipleJobPanel.updateThread != null)) {
          jobMonitorJFrame.multipleJobPanel.updateThread.restartThread();
        }
      }
      int updateMode = Utils.LB_MODE;
      if (jRadioButtonRGMAUpdate.isSelected()) {
        updateMode = Utils.RGMA_MODE;
      }
      if (jobTableModel.getRowCount() == 0) {
        jobMonitorJFrame.setLBMenuItems(lbVector); // lbVector is empty.
        jobMonitorJFrame.jMenuItemGetJobIdFromLB.setEnabled(false);
      } else {
        for (int i = 0; i < jobTableModel.getRowCount(); i++) {
          address = jobTableModel.getValueAt(i, LB_ADDRESS_COLUMN_INDEX)
              .toString();
          port = jobTableModel.getValueAt(i, LB_PORT_COLUMN_INDEX).toString();
          lbToSaveVector.add(Constant.getInstance(address + ":" + port));
          lbVector.add(address + ":" + port);
        }
        jobMonitorJFrame.setLBMenuItems(lbVector);
        jobMonitorJFrame.jMenuItemGetJobIdFromLB.setEnabled(true);
      }
      Ad userConfAd = GUIFileSystem.loadPrefFileAd();
      Ad userConfJobMonitorAd = new Ad();
      try {
        if (userConfAd.hasAttribute(Utils.PREF_FILE_JOB_MONITOR)) {
          userConfJobMonitorAd = userConfAd.getAd(Utils.PREF_FILE_JOB_MONITOR);
          userConfAd.delAttribute(Utils.PREF_FILE_JOB_MONITOR);
        }
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
      try {
        if (userConfJobMonitorAd.hasAttribute(Utils.PREF_FILE_UPDATE_MODE)) {
          userConfJobMonitorAd.delAttribute(Utils.PREF_FILE_UPDATE_MODE);
        }
        userConfJobMonitorAd.setAttribute(Utils.PREF_FILE_UPDATE_MODE,
            updateMode);
        if (userConfJobMonitorAd.hasAttribute(Utils.PREF_FILE_UPDATE_RATE)) {
          userConfJobMonitorAd.delAttribute(Utils.PREF_FILE_UPDATE_RATE);
        }
        userConfJobMonitorAd.setAttribute(Utils.PREF_FILE_UPDATE_RATE, Integer
            .parseInt(jTextFieldUpdateRate.getText().trim(), 10));
        if (userConfJobMonitorAd.hasAttribute(Utils.PREF_FILE_LB_ADDRESS)) {
          userConfJobMonitorAd.delAttribute(Utils.PREF_FILE_LB_ADDRESS);
        }
        if (lbToSaveVector.size() != 0) {
          userConfJobMonitorAd.setAttribute(Utils.PREF_FILE_LB_ADDRESS,
              new ListExpr(lbToSaveVector));
        }
        logger.debug("userConfJobMonitorAd:" + userConfJobMonitorAd);
        userConfAd.setAttribute(Utils.PREF_FILE_JOB_MONITOR,
            userConfJobMonitorAd);
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
      try {
        GUIFileSystem.saveTextFile(userConfFile, userConfAd
            .toString(true, true));
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        JOptionPane.showOptionDialog(jDialog, title
            + "\nUnable to save preferences settings to file",
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
    }
    return Utils.SUCCESS;
  }

  void loadPreferencesFromFile() {
    logger.info("Loading Preferences from File");
    Ad userConfAd = GUIFileSystem.loadPrefFileAd();
    jobTableModel.removeAllRows();
    // JOB SUBMITTER
    if (jobSubmitterJFrame != null) {
      try {
        if (userConfAd.hasAttribute(Utils.PREF_FILE_JOB_SUBMITTER)) {
          Ad userConfJobSubmitterAd = userConfAd
              .getAd(Utils.PREF_FILE_JOB_SUBMITTER);
          if (userConfJobSubmitterAd.hasAttribute(Utils.PREF_FILE_NS_ADDRESS)) {
            Vector nsVector = userConfJobSubmitterAd
                .getStringValue(Utils.PREF_FILE_NS_ADDRESS);
            if (nsVector.size() != 0) {
              if (userConfJobSubmitterAd
                  .hasAttribute(Utils.PREF_FILE_LB_ADDRESS)) {
                Vector lbVector = userConfJobSubmitterAd
                    .getStringValue(Utils.PREF_FILE_LB_ADDRESS);
                if (lbVector.size() != 0) {
                  if (lbVector.get(0) instanceof String) {
                    for (int i = 1; i < lbVector.size(); i++) {
                      if (!(lbVector.get(i) instanceof String)) {
                        throw new Exception(
                            "Unable to parse preferences file, " + "'"
                                + Utils.CONF_FILE_LBADDRESSES + "' attribute");
                      }
                    }
                  } else { // Vector.
                    for (int i = 1; i < lbVector.size(); i++) {
                      if (lbVector.get(i) instanceof String) {
                        throw new Exception(
                            "Unable to parse preferences file, " + "'"
                                + Utils.CONF_FILE_LBADDRESSES + "' attribute");
                      }
                    }
                  }
                  String nsItem;
                  GUIGlobalVars.tempNSLBMap = new HashMap();
                  int lbVectorSize = lbVector.size();
                  Vector vectorElement = new Vector();
                  if (!(lbVector.get(0) instanceof String)) {
                    for (int i = 0; i < nsVector.size(); i++) {
                      nsItem = nsVector.get(i).toString();
                      if (!GUIGlobalVars.tempNSLBMap.containsKey(nsItem)) {
                        if (i < lbVectorSize) {
                          vectorElement = (Vector) lbVector.get(i);
                          if ((vectorElement.size() == 1)
                              && vectorElement.get(0).toString().equals("")) {
                            vectorElement = new Vector();
                          }
                        } else {
                          vectorElement = new Vector();
                        }
                        GUIGlobalVars.tempNSLBMap.put(nsItem, vectorElement);
                        addNSAddress(nsItem);
                      }
                    }
                    lbVector = (Vector) lbVector.get(0);
                  } else {
                    for (int i = 0; i < nsVector.size(); i++) {
                      nsItem = nsVector.get(i).toString();
                      if (!GUIGlobalVars.tempNSLBMap.containsKey(nsItem)) {
                        GUIGlobalVars.tempNSLBMap.put(nsItem, lbVector.clone());
                        addNSAddress(nsItem);
                      }
                    }
                  }
                }
                // Setting first NS values (i.e. Address, LB Addresses).
                if (jComboBoxNS.getItemCount() != 0) {
                  jComboBoxNS.setEnabled(true);
                  jComboBoxNS.setSelectedIndex(0);
                }
                setLBAddressTable(lbVector);
              } else {
                for (int i = 0; i < nsVector.size(); i++) {
                  addNSAddress(nsVector.get(i).toString());
                }
              }
            } else {
              for (int i = 0; i < nsVector.size(); i++) {
                addNSAddress(nsVector.get(i).toString());
              }
            }
          }
        }
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
      // JOB MONITOR
    } else if (jobMonitorJFrame != null) {
      if (userConfAd.hasAttribute(Utils.PREF_FILE_JOB_MONITOR)) {
        try {
          Ad userConfJobMonitorAd = userConfAd
              .getAd(Utils.PREF_FILE_JOB_MONITOR);
          if (userConfJobMonitorAd.hasAttribute(Utils.PREF_FILE_UPDATE_RATE)) {
            int updateRate = ((Integer) userConfJobMonitorAd.getIntValue(
                Utils.PREF_FILE_UPDATE_RATE).get(0)).intValue();
            if ((Utils.UPDATE_RATE_MIN_VAL <= updateRate)
                && (updateRate <= Utils.UPDATE_RATE_MAX_VAL)) {
              jTextFieldUpdateRate.setText(Integer.toString(updateRate));
            } else {
              jTextFieldUpdateRate.setText(Integer
                  .toString(Utils.UPDATE_RATE_DEF_VAL));
            }
          }
          if (userConfJobMonitorAd.hasAttribute(Utils.PREF_FILE_UPDATE_MODE)) {
            int updateMode = ((Integer) userConfJobMonitorAd.getIntValue(
                Utils.PREF_FILE_UPDATE_MODE).get(0)).intValue();
            GUIGlobalVars.setUpdateMode(updateMode);
            logger.debug("UPDATE MODE: " + updateMode);
            if (updateMode == Utils.RGMA_MODE) {
              jRadioButtonRGMAUpdate.setSelected(true);
              jRadioButtonRGMAUpdateEvent(null);
            } else {
              jRadioButtonLBUpdate.setSelected(true);
              jRadioButtonLBUpdateEvent(null);
            }
          }
          if (userConfJobMonitorAd.hasAttribute(Utils.PREF_FILE_LB_ADDRESS)) {
            Vector lbVector = userConfJobMonitorAd
                .getStringValue(Utils.PREF_FILE_LB_ADDRESS);
            String address;
            String port = LB_DEFAULT_PORT;
            int index = -1;
            for (int i = 0; i < lbVector.size(); i++) {
              address = lbVector.get(i).toString().trim();
              index = address.lastIndexOf(":");
              if (index != -1) {
                port = address.substring(index + 1);
                address = address.substring(0, index);
              }
              Vector rowToAdd = new Vector();
              rowToAdd.add(address);
              rowToAdd.add(port);
              jobTableModel.addRow(rowToAdd);
            }
          }
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
        }
      }
    }
  }

  void setLBAddressTable(Vector lbVector) {
    if (lbVector == null) {
      return;
    }
    jobTableModel.removeAllRows();
    String address;
    String port = LB_DEFAULT_PORT;
    int index = -1;
    for (int i = 0; i < lbVector.size(); i++) {
      address = lbVector.get(i).toString().trim();
      index = address.lastIndexOf(":");
      if (index != -1) {
        port = address.substring(index + 1);
        address = address.substring(0, index);
      }
      Vector rowToAdd = new Vector();
      rowToAdd.add(address);
      rowToAdd.add(port);
      jobTableModel.addRow(rowToAdd);
    }
  }

  void setMiscellaneousVisible(boolean bool) {
    jPanelMiscellaneous.setVisible(bool);
  }

  void setNSComboVisible(boolean bool) {
    jPanelNS.setVisible(bool);
  }

  void loadDefaultPreferences() {
    GUIGlobalVars.tempNSLBMap = Utils.cloneMap(GUIGlobalVars.defaultNSLBMap);
    GUIGlobalVars.nsLBMap = Utils.cloneMap(GUIGlobalVars.defaultNSLBMap);
    jobTableModel.removeAllRows();
    jComboBoxNS.removeAllItems();
    jComboBoxNS.setEnabled(false);
    if (GUIGlobalVars.nsLBMap.size() != 0) {
      Iterator iterator = GUIGlobalVars.nsLBMap.keySet().iterator();
      while (iterator.hasNext()) {
        addNSAddress(iterator.next().toString());
      }
      setLBAddressTable((Vector) GUIGlobalVars.nsLBMap.get(jComboBoxNS
          .getItemAt(0)));
      jComboBoxNS.setSelectedIndex(0);
      jComboBoxNS.setEnabled(true);
    }
    jTextFieldUpdateRate.setText(Integer.toString(Utils.UPDATE_RATE_DEF_VAL));
  }

  void setLBUpdateEnabled(boolean bool) {
    jTextFieldUpdateRate.setEnabled(bool);
    upUpdateRate.setEnabled(bool);
    downUpdateRate.setEnabled(bool);
    jLabelMin.setEnabled(bool);
  }

  void jRadioButtonLBUpdateEvent(ActionEvent e) {
    setLBUpdateEnabled(true);
  }

  void jRadioButtonRGMAUpdateEvent(ActionEvent e) {
    setLBUpdateEnabled(false);
  }

  void jComboBoxNSActionPerformed(ActionEvent ae) {
    String selectedNS = this.lastSelectedNS;
    Object selectedNSObject = jComboBoxNS.getSelectedItem();
    if (selectedNSObject != null) {
      selectedNS = selectedNSObject.toString().trim();
    }
    if (!selectedNS.equals(this.lastSelectedNS)) {
      jobTableModel.removeAllRows();
      Vector lbVector = (Vector) GUIGlobalVars.tempNSLBMap.get(selectedNS);
      setLBAddressTable(lbVector);
      this.lastSelectedNS = selectedNS;
    }
  }

  void addNSAddress(String address) {
    if (!GUIGlobalVars.tempNSLBMap.containsKey(address)) {
      GUIGlobalVars.tempNSLBMap.put(address, new Vector());
    }
    if (!GraphicUtils.comboBoxContains(jComboBoxNS, address)) {
      logger.debug("Adding address: " + address);
      jComboBoxNS.addItem(address);
      jComboBoxNS.setEnabled(true);
    }
    if (jComboBoxNS.getItemCount() != 0) {
      jComboBoxNS.setSelectedIndex(0);
      jComboBoxNSActionPerformed(null);
    }
  }

  void removeNSAddress(String address) {
    if (GUIGlobalVars.tempNSLBMap.containsKey(address)) {
      GUIGlobalVars.tempNSLBMap.remove(address);
    }
    jComboBoxNS.removeItem(address);
    if (jComboBoxNS.getItemCount() != 0) {
      jComboBoxNS.setSelectedIndex(0);
      jComboBoxNSActionPerformed(null);
    } else {
      jComboBoxNS.setEnabled(false);
    }
  }

  void replaceNSAddress(String oldAddress, String newAddress) {
    if (GUIGlobalVars.tempNSLBMap.containsKey(oldAddress)) {
      Vector lbVector = (Vector) GUIGlobalVars.tempNSLBMap.get(oldAddress);
      GUIGlobalVars.tempNSLBMap.remove(oldAddress);
      if (!GUIGlobalVars.tempNSLBMap.containsKey(newAddress)) {
        GUIGlobalVars.tempNSLBMap.put(newAddress, lbVector);
      }
    }
    jComboBoxNS.removeItem(oldAddress);
    if (!GraphicUtils.comboBoxContains(jComboBoxNS, newAddress)) {
      jComboBoxNS.addItem(newAddress);
      jComboBoxNS.setSelectedIndex(0);
      jComboBoxNSActionPerformed(null);
    }
  }

  void removeAllNSAddress() {
    GUIGlobalVars.tempNSLBMap.clear();
    jComboBoxNS.removeAllItems();
    jComboBoxNS.setEnabled(false);
    jobTableModel.removeAllRows();
  }
}