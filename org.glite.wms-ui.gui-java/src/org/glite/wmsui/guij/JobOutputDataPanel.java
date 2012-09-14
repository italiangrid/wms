/*
 * JobOutputDataPanel.java
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
import javax.swing.JButton;
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
import org.glite.jdl.Jdl;
import org.glite.jdl.JobAd;

/**
 * Implementation of the JobOutputDataPanel class.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class JobOutputDataPanel extends JPanel {
  static Logger logger = Logger.getLogger(JDLEditor.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int OUTPUT_FILE_COLUMN_INDEX = 0;

  static final int STORAGE_ELEMENT_COLUMN_INDEX = 1;

  static final int LOGICAL_FILE_NAME_COLUMN_INDEX = 2;

  String errorMsg = "";

  String warningMsg = "";

  JPanel jPanelOutputData = new JPanel();

  JScrollPane jScrollPaneOutputData = new JScrollPane();

  JTable jTableOutputData;

  JobTableModel jobTableModel;

  Vector vectorHeader = new Vector();

  JButton jButtonAdd = new JButton();

  JButton jButtonRemove = new JButton();

  JButton jButtonClear = new JButton();

  JTextField jTextFieldOutputFile = new JTextField();

  JTextField jTextFieldStorageElement = new JTextField();

  JTextField jTextFieldLogicalFileName = new JTextField();

  JLabel jLabelOutputFile = new JLabel();

  JLabel jLabelStorageElement = new JLabel();

  JLabel jLabelLogicalFileName = new JLabel();

  JPanel jPanelDataROSE = new JPanel();

  JTextField jTextFieldOutputSE = new JTextField();

  Component component;

  JDLEditorInterface jint;

  JButton jButtonEdit = new JButton();

  public JobOutputDataPanel(Component component) {
    this.component = component;
    if (component instanceof JDLEditor) {
      jint = (JDLEditor) component;
      /*
       } else if (component instanceof JDLEJInternalFrame) {
       jint = (JDLEJInternalFrame) component;
       } else if (component instanceof JDLEJApplet) {
       jint = (JDLEJApplet) component;
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
    vectorHeader.addElement("Output File");
    vectorHeader.addElement("Storage Element");
    vectorHeader.addElement("Logical File Name");
    jobTableModel = new JobTableModel(vectorHeader, 0);
    jTableOutputData = new JTable(jobTableModel);
    jTableOutputData.getTableHeader().setReorderingAllowed(false);
    jTextFieldStorageElement.setText("");
    jTextFieldStorageElement
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            GraphicUtils.jTextFieldDeselect(jTextFieldStorageElement);
          }
        });
    jTextFieldLogicalFileName.setText("");
    jTextFieldLogicalFileName
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            GraphicUtils.jTextFieldDeselect(jTextFieldLogicalFileName);
          }
        });
    jLabelOutputFile.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelOutputFile.setText("Output File");
    jLabelStorageElement.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelStorageElement.setText("Storage Element");
    jLabelLogicalFileName.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelLogicalFileName.setText("Logical File Name");
    jButtonEdit.setText("Replace");
    jButtonAdd.setText("Add");
    jButtonRemove.setText("Remove");
    jButtonClear.setText("Clear");
    jTextFieldOutputFile.setText("");
    jTextFieldOutputFile.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldOutputFile);
      }
    });
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
    jButtonClear.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonClearEvent(e);
      }
    });
    jButtonEdit.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonReplaceEvent(e);
      }
    });
    jTextFieldOutputSE.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldOutputSE);
      }
    });
    jTableOutputData.addMouseListener(new MouseAdapter() {
      public void mouseClicked(MouseEvent me) {
        if (me.getClickCount() == 2) {
          Point point = me.getPoint();
          int row = jTableOutputData.rowAtPoint(point);
          int column = jTableOutputData.columnAtPoint(point);
          String outputFile = jobTableModel.getValueAt(row,
              OUTPUT_FILE_COLUMN_INDEX).toString().trim();
          String storageElement = jobTableModel.getValueAt(row,
              STORAGE_ELEMENT_COLUMN_INDEX).toString().trim();
          String localFile = jobTableModel.getValueAt(row,
              LOGICAL_FILE_NAME_COLUMN_INDEX).toString().trim();
          jTextFieldOutputFile.setText(outputFile);
          jTextFieldStorageElement.setText(storageElement);
          jTextFieldLogicalFileName.setText(localFile);
        }
      }
    });
    GridBagLayout gbl = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.insets = new Insets(3, 3, 3, 3);
    // jPanelOutputData
    jPanelOutputData.setLayout(gbl);
    jPanelOutputData.setBorder(new TitledBorder(new EtchedBorder(),
        " Output Data ", 0, 0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelOutputData.add(jLabelOutputFile, GraphicUtils.setGridBagConstraints(
        gbc, 0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.EAST,
        GridBagConstraints.NONE, null, 0, 0));
    jPanelOutputData.add(jTextFieldOutputFile, GraphicUtils
        .setGridBagConstraints(gbc, 1, 0, 2, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    jPanelOutputData.add(jLabelStorageElement, GraphicUtils
        .setGridBagConstraints(gbc, 0, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.EAST, GridBagConstraints.NONE, null, 0, 0));
    jPanelOutputData.add(jTextFieldStorageElement, GraphicUtils
        .setGridBagConstraints(gbc, 1, 1, 2, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    jPanelOutputData.add(jLabelLogicalFileName, GraphicUtils
        .setGridBagConstraints(gbc, 0, 2, 1, 1, 0.0, 0.0,
            GridBagConstraints.EAST, GridBagConstraints.NONE, null, 0, 0));
    jPanelOutputData.add(jTextFieldLogicalFileName, GraphicUtils
        .setGridBagConstraints(gbc, 1, 2, 2, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    jScrollPaneOutputData.getViewport().add(jTableOutputData, null);
    jScrollPaneOutputData.getViewport().setBackground(Color.white);
    jPanelOutputData.add(jScrollPaneOutputData, GraphicUtils
        .setGridBagConstraints(gbc, 0, 3, 2, 4, 1.0, 1.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.BOTH, null,
            0, 0));
    jPanelOutputData.add(jButtonAdd, GraphicUtils.setGridBagConstraints(gbc, 2,
        3, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelOutputData.add(jButtonEdit, GraphicUtils.setGridBagConstraints(gbc,
        2, 4, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelOutputData.add(jButtonRemove, GraphicUtils.setGridBagConstraints(gbc,
        2, 5, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelOutputData.add(jButtonClear, GraphicUtils.setGridBagConstraints(gbc,
        2, 6, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    // jPanelDataROSE
    jPanelDataROSE.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelDataROSE.setBorder(new TitledBorder(new EtchedBorder(),
        " Output Storage Element ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelDataROSE.add(jTextFieldOutputSE, GraphicUtils.setGridBagConstraints(
        gbc, 0, 0, 1, 1, 1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    // this
    this.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    this.add(jPanelOutputData, GraphicUtils.setGridBagConstraints(gbc, 0, 0, 1,
        1, 1.0, 0.3, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, new Insets(1, 1, 1, 1), 0, 0));
    this.add(jPanelDataROSE, GraphicUtils.setGridBagConstraints(gbc, 0, 1, 1,
        1, 0.0, 0.7, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
  }

  void jButtonAddEvent(ActionEvent e) {
    String outputFile = jTextFieldOutputFile.getText().trim();
    jTextFieldOutputFile.grabFocus();
    if (!outputFile.equals("")) {
      if (!jobTableModel.isElementPresentInColumn(outputFile,
          OUTPUT_FILE_COLUMN_INDEX)) {
        String storageElement = jTextFieldStorageElement.getText().trim();
        String logicalFileName = jTextFieldLogicalFileName.getText().trim();
        if (!logicalFileName.equals("")
            && jobTableModel.isElementPresentInColumn(logicalFileName,
                LOGICAL_FILE_NAME_COLUMN_INDEX)) {
          jTextFieldLogicalFileName.grabFocus();
          JOptionPane.showOptionDialog(JobOutputDataPanel.this,
              "Inserted Logical File Name is already present",
              Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.WARNING_MESSAGE, null, null, null);
          jTextFieldLogicalFileName.selectAll();
          return;
        }
        if (!jobTableModel.isRowPresent(outputFile + storageElement
            + logicalFileName)) {
          Vector rowToAdd = new Vector();
          rowToAdd.add(outputFile);
          rowToAdd.add(storageElement);
          rowToAdd.add(logicalFileName);
          jobTableModel.addRow(rowToAdd);
        } else {
          JOptionPane.showOptionDialog(JobOutputDataPanel.this,
              "Inserted values are already present", Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
              null, null);
        }
      } else {
        JOptionPane.showOptionDialog(JobOutputDataPanel.this,
            "Inserted Output File is already present", Utils.WARNING_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null,
            null, null);
      }
    } else {
      JOptionPane.showOptionDialog(JobOutputDataPanel.this,
          "Output File field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldOutputFile.setText("");
    }
    jTextFieldOutputFile.selectAll();
  }

  void jButtonRemoveEvent(ActionEvent e) {
    int[] selectedRow = jTableOutputData.getSelectedRows();
    int selectedRowCount = selectedRow.length;
    jTextFieldOutputFile.grabFocus();
    if (selectedRowCount != 0) {
      for (int i = selectedRowCount - 1; i >= 0; i--) {
        jobTableModel.removeRow(selectedRow[i]);
      }
      if (jobTableModel.getRowCount() != 0) {
        int selectableRow = selectedRow[selectedRowCount - 1] + 1
            - selectedRowCount; // Next.
        if (selectableRow > jobTableModel.getRowCount() - 1) {
          selectableRow--; // Prev. (selectedRow[selectedRowCount - 1] - selectedRowCount).
        }
        jTableOutputData.setRowSelectionInterval(selectableRow, selectableRow);
      }
    } else {
      JOptionPane.showOptionDialog(this, Utils.SELECT_AN_ITEM,
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
    }
    jTextFieldOutputFile.selectAll();
  }

  void jButtonClearEvent(ActionEvent e) {
    jTextFieldOutputFile.grabFocus();
    if (jobTableModel.getRowCount() != 0) {
      int choice = JOptionPane.showOptionDialog(this,
          "Clear Output Data table?", "Confirm Clear",
          JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null,
          null);
      if (choice == 0) {
        jobTableModel.removeAllRows();
      }
    }
    jTextFieldOutputFile.selectAll();
  }

  void jButtonReplaceEvent(ActionEvent e) {
    if (jTableOutputData.getSelectedRowCount() == 0) {
      JOptionPane.showOptionDialog(JobOutputDataPanel.this,
          "Please first select from table a row to replace",
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
      return;
    } else if (jTableOutputData.getSelectedRowCount() != 1) {
      JOptionPane.showOptionDialog(JobOutputDataPanel.this,
          "Please select from table a single row to replace",
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
      return;
    }
    int selectedRow = jTableOutputData.getSelectedRow();
    String selectedOutputFile = jobTableModel.getValueAt(selectedRow,
        OUTPUT_FILE_COLUMN_INDEX).toString().trim();
    String selectedLogicalFileName = jobTableModel.getValueAt(selectedRow,
        LOGICAL_FILE_NAME_COLUMN_INDEX).toString().trim();
    String outputFile = jTextFieldOutputFile.getText().trim();
    jTextFieldOutputFile.grabFocus();
    if (!outputFile.equals("")) {
      if (!selectedOutputFile.equals(outputFile)
          && jobTableModel.isElementPresentInColumn(outputFile,
              OUTPUT_FILE_COLUMN_INDEX)) {
        jTextFieldOutputFile.grabFocus();
        JOptionPane.showOptionDialog(JobOutputDataPanel.this,
            "Inserted Output File is already present", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        jTextFieldOutputFile.selectAll();
        return;
      }
      String storageElement = jTextFieldStorageElement.getText().trim();
      String logicalFileName = jTextFieldLogicalFileName.getText().trim();
      if (!logicalFileName.equals("")) {
        if (!selectedLogicalFileName.equals(logicalFileName)
            && jobTableModel.isElementPresentInColumn(logicalFileName,
                LOGICAL_FILE_NAME_COLUMN_INDEX)) {
          jTextFieldLogicalFileName.grabFocus();
          JOptionPane.showOptionDialog(JobOutputDataPanel.this,
              "Inserted Logical File Name is already present",
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
          jTextFieldLogicalFileName.selectAll();
          return;
        }
      }
      jobTableModel.setValueAt(outputFile, selectedRow,
          OUTPUT_FILE_COLUMN_INDEX);
      jobTableModel.setValueAt(storageElement, selectedRow,
          STORAGE_ELEMENT_COLUMN_INDEX);
      jobTableModel.setValueAt(logicalFileName, selectedRow,
          LOGICAL_FILE_NAME_COLUMN_INDEX);
    } else {
      JOptionPane.showOptionDialog(JobOutputDataPanel.this,
          "Output File field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldOutputFile.setText("");
    }
    jTextFieldOutputFile.selectAll();
  }

  /*
   void jButtonEditEvent(ActionEvent e) {
   int[] selectedRow = jTableOutputData.getSelectedRows();
   int selectedRowCount = selectedRow.length;
   if(selectedRowCount == 1) {
   jTextFieldOutputFile.setText(jTableOutputData.getModel().getValueAt(selectedRow[0],
   OUTPUT_FILE_COLUMN_INDEX).toString());
   jTextFieldStorageElement.setText(jTableOutputData.getModel().getValueAt(selectedRow[0],
   STORAGE_ELEMENT_COLUMN_INDEX).toString());
   jTextFieldLogicalFileName.setText(jTableOutputData.getModel().getValueAt(selectedRow[0],
   LOGICAL_FILE_NAME_COLUMN_INDEX).toString());
   jTextFieldOutputFile.selectAll();
   jTextFieldOutputFile.grabFocus();
   //isEditing = true;
   } else if(selectedRowCount > 1) {
   JOptionPane.showOptionDialog(JobOutputDataPanel.this,
   "Please select a single table row",
   Utils.INFORMATION_MSG_TXT,
   JOptionPane.DEFAULT_OPTION,
   JOptionPane.INFORMATION_MESSAGE,
   null, null, null);
   jTableOutputData.clearSelection();
   } else {
   JOptionPane.showOptionDialog(JobOutputDataPanel.this,
   "Please select a table row first",
   Utils.INFORMATION_MSG_TXT,
   JOptionPane.DEFAULT_OPTION,
   JOptionPane.INFORMATION_MESSAGE,
   null, null, null);
   }
   }
   */
  String jButtonJobOutputDataViewEvent(boolean showWarningMsg,
      boolean showErrorMsg, ActionEvent e) {
    String result = "";
    warningMsg = "";
    errorMsg = "";
    JobAd jobAdCheck = new JobAd();
    String outputFile;
    String storageElement;
    String logicalFileName;
    String outputDataResult = "";
    int rowCount = jobTableModel.getRowCount();
    for (int i = 0; i < rowCount; i++) {
      outputFile = jobTableModel.getValueAt(i, OUTPUT_FILE_COLUMN_INDEX)
          .toString().trim();
      storageElement = jobTableModel
          .getValueAt(i, STORAGE_ELEMENT_COLUMN_INDEX).toString().trim();
      logicalFileName = jobTableModel.getValueAt(i,
          LOGICAL_FILE_NAME_COLUMN_INDEX).toString().trim();
      //!!!outputDataResult += "[ OutputFile = \"" + outputFile + "\";\n";  Use this line!!!
      outputDataResult += "[ " + Utils.OUTPUT_FILE + " = \"" + outputFile
          + "\";\n";
      if (!storageElement.equals("")) {
        outputDataResult += Utils.STORAGE_ELEMENT + " = \"" + storageElement
            + "\";\n";
      }
      if (!logicalFileName.equals("")) {
        outputDataResult += Utils.LOGICAL_FILE_NAME + " = \""
            + JobInputDataPanel.LFN_PREFIX + logicalFileName + "\";\n";
      }
      //!!!outputDataResult += "],\n";  Use this line!!!
      outputDataResult += "],\n";
    }
    if (!outputDataResult.equals("")) {
      outputDataResult = outputDataResult.substring(0, outputDataResult
          .lastIndexOf(","));
      if (rowCount == 1) {
        outputDataResult = Jdl.OUTPUTDATA + " = " + outputDataResult + ";\n";
      } else {
        outputDataResult = Jdl.OUTPUTDATA + " = {\n" + outputDataResult
            + "};\n";
      }
      result += outputDataResult;
    }
    String outputSEText = jTextFieldOutputSE.getText().trim();
    if (!outputSEText.equals("")) {
      if (checkAttributeSet(Jdl.OUTPUT_SE, outputSEText)) {
        ;
      }
      result += Jdl.OUTPUT_SE + " = \"" + outputSEText + "\";\n";
    }
    warningMsg = ExprChecker.checkResult(result,
        Utils.jobOutputDataAttributeArray);
    errorMsg = errorMsg.trim();
    warningMsg = warningMsg.trim();
    if (!errorMsg.equals("") && showErrorMsg) {
      GraphicUtils.showOptionDialogMsg(JobOutputDataPanel.this, errorMsg,
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE, null,
          null);
    } else {
      if (!warningMsg.equals("") && showWarningMsg) {
        GraphicUtils.showOptionDialogMsg(JobOutputDataPanel.this, warningMsg,
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

  String getErrorMsg() {
    return errorMsg;
  }

  String getWarningMsg() {
    return warningMsg;
  }

  void jButtonJobOutputDataResetEvent(ActionEvent e) {
    jTextFieldOutputFile.setText("");
    jTextFieldStorageElement.setText("");
    jTextFieldLogicalFileName.setText("");
    jobTableModel.removeAllRows();
    jTextFieldOutputSE.setText("");
    jTextFieldOutputFile.grabFocus();
    jint.setJTextAreaJDL("");
  }

  void setOutputSEText(String text) {
    //dataReq.jCheckBoxOutputSE.setSelected(true);
    //dataReq.jTextFieldOutputSE.setEnabled(true);
    jTextFieldOutputSE.setText(text);
  }

  void setOutputData(Vector adVector) {
    String outputFile;
    String storageElement;
    String logicalFileName;
    Ad ad;
    jobTableModel.removeAllRows();
    for (int i = 0; i < adVector.size(); i++) {
      ad = (Ad) adVector.get(i);
      outputFile = "";
      storageElement = "";
      logicalFileName = "";
      try {
        outputFile = ad.getStringValue(Jdl.OD_OUTPUT_FILE).get(0).toString();
      } catch (NoSuchFieldException e) {
        // Attribute not present in jobad.
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(JobOutputDataPanel.this, e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
      try {
        storageElement = ad.getStringValue(Jdl.OD_STORAGE_ELEMENT).get(0)
            .toString();
      } catch (NoSuchFieldException e) {
        // Attribute not present in jobad.
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(JobOutputDataPanel.this, e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
      try {
        logicalFileName = ad.getStringValue(Jdl.OD_LOGICAL_FILENAME).get(0)
            .toString().substring(JobInputDataPanel.LFN_PREFIX.length());
      } catch (NoSuchFieldException e) {
        // Attribute not present in jobad.
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(JobOutputDataPanel.this, e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
      Vector elementToAdd = new Vector();
      elementToAdd.add(outputFile);
      elementToAdd.add(storageElement);
      elementToAdd.add(logicalFileName);
      jobTableModel.addRow(elementToAdd);
    }
  }
}