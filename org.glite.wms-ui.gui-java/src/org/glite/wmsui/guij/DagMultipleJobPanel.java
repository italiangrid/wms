/*
 * DagMultipleJobPanel.java
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
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.util.Date;
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.table.TableColumn;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Ad;
import org.glite.jdl.Jdl;
import org.glite.jdl.JobAd;
import org.glite.jdl.JobAdException;
import org.glite.wmsui.apij.Job;
import org.glite.wmsui.apij.JobCollection;
import org.glite.wmsui.apij.JobId;
import org.glite.wmsui.apij.JobStatus;
import org.glite.wmsui.apij.Result;

/*
 import org.edg.info.Consumer;
 import org.edg.info.ResultSet;
 import org.edg.info.CanonicalProducer;
 import org.edg.info.ServletConnection;
 */
/**
 * Implementation of the DagMultipleJobPanel class.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class DagMultipleJobPanel extends MultipleJobPanel {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int NODE_NAME_COLUMN_INDEX = 5;

  JPanel jPanelDag = new JPanel();

  JLabel jLabelDagJobId = new JLabel("Dag Id");

  JTextField jTextFieldDagJobId = new JTextField();

  JLabel jLabelJobStatus = new JLabel("Status");

  JTextField jTextFieldJobStatus = new JTextField();

  JLabel jLabelSubmissionTime = new JLabel("Submission Time");

  JTextField jTextFieldSubmissionTime = new JTextField();

  JPanel jPanelJobId = new JPanel();

  JPanel jPanelStatus = new JPanel();

  protected String dagJobId;

  protected boolean isFirstTimeNodeName = true;

  protected MultipleJobFrame multipleJobFrame;

  /**
   * Constructor.
   */
  public DagMultipleJobPanel(MultipleJobFrame multipleJobFrame,
      JobMonitor jobMonitorFrame, String dagJobId, String jobStatus,
      String submissionTime, JobCollection jobCollection) throws Exception {
    super(jobMonitorFrame);
    this.multipleJobFrame = multipleJobFrame;
    this.dagJobId = dagJobId;
    this.jobCollection = jobCollection;
    JobId jobId;
    try {
      jobId = new JobId(dagJobId);
    } catch (Exception e) {
      throw e;
    }
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit(jobStatus, submissionTime);
      setJobStatusTableJobs();
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      throw e;
    }
    try {
      this.jobCollection.insertId(jobId);
    } catch (Exception e) {
      throw e;
    }
  }

  private void jbInit(String jobStatus, String submissionTime) throws Exception {
    isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG) ? true
        : false;
    startUpdateThread();
    TableColumn nodeNameColumn = new TableColumn(5);
    nodeNameColumn.setCellRenderer(new GUITableTooltipCellRenderer());
    jobTableModel.addColumn("Node Name");
    jTableJobs.addColumn(nodeNameColumn);
    jTextFieldDagJobId.setText(this.dagJobId);
    jTextFieldDagJobId.setEditable(false);
    jTextFieldDagJobId.setBorder(BorderFactory.createEtchedBorder());
    jTextFieldDagJobId.setBackground(Color.white);
    jTextFieldDagJobId.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldDagJobId);
      }
    });
    jTextFieldJobStatus.setText(jobStatus);
    jTextFieldJobStatus.setEditable(false);
    jTextFieldJobStatus.setPreferredSize(new Dimension(150, 20));
    jTextFieldJobStatus.setMaximumSize(new Dimension(150, 20));
    jTextFieldJobStatus.setBorder(BorderFactory.createEtchedBorder());
    jTextFieldJobStatus.setBackground(Color.white);
    jTextFieldJobStatus.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldJobStatus);
      }
    });
    jTextFieldSubmissionTime.setText(submissionTime);
    jTextFieldSubmissionTime.setEditable(false);
    jTextFieldSubmissionTime.setBorder(BorderFactory.createEtchedBorder());
    jTextFieldSubmissionTime.setBackground(Color.white);
    jTextFieldSubmissionTime.setPreferredSize(new Dimension(200, 20));
    jTextFieldSubmissionTime.setMaximumSize(new Dimension(200, 20));
    jTextFieldSubmissionTime
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            GraphicUtils.jTextFieldDeselect(jTextFieldSubmissionTime);
          }
        });
    jPanelJobId.setLayout(new BoxLayout(jPanelJobId, BoxLayout.X_AXIS));
    jPanelJobId.setBorder(GraphicUtils.SPACING_BORDER);
    jPanelJobId.add(jLabelDagJobId, null);
    jPanelJobId.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelJobId.add(jTextFieldDagJobId, null);
    jPanelStatus.setLayout(new BoxLayout(jPanelStatus, BoxLayout.X_AXIS));
    jPanelStatus.setBorder(GraphicUtils.SPACING_BORDER);
    jPanelStatus.add(jLabelJobStatus, null);
    jPanelStatus.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP + 2));
    jPanelStatus.add(jTextFieldJobStatus, null);
    jPanelStatus.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelStatus.add(Box.createGlue());
    jPanelStatus.add(jLabelSubmissionTime, null);
    jPanelStatus.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelStatus.add(jTextFieldSubmissionTime, null);
    jPanelDag.setBorder(new TitledBorder(new EtchedBorder(), " Dag ", 0, 0,
        null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelDag.setLayout(new BorderLayout());
    jPanelDag.add(jPanelJobId, BorderLayout.NORTH);
    jPanelDag.add(jPanelStatus, BorderLayout.SOUTH);
    jPanelNorth.remove(jPanelJobId);
    jPanelNorth.add(jPanelDag, BorderLayout.SOUTH);
    jPanelJobStatusTable.setBorder(new TitledBorder(new EtchedBorder(),
        " Dag Nodes Status Table ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jButtonDagNodes.setVisible(false);
    //jButtonDagMon.setVisible(false);
    jButtonCancel.setVisible(false);
    //jButtonBack.setVisible(false);
    jButtonBack.setText("Close");
  }

  void jButtonBackEvent(ActionEvent e) {
    this.multipleJobFrame.dispose();
  }

  Vector makeRowToAddVector(JobStatus jobStatus) {
    Vector rowToAddVector = new Vector();
    String jobType = "";
    String jdlText = jobStatus.getValString(JobStatus.JDL).trim();
    int type = jobStatus.getValInt(JobStatus.JOBTYPE);
    if ((jdlText != null) && !jdlText.equals("")) {
      if (type == JobStatus.JOBTYPE_JOB) { // Not a Dag.
        try {
          JobAd jobAd = new JobAd();
          jobAd.fromString(jdlText);
          Vector valuesVector = jobAd.getStringValue(Jdl.JOBTYPE);
          if (valuesVector.size() == 1) {
            jobType = valuesVector.get(0).toString();
          } else if (valuesVector.contains(Jdl.JOBTYPE_INTERACTIVE)
              && valuesVector.contains(Jdl.JOBTYPE_CHECKPOINTABLE)) {
            jobType = Jdl.JOBTYPE_CHECKPOINTABLE + Utils.JOBTYPE_LIST_SEPARATOR
                + Jdl.JOBTYPE_INTERACTIVE;
          } else if (valuesVector.contains(Jdl.JOBTYPE_INTERACTIVE)
              && valuesVector.contains(Jdl.JOBTYPE_MPICH)) {
            jobType = Jdl.JOBTYPE_CHECKPOINTABLE + Utils.JOBTYPE_LIST_SEPARATOR
                + Jdl.JOBTYPE_MPICH;
          }
        } catch (JobAdException jae) {
          // Unespected exception.
          // Ignore it, jobType equals blank string.
          if (isDebugging) {
            jae.printStackTrace();
          }
        } catch (IllegalArgumentException iae) {
          // Unespected exception.
          // Ignore it, jobType equals blank string.
          if (isDebugging) {
            iae.printStackTrace();
          }
        } catch (NoSuchFieldException nsfe) {
          // Unespected exception.
          // Ignore it, jobType equals blank string.
          if (isDebugging) {
            nsfe.printStackTrace();
          }
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
          JOptionPane.showOptionDialog(DagMultipleJobPanel.this,
              e.getMessage(), Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
        }
      }
    }
    rowToAddVector.addElement(jobStatus.getValString(JobStatus.JOB_ID));
    if ((type == JobStatus.JOBTYPE_JOB) && (jobType.equals(""))) {
      jobType = Jdl.JOBTYPE_NORMAL;
    } else if (type == JobStatus.JOBTYPE_DAG) {
      jobType = Jdl.TYPE_DAG;
    }
    rowToAddVector.addElement(jobType);
    String jobStatusName = jobStatus.name().trim();
    if (jobStatusName.indexOf(JobStatus.code[JobStatus.DONE]) != -1) {
      if (jobStatus.getValInt(JobStatus.DONE_CODE) == 1) {
        rowToAddVector.addElement(jobStatusName + Utils.STATE_FAILED);
      } else if (jobStatus.getValInt(JobStatus.DONE_CODE) == 2) {
        rowToAddVector.addElement(jobStatusName + Utils.STATE_CANCELLED);
      } else { // DONE_CODE = 0.
        int exitCode = jobStatus.getValInt(JobStatus.EXIT_CODE);
        if (exitCode != 0) {
          rowToAddVector.addElement(jobStatusName
              + Utils.STATE_EXIT_CODE_NOT_ZERO);
        } else {
          rowToAddVector.addElement(jobStatusName);
        }
      }
    } else {
      if ((jobStatus.getValInt(JobStatus.CANCELLING) == 1)
          && (jobStatus.code() != JobStatus.CANCELLED)) {
        rowToAddVector.addElement(jobStatusName + Utils.STATE_CANCELLING);
      } else {
        rowToAddVector.addElement(jobStatusName);
      }
    }
    Vector stateEnterTimesVector = (Vector) jobStatus
        .get(JobStatus.STATE_ENTER_TIMES);
    logger.debug("State Enter Times (Vector): " + stateEnterTimesVector);
    if (stateEnterTimesVector != null) {
      logger.debug("State Enter Time (Vector.get(1)): "
          + stateEnterTimesVector.get(1).toString().trim());
      rowToAddVector.addElement(Utils.toDate(stateEnterTimesVector.get(1)
          .toString().trim()));
    } else {
      rowToAddVector.addElement("");
    }
    String destination = jobStatus.getValString(JobStatus.DESTINATION);
    if (destination != null) {
      rowToAddVector.addElement(destination);
    } else {
      rowToAddVector.addElement("");
    }
    if (isFirstTimeNodeName) {
      String nodeName = "";
      Ad userTagsAd = new Ad();
      try {
        logger.debug("INSIDE");
        logger.debug("USER_TAGS Ad: "
            + jobStatus.getValString(JobStatus.USER_TAGS));
        userTagsAd.fromString(jobStatus.getValString(JobStatus.USER_TAGS));
        nodeName = userTagsAd.getStringValue(Jdl.EDG_WL_UI_DAG_NODE_NAME)
            .get(0).toString();
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
          // NodeName not present (or some problems occures). Do nothing.
        }
      }
      logger.debug("NODE NAME: " + nodeName);
      rowToAddVector.addElement(nodeName);
    }
    return rowToAddVector;
  }

  void updateJobStatusTableJobs() {
    logger.debug("setJTableJobs() - jobCollection.size(): "
        + DagMultipleJobPanel.this.jobCollection.size());
    try {
      DagMultipleJobPanel.this.jobVector = DagMultipleJobPanel.this.jobCollection
          .getStatus();
    } catch (InterruptedException ie) {
      // Thread has been interrupted, maybe during update event from user command
      // button "Back", do nothing.
      if (isDebugging) {
        ie.printStackTrace();
      }
      return;
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      JOptionPane.showOptionDialog(DagMultipleJobPanel.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return;
    }
    String warningMsg = "";
    try {
      String jobIdText = "";
      String JobIdTextStatus;
      Result jobResult;
      JobStatus jobStatus;
      int resultCode = 0;
      int index = -1;
      for (int i = 0; i < DagMultipleJobPanel.this.jobVector.size(); i++) {
        jobResult = (Result) DagMultipleJobPanel.this.jobVector.get(i);
        if (jobResult != null) {
          resultCode = jobResult.getCode();
          jobIdText = jobResult.getId().trim();
          // Checks if jobCollection contains jobs after update event. User could remove some jobs
          // from table during update event. In this case jobStatus must not be shown, the job is
          // removed from table.
          if (!DagMultipleJobPanel.this.jobCollection.contains(new Job(
              new JobId(jobIdText)))) {
            continue;
          }
          index = jobTableModel.getIndexOfElementInColumn(jobIdText,
              JOB_ID_COLUMN_INDEX);
          if ((resultCode != Result.STATUS_FAILURE)
              && (resultCode != Result.STATUS_FORBIDDEN)) {
            jobStatus = (JobStatus) jobResult.getResult();
            logger.debug("setJTableJobs() - Job Id: " + jobIdText);
            // Stores job status information in hash map structure.
            // This hash map is used when user asks for details, in this case you
            // don't recall getStatus() API (it will be done when you ask for an update).
            if (DagMultipleJobPanel.this.jobStatusHashMap
                .containsKey(jobIdText)) {
              DagMultipleJobPanel.this.jobStatusHashMap.remove(jobIdText);
            }
            DagMultipleJobPanel.this.jobStatusHashMap.put(jobIdText, jobStatus);
            Vector rowToAddVector = makeRowToAddVector(jobStatus); // code different from super version.
            if (index != -1) {
              // // code different from super version. Vector rowToAddVector = makeRowToAddVector(jobStatus);
              if (rowToAddVector.size() != 0) {
                jobTableModel.setValueAt(rowToAddVector.get(1), index,
                    JOB_TYPE_COLUMN_INDEX);
                jobTableModel.setValueAt(rowToAddVector.get(2), index,
                    JOB_STATUS_COLUMN_INDEX);
                jobTableModel.setValueAt(rowToAddVector.get(3), index,
                    SUBMISSION_TIME_COLUMN_INDEX);
                jobTableModel.setValueAt(rowToAddVector.get(4), index,
                    DESTINATION_COLUMN_INDEX);
                if (isFirstTimeNodeName) {
                  jobTableModel.setValueAt(rowToAddVector.get(5), index,
                      NODE_NAME_COLUMN_INDEX);
                }
              }
            } else {
              logger.debug("setJTableJobs() - Job Id not present in column: "
                  + jobIdText);
              // code different from super version.
              if (jobIdText.equals(this.dagJobId)) {
                jTextFieldJobStatus.setText(rowToAddVector.get(2).toString());
                jTextFieldSubmissionTime.setText(rowToAddVector.get(3)
                    .toString());
              }
              // END
            }
          } else {
            if (DagMultipleJobPanel.this.jobStatusHashMap
                .containsKey(jobIdText)) {
              DagMultipleJobPanel.this.jobStatusHashMap.remove(jobIdText);
            }
            if (index != -1) {
              if (jobTableModel.getValueAt(index, JOB_STATUS_COLUMN_INDEX)
                  .toString().equals(Utils.COLLECTING_STATE)) {
                jobTableModel.setValueAt(Utils.UNABLE_TO_GET_STATUS, index,
                    JOB_STATUS_COLUMN_INDEX);
              }
            } else {
              // code different from super version.
              if (jobIdText.equals(this.dagJobId)) {
                if (jobTableModel.getValueAt(index, JOB_STATUS_COLUMN_INDEX)
                    .toString().equals(Utils.COLLECTING_STATE)) {
                  jTextFieldJobStatus.setText(Utils.UNABLE_TO_GET_STATUS);
                }
              }
              // END
            }
            warningMsg += ((Exception) jobResult.getResult()).getMessage()
                + "\n";
          }
          jLabelTotalDisplayedJobs.setText(Integer.toString(jobTableModel
              .getRowCount()));
        } else {
          logger.debug("setJTableJobs() - Job result is null");
        }
      }
      isFirstTimeNodeName = false;
      //jobTableModel.sortBy(this, jTableJobs, sortingColumn, true);
      jobTableModel.sortBy(jTableJobs, sortingColumn, true);
      Date date = new Date();
      String timeText = date.toString(); // look timeText below
      jLabelLastUpdate.setText(timeText);
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    jLabelTotalDisplayedJobs.setText(Integer.toString(jobTableModel
        .getRowCount()));
    warningMsg = warningMsg.trim();
    if (!warningMsg.equals("")) {
      GraphicUtils.showOptionDialogMsg(DagMultipleJobPanel.this, warningMsg,
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE, null,
          null);
    }
  }

  protected void renewJPopupMenu() {
    jPopupMenuTable.add(jMenuItemRemove);
    jPopupMenuTable.add(jMenuItemClear);
    jPopupMenuTable.addSeparator();
    jPopupMenuTable.add(jMenuItemSelectAll);
    jPopupMenuTable.add(jMenuItemSelectNone);
    jPopupMenuTable.add(jMenuItemInvertSelection);
    jPopupMenuTable.addSeparator();
    jPopupMenuTable.add(jMenuItemDetails);
    jPopupMenuTable.add(jMenuItemLogInfo);
    jPopupMenuTable.add(jMenuItemUpdate);
    jPopupMenuTable.addSeparator();
    //jPopupMenuTable.add(jMenuItemJobCancel);
    jPopupMenuTable.add(jMenuItemJobOutput);
    jPopupMenuTable.addSeparator();
    jPopupMenuTable.add(jMenuItemInteractiveConsole);
    jPopupMenuTable.add(jMenuItemRetrieveCheckpointState);
    //jPopupMenuTable.addSeparator();
    //jPopupMenuTable.add(jMenuItemDagNodes);
    //jPopupMenuTable.add(jMenuItemDagMonitor);
    jPopupMenuTable.addSeparator();
    jPopupMenuTable.add(jMenuItemSortAddingOrder);
  }

  protected void createJPopupMenu() {
    ActionListener alst = null;
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
    jPopupMenuTable.add(jMenuItemRemove);
    jPopupMenuTable.add(jMenuItemClear);
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
    jPopupMenuTable.add(jMenuItemSelectAll);
    jPopupMenuTable.add(jMenuItemSelectNone);
    jPopupMenuTable.add(jMenuItemInvertSelection);
    jPopupMenuTable.addSeparator();
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonDetailsEvent(null);
      }
    };
    jMenuItemDetails.addActionListener(alst);
    jPopupMenuTable.add(jMenuItemDetails);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonLogInfoEvent(null);
      }
    };
    jMenuItemLogInfo.addActionListener(alst);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonUpdateEvent(null);
      }
    };
    jMenuItemUpdate.addActionListener(alst);
    jPopupMenuTable.add(jMenuItemDetails);
    jPopupMenuTable.add(jMenuItemLogInfo);
    jPopupMenuTable.add(jMenuItemUpdate);
    jPopupMenuTable.addSeparator();
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonGetOutputEvent(null);
      }
    };
    jMenuItemJobOutput.addActionListener(alst);
    jPopupMenuTable.add(jMenuItemJobOutput);
    jPopupMenuTable.addSeparator();
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuInteractiveConsole(e);
      }
    };
    jMenuItemInteractiveConsole.addActionListener(alst);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonRetrieveCheckpointStateEvent(null);
      }
    };
    jMenuItemRetrieveCheckpointState.addActionListener(alst);
    jPopupMenuTable.add(jMenuItemInteractiveConsole);
    jPopupMenuTable.add(jMenuItemRetrieveCheckpointState);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jobTableModel.sortBy(jTableJobs, Utils.NO_SORTING, true);
      }
    };
    jMenuItemSortAddingOrder.addActionListener(alst);
    jPopupMenuTable.addSeparator();
    jPopupMenuTable.add(jMenuItemSortAddingOrder);
  }
}