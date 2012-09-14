/*
 * JobTypePanel.java
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
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.FocusEvent;
import java.util.Vector;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JList;
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
import javax.swing.table.TableColumn;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Jdl;

/**
 * Implementation of the JobTypePanel class.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class JobTypePanel extends JPanel {
  static Logger logger = Logger.getLogger(JDLEditor.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int CHECKPOINTABLE = 0;

  static final int PARTITIONABLE = 1;

  static final int STEP_COLUMN_INDEX = 0;

  static final int WEIGHT_COLUMN_INDEX = 1;

  static final String DASH = "-";

  int mode = CHECKPOINTABLE;

  Vector labelListVector = new Vector();

  JDLEditorInterface jint;

  Component component;

  ButtonGroup buttonGroupJobSteps = new ButtonGroup();

  JRadioButton jRadioButtonLabelList = new JRadioButton("Label List");

  JRadioButton jRadioButtonNumericValue = new JRadioButton("Numeric Value");

  ButtonGroup buttonGroup = new ButtonGroup();

  JTextField jTextFieldLabelList = new JTextField();

  JPanel jPanelJobSteps = new JPanel();

  JScrollPane jScrollPaneLabelList = new JScrollPane();

  JList jListLabelList = new JList();

  JTable jTableLabelList;

  JobTableModel jobTableModel;

  JButton jButtonAdd = new JButton();

  JButton jButtonRemove = new JButton();

  JButton jButtonClear = new JButton();

  BasicArrowButton upLastStep = new BasicArrowButton(BasicArrowButton.NORTH);

  BasicArrowButton downLastStep = new BasicArrowButton(BasicArrowButton.SOUTH);

  JTextField jTextFieldLastStep = new JTextField();

  JLabel jLabelCurrentIndex = new JLabel();

  String errorMsg = "";

  String warningMsg = "";

  BasicArrowButton upFirstStep = new BasicArrowButton(BasicArrowButton.NORTH);

  BasicArrowButton downFirstStep = new BasicArrowButton(BasicArrowButton.SOUTH);

  JTextField jTextFieldFirstStep = new JTextField();

  BasicArrowButton downNodeNumber = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton upNodeNumber = new BasicArrowButton(BasicArrowButton.NORTH);

  JTextField jTextFieldNodeNumber = new JTextField();

  JLabel jLabelType = new JLabel();

  JComboBox jComboBoxJobType = new JComboBox(Utils.JOB_TYPES);

  JPanel jPanelListenerPort = new JPanel();

  BasicArrowButton downListenerPort = new BasicArrowButton(
      BasicArrowButton.SOUTH);

  BasicArrowButton upListenerPort = new BasicArrowButton(BasicArrowButton.NORTH);

  JTextField jTextFieldListenerPort = new JTextField();

  JLabel jLabelFirstStepNumeric = new JLabel();

  JPanel jPanelNodeNumber = new JPanel();

  JPanel jPanelJobType = new JPanel();

  JComboBox jComboBoxCurrentItem = new JComboBox();

  JLabel jLabelFirstStepList = new JLabel();

  JCheckBox jCheckBoxJobSteps = new JCheckBox();

  JPanel jPanelJobStepsList = new JPanel();

  JPanel jPanelJobStepsNumeric = new JPanel();

  JLabel jLabelJobSteps = new JLabel();

  JLabel jLabelLastStep = new JLabel();

  JLabel jLabelStepsList = new JLabel();

  JCheckBox jCheckBoxListenerPort = new JCheckBox();

  JLabel jLabelNodeNumber = new JLabel("NodeNumber");

  Vector vectorHeader = new Vector();

  JDLEditor editor;

  public JobTypePanel(Component component) {
    this.component = component;
    if (component instanceof JDLEditor) {
      jint = (JDLEditor) component;
      editor = (JDLEditor) component;
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
    vectorHeader.addElement("Step");
    vectorHeader.addElement("Weight");
    jobTableModel = new JobTableModel(vectorHeader, 0);
    jTableLabelList = new JTable(jobTableModel);
    jTableLabelList.getTableHeader().setReorderingAllowed(false);
    TableColumn col = jTableLabelList.getColumnModel().getColumn(
        STEP_COLUMN_INDEX);
    col.setCellRenderer(new GUITableTooltipCellRenderer());
    col.setPreferredWidth(250);
    col = jTableLabelList.getColumnModel().getColumn(WEIGHT_COLUMN_INDEX);
    //col.setCellRenderer(new GUITableTooltipCellRenderer());
    col.setCellRenderer(jTableLabelList.getDefaultRenderer(Integer.class));
    jobTableModel.setColumnEditable(WEIGHT_COLUMN_INDEX, true);
    jTableLabelList.setEnabled(false);
    jPanelJobStepsList.setEnabled(true);
    jPanelJobStepsNumeric.setEnabled(false);
    jPanelJobSteps.setVisible(true);
    jPanelListenerPort.setVisible(true);
    jPanelNodeNumber.setVisible(false);
    setNumberOfStepsEnabled(false);
    setLabelListEnabled(false);
    setCurrentIndexEnabled(false);
    setCurrentStepEnabled(false);
    jRadioButtonLabelList.setSelected(true);
    setJobStepsPanelEnabled(false);
    setListenerPortEnabled(false);
    jCheckBoxJobSteps.setText("JobSteps");
    jTextFieldLabelList.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jTextFieldLabelListFocusLost(e);
      }
    });
    jButtonAdd.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonCheckpointAddEvent(e);
      }
    });
    jButtonAdd.setText("Add");
    jButtonRemove.setText("Remove");
    jButtonRemove.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonCheckpointRemoveEvent(e);
      }
    });
    jButtonClear.setText("Clear");
    jButtonClear.setToolTipText("");
    jButtonClear.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        if (JobTypePanel.this.labelListVector.size() != 0) {
          int choice = JOptionPane.showOptionDialog(component,
              "Clear Steps List?", "Confirm Clear", JOptionPane.YES_NO_OPTION,
              JOptionPane.QUESTION_MESSAGE, null, null, null);
          if (choice == 0) {
            jButtonCheckpointClearEvent(e);
          }
        }
        jButtonClear.grabFocus();
        jTextFieldLabelList.grabFocus();
        jTextFieldLabelList.selectAll();
      }
    });
    upLastStep.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldLastStep, Utils.INTEGER, Integer
            .toString(Utils.JOBSTEPS_DEF_VAL), Utils.JOBSTEPS_MIN_VAL,
            Utils.JOBSTEPS_MAX_VAL);
      }
    });
    downLastStep.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldLastStep, Utils.INTEGER, Integer
            .toString(Utils.JOBSTEPS_DEF_VAL), Utils.JOBSTEPS_MIN_VAL,
            Utils.JOBSTEPS_MAX_VAL);
        if (Integer.parseInt(jTextFieldLastStep.getText(), 10) < Integer
            .parseInt(jTextFieldFirstStep.getText(), 10)) {
          jTextFieldFirstStep.setText(Integer
              .toString(Utils.CURRENTSTEP_DEF_VAL));
        }
      }
    });
    jTextFieldLastStep.setText(Integer.toString(Utils.JOBSTEPS_DEF_VAL));
    jTextFieldLastStep.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldLastStep.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldLastStep, Utils.INTEGER,
            Integer.toString(Utils.JOBSTEPS_DEF_VAL), Utils.JOBSTEPS_MIN_VAL,
            Utils.JOBSTEPS_MAX_VAL);
      }

      public void focusGained(FocusEvent e) {
      }
    });
    upFirstStep.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldFirstStep, Utils.INTEGER, Integer
            .toString(Utils.CURRENTSTEP_DEF_VAL), Utils.CURRENTSTEP_MIN_VAL,
            Integer.parseInt(jTextFieldLastStep.getText()));
      }
    });
    downFirstStep.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldFirstStep, Utils.INTEGER, Integer
            .toString(Utils.CURRENTSTEP_DEF_VAL), Utils.CURRENTSTEP_MIN_VAL,
            Integer.parseInt(jTextFieldLastStep.getText()));
      }
    });
    jTextFieldFirstStep.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldFirstStep.setText(Integer.toString(Utils.CURRENTSTEP_DEF_VAL));
    jTextFieldFirstStep.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldFirstStep, Utils.INTEGER,
            Integer.toString(Utils.CURRENTSTEP_DEF_VAL),
            Utils.CURRENTSTEP_MIN_VAL, Integer.parseInt(jTextFieldLastStep
                .getText()));
      }

      public void focusGained(FocusEvent e) {
      }
    });
    downNodeNumber.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldNodeNumber, Utils.INTEGER, Integer
            .toString(Utils.NODENUMBER_DEF_VAL), Utils.NODENUMBER_MIN_VAL,
            Utils.NODENUMBER_MAX_VAL);
      }
    });
    upNodeNumber.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldNodeNumber, Utils.INTEGER, Integer
            .toString(Utils.NODENUMBER_DEF_VAL), Utils.NODENUMBER_MIN_VAL,
            Utils.NODENUMBER_MAX_VAL);
      }
    });
    jTextFieldNodeNumber.setText(Integer.toString(Utils.NODENUMBER_DEF_VAL));
    jTextFieldNodeNumber.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldNodeNumber.addFocusListener(new java.awt.event.FocusListener() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldNodeNumber, Utils.INTEGER,
            Integer.toString(Utils.NODENUMBER_DEF_VAL),
            Utils.NODENUMBER_MIN_VAL, Utils.NODENUMBER_MAX_VAL);
      }

      public void focusGained(FocusEvent e) {
      }
    });
    jLabelType.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelType.setText("JobType");
    jComboBoxJobType.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jComboBoxJobTypeEvent(e);
      }
    });
    downListenerPort.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldListenerPort, Utils.INTEGER, Integer
            .toString(Utils.SHADOWPORT_DEF_VAL), Utils.SHADOWPORT_MIN_VAL,
            Utils.SHADOWPORT_MAX_VAL);
      }
    });
    upListenerPort.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldListenerPort, Utils.INTEGER, Integer
            .toString(Utils.SHADOWPORT_DEF_VAL), Utils.SHADOWPORT_MIN_VAL,
            Utils.SHADOWPORT_MAX_VAL);
      }
    });
    jTextFieldListenerPort.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldListenerPort.setText(Integer.toString(Utils.SHADOWPORT_DEF_VAL));
    jTextFieldListenerPort.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldListenerPort, Utils.INTEGER,
            Integer.toString(Utils.SHADOWPORT_DEF_VAL),
            Utils.SHADOWPORT_MIN_VAL, Utils.SHADOWPORT_MAX_VAL);
      }

      public void focusGained(FocusEvent e) {
      }
    });
    jLabelFirstStepNumeric.setText("First Step");
    jLabelFirstStepList.setText("First Step");
    jCheckBoxJobSteps.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jCheckBoxJobStepsEvent(e);
      }
    });
    jLabelJobSteps.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelJobSteps.setText("JobSteps");
    jLabelLastStep.setText("Last Step");
    jLabelStepsList.setText("Steps List");
    jListLabelList.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jListLabelListFocusLost(e);
      }
    });
    jTableLabelList.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jTableLabelListFocusLost(e);
      }
    });
    jCheckBoxListenerPort.setText("ListenerPort");
    jCheckBoxListenerPort
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jCheckBoxListenerPortEvent(e);
          }
        });
    jRadioButtonLabelList
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jRadioButtonLabelListEvent(e);
          }
        });
    jRadioButtonNumericValue
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jRadioButtonNumericValueEvent(e);
          }
        });
    buttonGroup.add(jRadioButtonLabelList);
    buttonGroup.add(jRadioButtonNumericValue);
    jComboBoxCurrentItem.setRenderer(new GUIListCellTooltipRenderer());
    GridBagLayout gbl = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.insets = new Insets(3, 3, 3, 3);
    // jPanelJobType
    jPanelJobType.setLayout(gbl);
    jPanelJobType.setBorder(new TitledBorder(new EtchedBorder(), " Job Type ",
        0, 0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelJobType.add(jLabelType, GraphicUtils.setGridBagConstraints(gbc, 0, 0,
        1, 1, 0.0, 0.0, GridBagConstraints.CENTER, GridBagConstraints.NONE,
        null, 0, 0));
    jPanelJobType.add(jComboBoxJobType, GraphicUtils.setGridBagConstraints(gbc,
        1, 0, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    // jPanelListenerPort
    jPanelListenerPort.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelListenerPort
        .setBorder(new TitledBorder(new EtchedBorder(), " Listener Port ", 0,
            0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    JPanel jPanelArrowListenerPort = new JPanel();
    jPanelArrowListenerPort.setLayout(new BoxLayout(jPanelArrowListenerPort,
        BoxLayout.Y_AXIS));
    jPanelArrowListenerPort.add(upListenerPort, null);
    jPanelArrowListenerPort.add(downListenerPort, null);
    jPanelListenerPort
        .add(jCheckBoxListenerPort, GraphicUtils.setGridBagConstraints(gbc, 0,
            0, 1, 1, 0.0, 0.0, GridBagConstraints.WEST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jTextFieldListenerPort.setPreferredSize(new Dimension(50, 32));
    jPanelListenerPort
        .add(jTextFieldListenerPort, GraphicUtils.setGridBagConstraints(gbc, 1,
            0, 1, 1, 0.0, 0.0, GridBagConstraints.EAST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelListenerPort.add(jPanelArrowListenerPort, GraphicUtils
        .setGridBagConstraints(gbc, 2, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    // jPanelNodeNumber
    jPanelNodeNumber.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelNodeNumber.setBorder(new TitledBorder(new EtchedBorder(),
        " Number of Computational Nodes ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    JPanel jPanelArrowNodeNumber = new JPanel();
    jPanelArrowNodeNumber.setLayout(new BoxLayout(jPanelArrowNodeNumber,
        BoxLayout.Y_AXIS));
    jPanelArrowNodeNumber.add(upNodeNumber, null);
    jPanelArrowNodeNumber.add(downNodeNumber, null);
    jPanelNodeNumber.add(jLabelNodeNumber, GraphicUtils.setGridBagConstraints(
        gbc, 0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.WEST,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jTextFieldNodeNumber
        .setPreferredSize(GraphicUtils.NUMERIC_TEXT_FIELD_DIMENSION);
    jPanelNodeNumber
        .add(jTextFieldNodeNumber, GraphicUtils.setGridBagConstraints(gbc, 1,
            0, 1, 1, 0.0, 0.0, GridBagConstraints.EAST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelNodeNumber.add(jPanelArrowNodeNumber, GraphicUtils
        .setGridBagConstraints(gbc, 2, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    // jPanelJobSteps
    jPanelJobStepsList.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelJobStepsList.add(jLabelStepsList, GraphicUtils.setGridBagConstraints(
        gbc, 0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.NONE, null, 0, 0));
    jTextFieldLabelList.setPreferredSize(new Dimension(250, 20));
    jPanelJobStepsList.add(jTextFieldLabelList, GraphicUtils
        .setGridBagConstraints(gbc, 1, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    jPanelJobStepsList.add(jButtonAdd, GraphicUtils.setGridBagConstraints(gbc,
        2, 0, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jScrollPaneLabelList.setPreferredSize(new Dimension(150, 150));
    //jScrollPaneLabelList.getViewport().add(jTableLabelList, null);
    jScrollPaneLabelList.getViewport().setBackground(Color.white);
    jScrollPaneLabelList.getViewport().add(jListLabelList, null);
    jPanelJobStepsList.add(jScrollPaneLabelList, GraphicUtils
        .setGridBagConstraints(gbc, 0, 1, 2, 2, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH, null, 0, 0));
    jPanelJobStepsList.add(jButtonRemove, GraphicUtils.setGridBagConstraints(
        gbc, 2, 1, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelJobStepsList.add(jButtonClear, GraphicUtils.setGridBagConstraints(
        gbc, 2, 2, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelJobStepsList.add(jLabelFirstStepList, GraphicUtils
        .setGridBagConstraints(gbc, 0, 3, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE, null, 0, 0));
    jComboBoxCurrentItem.setPreferredSize(new Dimension(250, 20));
    jPanelJobStepsList.add(jComboBoxCurrentItem,
        GraphicUtils
            .setGridBagConstraints(gbc, 1, 3, 1, 1, 0.0, 0.0,
                GridBagConstraints.NORTH, GridBagConstraints.HORIZONTAL, null,
                0, 0));
    jPanelJobStepsNumeric.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    JPanel jPanelArrowLastStep = new JPanel();
    jPanelArrowLastStep.setLayout(new BoxLayout(jPanelArrowLastStep,
        BoxLayout.Y_AXIS));
    jPanelArrowLastStep.add(upLastStep, null);
    jPanelArrowLastStep.add(downLastStep, null);
    jPanelJobStepsNumeric
        .add(jLabelLastStep, GraphicUtils.setGridBagConstraints(gbc, 0, 0, 1,
            1, 0.0, 0.0, GridBagConstraints.WEST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jTextFieldLastStep
        .setPreferredSize(GraphicUtils.NUMERIC_TEXT_FIELD_DIMENSION);
    jPanelJobStepsNumeric
        .add(jTextFieldLastStep, GraphicUtils.setGridBagConstraints(gbc, 1, 0,
            1, 1, 0.0, 0.0, GridBagConstraints.EAST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelJobStepsNumeric.add(jPanelArrowLastStep, GraphicUtils
        .setGridBagConstraints(gbc, 2, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    JPanel jPanelArrowFirstStep = new JPanel();
    jPanelArrowFirstStep.setLayout(new BoxLayout(jPanelArrowFirstStep,
        BoxLayout.Y_AXIS));
    jPanelArrowFirstStep.add(upFirstStep, null);
    jPanelArrowFirstStep.add(downFirstStep, null);
    jPanelJobStepsNumeric
        .add(jLabelFirstStepNumeric, GraphicUtils.setGridBagConstraints(gbc, 0,
            1, 1, 1, 0.0, 0.0, GridBagConstraints.WEST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jTextFieldFirstStep
        .setPreferredSize(GraphicUtils.NUMERIC_TEXT_FIELD_DIMENSION);
    jPanelJobStepsNumeric
        .add(jTextFieldFirstStep, GraphicUtils.setGridBagConstraints(gbc, 1, 1,
            1, 1, 0.0, 0.0, GridBagConstraints.EAST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelJobStepsNumeric.add(jPanelArrowFirstStep, GraphicUtils
        .setGridBagConstraints(gbc, 2, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    JPanel jPanelJobStepsInner = new JPanel();
    jPanelJobStepsInner.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelJobStepsInner.add(new JPanelCheckBox(jRadioButtonLabelList,
        jPanelJobStepsList), GraphicUtils.setGridBagConstraints(gbc, 0, 1, 1,
        1, 1.0, 1.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, null, 0, 0));
    jPanelJobStepsInner.add(new JPanelCheckBox(jRadioButtonNumericValue,
        jPanelJobStepsNumeric), GraphicUtils.setGridBagConstraints(gbc, 1, 1,
        1, 1, 0.0, 0.0, GridBagConstraints.PAGE_START, GridBagConstraints.NONE,
        null, 0, 0));
    jPanelJobSteps = new JPanelCheckBox(jCheckBoxJobSteps, jPanelJobStepsInner);
    // this
    this.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    this.add(jPanelJobType, GraphicUtils.setGridBagConstraints(gbc, 0, 0, 1, 1,
        0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, new Insets(1, 1, 1, 1), 0, 0));
    this.add(jPanelListenerPort, GraphicUtils.setGridBagConstraints(gbc, 0, 1,
        1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    this.add(jPanelNodeNumber, GraphicUtils.setGridBagConstraints(gbc, 0, 2, 1,
        1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    this.add(jPanelJobSteps, GraphicUtils.setGridBagConstraints(gbc, 0, 3, 2,
        1, 1.0, 0.2, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, null, 0, 0));
    this.add(new JPanel(), GraphicUtils.setGridBagConstraints(gbc, 0, 4, 2, 1,
        1.0, 0.8, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.VERTICAL, null, 0, 0));
    jPanelJobStepsList.setEnabled(true);
    jPanelJobStepsNumeric.setEnabled(false);
  }

  void jButtonCheckpointAddEvent(ActionEvent e) {
    String insertedText = jTextFieldLabelList.getText().trim();
    jTextFieldLabelList.grabFocus();
    if (this.mode == CHECKPOINTABLE) {
      if ((!insertedText.equals(""))
          && (!labelListVector.contains(insertedText))) {
        String checkErrorMsg = Utils.checkAttributeAdd(Jdl.CHKPT_STEPS,
            insertedText);
        if (checkErrorMsg.equals("")) {
          labelListVector.add(insertedText);
          jListLabelList.setListData(labelListVector);
          jComboBoxCurrentItem.addItem(insertedText);
          setCurrentIndexEnabled(true);
        } else {
          JOptionPane.showOptionDialog(component, checkErrorMsg,
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
        }
      }
    } else {
      if ((!insertedText.equals(""))
          && (!jobTableModel.isElementPresentInColumn(insertedText,
              STEP_COLUMN_INDEX))) {
        String checkErrorMsg = Utils.checkAttributeAdd(Jdl.CHKPT_STEPS,
            insertedText);
        if (checkErrorMsg.equals("")) {
          Vector rowElement = new Vector();
          rowElement.addElement(insertedText);
          rowElement.addElement(DASH);
          jobTableModel.addRow(rowElement);
          jComboBoxCurrentItem.addItem(insertedText);
          setCurrentIndexEnabled(true);
        } else {
          JOptionPane.showOptionDialog(component, checkErrorMsg,
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
        }
      }
    }
    jTextFieldLabelList.selectAll();
  }

  String jButtonCheckpointViewEvent(boolean showWarningMsg,
      boolean showErrorMsg, ActionEvent e) {
    String result = "";
    errorMsg = "";
    result += Jdl.TYPE + " = \"Job\";\n";
    String jobType = jComboBoxJobType.getSelectedItem().toString().trim();
    int index = jobType.indexOf(Utils.JOBTYPE_LIST_SEPARATOR);
    if (index != -1) {
      String type1 = jobType.substring(0, index);
      String type2 = jobType.substring(index + 1);
      result += Jdl.JOBTYPE + " = {\"" + type1 + "\", \"" + type2 + "\"};\n";
    } else {
      result += Jdl.JOBTYPE + " = \"" + jobType + "\";\n";
    }
    if (jPanelNodeNumber.isVisible()) {
      if (jTextFieldNodeNumber.isEnabled()) {
        result += Jdl.NODENUMB + " = " + jTextFieldNodeNumber.getText().trim()
            + ";\n";
      }
    }
    if (jPanelJobSteps.isVisible() && jCheckBoxJobSteps.isSelected()) {
      if (jPanelJobStepsList.isEnabled()) {
        if (mode == CHECKPOINTABLE) {
          int itemsCount = labelListVector.size();
          if (itemsCount != 0) {
            result += Jdl.CHKPT_STEPS + " = ";
            if (itemsCount == 1) {
              result += "\"" + labelListVector.get(0).toString() + "\";\n";
            } else {
              result += "{";
              for (int i = 0; i < itemsCount - 1; i++) {
                result += "\"" + labelListVector.get(i) + "\", ";
              }
              result += "\"" + labelListVector.get(itemsCount - 1) + "\"};\n";
            }
            result += Jdl.CHKPT_CURRENTSTEP + " = "
                + (jComboBoxCurrentItem.getSelectedIndex() + 1) + ";\n";
          } else {
            errorMsg += "Steps list must contain at least one item";
            jTextFieldLabelList.selectAll();
            jTextFieldLabelList.grabFocus();
          }
        } else { // mode == PARTITIONABLE
          if (!checkWeights()) {
            String msg = "Step Weights must be all int values or '" + DASH
                + "'";
            JOptionPane.showOptionDialog(JobTypePanel.this, msg,
                Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE, null, null, null);
            jint.setJTextAreaJDL(msg);
            return "";
          }
          int itemsCount = jobTableModel.getRowCount();
          if (itemsCount != 0) {
            String stepWeight = Jdl.STEP_WEIGHT + " = ";
            for (int i = 0; i < itemsCount - 1; i++) {
            }
            result += Jdl.CHKPT_STEPS + " = ";
            if (itemsCount == 1) {
              result += "\""
                  + jobTableModel.getValueAt(0, STEP_COLUMN_INDEX).toString()
                  + "\";\n";
              stepWeight += jobTableModel.getValueAt(0, WEIGHT_COLUMN_INDEX)
                  .toString()
                  + ";\n";
            } else {
              result += "{";
              stepWeight += "{";
              for (int i = 0; i < itemsCount - 1; i++) {
                result += "\""
                    + jobTableModel.getValueAt(i, STEP_COLUMN_INDEX).toString()
                    + "\", ";
                stepWeight += jobTableModel.getValueAt(i, WEIGHT_COLUMN_INDEX)
                    .toString()
                    + ", ";
              }
              result += "\""
                  + jobTableModel.getValueAt(itemsCount - 1, STEP_COLUMN_INDEX)
                      .toString() + "\"};\n";
              stepWeight += jobTableModel.getValueAt(itemsCount - 1,
                  WEIGHT_COLUMN_INDEX).toString()
                  + "};\n";
            }
            result += Jdl.CHKPT_CURRENTSTEP + " = "
                + (jComboBoxCurrentItem.getSelectedIndex() + 1) + ";\n";
            if (stepWeight.indexOf(DASH) == -1) {
              result += stepWeight;
            }
          } else {
            errorMsg += "Steps list must contain at least one item";
            jTextFieldLabelList.selectAll();
            jTextFieldLabelList.grabFocus();
          }
        }
        //} else if (jPanelJobStepsNumeric.isVisible()) {
      } else if (jPanelJobStepsNumeric.isEnabled()) {
        result += Jdl.CHKPT_STEPS + " = " + jTextFieldLastStep.getText().trim()
            + ";\n";
        result += Jdl.CHKPT_CURRENTSTEP + " = "
            + jTextFieldFirstStep.getText().trim() + ";\n";
      }
    }
    //if (jPanelListenerPort.isEnabled()) {
    if (jPanelListenerPort.isVisible()) {
      if (jTextFieldListenerPort.isEnabled()) {
        //!!! No Jdl constant for ListenerPort check it!
        result += Jdl.SHPORT + " = " + jTextFieldListenerPort.getText().trim()
            + ";\n";
      }
    }
    logger.debug("result: " + result);
    warningMsg = ExprChecker.checkResult(result, Utils.jobTypeAttributeArray);
    errorMsg = errorMsg.trim();
    warningMsg = warningMsg.trim();
    if (!errorMsg.trim().equals("") && showErrorMsg) {
      GraphicUtils.showOptionDialogMsg(JobTypePanel.this, errorMsg,
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE, null,
          null);
    } else {
      if (!warningMsg.trim().equals("") && showWarningMsg) {
        GraphicUtils.showOptionDialogMsg(JobTypePanel.this, warningMsg,
            Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
            null, null);
      }
      jint.setJTextAreaJDL(result);
    }
    return result;
  }

  void jButtonCheckpointResetEvent(ActionEvent e) {
    editor.setPartitionablePanelVisible(false);
    jComboBoxJobType.setSelectedIndex(0);
    jTextFieldNodeNumber.setText(Integer.toString(Utils.NODENUMBER_DEF_VAL));
    setNodeNumberEnabled(false);
    jCheckBoxJobSteps.setSelected(false);
    setJobStepsPanelEnabled(false);
    jTextFieldLabelList.setText("");
    jButtonCheckpointClearEvent(null);
    jTextFieldLastStep.setText(Integer.toString(Utils.JOBSTEPS_DEF_VAL));
    jTextFieldLabelList.selectAll();
    jTextFieldLabelList.grabFocus();
    jint.setJTextAreaJDL("");
  }

  String getErrorMsg() {
    return errorMsg;
  }

  String getWarningMsg() {
    return warningMsg;
  }

  void jButtonCheckpointClearEvent(ActionEvent e) {
    this.labelListVector.removeAllElements();
    jComboBoxCurrentItem.removeAllItems();
    jobTableModel.removeAllRows();
    jListLabelList.setListData(this.labelListVector);
    setCurrentIndexEnabled(false);
    jTextFieldLabelList.selectAll();
    jTextFieldLabelList.grabFocus();
  }

  void jButtonCheckpointRemoveEvent(ActionEvent e) {
    if (mode == CHECKPOINTABLE) {
      int[] selectedItems = jListLabelList.getSelectedIndices();
      int selectedItemsCount = selectedItems.length;
      jTextFieldLabelList.grabFocus();
      if (selectedItemsCount != 0) {
        for (int i = selectedItemsCount - 1; i >= 0; i--) {
          labelListVector.removeElementAt(selectedItems[i]);
          jComboBoxCurrentItem.removeItemAt(selectedItems[i]);
        }
        jListLabelList.setListData(labelListVector);
        if (jListLabelList.getModel().getSize() != 0) {
          int selectableItem = selectedItems[selectedItemsCount - 1] + 1
              - selectedItemsCount; //Next.
          if (selectableItem > jListLabelList.getModel().getSize() - 1) {
            selectableItem--; // Prev. (selectedItems[selectedItemsCount - 1] - selectedItemsCount).
          }
          jListLabelList.setSelectedIndex(selectableItem);
        } else {
          setCurrentIndexEnabled(false);
        }
      } else {
        JOptionPane.showOptionDialog(JobTypePanel.this, Utils.SELECT_AN_ITEM,
            Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.INFORMATION_MESSAGE, null, null, null);
      }
    } else { // mode == PARTITIONABLE
      int[] selectedRows = jTableLabelList.getSelectedRows();
      int selectedRowCount = selectedRows.length;
      if (selectedRowCount != 0) {
        for (int i = selectedRowCount - 1; i >= 0; i--) {
          jobTableModel.removeRow(selectedRows[i]);
          jComboBoxCurrentItem.removeItemAt(selectedRows[i]);
        }
        if (jobTableModel.getRowCount() != 0) {
          int selectableRow = selectedRows[selectedRowCount - 1] + 1
              - selectedRowCount; // Next.
          if (selectableRow > jobTableModel.getRowCount() - 1) {
            selectableRow--; // Prev. (selectedRow[selectedRowCount - 1] - selectedRowCount).
          }
          jTableLabelList.setRowSelectionInterval(selectableRow, selectableRow);
        } else {
          setCurrentIndexEnabled(false);
        }
      } else {
        JOptionPane.showOptionDialog(JobTypePanel.this, Utils.SELECT_AN_ITEM,
            Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.INFORMATION_MESSAGE, null, null, null);
      }
    }
    jTextFieldLabelList.selectAll();
  }

  void setNumberOfStepsEnabled(boolean bool) {
    jLabelLastStep.setEnabled(bool);
    jTextFieldLastStep.setEnabled(bool);
    upLastStep.setEnabled(bool);
    downLastStep.setEnabled(bool);
  }

  void setCurrentStepEnabled(boolean bool) {
    jLabelFirstStepNumeric.setEnabled(bool);
    jTextFieldFirstStep.setEnabled(bool);
    upFirstStep.setEnabled(bool);
    downFirstStep.setEnabled(bool);
  }

  void setCurrentIndexEnabled(boolean bool) {
    jLabelFirstStepList.setEnabled(bool);
    jComboBoxCurrentItem.setEnabled(bool);
  }

  void setLabelListEnabled(boolean bool) {
    jLabelStepsList.setEnabled(bool);
    jTextFieldLabelList.setEnabled(bool);
    jTableLabelList.setEnabled(bool);
    jTableLabelList.getTableHeader().setEnabled(bool);
    jListLabelList.setEnabled(bool);
    jScrollPaneLabelList.setEnabled(bool);
    jButtonAdd.setEnabled(bool);
    jButtonRemove.setEnabled(bool);
    jButtonClear.setEnabled(bool);
  }

  void jComboBoxJobTypeEvent(ActionEvent e) {
    String selectedItem = jComboBoxJobType.getSelectedItem().toString();
    if (selectedItem.indexOf(Jdl.JOBTYPE_MPICH) != -1) {
      jPanelNodeNumber.setVisible(true);
      setNodeNumberEnabled(true);
    } else {
      jPanelNodeNumber.setVisible(false);
      setNodeNumberEnabled(false);
    }
    if ((selectedItem.indexOf(Jdl.JOBTYPE_CHECKPOINTABLE) != -1)
        || (selectedItem.indexOf(Jdl.JOBTYPE_PARTITIONABLE) != -1)) {
      if (selectedItem.indexOf(Jdl.JOBTYPE_CHECKPOINTABLE) != -1) {
        setMode(CHECKPOINTABLE);
      } else {
        setMode(PARTITIONABLE);
        editor.setPartitionablePanelVisible(true);
      }
      jPanelJobSteps.setVisible(true);
    } else {
      jPanelJobSteps.setVisible(false);
      editor.setPartitionablePanelVisible(false);
    }
    if (selectedItem.indexOf(Jdl.JOBTYPE_INTERACTIVE) != -1) {
      jPanelListenerPort.setVisible(true);
      jint.setDef1StandardStreamsEnabled(false);
    } else {
      jPanelListenerPort.setVisible(false);
      jint.setDef1StandardStreamsEnabled(true);
    }
  }

  void setNodeNumberEnabled(boolean bool) {
    jTextFieldNodeNumber.setEnabled(bool);
    upNodeNumber.setEnabled(bool);
    downNodeNumber.setEnabled(bool);
  }

  String getJobTypeValue() {
    return jComboBoxJobType.getSelectedItem().toString();
  }

  String getNodeNumberValue() {
    return jTextFieldNodeNumber.getText().trim();
  }

  void setListenerPortEnabled(boolean bool) {
    jTextFieldListenerPort.setEnabled(bool);
    upListenerPort.setEnabled(bool);
    downListenerPort.setEnabled(bool);
  }

  void jCheckBoxJobStepsEvent(ActionEvent e) {
    if (jCheckBoxJobSteps.isSelected()) {
      setJobStepsPanelEnabled(true);
    } else {
      setJobStepsPanelEnabled(false);
    }
  }

  void setJobStepsSelected(boolean bool) {
    jCheckBoxJobSteps.setSelected(bool);
    setJobStepsPanelEnabled(bool);
  }

  void setNumericValueSelected(boolean bool) {
    if (bool) {
      jRadioButtonNumericValue.setSelected(true);
    } else {
      jRadioButtonLabelList.setSelected(true);
    }
  }

  void setJobStepsPanelEnabled(boolean bool) {
    jRadioButtonLabelList.setEnabled(bool);
    jRadioButtonNumericValue.setEnabled(bool);
    if (bool) {
      if (jRadioButtonLabelList.isSelected()) {
        jPanelJobStepsNumeric.setEnabled(false);
        setNumberOfStepsEnabled(false);
        setCurrentStepEnabled(false);
        jPanelJobStepsList.setEnabled(true);
        setLabelListEnabled(true);
        if (jListLabelList.getModel().getSize() != 0) {
          setCurrentIndexEnabled(true);
        }
      } else if (jRadioButtonNumericValue.isSelected()) {
        jPanelJobStepsNumeric.setEnabled(true);
        setNumberOfStepsEnabled(true);
        setCurrentStepEnabled(true);
        jPanelJobStepsList.setEnabled(false);
        setLabelListEnabled(false);
        setCurrentIndexEnabled(false);
      }
    } else {
      jRadioButtonLabelList.setEnabled(false);
      jRadioButtonNumericValue.setEnabled(false);
      jPanelJobStepsNumeric.setEnabled(false);
      setNumberOfStepsEnabled(false);
      setCurrentStepEnabled(false);
      jPanelJobStepsList.setEnabled(false);
      setLabelListEnabled(false);
      setCurrentIndexEnabled(false);
    }
  }

  void setJobType(String value) {
    int jobTypesCount = Utils.JOB_TYPES.length;
    value = value.trim().toUpperCase();
    String item = "";
    for (int i = 0; i < jobTypesCount; i++) {
      item = Utils.JOB_TYPES[i].toUpperCase();
      if (value.equals(item)) {
        jComboBoxJobType.setSelectedItem(Utils.JOB_TYPES[i]);
        break;
      }
    }
    jComboBoxJobTypeEvent(null);
  }

  void setNodeNumberValue(String text) {
    jTextFieldNodeNumber.setText(text);
  }

  void setCurrentStepValue(String text) {
    setCurrentStepEnabled(true);
    jTextFieldFirstStep.setText(text);
  }

  void setCurrentIndexValue(String text) {
    jComboBoxCurrentItem.setSelectedItem(text);
    setCurrentIndexEnabled(true);
  }

  void setJobStepsValue(String text) {
    jTextFieldLastStep.setText(text);
  }

  void setJobStepsList(Vector itemVector) {
    String item;
    labelListVector.clear();
    for (int i = 0; i < itemVector.size(); i++) {
      item = itemVector.get(i).toString().trim();
      labelListVector.add(item);
      jComboBoxCurrentItem.addItem(item);
    }
    jListLabelList.setListData(labelListVector);
  }

  void setJobStepsTable(Vector itemVector) {
    Vector dashVector = new Vector();
    for (int i = 0; i < itemVector.size(); i++) {
      dashVector.add(DASH);
    }
    setJobStepsTable(itemVector, dashVector);
  }

  void setJobStepsTable(Vector itemVector, Vector weightVector) {
    String item;
    String weight;
    jobTableModel.removeAllRows();
    Vector element;
    for (int i = 0; i < itemVector.size(); i++) {
      item = itemVector.get(i).toString().trim();
      weight = weightVector.get(i).toString().trim();
      element = new Vector();
      element.add(item);
      element.add(weight);
      jobTableModel.addRow(element);
      jComboBoxCurrentItem.addItem(item);
    }
  }

  void setListenerPortValue(String text) {
    jCheckBoxListenerPort.setSelected(true);
    jTextFieldListenerPort.setEnabled(true);
    upListenerPort.setEnabled(true);
    downListenerPort.setEnabled(true);
    jTextFieldListenerPort.setText(text);
  }

  void jTextFieldLabelListFocusLost(FocusEvent e) {
    GraphicUtils.jTextFieldDeselect(jTextFieldLabelList);
  }

  void jListLabelListFocusLost(FocusEvent e) {
    if (e.getOppositeComponent() != jButtonRemove) { // Get component that "has the focus".
      jListLabelList.clearSelection();
    }
  }

  void jTableLabelListFocusLost(FocusEvent e) {
    /*if (e.getOppositeComponent() != jButtonRemove) { // Get component that "has the focus".
     jTableLabelList.clearSelection();
     }*/
    /*System.out.println("-----" + e.getOppositeComponent());
     System.out.println("-----" + e.getOppositeComponent().getParent());
     if ((e.getOppositeComponent().getParent() != jTableLabelList)
     && (e.getOppositeComponent().getParent() != jScrollPaneLabelList)) {*/
    //jTableLabelList.getCellEditor().cancelCellEditing();
    //}
  }

  void jCheckBoxListenerPortEvent(ActionEvent e) {
    String listenerPort = jTextFieldListenerPort.getText().trim();
    if (listenerPort.equals("")) {
      jTextFieldListenerPort
          .setText(Integer.toString(Utils.SHADOWPORT_DEF_VAL));
    }
    if (jCheckBoxListenerPort.isSelected()) {
      setListenerPortEnabled(true);
      jTextFieldListenerPort.selectAll();
      jTextFieldListenerPort.grabFocus();
    } else {
      setListenerPortEnabled(false);
    }
  }

  void jRadioButtonLabelListEvent(ActionEvent e) {
    jPanelJobStepsNumeric.setEnabled(false);
    setNumberOfStepsEnabled(false);
    setCurrentStepEnabled(false);
    jPanelJobStepsList.setEnabled(true);
    setLabelListEnabled(true);
    if (jListLabelList.getModel().getSize() != 0) {
      setCurrentIndexEnabled(true);
    }
  }

  void jRadioButtonNumericValueEvent(ActionEvent e) {
    jPanelJobStepsList.setEnabled(false);
    setLabelListEnabled(false);
    setCurrentIndexEnabled(false);
    jPanelJobStepsNumeric.setEnabled(true);
    setNumberOfStepsEnabled(true);
    setCurrentStepEnabled(true);
  }

  void setNodeNumberVisible(boolean bool) {
    jPanelNodeNumber.setVisible(bool);
    setNodeNumberEnabled(bool);
  }

  void setListenerPortVisible(boolean bool) {
    jPanelListenerPort.setVisible(bool);
    setListenerPortEnabled(bool);
  }

  void setMode(int mode) {
    this.mode = mode;
    switch (mode) {
      case CHECKPOINTABLE:
        /*jTableLabelList.setVisible(false);
         jListLabelList.setVisible(true);*/
        jScrollPaneLabelList.getViewport().remove(jTableLabelList);
        jScrollPaneLabelList.getViewport().add(jListLabelList, null);
      break;
      case PARTITIONABLE:
        /*jListLabelList.setVisible(false);
         jTableLabelList.setVisible(true);*/
        jScrollPaneLabelList.getViewport().remove(jListLabelList);
        jScrollPaneLabelList.getViewport().add(jTableLabelList, null);
      break;
    }
  }

  boolean checkWeights() {
    int dashCount = 0;
    int numberCount = 0;
    int rowCount = jobTableModel.getRowCount();
    String currentValue;
    for (int i = 0; i < rowCount; i++) {
      currentValue = jobTableModel.getValueAt(i, WEIGHT_COLUMN_INDEX)
          .toString().trim();
      if (currentValue.equals(DASH)) {
        if (numberCount != 0) {
          return false;
        }
        dashCount++;
      } else {
        if (dashCount != 0) {
          return false;
        }
        if (Utils.getValueType(currentValue) != Utils.INTEGER) {
          return false;
        }
        numberCount++;
      }
    }
    return true;
  }
}