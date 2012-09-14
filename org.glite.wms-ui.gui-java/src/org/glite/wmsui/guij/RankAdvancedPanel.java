/*
 * RankAdvancedPanel.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.awt.AWTEvent;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.FocusEvent;
import java.util.ArrayList;
import java.util.Vector;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JTree;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.MutableTreeNode;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Jdl;
import condor.classad.ClassAdParser;

/**
 * Implementation of the RankAdvancedPanel class.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class RankAdvancedPanel extends JPanel {
  static Logger logger = Logger.getLogger(JDLEditor.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  String[] operators = { "+", "-", "*", "/", "%"
  };

  String errorMsg = "";

  String warningMsg = "";

  JPanel contentPane;

  JLabel jLabelOperator = new JLabel();

  JComboBox jComboBoxOperators = new JComboBox(operators);

  JTextField jTextFieldValue = new JTextField();

  JScrollPane jScrollPaneTreeExpr = new JScrollPane();

  JButton jButtonAdd = new JButton();

  JButton jButtonRemove = new JButton();

  protected JTree jTreeExpr = null;

  protected DefaultTreeModel dtm = null;

  DefaultMutableTreeNode rootNode = null;

  DefaultMutableTreeNode parentNode = null;

  String allExp = "<html><font color=\"#800080\">" + "<b>Exp</b>" + "</font>";

  String subExp = "<html><font color=\"#602080\">" + "<b>Sub-Exp</b>"
      + "</font>";

  JButton jButtonClear = new JButton();

  JLabel jLabelQuestionMark = new JLabel();

  JPanel jPanelOperand = new JPanel();

  JComboBox jComboBoxAttributes = new JComboBox();

  JRadioButton jRadioButtonAttribute = new JRadioButton();

  JComboBox jComboBoxFunctions = new JComboBox();

  JRadioButton jRadioButtonFunction = new JRadioButton();

  JComboBox jComboBoxParam1 = new JComboBox();

  ButtonGroup buttonGroup = new ButtonGroup();

  JPanel jPanelClassAdTree = new JPanel();

  ArrayList functionsArrayList = new ArrayList();

  ArrayList attributesArrayList = new ArrayList();

  JRadioButton jRadioButtonValue = new JRadioButton();

  JComboBox jComboBoxParam2 = new JComboBox();

  JComboBox jComboBoxParam3 = new JComboBox();

  JDLEditorInterface jint;

  JRadioButton jRadioButtonConditional = new JRadioButton();

  JComboBox jComboBoxCond = new JComboBox();

  JTextField jTextFieldExp1 = new JTextField();

  JTextField jTextFieldExp2 = new JTextField();

  JLabel jLabelThen = new JLabel();

  JLabel jLabelIf = new JLabel();

  JLabel jLabelElse = new JLabel();

  Vector confFileAttributesVector = new Vector();

  Vector confFileFunctionsVector = new Vector();

  Vector attributesNameVector = new Vector();

  RankPanel rank;

  String[] signsArray = { "+", "-"
  };

  JComboBox jComboBoxSign = new JComboBox(signsArray);

  public RankAdvancedPanel(JDLEditorInterface jint, RankPanel rank) {
    this.jint = jint;
    this.rank = rank;
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
    Utils utils = new Utils();
    confFileAttributesVector = GUIFileSystem.getConfigurationAttributes(this);
    confFileFunctionsVector = GUIFileSystem.getConfigurationFunctions(this);
    attributeInitialize();
    functionInitialize();
    functionsSetJCombo(); //!!! Added 22/04/2004
    GUIListCellTooltipRenderer cellRenderer = new GUIListCellTooltipRenderer();
    jComboBoxAttributes.setRenderer(cellRenderer);
    jComboBoxFunctions.setRenderer(cellRenderer);
    jComboBoxParam1.setRenderer(cellRenderer);
    jComboBoxParam2.setRenderer(cellRenderer);
    jComboBoxParam3.setRenderer(cellRenderer);
    jComboBoxCond.setRenderer(cellRenderer);
    jButtonAdd.setText("Add");
    jButtonAdd.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonAddEvent(e);
      }
    });
    String operator = "<html><font color=\"#0000AA\">" + " Operator "
        + "</font>";
    jLabelOperator.setText(operator);
    jComboBoxOperators.setFont(new java.awt.Font("Dialog", 1, 12));
    jTextFieldValue.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldValue);
      }
    });
    jButtonRemove.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonRemoveEvent(e);
      }
    });
    jButtonRemove.setText("Remove");
    jButtonClear.setText("Clear");
    jButtonClear.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonClearEvent(e);
      }
    });
    rootNode = new DefaultMutableTreeNode(allExp);
    dtm = new DefaultTreeModel(rootNode);
    jTreeExpr = new JTree(dtm);
    jTreeExpr.setRootVisible(false);
    jTreeExpr.getSelectionModel().setSelectionMode(
        TreeSelectionModel.SINGLE_TREE_SELECTION);
    DefaultTreeCellRenderer dtcr = new DefaultTreeCellRenderer();
    dtcr.setOpenIcon(null);
    dtcr.setClosedIcon(null);
    dtcr.setLeafIcon(null);
    jTreeExpr.setCellRenderer(dtcr);
    jTreeExpr.putClientProperty("JTree.lineStyle", "Angled");
    jRadioButtonAttribute.setSelected(true);
    jRadioButtonAttribute
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jRadioButtonChangeEvent("Attribute", e);
          }
        });
    jRadioButtonAttribute.setText("Attribute");
    jComboBoxAttributes.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        //jComboBoxAttributesEvent(e);
      }
    });
    jComboBoxFunctions.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        //functionsSetJCombo();
        jComboBoxFunctionsEvent(e);
      }
    });
    jRadioButtonFunction.setText("Function");
    jRadioButtonFunction.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jRadioButtonChangeEvent("Function", e);
      }
    });
    jComboBoxParam1.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jComboBoxParam1Event(e);
      }
    });
    jComboBoxParam1.getEditor().getEditorComponent().addFocusListener(
        new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            jComboBoxParam1FocusLost(e);
          }
        });
    jComboBoxParam1.setEditable(true);
    jRadioButtonValue.setText("Value");
    jRadioButtonValue.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jRadioButtonChangeEvent("Value", e);
      }
    });
    jComboBoxParam2.setEditable(true);
    jComboBoxParam2.getEditor().getEditorComponent().addFocusListener(
        new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            jComboBoxParam2FocusLost(e);
          }
        });
    jComboBoxParam3.setEditable(true);
    jComboBoxParam3.getEditor().getEditorComponent().addFocusListener(
        new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            jComboBoxParam3FocusLost(e);
          }
        });
    jRadioButtonConditional.setText("Conditional");
    jRadioButtonConditional
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jRadioButtonChangeEvent("Conditional", e);
          }
        });
    jComboBoxCond.getEditor().getEditorComponent().addFocusListener(
        new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            jComboBoxCondFocusLost(e);
          }
        });
    jTextFieldExp1.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldExp1);
      }
    });
    jTextFieldExp2.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldExp2);
      }
    });
    jLabelThen.setFont(new java.awt.Font("Dialog", 1, 12));
    jLabelThen.setText("Then");
    jLabelIf.setFont(new java.awt.Font("Dialog", 1, 12));
    jLabelIf.setText("If");
    jLabelElse.setText("Else");
    jLabelElse.setFont(new java.awt.Font("Dialog", 1, 12));
    buttonGroup.add(jRadioButtonAttribute);
    buttonGroup.add(jRadioButtonFunction);
    buttonGroup.add(jRadioButtonConditional);
    buttonGroup.add(jRadioButtonValue);
    jRadioButtonChangeEvent("Attribute", null);
    jScrollPaneTreeExpr.getViewport().add(jTreeExpr, null);
    for (int i = 0; i < attributesArrayList.size(); i++) {
      if (getAttributeType(i) == Utils.BOOLEAN) {
        jComboBoxCond.addItem(getAttributeName(i));
      }
    }
    jComboBoxCond.setEditable(true);
    jComboBoxCond.setSelectedItem("");
    blankJCombo();
    GridBagLayout gbl = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.insets = new Insets(3, 3, 3, 3);
    // jPanelOperand
    jPanelOperand.setLayout(gbl);
    jPanelOperand.setBorder(new TitledBorder(new EtchedBorder(), " Operand ",
        0, 0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelOperand.add(jRadioButtonAttribute, GraphicUtils
        .setGridBagConstraints(gbc, 0, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    jPanelOperand.add(jComboBoxAttributes, GraphicUtils.setGridBagConstraints(
        gbc, 1, 0, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelOperand.add(jComboBoxSign, GraphicUtils.setGridBagConstraints(gbc, 2,
        0, 1, 1, 0.0, 0.0, GridBagConstraints.WEST, GridBagConstraints.NONE,
        null, 0, 0));
    jPanelOperand.add(jRadioButtonFunction, GraphicUtils.setGridBagConstraints(
        gbc, 0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.WEST,
        GridBagConstraints.NONE, null, 0, 0));
    jPanelOperand.add(jComboBoxFunctions, GraphicUtils.setGridBagConstraints(
        gbc, 1, 1, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    JPanel jPanelFunctionParams = new JPanel();
    jPanelFunctionParams.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelFunctionParams.add(jComboBoxParam1, GraphicUtils
        .setGridBagConstraints(gbc, 0, 0, 1, 1, 0.3, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jPanelFunctionParams.add(jComboBoxParam2, GraphicUtils
        .setGridBagConstraints(gbc, 1, 0, 1, 1, 0.3, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jPanelFunctionParams.add(jComboBoxParam3, GraphicUtils
        .setGridBagConstraints(gbc, 2, 0, 1, 1, 0.3, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jPanelOperand.add(jPanelFunctionParams, GraphicUtils.setGridBagConstraints(
        gbc, 0, 2, 4, 1, 1.0, 0.0, GridBagConstraints.WEST,
        GridBagConstraints.NONE, null, 0, 0));
    jLabelIf.setVerticalTextPosition(JLabel.BOTTOM);
    jPanelOperand.add(jLabelIf, GraphicUtils.setGridBagConstraints(gbc, 1, 3,
        1, 1, 0.0, 0.0, GridBagConstraints.LAST_LINE_START,
        GridBagConstraints.NONE, new Insets(0, 3, 0, 3), 0, 0));
    jLabelThen.setVerticalTextPosition(JLabel.BOTTOM);
    jPanelOperand.add(jLabelThen, GraphicUtils.setGridBagConstraints(gbc, 2, 3,
        1, 1, 0.0, 0.0, GridBagConstraints.LAST_LINE_START,
        GridBagConstraints.NONE, null, 0, 0));
    jLabelElse.setVerticalTextPosition(JLabel.BOTTOM);
    jPanelOperand.add(jLabelElse, GraphicUtils.setGridBagConstraints(gbc, 3, 3,
        1, 1, 0.0, 0.0, GridBagConstraints.LAST_LINE_START,
        GridBagConstraints.NONE, null, 0, 0));
    jPanelOperand.add(jRadioButtonConditional, GraphicUtils
        .setGridBagConstraints(gbc, 0, 4, 1, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.NONE, null,
            0, 0));
    jPanelOperand.add(jComboBoxCond, GraphicUtils.setGridBagConstraints(gbc, 1,
        4, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelOperand.add(jTextFieldExp1, GraphicUtils.setGridBagConstraints(gbc,
        2, 4, 1, 1, 0.5, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 4));
    jPanelOperand.add(jTextFieldExp2, GraphicUtils.setGridBagConstraints(gbc,
        3, 4, 1, 1, 0.5, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 4));
    jPanelOperand.add(jRadioButtonValue, GraphicUtils.setGridBagConstraints(
        gbc, 0, 5, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.NONE, new Insets(3, 3, 3, 3), 0, 0));
    jPanelOperand.add(jTextFieldValue, GraphicUtils.setGridBagConstraints(gbc,
        1, 5, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 4));
    JPanel jPanelButton = new JPanel();
    jPanelButton.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelButton.add(jLabelOperator, GraphicUtils.setGridBagConstraints(gbc, 0,
        0, 1, 1, 0.0, 0.0, GridBagConstraints.WEST, GridBagConstraints.NONE,
        null, 0, 0));
    jPanelButton.add(jComboBoxOperators, GraphicUtils.setGridBagConstraints(
        gbc, 1, 0, 1, 1, 0.0, 0.0, GridBagConstraints.WEST,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelButton.add(jButtonAdd, GraphicUtils.setGridBagConstraints(gbc, 2, 0,
        1, 1, 0.0, 0.0, GridBagConstraints.WEST, GridBagConstraints.NONE, null,
        0, 0));
    jPanelButton.add(jButtonRemove, GraphicUtils.setGridBagConstraints(gbc, 3,
        0, 1, 1, 1.0, 0.0, GridBagConstraints.EAST, GridBagConstraints.NONE,
        null, 0, 0));
    jPanelButton.add(jButtonClear, GraphicUtils.setGridBagConstraints(gbc, 4,
        0, 1, 1, 0.0, 0.0, GridBagConstraints.EAST, GridBagConstraints.NONE,
        null, 0, 0));
    jPanelClassAdTree.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelClassAdTree.setBorder(new TitledBorder(new EtchedBorder(),
        " Expression Tree ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelClassAdTree.add(jPanelButton, GraphicUtils.setGridBagConstraints(gbc,
        0, 0, 1, 1, 1.0, 0.0, GridBagConstraints.WEST,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelClassAdTree.add(jScrollPaneTreeExpr, GraphicUtils
        .setGridBagConstraints(gbc, 0, 1, 1, 1, 1.0, 1.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.BOTH, null,
            0, 0));
    // this
    this.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    this.add(jPanelOperand, GraphicUtils.setGridBagConstraints(gbc, 0, 0, 1, 1,
        1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, new Insets(1, 1, 1, 1), 0, 0));
    this.add(jPanelClassAdTree, GraphicUtils.setGridBagConstraints(gbc, 0, 1,
        1, 1, 0.0, 1.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, null, 0, 0));
  }

  // Show a JOptionPane.
  private int showJOptionPane(String msg, String title) {
    int choice = JOptionPane.showOptionDialog(RankAdvancedPanel.this, msg,
        title, JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
        null, null);
    return choice;
  }

  void jButtonAddEvent(ActionEvent e) {
    DefaultMutableTreeNode selectedNode = (DefaultMutableTreeNode) jTreeExpr
        .getLastSelectedPathComponent();
    DefaultMutableTreeNode parentNode = null;
    DefaultMutableTreeNode previousNode = null;
    DefaultMutableTreeNode nextNode = null;
    DefaultMutableTreeNode lastNode = null;
    DefaultMutableTreeNode operatorNode = null;
    Object selectedOperator = jComboBoxOperators.getSelectedItem();
    String selectedOperatorSubText = selectedOperator.toString(); //.substring(0, 2);
    Object selectedFunction = jComboBoxFunctions.getSelectedItem();
    String nodeText = "";
    if (jRadioButtonAttribute.isSelected()) {
      nodeText += "other." + jComboBoxAttributes.getSelectedItem().toString();
      // + selectedRelation.toString() + value;
    } else if (jRadioButtonFunction.isSelected()) {
      String selectedFunctionText = selectedFunction.toString();
      Vector functionParamsVector = getFunctionParameterTypes(selectedFunctionText);
      int functionTextLength = selectedFunctionText.length();
      int index = selectedFunctionText.indexOf("(");
      String functionName = selectedFunctionText.substring(0, index + 1);
      nodeText += functionName;
      int selectedFunctionFirstParameterType = Utils.UNKNOWN;
      if (jComboBoxParam1.isVisible()) {
        selectedFunctionFirstParameterType = Integer.parseInt(
            functionParamsVector.get(0).toString(), 10);
      }
      if (jComboBoxParam1.isVisible()
          && selectedFunctionFirstParameterType == Utils.LIST) {
        String param1 = jComboBoxParam1.getEditor().getItem().toString().trim();
        if (param1.equals("")) {
          showJOptionPane("Operand: first parameter field cannot be blank",
              Utils.WARNING_MSG_TXT);
          if (jComboBoxParam1.getItemCount() != 0) {
            jComboBoxParam1.showPopup();
          }
          return;
        }
        if (!attributesNameVector.contains(param1)) {
          nodeText += param1;
        } else {
          nodeText += "other." + param1;
        }
        if (jComboBoxParam2.isVisible()) {
          String param2 = jComboBoxParam2.getEditor().getItem().toString()
              .trim();
          if (param2.equals("")) {
            showJOptionPane("Operand: second parameter field cannot be blank",
                Utils.WARNING_MSG_TXT);
            if (jComboBoxParam2.getItemCount() != 0) {
              jComboBoxParam2.showPopup();
            }
            return;
          }
          if (!attributesNameVector.contains(param1)) {
            if (!attributesNameVector.contains(param2)) {
              nodeText += ", " + param2;
            } else {
              nodeText += ", " + "other." + param2;
            }
          } else {
            int param2Type = Utils.UNKNOWN;
            if (attributesNameVector.contains(param2)) {
              param2Type = getAttributeType(param2);
            } else {
              if (getAttributeType(param1) == Utils.STRING) {
                param2Type = Utils.STRING;
              } else {
                param2Type = Utils.getValueType(param2);
              }
            }
            if (param2Type == getAttributeType(param1)) {
              if (!attributesNameVector.contains(param2)) {
                if (param2Type == Utils.STRING) {
                  nodeText += ", " + "\"" + param2 + "\"";
                } else {
                  nodeText += ", " + param2;
                }
              } else {
                nodeText += ", " + "other." + param2;
              }
            } else {
              showJOptionPane("Operand: second parameter field type mismatch",
                  Utils.WARNING_MSG_TXT);
              if (jComboBoxParam2.getItemCount() != 0) {
                jComboBoxParam2.showPopup();
              }
              return;
            }
          }
        }
        if (jComboBoxParam3.isVisible()) {
          String param3 = jComboBoxParam3.getEditor().getItem().toString()
              .trim();
          if (param3.equals("")) {
            showJOptionPane("Operand: third parameter field cannot be blank",
                Utils.WARNING_MSG_TXT);
            if (jComboBoxParam3.getItemCount() != 0) {
              jComboBoxParam3.showPopup();
            }
            return;
          }
          if (!attributesNameVector.contains(param1)) {
            if (!attributesNameVector.contains(param3)) {
              nodeText += ", " + param3;
            } else {
              nodeText += ", " + "other." + param3;
            }
          } else {
            int param3Type = Utils.UNKNOWN;
            if (attributesNameVector.contains(param3)) {
              param3Type = getAttributeType(param3);
            } else if (getAttributeType(param1) == Utils.STRING) {
              param3Type = Utils.STRING;
            } else {
              param3Type = Utils.getValueType(param3);
            }
            if (param3Type == getAttributeType(param1)) {
              if (!attributesNameVector.contains(param3)) {
                if (param3Type == Utils.STRING) {
                  nodeText += ", " + "\"" + param3 + "\"";
                } else {
                  nodeText += ", " + param3;
                }
              } else {
                nodeText += ", " + "other." + param3;
              }
            } else {
              showJOptionPane("Operand: third parameter field type mismatch",
                  Utils.WARNING_MSG_TXT);
              if (jComboBoxParam3.getItemCount() != 0) {
                jComboBoxParam3.showPopup();
              }
              return;
            }
          }
        }
        nodeText += ")";
        // This line is added to invert the parameters order of the function
        // when it is Member() or isMember() (first parameter is of LIST type).
        // A new utility method invertFunctionParameter() is necessary.
        if (selectedFunctionFirstParameterType == Utils.LIST) {
          nodeText = Utils.swapFunctionParameters(nodeText);
        }
      } else {
        if (jComboBoxParam1.isVisible()) {
          String param1 = jComboBoxParam1.getEditor().getItem().toString()
              .trim();
          if (param1.equals("")) {
            showJOptionPane("Operand: first parameter field cannot be blank",
                Utils.WARNING_MSG_TXT);
            if (jComboBoxParam1.getItemCount() != 0) {
              jComboBoxParam1.showPopup();
            }
            return;
          } else {
            if (!isParameterTypeMatch(0, param1)) {
              showJOptionPane("Operand: first parameter field type mismatch",
                  Utils.WARNING_MSG_TXT);
              if (jComboBoxParam1.getItemCount() != 0) {
                jComboBoxParam1.showPopup();
              }
              return;
            }
            Vector jComboBoxParam1Vector = new Vector();
            for (int i = 0; i < jComboBoxParam1.getModel().getSize(); i++) {
              jComboBoxParam1Vector.add((String) jComboBoxParam1.getModel()
                  .getElementAt(i));
            }
            if (jComboBoxParam1Vector.contains(param1)) {
              nodeText += "other." + param1;
            } else if (Integer.parseInt(functionParamsVector.get(0).toString(),
                10) == Utils.STRING) {
              nodeText += "\"" + param1 + "\"";
            } else {
              nodeText += parseNumericValue(param1);
            }
          }
        }
        if (jComboBoxParam2.isVisible()) {
          String param2 = jComboBoxParam2.getEditor().getItem().toString()
              .trim();
          if (param2.equals("")) {
            showJOptionPane("Operand: second parameter field cannot be blank",
                Utils.WARNING_MSG_TXT);
            if (jComboBoxParam2.getItemCount() != 0) {
              jComboBoxParam2.showPopup();
            }
            return;
          } else {
            if (!isParameterTypeMatch(1, param2)) {
              showJOptionPane("Operand: second parameter field type mismatch",
                  Utils.WARNING_MSG_TXT);
              if (jComboBoxParam2.getItemCount() != 0) {
                jComboBoxParam2.showPopup();
              }
              return;
            }
            Vector jComboBoxParam2Vector = new Vector();
            for (int i = 0; i < jComboBoxParam2.getModel().getSize(); i++) {
              jComboBoxParam2Vector.add((String) jComboBoxParam2.getModel()
                  .getElementAt(i));
            }
            if (jComboBoxParam2Vector.contains(param2)) {
              nodeText += ", other." + param2;
            } else if (Integer.parseInt(functionParamsVector.get(1).toString(),
                10) == Utils.STRING) {
              nodeText += ", " + "\"" + param2 + "\"";
            } else {
              nodeText += ", " + parseNumericValue(param2);
            }
          }
        }
        if (jComboBoxParam3.isVisible()) {
          String param3 = jComboBoxParam3.getEditor().getItem().toString()
              .trim();
          if (param3.equals("")) {
            showJOptionPane("Operand: third parameter field cannot be blank",
                Utils.WARNING_MSG_TXT);
            if (jComboBoxParam3.getItemCount() != 0) {
              jComboBoxParam3.showPopup();
            }
            return;
          } else {
            if (!isParameterTypeMatch(2, param3)) {
              showJOptionPane("Operand: third parameter field type mismatch",
                  Utils.WARNING_MSG_TXT);
              if (jComboBoxParam3.getItemCount() != 0) {
                jComboBoxParam3.showPopup();
              }
              return;
            }
            Vector jComboBoxParam3Vector = new Vector();
            for (int i = 0; i < jComboBoxParam3.getModel().getSize(); i++) {
              jComboBoxParam3Vector.add((String) jComboBoxParam3.getModel()
                  .getElementAt(i));
            }
            if (jComboBoxParam3Vector.contains(param3)) {
              nodeText += ", other." + param3;
            } else if (Integer.parseInt(functionParamsVector.get(2).toString(),
                10) == Utils.STRING) {
              nodeText += ", " + "\"" + param3 + "\"";
            } else {
              nodeText += ", " + parseNumericValue(param3);
            }
          }
        }
        nodeText += ")";
      }
    } else if (jRadioButtonConditional.isSelected()) {
      String condition = jComboBoxCond.getEditor().getItem().toString().trim();
      Vector jComboBoxCondVector = new Vector();
      for (int i = 0; i < jComboBoxCond.getModel().getSize(); i++) {
        jComboBoxCondVector.add((String) jComboBoxCond.getModel().getElementAt(
            i));
      }
      if (jComboBoxCondVector.contains(condition)) {
        condition = "other." + condition;
      }
      String expr1 = jTextFieldExp1.getText().trim();
      String expr2 = jTextFieldExp2.getText().trim();
      if (condition.equals("") || expr1.equals("") || expr2.equals("")) {
        showJOptionPane("Operand: conditional fields cannot be blank",
            Utils.WARNING_MSG_TXT);
        return;
      } else {
        if (!isFloat(expr1)) {
          jTextFieldExp1.grabFocus();
          showJOptionPane(
              "Operand: first conditional expression field must be numeric",
              Utils.WARNING_MSG_TXT);
          jTextFieldExp1.selectAll();
          return;
        } else {
          expr1 = parseNumericValue(expr1);
        }
        if (!isFloat(expr2)) {
          jTextFieldExp2.grabFocus();
          showJOptionPane(
              "Operand: second conditional expression field must be numeric",
              Utils.WARNING_MSG_TXT);
          jTextFieldExp2.selectAll();
          return;
        } else {
          expr2 = parseNumericValue(expr2);
        }
      }
      nodeText += "(" + condition + " ? " + expr1 + " : " + expr2 + ")";
    } else if (jRadioButtonValue.isSelected()) {
      String value = jTextFieldValue.getText().trim();
      if (value.equals("")) {
        jTextFieldValue.grabFocus();
        showJOptionPane("Operand: value field cannot be blank",
            Utils.WARNING_MSG_TXT);
        jTextFieldValue.selectAll();
        return;
      } else {
        if (!isFloat(value)) {
          jTextFieldValue.grabFocus();
          showJOptionPane("Operand: value field must be numeric",
              Utils.WARNING_MSG_TXT);
          jTextFieldValue.selectAll();
          return;
        } else {
          value = parseNumericValue(value);
        }
        nodeText += value;
      }
    }
    if (jComboBoxSign.getSelectedItem().toString().equals("-")) {
      nodeText = "-" + nodeText;
      jComboBoxSign.setSelectedIndex(0);
    }
    TreePath selectedNodePath = jTreeExpr.getSelectionPath();
    // Check if selected node is a logical operator node
    // (you can't add anything to this node).
    String selectedNodeText = new String("");
    if (selectedNode != null) {
      selectedNodeText = selectedNode.getUserObject().toString();
    }
    if ((selectedNodeText.equals("+")) || (selectedNodeText.equals("-"))
        || (selectedNodeText.equals("/")) || (selectedNodeText.equals("*"))
        || (selectedNodeText.equals("%"))) {
      return;
    }
    // If there's no selection, default node is root node.
    if (selectedNodePath == null) {
      selectedNode = rootNode;
      // If statements have to be in this order!
    }
    if (rootNode.getChildCount() == 0) {
      // Adding first attribute.
      addNode(rootNode, nodeText, true);
    } else if ((rootNode.getChildCount() == 1)
        && (selectedNode.isRoot() || (selectedNode == rootNode.getFirstChild()))) {
      // Selected node is root node or the only child node of the root node.
      // Adding second attribute.
      addNode(rootNode, selectedOperator, true);
      addNode(rootNode, nodeText, true);
      jTreeExpr.setShowsRootHandles(true); // Add the handle (open, close tree) to root node.
      jTreeExpr.setRootVisible(true);
    } else if (selectedNode.isRoot() || selectedNodeText.equals(subExp)) {
      // Selected node is rootNode (Exp) or a Sub-Exp node.
      // Getting tree level operator (maybe you can associate operator to parent node).
      operatorNode = (DefaultMutableTreeNode) selectedNode.getChildAt(1); //Second child.
      String operatorNodeSubText = operatorNode.getUserObject().toString(); //.substring(0, 2);
      // if(operatorNode.getUserObject() == selectedOperator) {
      if (operatorNodeSubText.equals(selectedOperatorSubText)) {
        // Adding the operator in the same tree level.
        addNode(selectedNode, selectedOperator, true);
        addNode(selectedNode, nodeText, true);
      } else {
        // Adding the operator in a new tree level.
        createSubtree(selectedNode);
        addNode(selectedNode, selectedOperator, true);
        addNode(selectedNode, nodeText, true);
        packExprTree(rootNode); // Added 10/09/02.
        expandTree(rootNode); // Added 10/09/02.
      }
    } else if (selectedNode == selectedNode.getParent().getChildAt(0)) {
      // Selected node is the first Attribute in the (sub)tree.
      parentNode = (DefaultMutableTreeNode) selectedNode.getParent();
      operatorNode = (DefaultMutableTreeNode) parentNode
          .getChildAfter(selectedNode);
      String operatorNodeSubText = operatorNode.getUserObject().toString(); //.substring(0, 2);
      //if(operatorNode.getUserObject() == selectedOperator) {
      if (operatorNodeSubText.equals(selectedOperatorSubText)) {
        // Adding attribute with the same logical operator. Add new attribute
        // at the end of (sub)tree.
        parentNode = (DefaultMutableTreeNode) selectedNode.getParent();
        addNode(parentNode, selectedOperator, true);
        addNode(parentNode, nodeText, true);
      } else {
        // Adding attribute with a different logical operator.
        Object tempInfo = selectedNode.getUserObject();
        selectedNode.setUserObject(subExp);
        addNode(selectedNode, tempInfo, true);
        addNode(selectedNode, selectedOperator, true);
        addNode(selectedNode, nodeText, true);
      }
    } else {
      parentNode = (DefaultMutableTreeNode) selectedNode.getParent();
      operatorNode = (DefaultMutableTreeNode) parentNode
          .getChildBefore(selectedNode);
      String operatorNodeSubText = operatorNode.getUserObject().toString(); //.substring(0, 2);
      //if(operatorNode.getUserObject() == selectedOperator) {
      if (operatorNodeSubText.equals(selectedOperatorSubText)) {
        addNode(parentNode, selectedOperator, true);
        addNode(parentNode, nodeText, true);
      } else {
        Object tempInfo = selectedNode.getUserObject();
        selectedNode.setUserObject(subExp);
        addNode(selectedNode, tempInfo, true);
        addNode(selectedNode, selectedOperator, true);
        addNode(selectedNode, nodeText, true);
      }
    }
    jTreeExpr.repaint();
    if (jRadioButtonValue.isSelected()) {
      jTextFieldValue.grabFocus();
      jTextFieldValue.selectAll();
    }
  }

  void jButtonRemoveEvent(ActionEvent e) {
    DefaultMutableTreeNode selectedNode = (DefaultMutableTreeNode) jTreeExpr
        .getLastSelectedPathComponent();
    TreePath selectedNodePath = jTreeExpr.getSelectionPath();
    DefaultMutableTreeNode operatorNode = null;
    DefaultMutableTreeNode selectableNode = null; /// Selection
    TreePath selectableNodePath = null; /// Selection
    int selectableIndex = 0;
    // Check if selected node is a logical operator node
    // (you can't directly remove this node).
    String selectedNodeText = new String("");
    if (selectedNode != null) {
      selectedNodeText = selectedNode.getUserObject().toString();
    }
    if ((selectedNodeText.equals("+")) || (selectedNodeText.equals("-"))
        || (selectedNodeText.equals("/")) || (selectedNodeText.equals("*"))
        || (selectedNodeText.equals("%"))) {
      return;
    }
    if (selectedNodePath == null) {
      int choice = JOptionPane.showOptionDialog(RankAdvancedPanel.this,
          "You have to select a node first", Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
          null, null);
    } else if (selectedNode.isRoot()) {
      int choice = JOptionPane.showOptionDialog(RankAdvancedPanel.this,
          "Selected node is root node\nRemove the whole expression?",
          "Confirm Remove", JOptionPane.YES_NO_OPTION,
          JOptionPane.WARNING_MESSAGE, null, null, null);
      if (choice == 0) {
        removeAllChildrenNodes(rootNode);
        jTreeExpr.setShowsRootHandles(false);
        jTreeExpr.setRootVisible(false);
      }
    } else if ((selectedNode.getParent() == rootNode)
        && (rootNode.getChildCount() == 1)) {
      removeNode(selectedNode);
      jTreeExpr.setSelectionPath(null); /// Selection
    } else {
      parentNode = (DefaultMutableTreeNode) selectedNode.getParent();
      if (selectedNode == parentNode.getChildAt(0)) {
        // Selected node is the first Attribute in the tree.
        operatorNode = (DefaultMutableTreeNode) parentNode
            .getChildAfter(selectedNode);
        /// Selection
        selectableNode = (DefaultMutableTreeNode) parentNode
            .getChildAfter(operatorNode);
        removeNode(selectedNode);
        removeNode(operatorNode);
      } else {
        operatorNode = (DefaultMutableTreeNode) parentNode
            .getChildBefore(selectedNode);
        /// Selection
        if (parentNode.getChildAfter(selectedNode) != null) {
          DefaultMutableTreeNode operatorNodeAfter = (DefaultMutableTreeNode) parentNode
              .getChildAfter(selectedNode);
          selectableNode = (DefaultMutableTreeNode) parentNode
              .getChildAfter(operatorNodeAfter);
        } else {
          selectableNode = (DefaultMutableTreeNode) parentNode
              .getChildBefore(operatorNode);
        }
        selectableNodePath = new TreePath(selectableNode);
        selectableIndex = rootNode.getIndex(selectableNode) - 1;
        removeNode(operatorNode);
        removeNode(selectedNode);
        if (selectableNode.getUserObject().toString().trim().equals("")) {
          removeNode(selectableNode); // The node is a blank node before a sign.
          if (rootNode.getChildCount() == 0) {
            jTreeExpr.setShowsRootHandles(false);
            jTreeExpr.setRootVisible(false);
          }
        }
      }
      if ((parentNode.getChildCount() == 1) && (!parentNode.isRoot())) {
        // Selected node has not siblings. Parent node is not the root node.
        DefaultMutableTreeNode childNode = (DefaultMutableTreeNode) parentNode
            .getFirstChild();
        parentNode.setUserObject(childNode.getUserObject());
        String childNodeText = childNode.getUserObject().toString();
        if (childNodeText.equals(subExp)) {
          int childrenNumber = childNode.getChildCount();
          DefaultMutableTreeNode children[] = new DefaultMutableTreeNode[childrenNumber];
          for (int i = 0; i < childrenNumber; i++) {
            children[i] = (DefaultMutableTreeNode) childNode.getChildAt(i);
          }
          for (int i = 0; i < childrenNumber; i++) {
            removeNode(children[i]);
          }
          for (int i = 0; i < childrenNumber; i++) {
            dtm.insertNodeInto(children[i], parentNode, parentNode
                .getChildCount());
          }
          expandTree(parentNode);
        }
        selectableNode = (DefaultMutableTreeNode) childNode.getParent(); /// Selection
        selectableNodePath = new TreePath(selectableNode); /// Selection
        removeNode(childNode);
      }
    }
    if (rootNode.getChildCount() == 1) {
      // Root node has only a child.
      DefaultMutableTreeNode firstChild = (DefaultMutableTreeNode) rootNode
          .getFirstChild();
      if (firstChild.getUserObject().toString().equals(subExp)) {
        // The only child of root node is a "Sub-Exp" node. This will change
        // childcount value!
        // it will be the child number of the "Sub-Exp" node (at least 3!).
        int childrenNumber = firstChild.getChildCount();
        DefaultMutableTreeNode children[] = new DefaultMutableTreeNode[childrenNumber];
        for (int i = 0; i < childrenNumber; i++) {
          children[i] = (DefaultMutableTreeNode) firstChild.getChildAt(i);
        }
        for (int i = 0; i < childrenNumber; i++) {
          removeNode(children[i]);
        }
        for (int i = 0; i < childrenNumber; i++) {
          dtm.insertNodeInto(children[i], rootNode, rootNode.getChildCount());
        }
        for (int i = 0; i < childrenNumber; i++) {
          jTreeExpr.scrollPathToVisible(new TreePath(children[i].getPath()));
        }
        removeNode(firstChild);
        expandTree(rootNode);
      } else {
        jTreeExpr.setShowsRootHandles(false);
        jTreeExpr.setRootVisible(false);
      }
    }
    packExprTree(rootNode);
  }

  private void jRadioButtonChangeEvent(String choice, ActionEvent e) {
    boolean attribute = false, function = false, conditional = false, value = false;
    if (choice.equals("Attribute")) {
      attribute = true;
    } else if (choice.equals("Function")) {
      function = true;
      functionsSetJCombo();
    } else if (choice.equals("Conditional")) {
      conditional = true;
    } else if (choice.equals("Value")) {
      value = true;
    }
    if (!GUIFileSystem.isConfFileError) { // If isConfFileError AdvancedPanel must be disabled.
      // Attribute
      jComboBoxAttributes.setEnabled(attribute);
      // END Attribute
      // Function
      jComboBoxFunctions.setEnabled(function);
      jComboBoxParam1.setEnabled(function);
      jComboBoxParam2.setEnabled(function);
      jComboBoxParam3.setEnabled(function);
      // END Function
      // Conditional
      jLabelIf.setEnabled(conditional);
      jComboBoxCond.setEnabled(conditional);
      jLabelThen.setEnabled(conditional);
      jTextFieldExp1.setEnabled(conditional);
      jLabelElse.setEnabled(conditional);
      jTextFieldExp2.setEnabled(conditional);
      // END Conditional
      // Value
      jTextFieldValue.setEnabled(value);
      // END Value
    }
  }

  String getErrorMsg() {
    return errorMsg;
  }

  String getWarningMsg() {
    return warningMsg;
  }

  private void jButtonClearEvent(ActionEvent e) {
    int choice = JOptionPane.showOptionDialog(RankAdvancedPanel.this,
        "Clear the whole expression?", "Confirm Clear",
        JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null, null,
        null);
    if (choice == 0) {
      removeAllChildrenNodes(rootNode);
      jTreeExpr.setShowsRootHandles(false);
      jTreeExpr.setRootVisible(false);
    }
  }

  //METHODS
  // Compute expression from tree representation.
  private String computeExp(DefaultMutableTreeNode currentNode) {
    int currentNodeChildrenNumber = currentNode.getChildCount();
    String expr = "(";
    if (currentNodeChildrenNumber == 0) {
      expr = "";
    }
    for (int i = 0; i < currentNodeChildrenNumber; i++) {
      DefaultMutableTreeNode childNode = (DefaultMutableTreeNode) currentNode
          .getChildAt(i);
      String childNodeText = childNode.getUserObject().toString();
      if (childNodeText.equals(subExp)) {
        expr = expr + computeExp(childNode);
      } else if (childNodeText.equals("+")) {
        expr = expr + " + ";
      } else if (childNodeText.equals("-")) {
        expr = expr + " - ";
      } else if (childNodeText.equals("*")) {
        expr = expr + " * ";
      } else if (childNodeText.equals("/")) {
        expr = expr + " / ";
      } else if (childNodeText.equals("%")) {
        expr = expr + " % ";
      } else {
        if (childNode != currentNode.getLastChild()) {
          expr = expr + childNodeText;
        } else {
          expr = expr + childNodeText + ")";
        }
      }
    }
    if (currentNode != rootNode) {
      DefaultMutableTreeNode parentNode = (DefaultMutableTreeNode) currentNode
          .getParent();
      String parentNodeText = parentNode.getUserObject().toString();
      if (currentNode == parentNode.getLastChild()
          && parentNodeText.equals(subExp)) {
        expr += ")";
      }
    }
    int rootNodeChildCount = rootNode.getChildCount();
    if (rootNodeChildCount != 0) {
      DefaultMutableTreeNode rootNodeLastChild = (DefaultMutableTreeNode) rootNode
          .getLastChild();
      if (currentNode == rootNode.getLastChild()
          && currentNode.getUserObject().toString().equals(subExp)) {
        expr += ")";
      }
    }
    return expr;
  }

  // Create a node and add it to parent node.
  private DefaultMutableTreeNode addNode(DefaultMutableTreeNode parentNode,
      Object child, boolean visible) {
    DefaultMutableTreeNode childNode = new DefaultMutableTreeNode(child);
    dtm.insertNodeInto(childNode, parentNode, parentNode.getChildCount());
    String childText = child.toString();
    if (childText.equals("+") || childText.equals("-") || childText.equals("*")
        || childText.equals("/") || childText.equals("%")) {
      childNode.setAllowsChildren(false);
    }
    if (visible) {
      jTreeExpr.scrollPathToVisible(new TreePath(childNode.getPath()));
    }
    return childNode;
  }

  // Remove currently selected node.
  private void removeCurrentNode() {
    TreePath currentSelection = jTreeExpr.getSelectionPath();
    if (currentSelection != null) {
      DefaultMutableTreeNode currentNode = (DefaultMutableTreeNode) (currentSelection
          .getLastPathComponent());
      MutableTreeNode parentNode = (MutableTreeNode) (currentNode.getParent());
      if (parentNode != null) {
        dtm.removeNodeFromParent(currentNode);
        return;
      }
    }
  }

  // Remove the node passed as argument.
  private void removeNode(DefaultMutableTreeNode nodeToRemove) {
    if (nodeToRemove != null) {
      MutableTreeNode parentNode = (MutableTreeNode) (nodeToRemove.getParent());
      if (parentNode != null) {
        dtm.removeNodeFromParent(nodeToRemove);
        return;
      }
    }
  }

  // Remove all children of the node passed as argument.
  private void removeAllChildrenNodes(DefaultMutableTreeNode parentNode) {
    int childrenNumber = parentNode.getChildCount();
    for (int i = 0; i < childrenNumber; i++) {
      removeNode((DefaultMutableTreeNode) parentNode.getFirstChild());
    }
  }

  // Create a Sub-Exp sub tree.
  private void createSubtree(DefaultMutableTreeNode node) {
    int childrenNumber = node.getChildCount();
    DefaultMutableTreeNode children[] = new DefaultMutableTreeNode[childrenNumber];
    for (int i = 0; i < childrenNumber; i++) {
      DefaultMutableTreeNode firstChild = (DefaultMutableTreeNode) node
          .getFirstChild();
      children[i] = firstChild;
      removeNode(firstChild);
    }
    DefaultMutableTreeNode childNode = addNode(node, subExp, true);
    for (int i = 0; i < childrenNumber; i++) {
      childNode.add((MutableTreeNode) children[i]);
      //for(int i = 0; i < childrenNumber; i++)
      //  jTreeExpr.scrollPathToVisible(new TreePath(children[i].getPath()));
    }
    expandTree(childNode);
  }

  // Set all nodes in the tree visible.
  private void expandTree(DefaultMutableTreeNode rootNode) {
    int rootNodeChildrenNumber = rootNode.getChildCount();
    if ((rootNodeChildrenNumber == 0) || (rootNodeChildrenNumber == 1)) {
      return;
    }
    for (int i = 0; i < rootNodeChildrenNumber; i++) {
      DefaultMutableTreeNode childNode = (DefaultMutableTreeNode) rootNode
          .getChildAt(i);
      String childNodeText = childNode.getUserObject().toString();
      if (childNodeText.equals(subExp)) {
        expandTree(childNode);
      } else {
        jTreeExpr.scrollPathToVisible(new TreePath(childNode.getPath()));
      }
    }
  }

  // Put subexpression with same operator of upper level expression in same level.
  private int packExprTree(DefaultMutableTreeNode parentNode) {
    if (parentNode == null) {
      parentNode = rootNode;
    }
    int parentNodeChildrenNumber = parentNode.getChildCount();
    if ((parentNodeChildrenNumber == 0) || (parentNodeChildrenNumber == 1)) {
      return 0;
    }
    DefaultMutableTreeNode operatorNode = null;
    String operatorNodeText = null;
    DefaultMutableTreeNode parentNodeParent = null;
    DefaultMutableTreeNode parentNodeParentOperator = null;
    String parentNodeParentOperatorText = null;
    DefaultMutableTreeNode childNode = null;
    String childNodeText = null;
    for (int i = 0; i < parentNodeChildrenNumber; i++) {
      childNode = (DefaultMutableTreeNode) parentNode.getChildAt(i);
      childNodeText = childNode.getUserObject().toString();
      if (childNodeText.equals(subExp)) {
        parentNodeChildrenNumber += packExprTree(childNode);
      }
    }
    operatorNode = (DefaultMutableTreeNode) parentNode.getChildAt(1);
    operatorNodeText = operatorNode.getUserObject().toString(); // .substring(0, 2);
    parentNodeParent = (DefaultMutableTreeNode) parentNode.getParent();
    if (parentNodeParent != null) {
      parentNodeParentOperator = (DefaultMutableTreeNode) parentNodeParent
          .getChildAt(1);
      parentNodeParentOperatorText = parentNodeParentOperator.getUserObject()
          .toString(); // .substring(0, 2);
    }
    if (parentNodeParentOperatorText != null) {
      if (operatorNodeText.equals(parentNodeParentOperatorText)) {
        int childrenNumber = parentNode.getChildCount();
        int position = parentNodeParent.getIndex(parentNode);
        DefaultMutableTreeNode children[] = new DefaultMutableTreeNode[childrenNumber];
        for (int j = 0; j < childrenNumber; j++) {
          children[j] = (DefaultMutableTreeNode) parentNode.getChildAt(j);
        }
        for (int j = 0; j < childrenNumber; j++) {
          removeNode(children[j]);
        }
        removeNode(parentNode);
        for (int j = 0; j < childrenNumber; j++) {
          dtm.insertNodeInto(children[j],
              (DefaultMutableTreeNode) parentNodeParent, position + j);
        }
        return 2;
      }
    }
    return 0;
  }

  // Represent expression with a tree.
  public void setExprTree(String Expr) {
    if (Expr.length() == 0) {
      return;
    }
    /// Changing panel
    removeAllChildrenNodes(rootNode);
    jTreeExpr.setShowsRootHandles(false);
    jTreeExpr.setRootVisible(false);
    ///
    createTreeFromExpr(Expr, rootNode);
    packExprTree(rootNode);
    expandTree(rootNode);
  }

  // Create a tree from expression after checked it.
  private void createTreeFromExpr(String expr, DefaultMutableTreeNode parentNode) {
    int length = expr.length();
    /// Changing panel
    if (length == 0) {
      //!!! in this case if expr is a sub-exp, you can add a blank node!
      // (instead of return).
      return; // maybe it could be useful also in RequirementsAdvanced.
    }
    ///
    if (expr.substring(0, 1).equals("(")) {
      expr = expr.substring(1, length - 1); // Remove first and last parenthesis if present.
      length = expr.length();
    }
    int openedPh = 0;
    boolean isAString = false;
    boolean haveToAdd = false;
    String subExpr1 = null;
    String subExpr2 = null;
    String currentChar = null;
    String operatorType = null;
    for (int i = 0; i < length; i++) {
      currentChar = expr.substring(i, i + 1);
      if (currentChar.equals("\"")) {
        isAString = !isAString;
      }
      if (isAString) {
        continue;
      }
      if (currentChar.equals("(")) {
        openedPh++;
      } else if (currentChar.equals(")")) {
        openedPh--;
      }
      if (openedPh == 0) {
        if (currentChar.equals("+")) {
          subExpr1 = expr.substring(0, i);
          subExpr2 = expr.substring(i + 1, length);
          operatorType = "+";
          haveToAdd = true;
        } else if (currentChar.equals("-")) {
          subExpr1 = expr.substring(0, i);
          subExpr2 = expr.substring(i + 1, length);
          operatorType = "-";
          haveToAdd = true;
        } else if (currentChar.equals("*")) {
          subExpr1 = expr.substring(0, i);
          subExpr2 = expr.substring(i + 1, length);
          operatorType = "*";
          haveToAdd = true;
        } else if (currentChar.equals("/")) {
          subExpr1 = expr.substring(0, i);
          subExpr2 = expr.substring(i + 1, length);
          operatorType = "/";
          haveToAdd = true;
        } else if (currentChar.equals("%")) {
          subExpr1 = expr.substring(0, i);
          subExpr2 = expr.substring(i + 1, length);
          operatorType = "%";
          haveToAdd = true;
        }
        if (haveToAdd) {
          /// Changing panel
          subExpr1 = subExpr1.trim();
          subExpr2 = subExpr2.trim();
          ///
          if (!subExpr1.equals("")) { /// minus
            DefaultMutableTreeNode expr1Node = addNode(parentNode, subExpr1,
                true);
            addNode(parentNode, operatorType, true);
            DefaultMutableTreeNode expr2Node = addNode(parentNode, subExpr2,
                true);
            if (parentNode != rootNode) {
              parentNode.setUserObject(subExp);
            }
            haveToAdd = false;
            createTreeFromExpr(subExpr1, expr1Node);
            createTreeFromExpr(subExpr2, expr2Node);
            /// minus
          } else {
            //if(parentNode != rootNode) parentNode.setUserObject(subExp);
            DefaultMutableTreeNode expr1Node = addNode(parentNode, " ", true);
            addNode(parentNode, operatorType, true);
            DefaultMutableTreeNode expr2Node = addNode(parentNode, subExpr2,
                true);
            if (parentNode != rootNode) {
              parentNode.setUserObject(subExp);
            }
            haveToAdd = false;
            //if (parentNode.getChildCount() != 0) parentNode.setUserObject(subExp);
            //parentNode.setUserObject(operatorType + subExpr2);
            createTreeFromExpr(subExpr1, expr1Node);
            createTreeFromExpr(subExpr2, expr2Node);
          }
          /// END minus
        }
      }
    }
    // Expr does not contain any operator. Add it as the only child of root node.
    // N.B. -> Maybe it is better to check that in setExprTree() method.
    if (rootNode.getChildCount() == 0) {
      addNode(rootNode, expr, true);
    } else {
      jTreeExpr.setRootVisible(true);
      jTreeExpr.setShowsRootHandles(true);
    }
    //packExprTree(rootNode);
  }

  private void functionsSetJCombo() {
    if (jComboBoxFunctions.getItemCount() != 0) {
      String selectedFunctionItem = jComboBoxFunctions.getSelectedItem()
          .toString(); // Function name.
      int selectedFunctionType = getFunctionType(selectedFunctionItem); // Function type.
      Vector selectedFunctionParamTypes = getFunctionParameterTypes(selectedFunctionItem); // Function parameters.
      // Setting items in second operand, first, second, third parameter
      // attributes combo boxes.
      // These items must have the same type as second operand function combo
      // box selected item.
      jComboBoxParam1.removeAllItems();
      jComboBoxParam2.removeAllItems();
      jComboBoxParam3.removeAllItems();
      for (int j = 0; j < confFileFunctionsVector.size(); j++) {
        String functionName = ((Function) confFileFunctionsVector.get(j))
            .getName();
        if (selectedFunctionItem.equals(functionName)) {
          int parameterCount = ((Function) confFileFunctionsVector.get(j))
              .getParameterCount();
          if (parameterCount != 0) {
            int firstParamType = Integer.parseInt(selectedFunctionParamTypes
                .get(0).toString(), 10);
            if (firstParamType == Utils.LIST) {
              for (int i = 0; i < attributesArrayList.size(); i++) {
                if (getAttributeIsMultivalued(i)) {
                  jComboBoxParam1.addItem(getAttributeName(i));
                }
                // add all attributes in the arraylist.
                jComboBoxParam2.addItem(getAttributeName(i));
              }
            } else {
              for (int i = 0; i < attributesArrayList.size(); i++) {
                if (parameterCount >= 1) {
                  if (getAttributeType(i) == Integer.parseInt(
                      selectedFunctionParamTypes.get(0).toString(), 10)) {
                    if (!getAttributeIsMultivalued(i)) {
                      jComboBoxParam1.addItem(getAttributeName(i));
                    }
                  }
                }
                if (parameterCount >= 2) {
                  if (getAttributeType(i) == Integer.parseInt(
                      selectedFunctionParamTypes.get(1).toString(), 10)) {
                    if (!getAttributeIsMultivalued(i)) {
                      jComboBoxParam2.addItem(getAttributeName(i));
                    }
                  }
                }
                if (parameterCount == 3) {
                  if (getAttributeType(i) == Integer.parseInt(
                      selectedFunctionParamTypes.get(2).toString(), 10)) {
                    if (!getAttributeIsMultivalued(i)) {
                      jComboBoxParam3.addItem(getAttributeName(i));
                    }
                  }
                }
              }
            }
          }
          functionsSetParamJText(parameterCount);
          blankJCombo();
          return;
        }
      }
    }
  }

  void jComboBoxAttributesEvent(ActionEvent e) {
    //attributesSetJCombo();
    //attributesSetJRadio();
  }

  private void jComboBoxFunctionsEvent(ActionEvent e) {
    functionsSetJCombo();
  }

  private void functionsSetParamJText(int selectedFunctionParamNumber) {
    switch (selectedFunctionParamNumber) {
      case Utils.INFINITE: //strcat()
      // Show different Frame.
      break;
      case 0: //unixTime()
        jComboBoxParam1.setVisible(false);
        jComboBoxParam2.setVisible(false);
        jComboBoxParam3.setVisible(false);
      break;
      case 1:
        // int(), real(), string(), floor(), ceiling(), round(), timeInterval(),
        // localTimeString(), gmtTimeString()
        jComboBoxParam1.setVisible(true);
        jComboBoxParam2.setVisible(false);
        jComboBoxParam3.setVisible(false);
      break;
      case 2:
        // strcmp(), stricmp(), glob(), iglob()
        jComboBoxParam1.setVisible(true);
        jComboBoxParam2.setVisible(true);
        jComboBoxParam3.setVisible(false);
      break;
      case 3: //substr()
        jComboBoxParam1.setVisible(true);
        jComboBoxParam2.setVisible(true);
        jComboBoxParam3.setVisible(true);
    }
  }

  private int getAttributeType(String attributeName) {
    for (int i = 0; i < attributesArrayList.size(); i++) {
      if ((((Attribute) attributesArrayList.get(i)).getName())
          .equals(attributeName)) {
        return ((Attribute) attributesArrayList.get(i)).getType();
      }
    }
    return -1;
  }

  private String getAttributeName(int index) {
    return ((Attribute) attributesArrayList.get(index)).getName();
  }

  private int getAttributeType(int index) {
    return ((Attribute) attributesArrayList.get(index)).getType();
  }

  private boolean getAttributeIsMultivalued(int index) {
    return ((Attribute) attributesArrayList.get(index)).isMultivalued();
  }

  private String getFunctionName(int index) {
    return ((Function) confFileFunctionsVector.get(index)).getName();
  }

  private int getFunctionType(int index) {
    return ((Function) confFileFunctionsVector.get(index)).getType();
  }

  private int getFunctionType(String functionName) {
    for (int i = 0; i < confFileFunctionsVector.size(); i++) {
      if ((((Function) confFileFunctionsVector.get(i)).getName())
          .equals(functionName)) {
        return ((Function) confFileFunctionsVector.get(i)).getType();
      }
    }
    return -1;
  }

  private int getFunctionParameterCount(String functionName) {
    for (int i = 0; i < confFileFunctionsVector.size(); i++) {
      if ((((Function) confFileFunctionsVector.get(i)).getName())
          .equals(functionName)) {
        return ((Function) confFileFunctionsVector.get(i)).getParameterCount();
      }
    }
    return -1;
  }

  private Vector getFunctionParameterTypes(String functionName) {
    for (int i = 0; i < confFileFunctionsVector.size(); i++) {
      if ((((Function) confFileFunctionsVector.get(i)).getName())
          .equals(functionName)) {
        return ((Function) confFileFunctionsVector.get(i))
            .getParameterTypeArray();
      }
    }
    Vector returnArray = new Vector();
    returnArray.add("-1");
    return returnArray;
  }

  private Vector getFunctionParameterTypes(int index) {
    return ((Function) confFileFunctionsVector.get(index))
        .getParameterTypeArray();
  }

  private void functionInitialize() {
    String currentFunction = "";
    for (int i = 0; i < confFileFunctionsVector.size(); i++) {
      if ((getFunctionType(i) == Utils.INTEGER)
          || (getFunctionType(i) == Utils.FLOAT)
          || (getFunctionType(i) == Utils.BOOLEAN)
          || (getFunctionType(i) == Utils.SECONDS)) {
        currentFunction = ((Function) confFileFunctionsVector.get(i)).getName();
        jComboBoxFunctions.addItem(currentFunction);
      }
    }
  }

  private void attributeInitialize() {
    attributesArrayList.clear();
    for (int i = 0; i < confFileAttributesVector.size(); i++) {
      attributesArrayList.add((Attribute) confFileAttributesVector.get(i));
      attributesNameVector.add(((Attribute) confFileAttributesVector.get(i))
          .getName());
    }
    String currentAttribute = "";
    jComboBoxAttributes.removeAllItems();
    for (int i = 0; i < attributesArrayList.size(); i++) {
      currentAttribute = ((Attribute) attributesArrayList.get(i)).getName();
      if ((getAttributeType(i) == Utils.INTEGER)
          || (getAttributeType(i) == Utils.FLOAT)
          || (getAttributeType(i) == Utils.BOOLEAN)
          || (getAttributeType(i) == Utils.SECONDS)) {
        jComboBoxAttributes.addItem(currentAttribute);
      }
    }
  }

  void jButtonRankAdvancedPanelResetEvent(ActionEvent e) {
    // Reset initial status.
    functionInitialize();
    attributeInitialize();
    blankJCombo();
    jRadioButtonAttribute.setSelected(true);
    jRadioButtonChangeEvent("Attribute", null);
    jTextFieldValue.setText("");
    jComboBoxSign.setSelectedIndex(0);
    if (jComboBoxAttributes.getItemCount() != 0) {
      jComboBoxAttributes.setSelectedIndex(0);
    }
    if (jComboBoxFunctions.getItemCount() != 0) {
      jComboBoxFunctions.setSelectedIndex(0);
    }
    if (jComboBoxOperators.getItemCount() != 0) {
      jComboBoxOperators.setSelectedIndex(0);
    }
    jComboBoxCond.setSelectedItem("");
    jTextFieldExp1.setText("");
    jTextFieldExp2.setText("");
    // Remove expression tree.
    removeAllChildrenNodes(rootNode);
    jTreeExpr.setShowsRootHandles(false);
    jTreeExpr.setRootVisible(false);
    rank.jRadioButtonFuzzyRankFalse.setSelected(true);
    jint.setJTextAreaJDL("");
  }

  void blankJCombo() {
    jComboBoxParam1.setSelectedItem("");
    jComboBoxParam2.setSelectedItem("");
    jComboBoxParam3.setSelectedItem("");
  }

  String jButtonRankAdvancedPanelViewEvent(boolean showWarningMsg,
      boolean showErrorMsg, ActionEvent e) {
    errorMsg = "";
    warningMsg = "";
    String fuzzyResult = "";
    if (rank.jRadioButtonFuzzyRankTrue.isSelected()) {
      fuzzyResult += Jdl.FUZZY_RANK + " = true;\n";
    }
    String result = computeExp(rootNode).trim();
    //String defaultRank = GUIGlobalVars.getGUIConfVarRank();
    if (result.equals("")) {
      /*if (jint.getJobTypeValue().equals(Jdl.JOBTYPE_MPICH)) {
       String defaultRankMPI = GUIGlobalVars.getGUIConfVarRankMPI();
       if (!defaultRankMPI.equals("")) {
       result = fuzzyResult + Jdl.RANK + " = " + defaultRankMPI + ";\n";
       } else {
       if (!defaultRank.equals("")) {
       result = fuzzyResult + Jdl.RANK + " = " + defaultRank + ";\n";
       } else {
       result = fuzzyResult;
       }
       }
       } else {
       if (!defaultRank.equals("")) {
       result = fuzzyResult + Jdl.RANK + " = " + defaultRank + ";\n";
       } else {
       result = fuzzyResult;
       }
       }*/
    } else {
      result = Jdl.RANK + " = " + result + ";\n";
      //!!! Pharenteses inserting.
      ClassAdParser cap = new ClassAdParser("[" + result + "]");
      result = cap.parse().toString();
      int length = result.length();
      result = result.substring(1, length - 1).trim() + ";\n";
      // END
      result = fuzzyResult + result;
    }
    if (result.equals("")) {
      jint.setJTextAreaJDL("");
      return "";
    }
    warningMsg = ExprChecker.checkResult(result, Utils.rankAttributeArray);
    errorMsg = errorMsg.trim();
    warningMsg = warningMsg.trim();
    if (!errorMsg.equals("") && showErrorMsg) {
      GraphicUtils.showOptionDialogMsg(RankAdvancedPanel.this, errorMsg,
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE, null,
          null);
      return "";
    } else {
      if (!warningMsg.equals("") && showWarningMsg) {
        GraphicUtils.showOptionDialogMsg(RankAdvancedPanel.this, warningMsg,
            Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
            null, null);
      }
      jint.setJTextAreaJDL(result);
    }
    return result;
  }

  boolean isParameterTypeMatch(int paramNumber, String param) {
    String item;
    param = param.toUpperCase();
    String selectedFunctionItem = jComboBoxFunctions.getSelectedItem()
        .toString();
    Vector selectedFunctionParamTypes = getFunctionParameterTypes(selectedFunctionItem);
    for (int i = 0; i < attributesArrayList.size(); i++) {
      item = getAttributeName(i).toUpperCase();
      if (param.equals(item)) {
        if (getAttributeType(i) != Integer.parseInt(selectedFunctionParamTypes
            .get(paramNumber).toString())) {
          return false;
        } else {
          return true;
        }
      }
    }
    switch (Integer.parseInt(selectedFunctionParamTypes.get(paramNumber)
        .toString())) {
      case Utils.BOOLEAN:
        if (!param.equals("TRUE") || !param.equals("FALSE")) {
          return false;
        }
      break;
      case Utils.INTEGER:
        try {
          Integer.parseInt(param, 10);
        } catch (NumberFormatException e) {
          return false;
        }
      break;
      case Utils.FLOAT:
        try {
          Float.parseFloat(param);
        } catch (NumberFormatException e) {
          return false;
        }
      break;
    }
    return true;
  }

  boolean isFloat(String value) {
    try {
      Float.parseFloat(value);
    } catch (NumberFormatException e) {
      return false;
    }
    return true;
  }

  boolean isInteger(String value) {
    try {
      Integer.parseInt(value, 10);
    } catch (NumberFormatException e) {
      return false;
    }
    return true;
  }

  boolean isBoolean(String value) {
    value = value.trim().toUpperCase();
    if (value.equals("TRUE") || value.equals("FALSE")) {
      return true;
    }
    return false;
  }

  void jComboBoxParam1FocusLost(FocusEvent e) {
    ((JTextField) jComboBoxParam1.getEditor().getEditorComponent())
        .select(0, 0);
  }

  void jComboBoxParam2FocusLost(FocusEvent e) {
    ((JTextField) jComboBoxParam2.getEditor().getEditorComponent())
        .select(0, 0);
  }

  void jComboBoxParam3FocusLost(FocusEvent e) {
    ((JTextField) jComboBoxParam3.getEditor().getEditorComponent())
        .select(0, 0);
  }

  void jComboBoxCondFocusLost(FocusEvent e) {
    ((JTextField) jComboBoxCond.getEditor().getEditorComponent()).select(0, 0);
  }

  void setAdvancedPanelEnabled(boolean bool) {
    jComboBoxSign.setEnabled(bool);
    jRadioButtonAttribute.setEnabled(bool);
    jComboBoxAttributes.setEnabled(bool);
    jRadioButtonFunction.setEnabled(bool);
    jComboBoxFunctions.setEnabled(bool);
    jComboBoxParam1.setEnabled(bool);
    jComboBoxParam2.setEnabled(bool);
    jComboBoxParam3.setEnabled(bool);
    jRadioButtonConditional.setEnabled(bool);
    jLabelIf.setEnabled(bool);
    jComboBoxCond.setEnabled(bool);
    jLabelThen.setEnabled(bool);
    jTextFieldExp1.setEnabled(bool);
    jLabelElse.setEnabled(bool);
    jTextFieldExp2.setEnabled(bool);
    jRadioButtonValue.setEnabled(bool);
    jTextFieldValue.setEnabled(bool);
    jLabelOperator.setEnabled(bool);
    jComboBoxOperators.setEnabled(bool);
    jButtonAdd.setEnabled(bool);
    jButtonRemove.setEnabled(bool);
    jButtonClear.setEnabled(bool);
  }

  void jComboBoxParam1Event(ActionEvent e) {
    if (jComboBoxFunctions.getItemCount() != 0) {
      String selectedFunctionItem = jComboBoxFunctions.getSelectedItem()
          .toString().trim(); // Function name.
      int selectedFunctionType = getFunctionType(selectedFunctionItem); // Function type.
      Vector selectedFunctionParamTypes = getFunctionParameterTypes(selectedFunctionItem);
      int parameterCount = selectedFunctionParamTypes.size();
      String selectedAttributeParam1 = "";
      if (jComboBoxParam1.getItemCount() != 0) {
        selectedAttributeParam1 = jComboBoxParam1.getEditor().getItem()
            .toString();
      } else {
        return;
      }
      if (Integer.parseInt(selectedFunctionParamTypes.get(0).toString(), 10) == Utils.LIST) {
        int parametersType = getAttributeType(selectedAttributeParam1);
        jComboBoxParam2.removeAllItems();
        jComboBoxParam3.removeAllItems();
        if ((selectedAttributeParam1.equals("") || !attributesNameVector
            .contains(selectedAttributeParam1))) {
          jComboBoxParam2.removeAllItems();
          jComboBoxParam3.removeAllItems();
          for (int i = 0; i < attributesArrayList.size(); i++) {
            if (parameterCount >= 2) {
              jComboBoxParam2.addItem(getAttributeName(i));
            }
            if (parameterCount == 3) {
              jComboBoxParam3.addItem(getAttributeName(i));
            }
          }
        } else {
          for (int i = 0; i < attributesArrayList.size(); i++) {
            if (parameterCount >= 2) {
              if (getAttributeType(i) == parametersType) {
                jComboBoxParam2.addItem(getAttributeName(i));
              }
            }
            if (parameterCount == 3) {
              if (getAttributeType(i) == parametersType) {
                jComboBoxParam3.addItem(getAttributeName(i));
              }
            }
          }
        }
      }
      jComboBoxParam2.setSelectedItem("");
      jComboBoxParam3.setSelectedItem("");
    }
  }

  String parseNumericValue(String value) {
    int valueType = Utils.getValueType(value);
    if (valueType == Utils.FLOAT) {
      float floatValue = Float.parseFloat(value);
      value = (new Float(floatValue)).toString();
    } else if (valueType == Utils.INTEGER) {
      int intValue = Integer.parseInt(value, 10);
      value = (new Integer(intValue)).toString();
    }
    return value;
  }
}