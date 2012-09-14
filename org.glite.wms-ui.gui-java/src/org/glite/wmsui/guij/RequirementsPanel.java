/*
 * RequirementsPanel.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.awt.AWTEvent;
import java.awt.Component;
import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.FocusEvent;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.plaf.basic.BasicArrowButton;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Jdl;
import condor.classad.ClassAdParser;
import condor.classad.RecordExpr;

/**
 * Implementation of the RequirementsPanel class.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class RequirementsPanel extends JPanel {
  static Logger logger = Logger.getLogger(JDLEditor.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  String warningMsg = "";

  String errorMsg = "";

  JPanel jPanelRemoteMachineType = new JPanel();

  JLabel jLabelArchitecture = new JLabel();

  JLabel jLabelOSType = new JLabel();

  JLabel jLabelOSVersion = new JLabel();

  JTextField jTextFieldArchitecture = new JTextField();

  JTextField jTextFieldOSType = new JTextField();

  JTextField jTextFieldRuntimeEnvironment = new JTextField();

  JPanel jPanelReqConnectivity = new JPanel();

  JPanel jPanelBenchmark = new JPanel();

  JTextField jTextFieldMinMainMemory = new JTextField();

  JTextField jTextFieldSpecFloat2000 = new JTextField();

  JLabel jLabelLRMSType = new JLabel();

  JPanel jPanelReqQueueManagement = new JPanel();

  JTextField jTextFieldLRMSVersion = new JTextField();

  JLabel jLabelLRMSVersion = new JLabel();

  JTextField jTextFieldLRMSType = new JTextField();

  BasicArrowButton upMinMainMemory = new BasicArrowButton(
      BasicArrowButton.NORTH);

  BasicArrowButton downMinMainMemory = new BasicArrowButton(
      BasicArrowButton.SOUTH);

  BasicArrowButton upSpecFloat2000 = new BasicArrowButton(
      BasicArrowButton.NORTH);

  BasicArrowButton downSpecFloat2000 = new BasicArrowButton(
      BasicArrowButton.SOUTH);

  BasicArrowButton downMinNumberOfFreeCPUs = new BasicArrowButton(
      BasicArrowButton.SOUTH);

  BasicArrowButton upMinNumberOfFreeCPUs = new BasicArrowButton(
      BasicArrowButton.NORTH);

  JTextField jTextFieldMinNumberOfFreeCPUs = new JTextField();

  BasicArrowButton downMaxAvailableCPUTime = new BasicArrowButton(
      BasicArrowButton.SOUTH);

  BasicArrowButton upMaxAvailableCPUTime = new BasicArrowButton(
      BasicArrowButton.NORTH);

  JTextField jTextFieldMaxAvailableCPUTime = new JTextField();

  JPanel jPanelReqMiscellaneous = new JPanel();

  JTextField jTextFieldTimeToTraverseQueue = new JTextField();

  BasicArrowButton upSpecInt2000 = new BasicArrowButton(BasicArrowButton.NORTH);

  BasicArrowButton downTimeToTraverseQueue = new BasicArrowButton(
      BasicArrowButton.SOUTH);

  JTextField jTextFieldSpecInt2000 = new JTextField();

  BasicArrowButton upTimeToTraverseQueue = new BasicArrowButton(
      BasicArrowButton.NORTH);

  BasicArrowButton downSpecInt2000 = new BasicArrowButton(
      BasicArrowButton.SOUTH);

  JLabel jLabelSec1 = new JLabel();

  JLabel jLabelSec2 = new JLabel();

  JLabel jLabelMb = new JLabel();

  JButton jButtonAdvanced = new JButton();

  JPanel jPanelSimple = new JPanel();

  RequirementsAdvancedPanel requirementsAdvancedPanel;

  JCheckBox jCheckBoxInbound = new JCheckBox();

  JCheckBox jCheckBoxOutbound = new JCheckBox();

  JDLEditorInterface jint;

  JCheckBox jCheckBoxMinMainMemory = new JCheckBox();

  JCheckBox jCheckBoxSpecFloat2000 = new JCheckBox();

  JCheckBox jCheckBoxMaxAvailableCPUTime = new JCheckBox();

  JCheckBox jCheckBoxMinNumberOfFreeCPUs = new JCheckBox();

  JCheckBox jCheckBoxTimeToTraverseQueue = new JCheckBox();

  JCheckBox jCheckBoxSpecInt2000 = new JCheckBox();

  Component component;

  JPanel jPanelApplicationSoftware = new JPanel();

  JLabel jLabelRuntimeEnvironment = new JLabel();

  JTextField jTextFieldOSVersion = new JTextField();

  java.util.jar.Attributes exprAttributesName;

  JPanel jPanelStorageRequirements = new JPanel();

  JTextField jTextFieldMinSpaceOnSE = new JTextField();

  BasicArrowButton upMinSpaceOnSE = new BasicArrowButton(BasicArrowButton.NORTH);

  JCheckBox jCheckBoxMinSpaceOnSE = new JCheckBox();

  BasicArrowButton downMinSpaceOnSE = new BasicArrowButton(
      BasicArrowButton.SOUTH);

  public RequirementsPanel(Component component) {
    if (component instanceof JDLEditor) {
      jint = (JDLEditor) component;
      this.component = component;
      requirementsAdvancedPanel = new RequirementsAdvancedPanel(jint);
      /*
       } else if (component instanceof JDLEJInternalFrame) {
       jint = (JDLEJInternalFrame) component;
       this.component = component;
       requirementsAdvancedPanel = new RequirementsAdvancedPanel(jint);
       } else if (component instanceof JDLEJApplet) {
       jint = (JDLEJApplet) component;
       this.component = component;
       requirementsAdvancedPanel = new RequirementsAdvancedPanel(jint);
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
    exprAttributesName = GUIFileSystem.getExprAttributesName();
    enableAttributeFromConfFile();
    setCheckBoxesEnabled(false);
    //LookAndFeel.installColorAndFont(jLabelOpSys, "", "", labelFont);
    jLabelArchitecture.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelArchitecture.setText("Architecture");
    jLabelOSType.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelOSType.setText("OS Type");
    jLabelOSVersion.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelOSVersion.setText("OS Version");
    jTextFieldArchitecture.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldArchitecture);
      }
    });
    jTextFieldOSType.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldOSType);
      }
    });
    jTextFieldRuntimeEnvironment
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            GraphicUtils.jTextFieldDeselect(jTextFieldRuntimeEnvironment);
          }
        });
    downMinMainMemory.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldMinMainMemory, Utils.FLOAT, Float
            .toString(Utils.MIN_MAIN_MEMORY_DEF_VAL),
            Utils.MIN_MAIN_MEMORY_MIN_VAL, Utils.MIN_MAIN_MEMORY_MAX_VAL);
      }
    });
    upMinMainMemory.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldMinMainMemory, Utils.FLOAT, Float
            .toString(Utils.MIN_MAIN_MEMORY_DEF_VAL),
            Utils.MIN_MAIN_MEMORY_MIN_VAL, Utils.MIN_MAIN_MEMORY_MAX_VAL);
      }
    });
    downSpecFloat2000.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldSpecFloat2000, Utils.FLOAT, Float
            .toString(Utils.SPEC_FLOAT_2000_DEF_VAL),
            Utils.SPEC_FLOAT_2000_MIN_VAL, Utils.SPEC_FLOAT_2000_MAX_VAL);
      }
    });
    upSpecFloat2000.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldSpecFloat2000, Utils.FLOAT, Float
            .toString(Utils.SPEC_FLOAT_2000_DEF_VAL),
            Utils.SPEC_FLOAT_2000_MIN_VAL, Utils.SPEC_FLOAT_2000_MAX_VAL);
      }
    });
    jTextFieldMinMainMemory.setText(Float
        .toString(Utils.MIN_MAIN_MEMORY_DEF_VAL));
    jTextFieldMinMainMemory.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldMinMainMemory
        .addFocusListener(new java.awt.event.FocusListener() {
          public void focusLost(FocusEvent e) {
            GraphicUtils.jTextFieldFocusLost(jTextFieldMinMainMemory,
                Utils.FLOAT, Float.toString(Utils.MIN_MAIN_MEMORY_DEF_VAL),
                Utils.MIN_MAIN_MEMORY_MIN_VAL, Utils.MIN_MAIN_MEMORY_MAX_VAL);
          }

          public void focusGained(FocusEvent e) {
          }
        });
    jTextFieldSpecFloat2000.setText(Float
        .toString(Utils.SPEC_FLOAT_2000_DEF_VAL));
    jTextFieldSpecFloat2000.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldSpecFloat2000.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldSpecFloat2000, Utils.FLOAT,
            Float.toString(Utils.SPEC_FLOAT_2000_DEF_VAL),
            Utils.SPEC_FLOAT_2000_MIN_VAL, Utils.SPEC_FLOAT_2000_MAX_VAL);
      }

      public void focusGained(FocusEvent e) {
      }
    });
    jLabelLRMSType.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelLRMSType.setText("LRMS Type");
    jTextFieldLRMSVersion.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldLRMSVersion);
      }
    });
    jLabelLRMSVersion.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelLRMSVersion.setText("LRMS Version");
    jTextFieldLRMSType.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldLRMSType);
      }
    });
    downMinNumberOfFreeCPUs
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            Utils.downButtonEvent(jTextFieldMinNumberOfFreeCPUs, Utils.INTEGER,
                Integer.toString(Utils.MIN_NUMBER_OF_FREE_CPUS_DEF_VAL),
                Utils.MIN_NUMBER_OF_FREE_CPUS_MIN_VAL,
                Utils.MIN_NUMBER_OF_FREE_CPUS_MAX_VAL);
          }
        });
    upMinNumberOfFreeCPUs
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            Utils.upButtonEvent(jTextFieldMinNumberOfFreeCPUs, Utils.INTEGER,
                Integer.toString(Utils.MIN_NUMBER_OF_FREE_CPUS_DEF_VAL),
                Utils.MIN_NUMBER_OF_FREE_CPUS_MIN_VAL,
                Utils.MIN_NUMBER_OF_FREE_CPUS_MAX_VAL);
          }
        });
    jTextFieldMinNumberOfFreeCPUs
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            GraphicUtils.jTextFieldFocusLost(jTextFieldMinNumberOfFreeCPUs,
                Utils.INTEGER, Integer
                    .toString(Utils.MIN_NUMBER_OF_FREE_CPUS_DEF_VAL),
                Utils.MIN_NUMBER_OF_FREE_CPUS_MIN_VAL,
                Utils.MIN_NUMBER_OF_FREE_CPUS_MAX_VAL);
          }

          public void focusGained(FocusEvent e) {
          }
        });
    jTextFieldMinNumberOfFreeCPUs.setText(Integer
        .toString(Utils.MIN_NUMBER_OF_FREE_CPUS_DEF_VAL));
    jTextFieldMinNumberOfFreeCPUs.setHorizontalAlignment(SwingConstants.RIGHT);
    downMaxAvailableCPUTime
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            Utils.downButtonEvent(jTextFieldMaxAvailableCPUTime, Utils.FLOAT,
                Float.toString(Utils.MAX_AVAILABLE_CPU_TIME_DEF_VAL),
                Utils.MAX_AVAILABLE_CPU_TIME_MIN_VAL,
                Utils.MAX_AVAILABLE_CPU_TIME_MAX_VAL);
          }
        });
    upMaxAvailableCPUTime
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            Utils.upButtonEvent(jTextFieldMaxAvailableCPUTime, Utils.FLOAT,
                Float.toString(Utils.MAX_AVAILABLE_CPU_TIME_DEF_VAL),
                Utils.MAX_AVAILABLE_CPU_TIME_MIN_VAL,
                Utils.MAX_AVAILABLE_CPU_TIME_MAX_VAL);
          }
        });
    jTextFieldMaxAvailableCPUTime
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            GraphicUtils.jTextFieldFocusLost(jTextFieldMaxAvailableCPUTime,
                Utils.FLOAT, Float
                    .toString(Utils.MAX_AVAILABLE_CPU_TIME_DEF_VAL),
                Utils.MAX_AVAILABLE_CPU_TIME_MIN_VAL,
                Utils.MAX_AVAILABLE_CPU_TIME_MAX_VAL);
          }

          public void focusGained(FocusEvent e) {
          }
        });
    jTextFieldMaxAvailableCPUTime.setText(Float
        .toString(Utils.MAX_AVAILABLE_CPU_TIME_DEF_VAL));
    jTextFieldMaxAvailableCPUTime.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldTimeToTraverseQueue.setText(Float
        .toString(Utils.TIME_TO_TRAVERSE_QUEUE_DEF_VAL));
    jTextFieldTimeToTraverseQueue.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldTimeToTraverseQueue
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            GraphicUtils.jTextFieldFocusLost(jTextFieldTimeToTraverseQueue,
                Utils.FLOAT, Float
                    .toString(Utils.TIME_TO_TRAVERSE_QUEUE_DEF_VAL),
                Utils.TIME_TO_TRAVERSE_QUEUE_MIN_VAL,
                Utils.TIME_TO_TRAVERSE_QUEUE_MAX_VAL);
          }

          public void focusGained(FocusEvent e) {
          }
        });
    upSpecInt2000.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldSpecInt2000, Utils.INTEGER, Integer
            .toString(Utils.SPEC_INT_2000_DEF_VAL),
            Utils.SPEC_INT_2000_MIN_VAL, Utils.SPEC_INT_2000_MAX_VAL);
      }
    });
    downTimeToTraverseQueue
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            Utils.downButtonEvent(jTextFieldTimeToTraverseQueue, Utils.FLOAT,
                Float.toString(Utils.TIME_TO_TRAVERSE_QUEUE_DEF_VAL),
                Utils.TIME_TO_TRAVERSE_QUEUE_MIN_VAL,
                Utils.TIME_TO_TRAVERSE_QUEUE_MAX_VAL);
          }
        });
    jTextFieldSpecInt2000
        .setText(Integer.toString(Utils.SPEC_INT_2000_DEF_VAL));
    jTextFieldSpecInt2000.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldSpecInt2000.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldSpecInt2000, Utils.INTEGER,
            Integer.toString(Utils.SPEC_INT_2000_DEF_VAL),
            Utils.SPEC_INT_2000_MIN_VAL, Utils.SPEC_INT_2000_MAX_VAL);
      }

      public void focusGained(FocusEvent e) {
      }
    });
    upTimeToTraverseQueue
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            Utils.upButtonEvent(jTextFieldTimeToTraverseQueue, Utils.FLOAT,
                Float.toString(Utils.TIME_TO_TRAVERSE_QUEUE_DEF_VAL),
                Utils.TIME_TO_TRAVERSE_QUEUE_MIN_VAL,
                Utils.TIME_TO_TRAVERSE_QUEUE_MAX_VAL);
          }
        });
    downSpecInt2000.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldSpecInt2000, Utils.INTEGER, Integer
            .toString(Utils.SPEC_INT_2000_DEF_VAL),
            Utils.SPEC_INT_2000_MIN_VAL, Utils.SPEC_INT_2000_MAX_VAL);
      }
    });
    jLabelSec1.setText("sec.");
    jLabelSec2.setText("sec.");
    jLabelMb.setText("Mb");
    jButtonAdvanced.setText("Advanced >>");
    jButtonAdvanced.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonAdvancedEvent(e);
      }
    });
    jCheckBoxInbound.setText("Inbound");
    jCheckBoxOutbound.setText("Outbound");
    //jCheckBoxAFSAvailable.setText("AFSAvailable");
    jCheckBoxMinMainMemory.setText("Min Main Memory >");
    jCheckBoxMinMainMemory
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jCheckBoxMinPhysicalMemoryEvent(e);
          }
        });
    jCheckBoxSpecFloat2000
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jCheckBoxMinLocalDiskSpaceEvent(e);
          }
        });
    jCheckBoxSpecFloat2000.setText("Spec Float 2000 >");
    jCheckBoxMaxAvailableCPUTime.setText("Max Available CPU Time >");
    jCheckBoxMaxAvailableCPUTime
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jCheckBoxMaxCPUTimeEvent(e);
          }
        });
    jCheckBoxMinNumberOfFreeCPUs.setText("Min Number of Free CPUs >");
    jCheckBoxMinNumberOfFreeCPUs
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jCheckBoxFreeCPUsEvent(e);
          }
        });
    jCheckBoxTimeToTraverseQueue.setToolTipText("");
    jCheckBoxTimeToTraverseQueue.setText("Time to Traverse Queue <");
    jCheckBoxTimeToTraverseQueue
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jCheckBoxEstimatedTraversalTimeEvent(e);
          }
        });
    jCheckBoxSpecInt2000.setText("Spec Int 2000 >");
    jCheckBoxSpecInt2000.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jCheckBoxAverageSI00Event(e);
      }
    });
    jLabelRuntimeEnvironment.setText("Runtime Environment");
    jLabelRuntimeEnvironment.setHorizontalAlignment(SwingConstants.LEFT);
    jTextFieldOSVersion.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldOSVersion);
      }
    });
    jTextFieldMinSpaceOnSE.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldMinSpaceOnSE, Utils.FLOAT,
            Float.toString(Utils.MIN_SPACE_ON_SE_DEF_VAL),
            Utils.MIN_SPACE_ON_SE_MIN_VAL, Utils.MIN_SPACE_ON_SE_MAX_VAL);
      }

      public void focusGained(FocusEvent e) {
      }
    });
    jTextFieldMinSpaceOnSE.setText(Float
        .toString(Utils.MIN_SPACE_ON_SE_DEF_VAL));
    jTextFieldMinSpaceOnSE.setHorizontalAlignment(SwingConstants.RIGHT);
    upMinSpaceOnSE.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldMinSpaceOnSE, Utils.FLOAT, Float
            .toString(Utils.MIN_SPACE_ON_SE_DEF_VAL),
            Utils.MIN_SPACE_ON_SE_MIN_VAL, Utils.MIN_SPACE_ON_SE_MAX_VAL);
      }
    });
    jCheckBoxMinSpaceOnSE
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jCheckBoxMinSpaceOnSEEvent(e);
          }
        });
    jCheckBoxMinSpaceOnSE.setText("Min Space on SE >");
    downMinSpaceOnSE.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldMinSpaceOnSE, Utils.FLOAT, Float
            .toString(Utils.MIN_SPACE_ON_SE_DEF_VAL),
            Utils.MIN_SPACE_ON_SE_MIN_VAL, Utils.MIN_SPACE_ON_SE_MAX_VAL);
      }
    });
    GridBagLayout gbl = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.insets = new Insets(2, 2, 2, 2);
    jPanelRemoteMachineType.setLayout(gbl);
    jPanelRemoteMachineType.setBorder(new TitledBorder(new EtchedBorder(),
        " Remote Machine Type ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelRemoteMachineType
        .add(jLabelArchitecture, GraphicUtils.setGridBagConstraints(gbc, 0, 0,
            1, 1, 0.0, 0.0, GridBagConstraints.EAST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelRemoteMachineType.add(jTextFieldArchitecture, GraphicUtils
        .setGridBagConstraints(gbc, 1, 0, 1, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jPanelRemoteMachineType
        .add(jLabelOSType, GraphicUtils.setGridBagConstraints(gbc, 0, 1, 1, 1,
            0.0, 0.0, GridBagConstraints.EAST, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    jPanelRemoteMachineType.add(jTextFieldOSType, GraphicUtils
        .setGridBagConstraints(gbc, 1, 1, 1, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jPanelRemoteMachineType
        .add(jLabelOSVersion, GraphicUtils.setGridBagConstraints(gbc, 0, 2, 1,
            1, 0.0, 0.0, GridBagConstraints.EAST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelRemoteMachineType.add(jTextFieldOSVersion, GraphicUtils
        .setGridBagConstraints(gbc, 1, 2, 1, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    // jPanelReqConnectivity
    jPanelReqConnectivity.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelReqConnectivity.setBorder(new TitledBorder(new EtchedBorder(),
        " Connectivity ", 0, 0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelReqConnectivity
        .add(jCheckBoxInbound, GraphicUtils.setGridBagConstraints(gbc, 0, 0, 1,
            1, 0.0, 0.0, GridBagConstraints.WEST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelReqConnectivity
        .add(jCheckBoxOutbound, GraphicUtils.setGridBagConstraints(gbc, 0, 1,
            1, 1, 0.0, 0.0, GridBagConstraints.WEST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    // jPanelBenchmark
    jPanelBenchmark.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelBenchmark.setBorder(new TitledBorder(new EtchedBorder(),
        " Benchmark ", 0, 0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    JPanel jPanelArrowSpecInt = new JPanel();
    jPanelArrowSpecInt.setLayout(new BoxLayout(jPanelArrowSpecInt,
        BoxLayout.Y_AXIS));
    jPanelArrowSpecInt.add(upSpecInt2000, null);
    jPanelArrowSpecInt.add(downSpecInt2000, null);
    /*JPanel jPanelArrowSpecInt = new JPanel();
     jPanelArrowSpecInt.setLayout(gbl);
     GraphicUtils.setDefaultGridBagConstraints(gbc);
     jPanelArrowSpecInt.add(upSpecInt2000, GraphicUtils.setGridBagConstraints(
     gbc, 0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
     GridBagConstraints.BOTH, new Insets(1, 1, 1, 1), 0, 0));
     jPanelArrowSpecInt.add(downSpecInt2000, GraphicUtils.setGridBagConstraints(
     gbc, 0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
     GridBagConstraints.BOTH, null, 0, 0));
     gbc.insets = new Insets(3, 3, 3, 3);
     GraphicUtils.setDefaultGridBagConstraints(gbc);*/
    JPanel jPanelArrowSpecFloat = new JPanel();
    jPanelArrowSpecFloat.setLayout(new BoxLayout(jPanelArrowSpecFloat,
        BoxLayout.Y_AXIS));
    jPanelArrowSpecFloat.add(upSpecFloat2000, null);
    jPanelArrowSpecFloat.add(downSpecFloat2000, null);
    jPanelBenchmark
        .add(jCheckBoxSpecInt2000, GraphicUtils.setGridBagConstraints(gbc, 0,
            0, 1, 1, 0.0, 0.0, GridBagConstraints.WEST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jTextFieldSpecInt2000
        .setPreferredSize(GraphicUtils.NUMERIC_TEXT_FIELD_DIMENSION);
    jPanelBenchmark
        .add(jTextFieldSpecInt2000, GraphicUtils.setGridBagConstraints(gbc, 1,
            0, 1, 1, 0.0, 0.0, GridBagConstraints.EAST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelBenchmark.add(jPanelArrowSpecInt, GraphicUtils.setGridBagConstraints(
        gbc, 2, 0, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.BOTH, null, 0, 0));
    jPanelBenchmark
        .add(jCheckBoxSpecFloat2000, GraphicUtils.setGridBagConstraints(gbc, 0,
            1, 1, 1, 0.0, 0.0, GridBagConstraints.WEST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jTextFieldSpecFloat2000
        .setPreferredSize(GraphicUtils.NUMERIC_TEXT_FIELD_DIMENSION);
    jPanelBenchmark
        .add(jTextFieldSpecFloat2000, GraphicUtils.setGridBagConstraints(gbc,
            1, 1, 1, 1, 0.0, 0.0, GridBagConstraints.EAST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelBenchmark.add(jPanelArrowSpecFloat, GraphicUtils
        .setGridBagConstraints(gbc, 2, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH, null, 0, 0));
    // jPanelReqQueueManagement
    jPanelReqQueueManagement.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelReqQueueManagement.setBorder(new TitledBorder(new EtchedBorder(),
        " Remote Site Job Manager ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelReqQueueManagement
        .add(jLabelLRMSType, GraphicUtils.setGridBagConstraints(gbc, 0, 0, 1,
            1, 0.0, 0.0, GridBagConstraints.EAST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelReqQueueManagement
        .add(jTextFieldLRMSType, GraphicUtils.setGridBagConstraints(gbc, 1, 0,
            1, 1, 1.0, 0.0, GridBagConstraints.WEST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelReqQueueManagement
        .add(jLabelLRMSVersion, GraphicUtils.setGridBagConstraints(gbc, 0, 1,
            1, 1, 0.0, 0.0, GridBagConstraints.EAST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelReqQueueManagement
        .add(jTextFieldLRMSVersion, GraphicUtils.setGridBagConstraints(gbc, 1,
            1, 1, 1, 1.0, 0.0, GridBagConstraints.WEST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    // jPanelReqMiscellaneous
    jPanelReqMiscellaneous.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelReqMiscellaneous.setBorder(new TitledBorder(new EtchedBorder(),
        " Remote Site Parameters ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    JPanel jPanelArrowFreeCPU = new JPanel();
    jPanelArrowFreeCPU.setLayout(new BoxLayout(jPanelArrowFreeCPU,
        BoxLayout.Y_AXIS));
    jPanelArrowFreeCPU.add(upMinNumberOfFreeCPUs, null);
    jPanelArrowFreeCPU.add(downMinNumberOfFreeCPUs, null);
    JPanel jPanelArrowMainMemory = new JPanel();
    jPanelArrowMainMemory.setLayout(new BoxLayout(jPanelArrowMainMemory,
        BoxLayout.Y_AXIS));
    jPanelArrowMainMemory.add(upMinMainMemory, null);
    jPanelArrowMainMemory.add(downMinMainMemory, null);
    JPanel jPanelArrowCPUTimet = new JPanel();
    jPanelArrowCPUTimet.setLayout(new BoxLayout(jPanelArrowCPUTimet,
        BoxLayout.Y_AXIS));
    jPanelArrowCPUTimet.add(upMaxAvailableCPUTime, null);
    jPanelArrowCPUTimet.add(downMaxAvailableCPUTime, null);
    JPanel jPanelArrowTraverseQueue = new JPanel();
    jPanelArrowTraverseQueue.setLayout(new BoxLayout(jPanelArrowTraverseQueue,
        BoxLayout.Y_AXIS));
    jPanelArrowTraverseQueue.add(upTimeToTraverseQueue, null);
    jPanelArrowTraverseQueue.add(downTimeToTraverseQueue, null);
    jPanelReqMiscellaneous
        .add(jCheckBoxMinNumberOfFreeCPUs, GraphicUtils.setGridBagConstraints(
            gbc, 0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.WEST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jTextFieldMinNumberOfFreeCPUs
        .setPreferredSize(GraphicUtils.NUMERIC_TEXT_FIELD_DIMENSION);
    jPanelReqMiscellaneous
        .add(jTextFieldMinNumberOfFreeCPUs, GraphicUtils.setGridBagConstraints(
            gbc, 1, 0, 1, 1, 0.0, 0.0, GridBagConstraints.EAST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelReqMiscellaneous.add(jPanelArrowFreeCPU, GraphicUtils
        .setGridBagConstraints(gbc, 2, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.BOTH, null, 0, 0));
    JPanel jPanelFill = new JPanel();
    jPanelReqMiscellaneous.add(jPanelFill, GraphicUtils.setGridBagConstraints(
        gbc, 3, 0, 1, 1, 1.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.BOTH, null, 0, 0));
    jPanelReqMiscellaneous
        .add(jCheckBoxMinMainMemory, GraphicUtils.setGridBagConstraints(gbc, 4,
            0, 1, 1, 0.0, 0.0, GridBagConstraints.WEST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jTextFieldMinMainMemory
        .setPreferredSize(GraphicUtils.NUMERIC_TEXT_FIELD_DIMENSION);
    jPanelReqMiscellaneous
        .add(jTextFieldMinMainMemory, GraphicUtils.setGridBagConstraints(gbc,
            5, 0, 1, 1, 0.0, 0.0, GridBagConstraints.EAST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelReqMiscellaneous.add(jPanelArrowMainMemory, GraphicUtils
        .setGridBagConstraints(gbc, 6, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.BOTH, null, 0, 0));
    jPanelReqMiscellaneous.add(jLabelMb, GraphicUtils.setGridBagConstraints(
        gbc, 7, 0, 1, 1, 0.0, 0.0, GridBagConstraints.WEST,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelReqMiscellaneous
        .add(jCheckBoxMaxAvailableCPUTime, GraphicUtils.setGridBagConstraints(
            gbc, 0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.WEST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jTextFieldMaxAvailableCPUTime
        .setPreferredSize(GraphicUtils.NUMERIC_TEXT_FIELD_DIMENSION);
    jPanelReqMiscellaneous
        .add(jTextFieldMaxAvailableCPUTime, GraphicUtils.setGridBagConstraints(
            gbc, 1, 1, 1, 1, 0.0, 0.0, GridBagConstraints.EAST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelReqMiscellaneous.add(jPanelArrowCPUTimet, GraphicUtils
        .setGridBagConstraints(gbc, 2, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.BOTH, null, 0, 0));
    jPanelReqMiscellaneous.add(jLabelSec1, GraphicUtils.setGridBagConstraints(
        gbc, 3, 1, 1, 1, 1.0, 0.0, GridBagConstraints.WEST,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelReqMiscellaneous
        .add(jCheckBoxTimeToTraverseQueue, GraphicUtils.setGridBagConstraints(
            gbc, 4, 1, 1, 1, 0.0, 0.0, GridBagConstraints.WEST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jTextFieldTimeToTraverseQueue
        .setPreferredSize(GraphicUtils.NUMERIC_TEXT_FIELD_DIMENSION);
    jPanelReqMiscellaneous
        .add(jTextFieldTimeToTraverseQueue, GraphicUtils.setGridBagConstraints(
            gbc, 5, 1, 1, 1, 0.0, 0.0, GridBagConstraints.EAST,
            GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelReqMiscellaneous.add(jPanelArrowTraverseQueue, GraphicUtils
        .setGridBagConstraints(gbc, 6, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.BOTH, null, 0, 0));
    jPanelReqMiscellaneous.add(jLabelSec2, GraphicUtils.setGridBagConstraints(
        gbc, 7, 1, 1, 1, 0.0, 0.0, GridBagConstraints.WEST,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    // jPanelApplicationSoftware
    jPanelApplicationSoftware.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelApplicationSoftware.setBorder(new TitledBorder(new EtchedBorder(),
        " Application Software ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelApplicationSoftware.add(jLabelRuntimeEnvironment, GraphicUtils
        .setGridBagConstraints(gbc, 0, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jPanelApplicationSoftware.add(jTextFieldRuntimeEnvironment, GraphicUtils
        .setGridBagConstraints(gbc, 1, 0, 1, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    // jPanelStorageRequirements
    jPanelStorageRequirements.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelStorageRequirements.setBorder(new TitledBorder(new EtchedBorder(),
        " Storage Requirements ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    JPanel jPanelArrowMinSpaceOnSE = new JPanel();
    jPanelArrowMinSpaceOnSE.setLayout(new BoxLayout(jPanelArrowMinSpaceOnSE,
        BoxLayout.Y_AXIS));
    jPanelArrowMinSpaceOnSE.add(upMinSpaceOnSE, null);
    jPanelArrowMinSpaceOnSE.add(downMinSpaceOnSE, null);
    jPanelStorageRequirements.add(jCheckBoxMinSpaceOnSE, GraphicUtils
        .setGridBagConstraints(gbc, 0, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jTextFieldMinSpaceOnSE
        .setPreferredSize(GraphicUtils.NUMERIC_TEXT_FIELD_DIMENSION);
    jPanelStorageRequirements.add(jTextFieldMinSpaceOnSE, GraphicUtils
        .setGridBagConstraints(gbc, 1, 0, 1, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jPanelStorageRequirements.add(jPanelArrowMinSpaceOnSE, GraphicUtils
        .setGridBagConstraints(gbc, 2, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH, null, 0, 0));
    // jPanelNorth
    JPanel jPanelNorth = new JPanel();
    ((FlowLayout) jPanelNorth.getLayout()).setAlignment(FlowLayout.RIGHT);
    jPanelNorth.add(jButtonAdvanced);
    // jPanelSimple
    jPanelSimple.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelSimple.add(jPanelRemoteMachineType, GraphicUtils
        .setGridBagConstraints(gbc, 0, 0, 3, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH, null, 0, 0));
    jPanelSimple.add(jPanelReqConnectivity, GraphicUtils.setGridBagConstraints(
        gbc, 4, 0, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.BOTH, null, 0, 0));
    jPanelSimple.add(jPanelBenchmark, GraphicUtils.setGridBagConstraints(gbc,
        0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.BOTH, null, 0, 0));
    jPanelSimple.add(jPanelReqQueueManagement, GraphicUtils
        .setGridBagConstraints(gbc, 1, 1, 5, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH, null, 0, 0));
    jPanelSimple.add(jPanelReqMiscellaneous, GraphicUtils
        .setGridBagConstraints(gbc, 0, 2, 5, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH, null, 0, 0));
    jPanelSimple.add(jPanelApplicationSoftware, GraphicUtils
        .setGridBagConstraints(gbc, 0, 3, 5, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH, null, 0, 0));
    jPanelSimple.add(jPanelStorageRequirements, GraphicUtils
        .setGridBagConstraints(gbc, 0, 4, 2, 1, 0.0, 1.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    // this
    this.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    this.add(jPanelNorth, GraphicUtils.setGridBagConstraints(gbc, 0, 0, 1, 1,
        1.0, 0.0, GridBagConstraints.CENTER, GridBagConstraints.BOTH,
        new Insets(1, 1, 1, 1), 0, 0));
    this.add(jPanelSimple, GraphicUtils.setGridBagConstraints(gbc, 0, 1, 1, 1,
        0.0, 1.0, GridBagConstraints.CENTER, GridBagConstraints.BOTH, null, 0,
        0));
    this.add(requirementsAdvancedPanel, GraphicUtils.setGridBagConstraints(gbc,
        0, 2, 1, 1, 0.0, 1.0, GridBagConstraints.CENTER,
        GridBagConstraints.BOTH, null, 0, 0));
    requirementsAdvancedPanel.setVisible(false);
  }

  public void setRequirementsTree(String expr) {
    if (!expr.trim().toUpperCase().equals(
        GUIGlobalVars.getGUIConfVarRequirements())) {
      requirementsAdvancedPanel.setExprTree(expr);
      jPanelSimple.setVisible(false);
      requirementsAdvancedPanel.setVisible(true);
      showRequirementsAdvancedPanel();
    }
  }

  void showRequirementsAdvancedPanel() {
    jButtonAdvanced.setText("<< Simple");
    jPanelSimple.setVisible(false);
    requirementsAdvancedPanel.setVisible(true);
  }

  void showRequirementsSimplePanel() {
    jButtonAdvanced.setText("Advanced >>");
    requirementsAdvancedPanel.setVisible(false);
    jPanelSimple.setVisible(true);
  }

  void jButtonAdvancedEvent(ActionEvent e) {
    if (jButtonAdvanced.getText().equals("Advanced >>")) {
      //jButtonAdvanced.setText("<< Simple");
      //jPanelSimple.setVisible(false);
      //requirementsAdvancedPanel.setVisible(true);
      /// Changing panel
      String result = jButtonRequirementsViewEvent(false, false, null).trim();
      //if(!result.toUpperCase().equals("REQUIREMENTS = TRUE;")) {
      if (!result.equals("")
          && !result.equals(Jdl.REQUIREMENTS + " = "
              + GUIGlobalVars.getGUIConfVarRequirements() + ";")
          && !result.equals(Jdl.REQUIREMENTS + " = ("
              + GUIGlobalVars.getGUIConfVarRequirements() + ");")) {
        //if(!result.equals(requirementsDefaultValue)) {
        int choice = JOptionPane.showOptionDialog(RequirementsPanel.this,
            "Do you want to import Simple Panel settings in Advanced Panel?"
                + "\n(All previous Advanced Panel settings will be lost)",
            Utils.WARNING_MSG_TXT, JOptionPane.YES_NO_CANCEL_OPTION,
            JOptionPane.WARNING_MESSAGE, null, null, null);
        if (choice == 0) {
          //!!! check for null recordexpr?
          ClassAdParser cap = new ClassAdParser("[" + result + "]");
          RecordExpr requirementsRecordExpr = (RecordExpr) cap.parse();
          result = requirementsRecordExpr.lookup(Jdl.REQUIREMENTS).toString()
              .trim();
          setRequirementsTree(result);
          jButtonAdvanced.setText("<< Simple");
          jPanelSimple.setVisible(false);
          requirementsAdvancedPanel.setVisible(true);
        } else if (choice == 1) {
          jButtonAdvanced.setText("<< Simple");
          jPanelSimple.setVisible(false);
          requirementsAdvancedPanel.setVisible(true);
        }
      } else {
        jButtonAdvanced.setText("<< Simple");
        jPanelSimple.setVisible(false);
        requirementsAdvancedPanel.setVisible(true);
      }
      ///
    } else {
      String result = requirementsAdvancedPanel
          .jButtonRequirementsAdvancedPanelViewEvent(false, false, null).trim();
      //if(!result.toUpperCase().equals("REQUIREMENTS = TRUE;")) {
      if (!result.equals("")
          && !result.equals(Jdl.REQUIREMENTS + " = "
              + GUIGlobalVars.getGUIConfVarRequirements() + ";")
          && !result.equals(Jdl.REQUIREMENTS + " = ("
              + GUIGlobalVars.getGUIConfVarRequirements() + ");")) {
        //if(!result.equals(requirementsDefaultValue)) {
        int choice = JOptionPane
            .showOptionDialog(
                RequirementsPanel.this,
                "JDL file will contain Simple Panel settings\n(Advanced Panel settings will not be lost)",
                Utils.WARNING_MSG_TXT, JOptionPane.OK_CANCEL_OPTION,
                JOptionPane.WARNING_MESSAGE, null, null, null);
        if (choice == 0) {
          jButtonAdvanced.setText("Advanced >>");
          requirementsAdvancedPanel.setVisible(false);
          jPanelSimple.setVisible(true);
        }
      } else {
        jButtonAdvanced.setText("Advanced >>");
        requirementsAdvancedPanel.setVisible(false);
        jPanelSimple.setVisible(true);
      }
    }
    jint.setJTextAreaJDL("");
  }

  String getErrorMsg() {
    return errorMsg;
  }

  String getWarningMsg() {
    return warningMsg;
  }

  void setCheckBoxesEnabled(boolean bool) {
    setMinPhysicalMemoryEnabled(bool);
    setMinLocalDiskSpaceEnabled(bool);
    setMaxCPUTimeEnabled(bool);
    setFreeCPUsEnabled(bool);
    setEstimatedTraversalTimeEnabled(bool);
    setAverageSI00Enabled(bool);
    setMinSpaceOnSEEnabled(bool);
  }

  void setMinPhysicalMemoryEnabled(boolean bool) {
    jTextFieldMinMainMemory.setEnabled(bool);
    upMinMainMemory.setEnabled(bool);
    downMinMainMemory.setEnabled(bool);
  }

  void setMinLocalDiskSpaceEnabled(boolean bool) {
    jTextFieldSpecFloat2000.setEnabled(bool);
    upSpecFloat2000.setEnabled(bool);
    downSpecFloat2000.setEnabled(bool);
  }

  void setMaxCPUTimeEnabled(boolean bool) {
    jTextFieldMaxAvailableCPUTime.setEnabled(bool);
    upMaxAvailableCPUTime.setEnabled(bool);
    downMaxAvailableCPUTime.setEnabled(bool);
  }

  void setFreeCPUsEnabled(boolean bool) {
    jTextFieldMinNumberOfFreeCPUs.setEnabled(bool);
    upMinNumberOfFreeCPUs.setEnabled(bool);
    downMinNumberOfFreeCPUs.setEnabled(bool);
  }

  void setEstimatedTraversalTimeEnabled(boolean bool) {
    jTextFieldTimeToTraverseQueue.setEnabled(bool);
    upTimeToTraverseQueue.setEnabled(bool);
    downTimeToTraverseQueue.setEnabled(bool);
  }

  void setAverageSI00Enabled(boolean bool) {
    jTextFieldSpecInt2000.setEnabled(bool);
    upSpecInt2000.setEnabled(bool);
    downSpecInt2000.setEnabled(bool);
  }

  void setMinSpaceOnSEEnabled(boolean bool) {
    jTextFieldMinSpaceOnSE.setEnabled(bool);
    upMinSpaceOnSE.setEnabled(bool);
    downMinSpaceOnSE.setEnabled(bool);
  }

  void jButtonRequirementsResetEvent(ActionEvent e) {
    jTextFieldArchitecture.setText("");
    jTextFieldOSType.setText("");
    jTextFieldOSVersion.setText("");
    jTextFieldRuntimeEnvironment.setText("");
    jCheckBoxInbound.setSelected(false);
    jCheckBoxOutbound.setSelected(false);
    //jCheckBoxAFSAvailable.setSelected(false);
    jTextFieldLRMSType.setText("");
    jTextFieldLRMSVersion.setText("");
    jTextFieldMinMainMemory.setText(Float
        .toString(Utils.MIN_MAIN_MEMORY_DEF_VAL));
    jCheckBoxMinMainMemory.setSelected(false);
    setMinPhysicalMemoryEnabled(false);
    jTextFieldSpecFloat2000.setText(Float
        .toString(Utils.SPEC_FLOAT_2000_DEF_VAL));
    jCheckBoxSpecFloat2000.setSelected(false);
    setMinLocalDiskSpaceEnabled(false);
    jTextFieldMinNumberOfFreeCPUs.setText(Integer
        .toString(Utils.MIN_NUMBER_OF_FREE_CPUS_DEF_VAL));
    jCheckBoxMinNumberOfFreeCPUs.setSelected(false);
    setFreeCPUsEnabled(false);
    jTextFieldMaxAvailableCPUTime.setText(Float
        .toString(Utils.MAX_AVAILABLE_CPU_TIME_DEF_VAL));
    jCheckBoxMaxAvailableCPUTime.setSelected(false);
    setMaxCPUTimeEnabled(false);
    jTextFieldTimeToTraverseQueue.setText(Float
        .toString(Utils.TIME_TO_TRAVERSE_QUEUE_DEF_VAL));
    jCheckBoxTimeToTraverseQueue.setSelected(false);
    setEstimatedTraversalTimeEnabled(false);
    jTextFieldSpecInt2000
        .setText(Integer.toString(Utils.SPEC_INT_2000_DEF_VAL));
    jCheckBoxSpecInt2000.setSelected(false);
    setAverageSI00Enabled(false);
    jTextFieldMinSpaceOnSE.setText(Float
        .toString(Utils.MIN_SPACE_ON_SE_DEF_VAL));
    jCheckBoxMinSpaceOnSE.setSelected(false);
    setMinSpaceOnSEEnabled(false);
    jint.setJTextAreaJDL("");
    jTextFieldArchitecture.grabFocus();
  }

  void showAdvancedPanelOnly() {
    jButtonAdvanced.setEnabled(false);
    requirementsAdvancedPanel.setAdvancedPanelEnabled(false);
    showRequirementsAdvancedPanel();
  }

  String jButtonRequirementsViewEvent(boolean showWarningMsg,
      boolean showErrorMsg, ActionEvent e) {
    warningMsg = "";
    errorMsg = "";
    String result = "";
    String architectureText = jTextFieldArchitecture.getText().trim();
    if (!architectureText.equals("")) {
      result += " && other." + exprAttributesName.getValue("architecture")
          + " == \"" + architectureText + "\"";
    }
    String opSysText = jTextFieldOSType.getText().trim();
    String oSVersionText = jTextFieldOSVersion.getText().trim();
    if (!opSysText.equals("")) {
      if ((exprAttributesName.getValue("OSTypeAndVersion") != null)
          && !(exprAttributesName.getValue("OSTypeAndVersion").trim()
              .equals(""))) {
        result += " && other."
            + exprAttributesName.getValue("OSTypeAndVersion") + " == \""
            + opSysText + oSVersionText + "\"";
      } else {
        String oSType = exprAttributesName.getValue("OSType");
        if ((oSType != null) && !(oSType.trim().equals(""))) {
          result += " && other." + oSType + " == \"" + opSysText + "\"";
        }
        String oSVersion = exprAttributesName.getValue("OSVersion");
        if (!oSVersionText.equals("") && (oSVersion != null)
            && !(oSVersion.trim().equals(""))) {
          result += " && other." + oSVersion + " == \"" + oSVersionText + "\"";
        }
      }
    } else {
      if (!oSVersionText.equals("")) {
        JOptionPane.showOptionDialog(RequirementsPanel.this,
            "You cannot specify OSVersion without OSType", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        jTextFieldOSType.setText("");
        jTextFieldOSType.grabFocus();
      }
    }
    String runTimeEnvironmentText = jTextFieldRuntimeEnvironment.getText()
        .trim();
    if (!runTimeEnvironmentText.equals("")) {
      result += " && Member(\"" + runTimeEnvironmentText + "\", other."
          + exprAttributesName.getValue("runtimeEnvironment") + ")";
    }
    if (jCheckBoxInbound.isSelected()) {
      result += " && other." + exprAttributesName.getValue("inbound")
          + " == true";
    }
    if (jCheckBoxOutbound.isSelected()) {
      result += " && other." + exprAttributesName.getValue("outbound")
          + " == true";
    }
    String minPhysicalMemoryText = jTextFieldMinMainMemory.getText().trim();
    if (jCheckBoxMinMainMemory.isSelected() == true) {
      result += " && other." + exprAttributesName.getValue("minMainMemory")
          + " > " + minPhysicalMemoryText;
    }
    String specFloat2000 = jTextFieldSpecFloat2000.getText().trim();
    if (jCheckBoxSpecFloat2000.isSelected() == true) {
      result += " && other." + exprAttributesName.getValue("specFloat2000")
          + " > " + specFloat2000;
    }
    String LRMSTypeText = jTextFieldLRMSType.getText().trim();
    if (!(LRMSTypeText.equals(""))) {
      result += " && other." + exprAttributesName.getValue("LRMSType")
          + " == \"" + LRMSTypeText + "\"";
    }
    String LRMSVersionText = jTextFieldLRMSVersion.getText().trim();
    if (!(LRMSVersionText.equals(""))) {
      result += " && other." + exprAttributesName.getValue("LRMSVersion")
          + " == \"" + LRMSVersionText + "\"";
    }
    String maxCPUTimeText = jTextFieldMaxAvailableCPUTime.getText().trim();
    if (jCheckBoxMaxAvailableCPUTime.isSelected() == true) {
      result += " && other."
          + exprAttributesName.getValue("maxAvailableCPUTime") + " > "
          + maxCPUTimeText;
    }
    String freeCPUsText = jTextFieldMinNumberOfFreeCPUs.getText().trim();
    if (jCheckBoxMinNumberOfFreeCPUs.isSelected() == true) {
      result += " && other."
          + exprAttributesName.getValue("minNumberOfFreeCPUs") + " > "
          + freeCPUsText;
    }
    String estimatedTraversalTimeText = jTextFieldTimeToTraverseQueue.getText()
        .trim();
    if (jCheckBoxTimeToTraverseQueue.isSelected() == true) {
      result += " && other."
          + exprAttributesName.getValue("timeToTraverseQueue") + " < "
          + estimatedTraversalTimeText;
    }
    String averageSI00Text = jTextFieldSpecInt2000.getText().trim();
    if (jCheckBoxSpecInt2000.isSelected() == true) {
      result += " && other." + exprAttributesName.getValue("specInt2000")
          + " > " + averageSI00Text;
    }
    if (jCheckBoxMinSpaceOnSE.isSelected() == true) {
      String minSpaceOnSE = jTextFieldMinSpaceOnSE.getText().trim();
      result += " && anyMatch(other.storage.CloseSEs, target.GlueSAStateAvailableSpace > "
          + minSpaceOnSE + ")";
    }
    result = result.trim();
    if (result.equals("")) { //&& defaultRequirements.equals("")) {
      jint.setJTextAreaJDL("");
      return "";
    }
    //!!! Default requirements
    /*
     String defaultRequirements = GUIGlobalVars.getGUIConfVarRequirements();
     if (!result.equals("")) {
     result = result.substring(3);
     if (!result.equals(defaultRequirements)
     && !result.equals("(" + defaultRequirements + ")")) {
     if (!defaultRequirements.equals("")) {
     result += " && " + defaultRequirements;
     }
     }
     } else {
     result = defaultRequirements;
     logger.debug("GUIGlobalVars.getGUIConfVarRequirements(): "
     + defaultRequirements);
     }*/
    result = Jdl.REQUIREMENTS + " = " + result + ";\n";
    warningMsg = ExprChecker.checkResult(result,
        Utils.requirementsAttributeArray);
    errorMsg = errorMsg.trim();
    warningMsg = warningMsg.trim();
    if (!errorMsg.equals("") && showErrorMsg) {
      GraphicUtils.showOptionDialogMsg(RequirementsPanel.this, errorMsg,
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE, null,
          null);
      return "";
    } else {
      if (!warningMsg.equals("") && showWarningMsg) {
        GraphicUtils.showOptionDialogMsg(RequirementsPanel.this, warningMsg,
            Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
            null, null);
      }
      jint.setJTextAreaJDL(result);
    }
    return result;
  }

  void enableAttributeFromConfFile() {
    String architecture = exprAttributesName.getValue("architecture");
    if ((architecture == null) || (architecture.trim().equals(""))) {
      jLabelArchitecture.setEnabled(false);
      jTextFieldArchitecture.setEnabled(false);
    }
    String oSType = exprAttributesName.getValue("OSType");
    if ((oSType == null) || (oSType.trim().equals(""))) {
      jLabelOSType.setEnabled(false);
      jTextFieldOSType.setEnabled(false);
    }
    String oSVersion = exprAttributesName.getValue("OSVersion");
    if ((oSVersion == null) || (oSVersion.trim().equals(""))) {
      jLabelOSVersion.setEnabled(false);
      jTextFieldOSVersion.setEnabled(false);
    }
    String inbound = exprAttributesName.getValue("inbound");
    if ((inbound == null) || (inbound.trim().equals(""))) {
      jCheckBoxInbound.setEnabled(false);
    }
    String outbound = exprAttributesName.getValue("outbound");
    if ((outbound == null) || (outbound.trim().equals(""))) {
      jCheckBoxOutbound.setEnabled(false);
    }
    String specInt2000 = exprAttributesName.getValue("specInt2000");
    if ((specInt2000 == null) || (specInt2000.trim().equals(""))) {
      jCheckBoxSpecInt2000.setEnabled(false);
    }
    String specFloat2000 = exprAttributesName.getValue("specFloat2000");
    if ((specFloat2000 == null) || (specFloat2000.trim().equals(""))) {
      jCheckBoxSpecFloat2000.setEnabled(false);
    }
    String lRMSType = exprAttributesName.getValue("LRMSType");
    if ((lRMSType == null) || (lRMSType.trim().equals(""))) {
      jLabelLRMSType.setEnabled(false);
      jTextFieldLRMSType.setEnabled(false);
    }
    String lRMSVersion = exprAttributesName.getValue("LRMSVersion");
    if ((lRMSVersion == null) || (lRMSVersion.trim().equals(""))) {
      jLabelLRMSVersion.setEnabled(false);
      jTextFieldLRMSVersion.setEnabled(false);
    }
    String minMainMemory = exprAttributesName.getValue("minMainMemory");
    if ((minMainMemory == null) || (minMainMemory.trim().equals(""))) {
      jCheckBoxMinMainMemory.setEnabled(false);
      jLabelMb.setEnabled(false);
    }
    String minNumberOfFreeCPUs = exprAttributesName
        .getValue("minNumberOfFreeCPUs");
    if ((minNumberOfFreeCPUs == null)
        || (minNumberOfFreeCPUs.trim().equals(""))) {
      jCheckBoxMinNumberOfFreeCPUs.setEnabled(false);
    }
    String maxAvailableCPUTime = exprAttributesName
        .getValue("maxAvailableCPUTime");
    if ((maxAvailableCPUTime == null)
        || (maxAvailableCPUTime.trim().equals(""))) {
      jCheckBoxMaxAvailableCPUTime.setEnabled(false);
      jLabelSec1.setEnabled(false);
    }
    String timeToTraverseQueue = exprAttributesName
        .getValue("timeToTraverseQueue");
    if ((timeToTraverseQueue == null)
        || (timeToTraverseQueue.trim().equals(""))) {
      jCheckBoxTimeToTraverseQueue.setEnabled(false);
      jLabelSec2.setEnabled(false);
    }
    String runtimeEnvironment = exprAttributesName
        .getValue("runtimeEnvironment");
    if ((runtimeEnvironment == null) || (runtimeEnvironment.trim().equals(""))) {
      jLabelRuntimeEnvironment.setEnabled(false);
      jTextFieldRuntimeEnvironment.setEnabled(false);
    }
  }

  void jCheckBoxMinPhysicalMemoryEvent(ActionEvent e) {
    if (jCheckBoxMinMainMemory.isSelected() == false) {
      setMinPhysicalMemoryEnabled(false);
    } else {
      setMinPhysicalMemoryEnabled(true);
    }
  }

  void jCheckBoxMinLocalDiskSpaceEvent(ActionEvent e) {
    if (jCheckBoxSpecFloat2000.isSelected() == false) {
      setMinLocalDiskSpaceEnabled(false);
    } else {
      setMinLocalDiskSpaceEnabled(true);
    }
  }

  void jCheckBoxFreeCPUsEvent(ActionEvent e) {
    if (jCheckBoxMinNumberOfFreeCPUs.isSelected() == false) {
      setFreeCPUsEnabled(false);
    } else {
      setFreeCPUsEnabled(true);
    }
  }

  void jCheckBoxMaxCPUTimeEvent(ActionEvent e) {
    if (jCheckBoxMaxAvailableCPUTime.isSelected() == false) {
      setMaxCPUTimeEnabled(false);
    } else {
      setMaxCPUTimeEnabled(true);
    }
  }

  void jCheckBoxAverageSI00Event(ActionEvent e) {
    if (jCheckBoxSpecInt2000.isSelected() == false) {
      setAverageSI00Enabled(false);
    } else {
      setAverageSI00Enabled(true);
    }
  }

  void jCheckBoxEstimatedTraversalTimeEvent(ActionEvent e) {
    if (jCheckBoxTimeToTraverseQueue.isSelected() == false) {
      setEstimatedTraversalTimeEnabled(false);
    } else {
      setEstimatedTraversalTimeEnabled(true);
    }
  }

  void jCheckBoxMinSpaceOnSEEvent(ActionEvent e) {
    if (jCheckBoxMinSpaceOnSE.isSelected() == false) {
      setMinSpaceOnSEEnabled(false);
    } else {
      setMinSpaceOnSEEnabled(true);
    }
  }
}