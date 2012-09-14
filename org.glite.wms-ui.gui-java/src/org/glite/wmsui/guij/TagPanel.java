/*
 * TagPanel.java
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
import java.util.Iterator;
import java.util.Vector;

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
 * Implementation of the TagPanel class.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class TagPanel extends JPanel {
  static Logger logger = Logger.getLogger(JDLEditor.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;
  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int TAG_NAME_COLUMN_INDEX = 0;
  static final int TAG_VALUE_COLUMN_INDEX = 1;

  private String errorMsg = "";
  private String warningMsg = "";

  Vector vectorHeader = new Vector();

  JTable jTableTags;
  Component component;
  JDLEditorInterface jint;

  JobTableModel jobTableModel;
  JPanel jPanelTags = new JPanel();
  JButton jButtonAdd = new JButton();
  JButton jButtonRemove = new JButton();
  JButton jButtonClear = new JButton();
  JScrollPane jScrollPaneTags = new JScrollPane();
  JTextField jTextFieldTagName = new JTextField();
  JTextField jTextFieldTagValue = new JTextField();
  JLabel jLabelTagName = new JLabel();
  JLabel jLabelTagValue = new JLabel();
  JButton jButtonReplace = new JButton();

  /**
   * Constructor.
   */
  public TagPanel(Component component) {
    this.component = component;
    if (component instanceof JDLEditor) {
      jint = (JDLEditor) component;
      /*
          } else if(component instanceof JDLEJInternalFrame) {
            jint = (JDLEJInternalFrame) component;
          } else if(component instanceof JDLEJApplet) {
            jint = (JDLEJApplet) component;
       */
    } else {
      System.exit( -1);
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
    isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG) ? true : false;

    vectorHeader.addElement("Tag Name");
    vectorHeader.addElement("Tag Value");
    jobTableModel = new JobTableModel(vectorHeader, 0);
    jTableTags = new JTable(jobTableModel);
    jTableTags.getTableHeader().setReorderingAllowed(false);
    jTextFieldTagName.setText("");
    jTextFieldTagName.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldTagName);
      }
    });
    jTextFieldTagValue.setText("");
    jTextFieldTagValue.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldTagValue);
      }
    });
    jTextFieldTagName.setText("");
    jTextFieldTagName.setHorizontalAlignment(SwingConstants.LEADING);
    jTextFieldTagValue.setText("");
    jTextFieldTagValue.setHorizontalAlignment(SwingConstants.LEADING);
    jButtonReplace.setText("Replace");
    jButtonAdd.setText("Add");
    jButtonRemove.setText("Remove");
    jButtonClear.setText("Clear");
    jLabelTagName.setRequestFocusEnabled(true);
    jLabelTagName.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelTagName.setText("Tag Name");
    jLabelTagValue.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelTagValue.setText("Tag Value");
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
    jButtonReplace.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonReplaceEvent(e);
      }
    });

    jTableTags.addMouseListener(new MouseAdapter() {
      public void mouseClicked(MouseEvent me) {
        if (me.getClickCount() == 2) {
          Point point = me.getPoint();
          int row = jTableTags.rowAtPoint(point);
          int column = jTableTags.columnAtPoint(point);
          jTextFieldTagName.setText(jobTableModel.getValueAt(row,
              TAG_NAME_COLUMN_INDEX)
              .toString().trim());
          jTextFieldTagValue.setText(jobTableModel.getValueAt(row,
              TAG_VALUE_COLUMN_INDEX)
              .toString().trim());
        }
      }
    });

    GridBagLayout gbl = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.insets = new Insets(3, 3, 3, 3);

    // jPanelEnvironment
    jPanelTags.setLayout(gbl);
    jPanelTags.setBorder(new TitledBorder(new EtchedBorder(),
        " User Tags ", 0, 0,
        null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));

    jPanelTags.add(jLabelTagName, GraphicUtils.setGridBagConstraints(
        gbc, 0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.NONE, null, 0, 0));

    jPanelTags.add(jTextFieldTagName, GraphicUtils.setGridBagConstraints(
        gbc, 1, 0, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 160, 0));

    jPanelTags.add(jLabelTagValue, GraphicUtils.setGridBagConstraints(
        gbc, 2, 0, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.NONE, null, 0, 0));

    jPanelTags.add(jTextFieldTagValue, GraphicUtils.setGridBagConstraints(
        gbc, 3, 0, 2, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));

    jScrollPaneTags.getViewport().add(jTableTags, null);
    jScrollPaneTags.getViewport().setBackground(Color.white);
    jPanelTags.add(jScrollPaneTags, GraphicUtils.setGridBagConstraints(
        gbc, 0, 1, 4, 4, 1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, null, 0, 0));

    jPanelTags.add(jButtonAdd, GraphicUtils.setGridBagConstraints(
        gbc, 4, 1, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));

    jPanelTags.add(jButtonReplace, GraphicUtils.setGridBagConstraints(
        gbc, 4, 2, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));

    jPanelTags.add(jButtonRemove, GraphicUtils.setGridBagConstraints(
        gbc, 4, 3, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));

    jPanelTags.add(jButtonClear, GraphicUtils.setGridBagConstraints(
        gbc, 4, 4, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
        GridBagConstraints.HORIZONTAL, null, 0, 0));

    //this.setLayout(new BorderLayout());
    //this.add(jPanelTags, BorderLayout.CENTER);
    // this
    this.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    this.add(jPanelTags, GraphicUtils.setGridBagConstraints(
        gbc, 0, 0, 1, 1, 1.0, 1.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, new Insets(1, 1, 1, 1), 0, 0));
    /*
        JPanel jPanelFill = new JPanel();
        this.add(jPanelFill, GraphicUtils.setGridBagConstraints(
            gbc, 0, 1, 1, 1, 1.0, 0.8, GridBagConstraints.FIRST_LINE_START,
            GridBagConstraints.BOTH, null, 0, 0));*/

  }

  void jButtonAddEvent(ActionEvent e) {
    String tagName = jTextFieldTagName.getText().trim();
    String tagValue = jTextFieldTagValue.getText().trim();
    if ((!tagName.equals("")) && (tagValue.equals(""))) {
      jTextFieldTagValue.grabFocus();
      JOptionPane.showOptionDialog(TagPanel.this,
          "Tag Value field cannot be blank",
          Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE,
          null, null, null);
      jTextFieldTagValue.setText("");
    } else if ((tagName.equals("")) && (!tagValue.equals(""))) {
      jTextFieldTagName.grabFocus();
      JOptionPane.showOptionDialog(TagPanel.this,
          "Tag Name field cannot be blank",
          Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE,
          null, null, null);
      jTextFieldTagName.setText("");
    } else if ((!tagName.equals("")) && (!tagValue.equals(""))) {
      if (!jobTableModel.isElementPresentInColumn(tagName,
          TAG_NAME_COLUMN_INDEX)) {
        Vector rowElement = new Vector();
        rowElement.addElement(tagName);
        rowElement.addElement(tagValue);
        jobTableModel.addRow(rowElement);
        jTextFieldTagName.selectAll();
        jTextFieldTagName.grabFocus();
      } else {
        jTextFieldTagName.grabFocus();
        JOptionPane.showOptionDialog(TagPanel.this,
            "Inserted Tag Name is already present",
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        if (tagValue.equals("")) {
          jTextFieldTagValue.setText("");
        }
        jTextFieldTagName.selectAll();
      }
    } else {
      jTextFieldTagName.setText("");
      jTextFieldTagValue.setText("");
      jTextFieldTagName.grabFocus();
    }
  }

  void jButtonRemoveEvent(ActionEvent e) {
    int[] selectedRow = jTableTags.getSelectedRows();
    int selectedRowCount = selectedRow.length;
    if (selectedRowCount != 0) {
      for (int i = selectedRowCount - 1; i >= 0; i--) {
        jobTableModel.removeRow(selectedRow[i]);
      }
      if (jobTableModel.getRowCount() != 0) {
        int selectableRow = selectedRow[selectedRowCount - 1] + 1 -
            selectedRowCount; // Next.
        if (selectableRow > jobTableModel.getRowCount() - 1) {
          selectableRow--; // Prev. (selectedRow[selectedRowCount - 1] - selectedRowCount).
        }
        jTableTags.setRowSelectionInterval(selectableRow, selectableRow);
      }
    } else {
      jTextFieldTagName.grabFocus();
      JOptionPane.showOptionDialog(TagPanel.this,
          Utils.SELECT_AN_ITEM,
          Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE,
          null, null, null);
    }
    jTextFieldTagName.selectAll();
  }

  void jButtonClearEvent(ActionEvent e) {
    if (jobTableModel.getRowCount() != 0) {
      jTextFieldTagName.grabFocus();
      int choice = JOptionPane.showOptionDialog(this,
          "Clear User Tags table?",
          "Confirm Clear",
          JOptionPane.YES_NO_OPTION,
          JOptionPane.QUESTION_MESSAGE,
          null, null, null);
      if (choice == 0) {
        jobTableModel.removeAllRows();
      }
    }
    jTextFieldTagName.selectAll();
  }

  void jButtonReplaceEvent(ActionEvent e) {
    if (jTableTags.getSelectedRowCount() == 0) {
      JOptionPane.showOptionDialog(TagPanel.this,
          "Please first select a table row to replace",
          Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE,
          null, null, null);
      return;
    } else if (jTableTags.getSelectedRowCount() != 1) {
      JOptionPane.showOptionDialog(TagPanel.this,
          "Please select a single table row to replace",
          Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE,
          null, null, null);
      return;
    }
    int selectedRow = jTableTags.getSelectedRow();
    String selectedTagName = jobTableModel.getValueAt(selectedRow,
        TAG_NAME_COLUMN_INDEX).toString().trim();
    String selectedTagValue = jobTableModel.getValueAt(selectedRow,
        TAG_VALUE_COLUMN_INDEX).toString().trim();
    String tagName = jTextFieldTagName.getText().trim();
    if (!tagName.equals("")) {
      if (!selectedTagName.equals(tagName)
          && jobTableModel.isElementPresentInColumn(tagName,
          TAG_NAME_COLUMN_INDEX)) {
        jTextFieldTagName.grabFocus();
        JOptionPane.showOptionDialog(TagPanel.this,
            "Inserted Tag Name is already present",
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        jTextFieldTagName.selectAll();
        return;
      }
      String tagValue = jTextFieldTagValue.getText().trim();
      if (!tagValue.equals("")) {
        jobTableModel.setValueAt(tagName, selectedRow, TAG_NAME_COLUMN_INDEX);
        jobTableModel.setValueAt(tagValue, selectedRow, TAG_VALUE_COLUMN_INDEX);
        jTextFieldTagName.selectAll();
        jTextFieldTagName.grabFocus();
      } else {
        jTextFieldTagValue.grabFocus();
        JOptionPane.showOptionDialog(TagPanel.this,
            "Tag Value field cannot be blank",
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        jTextFieldTagValue.setText("");
      }
    } else {
      jTextFieldTagName.grabFocus();
      JOptionPane.showOptionDialog(TagPanel.this,
          "Tag Name field cannot be blank",
          Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE,
          null, null, null);
      jTextFieldTagName.setText("");
    }
  }

  String jButtonTagsViewEvent(boolean showWarningMsg, boolean showErrorMsg,
      ActionEvent e) {
    JobAd jobAdCheck = new JobAd();
    int itemsCount = jTableTags.getRowCount();
    String result = "";
    this.warningMsg = "";
    this.errorMsg = "";
    if (itemsCount != 0) {
      result += Jdl.USER_TAGS + " = ";
      result += "[";
      for (int i = 0; i < itemsCount - 1; i++) {
        result += jobTableModel.getValueAt(i, TAG_NAME_COLUMN_INDEX).toString()
            + " = \"" + jobTableModel.getValueAt(i,
            TAG_VALUE_COLUMN_INDEX).toString() + "\"; ";
      }
      result += jobTableModel.getValueAt(itemsCount - 1,
          TAG_NAME_COLUMN_INDEX).toString()
          + " = \"" + jobTableModel.getValueAt(itemsCount - 1,
          TAG_VALUE_COLUMN_INDEX).toString()
          + "\"];\n";
    }

    this.warningMsg = ExprChecker.checkResult(result, Utils.tagsAttributeArray);

    this.errorMsg = this.errorMsg.trim();
    this.warningMsg = this.warningMsg.trim();
    if (!this.errorMsg.equals("") && showErrorMsg) {
      GraphicUtils.showOptionDialogMsg(TagPanel.this, errorMsg,
          Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE,
          Utils.MESSAGE_LINES_PER_JOPTIONPANE, null, null);
    } else {
      if (!this.warningMsg.equals("") && showWarningMsg) {
        GraphicUtils.showOptionDialogMsg(TagPanel.this, warningMsg,
            Utils.WARNING_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE,
            Utils.MESSAGE_LINES_PER_JOPTIONPANE, null, null);
      }
      jint.setJTextAreaJDL(result);
    }
    return result;
  }

  String getErrorMsg() {
    return this.errorMsg;
  }

  String getWarningMsg() {
    return this.warningMsg;
  }

  void jButtonTagsResetEvent(ActionEvent e) {
    jint.setJTextAreaJDL("");
    jobTableModel.removeAllRows();
    jTextFieldTagName.setText("");
    jTextFieldTagValue.setText("");
    jTextFieldTagName.grabFocus();
  }

  void setUserTags(Ad adUserTags) {
    logger.debug("adUserTags: " + adUserTags);
    Iterator iterator = adUserTags.attributes();
    String tagName = "";
    String tagValue = "";
    while (iterator.hasNext()) {
      tagName = iterator.next().toString();
      try {
        tagValue = adUserTags.getStringValue(tagName).get(0).toString();
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        continue;
      }
      logger.debug("tagName: " + tagName + " tagValue: " + tagValue);
      Vector rowElement = new Vector();
      rowElement.addElement(tagName);
      rowElement.addElement(tagValue);
      jobTableModel.addRow(rowElement);
    }
  }

}
