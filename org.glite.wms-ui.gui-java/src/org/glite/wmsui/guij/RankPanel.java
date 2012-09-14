/*
 * RankPanel.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.awt.AWTEvent;
import java.awt.Component;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.border.EmptyBorder;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Jdl;
import condor.classad.ClassAdParser;
import condor.classad.RecordExpr;

/**
 * Implementation of the RankPanel class.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class RankPanel extends JPanel {
  static Logger logger = Logger.getLogger(JDLEditor.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  String errorMsg = "";

  String warningMsg = "";

  JPanel contentPane;

  JPanel jPanelRankingPolicies = new JPanel();

  JRadioButton jRadioButtonGreatestMainMemory = new JRadioButton();

  JRadioButton jRadioButtonShortestTimeToTraverseQueue = new JRadioButton();

  //JButton jButtonRankNuovo = new JButton();
  JButton jButtonAdvanced = new JButton();

  JPanel jPanelSimple = new JPanel();

  RankAdvancedPanel rankAdvancedPanel;

  ButtonGroup buttonGroup = new ButtonGroup();

  ButtonGroup buttonGroupFuzzyRank = new ButtonGroup();

  JDLEditorInterface jint;

  JRadioButton jRadioButtonFuzzyRankFalse = new JRadioButton();

  JPanel jPanelFuzzyRank = new JPanel();

  JRadioButton jRadioButtonFuzzyRankTrue = new JRadioButton();

  Component component;

  JRadioButton jRadioButtonGreatestAvailableCPUTime = new JRadioButton();

  JRadioButton jRadioButtonBestBenchmark = new JRadioButton();

  java.util.jar.Attributes exprAttributesName;

  JRadioButton jRadioButtonMaxNumberOfFreeCPUs = new JRadioButton();

  JRadioButton jRadioButtonMinNumberOfHandledJobs = new JRadioButton();

  String[] bestBenchmarkArray = { "Int", "Float"
  };

  JComboBox jComboBoxBestBenchmark = new JComboBox(bestBenchmarkArray);

  JRadioButton jRadioButtonDataAccessCost = new JRadioButton();

  JCheckBox jCheckBoxRankingPolicies = new JCheckBox();

  JPanel tempPanel = new JPanel();

  public RankPanel(Component component) {
    if (component instanceof JDLEditor) {
      jint = (JDLEditor) component;
      this.component = component;
      rankAdvancedPanel = new RankAdvancedPanel(jint, this);
      /*
       } else if (component instanceof JDLEJInternalFrame) {
       jint = (JDLEJInternalFrame) component;
       this.component = component;
       rankAdvancedPanel = new RankAdvancedPanel(jint, this);
       } else if (component instanceof JDLEJApplet) {
       jint = (JDLEJApplet) component;
       this.component = component;
       rankAdvancedPanel = new RankAdvancedPanel(jint, this);
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
    setRankingPoliciesEnabled(false);
    jRadioButtonFuzzyRankFalse.setSelected(true);
    jRadioButtonShortestTimeToTraverseQueue.setSelected(true);
    jComboBoxBestBenchmark.setVisible(true);
    exprAttributesName = GUIFileSystem.getExprAttributesName();
    enableAttributeFromConfFile();
    jRadioButtonGreatestAvailableCPUTime.setText("Greatest Available CPU Time");
    jRadioButtonBestBenchmark.setText("Best Benchmark");
    jRadioButtonMaxNumberOfFreeCPUs.setText("Max Number of Free CPUs");
    jRadioButtonMinNumberOfHandledJobs.setText("Min Number of Handled Jobs");
    jButtonAdvanced.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonAdvancedEvent(e);
      }
    });
    jRadioButtonDataAccessCost.setText("Best Data Access Cost");
    jCheckBoxRankingPolicies.setText("Ranking Policies");
    jCheckBoxRankingPolicies
        .setForeground(GraphicUtils.TITLED_ETCHED_BORDER_COLOR);
    jCheckBoxRankingPolicies
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jCheckBoxRankingPoliciesEvent(e);
          }
        });
    buttonGroupFuzzyRank.add(jRadioButtonFuzzyRankTrue);
    buttonGroupFuzzyRank.add(jRadioButtonFuzzyRankFalse);
    jRadioButtonGreatestMainMemory.setText("Greatest Main Memory");
    jRadioButtonShortestTimeToTraverseQueue
        .setText("Shortest Time to Traverse Queue");
    jButtonAdvanced.setText("Advanced >>");
    jRadioButtonFuzzyRankFalse.setText("Disabled");
    jRadioButtonFuzzyRankFalse.setSelected(true);
    jRadioButtonFuzzyRankTrue.setText("Enabled");
    buttonGroup.add(jRadioButtonShortestTimeToTraverseQueue);
    buttonGroup.add(jRadioButtonGreatestMainMemory);
    buttonGroup.add(jRadioButtonGreatestAvailableCPUTime);
    buttonGroup.add(jRadioButtonBestBenchmark);
    buttonGroup.add(jRadioButtonMaxNumberOfFreeCPUs);
    buttonGroup.add(jRadioButtonMinNumberOfHandledJobs);
    buttonGroup.add(jRadioButtonDataAccessCost);
    jPanelFuzzyRank.setLayout(new BoxLayout(jPanelFuzzyRank, BoxLayout.X_AXIS));
    jPanelFuzzyRank.setBorder(new TitledBorder(new EtchedBorder(),
        " Stochastic Ranking Policy ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelFuzzyRank.add(jRadioButtonFuzzyRankTrue);
    jPanelFuzzyRank.add(Box.createHorizontalStrut(25));
    jPanelFuzzyRank.add(jRadioButtonFuzzyRankFalse);
    JPanel jPanelNorth = new JPanel();
    jPanelNorth.setLayout(new BoxLayout(jPanelNorth, BoxLayout.X_AXIS));
    jPanelNorth.setBorder(GraphicUtils.SPACING_BORDER);
    jPanelNorth.add(jPanelFuzzyRank, null);
    jPanelNorth.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelNorth.add(Box.createGlue());
    jPanelNorth.add(jButtonAdvanced, null);
    GridBagLayout gbl = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.insets = new Insets(3, 3, 3, 3);
    // jPanelRankingPolicies
    jPanelRankingPolicies.setLayout(gbl);
    jPanelRankingPolicies.add(jRadioButtonShortestTimeToTraverseQueue,
        GraphicUtils.setGridBagConstraints(gbc, 0, 0, 1, 1, 0.5, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    jPanelRankingPolicies.add(jRadioButtonGreatestAvailableCPUTime,
        GraphicUtils.setGridBagConstraints(gbc, 1, 0, 2, 1, 0.5, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    jPanelRankingPolicies.add(jRadioButtonGreatestMainMemory, GraphicUtils
        .setGridBagConstraints(gbc, 0, 1, 1, 1, 0.5, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    jPanelRankingPolicies.add(jRadioButtonBestBenchmark, GraphicUtils
        .setGridBagConstraints(gbc, 1, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    jPanelRankingPolicies.add(jComboBoxBestBenchmark, GraphicUtils
        .setGridBagConstraints(gbc, 2, 1, 1, 1, 0.5, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    jPanelRankingPolicies.add(jRadioButtonMaxNumberOfFreeCPUs, GraphicUtils
        .setGridBagConstraints(gbc, 0, 2, 1, 1, 0.5, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    jPanelRankingPolicies.add(jRadioButtonMinNumberOfHandledJobs, GraphicUtils
        .setGridBagConstraints(gbc, 1, 2, 2, 1, 0.5, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    jPanelRankingPolicies.add(jRadioButtonDataAccessCost, GraphicUtils
        .setGridBagConstraints(gbc, 0, 3, 1, 1, 0.5, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    jPanelSimple = new JPanelCheckBox(jCheckBoxRankingPolicies,
        jPanelRankingPolicies);
    // this
    this.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    this.add(jPanelNorth, GraphicUtils.setGridBagConstraints(gbc, 0, 0, 1, 1,
        1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, new Insets(1, 1, 1, 1), 0, 0));
    this.add(jPanelSimple, GraphicUtils.setGridBagConstraints(gbc, 0, 1, 1, 1,
        0.0, 1.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    this.add(rankAdvancedPanel, GraphicUtils.setGridBagConstraints(gbc, 0, 2,
        1, 1, 0.0, 1.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, null, 0, 0));
    rankAdvancedPanel.setVisible(false);
  }

  void jButtonAdvancedEvent(ActionEvent e) {
    if (jButtonAdvanced.getText().equals("Advanced >>")) {
      /// Changing panel
      String result = jButtonRankViewEvent(false, false, null).trim();
      if (!result.equals("")
          && !result.equals(Jdl.RANK + " = "
              + GUIGlobalVars.getGUIConfVarRank() + ";")
          && !result.equals(Jdl.RANK + " = ("
              + GUIGlobalVars.getGUIConfVarRank() + ");")) {
        int choice = JOptionPane.showOptionDialog(RankPanel.this,
            "Do you want to import Simple Panel settings in Advanced Panel?\n"
                + "(All previous Advanced Panel settings will be lost!)",
            Utils.WARNING_MSG_TXT, JOptionPane.YES_NO_CANCEL_OPTION,
            JOptionPane.WARNING_MESSAGE, null, null, null);
        switch (choice) {
          case 0:
            //!!! check for null recordexpr?
            ClassAdParser cap = new ClassAdParser("[" + result + "]");
            RecordExpr rankRecordExpr = (RecordExpr) cap.parse();
            result = rankRecordExpr.lookup(Jdl.RANK).toString().trim();
            setRankTree(result);
            jButtonAdvanced.setText("<< Simple");
            jPanelSimple.setVisible(false);
            rankAdvancedPanel.setVisible(true);
          break;
          case 1:
            jButtonAdvanced.setText("<< Simple");
            jPanelSimple.setVisible(false);
            rankAdvancedPanel.setVisible(true);
          break;
        }
      } else {
        jButtonAdvanced.setText("<< Simple");
        jPanelSimple.setVisible(false);
        rankAdvancedPanel.setVisible(true);
      }
    } else {
      String result = rankAdvancedPanel.jButtonRankAdvancedPanelViewEvent(
          false, false, null).trim();
      if (!result.equals("")
          && !result.equals(Jdl.RANK + " = "
              + GUIGlobalVars.getGUIConfVarRank() + ";")
          && !result.equals(Jdl.RANK + " = ("
              + GUIGlobalVars.getGUIConfVarRank() + ");")) {
        int choice = JOptionPane.showOptionDialog(RankPanel.this,
            "JDL file will contain Simple Panel settings\n"
                + "(Advanced Panel settings will not be lost)",
            Utils.WARNING_MSG_TXT, JOptionPane.OK_CANCEL_OPTION,
            JOptionPane.WARNING_MESSAGE, null, null, null);
        if (choice == 0) {
          jButtonAdvanced.setText("Advanced >>");
          rankAdvancedPanel.setVisible(false);
          jPanelSimple.setVisible(true);
        }
      } else {
        jButtonAdvanced.setText("Advanced >>");
        rankAdvancedPanel.setVisible(false);
        jPanelSimple.setVisible(true);
      }
    }
    jint.setJTextAreaJDL("");
  }

  void jButtonRankResetEvent(ActionEvent e) {
    jCheckBoxRankingPolicies.setSelected(false);
    setRankingPoliciesEnabled(false);
    jRadioButtonFuzzyRankFalse.setSelected(true);
    jRadioButtonShortestTimeToTraverseQueue.setSelected(true);
    if (jComboBoxBestBenchmark.isVisible()) {
      jComboBoxBestBenchmark.setSelectedIndex(0);
    }
    jint.setJTextAreaJDL("");
  }

  public void setRankTree(String expr) {
    if (!expr.trim().toUpperCase().equals(GUIGlobalVars.getGUIConfVarRank())) {
      rankAdvancedPanel.setExprTree(expr);
      jPanelSimple.setVisible(false);
      rankAdvancedPanel.setVisible(true);
      showRankAdvancedPanel();
    }
  }

  void showRankAdvancedPanel() {
    jButtonAdvanced.setText("<< Simple");
    jPanelSimple.setVisible(false);
    rankAdvancedPanel.setVisible(true);
  }

  void showAdvancedPanelOnly() {
    jRadioButtonFuzzyRankFalse.setEnabled(false);
    jRadioButtonFuzzyRankTrue.setEnabled(false);
    jButtonAdvanced.setEnabled(false);
    rankAdvancedPanel.setAdvancedPanelEnabled(false);
    showRankAdvancedPanel();
  }

  String getErrorMsg() {
    return errorMsg;
  }

  String getWarningMsg() {
    return warningMsg;
  }

  void showRankSimplePanel() {
    jButtonAdvanced.setText("Advanced >>");
    rankAdvancedPanel.setVisible(false);
    jPanelSimple.setVisible(true);
  }

  String jButtonRankViewEvent(boolean showWarningMsg, boolean showErrorMsg,
      ActionEvent e) {
    errorMsg = "";
    warningMsg = "";
    String result = "";
    String fuzzyResult = "";
    if (jRadioButtonFuzzyRankTrue.isSelected()) {
      fuzzyResult += Jdl.FUZZY_RANK + " = true;\n";
    }
    if (jCheckBoxRankingPolicies.isSelected()) {
      if (jRadioButtonShortestTimeToTraverseQueue.isSelected()) {
        result = "- other."
            + exprAttributesName.getValue("timeToTraverseQueue");
      } else if (jRadioButtonGreatestMainMemory.isSelected()) {
        result = "other." + exprAttributesName.getValue("minMainMemory");
      } else if (jRadioButtonGreatestAvailableCPUTime.isSelected()) {
        result = "other." + exprAttributesName.getValue("maxAvailableCPUTime");
      } else if (jRadioButtonBestBenchmark.isSelected()) {
        if (jComboBoxBestBenchmark.isVisible()) {
          String selectedBestBenchmark = jComboBoxBestBenchmark
              .getSelectedItem().toString();
          if (selectedBestBenchmark.equals(bestBenchmarkArray[0])) {
            result = "other." + exprAttributesName.getValue("specInt2000");
          } else {
            result = "other." + exprAttributesName.getValue("specFloat2000");
          }
        } else {
          result = "other." + exprAttributesName.getValue("specInt2000");
        }
      } else if (jRadioButtonMaxNumberOfFreeCPUs.isSelected()) {
        result = "other." + exprAttributesName.getValue("MinNumberOfFreeCPUs");
      } else if (jRadioButtonMinNumberOfHandledJobs.isSelected()) {
        result = "- other."
            + exprAttributesName.getValue("minNumberOfHandledJobs");
      } else if (jRadioButtonDataAccessCost.isSelected()) {
        result = "- other.DataAccessCost";
      }
    }
    result = result.trim();
    /*if (result.equals("")) {
     if (jint.getJobTypeValue().equals(Jdl.JOBTYPE_MPICH)) {
     String rankMPI = GUIGlobalVars.getGUIConfVarRankMPI();
     if (!rankMPI.equals("")) {
     result = rankMPI;
     } else {
     result = GUIGlobalVars.getGUIConfVarRank();
     }
     } else {
     result = GUIGlobalVars.getGUIConfVarRank();
     }
     }*/
    if (!result.equals("")) {
      result = fuzzyResult + Jdl.RANK + " = " + result + ";\n";
    } else {
      result = fuzzyResult;
    }
    if (result.equals("")) {
      jint.setJTextAreaJDL("");
      return "";
    }
    String checkResult = result.equals(fuzzyResult + Jdl.RANK + " = "
        + "other.DataAccessCost" + ";\n") ? (result + jint
        .jButtonDataReqViewEvent(false, false, null)) : result;
    warningMsg = ExprChecker.checkResult(checkResult, Utils.rankAttributeArray);
    errorMsg = errorMsg.trim();
    warningMsg = warningMsg.trim();
    if (!errorMsg.equals("") && showErrorMsg) {
      GraphicUtils.showOptionDialogMsg(RankPanel.this, errorMsg,
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE, null,
          null);
      return "";
    } else {
      if (!warningMsg.equals("") && showWarningMsg) {
        GraphicUtils.showOptionDialogMsg(RankPanel.this, warningMsg,
            Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
            null, null);
      }
      jint.setJTextAreaJDL(result);
    }
    return result;
  }

  void enableAttributeFromConfFile() {
    String shortestTimeToTraverseQueue = exprAttributesName
        .getValue("timeToTraverseQueue");
    if ((shortestTimeToTraverseQueue == null)
        || (shortestTimeToTraverseQueue.trim().equals(""))) {
      jRadioButtonShortestTimeToTraverseQueue.setEnabled(false);
    }
    String greatestAvailableCPUTime = exprAttributesName
        .getValue("maxAvailableCPUTime");
    if ((greatestAvailableCPUTime == null)
        || (greatestAvailableCPUTime.trim().equals(""))) {
      jRadioButtonGreatestAvailableCPUTime.setEnabled(false);
    }
    String greatestMainMemory = exprAttributesName.getValue("minMainMemory");
    if ((greatestMainMemory == null) || (greatestMainMemory.trim().equals(""))) {
      jRadioButtonGreatestMainMemory.setEnabled(false);
    }
    String bestBenchmarkInt = exprAttributesName.getValue("specInt2000");
    if ((bestBenchmarkInt == null) || (bestBenchmarkInt.trim().equals(""))) {
      jRadioButtonBestBenchmark.setEnabled(false);
    } else if ((bestBenchmarkInt != null)
        && !(bestBenchmarkInt.trim().equals(""))) {
      String bestBenchmarkFloat = exprAttributesName.getValue("specFloat2000");
      if ((bestBenchmarkFloat != null)
          && !(bestBenchmarkFloat.trim().equals(""))) {
        jComboBoxBestBenchmark.setVisible(true);
      }
    }
    String MinNumberOfFreeCPUs = exprAttributesName
        .getValue("minNumberOfFreeCPUs");
    if ((MinNumberOfFreeCPUs == null)
        || (MinNumberOfFreeCPUs.trim().equals(""))) {
      jRadioButtonMaxNumberOfFreeCPUs.setEnabled(false);
    }
    String minNumberOfHandledJobs = exprAttributesName
        .getValue("minNumberOfHandledJobs");
    if ((minNumberOfHandledJobs == null)
        || (minNumberOfHandledJobs.trim().equals(""))) {
      jRadioButtonMinNumberOfHandledJobs.setEnabled(false);
    }
  }

  void setFuzzyRankValue(String value) {
    if (value.toUpperCase().equals("TRUE")) {
      jRadioButtonFuzzyRankTrue.setSelected(true);
    } else {
      jRadioButtonFuzzyRankFalse.setSelected(true);
    }
  }

  void setRankingPoliciesEnabled(boolean bool) {
    jRadioButtonShortestTimeToTraverseQueue.setEnabled(bool);
    jRadioButtonGreatestAvailableCPUTime.setEnabled(bool);
    jRadioButtonGreatestMainMemory.setEnabled(bool);
    jRadioButtonBestBenchmark.setEnabled(bool);
    jComboBoxBestBenchmark.setEnabled(bool);
    jRadioButtonMaxNumberOfFreeCPUs.setEnabled(bool);
    jRadioButtonMinNumberOfHandledJobs.setEnabled(bool);
    jRadioButtonDataAccessCost.setEnabled(bool);
  }

  void jCheckBoxRankingPoliciesEvent(ActionEvent e) {
    if (jCheckBoxRankingPolicies.isSelected()) {
      setRankingPoliciesEnabled(true);
    } else {
      setRankingPoliciesEnabled(false);
    }
  }
}
/************************
 * class JPanelCheckBox
 ************************/

