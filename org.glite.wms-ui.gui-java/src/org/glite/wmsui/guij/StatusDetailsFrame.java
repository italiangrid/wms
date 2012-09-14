/*
 * StatusDetailsFrame.java
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
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Rectangle;
import java.awt.SystemColor;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ComponentEvent;
import java.awt.event.WindowEvent;
import java.util.Date;
import java.util.HashMap;
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.ListSelectionModel;
import javax.swing.SwingConstants;
import javax.swing.border.EtchedBorder;
import javax.swing.border.LineBorder;
import javax.swing.border.TitledBorder;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.table.TableColumn;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Ad;
import org.glite.wmsui.apij.JobStatus;

/**
 * Implementation of the StatusDetailsFrame class.
 *
 *
 * @ingroup gui
 * @brief This class provides some constant values and utility methods.
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class StatusDetailsFrame extends JFrame {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int FRAME_WIDTH = 620;

  static final int FRAME_HEIGHT = 600;

  static final String VALUE = "Value";

  static final String ATTRIBUTE_NAME = "Attribute";

  static int[] primaryAttNames = { JobStatus.JOB_ID,
      JobStatus.STATE_ENTER_TIME, JobStatus.REASON, JobStatus.STATUS,
      JobStatus.JOBTYPE, JobStatus.EXIT_CODE, JobStatus.OWNER,
      JobStatus.NETWORK_SERVER, JobStatus.DESTINATION, JobStatus.CONDOR_JDL,
      JobStatus.JDL, JobStatus.MATCHED_JDL, JobStatus.RSL
  //,
  /* To remove even if not present in primary panel*/
  /*JobStatus.STATUS_CODE*/
  };

  static final int[] statusAttributeIndex = { JobStatus.ACL,
      JobStatus.CANCEL_REASON, JobStatus.CANCELLING, JobStatus.CE_NODE,
      JobStatus.CHILDREN, JobStatus.CHILDREN_HIST, JobStatus.CHILDREN_NUM,
      JobStatus.CHILDREN_STATES, JobStatus.CONDOR_ID, JobStatus.CONDOR_JDL,
      JobStatus.CPU_TIME, JobStatus.DESTINATION, JobStatus.DONE_CODE,
      JobStatus.EXIT_CODE, JobStatus.EXPECT_FROM, JobStatus.EXPECT_UPDATE,
      JobStatus.GLOBUS_ID, JobStatus.JDL, JobStatus.JOB_ID, JobStatus.JOBTYPE,
      JobStatus.LAST_UPDATE_TIME, JobStatus.LOCAL_ID, JobStatus.LOCATION,
      JobStatus.MATCHED_JDL, JobStatus.NETWORK_SERVER, JobStatus.OWNER,
      JobStatus.PARENT_JOB, JobStatus.REASON, JobStatus.RESUBMITTED,
      JobStatus.RSL, JobStatus.SEED, JobStatus.STATE_ENTER_TIME,
      JobStatus.STATE_ENTER_TIMES, JobStatus.SUBJOB_FAILED,
      JobStatus.USER_TAGS, /*JobStatus.USER_VALUES*/
      JobStatus.USER_TAGS
  };

  static final String[] statusAttributeDescr = { "Access Control List",
      "Cancellation Reason", "Cancellation in progress",
      "Execution Worker Node", "Subjobs Id list", "Subjobs States Summary",
      "Subjobs Number", "Subjobs States", "Condor-G Job Id",
      "Condor-G Job Description", "Consumed CPU Time",
      "Destination CE Identifier", "Done Code", "Job Exit Code",
      "Source of Missing Information", "Some info has not arrived yet",
      "Globus Job Id", "User Job Description", "Job Identifier", "Job Type",
      "Last Job Event Time", "LRMS Job Id", "Job Current Location",
      "Job Description after Matchmaking", "Network Server", "Job Owner",
      "Parent job", "Status Reason", "Resubmitted", "Job RSL",
      "Subjob Id Generation Seed", "Status Enter Time",
      "All States Enter Time", "Subjob Failed", "User Tags Names",
      "User Tags Values"
  };

  static final String[] jobStatusJobType = { "Job", "DAG"
  };

  static final String[] jobStateNames = { "undefined", "Submitted", "Waiting",
      "Ready", "Scheduled", "Running", "Done", "Cleared", "Aborted",
      "Cancelled", "Unknown", "Purged"
  };

  private HashMap statusAttributeDescrMap = new HashMap();

  private Vector primaryAttNamesVector = new Vector();

  JobStatus jobStatus;

  MultipleJobPanel multipleJobPanel;

  JPanel jPanelButton = new JPanel();

  JScrollPane jScrollPaneMain = new JScrollPane();

  JLabel jLabelJobId = new JLabel();

  JTextField jTextFieldJobId = new JTextField();

  JPanel jPanelMain = new JPanel();

  JPanel jPanelViewText = new JPanel();

  JButton jButtonUpdate = new JButton();

  JTextPane jTextPane = new JTextPane();

  JScrollPane jScrollPaneViewText = new JScrollPane();

  JLabel jLabelStatus = new JLabel();

  JLabel jLabelDestination = new JLabel();

  JLabel jLabelStatusReason = new JLabel();

  JLabel jLabelStateEnterTime = new JLabel();

  JButton jButtonCondorJDL = new JButton();

  JButton jButtonJDL = new JButton();

  JButton jButtonMatchedJDL = new JButton();

  JButton jButtonBack = new JButton();

  JTextField jTextFieldStatus = new JTextField();

  JTextField jTextFieldStateEnterTime = new JTextField();

  JTextField jTextFieldDestination = new JTextField();

  JTextPane jTextFieldStatusReason = new JTextPane();

  JPanel jPanelJob = new JPanel();

  JLabel jLabelLastUpdateTitle = new JLabel();

  JTextField jTextFieldLastUpdate = new JTextField();

  JButton jButtonClose = new JButton();

  JLabel jLabelNetworkServer = new JLabel();

  JTextField jTextFieldNetworkServer = new JTextField();

  JLabel jLabelExitCode = new JLabel();

  JTextField jTextFieldExitCode = new JTextField();

  JLabel jLabelJobType = new JLabel();

  JTextField jTextFieldJobType = new JTextField();

  JTextPane jTextFieldOwner = new JTextPane();

  JLabel jLabelOwner = new JLabel();

  JPanel jPanelAttributes = new JPanel();

  JScrollPane jScrollPaneAttributes = new JScrollPane();

  JTable jTableValue;

  JTable jTableHeader;

  Vector valueTableHeaderVector = new Vector();

  Vector headerTableHeaderVector = new Vector();

  JobTableModel valueTableJobTableModel;

  JobTableModel headerTableJobTableModel;

  JButton jButtonRSL = new JButton();

  JPanel jPanelButtonJobDescr = new JPanel();

  JScrollPane jScrollPaneStatusReason = new JScrollPane();

  JScrollPane jScrollPaneJobOwner = new JScrollPane();

  /**
   * Constructor.
   */
  public StatusDetailsFrame(Component component, JobStatus jobStatus) {
    setTitle("Job Monitor - Job Status Details");
    this.jobStatus = jobStatus;
    if (component instanceof MultipleJobPanel) {
      multipleJobPanel = (MultipleJobPanel) component;
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
    setJobStatus(jobStatus);
  }

  private void jbInit() throws Exception {
    isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG) ? true
        : false;
    Toolkit toolkit = getToolkit();
    Dimension screenSize = toolkit.getScreenSize();
    int width = (int) (screenSize.width * GraphicUtils.SCREEN_WIDTH_PROPORTION * GraphicUtils.SCREEN_WIDTH_INFO_DETAILS_PROPORTION);
    int height = (int) (screenSize.height * GraphicUtils.SCREEN_HEIGHT_PROPORTION);
    this.setSize(new Dimension(width, height));
    this.addComponentListener(new java.awt.event.ComponentListener() {
      public void componentResized(ComponentEvent e) {
        updateValueTableWidth();
        /*
         int xPosition = getBounds().x;
         int yPosition = getBounds().y;
         int height = getBounds().height;
         int width = getBounds().width;
         boolean changed = false;
         if (width > FRAME_WIDTH) {
         width = FRAME_WIDTH;
         changed = true;
         }
         if (height > FRAME_HEIGHT) {
         height = FRAME_HEIGHT;
         changed = true;
         }
         if (changed) {
         setBounds(new Rectangle(xPosition, yPosition, width, height));
         } else {
         repaint();
         }*/
      }

      public void componentMoved(ComponentEvent e) {
      }

      public void componentHidden(ComponentEvent e) {
      }

      public void componentShown(ComponentEvent e) {
      }
    });
    for (int i = 0; i < primaryAttNames.length; i++) {
      this.primaryAttNamesVector.add(new Integer(primaryAttNames[i]));
    }
    for (int i = 0; i < statusAttributeIndex.length; i++) {
      this.statusAttributeDescrMap.put(new Integer(statusAttributeIndex[i]),
          statusAttributeDescr[i]);
    }
    jTextFieldLastUpdate.setEditable(false);
    jTextFieldLastUpdate.setFocusable(false);
    jTextFieldLastUpdate.setFont(GraphicUtils.BOLD_FONT);
    jTextFieldLastUpdate.setBorder(BorderFactory.createLoweredBevelBorder());
    jTextFieldLastUpdate.setHorizontalAlignment(SwingConstants.RIGHT);
    jPanelButtonJobDescr.setBorder(new TitledBorder(new EtchedBorder(),
        " Job Description ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelButtonJobDescr.setBounds(new Rectangle(6, 468, 593, 64));
    ((FlowLayout) jPanelButtonJobDescr.getLayout()).setHgap(40);
    valueTableHeaderVector.add(VALUE);
    valueTableJobTableModel = new JobTableModel(valueTableHeaderVector, 0);
    jTableValue = new JTable(valueTableJobTableModel);
    jTableValue.getTableHeader().setReorderingAllowed(false);
    headerTableHeaderVector.add(ATTRIBUTE_NAME);
    headerTableJobTableModel = new JobTableModel(headerTableHeaderVector, 0);
    jTableHeader = new JTable(headerTableJobTableModel);
    jTableHeader.getTableHeader().setReorderingAllowed(false);
    jTableHeader.getTableHeader().setResizingAllowed(false);
    jTableHeader.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    jTableHeader.setBackground(GraphicUtils.LIGHT_YELLOW);
    //jTableHeader.setBackground(SystemColor.window);
    jTableHeader.setFont(GraphicUtils.BOLD_FONT);
    jTableValue.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
    jTableValue.getTableHeader().setResizingAllowed(false);
    jTableValue.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    JPanel utilityPanel = new JPanel();
    utilityPanel.setBorder(LineBorder.createGrayLineBorder());
    utilityPanel.setBackground(SystemColor.window);
    jScrollPaneAttributes
        .setCorner(JScrollPane.LOWER_LEFT_CORNER, utilityPanel);
    jScrollPaneAttributes.setRowHeaderView(jTableHeader);
    jScrollPaneAttributes.setCorner(JScrollPane.UPPER_LEFT_CORNER, jTableHeader
        .getTableHeader());
    //jScrollPaneAttributes.getRowHeader().setBackground(Color.white);
    jScrollPaneAttributes.getViewport().setBackground(Color.white);
    jTextFieldStatus.setBackground(Color.white);
    jTextFieldStatus.setBorder(BorderFactory.createEtchedBorder());
    jTextFieldStatus.setEditable(false);
    jTextFieldStateEnterTime.setBackground(Color.white);
    jTextFieldStateEnterTime.setBorder(BorderFactory.createEtchedBorder());
    jTextFieldStateEnterTime.setEditable(false);
    jTextFieldDestination.setBackground(Color.white);
    jTextFieldDestination.setBorder(BorderFactory.createEtchedBorder());
    jTextFieldDestination.setEditable(false);
    jTextFieldStatusReason.setBackground(Color.white);
    jTextFieldStatusReason.setBorder(null);
    jTextFieldStatusReason.setEditable(false);
    jLabelLastUpdateTitle.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelLastUpdateTitle.setText("Last Update");
    jButtonUpdate.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonUpdateEvent(e);
      }
    });
    jButtonClose.setText("Close");
    jButtonClose.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonCloseEvent(e);
      }
    });
    jTextFieldJobId.setBackground(Color.white);
    jTextFieldJobId.setBorder(BorderFactory.createEtchedBorder());
    jTextFieldJobId.setEditable(false);
    jLabelNetworkServer.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelNetworkServer.setText("Network Server");
    jTextFieldNetworkServer.setEditable(false);
    jTextFieldNetworkServer.setBorder(BorderFactory.createEtchedBorder());
    jTextFieldNetworkServer.setBackground(Color.white);
    jLabelStatusReason.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelDestination.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelStatus.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelJobId.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelStateEnterTime.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelExitCode.setText("Job Exit Code");
    jLabelExitCode.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldExitCode.setEditable(false);
    jTextFieldExitCode.setBackground(Color.white);
    jTextFieldExitCode.setBorder(BorderFactory.createEtchedBorder());
    jLabelJobType.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelJobType.setText("Type");
    jTextFieldJobType.setBackground(Color.white);
    jTextFieldJobType.setBorder(BorderFactory.createEtchedBorder());
    jTextFieldJobType.setEditable(false);
    jTextFieldOwner.setBackground(Color.white);
    //jTextFieldOwner.setBorder(BorderFactory.createEtchedBorder());
    jTextFieldOwner.setEditable(false);
    jTextFieldOwner.setText("");
    jTextFieldOwner.setBorder(null);
    jLabelOwner.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelOwner.setText("Job Owner");
    jLabelJobId.setText("Job Id");
    jButtonUpdate.setText("Update");
    jLabelStatus.setText("Status");
    jLabelDestination.setText("Destination");
    jLabelStatusReason.setText("Status Reason");
    jLabelStateEnterTime.setText("State Enter Time");
    jButtonCondorJDL.setText("Condor JDL ");
    jButtonCondorJDL.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonShowJDLAttributeEvent(JobStatus.CONDOR_JDL, e);
      }
    });
    jButtonJDL.setText("    JDL    ");
    jButtonJDL.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonShowJDLAttributeEvent(JobStatus.JDL, e);
      }
    });
    jButtonMatchedJDL.setText("Matched JDL");
    jButtonMatchedJDL.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonShowJDLAttributeEvent(JobStatus.MATCHED_JDL, e);
      }
    });
    jButtonRSL.setText("    RSL    ");
    jButtonRSL.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonShowJDLAttributeEvent(JobStatus.RSL, e);
      }
    });
    jButtonBack.setText("   Back   ");
    jButtonBack.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonBackEvent(e);
      }
    });
    jTextPane.setEditable(false);
    jScrollPaneViewText = new JScrollPane(jTextPane);
    jScrollPaneStatusReason.getViewport().add(jTextFieldStatusReason, null);
    jScrollPaneStatusReason.setBorder(BorderFactory.createEtchedBorder());
    jScrollPaneJobOwner.getViewport().add(jTextFieldOwner, null);
    jScrollPaneJobOwner.setBorder(BorderFactory.createEtchedBorder());
    jPanelViewText.setLayout(new BorderLayout());
    jPanelViewText.add(jScrollPaneViewText, BorderLayout.CENTER);
    jPanelViewText.add(jPanelButton, BorderLayout.SOUTH);
    jPanelButton.add(jButtonBack);
    jPanelButtonJobDescr.add(jButtonJDL, null);
    jPanelButtonJobDescr.add(jButtonCondorJDL, null);
    jPanelButtonJobDescr.add(jButtonMatchedJDL, null);
    jPanelButtonJobDescr.add(jButtonRSL, null);
    ListSelectionModel listSelectionModel = jTableHeader.getSelectionModel();
    listSelectionModel.addListSelectionListener(new ListSelectionListener() {
      public void valueChanged(ListSelectionEvent e) {
        if (e.getValueIsAdjusting()) {
          int selectedRow = jTableHeader.getSelectedRow();
          jTableValue.setRowSelectionInterval(selectedRow, selectedRow);
        }
      }
    });
    listSelectionModel = jTableValue.getSelectionModel();
    listSelectionModel.addListSelectionListener(new ListSelectionListener() {
      public void valueChanged(ListSelectionEvent e) {
        if (e.getValueIsAdjusting()) {
          int selectedRow = jTableValue.getSelectedRow();
          jTableHeader.setRowSelectionInterval(selectedRow, selectedRow);
        }
      }
    });
    // jPanelLastUpdate
    JPanel jPanelLastUpdate = new JPanel();
    ((FlowLayout) jPanelLastUpdate.getLayout()).setAlignment(FlowLayout.RIGHT);
    jPanelLastUpdate.add(jLabelLastUpdateTitle);
    jPanelLastUpdate.add(jTextFieldLastUpdate);
    // jPanelJobId
    JPanel jPanelJobId = new JPanel();
    jPanelJobId.setLayout(new BoxLayout(jPanelJobId, BoxLayout.X_AXIS));
    jPanelJobId.setBorder(GraphicUtils.SPACING_BORDER);
    jPanelJobId.add(Box.createGlue());
    jPanelJobId.add(jLabelJobId);
    jPanelJobId.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelJobId.add(jTextFieldJobId);
    GridBagLayout gbl = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.insets = new Insets(3, 3, 3, 3);
    // jPanelJob
    jPanelJob.setLayout(gbl);
    jPanelJob.setBorder(new TitledBorder(new EtchedBorder(), " Common Info ",
        0, 0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelJob.add(jLabelStatus, GraphicUtils.setGridBagConstraints(gbc, 0, 0,
        1, 1, 0.0, 0.0, GridBagConstraints.EAST, GridBagConstraints.NONE, null,
        0, 0));
    jPanelJob.add(jTextFieldStatus, GraphicUtils.setGridBagConstraints(gbc, 1,
        0, 1, 1, 0.4, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelJob.add(jLabelStateEnterTime, GraphicUtils.setGridBagConstraints(gbc,
        2, 0, 1, 1, 0.0, 0.0, GridBagConstraints.EAST, GridBagConstraints.NONE,
        null, 0, 0));
    jPanelJob.add(jTextFieldStateEnterTime, GraphicUtils.setGridBagConstraints(
        gbc, 3, 0, 2, 1, 0.6, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelJob.add(jLabelStatusReason, GraphicUtils.setGridBagConstraints(gbc,
        0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_END,
        GridBagConstraints.NONE, null, 0, 0));
    jPanelJob.add(jScrollPaneStatusReason, GraphicUtils.setGridBagConstraints(
        gbc, 1, 1, 4, 1, 1.0, 0.5, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, null, 0, 0));
    jPanelJob.add(jLabelJobType, GraphicUtils.setGridBagConstraints(gbc, 0, 2,
        1, 1, 0.0, 0.0, GridBagConstraints.EAST, GridBagConstraints.NONE, null,
        0, 0));
    jPanelJob.add(jTextFieldJobType, GraphicUtils.setGridBagConstraints(gbc, 1,
        2, 2, 1, 0.6, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelJob.add(jLabelExitCode, GraphicUtils.setGridBagConstraints(gbc, 3, 2,
        1, 1, 0.0, 0.0, GridBagConstraints.EAST, GridBagConstraints.NONE, null,
        0, 0));
    jPanelJob.add(jTextFieldExitCode, GraphicUtils.setGridBagConstraints(gbc,
        4, 2, 1, 1, 0.4, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelJob.add(jLabelOwner, GraphicUtils.setGridBagConstraints(gbc, 0, 3, 1,
        1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_END,
        GridBagConstraints.NONE, null, 0, 0));
    jPanelJob.add(jScrollPaneJobOwner, GraphicUtils.setGridBagConstraints(gbc,
        1, 3, 4, 1, 1.0, 0.5, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, null, 0, 0));
    jPanelJob.add(jLabelNetworkServer, GraphicUtils.setGridBagConstraints(gbc,
        0, 4, 1, 1, 0.0, 0.0, GridBagConstraints.EAST, GridBagConstraints.NONE,
        null, 0, 0));
    jPanelJob.add(jTextFieldNetworkServer, GraphicUtils.setGridBagConstraints(
        gbc, 1, 4, 4, 1, 1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelJob.add(jLabelDestination, GraphicUtils.setGridBagConstraints(gbc, 0,
        5, 1, 1, 0.0, 0.0, GridBagConstraints.EAST, GridBagConstraints.NONE,
        null, 0, 0));
    jPanelJob.add(jTextFieldDestination, GraphicUtils.setGridBagConstraints(
        gbc, 1, 5, 4, 1, 1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    // jPanelAttributes
    jPanelAttributes.setLayout(new BorderLayout());
    jPanelAttributes
        .setBorder(new TitledBorder(new EtchedBorder(), " Detailed Info ", 0,
            0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jScrollPaneAttributes.getViewport().add(jTableValue, null);
    jPanelAttributes.add(jScrollPaneAttributes, BorderLayout.CENTER);
    // jPanelButton
    JPanel jPanelButton2 = new JPanel();
    jPanelButton2.setLayout(new BoxLayout(jPanelButton2, BoxLayout.X_AXIS));
    jPanelButton2.setBorder(GraphicUtils.SPACING_BORDER);
    jPanelButton2.add(jButtonUpdate, null);
    jPanelButton2.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelButton2.add(Box.createGlue());
    jPanelButton2.add(jButtonClose, null);
    // jPanelMain
    jPanelMain.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelMain.add(jPanelLastUpdate, GraphicUtils.setGridBagConstraints(gbc, 0,
        0, 1, 1, 1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, new Insets(1, 1, 1, 1), 0, 0));
    jPanelMain.add(jPanelJobId, GraphicUtils.setGridBagConstraints(gbc, 0, 1,
        1, 1, 1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelMain.add(jPanelJob, GraphicUtils.setGridBagConstraints(gbc, 0, 2, 1,
        1, 1.0, 0.2, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, null, 0, 0));
    jPanelMain.add(jPanelAttributes, GraphicUtils.setGridBagConstraints(gbc, 0,
        3, 1, 1, 1.0, 0.8, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, null, 0, 0));
    jPanelMain.add(jPanelButtonJobDescr, GraphicUtils.setGridBagConstraints(
        gbc, 0, 4, 1, 1, 1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelMain.add(jPanelButton2, GraphicUtils.setGridBagConstraints(gbc, 0, 5,
        1, 1, 1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jScrollPaneMain.getViewport().add(jPanelMain);
    this.getContentPane().setLayout(new BorderLayout());
    this.getContentPane().add(jScrollPaneMain, BorderLayout.CENTER);
    jPanelMain.setPreferredSize(new Dimension(
        (int) (width - width * GraphicUtils.SCREEN_WIDTH_INFO_DETAILS_PREFERRED_PROPORTION),
        (int) (height - height * GraphicUtils.SCREEN_WIDTH_INFO_DETAILS_PREFERRED_PROPORTION)));
  }

  void showJPanelViewText(String text) {
    jTextPane.setText(text);
    //jScrollPaneMain.getViewport().remove(jPanelMain);
    //jScrollPaneMain.getViewport().add(jPanelViewText);
    this.getContentPane().remove(jScrollPaneMain);
    this.getContentPane().add(jPanelViewText);
    validate();
    repaint();
  }

  void jButtonBackEvent(ActionEvent e) {
    //jScrollPaneMain.getViewport().remove(jPanelViewText);
    //jScrollPaneMain.getViewport().add(jPanelMain);
    this.getContentPane().remove(jPanelViewText);
    this.getContentPane().add(jScrollPaneMain);
    validate();
    repaint();
  }

  void jButtonShowJDLAttributeEvent(int attribute, ActionEvent e) {
    String text = jobStatus.getValString(attribute);
    if ((text != null) && !text.equals("")) {
      Ad ad = new Ad();
      try {
        ad.fromString(text);
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        JOptionPane.showOptionDialog(StatusDetailsFrame.this,
            Utils.UNESPECTED_ERROR + ex.getMessage(), Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
      }
      showJPanelViewText(ad.toString(true, true));
    }
  }

  void setJobStatus(JobStatus jobStatus) {
    this.jobStatus = jobStatus; // Necessary if you need to display RSL, JDL or JSS_JDL.
    // (you need to update this.jobStatus).
    jTextFieldJobId.setText(jobStatus.getValString(JobStatus.JOB_ID));
    if (jobStatus.name().trim().equals(JobStatus.code[JobStatus.DONE])) {
      if (jobStatus.getValInt(JobStatus.DONE_CODE) == 1) {
        jTextFieldStatus.setText(jobStatus.name() + Utils.STATE_FAILED); // Status
      } else if (jobStatus.getValInt(JobStatus.DONE_CODE) == 2) {
        jTextFieldStatus.setText(jobStatus.name() + Utils.STATE_CANCELLED); // Status
      } else { // DONE_CODE = 0.
        int exitCode = jobStatus.getValInt(JobStatus.EXIT_CODE);
        if (exitCode != 0) {
          jTextFieldStatus.setText(jobStatus.name()
              + Utils.STATE_EXIT_CODE_NOT_ZERO);
        } else {
          jTextFieldStatus.setText(jobStatus.name());
        }
      }
    } else {
      jTextFieldStatus.setText(jobStatus.name());
    }
    jTextFieldStatusReason.setText(jobStatus.getValString(JobStatus.REASON));
    jTextFieldNetworkServer.setText(jobStatus
        .getValString(JobStatus.NETWORK_SERVER));
    int type = jobStatus.getValInt(JobStatus.JOBTYPE);
    if ((type >= 0) && (type <= jobStatusJobType.length)) {
      jTextFieldJobType.setText(jobStatusJobType[type]);
    } else {
      jTextFieldJobType.setText("");
    }
    jTextFieldDestination
        .setText(jobStatus.getValString(JobStatus.DESTINATION));
    jTextFieldOwner.setText(jobStatus.getValString(JobStatus.OWNER));
    jTextFieldStateEnterTime.setText(Utils.toTime(jobStatus
        .getValString(JobStatus.STATE_ENTER_TIME)));
    jTextFieldExitCode.setText(Integer.toString(jobStatus
        .getValInt(JobStatus.EXIT_CODE)));
    setTableStatusAttributes(jobStatus);
    String value = "";
    value = jobStatus.getValString(JobStatus.CONDOR_JDL);
    logger.debug("CONDOR_JDL attribute text: " + value);
    if ((value == null) || value.equals("")
        || !value.trim().substring(0, 1).equals("[")) {
      jButtonCondorJDL.setEnabled(false);
    } else {
      jButtonCondorJDL.setEnabled(true);
    }
    value = jobStatus.getValString(JobStatus.JDL);
    logger.debug("JDL attribute text: " + value);
    if ((value == null) || value.equals("")
        || !value.trim().substring(0, 1).equals("[")) {
      jButtonJDL.setEnabled(false);
    } else {
      jButtonJDL.setEnabled(true);
    }
    value = jobStatus.getValString(JobStatus.MATCHED_JDL);
    logger.debug("MATCHED_JDL attribute text: " + value);
    if ((value == null) || value.equals("")
        || !value.trim().substring(0, 1).equals("[")) {
      jButtonMatchedJDL.setEnabled(false);
    } else {
      jButtonMatchedJDL.setEnabled(true);
    }
    value = jobStatus.getValString(JobStatus.RSL);
    logger.debug("RSL attribute text: " + value);
    if ((value == null) || value.equals("")
        || !value.trim().substring(0, 1).equals("[")) {
      jButtonRSL.setEnabled(false);
    } else {
      jButtonRSL.setEnabled(true);
    }
    // If Jdl text panel is shown show Attributes panel.
    jButtonBackEvent(null);
  }

  void setTableStatusAttributes(JobStatus jobStatus) {
    headerTableJobTableModel.removeAllRows();
    valueTableJobTableModel.removeAllRows();
    Object value;
    Object attributeDescr;
    for (int i = 0; i < JobStatus.attNames.length; i++) {
      if (this.primaryAttNamesVector.contains(new Integer(i))) {
        continue;
      }
      value = jobStatus.get(i);
      if (value != null) {
        Vector rowToAddHeader = new Vector();
        Vector rowToAddValue = new Vector();
        switch (i) {
          case JobStatus.LAST_UPDATE_TIME:
            value = Utils.toTime(value.toString());
          break;
          case JobStatus.STATE_ENTER_TIMES:
            Vector stateTimeVector = (Vector) value;
            String currentTime;
            for (int j = 0; j < stateTimeVector.size(); j++) {
              currentTime = stateTimeVector.get(j).toString().trim();
              if (!currentTime.equals("0.0 s")) {
                rowToAddHeader.add("State Enter Time (" + jobStateNames[j]
                    + ")");
                rowToAddValue.add(Utils.toTime(currentTime));
                headerTableJobTableModel.addRow(rowToAddHeader);
                valueTableJobTableModel.addRow(rowToAddValue);
                rowToAddHeader = new Vector();
                rowToAddValue = new Vector();
              }
            }
            continue;
          case JobStatus.RESUBMITTED:
          case JobStatus.SUBJOB_FAILED:
          case JobStatus.EXPECT_UPDATE:
            switch (((Integer) value).intValue()) {
              case 0:
                value = "No";
              break;
              case 1:
                value = "Yes";
              break;
            }
          break;
          case JobStatus.CANCELLING:
            if ((((Integer) value).intValue() == 1)
                && (jobStatus.code() != JobStatus.CANCELLED)) {
              value = "Yes";
            } else {
              value = "No";
            }
          break;
          case JobStatus.DONE_CODE:
          case JobStatus.STATUS_CODE:
            continue;
        }
        attributeDescr = this.statusAttributeDescrMap.get(new Integer(i));
        if (attributeDescr != null) {
          rowToAddHeader.add(attributeDescr);
        } else {
          rowToAddHeader.add(JobStatus.attNames[i]);
        }
        rowToAddValue.add(value);
        headerTableJobTableModel.addRow(rowToAddHeader);
        valueTableJobTableModel.addRow(rowToAddValue);
      }
    }
    TableColumn firstColumn = jTableHeader.getColumn(ATTRIBUTE_NAME);
    int firstColumnWidth = headerTableJobTableModel.getColumnPreferredWidth(
        jTableHeader, firstColumn);
    firstColumn.setPreferredWidth(firstColumnWidth);
    jTableHeader.setPreferredScrollableViewportSize(new Dimension(firstColumn
        .getPreferredWidth()
        + jTableHeader.getColumnModel().getColumnMargin(), 0));
    jTableHeader.revalidate();
    TableColumn jTableValueColumn = jTableValue.getColumn(VALUE);
    int jTableValueColumnWidth = valueTableJobTableModel
        .getColumnPreferredWidth(jTableValue, jTableValueColumn);
    int jTableValueWidth = jScrollPaneAttributes.getSize().width
        - jTableHeader.getPreferredScrollableViewportSize().width;
    jTableValueColumnWidth = (jTableValueColumnWidth > jTableValueWidth) ? jTableValueColumnWidth
        : jTableValueWidth;
    jTableValueColumnWidth += jTableValue.getColumnModel().getColumnMargin();
    jTableValueColumn.setPreferredWidth(jTableValueColumnWidth);
    jTableValue.revalidate();
  }

  protected void updateValueTableWidth() {
    TableColumn jTableValueColumn = jTableValue.getColumn(VALUE);
    int jTableValueColumnWidth = valueTableJobTableModel
        .getColumnPreferredWidth(jTableValue, jTableValueColumn);
    int jTableValueWidth = jScrollPaneAttributes.getSize().width
        - jTableHeader.getPreferredScrollableViewportSize().width;
    jTableValueColumnWidth = (jTableValueColumnWidth > jTableValueWidth) ? jTableValueColumnWidth
        : jTableValueWidth;
    jTableValueColumnWidth += jTableValue.getColumnModel().getColumnMargin();
    jTableValueColumn.setPreferredWidth(jTableValueColumnWidth);
    jTableValue.revalidate();
  }

  protected void processWindowEvent(WindowEvent e) {
    super.processWindowEvent(e);
    if (e.getID() == WindowEvent.WINDOW_CLOSING) {
      multipleJobPanel.setIsSingleJobDialogShown(false);
    }
  }

  void jButtonUpdateEvent(ActionEvent e) {
    final SwingWorker worker = new SwingWorker() {
      public Object construct() {
        // Standard code
        String jobIdShown = getJobIdShown();
        JobStatus jobStatus = multipleJobPanel.getJobStatus(jobIdShown);
        StatusDetailsFrame.this.jobStatus = jobStatus;
        setJobStatus(jobStatus);
        Date date = new Date();
        setJLabelLastUpdate(date.toString());
        //multipleJobPanel.restartUpdateThread();
        // END Standard code
        return "";
      }
    };
    worker.start();
  }

  void setJLabelLastUpdate(String timeText) {
    jTextFieldLastUpdate.setText(timeText);
  }

  void jButtonCloseEvent(ActionEvent e) {
    multipleJobPanel.setIsSingleJobDialogShown(false);
    this.dispose();
  }

  String getJobIdShown() {
    return jTextFieldJobId.getText().trim();
  }
}