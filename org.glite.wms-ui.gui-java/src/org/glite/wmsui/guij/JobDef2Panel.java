/*
 * JobDef2Panel.java
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
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.FocusEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.Vector;
import javax.naming.directory.InvalidAttributeValueException;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.plaf.basic.BasicArrowButton;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Jdl;
import org.glite.jdl.JobAd;

/**
 * Implementation of the JobDef2Panel class.
 * 
 * @ingroup gui
 * @brief @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class JobDef2Panel extends JPanel {
  static Logger logger = Logger.getLogger(JDLEditor.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int VARIABLE_NAME_COLUMN_INDEX = 0;

  static final int VARIABLE_VALUE_COLUMN_INDEX = 1;

  String errorMsg = "";

  String warningMsg = "";

  JDLEditorInterface jint;

  JTextField jTextFieldVarName = new JTextField();

  JLabel jLabelVarName = new JLabel();

  JButton jButtonRemoveEnvironment = new JButton();

  JLabel jLabelValue = new JLabel();

  JPanel jPanelEnvironment = new JPanel();

  JButton jButtonAddEnvironment = new JButton();

  JButton jButtonClearEnvironment = new JButton();

  JTextField jTextFieldValue = new JTextField();

  JTextField jTextFieldMyProxyServer = new JTextField();

  JPanel jPanelMyProxyServer = new JPanel();

  Component component;

  JScrollPane jScrollPaneEnvironment = new JScrollPane();

  Vector vectorHeader = new Vector();

  JobTableModel jobTableModel;

  JTable jTableEnvironment;

  JButton jButtonReplaceEnvironment = new JButton();

  JPanel jPanelRetryCount = new JPanel();

  BasicArrowButton upRetryCount = new BasicArrowButton(BasicArrowButton.NORTH);

  JTextField jTextFieldRetryCount = new JTextField();

  BasicArrowButton downRetryCount = new BasicArrowButton(BasicArrowButton.SOUTH);

  JCheckBox jCheckBoxRetryCount = new JCheckBox();

  JPanel jPanelHLRLocation = new JPanel();

  JTextField jTextFieldHLRLocation = new JTextField();

  public JobDef2Panel(Component component) {
    this.component = component;
    if (component instanceof JDLEditor) {
      jint = (JDLEditor) component;
      /*
       * } else if (component instanceof JDLEJInternalFrame) { jint =
       * (JDLEJInternalFrame) component; } else if (component instanceof
       * JDLEJApplet) { jint = (JDLEJApplet) component;
       */
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
    logger.debug("GUIGlobalVars.getGUIConfVarRetryCount(): "
        + GUIGlobalVars.getGUIConfVarRetryCount());
    vectorHeader.addElement("Name");
    vectorHeader.addElement("Value");
    jobTableModel = new JobTableModel(vectorHeader, 0);
    jTableEnvironment = new JTable(jobTableModel);
    jTableEnvironment.getTableHeader().setReorderingAllowed(false);
    jTextFieldVarName.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldVarName);
      }
    });
    jLabelVarName.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelVarName.setText("Name");
    jButtonRemoveEnvironment
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jButtonRemoveEnvironmentEvent(e);
          }
        });
    jButtonRemoveEnvironment.setText("Remove");
    jLabelValue.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelValue.setText("Value");
    jButtonAddEnvironment
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jButtonAddEnvironmentEvent(e);
          }
        });
    jButtonAddEnvironment.setText("Add");
    jButtonClearEnvironment
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jButtonClearEnvironmentEvent(e);
          }
        });
    jButtonClearEnvironment.setText("Clear");
    jTextFieldValue.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldValue);
      }
    });
    jTextFieldMyProxyServer.setText(GUIGlobalVars.getMyProxyServer());
    jTextFieldMyProxyServer.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldMyProxyServer);
      }
    });
    jScrollPaneEnvironment.getViewport().setBackground(Color.white);
    jButtonReplaceEnvironment.setText("Replace");
    jButtonReplaceEnvironment
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jButtonReplaceEvent(e);
          }
        });
    jTextFieldRetryCount.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        if (GUIGlobalVars.getGUIConfVarRetryCount() != GUIGlobalVars.NO_RETRY_COUNT) {
          GraphicUtils.jTextFieldFocusLost(jTextFieldRetryCount, Utils.INTEGER,
              Integer.toString(GUIGlobalVars.getGUIConfVarRetryCount()),
              Utils.RETRYCOUNT_MIN_VAL, Utils.RETRYCOUNT_MAX_VAL);
        } else {
          GraphicUtils.jTextFieldFocusLost(jTextFieldRetryCount, Utils.INTEGER,
              Integer.toString(Utils.RETRYCOUNT_DEF_VAL),
              Utils.RETRYCOUNT_MIN_VAL, Utils.RETRYCOUNT_MAX_VAL);
        }
      }

      public void focusGained(FocusEvent e) {
      }
    });
    if (GUIGlobalVars.getGUIConfVarRetryCount() != GUIGlobalVars.NO_RETRY_COUNT) {
      jTextFieldRetryCount.setText(Integer.toString(GUIGlobalVars
          .getGUIConfVarRetryCount()));
      setRetryCountEnabled(true);
    } else {
      jTextFieldRetryCount.setText(Integer.toString(Utils.RETRYCOUNT_DEF_VAL));
      setRetryCountEnabled(false);
    }
    jTextFieldRetryCount.setHorizontalAlignment(SwingConstants.RIGHT);
    jCheckBoxRetryCount.setText("RetryCount");
    jCheckBoxRetryCount.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jCheckBoxRetryCountEvent(e);
      }
    });
    jTextFieldHLRLocation.setText(GUIGlobalVars.getHLRLocation());
    jTextFieldHLRLocation.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jTextFieldHLRLocationFocusLost(e);
      }
    });
    jScrollPaneEnvironment.getViewport().add(jTableEnvironment, null);
    upRetryCount.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        if (GUIGlobalVars.getGUIConfVarRetryCount() != GUIGlobalVars.NO_RETRY_COUNT) {
          Utils.upButtonEvent(jTextFieldRetryCount, Utils.INTEGER, Integer
              .toString(GUIGlobalVars.getGUIConfVarRetryCount()),
              Utils.RETRYCOUNT_MIN_VAL, Utils.RETRYCOUNT_MAX_VAL);
        } else {
          Utils.upButtonEvent(jTextFieldRetryCount, Utils.INTEGER, Integer
              .toString(Utils.RETRYCOUNT_DEF_VAL), Utils.RETRYCOUNT_MIN_VAL,
              Utils.RETRYCOUNT_MAX_VAL);
        }
      }
    });
    downRetryCount.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        if (GUIGlobalVars.getGUIConfVarRetryCount() != GUIGlobalVars.NO_RETRY_COUNT) {
          Utils.downButtonEvent(jTextFieldRetryCount, Utils.INTEGER, Integer
              .toString(GUIGlobalVars.getGUIConfVarRetryCount()),
              Utils.RETRYCOUNT_MIN_VAL, Utils.RETRYCOUNT_MAX_VAL);
        } else {
          Utils.downButtonEvent(jTextFieldRetryCount, Utils.INTEGER, Integer
              .toString(Utils.RETRYCOUNT_DEF_VAL), Utils.RETRYCOUNT_MIN_VAL,
              Utils.RETRYCOUNT_MAX_VAL);
        }
      }
    });
    jTableEnvironment.addMouseListener(new MouseAdapter() {
      public void mouseClicked(MouseEvent me) {
        if (me.getClickCount() == 2) {
          Point point = me.getPoint();
          int row = jTableEnvironment.rowAtPoint(point);
          int column = jTableEnvironment.columnAtPoint(point);
          String variableName = jobTableModel.getValueAt(row,
              VARIABLE_NAME_COLUMN_INDEX).toString().trim();
          String variableValue = jobTableModel.getValueAt(row,
              VARIABLE_VALUE_COLUMN_INDEX).toString().trim();
          jTextFieldVarName.setText(variableName);
          jTextFieldValue.setText(variableValue);
        }
      }
    });
    GridBagLayout gbl = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.insets = new Insets(3, 3, 3, 3);
    // jPanelEnvironment
    jPanelEnvironment.setLayout(gbl);
    jPanelEnvironment.setBorder(new TitledBorder(new EtchedBorder(),
        " Environment Variables Needed ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelEnvironment.add(jLabelVarName, GraphicUtils.setGridBagConstraints(
        gbc, 0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.NONE, null, 0, 0));
    jPanelEnvironment.add(jTextFieldVarName, GraphicUtils
        .setGridBagConstraints(gbc, 1, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 160, 0));
    jPanelEnvironment.add(jLabelValue, GraphicUtils.setGridBagConstraints(gbc,
        2, 0, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.NONE, null, 0, 0));
    jPanelEnvironment.add(jTextFieldValue, GraphicUtils.setGridBagConstraints(
        gbc, 3, 0, 2, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelEnvironment.add(jScrollPaneEnvironment, GraphicUtils
        .setGridBagConstraints(gbc, 0, 1, 4, 4, 1.0, 1.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.BOTH, null,
            0, 0));
    jPanelEnvironment.add(jButtonAddEnvironment, GraphicUtils
        .setGridBagConstraints(gbc, 4, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    jPanelEnvironment.add(jButtonReplaceEnvironment, GraphicUtils
        .setGridBagConstraints(gbc, 4, 2, 1, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    jPanelEnvironment.add(jButtonRemoveEnvironment, GraphicUtils
        .setGridBagConstraints(gbc, 4, 3, 1, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    jPanelEnvironment.add(jButtonClearEnvironment, GraphicUtils
        .setGridBagConstraints(gbc, 4, 4, 1, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    // jPanelMyProxyServer
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelMyProxyServer.setLayout(gbl);
    jPanelMyProxyServer.setBorder(new TitledBorder(new EtchedBorder(),
        " Server for Credential Delegation ( MyProxyServer ) ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelMyProxyServer.add(jTextFieldMyProxyServer, GraphicUtils
        .setGridBagConstraints(gbc, 0, 0, 1, 1, 1.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    // jPanelHLRLocation
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelHLRLocation.setLayout(gbl);
    jPanelHLRLocation.setBorder(new TitledBorder(new EtchedBorder(),
        " HLRLocation ", 0, 0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelHLRLocation.add(jTextFieldHLRLocation, GraphicUtils
        .setGridBagConstraints(gbc, 0, 0, 1, 1, 1.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    // jPanelRetryCount
    jPanelRetryCount.setBorder(new TitledBorder(new EtchedBorder(),
        " Max Number of Submission Retries ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    JPanel jPanelArrowButton = new JPanel();
    jTextFieldRetryCount
        .setPreferredSize(GraphicUtils.NUMERIC_TEXT_FIELD_DIMENSION);
    jPanelArrowButton.setLayout(new BoxLayout(jPanelArrowButton,
        BoxLayout.Y_AXIS));
    jPanelArrowButton.add(upRetryCount);
    jPanelArrowButton.add(downRetryCount);
    jPanelRetryCount.add(jCheckBoxRetryCount);
    jPanelRetryCount.add(jTextFieldRetryCount);
    jPanelRetryCount.add(jPanelArrowButton);
    // this
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    this.setLayout(gbl);
    this.add(jPanelEnvironment, GraphicUtils.setGridBagConstraints(gbc, 0, 0,
        2, 1, 1.0, 0.4, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, null, 0, 0));
    this.add(jPanelMyProxyServer, GraphicUtils.setGridBagConstraints(gbc, 0, 1,
        2, 1, 1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    this.add(jPanelHLRLocation, GraphicUtils.setGridBagConstraints(gbc, 0, 2,
        2, 1, 1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    this.add(jPanelRetryCount, GraphicUtils.setGridBagConstraints(gbc, 0, 3, 1,
        1, 0.3, 0.6, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 10));
    this.add(new JPanel(), GraphicUtils.setGridBagConstraints(gbc, 1, 3, 1, 1,
        0.7, 0.0, GridBagConstraints.FIRST_LINE_START, GridBagConstraints.BOTH,
        null, 0, 10));
  }

  String jButtonJobDef2ViewEvent(boolean showWarningMsg, boolean showErrorMsg,
      ActionEvent e) {
    JobAd jobAdCheck = new JobAd();
    int itemsCount = jTableEnvironment.getRowCount();
    String result = "";
    warningMsg = "";
    errorMsg = "";
    if (itemsCount != 0) {
      result += Jdl.ENVIRONMENT + " = ";
      if (itemsCount == 1) {
        result += "\""
            + jTableEnvironment.getValueAt(0, VARIABLE_NAME_COLUMN_INDEX)
                .toString()
            + "="
            + jTableEnvironment.getValueAt(0, VARIABLE_VALUE_COLUMN_INDEX)
                .toString() + "\";\n";
      } else {
        result += "{";
        for (int i = 0; i < itemsCount - 1; i++) {
          result += "\""
              + jTableEnvironment.getValueAt(i, VARIABLE_NAME_COLUMN_INDEX)
                  .toString()
              + "="
              + jTableEnvironment.getValueAt(i, VARIABLE_VALUE_COLUMN_INDEX)
                  .toString() + "\", ";
        }
        result += "\""
            + jTableEnvironment.getValueAt(itemsCount - 1,
                VARIABLE_NAME_COLUMN_INDEX).toString()
            + "="
            + jTableEnvironment.getValueAt(itemsCount - 1,
                VARIABLE_VALUE_COLUMN_INDEX).toString() + "\"};\n";
      }
    }
    String myProxyServerText = jTextFieldMyProxyServer.getText().trim();
    if (!myProxyServerText.equals("")) {
      if (checkAttributeSet(Jdl.MYPROXY, myProxyServerText)) {
        result += Jdl.MYPROXY + " = \"" + myProxyServerText + "\";\n";
      }
    }
    String hlrLocation = jTextFieldHLRLocation.getText().trim();
    if (!hlrLocation.equals("")) {
      if (checkAttributeSet(Jdl.HLR_LOCATION, hlrLocation)) {
        result += Jdl.HLR_LOCATION + " = \"" + hlrLocation + "\";\n";
      }
    }
    if (jCheckBoxRetryCount.isSelected()) {
      if (checkAttributeSet(Jdl.RETRYCOUNT, Integer.parseInt(
          jTextFieldRetryCount.getText().trim(), 10))) {
        result += Jdl.RETRYCOUNT + " = "
            + jTextFieldRetryCount.getText().trim() + ";\n";
      }
    }
    warningMsg = ExprChecker.checkResult(result,
        Utils.jobDefinition2AttributeArray);
    errorMsg = errorMsg.trim();
    warningMsg = warningMsg.trim();
    if (!errorMsg.equals("") && showErrorMsg) {
      GraphicUtils.showOptionDialogMsg(JobDef2Panel.this, errorMsg,
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE, null,
          null);
    } else {
      if (!warningMsg.equals("") && showWarningMsg) {
        GraphicUtils.showOptionDialogMsg(JobDef2Panel.this, warningMsg,
            Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
            null, null);
      }
      jint.setJTextAreaJDL(result);
    }
    return result;
  }

  boolean checkAttributeSet(String attribute, String value) {
    JobAd jobAdCheck = new JobAd();
    try {
      jobAdCheck.setAttribute(attribute, value);
    } catch (IllegalArgumentException iae) {
      errorMsg += "- " + iae.getMessage() + "\n";
      return false;
    } catch (InvalidAttributeValueException iave) {
      errorMsg += "- " + iave.getMessage() + "\n";
      return false;
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
      JOptionPane.showOptionDialog(component, ex.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
    }
    return true;
  }

  boolean checkAttributeSet(String attribute, int value) {
    JobAd jobAdCheck = new JobAd();
    try {
      jobAdCheck.setAttribute(attribute, value);
    } catch (IllegalArgumentException iae) {
      errorMsg += "- " + iae.getMessage() + "\n";
      return false;
    } catch (InvalidAttributeValueException iave) {
      errorMsg += "- " + iave.getMessage() + "\n";
      return false;
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
      JOptionPane.showOptionDialog(component, ex.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
    }
    return true;
  }

  void jButtonJobDef2ResetEvent(ActionEvent e) {
    jobTableModel.removeAllRows();
    jTextFieldVarName.setText("");
    jTextFieldValue.setText("");
    jTextFieldHLRLocation.setText(GUIGlobalVars.getHLRLocation());
    jTextFieldMyProxyServer.setText(GUIGlobalVars.getMyProxyServer());
    if (GUIGlobalVars.getGUIConfVarRetryCount() != GUIGlobalVars.NO_RETRY_COUNT) {
      jTextFieldRetryCount.setText(Integer.toString(GUIGlobalVars
          .getGUIConfVarRetryCount()));
      setRetryCountEnabled(true);
    } else {
      jTextFieldRetryCount.setText(Integer.toString(Utils.RETRYCOUNT_DEF_VAL));
      setRetryCountEnabled(false);
    }
    jint.setJTextAreaJDL("");
    jTextFieldVarName.grabFocus();
  }

  String getWarningMsg() {
    return warningMsg;
  }

  void jButtonAddEnvironmentEvent(ActionEvent e) {
    String varName = jTextFieldVarName.getText().trim();
    String value = jTextFieldValue.getText().trim();
    if ((!varName.equals("")) && (value.equals(""))) {
      jTextFieldValue.grabFocus();
      JOptionPane.showOptionDialog(JobDef2Panel.this,
          "Value field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldValue.setText("");
    } else if ((varName.equals("")) && (!value.equals(""))) {
      jTextFieldVarName.grabFocus();
      JOptionPane.showOptionDialog(JobDef2Panel.this,
          "Name field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldVarName.setText("");
    } else if ((!varName.equals("")) && (!value.equals(""))) {
      if (!jobTableModel.isElementPresentInColumn(varName,
          VARIABLE_NAME_COLUMN_INDEX)) {
        jTextFieldVarName.grabFocus();
        JobAd jobAd = new JobAd();
        try {
          jobAd.addAttribute(Jdl.ENVIRONMENT, varName + "=" + value);
          Vector rowElement = new Vector();
          rowElement.addElement(varName);
          rowElement.addElement(value);
          jobTableModel.addRow(rowElement);
        } catch (IllegalArgumentException iae) {
          JOptionPane.showOptionDialog(component, "- " + iae.getMessage()
              + "\n", Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
        } catch (InvalidAttributeValueException iave) {
          JOptionPane.showOptionDialog(component, "- " + iave.getMessage()
              + "\n", Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
          JOptionPane.showOptionDialog(component, ex.getMessage(),
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
        }
        jTextFieldVarName.selectAll();
      } else {
        jTextFieldVarName.grabFocus();
        JOptionPane.showOptionDialog(JobDef2Panel.this, "Variable '" + varName
            + "' is alredy defined", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        if (value.equals("")) {
          jTextFieldValue.setText("");
        }
        jTextFieldVarName.selectAll();
      }
    } else {
      jTextFieldVarName.setText("");
      jTextFieldValue.setText("");
    }
  }

  void jButtonClearEnvironmentEvent(ActionEvent e) {
    jTextFieldVarName.grabFocus();
    if (jobTableModel.getRowCount() != 0) {
      int choice = JOptionPane.showOptionDialog(component,
          "Clear Environment Variables Table?", "Confirm Clear",
          JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null,
          null);
      if (choice == 0) {
        jobTableModel.removeAllRows();
      }
    }
    jTextFieldVarName.selectAll();
  }

  String getErrorMsg() {
    return errorMsg;
  }

  void jButtonRemoveEnvironmentEvent(ActionEvent e) {
    int[] selectedRow = jTableEnvironment.getSelectedRows();
    int selectedRowCount = selectedRow.length;
    jTextFieldVarName.grabFocus();
    if (selectedRowCount != 0) {
      for (int i = selectedRowCount - 1; i >= 0; i--) {
        jobTableModel.removeRow(selectedRow[i]);
      }
      if (jobTableModel.getRowCount() != 0) {
        int selectableRow = selectedRow[selectedRowCount - 1] + 1
            - selectedRowCount; // Next.
        if (selectableRow > jobTableModel.getRowCount() - 1) {
          selectableRow--; // Prev. (selectedRow[selectedRowCount - 1] -
          // selectedRowCount).
        }
        jTableEnvironment.setRowSelectionInterval(selectableRow, selectableRow);
      }
    } else {
      JOptionPane.showOptionDialog(JobDef2Panel.this,
          "You have to select a table row first", Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
          null, null);
    }
    jTextFieldVarName.selectAll();
  }

  void jButtonReplaceEvent(ActionEvent ae) {
    jTextFieldVarName.grabFocus();
    if (jTableEnvironment.getSelectedRowCount() == 0) {
      JOptionPane.showOptionDialog(JobDef2Panel.this,
          "Please first select from table a row to replace",
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
      jTextFieldVarName.selectAll();
      return;
    } else if (jTableEnvironment.getSelectedRowCount() != 1) {
      JOptionPane.showOptionDialog(JobDef2Panel.this,
          "Please select from table a single row to replace",
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
      jTextFieldVarName.selectAll();
      return;
    }
    int selectedRow = jTableEnvironment.getSelectedRow();
    String selectedVarName = jobTableModel.getValueAt(selectedRow,
        VARIABLE_NAME_COLUMN_INDEX).toString().trim();
    String selectedValue = jobTableModel.getValueAt(selectedRow,
        VARIABLE_VALUE_COLUMN_INDEX).toString().trim();
    String varName = jTextFieldVarName.getText().trim();
    if (!varName.equals("")) {
      if (!selectedVarName.equals(varName)
          && jobTableModel.isElementPresentInColumn(varName,
              VARIABLE_NAME_COLUMN_INDEX)) {
        JOptionPane.showOptionDialog(JobDef2Panel.this,
            "Inserted Variable is already present", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        jTextFieldVarName.selectAll();
        return;
      }
      String value = jTextFieldValue.getText().trim();
      if (!value.equals("")) {
        jobTableModel.setValueAt(varName, selectedRow,
            VARIABLE_NAME_COLUMN_INDEX);
        jobTableModel.setValueAt(value, selectedRow,
            VARIABLE_VALUE_COLUMN_INDEX);
        jTextFieldVarName.selectAll();
      } else {
        jTextFieldValue.grabFocus();
        JOptionPane.showOptionDialog(JobDef2Panel.this,
            "Value field cannot be blank", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        jTextFieldValue.setText("");
      }
    } else {
      JOptionPane.showOptionDialog(JobDef2Panel.this,
          "Name field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldVarName.setText("");
    }
  }

  void jButtonEditEvent(ActionEvent e) {
    int[] selectedRow = jTableEnvironment.getSelectedRows();
    int selectedRowCount = selectedRow.length;
    jTextFieldVarName.grabFocus();
    if (selectedRowCount == 1) {
      jTextFieldVarName.setText(jTableEnvironment.getModel().getValueAt(
          selectedRow[0], VARIABLE_NAME_COLUMN_INDEX).toString());
      jTextFieldValue.setText(jTableEnvironment.getModel().getValueAt(
          selectedRow[0], VARIABLE_VALUE_COLUMN_INDEX).toString());
      jTextFieldVarName.selectAll();
    } else if (selectedRowCount > 1) {
      JOptionPane.showOptionDialog(JobDef2Panel.this,
          "You have to select a single table row", Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
          null, null);
      jTableEnvironment.clearSelection();
    } else {
      JOptionPane.showOptionDialog(JobDef2Panel.this,
          "You have to select a table row first", Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
          null, null);
    }
  }

  void jCheckBoxRetryCountEvent(ActionEvent e) {
    boolean bool = false;
    if (jCheckBoxRetryCount.isSelected()) {
      bool = true;
    }
    jTextFieldRetryCount.setEnabled(bool);
    upRetryCount.setEnabled(bool);
    downRetryCount.setEnabled(bool);
  }

  void jTextFieldRetryCountFocusGained(FocusEvent e) {
  }

  void jTableEnvironmentFocusLost(FocusEvent e) {
    Component oppositeComponent = e.getOppositeComponent();
    if ((oppositeComponent != jButtonRemoveEnvironment)
        && (oppositeComponent != jButtonReplaceEnvironment)) {
      jTableEnvironment.clearSelection();
    }
  }

  void setHLRLocationText(String value) {
    jTextFieldHLRLocation.setText(value);
  }

  void setRetryCountValue(String text) {
    jCheckBoxRetryCount.setSelected(true);
    jTextFieldRetryCount.setEnabled(true);
    jTextFieldRetryCount.setText(text);
    upRetryCount.setEnabled(true);
    downRetryCount.setEnabled(true);
  }

  void setRetryCountEnabled(boolean bool) {
    jCheckBoxRetryCount.setSelected(bool);
    jTextFieldRetryCount.setEnabled(bool);
    upRetryCount.setEnabled(bool);
    downRetryCount.setEnabled(bool);
  }

  void setEnvironmentList(Vector itemVector) {
    String item = "";
    jobTableModel.removeAllRows();
    for (int i = 0; i < itemVector.size(); i++) {
      item = itemVector.get(i).toString().trim();
      int itemLength = item.length();
      char currentChar;
      for (int j = 0; j < itemLength; j++) {
        currentChar = item.charAt(j);
        if (currentChar == '=') {
          String varName = item.substring(0, j).trim();
          String value = item.substring(j + 1).trim();
          Vector rowElement = new Vector();
          rowElement.addElement(varName);
          rowElement.addElement(value);
          jobTableModel.addRow(rowElement);
          break;
        }
      }
    }
  }

  void setMyProxyServerText(String text) {
    jTextFieldMyProxyServer.setText(text);
  }

  void jTextFieldHLRLocationFocusLost(FocusEvent e) {
    GraphicUtils.jTextFieldDeselect(jTextFieldHLRLocation);
  }
}