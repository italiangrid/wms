/*
 * NSPanel.java 
 * 
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 * 
 */

package org.glite.wmsui.guij;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.URL;
import java.util.Date;
import java.util.Iterator;
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.ListSelectionModel;
import javax.swing.SwingConstants;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.table.JTableHeader;
import javax.swing.table.TableColumn;
import javax.swing.table.TableColumnModel;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Ad;
import org.glite.jdl.Jdl;
import org.glite.jdl.JobAd;
import org.glite.jdl.JobState;

/**
 * Implementation of the NSPanel class. The NS Panels contain some information
 * about the Network Servers and a table containing the jobs user want to
 * submit. This class provides a group of functionalities needed to operate on
 * the jobs contained in the panel.
 * 
 * @ingroup gui
 * @brief @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class NSPanel extends JPanel {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int JOB_NAME_COLUMN_INDEX = 0;

  static final String JOB_NAME_TABLE_HEADER = "Job Name";

  static final int JOB_ID_COLUMN_INDEX = 1;

  static final String JOB_ID_TABLE_HEADER = "Job Id";

  static final int JOB_SUBMIT_TIME_COLUMN_INDEX = 2;

  static final String JOB_SUBMIT_TIME_HEADER = "Submission Time";

  static final int JOB_TYPE_COLUMN_INDEX = 3;

  static final String JOB_TYPE_HEADER = "Job Type";

  // Table Column Sorting.
  int sortingColumn = Utils.NO_SORTING;

  boolean ascending = false;

  // The name of the NS panel.
  private String name;

  // Contains the names of all job added in the NS panel.
  private Vector jobNameVector = new Vector();

  // Table Header.
  private Vector vectorHeader = new Vector();

  // Popup menu.
  protected JPopupMenu jPopupMenuTable = new JPopupMenu();

  JMenuItem jMenuItemCut = new JMenuItem("Cut");

  JMenuItem jMenuItemCopy = new JMenuItem("Copy");

  JMenuItem jMenuItemPaste = new JMenuItem("Paste");

  protected JMenu jMenuCopyTo = new JMenu("     Copy to");

  protected JMenu jMenuMoveTo = new JMenu("     Move to");

  JMenuItem jMenuItemRemove = new JMenuItem("     Remove");

  JMenuItem jMenuItemClear = new JMenuItem("     Clear");

  JMenuItem jMenuItemRename = new JMenuItem("     Rename");

  JMenuItem jMenuItemSelectAll = new JMenuItem("     Select All");

  JMenuItem jMenuItemSelectNone = new JMenuItem("     Select None");

  JMenuItem jMenuItemInvertSelection = new JMenuItem("     Invert Selection");

  JMenuItem jMenuItemSelectSubmitted = new JMenuItem("     Select Submitted");

  JMenuItem jMenuItemSubmit = new JMenuItem("     Submit");

  JMenuItem jMenuItemSendToMonitor = new JMenuItem("     Send to Monitor");

  JMenuItem jMenuItemOpenInEditor = new JMenuItem("     Open in Editor");

  JMenuItem jMenuItemInteractiveConsole = new JMenuItem(
      "     Interactive Console");

  protected JMenu jMenuCheckpoint = new JMenu("     Checkpoint");

  JMenuItem jMenuItemAttachCheckpointState = new JMenuItem(
      "Link Checkpoint State");

  JMenuItem jMenuItemDetachCheckpointState = new JMenuItem(
      "Unlink Checkpoint State");

  JMenuItem jMenuItemViewCheckpointStateAttach = new JMenuItem(
      "View Checkpoint State Link");

  JMenuItem jMenuItemRetrieveCheckpointState = new JMenuItem(
      "Retrieve Checkpoint State");

  protected JMenu jMenuListmatch = new JMenu("     Listmatch");

  JMenuItem jMenuItemJobCEIdFile = new JMenuItem("CE Id List from File");

  JMenuItem jMenuItemJobCEIdListmatch = new JMenuItem("CE Id List from IS");

  JMenuItem jMenuItemViewJobCEIdSelection = new JMenuItem(
      "View CE Id Selection");

  JMenuItem jMenuItemRemoveJobCEIdSelection = new JMenuItem(
      "Remove CE Id Selection");

  JLabel jLabelTotalDisplayedJobs = new JLabel();

  JPanel jPanelJobTable = new JPanel();

  JLabel jLabelTotalSelected = new JLabel();

  JLabel jLabelTotalSelectedJobs = new JLabel();

  JLabel jLabelTotalDisplayed = new JLabel();

  JScrollPane jScrollPaneJobTable = new JScrollPane();

  JLabel jLabelAddress = new JLabel();

  JLabel jLabelInformationServiceSchema = new JLabel();

  JTextField jLabelAddressValue = new JTextField();

  JTextField jLabelInformationServiceSchemaValue = new JTextField();

  JPanel jPanelNSInfo = new JPanel();

  JPanel jPanelLabel = new JPanel();

  JPanel jPanelAddress = new JPanel();

  JPanel jPanelSchema = new JPanel();

  JPanel jPanelMain = new JPanel();

  JTable jTableJobs;

  JobTableModel jobTableModel;

  JobSubmitter jobSubmitterJFrame;

  /**
   * Constructor.
   */
  public NSPanel(String name, JobSubmitter jobSubmitterJFrame) {
    this.name = name;
    this.jobSubmitterJFrame = jobSubmitterJFrame;
    try {
      _Init();
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
  }

  private void _Init() throws Exception {
    isDebugging = isDebugging
        || ((Logger.getRootLogger().getLevel() == Level.DEBUG) ? true : false);
    createJPopupMenu();
    jScrollPaneJobTable.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusGained(FocusEvent e) {
        jScrollPaneJobTableFocusGained(e);
      }
    });
    jLabelTotalDisplayed.setText("Total Displayed Jobs");
    jLabelTotalDisplayed.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelTotalDisplayedJobs.setText("0");
    jLabelTotalDisplayedJobs.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelTotalDisplayedJobs
        .setBorder(BorderFactory.createLoweredBevelBorder());
    jLabelTotalDisplayedJobs.setPreferredSize(new Dimension(50, 18));
    jLabelTotalSelected.setText("Total Selected Jobs");
    jLabelTotalSelected.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelTotalSelectedJobs.setText("0");
    jLabelTotalSelectedJobs.setPreferredSize(new Dimension(50, 18));
    jLabelTotalSelectedJobs.setBorder(BorderFactory.createLoweredBevelBorder());
    jLabelTotalSelectedJobs.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelAddress.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelAddress.setText("Address");
    jLabelAddressValue.setBorder(new EtchedBorder());
    jLabelAddressValue.setBackground(Color.white);
    jLabelAddressValue.setEditable(false);
    jLabelInformationServiceSchema.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelInformationServiceSchema.setText("Information Service Schema");
    jLabelInformationServiceSchemaValue.setBorder(new EtchedBorder());
    jLabelInformationServiceSchemaValue.setBackground(Color.white);
    jLabelInformationServiceSchemaValue.setEditable(false);
    jLabelInformationServiceSchemaValue
        .setPreferredSize(new Dimension(200, 20));
    jLabelInformationServiceSchemaValue.setMaximumSize(new Dimension(200, 20));
    jLabelAddressValue.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jLabelAddressValueFocusLost(e);
      }
    });
    jLabelInformationServiceSchemaValue
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            jLabelInformationServiceSchemaValueFocusLost(e);
          }
        });
    NetworkServer ns = (NetworkServer) GUIGlobalVars.nsMap.get(this.name);
    if (ns != null) {
      jLabelAddressValue.setText(ns.getAddress());
      jLabelInformationServiceSchemaValue.setText(ns.getJDLESchema());
    } else {
      jLabelAddressValue.setText("");
      jLabelInformationServiceSchemaValue.setText("");
    }
    vectorHeader.add(JOB_NAME_TABLE_HEADER);
    vectorHeader.add(JOB_ID_TABLE_HEADER);
    vectorHeader.add(JOB_SUBMIT_TIME_HEADER);
    vectorHeader.add(JOB_TYPE_HEADER);
    jobTableModel = new JobTableModel(vectorHeader, 0);
    jTableJobs = new JTable(jobTableModel);
    jTableJobs.getTableHeader().setReorderingAllowed(false);
    TableColumn col = jTableJobs.getColumnModel().getColumn(
        JOB_NAME_COLUMN_INDEX);
    col.setCellRenderer(new GUITableTooltipCellRenderer());
    col = jTableJobs.getColumnModel().getColumn(JOB_ID_COLUMN_INDEX);
    col.setCellRenderer(new GUITableTooltipCellRenderer());
    col = jTableJobs.getColumnModel().getColumn(JOB_SUBMIT_TIME_COLUMN_INDEX);
    col.setCellRenderer(new GUITableTooltipCellRenderer());
    col = jTableJobs.getColumnModel().getColumn(JOB_TYPE_COLUMN_INDEX);
    col.setCellRenderer(new GUITableTooltipCellRenderer());
    ListSelectionModel listSelectionModel = jTableJobs.getSelectionModel();
    listSelectionModel.addListSelectionListener(new ListSelectionListener() {
      public void valueChanged(ListSelectionEvent e) {
        if (e.getValueIsAdjusting()) {
          jLabelTotalSelectedJobs.setText(Integer.toString(jTableJobs
              .getSelectedRowCount()));
        }
      }
    });
    jPanelLabel.setLayout(new BoxLayout(jPanelLabel, BoxLayout.X_AXIS));
    jPanelLabel.setBorder(GraphicUtils.SPACING_BORDER);
    jPanelLabel.add(jLabelTotalDisplayed, null);
    jPanelLabel.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelLabel.add(jLabelTotalDisplayedJobs, null);
    jPanelLabel.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelLabel.add(Box.createGlue());
    jPanelLabel.add(jLabelTotalSelected, null);
    jPanelLabel.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelLabel.add(jLabelTotalSelectedJobs, null);
    jScrollPaneJobTable.getViewport().setBackground(Color.WHITE);
    jScrollPaneJobTable
        .setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
    jScrollPaneJobTable.setBorder(BorderFactory.createEtchedBorder());
    jScrollPaneJobTable.getViewport().add(jTableJobs, null);
    jPanelJobTable.setLayout(new BorderLayout());
    jPanelJobTable.setBorder(new TitledBorder(new EtchedBorder(),
        " Job Table ", 0, 0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelJobTable.add(jPanelLabel, BorderLayout.NORTH);
    jPanelJobTable.add(jScrollPaneJobTable, BorderLayout.CENTER);
    JPanel jPanelInnerNSInfo = new JPanel();
    jPanelInnerNSInfo.setLayout(new BoxLayout(jPanelInnerNSInfo,
        BoxLayout.X_AXIS));
    jPanelInnerNSInfo.setBorder(GraphicUtils.SPACING_BORDER);
    jPanelInnerNSInfo.add(jLabelAddress, null);
    jPanelInnerNSInfo.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelInnerNSInfo.add(jLabelAddressValue, null);
    jPanelInnerNSInfo.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelInnerNSInfo.add(jLabelInformationServiceSchema, null);
    jPanelInnerNSInfo.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelInnerNSInfo.add(jLabelInformationServiceSchemaValue, null);
    jPanelNSInfo.setLayout(new BorderLayout());
    jPanelNSInfo.setBorder(new TitledBorder(new EtchedBorder(),
        " Network Server Info ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelNSInfo.add(jPanelInnerNSInfo, BorderLayout.CENTER);
    jPanelMain.setLayout(new BorderLayout());
    jPanelMain.add(jPanelNSInfo, BorderLayout.NORTH);
    jPanelMain.add(jPanelJobTable, BorderLayout.CENTER);
    this.setLayout(new BorderLayout());
    this.setBorder(GraphicUtils.SPACING_BORDER);
    this.add(jPanelMain, BorderLayout.CENTER);
    jTableJobs.addMouseListener(new MouseAdapter() {
      public void mouseClicked(MouseEvent me) {
        if (me.getClickCount() == 2) {
          Point point = me.getPoint();
          int row = jTableJobs.rowAtPoint(point);
          jobSubmitterJFrame.jButtonEditorEvent(null, false);
        }
      }
    });
    jTableJobs.addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        showJPopupMenuTable(e);
      }

      public void mouseReleased(MouseEvent e) {
        showJPopupMenuTable(e);
      }
    });
    jScrollPaneJobTable.addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        if (jTableJobs.getRowCount() != 0) {
          jMenuSelectNone();
        }
        showJPopupMenuTable(e);
      }

      public void mouseReleased(MouseEvent e) {
        if (jTableJobs.getRowCount() != 0) {
          jMenuSelectNone();
        }
        showJPopupMenuTable(e);
      }
    });
    jTableJobs.setAutoCreateColumnsFromModel(false);
    JTableHeader tableHeader = jTableJobs.getTableHeader();
    tableHeader.setUpdateTableInRealTime(true);
    tableHeader.addMouseListener(new MouseAdapter() {
      public void mouseClicked(MouseEvent me) {
        TableColumnModel columnModel = jTableJobs.getColumnModel();
        int columnIndex = columnModel.getColumnIndexAtX(me.getX());
        int modelIndex = columnModel.getColumn(columnIndex).getModelIndex();
        //logger.debug("columnIndex: " + columnIndex + " modelIndex: "
        // + modelIndex);
        if (modelIndex < 0) {
          return;
        }
        ascending = (sortingColumn == modelIndex) ? !ascending : true;
        sortingColumn = modelIndex;
        int[] selectedRows = jTableJobs.getSelectedRows();
        Vector selectedJobIdVector = new Vector();
        JobTableModel jobTableModel = (JobTableModel) jTableJobs.getModel();
        for (int i = 0; i < selectedRows.length; i++) {
          selectedJobIdVector.add(jobTableModel.getValueAt(selectedRows[i],
              MultipleJobPanel.JOB_ID_COLUMN_INDEX).toString().trim());
        }
        jobTableModel.sortBy(jTableJobs, modelIndex, ascending);
        int index = 0;
        for (int i = 0; i < selectedJobIdVector.size(); i++) {
          index = jobTableModel.getIndexOfElementInColumn(selectedJobIdVector
              .get(i).toString(), MultipleJobPanel.JOB_ID_COLUMN_INDEX);
          if (index != -1) {
            jTableJobs.addRowSelectionInterval(index, index);
          }
        }
      }
    });
  }

  /**
   * Returns the name of the Network Server panel.
   * 
   * @return the name of the Network Server panel.
   */
  public String getName() {
    return this.name;
  }

  /**
   * Returns the progressive job number used to create job name inserted in the
   * Job Table.
   * 
   * @return the progressive job number
   */
  protected int getProgressiveJobNumber(String name) {
    name = name.toUpperCase().trim();
    Vector progressiveNumberVector = new Vector();
    String progressiveNumberTxt = "";
    String jobName = "";
    int progressiveNumber = 0;
    for (int i = 0; i < jobTableModel.getRowCount(); i++) {
      jobName = jobTableModel.getValueAt(i, JOB_NAME_COLUMN_INDEX).toString()
          .toUpperCase().trim();
      logger.debug("-------- Name:" + name);
      logger.debug("-------- Job Name:" + jobName);
      if (jobName.indexOf(name) == 0) {
        progressiveNumberTxt = jobName.substring(name.length());
        try {
          progressiveNumber = Integer.parseInt(progressiveNumberTxt, 10);
          progressiveNumberVector.add(Integer.toString(progressiveNumber));
        } catch (NumberFormatException nfe) {
          if (isDebugging) {
            nfe.printStackTrace();
          }
          // Value after
          // GUIFileSystem.DEFAULT_JDL_EDITOR_SAVE_FILE_NAME is not
          // an integer. Do nothing.
        }
      }
    }
    Iterator keyIterator = GUIGlobalVars.openedEditorHashMap.keySet()
        .iterator();
    String key;
    String nsName;
    int lastIndex = -1;
    while (keyIterator.hasNext()) {
      key = keyIterator.next().toString().trim();
      lastIndex = key.lastIndexOf("-");
      if (lastIndex != -1) {
        nsName = key.substring(0, lastIndex).trim();
        if (nsName.equals(this.name)) {
          jobName = key.substring(lastIndex + 1).toUpperCase().trim();
          logger.debug("-------- Name:" + name);
          logger.debug("-------- Job Name:" + jobName);
          if (jobName.indexOf(name) == 0) {
            try {
              progressiveNumber = Integer.parseInt(jobName.substring(name
                  .length()), 10);
              logger.debug("-------- Progressive number:" + progressiveNumber);
              progressiveNumberVector.add(Integer.toString(progressiveNumber));
            } catch (NumberFormatException nfe) {
              if (isDebugging) {
                nfe.printStackTrace();
                // Value after
                // Utils.DEFAULT_JDL_EDITOR_SAVE_FILE_NAME is
                // not
                // an integer.
                // Do nothing.
              }
            }
          }
        }
      }
    }
    int progressiveNumberVectorSize = progressiveNumberVector.size();
    for (int i = 1; i <= progressiveNumberVectorSize; i++) {
      if (!progressiveNumberVector.contains(Integer.toString(i))) {
        return i;
      }
    }
    return progressiveNumberVectorSize + 1;
  }

  protected String getAvailableJobName(String name) {
    logger.debug("-------- Available name: " + name);
    String key = this.name + " - " + name;
    logger.debug("-------- Key: " + key);
    if (jobTableModel.isElementPresentInColumn(name, JOB_NAME_COLUMN_INDEX)
        || GUIGlobalVars.openedEditorHashMap.containsKey(key)) {
      name = name + getProgressiveJobNumber(name);
    }
    return name;
  }

  /**
   * Returns the progressive job number used to create default job name inserted
   * in the Job Table.
   * 
   * @return the progressive job number
   */
  protected int getProgressiveJobNumber() {
    return getProgressiveJobNumber(GUIFileSystem.DEFAULT_JDL_EDITOR_SAVE_FILE_NAME);
  }

  /**
   * Returns the progressive job number used to create default job name inserted
   * in the Job Table during a copy/paste operation. (e.g. Copy (1) of Job2, '1'
   * is the progressive number).
   * 
   * @param inputJobName
   *          the job name from which compute the number
   * @return the progressive job number
   */
  protected int getProgressiveJobNumberSameJobName(String inputJobName) {
    int length = inputJobName.length();
    String jobName = "";
    Vector progressiveNumberVector = new Vector();
    String progressiveNumberTxt = "";
    int progressiveNumber = 0;
    int firstIndex = 0;
    int secondIndex = 0;
    for (int i = 0; i < jobTableModel.getRowCount(); i++) {
      jobName = jobTableModel.getValueAt(i, 0).toString().trim();
      secondIndex = jobName.indexOf(") of ");
      if (secondIndex != -1) {
        if (jobName.substring(secondIndex + 5).equals(inputJobName)) {
          firstIndex = jobName.indexOf("Copy (");
          if (firstIndex == 0) {
            progressiveNumberTxt = jobName.substring(6, secondIndex);
          }
        }
        try {
          progressiveNumber = Integer.parseInt(progressiveNumberTxt, 10);
          progressiveNumberVector.add(Integer.toString(progressiveNumber));
        } catch (NumberFormatException nfe) {
          // Do nothing.
        }
      }
    }
    int progressiveNumberVectorSize = progressiveNumberVector.size();
    for (int i = 1; i <= progressiveNumberVectorSize; i++) {
      if (!progressiveNumberVector.contains(Integer.toString(i))) {
        return i;
      }
    }
    return progressiveNumberVectorSize + 1;
  }

  void updateTotalDisplayedJobsLabel() {
    jLabelTotalDisplayedJobs
        .setText(Integer.toString(jTableJobs.getRowCount()));
  }

  /**
   * Returns the current number of jobs inserted in the Network Server panel Job
   * Table.
   * 
   * @return the number of the jobs in the table
   */
  protected int getTotalDisplayedJobs() {
    return jTableJobs.getRowCount();
  }

  void addJobNameVectorElement(String item) {
    this.jobNameVector.add(item);
  }

  void showJPopupMenuTable(MouseEvent e) {
    if (e.isPopupTrigger()) {
      Point point = e.getPoint();
      int row = jTableJobs.rowAtPoint(point);
      int column = jTableJobs.columnAtPoint(point);
      if ((row != -1) && !jTableJobs.isRowSelected(row)) {
        jTableJobs.setRowSelectionInterval(row, row);
      }
      if (GUIGlobalVars.selectedJobNameCopyVector.size() == 0) {
        jMenuItemPaste.setEnabled(false);
      } else {
        jMenuItemPaste.setEnabled(true);
      }
      if (jTableJobs.getRowCount() != 0) {
        jMenuItemSelectAll.setEnabled(true);
        jMenuItemSelectNone.setEnabled(true);
        jMenuItemInvertSelection.setEnabled(true);
        jMenuItemSelectSubmitted.setEnabled(true);
        jMenuItemClear.setEnabled(true);
        int selectedRowCount = jTableJobs.getSelectedRowCount();
        switch (selectedRowCount) {
          case 0:
            jMenuItemCut.setEnabled(false);
            jMenuItemCopy.setEnabled(false);
            jMenuCopyTo.setEnabled(false);
            jMenuMoveTo.setEnabled(false);
            jMenuItemRemove.setEnabled(false);
            jMenuItemClear.setEnabled(true);
            jMenuItemRename.setEnabled(false);
            jMenuItemSelectAll.setEnabled(true);
            jMenuItemSelectNone.setEnabled(false);
            jMenuItemInvertSelection.setEnabled(true);
            jMenuItemSelectSubmitted.setEnabled(true);
            jMenuItemSubmit.setEnabled(false);
            jMenuItemSendToMonitor.setEnabled(false);
            jMenuItemOpenInEditor.setEnabled(false);
            jMenuItemInteractiveConsole.setEnabled(false);
            jMenuItemAttachCheckpointState.setEnabled(false);
            jMenuItemDetachCheckpointState.setEnabled(false);
            jMenuItemViewCheckpointStateAttach.setEnabled(false);
            jMenuItemRetrieveCheckpointState.setEnabled(false);
            jMenuItemJobCEIdFile.setEnabled(false);
            jMenuItemJobCEIdListmatch.setEnabled(false);
            jMenuItemViewJobCEIdSelection.setEnabled(false);
            jMenuItemRemoveJobCEIdSelection.setEnabled(false);
          break;
          case 1:
            int selectedRow = jTableJobs.getSelectedRow();
            String jobIdText = jobTableModel.getValueAt(selectedRow,
                JOB_ID_COLUMN_INDEX).toString().trim();
            String jobType = jobTableModel.getValueAt(selectedRow,
                JOB_TYPE_COLUMN_INDEX).toString().trim();
            String keyJobName = jobTableModel.getValueAt(selectedRow,
                JOB_NAME_COLUMN_INDEX).toString();
            String temporaryPhysicalFileName = GUIFileSystem
                .getJobTemporaryFileDirectory()
                + this.name
                + File.separator
                + keyJobName
                + GUIFileSystem.JDL_FILE_EXTENSION;
            String selectedCEId = "";
            if (!jobType.equals(Jdl.TYPE_DAG)) {
              selectedCEId = getSelectedCEId(temporaryPhysicalFileName);
            }
            if (jobIdText.equals(Utils.NOT_SUBMITTED_TEXT)) {
              // The job is not submitted.
              jMenuItemSubmit.setEnabled(true);
              jMenuItemJobCEIdFile.setEnabled(true);
              jMenuItemJobCEIdListmatch.setEnabled(true);
              if (!selectedCEId.equals("")) {
                jMenuItemRemoveJobCEIdSelection.setEnabled(true);
              } else {
                jMenuItemRemoveJobCEIdSelection.setEnabled(false);
              }
              jMenuItemInteractiveConsole.setEnabled(false);
              if (!jobType.equals(Jdl.TYPE_DAG)) {
                jMenuItemOpenInEditor.setEnabled(true);
              } else { // Dag.
                jMenuItemOpenInEditor.setEnabled(false);
              }
              jMenuItemSendToMonitor.setEnabled(false);
            } else if (jobIdText.equals(Utils.SUBMITTING_TEXT)
                || jobIdText.equals(Utils.LISTMATCHING_TEXT)) {
              // Job submission is on-going.
              jMenuItemSubmit.setEnabled(false);
              jMenuItemJobCEIdFile.setEnabled(false);
              jMenuItemJobCEIdListmatch.setEnabled(false);
              jMenuItemRemoveJobCEIdSelection.setEnabled(false);
              jMenuItemInteractiveConsole.setEnabled(false);
              jMenuItemOpenInEditor.setEnabled(false);
              jMenuItemSendToMonitor.setEnabled(false);
            } else {
              // The job is submitted.
              jMenuItemSubmit.setEnabled(false);
              jMenuItemJobCEIdFile.setEnabled(false);
              jMenuItemJobCEIdListmatch.setEnabled(false);
              jMenuItemRemoveJobCEIdSelection.setEnabled(false);
              if (jobType.indexOf(Jdl.JOBTYPE_INTERACTIVE) != -1) {
                jMenuItemInteractiveConsole.setEnabled(true);
              } else {
                jMenuItemInteractiveConsole.setEnabled(false);
              }
              jMenuItemOpenInEditor.setEnabled(true);
              jMenuItemSendToMonitor.setEnabled(true);
            }
            if (!jobType.equals(Jdl.TYPE_DAG)) {
              if (!selectedCEId.equals("")) {
                jMenuItemViewJobCEIdSelection.setEnabled(true);
              } else {
                jMenuItemViewJobCEIdSelection.setEnabled(false);
              }
            } else { // Dag.
              jMenuItemJobCEIdFile.setEnabled(false);
              jMenuItemJobCEIdListmatch.setEnabled(false);
              jMenuItemViewJobCEIdSelection.setEnabled(false);
              jMenuItemRemoveJobCEIdSelection.setEnabled(false);
            }
            if (jobType.indexOf(Jdl.JOBTYPE_CHECKPOINTABLE) != -1) {
              // The job is checkpointable.
              if (jobIdText.equals(Utils.NOT_SUBMITTED_TEXT)) {
                if (!getAttachedJobState(temporaryPhysicalFileName).equals("")) {
                  jMenuItemDetachCheckpointState.setEnabled(true);
                  jMenuItemViewCheckpointStateAttach.setEnabled(true);
                } else {
                  jMenuItemDetachCheckpointState.setEnabled(false);
                  jMenuItemViewCheckpointStateAttach.setEnabled(false);
                }
                jMenuItemAttachCheckpointState.setEnabled(true);
                jMenuItemRetrieveCheckpointState.setEnabled(false);
              } else if (jobIdText.equals(Utils.SUBMITTING_TEXT)
                  || jobIdText.equals(Utils.LISTMATCHING_TEXT)) {
                if (!getAttachedJobState(temporaryPhysicalFileName).equals("")) {
                  jMenuItemViewCheckpointStateAttach.setEnabled(true);
                } else {
                  jMenuItemViewCheckpointStateAttach.setEnabled(false);
                }
                jMenuItemDetachCheckpointState.setEnabled(false);
                jMenuItemAttachCheckpointState.setEnabled(false);
                jMenuItemRetrieveCheckpointState.setEnabled(false);
              } else {
                jMenuItemAttachCheckpointState.setEnabled(false);
                jMenuItemDetachCheckpointState.setEnabled(false);
                jMenuItemRetrieveCheckpointState.setEnabled(true);
                if (!getAttachedJobState(temporaryPhysicalFileName).equals("")) {
                  jMenuItemViewCheckpointStateAttach.setEnabled(true);
                } else {
                  jMenuItemViewCheckpointStateAttach.setEnabled(false);
                }
              }
            } else {
              jMenuItemAttachCheckpointState.setEnabled(false);
              jMenuItemDetachCheckpointState.setEnabled(false);
              jMenuItemViewCheckpointStateAttach.setEnabled(false);
              jMenuItemRetrieveCheckpointState.setEnabled(false);
            }
            int nsNameVectorSize = jobSubmitterJFrame.getNSNameVector().size();
            if (nsNameVectorSize == 1) {
              jMenuMoveTo.setEnabled(false);
            } else if ((nsNameVectorSize > 1) && (selectedRowCount > 0)) {
              jMenuMoveTo.setEnabled(true);
            }
            jMenuItemCut.setEnabled(true);
            jMenuItemCopy.setEnabled(true);
            jMenuCopyTo.setEnabled(true);
            jMenuItemRemove.setEnabled(true);
            jMenuItemClear.setEnabled(true);
            jMenuItemRename.setEnabled(true);
            jMenuItemSelectAll.setEnabled(true);
            jMenuItemSelectNone.setEnabled(true);
            jMenuItemInvertSelection.setEnabled(true);
            jMenuItemSelectSubmitted.setEnabled(true);
          break;
          // END case 1
          default:
            jMenuItemCut.setEnabled(true);
            jMenuItemCopy.setEnabled(true);
            jMenuCopyTo.setEnabled(true);
            jMenuMoveTo.setEnabled(true);
            jMenuItemRemove.setEnabled(true);
            jMenuItemClear.setEnabled(true);
            jMenuItemRename.setEnabled(false);
            jMenuItemSelectAll.setEnabled(true);
            jMenuItemSelectNone.setEnabled(true);
            jMenuItemInvertSelection.setEnabled(true);
            jMenuItemSelectSubmitted.setEnabled(true);
            jMenuItemSubmit.setEnabled(true);
            jMenuItemSendToMonitor.setEnabled(true);
            jMenuItemOpenInEditor.setEnabled(false);
            jMenuItemInteractiveConsole.setEnabled(false);
            jMenuItemAttachCheckpointState.setEnabled(false);
            jMenuItemDetachCheckpointState.setEnabled(false);
            jMenuItemViewCheckpointStateAttach.setEnabled(false);
            jMenuItemRetrieveCheckpointState.setEnabled(false);
            jMenuItemJobCEIdFile.setEnabled(false);
            jMenuItemJobCEIdListmatch.setEnabled(false);
            jMenuItemViewJobCEIdSelection.setEnabled(false);
            jMenuItemRemoveJobCEIdSelection.setEnabled(false);
        }
      } else {
        jMenuItemCut.setEnabled(false);
        jMenuItemCopy.setEnabled(false);
        jMenuCopyTo.setEnabled(false);
        jMenuMoveTo.setEnabled(false);
        jMenuItemRemove.setEnabled(false);
        jMenuItemClear.setEnabled(false);
        jMenuItemRename.setEnabled(false);
        jMenuItemSelectAll.setEnabled(false);
        jMenuItemSelectNone.setEnabled(false);
        jMenuItemInvertSelection.setEnabled(false);
        jMenuItemSelectSubmitted.setEnabled(false);
        jMenuItemSubmit.setEnabled(false);
        jMenuItemSendToMonitor.setEnabled(false);
        jMenuItemOpenInEditor.setEnabled(false);
        jMenuItemInteractiveConsole.setEnabled(false);
        jMenuItemAttachCheckpointState.setEnabled(false);
        jMenuItemDetachCheckpointState.setEnabled(false);
        jMenuItemViewCheckpointStateAttach.setEnabled(false);
        jMenuItemRetrieveCheckpointState.setEnabled(false);
        jMenuItemJobCEIdFile.setEnabled(false);
        jMenuItemJobCEIdListmatch.setEnabled(false);
        jMenuItemViewJobCEIdSelection.setEnabled(false);
        jMenuItemRemoveJobCEIdSelection.setEnabled(false);
      }
      if (jMenuItemAttachCheckpointState.isEnabled()
          || jMenuItemDetachCheckpointState.isEnabled()
          || jMenuItemViewCheckpointStateAttach.isEnabled()
          || jMenuItemRetrieveCheckpointState.isEnabled()) {
        jMenuCheckpoint.setEnabled(true);
      } else {
        jMenuCheckpoint.setEnabled(false);
      }
      if (jMenuItemJobCEIdFile.isEnabled()
          || jMenuItemJobCEIdListmatch.isEnabled()
          || jMenuItemViewJobCEIdSelection.isEnabled()
          || jMenuItemRemoveJobCEIdSelection.isEnabled()) {
        jMenuListmatch.setEnabled(true);
      } else {
        jMenuListmatch.setEnabled(false);
      }
      jPopupMenuTable = new JPopupMenu();
      renewJPopupMenu();
      jPopupMenuTable.show(e.getComponent(), e.getX(), e.getY());
    }
  }

  void jScrollPaneJobTableFocusGained(FocusEvent e) {
    jTableJobs.clearSelection();
  }

  void renewJPopupMenu() {
    jPopupMenuTable.removeAll();
    jPopupMenuTable.add(jMenuItemCut);
    jPopupMenuTable.add(jMenuItemCopy);
    jPopupMenuTable.add(jMenuItemPaste);
    jPopupMenuTable.addSeparator();
    jPopupMenuTable.add(jMenuCopyTo);
    jPopupMenuTable.add(jMenuMoveTo);
    jPopupMenuTable.addSeparator();
    jPopupMenuTable.add(jMenuItemRemove);
    jPopupMenuTable.add(jMenuItemClear);
    jPopupMenuTable.add(jMenuItemRename);
    jPopupMenuTable.addSeparator();
    jPopupMenuTable.add(jMenuItemSelectAll);
    jPopupMenuTable.add(jMenuItemSelectNone);
    jPopupMenuTable.add(jMenuItemSelectSubmitted);
    jPopupMenuTable.add(jMenuItemInvertSelection);
    jPopupMenuTable.addSeparator();
    jPopupMenuTable.add(jMenuListmatch);
    jPopupMenuTable.addSeparator();
    jPopupMenuTable.add(jMenuItemSubmit);
    jPopupMenuTable.add(jMenuItemSendToMonitor);
    jPopupMenuTable.add(jMenuItemOpenInEditor);
    jPopupMenuTable.addSeparator();
    jPopupMenuTable.add(jMenuItemInteractiveConsole);
    jPopupMenuTable.addSeparator();
    jPopupMenuTable.add(jMenuCheckpoint);
  }

  void createJPopupMenu() {
    ActionListener alst = null;
    URL url = JobSubmitter.class.getResource(Utils.ICON_CUT);
    if (url != null) {
      jMenuItemCut.setIcon(new ImageIcon(url));
    }
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuCut();
      }
    };
    jMenuItemCut.addActionListener(alst);
    jMenuItemCut.setAccelerator(GraphicUtils.CUT_ACCELERATOR);
    url = JobSubmitter.class.getResource(Utils.ICON_COPY);
    if (url != null) {
      jMenuItemCopy.setIcon(new ImageIcon(url));
    }
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuCopy();
      }
    };
    jMenuItemCopy.addActionListener(alst);
    jMenuItemCopy.setAccelerator(GraphicUtils.COPY_ACCELERATOR);
    url = JobSubmitter.class.getResource(Utils.ICON_PASTE);
    if (url != null) {
      jMenuItemPaste.setIcon(new ImageIcon(url));
    }
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuPaste();
      }
    };
    jMenuItemPaste.addActionListener(alst);
    jMenuItemPaste.setAccelerator(GraphicUtils.PASTE_ACCELERATOR);
    jPopupMenuTable.add(jMenuItemCut);
    jPopupMenuTable.add(jMenuItemCopy);
    jPopupMenuTable.add(jMenuItemPaste);
    jPopupMenuTable.addSeparator();
    JMenuItem jMenuItem = new JMenuItem();
    Vector nsVector = jobSubmitterJFrame.getNSVector();
    int nsVectorSize = nsVector.size();
    jMenuCopyTo.removeAll();
    for (int i = 0; i < nsVectorSize; i++) {
      jMenuItem = new JMenuItem(((NetworkServer) nsVector.get(i)).getName());
      alst = new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          jMenuCopyTo(e);
        }
      };
      jMenuItem.addActionListener(alst);
      jMenuCopyTo.add(jMenuItem);
    }
    jPopupMenuTable.add(jMenuCopyTo);
    jMenuMoveTo.removeAll();
    for (int i = 0; i < nsVectorSize; i++) {
      jMenuItem = new JMenuItem(((NetworkServer) nsVector.get(i)).getName());
      alst = new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          jMenuMoveTo(e);
        }
      };
      jMenuItem.addActionListener(alst);
      jMenuMoveTo.add(jMenuItem);
    }
    jPopupMenuTable.add(jMenuMoveTo);
    jPopupMenuTable.addSeparator();
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuRemove();
      }
    };
    jMenuItemRemove.addActionListener(alst);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuClear();
      }
    };
    jMenuItemClear.addActionListener(alst);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuRename();
      }
    };
    jMenuItemRename.addActionListener(alst);
    jPopupMenuTable.add(jMenuItemRemove);
    jPopupMenuTable.add(jMenuItemClear);
    jPopupMenuTable.add(jMenuItemRename);
    jPopupMenuTable.addSeparator();
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuSelectAll();
      }
    };
    jMenuItemSelectAll.addActionListener(alst);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuSelectNone();
      }
    };
    jMenuItemSelectNone.addActionListener(alst);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuInvertSelection();
      }
    };
    jMenuItemInvertSelection.addActionListener(alst);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuSelectSubmitted();
      }
    };
    jMenuItemSelectSubmitted.addActionListener(alst);
    jPopupMenuTable.add(jMenuItemSelectAll);
    jPopupMenuTable.add(jMenuItemSelectNone);
    jPopupMenuTable.add(jMenuItemSelectSubmitted);
    jPopupMenuTable.add(jMenuItemInvertSelection);
    jPopupMenuTable.addSeparator();
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuSubmit();
      }
    };
    jMenuItemSubmit.addActionListener(alst);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuSendToMonitor();
      }
    };
    jMenuItemSendToMonitor.addActionListener(alst);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuOpenInEditor();
      }
    };
    jMenuItemOpenInEditor.addActionListener(alst);
    jPopupMenuTable.add(jMenuItemSubmit);
    jPopupMenuTable.add(jMenuItemSendToMonitor);
    jPopupMenuTable.add(jMenuItemOpenInEditor);
    jPopupMenuTable.addSeparator();
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuInteractiveConsole();
      }
    };
    jMenuItemInteractiveConsole.addActionListener(alst);
    jPopupMenuTable.add(jMenuItemInteractiveConsole);
    jPopupMenuTable.addSeparator();
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuAttachCheckpointState();
      }
    };
    jMenuItemAttachCheckpointState.addActionListener(alst);
    jMenuCheckpoint.add(jMenuItemAttachCheckpointState);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuDetachCheckpointState();
      }
    };
    jMenuItemDetachCheckpointState.addActionListener(alst);
    jMenuCheckpoint.add(jMenuItemDetachCheckpointState);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuViewCheckpointStateAttach();
      }
    };
    jMenuItemViewCheckpointStateAttach.addActionListener(alst);
    jMenuCheckpoint.add(jMenuItemViewCheckpointStateAttach);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuRetrieveCheckpointState();
      }
    };
    jMenuItemRetrieveCheckpointState.addActionListener(alst);
    jMenuCheckpoint.add(jMenuItemRetrieveCheckpointState);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuListmatch();
      }
    };
    jMenuItemJobCEIdListmatch.addActionListener(alst);
    jMenuListmatch.add(jMenuItemJobCEIdListmatch);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuListmatchFile();
      }
    };
    jMenuItemJobCEIdFile.addActionListener(alst);
    jMenuListmatch.add(jMenuItemJobCEIdFile);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuViewJobCEIdSelection();
      }
    };
    jMenuItemViewJobCEIdSelection.addActionListener(alst);
    jMenuListmatch.add(jMenuItemViewJobCEIdSelection);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuRemoveJobCEIdSelection();
      }
    };
    jMenuItemRemoveJobCEIdSelection.addActionListener(alst);
    jMenuListmatch.add(jMenuItemRemoveJobCEIdSelection);
    jPopupMenuTable.add(jMenuListmatch);
  }

  void jMenuListmatchFile() {
    final SwingWorker worker = new SwingWorker() {
      public Object construct() {
        // Standard code
        int[] selectedRows = jTableJobs.getSelectedRows();
        if (selectedRows.length == 1) {
          String keyJobName = jTableJobs.getValueAt(selectedRows[0],
              JOB_NAME_COLUMN_INDEX).toString().trim();
          String nsAddress = jLabelAddressValue.getText().trim();
          String temporaryPhysicalFileName = GUIFileSystem
              .getJobTemporaryFileDirectory()
              + NSPanel.this.name
              + File.separator
              + keyJobName
              + GUIFileSystem.JDL_FILE_EXTENSION;
          JFileChooser fileChooser = new JFileChooser();
          fileChooser.setDialogTitle("Open CE Ids List File");
          fileChooser.setCurrentDirectory(new File(GUIGlobalVars
              .getFileChooserWorkingDirectory()));
          String[] extensions = { "LST"
          };
          GUIFileFilter classadFileFilter = new GUIFileFilter("*.lst",
              extensions);
          fileChooser.addChoosableFileFilter(classadFileFilter);
          int choice = fileChooser.showOpenDialog(NSPanel.this);
          if (choice != JFileChooser.APPROVE_OPTION) {
            return "";
          } else {
            File inputFile = fileChooser.getSelectedFile();
            String selectedFile = inputFile.toString();
            if (inputFile.isFile()) {
              try {
                Vector ceIdVector = Utils.readTextFileLines(selectedFile);
                if (ceIdVector.size() != 0) {
                  ListmatchFrame listmatch;
                  if (!GUIGlobalVars.openedListmatchMap
                      .containsKey(NSPanel.this.name + " - " + keyJobName)) {
                    listmatch = new ListmatchFrame(NSPanel.this,
                        NSPanel.this.name, nsAddress, keyJobName,
                        "CE Id List From File - ");
                    GUIGlobalVars.openedListmatchMap.put(NSPanel.this.name
                        + " - " + keyJobName, listmatch);
                    GraphicUtils.windowCenterWindow(jobSubmitterJFrame,
                        listmatch);
                  } else {
                    listmatch = (ListmatchFrame) GUIGlobalVars.openedListmatchMap
                        .get(NSPanel.this.name + " - " + keyJobName);
                    String title = listmatch.getTitle();
                    listmatch.setTitle("CE Id List From File "
                        + title.substring(title.indexOf("-")));
                    listmatch.setVisible(false);
                    GraphicUtils.deiconifyFrame(listmatch);
                  }
                  listmatch.setCEIdTable(ceIdVector);
                  listmatch.setJButtonSaveVisible(false);
                  listmatch.setVisible(true);
                } else {
                  JOptionPane.showOptionDialog(NSPanel.this,
                      "Unable to find CE Id(s) in file: " + selectedFile,
                      Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                      JOptionPane.ERROR_MESSAGE, null, null, null);
                }
              } catch (Exception e) {
                if (isDebugging) {
                  e.printStackTrace();
                }
                JOptionPane.showOptionDialog(NSPanel.this, e.getMessage(),
                    Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                    JOptionPane.ERROR_MESSAGE, null, null, null);
              }
            } else {
              JOptionPane.showOptionDialog(NSPanel.this,
                  "Unable to find file: " + selectedFile, Utils.ERROR_MSG_TXT,
                  JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
                  null, null);
            }
          }
        }
        // END Standard code
        return "";
      }
    };
    worker.start();
  }

  void jMenuListmatch() {
    final SwingWorker worker = new SwingWorker() {
      public Object construct() {
        // Standard code
        int[] selectedRows = jTableJobs.getSelectedRows();
        String jobIdText = jTableJobs.getValueAt(selectedRows[0],
            JOB_ID_COLUMN_INDEX).toString().trim();
        if (jobIdText.equals(Utils.NOT_SUBMITTED_TEXT)) {
          jobTableModel.setValueAt(Utils.LISTMATCHING_TEXT, selectedRows[0],
              JOB_ID_COLUMN_INDEX);
          String keyJobName = jTableJobs.getValueAt(selectedRows[0],
              JOB_NAME_COLUMN_INDEX).toString();
          String rbAddress = jLabelAddressValue.getText().trim();
          String temporaryPhysicalFileName = GUIFileSystem
              .getJobTemporaryFileDirectory()
              + NSPanel.this.name
              + File.separator
              + keyJobName
              + GUIFileSystem.JDL_FILE_EXTENSION;
          ListmatchFrame listmatch;
          if (!GUIGlobalVars.openedListmatchMap.containsKey(NSPanel.this.name
              + " - " + keyJobName)) {
            listmatch = new ListmatchFrame(NSPanel.this, NSPanel.this.name,
                rbAddress, keyJobName, "CE Id List From IS - ");
            GUIGlobalVars.openedListmatchMap.put(NSPanel.this.name + " - "
                + keyJobName, listmatch);
            if (listmatch.setCEIdTable(temporaryPhysicalFileName, keyJobName) == -1) {
              GUIGlobalVars.openedListmatchMap.remove(NSPanel.this.name + " - "
                  + keyJobName);
              listmatch.dispose();
              int index = jobTableModel.getIndexOfElementInColumn(keyJobName,
                  JOB_NAME_COLUMN_INDEX);
              if (index != -1) {
                jobTableModel.setValueAt(Utils.NOT_SUBMITTED_TEXT, index,
                    JOB_ID_COLUMN_INDEX);
              }
              return "";
            }
            logger.debug("keyJobName:" + keyJobName);
            ListmatchFrame listmatchInMap = (ListmatchFrame) GUIGlobalVars.openedListmatchMap
                .get(NSPanel.this.name + " - " + keyJobName);
            logger.debug("listmatchInMap:" + listmatchInMap);
            logger.debug("listmatch:" + listmatch);
            if ((listmatchInMap != null) && (listmatchInMap == listmatch)) {
              GraphicUtils.windowCenterWindow(jobSubmitterJFrame, listmatch);
              listmatch.setVisible(true);
            } else {
              GUIGlobalVars.openedListmatchMap.remove(NSPanel.this.name + " - "
                  + keyJobName);
              listmatch.dispose();
            }
          } else {
            listmatch = (ListmatchFrame) GUIGlobalVars.openedListmatchMap
                .get(NSPanel.this.name + " - " + keyJobName);
            String title = listmatch.getTitle();
            listmatch.setTitle("CE Id List From IS "
                + title.substring(title.indexOf("-")));
            if (listmatch.setCEIdTable(temporaryPhysicalFileName, keyJobName) == -1) {
              GUIGlobalVars.openedListmatchMap.remove(NSPanel.this.name + " - "
                  + keyJobName);
              listmatch.dispose();
              int index = jobTableModel.getIndexOfElementInColumn(keyJobName,
                  JOB_NAME_COLUMN_INDEX);
              if (index != -1) {
                jobTableModel.setValueAt(Utils.NOT_SUBMITTED_TEXT, index,
                    JOB_ID_COLUMN_INDEX);
              }
              return "";
            }
            logger.debug("keyJobName:" + keyJobName);
            ListmatchFrame listmatchInMap = (ListmatchFrame) GUIGlobalVars.openedListmatchMap
                .get(NSPanel.this.name + " - " + keyJobName);
            logger.debug("listmatchInMap:" + listmatchInMap);
            logger.debug("listmatch:" + listmatch);
            if ((listmatchInMap != null) && (listmatchInMap == listmatch)) {
              listmatch.setVisible(false);
              GraphicUtils.deiconifyFrame(listmatch);
              listmatch.setVisible(true);
            } else {
              GUIGlobalVars.openedListmatchMap.remove(NSPanel.this.name + " - "
                  + keyJobName);
              listmatch.dispose();
            }
          }
          int index = jobTableModel.getIndexOfElementInColumn(keyJobName,
              JOB_NAME_COLUMN_INDEX);
          if (index != -1) {
            jobTableModel.setValueAt(Utils.NOT_SUBMITTED_TEXT, index,
                JOB_ID_COLUMN_INDEX);
          }
        }
        // END Standard code
        return "";
      }
    };
    worker.start();
  }

  void jMenuRetrieveCheckpointState() {
    int[] selectedRows = jTableJobs.getSelectedRows();
    jobSubmitterJFrame.jMenuRetrieveCheckpointState(jobTableModel.getValueAt(
        selectedRows[0], JOB_ID_COLUMN_INDEX).toString(), true);
  }

  void jMenuCut() {
    int[] selectedRows = jTableJobs.getSelectedRows();
    int selectedRowsCount = selectedRows.length;
    if (selectedRowsCount != 0) {
      String temporaryFileDirectory = GUIFileSystem
          .getJobTemporaryFileDirectory();
      File sourceFile = null;
      File targetFile = null;
      String keyJobName = "";
      String informationMsg = "";
      boolean outcome = false;
      GUIGlobalVars.selectedJobNameCopyVector.removeAllElements();
      try {
        GUIFileSystem.removeDirectoryDescendant(new File(GUIFileSystem
            .getTemporaryCopyFileDirectory()));
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
      GUIGlobalVars.selectedRBPanelCopy = this.name;
      for (int i = selectedRowsCount - 1; i >= 0; i--) {
        keyJobName = jobTableModel.getValueAt(selectedRows[i],
            JOB_NAME_COLUMN_INDEX).toString().trim();
        if (GUIGlobalVars.openedEditorHashMap.containsKey(this.name + " - "
            + keyJobName)) {
          int choice = JOptionPane.showOptionDialog(NSPanel.this,
              "A JDL Editor is opened for the '" + keyJobName
                  + "' job\nClose Editor and cut job?", Utils.WARNING_MSG_TXT,
              JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null,
              null, null);
          if (choice != 0) {
            continue;
          } else {
            JDLEditor editor = (JDLEditor) GUIGlobalVars.openedEditorHashMap
                .get(this.name + " - " + keyJobName);
            editor.dispose();
            GUIGlobalVars.openedEditorHashMap.remove(this.name + " - "
                + keyJobName);
          }
        }
        String jobIdText = jobTableModel.getValueAt(selectedRows[i],
            JOB_ID_COLUMN_INDEX).toString().trim();
        if (jobIdText.equals(Utils.NOT_SUBMITTED_TEXT)) {
          ListmatchFrame listmatch = (ListmatchFrame) GUIGlobalVars.openedListmatchMap
              .get(this.name + " - " + keyJobName);
          if (listmatch != null) {
            listmatch.dispose();
            GUIGlobalVars.openedListmatchMap.remove(this.name + " - "
                + keyJobName);
          }
          GUIGlobalVars.selectedJobNameCopyVector.add(jobTableModel.getValueAt(
              selectedRows[i], JOB_NAME_COLUMN_INDEX));
          targetFile = new File(GUIFileSystem.getTemporaryCopyFileDirectory()
              + File.separator + keyJobName + GUIFileSystem.JDL_FILE_EXTENSION);
          sourceFile = new File(temporaryFileDirectory + this.name
              + File.separator + keyJobName + GUIFileSystem.JDL_FILE_EXTENSION);
          try {
            GUIFileSystem.copyFile(sourceFile, targetFile);
            outcome = sourceFile.delete();
          } catch (Exception e) {
            if (isDebugging) {
              e.printStackTrace();
            }
            return;
          }
          if (outcome) {
            jobTableModel.removeRow(selectedRows[i]);
          } else {
            JOptionPane.showOptionDialog(NSPanel.this, "Unable to delete '"
                + keyJobName + "' job", Utils.ERROR_MSG_TXT,
                JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
                null, null);
          }
        } else if (jobIdText.equals(Utils.LISTMATCHING_TEXT)) {
          informationMsg += "- The job '" + keyJobName
              + "' is in Searching CE phase\n";
        } else if (jobIdText.equals(Utils.SUBMITTING_TEXT)) {
          informationMsg += "- The job '" + keyJobName
              + "' is in Submitting phase\n";
        } else {
          informationMsg += "- The job '" + keyJobName + "' is submitted\n";
        }
      }
      informationMsg = informationMsg.trim();
      if (!informationMsg.equals("")) {
        GraphicUtils.showOptionDialogMsg(NSPanel.this, informationMsg,
            Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
            "Unable to cut the following job(s):", null);
      }
    }
  }

  void jMenuCopy() {
    int[] selectedRows = jTableJobs.getSelectedRows();
    int selectedRowsCount = selectedRows.length;
    if (selectedRowsCount != 0) {
      String temporaryFileDirectory = GUIFileSystem
          .getJobTemporaryFileDirectory();
      File sourceFile = null;
      File targetFile = null;
      String keyJobName = "";
      GUIGlobalVars.selectedJobNameCopyVector.removeAllElements();
      try {
        GUIFileSystem.removeDirectoryDescendant(new File(GUIFileSystem
            .getTemporaryCopyFileDirectory()));
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
      GUIGlobalVars.selectedRBPanelCopy = this.name;
      for (int i = selectedRowsCount - 1; i >= 0; i--) {
        keyJobName = jobTableModel.getValueAt(selectedRows[i],
            JOB_NAME_COLUMN_INDEX).toString();
        GUIGlobalVars.selectedJobNameCopyVector.add(jobTableModel.getValueAt(
            selectedRows[i], JOB_NAME_COLUMN_INDEX));
        targetFile = new File(GUIFileSystem.getTemporaryCopyFileDirectory()
            + File.separator + keyJobName + GUIFileSystem.JDL_FILE_EXTENSION);
        sourceFile = new File(temporaryFileDirectory + this.name
            + File.separator + keyJobName + GUIFileSystem.JDL_FILE_EXTENSION);
        try {
          GUIFileSystem.copyFile(sourceFile, targetFile);
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
        }
      }
    }
  }

  void jMenuMoveTo(ActionEvent e) {
    int[] selectedRows = jTableJobs.getSelectedRows();
    int selectedRowsCount = selectedRows.length;
    String targetNSPanelName = e.getActionCommand();
    JTabbedPane jTabbedPane = jobSubmitterJFrame.jTabbedPaneRB;
    NSPanel targetNSPanel = (NSPanel) jTabbedPane.getComponentAt(jTabbedPane
        .indexOfTab(targetNSPanelName));
    String rBName = targetNSPanel.name;
    String keyJobName = "";
    String jobType = "";
    String oldName = "";
    String temporaryFileDirectory = GUIFileSystem
        .getJobTemporaryFileDirectory();
    File targetFile = null;
    File sourceFile = null;
    Object[] options = { "Yes", "No", "All", "Cancel"
    };
    int choice = -1;
    boolean isUpdateAllFiles = false;
    String fileName = "";
    if (selectedRowsCount != 0) {
      GUIGlobalVars.selectedJobNameCopyVector.removeAllElements();
      try {
        GUIFileSystem.removeDirectoryDescendant(new File(GUIFileSystem
            .getTemporaryCopyFileDirectory()));
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
      GUIGlobalVars.selectedRBPanelCopy = this.name;
      for (int i = selectedRowsCount - 1; i >= 0; i--) {
        keyJobName = jobTableModel.getValueAt(selectedRows[i],
            JOB_NAME_COLUMN_INDEX).toString();
        GUIGlobalVars.selectedJobNameCopyVector.add(jobTableModel.getValueAt(
            selectedRows[i], JOB_NAME_COLUMN_INDEX));
        targetFile = new File(GUIFileSystem.getTemporaryCopyFileDirectory()
            + File.separator + keyJobName + GUIFileSystem.JDL_FILE_EXTENSION);
        sourceFile = new File(temporaryFileDirectory + this.name
            + File.separator + keyJobName + GUIFileSystem.JDL_FILE_EXTENSION);
        try {
          GUIFileSystem.copyFile(sourceFile, targetFile);
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
        }
      }
    }
    String fileNameToDelete = "";
    File fileToDelete = null;
    boolean result = false;
    String submittedJobErrorMsg = "";
    String jobIdText = "";
    String targetJobIdText = "";
    int row = -1;
    for (int i = selectedRowsCount - 1; i >= 0; i--) {
      keyJobName = jobTableModel.getValueAt(selectedRows[i],
          JOB_NAME_COLUMN_INDEX).toString();
      if (GUIGlobalVars.openedEditorHashMap.containsKey(this.name + " - "
          + keyJobName)) {
        choice = JOptionPane.showOptionDialog(NSPanel.this,
            "A JDL Editor is opened for the '" + keyJobName
                + "' job\nClose Editor and move job?", Utils.WARNING_MSG_TXT,
            JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null, null,
            null);
        if (choice != 0) {
          continue;
        } else {
          JDLEditor editor = (JDLEditor) GUIGlobalVars.openedEditorHashMap
              .get(this.name + " - " + keyJobName);
          editor.dispose();
          GUIGlobalVars.openedEditorHashMap.remove(this.name + " - "
              + keyJobName);
        }
      }
      jobIdText = jobTableModel
          .getValueAt(selectedRows[i], JOB_ID_COLUMN_INDEX).toString().trim();
      if (jobIdText.equals(Utils.NOT_SUBMITTED_TEXT)) {
        ListmatchFrame listmatch = (ListmatchFrame) GUIGlobalVars.openedListmatchMap
            .get(this.name + " - " + keyJobName);
        if (listmatch != null) {
          listmatch.dispose();
          GUIGlobalVars.openedListmatchMap.remove(this.name + " - "
              + keyJobName);
        }
        row = targetNSPanel.jobTableModel.getIndexOfElementInColumnCi(
            keyJobName, JOB_NAME_COLUMN_INDEX);
        if (row != -1) { // The element is present in column.
          targetJobIdText = targetNSPanel.jobTableModel.getValueAt(row,
              JOB_ID_COLUMN_INDEX).toString().trim();
          if (targetJobIdText.equals(Utils.NOT_SUBMITTED_TEXT)) {
            if (!isUpdateAllFiles) {
              choice = JOptionPane
                  .showOptionDialog(
                      NSPanel.this,
                      "The job '"
                          + keyJobName
                          + "' is already present\nDo you want to replace the old one?",
                      Utils.WARNING_MSG_TXT,
                      JOptionPane.YES_NO_OPTION,
                      JOptionPane.WARNING_MESSAGE,
                      null,
                      (GUIGlobalVars.selectedJobNameCopyVector.size() == 1) ? null
                          : options, null);
            }
            switch (choice) {
              case 3: // Cancel.
                return;
              case 2: // All.
                isUpdateAllFiles = true;
                fileName = GUIFileSystem.getTemporaryCopyFileDirectory()
                    + File.separator + keyJobName
                    + GUIFileSystem.JDL_FILE_EXTENSION;
                updateJobInTable(rBName, keyJobName, new File(fileName));
                fileNameToDelete = temporaryFileDirectory + this.name
                    + File.separator + keyJobName
                    + GUIFileSystem.JDL_FILE_EXTENSION;
                fileToDelete = new File(fileNameToDelete);
                try {
                  result = fileToDelete.delete();
                } catch (SecurityException se) {
                  if (isDebugging) {
                    se.printStackTrace();
                  }
                }
                if (result == false) {
                  logger.error("Cannot remove file: " + fileToDelete);
                }
                jobTableModel.removeRow(selectedRows[i]);
              break;
              case 1: // No.
                continue;
              case 0: // Yes.
                fileName = GUIFileSystem.getTemporaryCopyFileDirectory()
                    + File.separator + keyJobName
                    + GUIFileSystem.JDL_FILE_EXTENSION;
                updateJobInTable(rBName, keyJobName, new File(fileName));
                fileNameToDelete = temporaryFileDirectory + this.name
                    + File.separator + keyJobName
                    + GUIFileSystem.JDL_FILE_EXTENSION;
                fileToDelete = new File(fileNameToDelete);
                try {
                  result = fileToDelete.delete();
                } catch (SecurityException se) {
                  if (isDebugging) {
                    se.printStackTrace();
                  }
                }
                if (result == false) {
                  logger.error("Cannot remove file: " + fileToDelete);
                }
                jobTableModel.removeRow(selectedRows[i]);
              break;
            }
          } else if (targetJobIdText.equals(Utils.LISTMATCHING_TEXT)) {
            submittedJobErrorMsg += "- '" + keyJobName
                + "' (target job is in Searching CE phase)\n";
          } else if (targetJobIdText.equals(Utils.SUBMITTING_TEXT)) {
            submittedJobErrorMsg += "- '" + keyJobName
                + "' (target job is in Submitting phase)\n";
          } else {
            submittedJobErrorMsg += "- '" + keyJobName
                + "' (target job is submitted)\n";
          }
        } else {
          fileName = GUIFileSystem.getTemporaryCopyFileDirectory()
              + File.separator + keyJobName + GUIFileSystem.JDL_FILE_EXTENSION;
          addJobToTable(targetNSPanel, keyJobName, new File(fileName));
          fileNameToDelete = temporaryFileDirectory + this.name
              + File.separator + keyJobName + GUIFileSystem.JDL_FILE_EXTENSION;
          fileToDelete = new File(fileNameToDelete);
          try {
            result = fileToDelete.delete();
          } catch (SecurityException se) {
            if (isDebugging) {
              se.printStackTrace();
            }
          }
          if (result == false) {
            logger.error("Cannot remove file: " + fileToDelete);
          }
          jobTableModel.removeRow(selectedRows[i]);
        }
        updateTotalDisplayedJobsLabel();
      } else if (jobIdText.equals(Utils.LISTMATCHING_TEXT)) {
        submittedJobErrorMsg += "- '" + keyJobName
            + "' (the job is in Searching CE phase)\n";
      } else if (jobIdText.equals(Utils.SUBMITTING_TEXT)) {
        submittedJobErrorMsg += "- '" + keyJobName
            + "' (the job is in Submitting phase)\n";
      } else {
        submittedJobErrorMsg += "- '" + keyJobName
            + "' (the job is submitted)\n";
      }
    }
    isUpdateAllFiles = false;
    if (!submittedJobErrorMsg.trim().equals("")) {
      JOptionPane.showOptionDialog(NSPanel.this,
          "Unable to move the following Job(s):\n" + submittedJobErrorMsg,
          Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.WARNING_MESSAGE, null, null, null);
    }
  }

  void jMenuCopyTo(ActionEvent e) {
    jMenuCopy();
    String targetNSPanelName = e.getActionCommand();
    JTabbedPane jTabbedPane = jobSubmitterJFrame.jTabbedPaneRB;
    NSPanel targetNSPanel = (NSPanel) jTabbedPane.getComponentAt(jTabbedPane
        .indexOfTab(targetNSPanelName));
    jMenuPaste(targetNSPanel);
  }

  void jMenuPaste(NSPanel targetNSPanel) {
    String rBName = targetNSPanel.name;
    String keyJobName = "";
    String jobType = "";
    String oldName = "";
    String temporaryFileDirectory = GUIFileSystem
        .getJobTemporaryFileDirectory();
    Object[] options = { "Yes", "No", "All", "Cancel"
    };
    int choice = -1;
    boolean isUpdateAllFiles = false;
    String fileName = "";
    if (!GUIGlobalVars.selectedRBPanelCopy.equals(rBName)) {
      String submittedJobErrorMsg = "";
      String targetJobIdText = "";
      int row = -1;
      for (int i = GUIGlobalVars.selectedJobNameCopyVector.size() - 1; i >= 0; i--) {
        keyJobName = GUIGlobalVars.selectedJobNameCopyVector.get(i).toString();
        row = targetNSPanel.jobTableModel.getIndexOfElementInColumnCi(
            keyJobName, JOB_NAME_COLUMN_INDEX);
        if (row != -1) { // The element is present in column.
          if (!isUpdateAllFiles) {
            choice = JOptionPane.showOptionDialog(NSPanel.this, "The job '"
                + keyJobName + "' is already present"
                + "\nDo you want to replace the old one?",
                Utils.WARNING_MSG_TXT, JOptionPane.YES_NO_OPTION,
                JOptionPane.WARNING_MESSAGE, null,
                (GUIGlobalVars.selectedJobNameCopyVector.size() == 1) ? null
                    : options, null);
          }
          switch (choice) {
            case 3: // Cancel.
              return;
            case 2: // All.
              isUpdateAllFiles = true;
              fileName = GUIFileSystem.getTemporaryCopyFileDirectory()
                  + File.separator + keyJobName
                  + GUIFileSystem.JDL_FILE_EXTENSION;
              targetJobIdText = targetNSPanel.jobTableModel.getValueAt(row,
                  JOB_ID_COLUMN_INDEX).toString().trim();
              if (targetJobIdText.equals(Utils.NOT_SUBMITTED_TEXT)) {
                updateJobInTable(rBName, keyJobName, new File(fileName));
              } else if (targetJobIdText.equals(Utils.LISTMATCHING_TEXT)) {
                submittedJobErrorMsg += "- '" + keyJobName
                    + "' (target job is in Searching CE phase)\n";
              } else if (targetJobIdText.equals(Utils.SUBMITTING_TEXT)) {
                submittedJobErrorMsg += "- '" + keyJobName
                    + "' (target job is in Submitting phase)\n";
              } else {
                submittedJobErrorMsg += "- '" + keyJobName
                    + "' (target job is submitted)\n";
              }
            break;
            case 1: // No.
              continue;
            case 0: // Yes.
              fileName = GUIFileSystem.getTemporaryCopyFileDirectory()
                  + File.separator + keyJobName
                  + GUIFileSystem.JDL_FILE_EXTENSION;
              targetJobIdText = targetNSPanel.jobTableModel.getValueAt(row,
                  JOB_ID_COLUMN_INDEX).toString().trim();
              if (targetJobIdText.equals(Utils.NOT_SUBMITTED_TEXT)) {
                updateJobInTable(rBName, keyJobName, new File(fileName));
              } else if (targetJobIdText.equals(Utils.LISTMATCHING_TEXT)) {
                submittedJobErrorMsg += "- '" + keyJobName
                    + "' (target job is in Searching CE phase)\n";
              } else if (targetJobIdText.equals(Utils.SUBMITTING_TEXT)) {
                submittedJobErrorMsg += "- '" + keyJobName
                    + "' (target job is in Submitting phase)\n";
              } else {
                submittedJobErrorMsg += "- '" + keyJobName
                    + "' (target job is submitted)\n";
              }
            break;
          }
        } else {
          fileName = GUIFileSystem.getTemporaryCopyFileDirectory()
              + File.separator + keyJobName + GUIFileSystem.JDL_FILE_EXTENSION;
          addJobToTable(targetNSPanel, keyJobName, new File(fileName));
        }
      }
      isUpdateAllFiles = false;
      submittedJobErrorMsg = submittedJobErrorMsg.trim();
      if (!submittedJobErrorMsg.equals("")) {
        GraphicUtils.showOptionDialogMsg(NSPanel.this, submittedJobErrorMsg,
            Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
            "Unable to replace the following target Job(s):", null);
      }
    } else {
      for (int i = GUIGlobalVars.selectedJobNameCopyVector.size() - 1; i >= 0; i--) {
        keyJobName = GUIGlobalVars.selectedJobNameCopyVector.get(i).toString();
        oldName = keyJobName;
        if (jobTableModel.isElementPresentInColumnCi(keyJobName,
            JOB_NAME_COLUMN_INDEX)) {
          if (!jobTableModel.isElementPresentInColumnCi(
              "Copy of " + keyJobName, JOB_NAME_COLUMN_INDEX)) {
            keyJobName = "Copy of " + keyJobName;
          } else {
            keyJobName = "Copy ("
                + getProgressiveJobNumberSameJobName(keyJobName) + ") of "
                + keyJobName;
          }
          fileName = GUIFileSystem.getTemporaryCopyFileDirectory()
              + File.separator + oldName + GUIFileSystem.JDL_FILE_EXTENSION;
          addJobToTable(rBName, keyJobName, new File(fileName));
        } else {
          fileName = GUIFileSystem.getTemporaryCopyFileDirectory()
              + File.separator + keyJobName + GUIFileSystem.JDL_FILE_EXTENSION;
          addJobToTable(rBName, keyJobName, new File(fileName));
        }
      }
    }
  }

  void jMenuPaste() {
    JTabbedPane jTabbedPaneRB = jobSubmitterJFrame.jTabbedPaneRB;
    NSPanel selectedRB = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    jMenuPaste(selectedRB);
  }

  /**
   * Restores, into the Network Server panel Job Table, the job represented by
   * the jdl file <code>inputFile</code> with the name specified by
   * <code>keyJobName</code>. Restore means that before adding the job, the
   * method checks if the job has been already submitted. In this case the job
   * will be inserted as a submitted job with JobId and Submission Time read
   * from the jdl.
   * 
   * @param keyJobName
   *          the name of the job to restore
   * @param inputFile
   *          the jdl representation of the job to restore
   * @return true if the job has been successfully restored, false otherwise
   */
  protected boolean restoreJobToTable(String keyJobName, File inputFile) {
    Ad ad = new Ad();
    try {
      ad.fromFile(inputFile.toString());
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      return false;
    }
    String type = "";
    try {
      if (ad.hasAttribute(Jdl.TYPE)) {
        type = ad.getStringValue(Jdl.TYPE).get(0).toString();
      }
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    if (!type.equals(Jdl.TYPE_DAG)) {
      JobAd jobAd = new JobAd();
      Vector jobTypeVector = new Vector();
      String jobType = "";
      try {
        jobAd.fromFile(inputFile.toString());
        jobAd.checkAll();
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        return false;
      }
      try {
        if (jobAd.hasAttribute(Jdl.JOBTYPE)) {
          jobTypeVector = jobAd.getStringValue(Jdl.JOBTYPE);
        } else {
          jobType = Jdl.JOBTYPE_NORMAL;
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        return false;
      }
      if (jobTypeVector.size() != 0) {
        jobType += jobTypeVector.get(0).toString();
      }
      for (int i = 1; i < jobTypeVector.size(); i++) {
        jobType += Utils.JOBTYPE_LIST_SEPARATOR
            + jobTypeVector.get(i).toString();
      }
      String guiJobId = "";
      Date guiSubmissionTime = null;
      //boolean numberException = false;
      try {
        if (jobAd.hasAttribute(Utils.GUI_JOB_ID_ATTR_NAME)) {
          guiJobId = jobAd.getStringValue(Utils.GUI_JOB_ID_ATTR_NAME).get(0)
              .toString().trim();
          if (jobAd.hasAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME)) {
            try {
              guiSubmissionTime = new Date(Long.parseLong(
                  jobAd.getStringValue(Utils.GUI_SUBMISSION_TIME_ATTR_NAME)
                      .get(0).toString().trim(), 10));
            } catch (NumberFormatException nfe) {
              // Do nothing. Submission Time not available.
              // numberException = true;
            }
          }
        }
        GUIFileSystem.saveTextFile(inputFile.toString(), jobAd.toLines());
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        return false;
      }
      Vector vectorElement = new Vector();
      vectorElement.add(keyJobName);
      if (guiJobId.equals("")) { // The file has not been submitted.
        vectorElement.add(Utils.NOT_SUBMITTED_TEXT);
        vectorElement.add("");
      } else {
        vectorElement.add(guiJobId);
        //if (!numberException) {
        vectorElement.add(guiSubmissionTime);
        //} else {
        //  vectorElement.add(Utils.UNABLE_SUBMISSION_TIME);
        //}
      }
      vectorElement.add(jobType);
      jobTableModel.addRow(vectorElement);
      jTableJobs.repaint();
      addJobNameVectorElement(keyJobName);
      updateTotalDisplayedJobsLabel();
      return true;
    } else { // Dag.
      String guiJobId = "";
      Date guiSubmissionTime = null;
      try {
        if (ad.hasAttribute(Utils.GUI_JOB_ID_ATTR_NAME)) {
          guiJobId = ad.getStringValue(Utils.GUI_JOB_ID_ATTR_NAME).get(0)
              .toString().trim();
          if (ad.hasAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME)) {
            guiSubmissionTime = new Date(Long.parseLong(ad.getStringValue(
                Utils.GUI_SUBMISSION_TIME_ATTR_NAME).get(0).toString().trim(),
                10));
          }
        }
        GUIFileSystem.saveTextFile(inputFile.toString(), ad.toString());
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        return false;
      }
      Vector vectorElement = new Vector();
      vectorElement.add(keyJobName);
      if (guiJobId.equals("")) { // The file has not been submitted.
        vectorElement.add(Utils.NOT_SUBMITTED_TEXT);
        vectorElement.add("");
      } else {
        vectorElement.add(guiJobId);
        vectorElement.add(guiSubmissionTime);
      }
      vectorElement.add(Jdl.TYPE_DAG);
      jobTableModel.addRow(vectorElement);
      jTableJobs.repaint();
      addJobNameVectorElement(keyJobName);
      updateTotalDisplayedJobsLabel();
      return true;
    }
  }

  /**
   * Adds, into the <code>targetNSName</code> Network Server panel Job Table,
   * the job represented by the jdl file <code>inputFile</code> with the name
   * specified by <code>keyJobName</code>.
   * 
   * @param targetNSName
   *          target Network Server panel name
   * @param keyJobName
   *          the name of the job to add
   * @param inputFile
   *          the jdl representation of the job to add
   */
  protected void addJobToTable(String targetNSName, String keyJobName,
      File inputFile) {
    File outputFile = new File(GUIFileSystem.getJobTemporaryFileDirectory()
        + targetNSName + File.separator + keyJobName
        + GUIFileSystem.JDL_FILE_EXTENSION);
    logger.info("addJobToTable() - input file: " + inputFile.toString());
    logger.info("addJobToTable() - output file: " + outputFile.toString());
    try {
      File RBDirectory = new File(GUIFileSystem.getJobTemporaryFileDirectory()
          + targetNSName);
      if (!RBDirectory.isDirectory()) {
        RBDirectory.mkdirs();
      }
      GUIFileSystem.copyFile(inputFile, outputFile);
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      JOptionPane.showOptionDialog(NSPanel.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return;
    }
    String type = "";
    Ad ad = new Ad();
    try {
      ad.fromFile(outputFile.toString());
      type = ad.getStringValue(Jdl.TYPE).get(0).toString();
    } catch (NoSuchFieldException nsfe) {
      // type attribute not present. Do nothing.
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      return;
    }
    String jobType = "";
    if (!type.equals(Jdl.TYPE_DAG)) {
      JobAd jobAd = new JobAd();
      Vector jobTypeVector = new Vector();
      try {
        jobAd.fromFile(outputFile.toString());
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(NSPanel.this, e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        return;
      }
      try {
        if (jobAd.hasAttribute(Jdl.JOBTYPE)) {
          jobTypeVector = jobAd.getStringValue(Jdl.JOBTYPE);
        } else {
          jobType = Jdl.JOBTYPE_NORMAL;
        }
        boolean toSave = false;
        if (jobAd.hasAttribute(Utils.GUI_JOB_ID_ATTR_NAME)) {
          jobAd.delAttribute(Utils.GUI_JOB_ID_ATTR_NAME);
          toSave = true;
        }
        if (jobAd.hasAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME)) {
          jobAd.delAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME);
          toSave = true;
        }
        if (toSave) {
          GUIFileSystem.saveTextFile(outputFile, jobAd.toLines());
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(NSPanel.this, e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
      if (jobTypeVector.size() != 0) {
        jobType += jobTypeVector.get(0).toString();
      }
      for (int i = 1; i < jobTypeVector.size(); i++) {
        jobType += Utils.JOBTYPE_LIST_SEPARATOR
            + jobTypeVector.get(i).toString();
      }
    } else { // Dag.
      jobType = Jdl.TYPE_DAG;
      try {
        boolean toSave = false;
        if (ad.hasAttribute(Utils.GUI_JOB_ID_ATTR_NAME)) {
          ad.delAttribute(Utils.GUI_JOB_ID_ATTR_NAME);
          toSave = true;
        }
        if (ad.hasAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME)) {
          ad.delAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME);
          toSave = true;
        }
        if (toSave) {
          GUIFileSystem.saveTextFile(outputFile, ad.toString());
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(NSPanel.this, e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
    }
    Vector vectorElement = new Vector();
    //int length = keyJobName.length();
    JobTableModel jobTableModel = ((JobTableModel) jTableJobs.getModel());
    vectorElement.add(keyJobName);
    vectorElement.add(Utils.NOT_SUBMITTED_TEXT);
    vectorElement.add("");
    vectorElement.add(jobType);
    jobTableModel.addRow(vectorElement);
    jTableJobs.repaint();
    addJobNameVectorElement(keyJobName);
    updateTotalDisplayedJobsLabel();
  }

  /**
   * Adds, into the <code>targetNSPanel</code> Network Server panel Job Table,
   * the job represented by the jdl file <code>inputFile</code> with the name
   * specified by <code>keyJobName</code>.
   * 
   * @param targetNSPanel
   *          target Network Server panel
   * @param keyJobName
   *          the name of the job to add
   * @param inputFile
   *          the jdl representation of the job to add
   */
  protected void addJobToTable(NSPanel targetNSPanel, String keyJobName,
      File inputFile) {
    File outputFile = new File(GUIFileSystem.getJobTemporaryFileDirectory()
        + targetNSPanel.name + File.separator + keyJobName
        + GUIFileSystem.JDL_FILE_EXTENSION);
    try {
      File RBDirectory = new File(GUIFileSystem.getJobTemporaryFileDirectory()
          + targetNSPanel.name);
      if (!RBDirectory.isDirectory()) {
        RBDirectory.mkdirs();
      }
      GUIFileSystem.copyFile(inputFile, outputFile);
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      JOptionPane.showOptionDialog(NSPanel.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return;
    }
    Ad ad = new Ad();
    try {
      ad.fromFile(outputFile.toString());
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      return;
    }
    String type = "";
    try {
      type = ad.getStringValue(Jdl.TYPE).get(0).toString();
    } catch (Exception e) {
      // type attribute not present. Do nothing.
    }
    String jobType = "";
    if (!type.equals(Jdl.TYPE_DAG)) {
      JobAd jobAd = new JobAd();
      Vector jobTypeVector = new Vector();
      try {
        jobAd.fromFile(inputFile.toString());
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(NSPanel.this, e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        return;
      }
      try {
        if (jobAd.hasAttribute(Jdl.JOBTYPE)) {
          jobTypeVector = jobAd.getStringValue(Jdl.JOBTYPE);
        } else {
          jobType = Jdl.JOBTYPE_NORMAL;
        }
        boolean toSave = false;
        if (jobAd.hasAttribute(Utils.GUI_JOB_ID_ATTR_NAME)) {
          jobAd.delAttribute(Utils.GUI_JOB_ID_ATTR_NAME);
          toSave = true;
        }
        if (jobAd.hasAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME)) {
          jobAd.delAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME);
          toSave = true;
        }
        if (toSave) {
          GUIFileSystem.saveTextFile(outputFile, jobAd.toLines());
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(NSPanel.this, e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
      if (jobTypeVector.size() != 0) {
        jobType += jobTypeVector.get(0).toString();
      }
      for (int i = 1; i < jobTypeVector.size(); i++) {
        jobType += Utils.JOBTYPE_LIST_SEPARATOR
            + jobTypeVector.get(i).toString();
      }
    } else { // Dag.
      jobType = Jdl.TYPE_DAG;
      boolean toSave = false;
      try {
        if (ad.hasAttribute(Utils.GUI_JOB_ID_ATTR_NAME)) {
          ad.delAttribute(Utils.GUI_JOB_ID_ATTR_NAME);
          toSave = true;
        }
        if (ad.hasAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME)) {
          ad.delAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME);
          toSave = true;
        }
        if (toSave) {
          GUIFileSystem.saveTextFile(outputFile, ad.toString());
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(NSPanel.this, e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
    }
    Vector vectorElement = new Vector();
    vectorElement.add(keyJobName);
    vectorElement.add(Utils.NOT_SUBMITTED_TEXT);
    vectorElement.add("");
    vectorElement.add(jobType);
    targetNSPanel.jobTableModel.addRow(vectorElement);
    targetNSPanel.jTableJobs.repaint();
    targetNSPanel.addJobNameVectorElement(keyJobName);
    targetNSPanel.updateTotalDisplayedJobsLabel();
  }

  /**
   * Adds, into the <code>targetNSName</code> Network Server panel Job Table,
   * the job represented by the JobAd <code>jobAd</code>, with the name
   * specified by <code>keyJobName</code>.
   * 
   * @param targetNSName
   *          the name of the Network Server panel
   * @param keyJobName
   *          the name of the job to add
   * @param jobAd
   *          the JobAd representing the job
   */
  protected void addJobToTable(String targetNSName, String keyJobName, Ad jobAd) {
    try {
      File RBDirectory = new File(GUIFileSystem.getJobTemporaryFileDirectory()
          + targetNSName);
      if (!RBDirectory.isDirectory()) {
        RBDirectory.mkdirs();
      }
      String path = GUIFileSystem.getJobTemporaryFileDirectory() + targetNSName
          + File.separator + keyJobName + GUIFileSystem.JDL_FILE_EXTENSION;
      File outputFile = new File(path);
      GUIFileSystem.saveTextFile(outputFile, jobAd.toString(true, true));
      Vector vectorElement = new Vector();
      int length = keyJobName.length();
      JobTableModel jobTableModel = ((JobTableModel) jTableJobs.getModel());
      int row = jobTableModel.getIndexOfElementInColumn(keyJobName,
          JOB_NAME_COLUMN_INDEX);
      String jobType = "";
      String type = "";
      try {
        type = jobAd.getStringValue(Jdl.TYPE).get(0).toString();
      } catch (NoSuchFieldException nsfe) {
        if (isDebugging) {
          nsfe.printStackTrace();
          // Attribute not present in ad.
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(NSPanel.this, e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
      if (!type.equals(Jdl.TYPE_DAG)) {
        try {
          Vector valuesVector = jobAd.getStringValue(Jdl.JOBTYPE);
          jobType += valuesVector.get(0).toString();
          for (int i = 1; i < valuesVector.size(); i++) {
            jobType += Utils.JOBTYPE_LIST_SEPARATOR
                + valuesVector.get(i).toString();
          }
        } catch (NoSuchFieldException nsfe) {
          if (isDebugging) {
            nsfe.printStackTrace();
            // Attribute not present in jobad.
          }
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
          JOptionPane.showOptionDialog(NSPanel.this, e.getMessage(),
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
        }
      } else {
        jobType = Jdl.TYPE_DAG;
      }
      if (row == -1) { // Job Name not present.
        vectorElement.add(keyJobName);
        vectorElement.add(Utils.NOT_SUBMITTED_TEXT);
        vectorElement.add(""); // Submission time.
        vectorElement.add(jobType);
        jobTableModel.addRow(vectorElement);
        jTableJobs.repaint();
        addJobNameVectorElement(keyJobName);
        updateTotalDisplayedJobsLabel();
      } else {
        jobTableModel.setValueAt(jobType, row, JOB_TYPE_COLUMN_INDEX);
      }
    } catch (EOFException eofe) {
      if (isDebugging) {
        eofe.printStackTrace();
      }
    } catch (IOException ioe) {
      if (isDebugging) {
        ioe.printStackTrace();
      }
      JOptionPane.showOptionDialog(NSPanel.this,
          "Some problems occures creating temporary file"
              + "\nApplication will be terminated", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      System.exit(-1);
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
      JOptionPane.showOptionDialog(NSPanel.this, ex.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
    }
  }

  /**
   * Adds, into the <code>targetNSName</code> Network Server panel Job Table,
   * the job represented by the JobAd <code>jobAd</code>, with the name
   * specified by <code>keyJobName</code>.
   * 
   * @param targetNSName
   *          the name of the Network Server panel
   * @param keyJobName
   *          the name of the job to add
   * @param jobAd
   *          the JobAd representing the job
   */
  protected void addDagToTable(String targetNSName, String keyJobName, Ad ad) {
    try {
      File RBDirectory = new File(GUIFileSystem.getJobTemporaryFileDirectory()
          + targetNSName);
      if (!RBDirectory.isDirectory()) {
        RBDirectory.mkdirs();
      }
      String path = GUIFileSystem.getJobTemporaryFileDirectory() + targetNSName
          + File.separator + keyJobName + GUIFileSystem.JDL_FILE_EXTENSION;
      File outputFile = new File(path);
      DataOutputStream dos = new DataOutputStream(new BufferedOutputStream(
          new FileOutputStream(outputFile)));
      dos.writeBytes(ad.toString()); // check for toLines method.
      dos.flush();
      dos.close();
      Vector vectorElement = new Vector();
      int length = keyJobName.length();
      JobTableModel jobTableModel = ((JobTableModel) jTableJobs.getModel());
      int row = jobTableModel.getIndexOfElementInColumn(keyJobName,
          JOB_NAME_COLUMN_INDEX);
      String jobType = "";
      try {
        Vector valuesVector = ad.getStringValue(Jdl.JOBTYPE);
        jobType += valuesVector.get(0).toString();
        for (int i = 1; i < valuesVector.size(); i++) {
          jobType += Utils.JOBTYPE_LIST_SEPARATOR
              + valuesVector.get(i).toString();
        }
      } catch (NoSuchFieldException nsfe) {
        if (isDebugging) {
          nsfe.printStackTrace();
          // Attribute not present in jobad.
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(NSPanel.this, e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
      if (row == -1) { // Job Name not present.
        vectorElement.add(keyJobName);
        vectorElement.add(Utils.NOT_SUBMITTED_TEXT);
        vectorElement.add(""); // Submission time.
        if (jobType.equals("")) {
          jobType = Jdl.TYPE_DAG;
        }
        vectorElement.add(jobType);
        jobTableModel.addRow(vectorElement);
        jTableJobs.repaint();
        addJobNameVectorElement(keyJobName);
        updateTotalDisplayedJobsLabel();
      } else {
        jobTableModel.setValueAt(jobType, row, JOB_TYPE_COLUMN_INDEX);
      }
    } catch (EOFException eofe) {
      if (isDebugging) {
        eofe.printStackTrace();
      }
    } catch (IOException ioe) {
      if (isDebugging) {
        ioe.printStackTrace();
      }
      JOptionPane
          .showOptionDialog(
              NSPanel.this,
              "Some problems occures creating temporary file\nApplication will be terminated",
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
      System.exit(-1);
    }
  }

  void updateJobInTable(String rBName, String keyJobName, File inputFile) {
    JTabbedPane jTabbedPane = jobSubmitterJFrame.jTabbedPaneRB;
    NSPanel targetNSPanel = (NSPanel) jTabbedPane.getComponentAt(jTabbedPane
        .indexOfTab(rBName));
    JobTableModel jobTableModel = ((JobTableModel) targetNSPanel.jTableJobs
        .getModel());
    int row = jobTableModel.getIndexOfElementInColumnCi(keyJobName,
        JOB_NAME_COLUMN_INDEX);
    if (row != -1) {
      File outputFile = new File(GUIFileSystem.getJobTemporaryFileDirectory()
          + rBName + File.separator + keyJobName
          + GUIFileSystem.JDL_FILE_EXTENSION);
      logger.debug("updateJobInTable() - input file: " + inputFile.toString());
      logger
          .debug("updateJobInTable() - output file: " + outputFile.toString());
      try {
        GUIFileSystem.copyFile(inputFile, outputFile);
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
      Ad ad = new Ad();
      String type = "";
      try {
        ad.fromFile(outputFile.toString());
        type = ad.getStringValue(Jdl.TYPE).get(0).toString();
      } catch (NoSuchFieldException nsfe) {
        // type attribute not present. Do nothing.
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        return;
      }
      String jobType = "";
      if (!type.equals(Jdl.TYPE_DAG)) {
        JobAd jobAd = new JobAd();
        Vector jobTypeVector = new Vector();
        try {
          jobAd.fromFile(outputFile.toString());
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
          return;
        }
        try {
          if (jobAd.hasAttribute(Jdl.JOBTYPE)) {
            jobTypeVector = jobAd.getStringValue(Jdl.JOBTYPE);
          }
          if (jobAd.hasAttribute(Utils.GUI_JOB_ID_ATTR_NAME)) {
            jobAd.delAttribute(Utils.GUI_JOB_ID_ATTR_NAME);
            if (jobAd.hasAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME)) {
              jobAd.delAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME);
            }
            GUIFileSystem.saveTextFile(outputFile, jobAd.toLines());
          }
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
        }
        if (jobTypeVector.size() != 0) {
          jobType += jobTypeVector.get(0).toString();
        }
        for (int i = 1; i < jobTypeVector.size(); i++) {
          jobType += Utils.JOBTYPE_LIST_SEPARATOR
              + jobTypeVector.get(i).toString();
        }
      } else { // Dag.
        jobType = Jdl.TYPE_DAG;
        try {
          if (ad.hasAttribute(Utils.GUI_JOB_ID_ATTR_NAME)) {
            ad.delAttribute(Utils.GUI_JOB_ID_ATTR_NAME);
            if (ad.hasAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME)) {
              ad.delAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME);
            }
            GUIFileSystem.saveTextFile(outputFile, ad.toString());
          }
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
        }
      }
      NSPanel targetRBPanel = jobSubmitterJFrame.getRBPanelReference(rBName);
      targetRBPanel.jobTableModel.setValueAt(jobType, row,
          JOB_TYPE_COLUMN_INDEX);
    }
  }

  void jMenuRemove() {
    String rBName = this.name;
    int[] selectedRow = jTableJobs.getSelectedRows();
    int selectedRowCount = selectedRow.length;
    if (selectedRowCount != 0) {
      int choice = JOptionPane.showOptionDialog(NSPanel.this,
          "Remove selected Job(s) from table?",
          "JobSubmitter - Confirm Remove", JOptionPane.YES_NO_OPTION,
          JOptionPane.QUESTION_MESSAGE, null, null, null);
      if (choice == 0) {
        File outputFile = null;
        boolean outcome = false;
        String keyJobName = "";
        String jobIdText = "";
        for (int i = selectedRowCount - 1; i >= 0; i--) {
          jobIdText = jTableJobs
              .getValueAt(selectedRow[i], JOB_ID_COLUMN_INDEX).toString();
          keyJobName = jTableJobs.getValueAt(selectedRow[i],
              JOB_NAME_COLUMN_INDEX).toString();
          if (GUIGlobalVars.openedEditorHashMap.containsKey(this.name + " - "
              + keyJobName)) {
            choice = JOptionPane.showOptionDialog(NSPanel.this,
                "A JDL Editor is opened for the '" + keyJobName
                    + "' job\nClose Editor and remove job?",
                Utils.WARNING_MSG_TXT, JOptionPane.YES_NO_OPTION,
                JOptionPane.WARNING_MESSAGE, null, null, null);
            if (choice != 0) {
              continue;
            } else {
              JDLEditor editor = (JDLEditor) GUIGlobalVars.openedEditorHashMap
                  .get(this.name + " - " + keyJobName);
              editor.dispose();
              GUIGlobalVars.openedEditorHashMap.remove(this.name + " - "
                  + keyJobName);
            }
          }
          outputFile = new File(GUIFileSystem.getJobTemporaryFileDirectory()
              + rBName + File.separator + keyJobName
              + GUIFileSystem.JDL_FILE_EXTENSION);
          try {
            outcome = outputFile.delete();
          } catch (Exception ex) {
            if (isDebugging) {
              ex.printStackTrace();
            }
            JOptionPane.showOptionDialog(NSPanel.this,
                "Temporary file deleting error"
                    + "\nApplication will be terminated", Utils.ERROR_MSG_TXT,
                JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
                null, null);
            System.exit(-1);
          }
          if (outcome) {
            ListmatchFrame listmatch = (ListmatchFrame) GUIGlobalVars.openedListmatchMap
                .get(this.name + " - " + keyJobName);
            if (listmatch != null) {
              listmatch.dispose();
              GUIGlobalVars.openedListmatchMap.remove(this.name + " - "
                  + keyJobName);
            }
            jobTableModel.removeRow(selectedRow[i]);
            jobNameVector.remove(selectedRow[i]);
          } else {
            JOptionPane.showOptionDialog(NSPanel.this, "Unable to remove \'"
                + keyJobName + "\' job", "Job Submitter - Remove",
                JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
                null, null);
          }
        }
        updateTotalDisplayedJobsLabel();
      }
    }
  }

  void jMenuClear() {
    int choice = JOptionPane.showOptionDialog(NSPanel.this, "Clear job table?",
        "Job Submitter - Confirm Clear", JOptionPane.YES_NO_OPTION,
        JOptionPane.QUESTION_MESSAGE, null, null, null);
    if (choice == 0) {
      int rowCount = jobTableModel.getRowCount();
      File fileToRemove = null;
      String keyJobName = "";
      String jobIdText = "";
      String deletingErrorMsg = "";
      String temporaryFileDirectory = GUIFileSystem
          .getJobTemporaryFileDirectory();
      boolean outcome = false;
      for (int i = rowCount - 1; i >= 0; i--) {
        jobIdText = jobTableModel.getValueAt(i, JOB_ID_COLUMN_INDEX).toString();
        keyJobName = jobTableModel.getValueAt(i, JOB_NAME_COLUMN_INDEX)
            .toString();
        if (GUIGlobalVars.openedEditorHashMap.containsKey(this.name + " - "
            + keyJobName)) {
          choice = JOptionPane.showOptionDialog(NSPanel.this,
              "A JDL Editor is opened for the '" + keyJobName
                  + "' job\nClose Editor and remove job?",
              Utils.WARNING_MSG_TXT, JOptionPane.YES_NO_OPTION,
              JOptionPane.WARNING_MESSAGE, null, null, null);
          if (choice != 0) {
            continue;
          } else {
            JDLEditor editor = (JDLEditor) GUIGlobalVars.openedEditorHashMap
                .get(this.name + " - " + keyJobName);
            editor.dispose();
            GUIGlobalVars.openedEditorHashMap.remove(this.name + " - "
                + keyJobName);
          }
        }
        fileToRemove = new File(temporaryFileDirectory + this.name
            + File.separator + keyJobName + GUIFileSystem.JDL_FILE_EXTENSION);
        try {
          outcome = fileToRemove.delete();
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
        }
        if (outcome) {
          ListmatchFrame listmatch = (ListmatchFrame) GUIGlobalVars.openedListmatchMap
              .get(this.name + " - " + keyJobName);
          if (listmatch != null) {
            listmatch.dispose();
            GUIGlobalVars.openedListmatchMap.remove(this.name + " - "
                + keyJobName);
          }
          jobTableModel.removeRow(i);
        } else {
          deletingErrorMsg += "- '" + keyJobName + "'\n";
        }
      }
      deletingErrorMsg = deletingErrorMsg.trim();
      if (!deletingErrorMsg.equals("")) {
        GraphicUtils.showOptionDialogMsg(NSPanel.this, deletingErrorMsg,
            "Job Submitter - Clear", JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
            "Unable to remove job(s):", null);
      }
      updateTotalDisplayedJobsLabel();
    }
  }

  void jMenuRename() {
    int[] selectedRows = jTableJobs.getSelectedRows();
    int row = selectedRows[0];
    if (selectedRows.length == 1) {
      String jobIdText = jobTableModel.getValueAt(row, JOB_ID_COLUMN_INDEX)
          .toString().trim();
      if (jobIdText.equals(Utils.SUBMITTING_TEXT)) {
        JOptionPane.showOptionDialog(NSPanel.this,
            "Cannot rename job, submission is on-going",
            "Job Submitter - Rename", JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE, null, null, null);
        return;
      } else if (jobIdText.equals(Utils.LISTMATCHING_TEXT)) {
        JOptionPane.showOptionDialog(NSPanel.this,
            "Cannot rename job, Serching CE is in on-going",
            "Job Submitter - Rename", JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE, null, null, null);
        return;
      } else if (!jobIdText.equals(Utils.NOT_SUBMITTED_TEXT)) {
        JOptionPane.showOptionDialog(NSPanel.this,
            "Cannot rename submitted job", "Job Submitter - Rename",
            JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null,
            null, null);
        return;
      }
      String oldName = jobTableModel.getValueAt(row, JOB_NAME_COLUMN_INDEX)
          .toString().trim();
      String newName;
      Object input = JOptionPane.showInputDialog(NSPanel.this,
          "Insert new job name for '" + oldName + "':\n",
          "Job Submitter - Rename", JOptionPane.PLAIN_MESSAGE, null, null,
          oldName);
      if (input != null) {
        newName = input.toString().trim();
        if (oldName.equals(newName) || newName.equals("")) {
          return;
        }
        if (jobTableModel.isElementPresentInColumnCi(newName,
            JOB_NAME_COLUMN_INDEX)) {
          JOptionPane.showOptionDialog(NSPanel.this, "Cannot rename job '"
              + oldName + "'\ninserted job name is already present",
              "Job Submitter - Rename", JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
          return;
        }
        JTabbedPane jTabbedPaneRB = jobSubmitterJFrame.jTabbedPaneRB;
        NSPanel selectedRB = (NSPanel) jTabbedPaneRB.getSelectedComponent();
        String rBName = jTabbedPaneRB.getTitleAt(jTabbedPaneRB
            .getSelectedIndex());
        String oldFilePath = GUIFileSystem.getJobTemporaryFileDirectory()
            + rBName + File.separator + oldName
            + GUIFileSystem.JDL_FILE_EXTENSION;
        String newFilePath = GUIFileSystem.getJobTemporaryFileDirectory()
            + rBName + File.separator + newName
            + GUIFileSystem.JDL_FILE_EXTENSION;
        logger.info("jMenuRename() - Old file name: " + oldFilePath);
        logger.info("jMenuRename() - New file name: " + newFilePath);
        if (Utils.getOperatingSystem() == Utils.WINDOWS) {
          oldFilePath = oldFilePath.toUpperCase();
          newFilePath = newFilePath.toUpperCase();
        }
        if (!oldFilePath.equals(newFilePath)) {
          File oldFile = new File(oldFilePath);
          File newFile = new File(newFilePath);
          boolean outcome = false;
          try {
            outcome = oldFile.renameTo(newFile);
          } catch (Exception ex) {
            if (isDebugging) {
              ex.printStackTrace();
            }
            JOptionPane.showOptionDialog(NSPanel.this, Utils.FATAL_ERROR
                + "Temporary file renaming error"
                + "\nApplication will be terminated", "Job Submitter - Rename",
                JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
                null, null);
            System.exit(-1);
          }
          if (!outcome) {
            JOptionPane.showOptionDialog(NSPanel.this, "Unable to rename job '"
                + oldName + "' to '" + newName + "'", "Job Submitter - Rename",
                JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
                null, null);
          } else {
            jobTableModel.setValueAt(newName, row, JOB_NAME_COLUMN_INDEX);
            JDLEditor editor = (JDLEditor) GUIGlobalVars.openedEditorHashMap
                .get(rBName + " - " + oldName);
            if (editor != null) {
              GUIGlobalVars.openedEditorHashMap
                  .remove(rBName + " - " + oldName);
              GUIGlobalVars.openedEditorHashMap.put(rBName + " - " + newName,
                  editor);
              editor.keyJobName = newName;
              editor.setTitle("JDL Editor - " + rBName + " - " + newName);
            }
            ListmatchFrame listmatch = (ListmatchFrame) GUIGlobalVars.openedListmatchMap
                .get(rBName + " - " + oldName);
            if (listmatch != null) {
              GUIGlobalVars.openedListmatchMap.remove(rBName + " - " + oldName);
              GUIGlobalVars.openedListmatchMap.put(rBName + " - " + newName,
                  listmatch);
              listmatch.setJobName(newName);
              String title = listmatch.getTitle();
              listmatch.setTitle(title.substring(0, title.lastIndexOf("-") + 2)
                  + newName);
            }
          }
        }
      }
    }
  }

  void jMenuSelectAll() {
    jTableJobs.selectAll();
    jLabelTotalSelectedJobs.setText(Integer.toString(jTableJobs
        .getSelectedRowCount()));
  }

  void jMenuSelectNone() {
    jTableJobs.clearSelection();
    jLabelTotalSelectedJobs.setText(Integer.toString(0));
  }

  void jMenuInvertSelection() {
    int rowCount = jTableJobs.getRowCount();
    int selectedRowCount = jTableJobs.getSelectedRowCount();
    if (selectedRowCount != 0) {
      ListSelectionModel listSelectionModel = jTableJobs.getSelectionModel();
      for (int i = 0; i < rowCount; i++) {
        if (listSelectionModel.isSelectedIndex(i)) {
          listSelectionModel.removeSelectionInterval(i, i);
        } else {
          listSelectionModel.addSelectionInterval(i, i);
        }
      }
    } else {
      jTableJobs.selectAll();
    }
    jLabelTotalSelectedJobs.setText(Integer.toString(jTableJobs
        .getSelectedRowCount()));
  }

  void jMenuSelectSubmitted() {
    String jobIdText = "";
    jTableJobs.clearSelection();
    for (int i = 0; i < jobTableModel.getRowCount(); i++) {
      jobIdText = jobTableModel.getValueAt(i, JOB_ID_COLUMN_INDEX).toString();
      if (isSubmittedJob(jobIdText)) {
        jTableJobs.addRowSelectionInterval(i, i);
      }
    }
    jLabelTotalSelectedJobs.setText(Integer.toString(jTableJobs
        .getSelectedRowCount()));
  }

  void jMenuInteractiveConsole() {
    final SwingWorker worker = new SwingWorker() {
      public Object construct() {
        // Standard code
        int[] selectedRows = jTableJobs.getSelectedRows();
        if (selectedRows.length == 1) {
          String jobIdText = jTableJobs.getValueAt(selectedRows[0],
              JOB_ID_COLUMN_INDEX).toString();
          if (!GUIGlobalVars.openedListenerFrameMap.containsKey(jobIdText)) {
            ListenerFrame listener = new ListenerFrame();
            //!!! add this method to ListerFrame class
            // listener.setJobIdTextField(jobIdText);
            GUIGlobalVars.openedListenerFrameMap.put(jobIdText, listener);
            GraphicUtils.screenCenterWindow(listener);
            listener.show();
          } else {
            ListenerFrame listener = (ListenerFrame) GUIGlobalVars.openedListenerFrameMap
                .get(jobIdText);
            GraphicUtils.deiconifyFrame(listener);
            listener.setVisible(true);
          }
        }
        // END Standard code
        return "";
      }
    };
    worker.start();
  }

  void jMenuAttachCheckpointState() {
    int[] selectedRows = jTableJobs.getSelectedRows();
    if (selectedRows.length == 1) {
      String keyJobName = (String) jTableJobs.getValueAt(selectedRows[0],
          JOB_NAME_COLUMN_INDEX);
      String temporaryPhysicalFileName = GUIFileSystem
          .getJobTemporaryFileDirectory()
          + this.name
          + File.separator
          + keyJobName
          + GUIFileSystem.JDL_FILE_EXTENSION;
      JFileChooser fileChooser = new JFileChooser();
      fileChooser.setDialogTitle("Link Checkpoint State");
      fileChooser.setApproveButtonToolTipText("Link Checkpoint State File");
      fileChooser.setApproveButtonText("Link");
      fileChooser.setCurrentDirectory(new File(GUIGlobalVars
          .getFileChooserWorkingDirectory()));
      String[] extensions = { "CHKPT"
      };
      GUIFileFilter classadFileFilter = new GUIFileFilter("*.chkpt", extensions);
      fileChooser.addChoosableFileFilter(classadFileFilter);
      File jobStateFile = new File(
          getAttachedJobState(temporaryPhysicalFileName));
      if (jobStateFile.isFile()) {
        fileChooser.setSelectedFile(jobStateFile);
      }
      int choice = fileChooser.showOpenDialog(NSPanel.this);
      if (choice != JFileChooser.APPROVE_OPTION) {
        return;
      } else {
        File inputFile = fileChooser.getSelectedFile();
        String selectedFile = inputFile.toString();
        if (inputFile.isFile()) {
          JobState jobState = new JobState();
          try {
            jobState.fromFile(selectedFile);
            jobState.check();
          } catch (Exception e) {
            if (isDebugging) {
              e.printStackTrace();
            }
            GraphicUtils.showOptionDialogMsg(NSPanel.this, e.getMessage(),
                "Job Submitter - Link Checkpoint State File",
                JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE,
                Utils.MESSAGE_LINES_PER_JOPTIONPANE, null, null);
            return;
          }
          try {
            JobAd jobAd = new JobAd();
            jobAd.fromFile(temporaryPhysicalFileName);
            if (jobAd.hasAttribute(Utils.GUI_CHECKPOINT_STATE_FILE)) {
              jobAd.delAttribute(Utils.GUI_CHECKPOINT_STATE_FILE);
            }
            jobAd.setAttribute(Utils.GUI_CHECKPOINT_STATE_FILE, selectedFile);
            GUIFileSystem.saveTextFile(temporaryPhysicalFileName, jobAd
                .toString(true, true));
          } catch (Exception e) {
            if (isDebugging) {
              e.printStackTrace();
            }
            JOptionPane.showOptionDialog(NSPanel.this, e.getMessage(),
                Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE, null, null, null);
            return;
          }
        } else {
          JOptionPane.showOptionDialog(NSPanel.this, "Unable to find file: "
              + selectedFile, Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
        }
      }
    }
  }

  void jMenuDetachCheckpointState() {
    int[] selectedRows = jTableJobs.getSelectedRows();
    String keyJobName = (String) jTableJobs.getValueAt(selectedRows[0],
        JOB_NAME_COLUMN_INDEX);
    if (selectedRows.length == 1) {
      int choice = JOptionPane.showOptionDialog(NSPanel.this,
          "Unlink Checkpoint State for job " + keyJobName + "?",
          "Job Submitter - Confirm Checkpoint State Unlink",
          JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null,
          null);
      if (choice == 0) {
        String temporaryPhysicalFileName = GUIFileSystem
            .getJobTemporaryFileDirectory()
            + this.name
            + File.separator
            + keyJobName
            + GUIFileSystem.JDL_FILE_EXTENSION;
        JobState jobState = new JobState();
        try {
          jobState.fromFile(temporaryPhysicalFileName);
          jobState.check();
          if (jobState.hasAttribute(Utils.GUI_CHECKPOINT_STATE_FILE)) {
            jobState.delAttribute(Utils.GUI_CHECKPOINT_STATE_FILE);
            GUIFileSystem.saveTextFile(temporaryPhysicalFileName, jobState
                .toString(true, true));
          } else {
            JOptionPane.showOptionDialog(NSPanel.this,
                "No checkpoint state link for job " + keyJobName,
                "Job Submitter - Checkpoint State Unlink",
                JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE,
                null, null, null);
          }
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
          GraphicUtils.showOptionDialogMsg(NSPanel.this, e.getMessage(),
              "Job Submitter - Checkpoint State Unlink",
              JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE,
              Utils.MESSAGE_LINES_PER_JOPTIONPANE, null, null);
        }
      }
    }
  }

  void jMenuViewCheckpointStateAttach() {
    int[] selectedRows = jTableJobs.getSelectedRows();
    if (selectedRows.length == 1) {
      String keyJobName = (String) jTableJobs.getValueAt(selectedRows[0],
          JOB_NAME_COLUMN_INDEX);
      String temporaryPhysicalFileName = GUIFileSystem
          .getJobTemporaryFileDirectory()
          + this.name
          + File.separator
          + keyJobName
          + GUIFileSystem.JDL_FILE_EXTENSION;
      String checkpointStateFile = getAttachedJobState(temporaryPhysicalFileName);
      if (!checkpointStateFile.equals("")) {
        Object[] options = { "View File", "   OK   "
        };
        int choice = JOptionPane.showOptionDialog(NSPanel.this,
            "Checkpoint state link for job '" + keyJobName + "':\n"
                + checkpointStateFile,
            "Job Submitter - View Checkpoint State Link",
            JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
            options, null);
        if (choice == 0) {
          JobState jobState = new JobState();
          try {
            jobState.fromFile(checkpointStateFile);
            jobState.check();
            if (GUIGlobalVars.openedCheckpointStateMap
                .containsKey(checkpointStateFile)) {
              CheckpointStateFrame checkpointState = (CheckpointStateFrame) GUIGlobalVars.openedCheckpointStateMap
                  .get(checkpointStateFile);
              checkpointState.setVisible(false);
              GraphicUtils.deiconifyFrame(checkpointState);
              checkpointState.setVisible(true);
            } else {
              CheckpointStateFrame checkpointState = new CheckpointStateFrame(
                  this, jobState.toString(true, true), checkpointStateFile);
              GUIGlobalVars.openedCheckpointStateMap.put(checkpointStateFile,
                  checkpointState);
              GraphicUtils.screenCenterWindow(checkpointState);
              checkpointState.setVisible(true);
            }
          } catch (Exception e) {
            if (isDebugging) {
              e.printStackTrace();
            }
            GraphicUtils.showOptionDialogMsg(NSPanel.this, e.getMessage(),
                "Job Submitter - View Checkpoint State Link",
                JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE,
                Utils.MESSAGE_LINES_PER_JOPTIONPANE, null, null);
          }
        }
      } else {
        JOptionPane.showOptionDialog(NSPanel.this,
            "No checkpoint state link for job '" + keyJobName + "'",
            "Job Submitter - View Checkpoint State Link",
            JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
            null, null);
      }
    }
  }

  void jMenuSendToMonitor() {
    Vector newJobVector = new Vector();
    String element;
    String jobId;
    String warningMsg = "";
    String jobName;
    int[] selectedRow = jTableJobs.getSelectedRows();
    for (int i = 0; i < selectedRow.length; i++) {
      jobId = jobTableModel.getValueAt(selectedRow[i], JOB_ID_COLUMN_INDEX)
          .toString().trim();
      if (isSubmittedJob(jobId)) {
        newJobVector.add(jobId);
      } else {
        jobName = jobTableModel.getValueAt(selectedRow[i],
            JOB_NAME_COLUMN_INDEX).toString().trim();
        warningMsg += "- " + jobName + "\n";
      }
    }
    if (newJobVector.size() != 0) {
      jobSubmitterJFrame.addSubmittedJobsToMonitor(newJobVector);
    }
    warningMsg = warningMsg.trim();
    if (!warningMsg.equals("")) {
      GraphicUtils.showOptionDialogMsg(jobSubmitterJFrame.jobMonitorJFrame,
          warningMsg, Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
          "Cannot monitor job(s):",
          "Not submitted job(s) or submission/Searching CE is on-going");
    }
  }

  void jMenuOpenInEditor() {
    jobSubmitterJFrame.jButtonEditorEvent(null, false);
  }

  void jMenuSubmit() {
    jobSubmitterJFrame.jButtonSubmitEvent(null);
  }

  boolean hasSubmittedJobs() {
    int rowCount = jobTableModel.getRowCount();
    String jobIdText = "";
    for (int i = 0; i < rowCount; i++) {
      jobIdText = jobTableModel.getValueAt(i, JOB_ID_COLUMN_INDEX).toString()
          .trim();
      if (isSubmittedJob(jobIdText)) {
        return true;
      }
    }
    return false;
  }

  boolean hasSubmittingJobs() {
    int rowCount = jobTableModel.getRowCount();
    for (int i = 0; i < rowCount; i++) {
      if (jobTableModel.getValueAt(i, JOB_ID_COLUMN_INDEX).toString().trim()
          .equals(Utils.SUBMITTING_TEXT)) {
        return true;
      }
    }
    return false;
  }

  boolean hasJobs() {
    return (jobTableModel.getRowCount() > 0) ? true : false;
  }

  void setNSMenuItems(Vector nsVector) {
    int nsVectorSize = nsVector.size();
    if (nsVectorSize != 0) {
      JMenuItem jMenuItem;
      ActionListener alst;
      jMenuMoveTo.removeAll();
      jMenuCopyTo.removeAll();
      String nsName;
      for (int i = 0; i < nsVectorSize; i++) {
        nsName = nsVector.get(i).toString();
        if (!nsName.equals(this.name)) {
          jMenuItem = new JMenuItem(nsName);
          alst = new ActionListener() {
            public void actionPerformed(ActionEvent e) {
              jMenuMoveTo(e);
            }
          };
          jMenuItem.addActionListener(alst);
          jMenuMoveTo.add(jMenuItem);
        }
        jMenuItem = new JMenuItem(nsName);
        alst = new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jMenuCopyTo(e);
          }
        };
        jMenuItem.addActionListener(alst);
        jMenuCopyTo.add(jMenuItem);
      }
    }
  }

  String getAttachedJobState(String jobStateFileName) {
    String checkpointStateFile = "";
    JobState jobState = new JobState();
    try {
      jobState.fromFile(jobStateFileName);
      jobState.check();
      if (jobState.hasAttribute(Utils.GUI_CHECKPOINT_STATE_FILE)) {
        checkpointStateFile = jobState.getStringValue(
            Utils.GUI_CHECKPOINT_STATE_FILE).get(0).toString();
      }
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      JOptionPane.showOptionDialog(NSPanel.this, Utils.UNESPECTED_ERROR
          + e.getMessage(), Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
    }
    return checkpointStateFile;
  }

  String getSelectedCEId(String jobFileName) {
    String ceId = "";
    JobAd jobAd = new JobAd();
    try {
      jobAd.fromFile(jobFileName);
      jobAd.checkAll();
      if (jobAd.hasAttribute(Jdl.CEID)) {
        ceId = jobAd.getStringValue(Jdl.CEID).get(0).toString();
      }
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      JOptionPane.showOptionDialog(NSPanel.this, Utils.UNESPECTED_ERROR
          + e.getMessage(), Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
    }
    return ceId;
  }

  void jLabelAddressValueFocusLost(FocusEvent e) {
    GraphicUtils.jTextFieldDeselect(jLabelAddressValue);
  }

  void jLabelInformationServiceSchemaValueFocusLost(FocusEvent e) {
    GraphicUtils.jTextFieldDeselect(jLabelInformationServiceSchemaValue);
  }

  void jMenuViewJobCEIdSelection() {
    int[] selectedRows = jTableJobs.getSelectedRows();
    if (selectedRows.length == 1) {
      String keyJobName = (String) jTableJobs.getValueAt(selectedRows[0],
          JOB_NAME_COLUMN_INDEX);
      String temporaryPhysicalFileName = GUIFileSystem
          .getJobTemporaryFileDirectory()
          + this.name
          + File.separator
          + keyJobName
          + GUIFileSystem.JDL_FILE_EXTENSION;
      String selectedCEId = getSelectedCEId(temporaryPhysicalFileName);
      if (!selectedCEId.equals("")) {
        JOptionPane.showOptionDialog(NSPanel.this, "Selected CE Id for job '"
            + keyJobName + "':\n" + selectedCEId,
            "Job Submitter - View CE Id Selection", JOptionPane.DEFAULT_OPTION,
            JOptionPane.INFORMATION_MESSAGE, null, null, null);
      }
    }
  }

  void jMenuRemoveJobCEIdSelection() {
    int[] selectedRows = jTableJobs.getSelectedRows();
    String keyJobName = (String) jTableJobs.getValueAt(selectedRows[0],
        JOB_NAME_COLUMN_INDEX);
    if (selectedRows.length == 1) {
      int choice = JOptionPane.showOptionDialog(NSPanel.this,
          "Remove CE Id selection for job '" + keyJobName + "'?",
          "Job Submitter - Confirm Remove Job CE Id Selection",
          JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null,
          null);
      if (choice == 0) {
        String temporaryPhysicalFileName = GUIFileSystem
            .getJobTemporaryFileDirectory()
            + this.name
            + File.separator
            + keyJobName
            + GUIFileSystem.JDL_FILE_EXTENSION;
        try {
          JobAd jobAd = new JobAd();
          jobAd.fromFile(temporaryPhysicalFileName);
          if (jobAd.hasAttribute(Jdl.CEID)) {
            jobAd.delAttribute(Jdl.CEID);
          }
          GUIFileSystem.saveTextFile(temporaryPhysicalFileName, jobAd.toString(
              true, true));
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
        }
      }
    }
  }

  void saveSelectedJobIdList() {
    int[] selectedRows = jTableJobs.getSelectedRows();
    if (selectedRows.length != 0) {
      Vector jobIdsVector = new Vector();
      String jobId = "";
      for (int i = 0; i < selectedRows.length; i++) {
        jobId = jobTableModel.getValueAt(selectedRows[i], JOB_ID_COLUMN_INDEX)
            .toString();
        if (isSubmittedJob(jobId)) {
          jobIdsVector.add(jobId);
        }
      }
      if (jobIdsVector.size() == 0) {
        JOptionPane.showOptionDialog(NSPanel.this,
            "No selected Job Ids to save",
            "Job Submitter - Save Selected Job Ids List File",
            JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null,
            null, null);
        return;
      }
      JFileChooser fileChooser = new JFileChooser();
      fileChooser.setCurrentDirectory(new File(GUIGlobalVars
          .getFileChooserWorkingDirectory()));
      fileChooser.setDialogTitle("Save Selected Job Ids List File");
      int choice = fileChooser.showSaveDialog(NSPanel.this);
      if (choice != JFileChooser.APPROVE_OPTION) {
        return;
      } else {
        GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
            .getCurrentDirectory().toString());
        String selectedFile = fileChooser.getSelectedFile().toString();
        saveJobIdsFile(selectedFile, jobIdsVector);
      }
    } else {
      JOptionPane.showOptionDialog(NSPanel.this, "No selected Job Ids to save",
          "Job Submitter - Save Selected Job Ids List File",
          JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null, null,
          null);
    }
  }

  void saveJobIdList() {
    int rowCount = jobTableModel.getRowCount();
    if (rowCount != 0) {
      Vector jobIdsVector = new Vector();
      String jobId = "";
      for (int i = 0; i < rowCount; i++) {
        jobId = jobTableModel.getValueAt(i, JOB_ID_COLUMN_INDEX).toString();
        if (isSubmittedJob(jobId)) {
          jobIdsVector.add(jobId);
        }
      }
      if (jobIdsVector.size() == 0) {
        JOptionPane.showOptionDialog(NSPanel.this, "No Job Ids to save",
            "Job Submitter - Save Job Ids List File",
            JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null,
            null, null);
        return;
      }
      JFileChooser fileChooser = new JFileChooser();
      fileChooser.setCurrentDirectory(new File(GUIGlobalVars
          .getFileChooserWorkingDirectory()));
      fileChooser.setDialogTitle("Save Job Ids List File");
      int choice = fileChooser.showSaveDialog(NSPanel.this);
      if (choice != JFileChooser.APPROVE_OPTION) {
        return;
      } else {
        GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
            .getCurrentDirectory().toString());
        String selectedFile = fileChooser.getSelectedFile().toString();
        saveJobIdsFile(selectedFile, jobIdsVector);
      }
    } else {
      JOptionPane.showOptionDialog(NSPanel.this, "No Job Ids to save",
          "Job Submitter - Save Job Ids List File", JOptionPane.DEFAULT_OPTION,
          JOptionPane.WARNING_MESSAGE, null, null, null);
    }
  }

  void saveJobIdsFile(String fileName, Vector jobIdsVector) {
    File outputFile = new File(fileName);
    int choice = 0;
    if (outputFile.isFile()) {
      choice = JOptionPane.showOptionDialog(NSPanel.this,
          "Output file exists. Overwrite?", "Job Submitter - Confirm Save",
          JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null, null,
          null);
    }
    if (choice == 0) {
      try {
        DataOutputStream dos = new DataOutputStream(new BufferedOutputStream(
            new FileOutputStream(fileName)));
        for (int i = 0; i < jobIdsVector.size(); i++) {
          dos.writeBytes(jobIdsVector.get(i).toString() + "\n");
        }
        dos.flush();
        dos.close();
      } catch (EOFException eofe) {
        if (isDebugging) {
          eofe.printStackTrace();
        }
      } catch (IOException ioe) {
        if (isDebugging) {
          ioe.printStackTrace();
        }
        JOptionPane.showOptionDialog(NSPanel.this, "Unable to save file: "
            + outputFile, Utils.ERROR_MSG_TXT, JOptionPane.YES_NO_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
    }
  }

  void removeAllJobs() {
    jobTableModel.removeAllRows();
  }

  boolean isSubmittedJob(String jobIdText) {
    if (!jobIdText.equals(Utils.NOT_SUBMITTED_TEXT)
        && !jobIdText.equals(Utils.SUBMITTING_TEXT)
        && !jobIdText.equals(Utils.LISTMATCHING_TEXT)) {
      return true;
    }
    return false;
  }
}