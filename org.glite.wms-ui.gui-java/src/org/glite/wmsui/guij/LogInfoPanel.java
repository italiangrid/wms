/*
 * LogInfoPanel.java
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
import java.awt.SystemColor;
import java.awt.event.ActionEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.Date;
import java.util.HashMap;
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
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
import org.glite.wmsui.apij.Job;
import org.glite.wmsui.apij.JobId;
import org.glite.wmsui.apij.Result;

/**
 * Implementation of the LogInfoPanel class.
 *
 *
 * @ingroup gui
 * @brief This class provides some constant values and utility methods.
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class LogInfoPanel extends JPanel {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int EVENT_COLUMN_INDEX = 0;

  static final String EVENT_HEADER = "Event";

  static final int SOURCE_COLUMN_INDEX = 1;

  static final String SOURCE_HEADER = "Source";

  static final int TIME_COLUMN_INDEX = 2;

  static final String TIME_HEADER = "Time";

  static final String ATTRIBUTE_NAME = "Attribute";

  static final String VALUE = "Value";

  static int[] primaryAttNames = { org.glite.wmsui.apij.Event.CLASSAD,
      org.glite.wmsui.apij.Event.DESCR, org.glite.wmsui.apij.Event.JDL,
      org.glite.wmsui.apij.Event.JOB, org.glite.wmsui.apij.Event.JOBID
  };

  static final int[] eventAttributeIndex = {
      org.glite.wmsui.apij.Event.CLASSAD, org.glite.wmsui.apij.Event.DESCR,
      org.glite.wmsui.apij.Event.DEST_HOST,
      org.glite.wmsui.apij.Event.DEST_INSTANCE,
      org.glite.wmsui.apij.Event.DEST_ID,
      org.glite.wmsui.apij.Event.DEST_JOBID,
      org.glite.wmsui.apij.Event.DESTINATION,
      org.glite.wmsui.apij.Event.EXIT_CODE, org.glite.wmsui.apij.Event.FROM,
      org.glite.wmsui.apij.Event.FROM_HOST,
      org.glite.wmsui.apij.Event.FROM_INSTANCE,
      org.glite.wmsui.apij.Event.HELPER_NAME,
      org.glite.wmsui.apij.Event.HELPER_PARAMS,
      org.glite.wmsui.apij.Event.HOST, org.glite.wmsui.apij.Event.JDL,
      org.glite.wmsui.apij.Event.JOB, org.glite.wmsui.apij.Event.JOBID,
      org.glite.wmsui.apij.Event.LEVEL, org.glite.wmsui.apij.Event.LOCAL_JOBID,
      org.glite.wmsui.apij.Event.NAME, org.glite.wmsui.apij.Event.NODE,
      org.glite.wmsui.apij.Event.NS, org.glite.wmsui.apij.Event.NSUBJOBS,
      org.glite.wmsui.apij.Event.PARENT, org.glite.wmsui.apij.Event.PRIORITY,
      org.glite.wmsui.apij.Event.QUEUE, org.glite.wmsui.apij.Event.REASON,
      org.glite.wmsui.apij.Event.RESULT, org.glite.wmsui.apij.Event.RETVAL,
      org.glite.wmsui.apij.Event.SEED, org.glite.wmsui.apij.Event.SEQCODE,
      org.glite.wmsui.apij.Event.SOURCE,
      org.glite.wmsui.apij.Event.SRC_INSTANCE,
      org.glite.wmsui.apij.Event.SRC_ROLE,
      org.glite.wmsui.apij.Event.STATUS_CODE,
      org.glite.wmsui.apij.Event.SVC_HOST, org.glite.wmsui.apij.Event.SVC_NAME,
      org.glite.wmsui.apij.Event.SVC_PORT, org.glite.wmsui.apij.Event.TAG,
      org.glite.wmsui.apij.Event.TIMESTAMP, org.glite.wmsui.apij.Event.USER,
      org.glite.wmsui.apij.Event.VALUE
  };

  static final String[] eventAttributeDescr = { "Checkpoint State",
      "Current Job Description", "Destination Host", "Destination Instance",
      "Destination CE Id", "Dest Internal Job Id", "Job Transfer Destination",
      "Process Exit Code", "Accepted/Refused From", "Sending Component Host",
      "Sending Component Instance", "Helper Name", "Helper Call Params",
      "Source Host Name", " User Job Description", "Receiver Job Description",
      "Job Identifier", "Logging Level", "Local Job Identifier",
      "User Tag Name", "Execution Worker Node", "Network Server",
      "Number of Subjobs", "Parent Job Identifier", "Message Priority",
      "Destination Queue", "Status Reason", "Transfer Result/Result Code",
      "Helper Call Return", "Subjob Id Generation Seed", "Sequence Code",
      "Event Source", "Source Instance", "Source Role", "Status Code",
      "Listener Host", "Listener Name", "Listener Port",
      "Chkpt Tag/Resubmission Tag", "Event Generation Time", "User",
      "UserTag Value"
  };

  HashMap eventAttributeDescrMap = new HashMap();

  Vector primaryAttNamesVector = new Vector();

  JobTableModel jobTableModel;

  JobTableModel valueTableJobTableModel;

  JobTableModel headerTableJobTableModel;

  LogInfoFrame logInfoJDialog;

  JobId jobId;

  JLabel jLabelJobId = new JLabel();

  JTextField jTextFieldJobId = new JTextField();

  JPanel jPanelJobEvents = new JPanel();

  JPanel jPanelEventLogInfo = new JPanel();

  JLabel jLabelTotalEvents = new JLabel();

  JTextField jTextFieldTotalEvents = new JTextField();

  JScrollPane jScrollPaneTableEvents = new JScrollPane();

  JScrollPane jScrollPaneEventLogInfo = new JScrollPane();

  JTextPane jTextPaneEvent = new JTextPane();

  JTable jTableJobEvents;

  JTable jTableValue;

  JTable jTableHeader;

  Vector vectorHeader = new Vector();

  Vector eventVector = new Vector();

  JTable jTablePaneEvent;

  JScrollPane jScrollPaneAttribute = new JScrollPane();

  Vector valueTableHeaderVector = new Vector();

  Vector headerTableHeaderVector = new Vector();

  JButton jButtonClose = new JButton();

  JButton jButtonUpdate = new JButton();

  JButton jButtonChkptState = new JButton();

  JButton jButtonJobDesc = new JButton();

  JButton jButtonJDL = new JButton();

  JButton jButtonJob = new JButton();

  JTextField jTextFieldLastUpdate = new JTextField();

  JLabel jLabelLastUpdateTitle = new JLabel();

  JPanel jPanelButtonJDL = new JPanel();

  public LogInfoPanel() {
    try {
      jbInit();
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
  }

  public LogInfoPanel(Component component, Vector eventVector) {
    this.eventVector = eventVector;
    if (component instanceof LogInfoFrame) {
      logInfoJDialog = (LogInfoFrame) component;
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
    for (int i = 0; i < primaryAttNames.length; i++) {
      primaryAttNamesVector.add(new Integer(primaryAttNames[i]));
    }
    for (int i = 0; i < eventAttributeIndex.length; i++) {
      eventAttributeDescrMap.put(new Integer(eventAttributeIndex[i]),
          eventAttributeDescr[i]);
    }
    jLabelJobId.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelJobId.setText("Job Id");
    jTextFieldJobId.setEditable(false);
    jTextFieldJobId.setBackground(Color.white);
    jTextFieldJobId.setBorder(BorderFactory.createEtchedBorder());
    jLabelTotalEvents.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelTotalEvents.setText("Total Events");
    jTextFieldTotalEvents.setEditable(false);
    jTextFieldTotalEvents.setFocusable(false);
    jTextFieldTotalEvents.setPreferredSize(new Dimension(50, 18));
    //jTextFieldTotalEvents.setBackground(Utils.LIGTH_YELLOW);
    jTextFieldTotalEvents.setFont(GraphicUtils.BOLD_FONT);
    jTextFieldTotalEvents.setBorder(BorderFactory.createLoweredBevelBorder());
    jTextFieldTotalEvents.setHorizontalAlignment(SwingConstants.RIGHT);
    jScrollPaneTableEvents.getViewport().setBackground(Color.white);
    jButtonClose.setText("Close");
    jButtonClose.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonCloseEvent(e);
      }
    });
    jButtonUpdate.setText("Update");
    jButtonUpdate.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonUpdateEvent(e);
      }
    });
    jButtonChkptState.setText("Chkpt State");
    jButtonChkptState.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonShowJDLAttriuteEvent(org.glite.wmsui.apij.Event.CLASSAD, e);
      }
    });
    jButtonJobDesc.setText(" Job Descr ");
    jButtonJobDesc.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonShowJDLAttriuteEvent(org.glite.wmsui.apij.Event.DESCR, e);
      }
    });
    jButtonJDL.setText("    JDL    ");
    jButtonJDL.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonShowJDLAttriuteEvent(org.glite.wmsui.apij.Event.JDL, e);
      }
    });
    jButtonJob.setText("  Rec JDL  ");
    jButtonJob.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonShowJDLAttriuteEvent(org.glite.wmsui.apij.Event.JOB, e);
      }
    });
    jLabelLastUpdateTitle.setText("Last Update");
    jTextFieldLastUpdate.setEditable(false);
    jTextFieldLastUpdate.setFocusable(false);
    jTextFieldLastUpdate.setFont(GraphicUtils.BOLD_FONT);
    jTextFieldLastUpdate.setBorder(BorderFactory.createLoweredBevelBorder());
    jTextFieldLastUpdate.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelLastUpdateTitle.setHorizontalAlignment(SwingConstants.RIGHT);
    valueTableHeaderVector.add(VALUE);
    valueTableJobTableModel = new JobTableModel(valueTableHeaderVector, 0);
    jTableValue = new JTable(valueTableJobTableModel);
    jTableValue.getTableHeader().setReorderingAllowed(false);
    headerTableHeaderVector.add(ATTRIBUTE_NAME);
    headerTableJobTableModel = new JobTableModel(headerTableHeaderVector, 0);
    jTableHeader = new JTable(headerTableJobTableModel);
    jScrollPaneAttribute = new JScrollPane(jTableValue);
    jTableHeader.getTableHeader().setReorderingAllowed(false);
    jTableHeader.getTableHeader().setResizingAllowed(false);
    jTableHeader.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    jTableHeader.setBackground(GraphicUtils.LIGHT_YELLOW);
    //jTableHeader.setBackground(Color.lightGray);
    jTableHeader.setFont(GraphicUtils.BOLD_FONT);
    jTableValue.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
    jTableValue.getTableHeader().setResizingAllowed(false);
    jTableValue.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    jScrollPaneAttribute.setRowHeaderView(jTableHeader);
    jScrollPaneAttribute.setCorner(JScrollPane.UPPER_LEFT_CORNER, jTableHeader
        .getTableHeader());
    JPanel utilityPanel = new JPanel();
    utilityPanel.setBorder(LineBorder.createGrayLineBorder());
    utilityPanel.setBackground(SystemColor.window);
    jScrollPaneAttribute.setCorner(JScrollPane.LOWER_LEFT_CORNER, utilityPanel);
    jScrollPaneAttribute.getRowHeader().setBackground(Color.white);
    jScrollPaneAttribute.getViewport().setBackground(Color.white);
    jTextPaneEvent.setEditable(false);
    jScrollPaneEventLogInfo.getViewport().add(jTextPaneEvent, null);
    vectorHeader.add(EVENT_HEADER);
    vectorHeader.add(SOURCE_HEADER);
    vectorHeader.add(TIME_HEADER);
    jobTableModel = new JobTableModel(vectorHeader, 0);
    jTableJobEvents = new JTable(jobTableModel);
    jTableJobEvents.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    addTableEvents(eventVector);
    jScrollPaneTableEvents.getViewport().add(jTableJobEvents, null);
    // Add mouse listener to the grid
    jTableJobEvents.addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
      }

      public void mouseClicked(MouseEvent e) {
      }

      public void mouseReleased(MouseEvent e) {
        showEvent(e);
      }
    });
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
    TableColumn col = jTableJobEvents.getColumnModel().getColumn(
        TIME_COLUMN_INDEX);
    col.setPreferredWidth(120);
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
    // jPanelEventLogInfo
    jPanelEventLogInfo.setLayout(new BorderLayout());
    jPanelEventLogInfo.setBorder(new TitledBorder(new EtchedBorder(),
        " Selected Event Logging Info ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelEventLogInfo.add(jScrollPaneAttribute, BorderLayout.CENTER);
    // jPanelJobEvents
    jPanelJobEvents.setLayout(new BorderLayout());
    jPanelJobEvents.setBorder(new TitledBorder(new EtchedBorder(),
        " Job Event Table ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    JPanel jPanelTotalEvents = new JPanel();
    ((FlowLayout) jPanelTotalEvents.getLayout()).setAlignment(FlowLayout.LEFT);
    jPanelTotalEvents.add(jLabelTotalEvents);
    jPanelTotalEvents.add(jTextFieldTotalEvents);
    jPanelJobEvents.add(jPanelTotalEvents, BorderLayout.NORTH);
    jPanelJobEvents.add(jScrollPaneTableEvents, BorderLayout.CENTER);
    // jPanelButtonJDL
    jPanelButtonJDL.setBorder(new TitledBorder(new EtchedBorder(),
        " Job Description ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    ((FlowLayout) jPanelButtonJDL.getLayout()).setAlignment(FlowLayout.CENTER);
    ((FlowLayout) jPanelButtonJDL.getLayout()).setHgap(40);
    jPanelButtonJDL.add(jButtonJobDesc);
    jPanelButtonJDL.add(jButtonChkptState);
    jPanelButtonJDL.add(jButtonJDL);
    jPanelButtonJDL.add(jButtonJob);
    // jPanelButton
    JPanel jPanelButton = new JPanel();
    jPanelButton.setLayout(new BoxLayout(jPanelButton, BoxLayout.X_AXIS));
    jPanelButton.setBorder(GraphicUtils.SPACING_BORDER);
    jPanelButton.add(jButtonUpdate, null);
    jPanelButton.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelButton.add(Box.createGlue());
    jPanelButton.add(jButtonClose, null);
    GridBagLayout gbl = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.insets = new Insets(3, 3, 3, 3);
    // this
    this.setLayout(gbl);
    this.add(jPanelLastUpdate, GraphicUtils.setGridBagConstraints(gbc, 0, 0, 1,
        1, 1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, new Insets(1, 1, 1, 1), 0, 0));
    this.add(jPanelJobId, GraphicUtils.setGridBagConstraints(gbc, 0, 1, 1, 1,
        1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    this.add(jPanelJobEvents, GraphicUtils.setGridBagConstraints(gbc, 0, 2, 1,
        1, 1.0, 0.5, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, null, 0, 0));
    this.add(jPanelEventLogInfo, GraphicUtils.setGridBagConstraints(gbc, 0, 3,
        1, 1, 1.0, 0.5, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, null, 0, 0));
    this.add(jPanelButtonJDL, GraphicUtils.setGridBagConstraints(gbc, 0, 4, 1,
        1, 1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    this.add(jPanelButton, GraphicUtils.setGridBagConstraints(gbc, 0, 5, 1, 1,
        1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    setTableEventAttributes((org.glite.wmsui.apij.Event) eventVector.get(0));
  }

  // fill the text pane with the selected event's details
  // after a mouse selection
  void showEvent(MouseEvent e) {
    int selectedRow = jTableJobEvents.getSelectedRow();
    if (selectedRow != -1) {
      setTableEventAttributes(selectedRow);
    }
  }

  void setTableEventAttributes(int selectedRow) {
    org.glite.wmsui.apij.Event event = (org.glite.wmsui.apij.Event) eventVector
        .get(selectedRow);
    setTableEventAttributes(event);
  }

  void setTableEventAttributes(org.glite.wmsui.apij.Event event) {
    headerTableJobTableModel.removeAllRows();
    valueTableJobTableModel.removeAllRows();
    Object value;
    Object attributeDescr;
    for (int i = 0; i < org.glite.wmsui.apij.Event.attrNames.length; i++) {
      if (i == org.glite.wmsui.apij.Event.EVENT) {
        continue;
      }
      if (i == org.glite.wmsui.apij.Event.EVENT_CODE) {
        continue;
      }
      if (primaryAttNamesVector.contains(new Integer(i))) {
        continue;
      }
      value = event.get(i);
      if (value != null) {
        switch (i) {
          case org.glite.wmsui.apij.Event.TIMESTAMP:
            value = Utils.toTime(value.toString());
          break;
          default:
          break;
        }
        Vector rowToAddHeader = new Vector();
        Vector rowToAddValue = new Vector();
        attributeDescr = eventAttributeDescrMap.get(new Integer(i));
        if (attributeDescr != null) {
          rowToAddHeader.add(attributeDescr);
        } else {
          rowToAddHeader.add(org.glite.wmsui.apij.Event.attrNames[i]);
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
    int jTableValueWidth = jScrollPaneAttribute.getSize().width
        - jTableHeader.getPreferredScrollableViewportSize().width;
    jTableValueColumnWidth = (jTableValueColumnWidth > jTableValueWidth) ? jTableValueColumnWidth
        : jTableValueWidth;
    jTableValueColumnWidth += jTableValue.getColumnModel().getColumnMargin();
    jTableValueColumn.setPreferredWidth(jTableValueColumnWidth);
    jTableValue.revalidate();
    String valueTxt = "";
    valueTxt = event.getValString(org.glite.wmsui.apij.Event.CLASSAD);
    if ((valueTxt == null) || valueTxt.equals("")
        || !valueTxt.trim().substring(0, 1).equals("[")) {
      jButtonChkptState.setEnabled(false);
    } else {
      jButtonChkptState.setEnabled(true);
    }
    valueTxt = event.getValString(org.glite.wmsui.apij.Event.DESCR);
    if ((valueTxt == null) || valueTxt.equals("")
        || !valueTxt.trim().substring(0, 1).equals("[")) {
      jButtonJobDesc.setEnabled(false);
    } else {
      jButtonJobDesc.setEnabled(true);
    }
    valueTxt = event.getValString(org.glite.wmsui.apij.Event.JDL);
    if ((valueTxt == null) || valueTxt.equals("")
        || !valueTxt.trim().substring(0, 1).equals("[")) {
      jButtonJDL.setEnabled(false);
    } else {
      jButtonJDL.setEnabled(true);
    }
    valueTxt = event.getValString(org.glite.wmsui.apij.Event.JOB);
    if ((valueTxt == null) || valueTxt.equals("")
        || !valueTxt.trim().substring(0, 1).equals("[")) {
      jButtonJob.setEnabled(false);
    } else {
      jButtonJob.setEnabled(true);
    }
  }

  void updateValueTableWidth() {
    TableColumn jTableValueColumn = jTableValue.getColumn(VALUE);
    int jTableValueColumnWidth = valueTableJobTableModel
        .getColumnPreferredWidth(jTableValue, jTableValueColumn);
    int jTableValueWidth = jScrollPaneAttribute.getSize().width
        - jTableHeader.getPreferredScrollableViewportSize().width;
    jTableValueColumnWidth = (jTableValueColumnWidth > jTableValueWidth) ? jTableValueColumnWidth
        : jTableValueWidth;
    jTableValueColumnWidth += jTableValue.getColumnModel().getColumnMargin();
    jTableValueColumn.setPreferredWidth(jTableValueColumnWidth);
    jTableValue.revalidate();
  }

  // set all events cointained in the input vector as the elements of the event table
  // and display in the text area the logging info of the first event
  void setTableEvents(Vector eventVector) {
    this.eventVector = eventVector;
    jobTableModel.removeAllRows();
    addTableEvents(eventVector);
  }

  // add all events cointained in the input vector to the event table
  // and display in the text area the logging info of the first event
  // in the vector
  void addTableEvents(Vector eventVector) {
    org.glite.wmsui.apij.Event event;
    Vector rowToAddVector;
    String logInfoText = "";
    String timeStamp = "";
    for (int i = 0; i < eventVector.size(); i++) {
      rowToAddVector = new Vector(); //TBD added
      event = (org.glite.wmsui.apij.Event) eventVector.get(i);
      //this.eventVector.add(event);
      if (i == 0) {
        logInfoText = event.toString();
        jTextFieldJobId.setText(event
            .getValString(org.glite.wmsui.apij.Event.JOBID));
      }
      rowToAddVector.add(event.name().toString());
      rowToAddVector.add(event.getValString(org.glite.wmsui.apij.Event.SOURCE));
      timeStamp = event.getValString(org.glite.wmsui.apij.Event.TIMESTAMP);
      if (timeStamp != null) {
        rowToAddVector.add(Utils.toTime(timeStamp));
      } else {
        rowToAddVector.add(Utils.UNAVAILABLE_SUBMISSION_TIME);
      }
      jobTableModel.addRow(rowToAddVector);
    }
    jTextFieldTotalEvents.setText(Integer.toString(eventVector.size()));
    jTextPaneEvent.setText(logInfoText);
    jTableJobEvents.setRowSelectionInterval(0, 0);
  }

  void jButtonCloseEvent(ActionEvent e) {
    logInfoJDialog.close();
  }

  void jButtonShowJDLAttriuteEvent(int attribute, ActionEvent e) {
    int selectedRow = jTableJobEvents.getSelectedRow();
    org.glite.wmsui.apij.Event event = (org.glite.wmsui.apij.Event) eventVector
        .get(selectedRow);
    String text = event.getValString(attribute);
    if ((text != null) && !text.equals("")) {
      Ad ad = new Ad();
      try {
        ad.fromString(text);
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        JOptionPane.showOptionDialog(LogInfoPanel.this, Utils.UNESPECTED_ERROR
            + ex.getMessage(), Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
      logInfoJDialog.showTextPane(ad.toString(true, true));
    }
  }

  void jButtonUpdateEvent(ActionEvent e) {
    final SwingWorker worker = new SwingWorker() {
      public Object construct() {
        // Standard code
        String jobIdTxt = jTextFieldJobId.getText().trim();
        JobId jobId = null;
        Job job = null;
        Result result = null;
        try {
          jobId = new JobId(jobIdTxt);
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
          return "";
        }
        try {
          job = new Job(jobId);
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
          return "";
        }
        try {
          result = job.getLogInfo();
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
        }
        String logInfoText = "";
        Vector eventVector = new Vector();
        if (result.getCode() != Result.SUCCESS) {
          return "";
        } else {
          eventVector = (Vector) result.getResult();
          setTableEvents(eventVector);
        }
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
}