/*
 * QueryPanel.java
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
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.FocusEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.Calendar;
import java.util.Iterator;
import java.util.Vector;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
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
import javax.swing.plaf.basic.BasicArrowButton;
import javax.swing.table.TableColumn;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Ad;
import org.glite.jdl.JobAd;
import org.glite.wmsui.apij.JobStatus;

/**
 * Implementation of the QueryPanel class.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class QueryPanel extends JDialog {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  JLabel jLabelQueryName = new JLabel();

  JTextField jTextFieldQueryName = new JTextField();

  JLabel jLabelAvailableLB = new JLabel();

  JComboBox jComboBoxAvailableLB = new JComboBox();

  JLabel jLabelTagName = new JLabel();

  JComboBox jComboBoxTagName = new JComboBox();

  JLabel jLabelTagValue = new JLabel();

  JTextField jTextFieldTagValue = new JTextField();

  JPanel jPanelQuery = new JPanel();

  JScrollPane jScrollPaneTags = new JScrollPane();

  JTable jTableTags;

  JobTableModel jobTableModel;

  static final int TAG_NAME_COLUMN_INDEX = 0;

  static final int TAG_VALUE_COLUMN_INDEX = 1;

  static final String ALL_LB_SERVERS = "- All Available LB Servers -";

  Vector vectorHeader = new Vector();

  JButton jButtonAdd = new JButton();

  JButton jButtonReplace = new JButton();

  JButton jButtonRemove = new JButton();

  JButton jButtonCancel = new JButton();

  JButton jButtonOk = new JButton();

  JobMonitor jobMonitorJFrame;

  private Ad userQueryAd;

  private String queryName;

  private boolean isEditing;

  QueryPreferencesPanel queryPreferencesPanel;

  JCheckBox jCheckBoxOwned = new JCheckBox();

  JTextField jTextFieldYear = new JTextField();

  BasicArrowButton downYear = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton downDay = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton upMonth = new BasicArrowButton(BasicArrowButton.NORTH);

  BasicArrowButton upYear = new BasicArrowButton(BasicArrowButton.NORTH);

  BasicArrowButton downMonth = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton upDay = new BasicArrowButton(BasicArrowButton.NORTH);

  JTextField jTextFieldMonth = new JTextField();

  JTextField jTextFieldDay = new JTextField();

  Calendar calendar = Calendar.getInstance();

  JTextField jTextFieldYear1 = new JTextField();

  BasicArrowButton downYear1 = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton downDay1 = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton upMonth1 = new BasicArrowButton(BasicArrowButton.NORTH);

  BasicArrowButton upYear1 = new BasicArrowButton(BasicArrowButton.NORTH);

  BasicArrowButton downMonth1 = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton upDay1 = new BasicArrowButton(BasicArrowButton.NORTH);

  JTextField jTextFieldMonth1 = new JTextField();

  JTextField jTextFieldDay1 = new JTextField();

  JCheckBox jCheckBoxFrom = new JCheckBox();

  JCheckBox jCheckBoxTo = new JCheckBox();

  BasicArrowButton downHourFrom = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton upMinuteFrom = new BasicArrowButton(BasicArrowButton.NORTH);

  BasicArrowButton downMinuteFrom = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton upHourFrom = new BasicArrowButton(BasicArrowButton.NORTH);

  JTextField jTextFieldMinuteFrom = new JTextField();

  JTextField jTextFieldHourFrom = new JTextField();

  BasicArrowButton downHourTo = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton upMinuteTo = new BasicArrowButton(BasicArrowButton.NORTH);

  BasicArrowButton downMinuteTo = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton upHourTo = new BasicArrowButton(BasicArrowButton.NORTH);

  JTextField jTextFieldMinuteTo = new JTextField();

  JTextField jTextFieldHourTo = new JTextField();

  JLabel jLabel1 = new JLabel();

  JLabel jLabel2 = new JLabel();

  JLabel jLabel3 = new JLabel();

  JLabel jLabel4 = new JLabel();

  BasicArrowButton downSecondFrom = new BasicArrowButton(BasicArrowButton.SOUTH);

  JTextField jTextFieldSecondFrom = new JTextField();

  BasicArrowButton upSecondFrom = new BasicArrowButton(BasicArrowButton.NORTH);

  BasicArrowButton downSecondTo = new BasicArrowButton(BasicArrowButton.SOUTH);

  JTextField jTextFieldSecondTo = new JTextField();

  BasicArrowButton upSecondTo = new BasicArrowButton(BasicArrowButton.NORTH);

  JPanel jPanelButton = new JPanel();

  JCheckBox jCheckBoxSubmitted = new JCheckBox(
      JobStatus.code[JobStatus.SUBMITTED]);

  JCheckBox jCheckBoxWaiting = new JCheckBox(JobStatus.code[JobStatus.WAITING]);

  JCheckBox jCheckBoxReady = new JCheckBox(JobStatus.code[JobStatus.READY]);

  JCheckBox jCheckBoxScheduled = new JCheckBox(
      JobStatus.code[JobStatus.SCHEDULED]);

  JCheckBox jCheckBoxRunning = new JCheckBox(JobStatus.code[JobStatus.RUNNING]);

  JCheckBox jCheckBoxDone = new JCheckBox(JobStatus.code[JobStatus.DONE]);

  JCheckBox jCheckBoxCleared = new JCheckBox(JobStatus.code[JobStatus.CLEARED]);

  JCheckBox jCheckBoxAborted = new JCheckBox(JobStatus.code[JobStatus.ABORTED]);

  JCheckBox jCheckBoxCancelled = new JCheckBox(
      JobStatus.code[JobStatus.CANCELLED]);

  JCheckBox jCheckBoxPurged = new JCheckBox(JobStatus.code[JobStatus.PURGED]);

  /**
   * Constructor.
   */
  public QueryPanel(QueryPreferencesPanel queryPreferencesPanel,
      JobMonitor jobMonitorJFrame, Ad userQueryAd, boolean isEditing) {
    super(queryPreferencesPanel.jobMonitorPreferences);
    this.jobMonitorJFrame = jobMonitorJFrame;
    this.queryPreferencesPanel = queryPreferencesPanel;
    this.userQueryAd = userQueryAd;
    this.isEditing = isEditing;
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit();
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
  }

  /**
   * Constructor.
   */
  public QueryPanel(QueryPreferencesPanel queryPreferencesPanel,
      JobMonitor jobMonitorJFrame, Ad userQueryAd) {
    this(queryPreferencesPanel, jobMonitorJFrame, userQueryAd, false);
  }

  /**
   * Constructor.
   */
  public QueryPanel(QueryPreferencesPanel queryPreferencesPanel,
      JobMonitor jobMonitorJFrame) {
    this(queryPreferencesPanel, jobMonitorJFrame, null, false);
  }

  /**
   * Constructor.
   */
  public QueryPanel() {
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
    setFromEnabled(false);
    setToEnabled(false);
    vectorHeader.addElement("Tag Name");
    vectorHeader.addElement("Tag Value");
    jobTableModel = new JobTableModel(vectorHeader, 0);
    jTableTags = new JTable(jobTableModel);
    jTableTags.getTableHeader().setReorderingAllowed(false);
    GUIListCellTooltipRenderer cellRenderer = new GUIListCellTooltipRenderer();
    jComboBoxAvailableLB.setRenderer(cellRenderer);
    jComboBoxTagName.setRenderer(cellRenderer);
    TableColumn col = jTableTags.getColumnModel().getColumn(
        TAG_NAME_COLUMN_INDEX);
    col.setCellRenderer(new GUITableTooltipCellRenderer());
    col = jTableTags.getColumnModel().getColumn(TAG_VALUE_COLUMN_INDEX);
    col.setCellRenderer(new GUITableTooltipCellRenderer());
    col.setPreferredWidth(170);
    this.setModal(true);
    this.setResizable(false);
    this.setTitle("Job Monitor - Preferences - Query Definition");
    this.setSize(new Dimension(530, 400));
    jScrollPaneTags.getViewport().setBackground(Color.white);
    jScrollPaneTags.setBounds(new Rectangle(10, 233, 405, 90));
    jButtonAdd.setBounds(new Rectangle(429, 233, 85, 25));
    jButtonAdd.setText("Add");
    jButtonReplace.setBounds(new Rectangle(429, 266, 85, 25));
    jButtonReplace.setAlignmentY((float) 0.5);
    jButtonReplace.setText("Replace");
    jButtonRemove.setBounds(new Rectangle(429, 298, 85, 25));
    jButtonRemove.setText("Remove");
    jButtonCancel.setText("Cancel");
    jButtonCancel.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonCancelEvent(e);
      }
    });
    jButtonOk.setText("  Ok  ");
    jButtonOk.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonOkEvent(e);
      }
    });
    jCheckBoxOwned.setText("Owned jobs only");
    jCheckBoxOwned.setBounds(new Rectangle(337, 52, 142, 21));
    jCheckBoxOwned.setSelected(true);
    jTextFieldYear.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldYear.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldYear, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.YEAR)), 1970, 9999);
      }
    });
    jTextFieldYear.setBounds(new Rectangle(291, 82, 41, 25));
    jTextFieldYear.setText(Integer.toString(calendar.get(Calendar.YEAR)));
    downYear.setBounds(new Rectangle(332, 94, 16, 13));
    downDay.setBounds(new Rectangle(234, 94, 16, 13));
    upMonth.setBounds(new Rectangle(274, 81, 16, 14));
    upYear.setBounds(new Rectangle(332, 81, 16, 14));
    downMonth.setBounds(new Rectangle(274, 94, 16, 13));
    upDay.setBounds(new Rectangle(234, 81, 16, 14));
    jTextFieldMonth.setBounds(new Rectangle(251, 82, 23, 25));
    jTextFieldMonth.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldMonth, Utils.INTEGER,
            Integer.toString(calendar.get(Calendar.MONTH) + 1), 1, 12);
      }
    });
    jTextFieldMonth.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldMonth.setText(Integer.toString(calendar.get(Calendar.MONTH) + 1));
    jTextFieldDay.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldDay.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldDay, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.DAY_OF_MONTH)), 1, 31);
      }
    });
    jTextFieldDay.setBounds(new Rectangle(211, 82, 23, 25));
    jTextFieldDay
        .setText(Integer.toString(calendar.get(Calendar.DAY_OF_MONTH)));
    jTextFieldDay1.setText(Integer
        .toString(calendar.get(Calendar.DAY_OF_MONTH)));
    jTextFieldDay1.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldDay1.setBounds(new Rectangle(211, 117, 23, 25));
    jTextFieldDay1.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldDay1, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.DAY_OF_MONTH)), 1, 31);
      }
    });
    jTextFieldYear1.setBounds(new Rectangle(291, 117, 41, 25));
    jTextFieldYear1.setText(Integer.toString(calendar.get(Calendar.YEAR)));
    jTextFieldYear1.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldYear1.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldYear1, Utils.INTEGER,
            Integer.toString(calendar.get(Calendar.YEAR)), 1970, 9999);
      }
    });
    downYear1.setBounds(new Rectangle(332, 129, 16, 13));
    downDay1.setBounds(new Rectangle(234, 129, 16, 13));
    upMonth1.setBounds(new Rectangle(274, 116, 16, 14));
    upYear1.setBounds(new Rectangle(332, 116, 16, 14));
    downMonth1.setBounds(new Rectangle(274, 129, 16, 13));
    upDay1.setBounds(new Rectangle(234, 116, 16, 14));
    jTextFieldMonth1.setBounds(new Rectangle(251, 117, 23, 25));
    jTextFieldMonth1
        .setText(Integer.toString(calendar.get(Calendar.MONTH) + 1));
    jTextFieldMonth1.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldMonth1, Utils.INTEGER,
            Integer.toString(calendar.get(Calendar.MONTH) + 1), 1, 12);
      }
    });
    jTextFieldMonth1.setHorizontalAlignment(SwingConstants.RIGHT);
    jCheckBoxFrom.setText("From:");
    jCheckBoxFrom.setBounds(new Rectangle(17, 84, 61, 21));
    jCheckBoxFrom.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jCheckBoxFromEvent(e);
      }
    });
    jCheckBoxTo.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jCheckBoxToEvent(e);
      }
    });
    jCheckBoxTo.setText("To:");
    jCheckBoxTo.setBounds(new Rectangle(17, 119, 61, 21));
    downYear.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldYear, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.YEAR)), 1970, 9999);
      }
    });
    downDay.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldDay, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.DAY_OF_MONTH)), 1, 31);
      }
    });
    upMonth.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldMonth, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.MONTH) + 1), 1, 12);
      }
    });
    upYear.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldYear, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.YEAR)), 1970, 9999);
      }
    });
    downMonth.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldMonth, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.MONTH) + 1), 1, 12);
      }
    });
    upDay.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldDay, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.DAY_OF_MONTH)), 1, 31);
      }
    });
    downYear1.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldYear1, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.YEAR)), 1970, 9999);
      }
    });
    downDay1.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldDay1, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.DAY_OF_MONTH)), 1, 31);
      }
    });
    upMonth1.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldMonth1, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.MONTH) + 1), 1, 12);
      }
    });
    upYear1.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldYear1, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.YEAR)), 1970, 9999);
      }
    });
    downMonth1.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldMonth1, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.MONTH) + 1), 1, 12);
      }
    });
    upDay1.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldDay1, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.DAY_OF_MONTH)), 1, 31);
      }
    });
    jTextFieldTagValue.setText("");
    jTextFieldTagValue.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldTagValue);
      }
    });
    jTextFieldTagValue.setBounds(new Rectangle(267, 203, 246, 21));
    jPanelQuery.setBounds(new Rectangle(2, 4, 487, 297));
    jPanelQuery.setBorder(new TitledBorder(new EtchedBorder(),
        " Query Definition ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelQuery.setLayout(null);
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
    jButtonReplace.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonReplaceEvent(e);
      }
    });
    downHourFrom.setBounds(new Rectangle(414, 94, 16, 13));
    downHourFrom.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldHourFrom, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.HOUR_OF_DAY)), 0, 23);
      }
    });
    upMinuteFrom.setBounds(new Rectangle(454, 81, 16, 14));
    upMinuteFrom.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldMinuteFrom, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.MINUTE)), 0, 59);
      }
    });
    downMinuteFrom.setBounds(new Rectangle(454, 94, 16, 13));
    downMinuteFrom.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldMinuteFrom, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.MINUTE)), 0, 59);
      }
    });
    upHourFrom.setBounds(new Rectangle(414, 81, 16, 14));
    upHourFrom.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldHourFrom, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.HOUR_OF_DAY)), 0, 23);
      }
    });
    downHourTo.setBounds(new Rectangle(414, 129, 16, 13));
    downHourTo.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldHourTo, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.HOUR_OF_DAY)), 0, 23);
      }
    });
    upMinuteTo.setBounds(new Rectangle(454, 116, 16, 14));
    upMinuteTo.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldMinuteTo, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.MINUTE)), 0, 59);
      }
    });
    downMinuteTo.setBounds(new Rectangle(454, 129, 16, 13));
    downMinuteTo.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldMinuteTo, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.MINUTE)), 0, 59);
      }
    });
    upHourTo.setBounds(new Rectangle(414, 116, 16, 14));
    upHourTo.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldHourTo, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.HOUR_OF_DAY)), 0, 23);
      }
    });
    jTextFieldHourFrom.setText(Integer.toString(calendar.get(Calendar.HOUR)));
    jTextFieldHourFrom.setBounds(new Rectangle(391, 82, 23, 25));
    jTextFieldHourFrom.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldHourFrom, Utils.INTEGER,
            Integer.toString(calendar.get(Calendar.HOUR_OF_DAY)), 0, 59);
      }
    });
    jTextFieldMinuteFrom.setBounds(new Rectangle(431, 82, 23, 25));
    jTextFieldMinuteFrom.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldMinuteFrom, Utils.INTEGER,
            Integer.toString(calendar.get(Calendar.MINUTE)), 0, 59);
      }
    });
    jTextFieldHourTo.setText(Integer.toString(calendar
        .get(Calendar.HOUR_OF_DAY)));
    jTextFieldHourTo.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldHourTo.setBounds(new Rectangle(391, 117, 23, 25));
    jTextFieldHourTo.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldHourTo, Utils.INTEGER,
            Integer.toString(calendar.get(Calendar.HOUR_OF_DAY)), 0, 23);
      }
    });
    jTextFieldMinuteTo.setBounds(new Rectangle(431, 117, 23, 25));
    jTextFieldMinuteTo.setText(Integer.toString(calendar.get(Calendar.MINUTE)));
    jTextFieldMinuteTo.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldMinuteTo.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldMinuteTo, Utils.INTEGER,
            Integer.toString(calendar.get(Calendar.MINUTE)), 0, 59);
      }
    });
    jTextFieldMinuteFrom.setText(Integer
        .toString(calendar.get(Calendar.MINUTE)));
    jTextFieldMinuteFrom.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldMinuteFrom, Utils.INTEGER,
            Integer.toString(calendar.get(Calendar.MINUTE)), 0, 59);
      }
    });
    jTextFieldMinuteFrom.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldHourFrom.setText(Integer.toString(calendar
        .get(Calendar.HOUR_OF_DAY)));
    jTextFieldHourFrom.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldHourFrom, Utils.INTEGER,
            Integer.toString(calendar.get(Calendar.HOUR_OF_DAY)), 0, 23);
      }
    });
    jTextFieldHourFrom.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabel1.setText("Date");
    jLabel1.setBounds(new Rectangle(93, 82, 40, 21));
    jLabel2.setText("Time");
    jLabel2.setBounds(new Rectangle(297, 82, 40, 21));
    jLabel1.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabel1.setText("Date (dd/mm/yyyy)");
    jLabel1.setBounds(new Rectangle(79, 82, 126, 24));
    jLabel2.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabel2.setText("Date (dd/mm/yyyy)");
    jLabel2.setBounds(new Rectangle(79, 117, 126, 24));
    jLabel3.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabel3.setText("Time");
    jLabel3.setBounds(new Rectangle(351, 82, 37, 24));
    jLabel4.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabel4.setText("Time");
    jLabel4.setBounds(new Rectangle(351, 117, 37, 24));
    downSecondFrom.setBounds(new Rectangle(494, 94, 16, 13));
    downSecondFrom.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldSecondFrom, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.SECOND)), 0, 59);
      }
    });
    jTextFieldSecondFrom.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldSecondFrom.setText(Integer
        .toString(calendar.get(Calendar.SECOND)));
    jTextFieldSecondFrom.setBounds(new Rectangle(471, 82, 23, 25));
    jTextFieldSecondFrom.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldSecondFrom, Utils.INTEGER,
            Integer.toString(calendar.get(Calendar.SECOND)), 0, 59);
      }
    });
    upSecondFrom.setBounds(new Rectangle(494, 81, 16, 14));
    upSecondFrom.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldSecondFrom, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.SECOND)), 0, 59);
      }
    });
    downSecondTo.setBounds(new Rectangle(494, 129, 16, 13));
    downSecondTo.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldSecondTo, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.SECOND)), 0, 59);
      }
    });
    jTextFieldSecondTo.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldSecondTo.setText(Integer.toString(calendar.get(Calendar.SECOND)));
    jTextFieldSecondTo.setBounds(new Rectangle(471, 117, 23, 25));
    jTextFieldSecondTo.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldSecondTo, Utils.INTEGER,
            Integer.toString(calendar.get(Calendar.SECOND)), 0, 59);
      }
    });
    upSecondTo.setBounds(new Rectangle(494, 116, 16, 14));
    upSecondTo.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldSecondTo, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.SECOND)), 0, 59);
      }
    });
    jCheckBoxSubmitted.setBounds(new Rectangle(17, 149, 95, 21));
    jCheckBoxWaiting.setBounds(new Rectangle(118, 149, 95, 21));
    jCheckBoxReady.setBounds(new Rectangle(218, 149, 95, 21));
    jCheckBoxScheduled.setBounds(new Rectangle(319, 149, 95, 21));
    jCheckBoxDone.setBounds(new Rectangle(419, 149, 95, 21));
    jCheckBoxRunning.setBounds(new Rectangle(17, 170, 95, 21));
    jCheckBoxCleared.setBounds(new Rectangle(118, 170, 95, 21));
    jCheckBoxAborted.setBounds(new Rectangle(218, 170, 95, 21));
    jCheckBoxCancelled.setBounds(new Rectangle(319, 170, 95, 21));
    jCheckBoxPurged.setBounds(new Rectangle(419, 170, 95, 21));
    jPanelQuery.add(jLabelAvailableLB, null);
    jPanelQuery.add(jComboBoxAvailableLB, null);
    jPanelQuery.add(jCheckBoxOwned, null);
    jPanelQuery.add(jLabelQueryName, null);
    jPanelQuery.add(jTextFieldQueryName, null);
    jPanelQuery.add(jLabel2, null);
    jPanelQuery.add(jLabel1, null);
    jPanelQuery.add(jButtonRemove, null);
    jPanelQuery.add(jScrollPaneTags, null);
    jPanelQuery.add(jButtonReplace, null);
    jPanelQuery.add(jCheckBoxWaiting, null);
    jPanelQuery.add(jCheckBoxReady, null);
    jPanelQuery.add(jCheckBoxCleared, null);
    jPanelQuery.add(jCheckBoxAborted, null);
    jPanelQuery.add(jCheckBoxScheduled, null);
    jPanelQuery.add(jCheckBoxCancelled, null);
    jPanelQuery.add(jCheckBoxPurged, null);
    jPanelQuery.add(jCheckBoxRunning, null);
    jPanelQuery.add(jCheckBoxFrom, null);
    jPanelQuery.add(jCheckBoxTo, null);
    jPanelQuery.add(jCheckBoxSubmitted, null);
    jPanelQuery.add(jCheckBoxDone, null);
    jPanelQuery.add(jButtonAdd, null);
    jPanelQuery.add(downSecondTo, null);
    jPanelQuery.add(jLabel3, null);
    jPanelQuery.add(jTextFieldHourFrom, null);
    jPanelQuery.add(downHourFrom, null);
    jPanelQuery.add(upHourFrom, null);
    jPanelQuery.add(jTextFieldMinuteFrom, null);
    jPanelQuery.add(downMinuteFrom, null);
    jPanelQuery.add(upMinuteFrom, null);
    jPanelQuery.add(jTextFieldSecondFrom, null);
    jPanelQuery.add(downSecondFrom, null);
    jPanelQuery.add(upSecondFrom, null);
    jPanelQuery.add(upSecondTo, null);
    jPanelQuery.add(jTextFieldSecondTo, null);
    jPanelQuery.add(downMinuteTo, null);
    jPanelQuery.add(upMinuteTo, null);
    jPanelQuery.add(jTextFieldMinuteTo, null);
    jPanelQuery.add(downHourTo, null);
    jPanelQuery.add(upHourTo, null);
    jPanelQuery.add(jTextFieldHourTo, null);
    jPanelQuery.add(jLabel4, null);
    jPanelQuery.add(jTextFieldYear1, null);
    jPanelQuery.add(jTextFieldDay1, null);
    jPanelQuery.add(downDay1, null);
    jPanelQuery.add(upDay1, null);
    jPanelQuery.add(jTextFieldMonth1, null);
    jPanelQuery.add(downMonth1, null);
    jPanelQuery.add(upMonth1, null);
    jPanelQuery.add(downYear1, null);
    jPanelQuery.add(upYear1, null);
    jPanelQuery.add(downYear, null);
    jPanelQuery.add(upYear, null);
    jPanelQuery.add(jTextFieldYear, null);
    jPanelQuery.add(upMonth, null);
    jPanelQuery.add(downMonth, null);
    jPanelQuery.add(jTextFieldMonth, null);
    jPanelQuery.add(downDay, null);
    jPanelQuery.add(upDay, null);
    jPanelQuery.add(jTextFieldDay, null);
    jPanelQuery.add(jTextFieldTagValue, null);
    jPanelQuery.add(jLabelTagName, null);
    jPanelQuery.add(jComboBoxTagName, null);
    jPanelQuery.add(jLabelTagValue, null);
    jScrollPaneTags.getViewport().add(jTableTags, null);
    jPanelQuery.add(jLabel2, null);
    jPanelQuery.add(jLabel1, null);
    jPanelButton.setLayout(new BoxLayout(jPanelButton, BoxLayout.X_AXIS));
    jPanelButton.setBorder(GraphicUtils.SPACING_BORDER);
    jPanelButton.add(jButtonCancel);
    jPanelButton.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelButton.add(Box.createGlue());
    jPanelButton.add(jButtonOk);
    this.getContentPane().setLayout(new BorderLayout());
    this.getContentPane().add(jPanelQuery, BorderLayout.CENTER);
    this.getContentPane().add(jPanelButton, BorderLayout.SOUTH);
    jLabelQueryName.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelQueryName.setText("Query Name");
    jLabelQueryName.setBounds(new Rectangle(8, 24, 76, 21));
    jTextFieldQueryName.setText("");
    jTextFieldQueryName.setBounds(new Rectangle(87, 24, 109, 21));
    jLabelAvailableLB.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelAvailableLB.setHorizontalTextPosition(SwingConstants.TRAILING);
    jLabelAvailableLB.setText("LB Server");
    jLabelAvailableLB.setBounds(new Rectangle(18, 52, 66, 21));
    jComboBoxAvailableLB.setBounds(new Rectangle(87, 52, 237, 21));
    jLabelTagName.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelTagName.setText("Tag Name");
    jLabelTagName.setBounds(new Rectangle(12, 203, 66, 21));
    jComboBoxTagName.setBounds(new Rectangle(87, 203, 109, 21));
    jComboBoxTagName.setEditable(true);
    jLabelTagValue.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelTagValue.setText("Tag Value");
    jLabelTagValue.setBounds(new Rectangle(198, 203, 63, 21));
    jTableTags.addMouseListener(new MouseAdapter() {
      public void mouseClicked(MouseEvent me) {
        if (me.getClickCount() == 2) {
          Point point = me.getPoint();
          int row = jTableTags.rowAtPoint(point);
          int column = jTableTags.columnAtPoint(point);
          jComboBoxTagName.setSelectedItem(jobTableModel.getValueAt(row,
              TAG_NAME_COLUMN_INDEX).toString().trim());
          jTextFieldTagValue.setText(jobTableModel.getValueAt(row,
              TAG_VALUE_COLUMN_INDEX).toString().trim());
        }
      }
    });
    Vector availableLB = jobMonitorJFrame.getLBMenuItems();
    for (int i = 0; i < availableLB.size(); i++) {
      jComboBoxAvailableLB.addItem(availableLB.get(i));
    }
    jComboBoxAvailableLB.insertItemAt(ALL_LB_SERVERS, 0);
    jComboBoxAvailableLB.setSelectedIndex(0);
    if (userQueryAd != null) {
      setQuery(userQueryAd);
    }
    if (isEditing) {
      jTextFieldQueryName.setEditable(false);
      jTextFieldQueryName.setBackground(Color.white);
      try {
        String lbAddress = userQueryAd.getStringValue(Utils.LB_ADDRESS).get(0)
            .toString();
        if (!lbAddress.equals(ALL_LB_SERVERS)) {
          if (!jobMonitorJFrame.getLBMenuItems().contains(lbAddress)) {
            jComboBoxAvailableLB.insertItemAt(lbAddress, 1);
            jComboBoxAvailableLB.setSelectedIndex(1);
          }
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(QueryPanel.this,
            "Unable to Edit selected query", Utils.INFORMATION_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
            null, null);
        this.dispose();
        return;
      }
    } else {
      setDefaultIncludeStateSelection();
    }
    //jComboBoxTagName.addItem("Prova1");
    //jComboBoxTagName.addItem("Prova2");
  }

  void jButtonRemoveEvent(ActionEvent e) {
    int[] selectedRow = jTableTags.getSelectedRows();
    int selectedRowCount = selectedRow.length;
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
        jTableTags.setRowSelectionInterval(selectableRow, selectableRow);
      }
    } else {
      jComboBoxTagName.grabFocus();
      JOptionPane.showOptionDialog(QueryPanel.this, Utils.SELECT_AN_ITEM,
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
    }
  }

  void jButtonReplaceEvent(ActionEvent e) {
    if (jTableTags.getSelectedRowCount() == 0) {
      JOptionPane.showOptionDialog(QueryPanel.this,
          "Please first select a table row to replace",
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
      return;
    } else if (jTableTags.getSelectedRowCount() != 1) {
      JOptionPane.showOptionDialog(QueryPanel.this,
          "Please select a single table row to replace",
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
      return;
    }
    int selectedRow = jTableTags.getSelectedRow();
    String selectedTagName = jobTableModel.getValueAt(selectedRow,
        TAG_NAME_COLUMN_INDEX).toString().trim();
    String selectedTagValue = jobTableModel.getValueAt(selectedRow,
        TAG_VALUE_COLUMN_INDEX).toString().trim();
    String tagName = jComboBoxTagName.getSelectedItem().toString();
    if (!tagName.equals("")) {
      if (!selectedTagName.equals(tagName)
          && jobTableModel.isElementPresentInColumn(tagName,
              TAG_NAME_COLUMN_INDEX)) {
        jComboBoxTagName.grabFocus();
        JOptionPane.showOptionDialog(QueryPanel.this,
            "Inserted Tag Name is already present", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        return;
      }
      String tagValue = jTextFieldTagValue.getText().trim();
      if (!tagValue.equals("")) {
        jobTableModel.setValueAt(tagName, selectedRow, TAG_NAME_COLUMN_INDEX);
        jobTableModel.setValueAt(tagValue, selectedRow, TAG_VALUE_COLUMN_INDEX);
        jComboBoxTagName.grabFocus();
      } else {
        jTextFieldTagValue.grabFocus();
        JOptionPane.showOptionDialog(QueryPanel.this,
            "Tag Value field cannot be blank", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        jTextFieldTagValue.setText("");
      }
    } else {
      jComboBoxTagName.grabFocus();
      JOptionPane.showOptionDialog(QueryPanel.this,
          "Tag Name field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
    }
  }

  void jButtonAddEvent(ActionEvent ae) {
    String tagName = jComboBoxTagName.getEditor().getItem().toString().trim();
    String tagValue = jTextFieldTagValue.getText().trim();
    if ((!tagName.equals("")) && (tagValue.equals(""))) {
      jTextFieldTagValue.grabFocus();
      JOptionPane.showOptionDialog(QueryPanel.this,
          "Tag Value field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      jTextFieldTagValue.setText("");
    } else if ((tagName.equals("")) && (!tagValue.equals(""))) {
      jComboBoxTagName.grabFocus();
      JOptionPane.showOptionDialog(QueryPanel.this,
          "Tag Name field cannot be blank", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      if (jComboBoxTagName.getItemCount() != 0) {
        jComboBoxTagName.showPopup();
      }
    } else if ((!tagName.equals("")) && (!tagValue.equals(""))) {
      if (!jobTableModel.isElementPresentInColumn(tagName,
          TAG_NAME_COLUMN_INDEX)) {
        // Check for tagName format, no spaces, ecc.
        JobAd jobAdCheck = new JobAd();
        try {
          jobAdCheck.setAttribute(tagName, tagValue);
        } catch (Exception e) {
          jComboBoxTagName.grabFocus();
          JOptionPane.showOptionDialog(QueryPanel.this,
              "Cannot Add Tag, please check Name and Value format",
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
          jComboBoxTagName.getEditor().selectAll();
          return;
        }
        Vector rowElement = new Vector();
        rowElement.addElement(tagName);
        rowElement.addElement(tagValue);
        jobTableModel.addRow(rowElement);
        jComboBoxTagName.grabFocus();
      } else {
        jComboBoxTagName.grabFocus();
        JOptionPane.showOptionDialog(QueryPanel.this,
            "Inserted Tag Name is already present", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        jComboBoxTagName.getEditor().selectAll();
        if (tagValue.equals("")) {
          jTextFieldTagValue.setText("");
        }
      }
    } else {
      jTextFieldTagValue.setText("");
      jComboBoxTagName.grabFocus();
    }
  }

  void jButtonCancelEvent(ActionEvent ae) {
    this.dispose();
  }

  void jButtonOkEvent(ActionEvent ae) {
    String queryName = jTextFieldQueryName.getText().trim();
    if (!isEditing) {
      if (queryName.equals("")) {
        jTextFieldQueryName.grabFocus();
        JOptionPane.showOptionDialog(QueryPanel.this,
            "Query Name is a mandatory field", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        jTextFieldQueryName.setText("");
        return;
      }
      if (queryPreferencesPanel.jobTableModel.isElementPresentInColumnCi(
          queryName, QueryPreferencesPanel.QUERY_NAME_COLUMN_INDEX)) {
        jTextFieldQueryName.grabFocus();
        JOptionPane.showOptionDialog(QueryPanel.this, "Query '" + queryName
            + "' is already defined", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        jTextFieldQueryName.selectAll();
        return;
      }
    }
    if ((jTableTags.getRowCount() == 0) && !jCheckBoxFrom.isSelected()
        && !jCheckBoxTo.isSelected()) {
      jComboBoxTagName.grabFocus();
      JOptionPane.showOptionDialog(QueryPanel.this,
          "Please provide at least a Tag " + "or a From/To Date-Time",
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return;
    }
    Calendar fromCalendar = Calendar.getInstance();
    Calendar toCalendar = Calendar.getInstance();
    if (jCheckBoxFrom.isSelected()) {
      int fromYear = 1;
      int fromMouth = 1;
      int fromDay = 1970;
      int fromHour = 0;
      int fromMinute = 0;
      int fromSecond = 0;
      try {
        fromYear = Integer.parseInt(jTextFieldYear.getText(), 10);
        fromMouth = Integer.parseInt(jTextFieldMonth.getText(), 10);
        fromDay = Integer.parseInt(jTextFieldDay.getText(), 10);
        fromHour = Integer.parseInt(jTextFieldHourFrom.getText(), 10);
        fromMinute = Integer.parseInt(jTextFieldMinuteFrom.getText(), 10);
        fromSecond = Integer.parseInt(jTextFieldSecondFrom.getText(), 10);
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
      fromCalendar.set(fromYear, fromMouth - 1, fromDay, fromHour, fromMinute,
          fromSecond);
      logger.debug("fromCalendar.getTimeInMillis(): "
          + fromCalendar.getTimeInMillis());
      Calendar calendar = Calendar.getInstance();
      logger.debug("Calendar.getInstance().getTimeInMillis(): "
          + calendar.getTimeInMillis());
      if (fromCalendar.getTimeInMillis() > calendar.getTimeInMillis()) {
        JOptionPane.showOptionDialog(QueryPanel.this,
            "From Date-Time follows current Date-Time", Utils.WARNING_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null,
            null, null);
      }
    }
    if (jCheckBoxTo.isSelected()) {
      int toYear = 1;
      int toMouth = 1;
      int toDay = 1970;
      int toHour = 0;
      int toMinute = 0;
      int toSecond = 0;
      try {
        toYear = Integer.parseInt(jTextFieldYear1.getText(), 10);
        toMouth = Integer.parseInt(jTextFieldMonth1.getText(), 10);
        toDay = Integer.parseInt(jTextFieldDay1.getText(), 10);
        toHour = Integer.parseInt(jTextFieldHourTo.getText(), 10);
        toMinute = Integer.parseInt(jTextFieldMinuteTo.getText(), 10);
        toSecond = Integer.parseInt(jTextFieldSecondTo.getText(), 10);
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
      toCalendar.set(toYear, toMouth - 1, toDay, toHour, toMinute, toSecond);
      logger.debug("toCalendar.getTimeInMillis(): "
          + toCalendar.getTimeInMillis());
      if (jCheckBoxFrom.isSelected()
          && (toCalendar.getTimeInMillis() < fromCalendar.getTimeInMillis())) {
        JOptionPane.showOptionDialog(QueryPanel.this,
            "To Date-Time precedes From Date-Time", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        return;
      }
    }
    Ad userTagsAd = new Ad();
    for (int i = 0; i < jTableTags.getRowCount(); i++) {
      try {
        userTagsAd.setAttribute(jobTableModel.getValueAt(i,
            TAG_NAME_COLUMN_INDEX).toString(), jobTableModel.getValueAt(i,
            TAG_VALUE_COLUMN_INDEX).toString());
      } catch (Exception e) {
        JOptionPane.showOptionDialog(QueryPanel.this, "Unable to Add query",
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        return;
      }
    }
    String lbAddress = jComboBoxAvailableLB.getSelectedItem().toString();
    boolean allLBFlag = false;
    if (lbAddress.equals(ALL_LB_SERVERS)) {
      lbAddress = ALL_LB_SERVERS;
      allLBFlag = true;
    }
    Ad userQueryAd = new Ad();
    try {
      userQueryAd.setAttribute(Utils.QUERY_NAME, queryName);
      userQueryAd.setAttribute(Utils.ALL_LB_FLAG, allLBFlag);
      userQueryAd.setAttribute(Utils.LB_ADDRESS, lbAddress);
      userQueryAd.setAttribute(Utils.OWNED_JOBS_ONLY_FLAG, jCheckBoxOwned
          .isSelected());
      if (jCheckBoxFrom.isSelected()) {
        userQueryAd.setAttribute(Utils.FROM_DATE, new Long(fromCalendar
            .getTimeInMillis()).toString());
      }
      if (jCheckBoxTo.isSelected()) {
        userQueryAd.setAttribute(Utils.TO_DATE, new Long(toCalendar
            .getTimeInMillis()).toString());
      }
      userQueryAd.setAttribute(Utils.USER_TAGS, userTagsAd);
      userQueryAd.setAttribute(Utils.INCLUDE_STATE_ARRAY,
          getIncludeStateString());
    } catch (Exception e) {
      JOptionPane.showOptionDialog(QueryPanel.this, "Unable to Add query",
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
    }
    if (isEditing) {
      queryPreferencesPanel.setQueryToTable(queryName, lbAddress, new Boolean(
          jCheckBoxOwned.isSelected()), userQueryAd);
    } else {
      queryPreferencesPanel.addQueryToTable(queryName, lbAddress, new Boolean(
          jCheckBoxOwned.isSelected()), userQueryAd);
    }
    logger.debug("userQueryAd: " + userQueryAd);
    this.dispose();
  }

  void setQuery(Ad userQueryAd) {
    if ((userQueryAd != null) && (userQueryAd.size() != 0)) {
      try {
        this.queryName = userQueryAd.getStringValue(Utils.QUERY_NAME).get(0)
            .toString();
        jTextFieldQueryName.setText(this.queryName);
        if (((Boolean) userQueryAd.getBooleanValue(Utils.ALL_LB_FLAG).get(0))
            .booleanValue()) {
          jComboBoxAvailableLB.setSelectedIndex(0);
        } else {
          String lbAddress = userQueryAd.getStringValue(Utils.LB_ADDRESS)
              .get(0).toString();
          if (jobMonitorJFrame.getLBMenuItems().contains(lbAddress)) {
            jComboBoxAvailableLB.setSelectedItem(lbAddress);
          } else {
            jComboBoxAvailableLB.setSelectedIndex(0);
          }
        }
        if (((Boolean) userQueryAd.getBooleanValue(Utils.OWNED_JOBS_ONLY_FLAG)
            .get(0)).booleanValue()) {
          jCheckBoxOwned.setSelected(true);
        } else {
          jCheckBoxOwned.setSelected(false);
        }
        if (userQueryAd.hasAttribute(Utils.FROM_DATE)) {
          Calendar fromCalendar = Calendar.getInstance();
          fromCalendar.setTimeInMillis(Long.parseLong(userQueryAd
              .getStringValue(Utils.FROM_DATE).get(0).toString(), 10));
          jTextFieldDay.setText(new Integer(fromCalendar
              .get(Calendar.DAY_OF_MONTH)).toString());
          jTextFieldMonth.setText(new Integer(
              fromCalendar.get(Calendar.MONTH) + 1).toString());
          jTextFieldYear.setText(new Integer(fromCalendar.get(Calendar.YEAR))
              .toString());
          jTextFieldHourFrom.setText(new Integer(fromCalendar
              .get(Calendar.HOUR_OF_DAY)).toString());
          jTextFieldMinuteFrom.setText(new Integer(fromCalendar
              .get(Calendar.MINUTE)).toString());
          jTextFieldSecondFrom.setText(new Integer(fromCalendar
              .get(Calendar.SECOND)).toString());
          jCheckBoxFrom.setSelected(true);
          setFromEnabled(true);
        }
        if (userQueryAd.hasAttribute(Utils.TO_DATE)) {
          Calendar toCalendar = Calendar.getInstance();
          toCalendar.setTimeInMillis(Long.parseLong(userQueryAd.getStringValue(
              Utils.TO_DATE).get(0).toString(), 10));
          jTextFieldDay1.setText(new Integer(toCalendar
              .get(Calendar.DAY_OF_MONTH)).toString());
          jTextFieldMonth1.setText(new Integer(
              toCalendar.get(Calendar.MONTH) + 1).toString());
          jTextFieldYear1.setText(new Integer(toCalendar.get(Calendar.YEAR))
              .toString());
          jTextFieldHourTo.setText(new Integer(toCalendar
              .get(Calendar.HOUR_OF_DAY)).toString());
          jTextFieldMinuteTo.setText(new Integer(toCalendar
              .get(Calendar.MINUTE)).toString());
          jTextFieldSecondTo.setText(new Integer(toCalendar
              .get(Calendar.SECOND)).toString());
          jCheckBoxTo.setSelected(true);
          setToEnabled(true);
        }
        if (userQueryAd.hasAttribute(Utils.INCLUDE_STATE_ARRAY)) {
          String stateString = userQueryAd.getStringValue(
              Utils.INCLUDE_STATE_ARRAY).get(0).toString();
          jCheckBoxSubmitted.setSelected((stateString.charAt(0) == '1') ? true
              : false);
          jCheckBoxWaiting.setSelected((stateString.charAt(1) == '1') ? true
              : false);
          jCheckBoxReady.setSelected((stateString.charAt(2) == '1') ? true
              : false);
          jCheckBoxScheduled.setSelected((stateString.charAt(3) == '1') ? true
              : false);
          jCheckBoxRunning.setSelected((stateString.charAt(4) == '1') ? true
              : false);
          jCheckBoxDone.setSelected((stateString.charAt(5) == '1') ? true
              : false);
          jCheckBoxCleared.setSelected((stateString.charAt(6) == '1') ? true
              : false);
          jCheckBoxAborted.setSelected((stateString.charAt(7) == '1') ? true
              : false);
          jCheckBoxCancelled.setSelected((stateString.charAt(8) == '1') ? true
              : false);
          jCheckBoxPurged.setSelected((stateString.charAt(9) == '1') ? true
              : false);
        } else {
          setDefaultIncludeStateSelection();
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        return;
      }
      Ad userTagsAd = new Ad();
      try {
        userTagsAd = (Ad) queryPreferencesPanel.getQueryFromMap(this.queryName)
            .getAdValue(Utils.USER_TAGS).get(0);
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
          // error
        }
        return;
      }
      if (userTagsAd != null) {
        Iterator iterator = userTagsAd.attributes();
        String tagName;
        String tagValue;
        while (iterator.hasNext()) {
          tagName = iterator.next().toString();
          try {
            tagValue = userTagsAd.getStringValue(tagName).get(0).toString();
          } catch (Exception e) {
            if (isDebugging) {
              e.printStackTrace();
              // error
            }
            return;
          }
          Vector rowElement = new Vector();
          rowElement.addElement(tagName);
          rowElement.addElement(tagValue);
          jobTableModel.addRow(rowElement);
        }
      }
    }
  }

  /**
   * Sets default include state check boxes selection
   */
  void setDefaultIncludeStateSelection() {
    jCheckBoxSubmitted.setSelected(true);
    jCheckBoxWaiting.setSelected(true);
    jCheckBoxReady.setSelected(true);
    jCheckBoxScheduled.setSelected(true);
    jCheckBoxRunning.setSelected(true);
    jCheckBoxDone.setSelected(true);
    jCheckBoxCleared.setSelected(true);
    jCheckBoxAborted.setSelected(true);
    jCheckBoxCancelled.setSelected(true);
    jCheckBoxPurged.setSelected(false);
  }

  /**
   * Gets user job states selection to include in a query
   * @return a boolean array
   */
  boolean[] getIncludeStateArray() {
    boolean[] boolArray = { false, false, false, false, false, false, false,
        false, false, false
    };
    boolArray[0] = jCheckBoxSubmitted.isSelected() ? true : false;
    boolArray[1] = jCheckBoxWaiting.isSelected() ? true : false;
    boolArray[2] = jCheckBoxReady.isSelected() ? true : false;
    boolArray[3] = jCheckBoxScheduled.isSelected() ? true : false;
    boolArray[4] = jCheckBoxRunning.isSelected() ? true : false;
    boolArray[5] = jCheckBoxDone.isSelected() ? true : false;
    boolArray[6] = jCheckBoxCleared.isSelected() ? true : false;
    boolArray[7] = jCheckBoxAborted.isSelected() ? true : false;
    boolArray[8] = jCheckBoxCancelled.isSelected() ? true : false;
    boolArray[9] = jCheckBoxPurged.isSelected() ? true : false;
    return boolArray;
  }

  /**
   * Gets user job states selection to include in a query
   * @return a String
   */
  String getIncludeStateString() {
    boolean[] boolArray = getIncludeStateArray();
    String string = "";
    for (int i = 0; i < boolArray.length; i++) {
      string += boolArray[i] ? "1" : "0";
    }
    return string;
  }

  /**
   * Gets user job states selection to exclude in a query
   * @return a boolean array
   */
  boolean[] getExcludeStateArray() {
    boolean[] boolArray = { false, false, false, false, false, false, false,
        false, false, false
    };
    boolArray[0] = jCheckBoxSubmitted.isSelected() ? false : true;
    boolArray[1] = jCheckBoxWaiting.isSelected() ? false : true;
    boolArray[2] = jCheckBoxReady.isSelected() ? false : true;
    boolArray[3] = jCheckBoxScheduled.isSelected() ? false : true;
    boolArray[4] = jCheckBoxRunning.isSelected() ? false : true;
    boolArray[5] = jCheckBoxDone.isSelected() ? false : true;
    boolArray[6] = jCheckBoxCleared.isSelected() ? false : true;
    boolArray[7] = jCheckBoxAborted.isSelected() ? false : true;
    boolArray[8] = jCheckBoxCancelled.isSelected() ? false : true;
    boolArray[9] = jCheckBoxPurged.isSelected() ? false : true;
    return boolArray;
  }

  void jCheckBoxFromEvent(ActionEvent e) {
    setFromEnabled(jCheckBoxFrom.isSelected() ? true : false);
  }

  void jCheckBoxToEvent(ActionEvent e) {
    setToEnabled(jCheckBoxTo.isSelected() ? true : false);
  }

  void setFromEnabled(boolean bool) {
    jTextFieldDay.setEnabled(bool);
    upDay.setEnabled(bool);
    downDay.setEnabled(bool);
    jTextFieldMonth.setEnabled(bool);
    upMonth.setEnabled(bool);
    downMonth.setEnabled(bool);
    jTextFieldYear.setEnabled(bool);
    upYear.setEnabled(bool);
    downYear.setEnabled(bool);
    jTextFieldHourFrom.setEnabled(bool);
    upHourFrom.setEnabled(bool);
    downHourFrom.setEnabled(bool);
    jTextFieldMinuteFrom.setEnabled(bool);
    upMinuteFrom.setEnabled(bool);
    downMinuteFrom.setEnabled(bool);
    jTextFieldSecondFrom.setEnabled(bool);
    upSecondFrom.setEnabled(bool);
    downSecondFrom.setEnabled(bool);
  }

  void setToEnabled(boolean bool) {
    jTextFieldDay1.setEnabled(bool);
    upDay1.setEnabled(bool);
    downDay1.setEnabled(bool);
    jTextFieldMonth1.setEnabled(bool);
    upMonth1.setEnabled(bool);
    downMonth1.setEnabled(bool);
    jTextFieldYear1.setEnabled(bool);
    upYear1.setEnabled(bool);
    downYear1.setEnabled(bool);
    jTextFieldHourTo.setEnabled(bool);
    upHourTo.setEnabled(bool);
    downHourTo.setEnabled(bool);
    jTextFieldMinuteTo.setEnabled(bool);
    upMinuteTo.setEnabled(bool);
    downMinuteTo.setEnabled(bool);
    jTextFieldSecondTo.setEnabled(bool);
    upSecondTo.setEnabled(bool);
    downSecondTo.setEnabled(bool);
  }
}