class JPanelCheckBox extends JPanel {
  private JPanel innerPanel;

  private Component component;

  public JPanelCheckBox(Component component, JPanel innerPanel) {
    super();
    //this.checkBox = checkBox;
    this.component = component;
    this.innerPanel = innerPanel;
    try {
      jbInit();
    } catch (Exception ex) {
      ex.printStackTrace();
    }
  }

  void jbInit() throws Exception {
    JPanel jPanelFill = new JPanel();
    jPanelFill.setBorder(new EmptyBorder(2, 2, 2, 2));
    GridBagLayout gbl = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.insets = new Insets(0, 3, 3, 3);
    JPanel insidePanel = new JPanel();
    insidePanel.setBorder(new EtchedBorder());
    //insidePanel.setLayout(new BorderLayout());
    insidePanel.setLayout(gbl);
    //insidePanel.add(jPanelFill, BorderLayout.NORTH);
    //insidePanel.add(this.innerPanel, BorderLayout.CENTER);
    insidePanel.add(jPanelFill, GraphicUtils.setGridBagConstraints(gbc, 0, 0,
        1, 1, 1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    insidePanel.add(this.innerPanel, GraphicUtils.setGridBagConstraints(gbc, 0,
        1, 1, 1, 1.0, 1.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, null, 0, 0));
    this.setLayout(gbl);
    //JLayeredPane jPanelLayered = new JLayeredPane();
    //jPanelLayered.add(component, 0);
    //jPanelLayered.add(insidePanel, 1);
    //jPanelLayered.setLayer(insidePanel, 0);
    //jPanelLayered.moveToFront(insidePanel);
    //this.setLayout(new BorderLayout());
    //this.add(jPanelLayered, BorderLayout.CENTER);
    gbc = new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0,
        GridBagConstraints.FIRST_LINE_START, GridBagConstraints.NONE,
        new Insets(0, 10, 0, 0), 0, 0);
    this.add(component, gbc);
    gbc = new GridBagConstraints(0, 0, 1, 1, 1.0, 1.0,
        GridBagConstraints.FIRST_LINE_START, GridBagConstraints.BOTH,
        new Insets(10, 0, 0, 0), 0, 0);
    this.add(insidePanel, gbc);
  }
}