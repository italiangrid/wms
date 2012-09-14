/*
 * RequirementsAdvancedPanel.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.awt.AWTEvent;
import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.FocusEvent;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.SimpleTimeZone;
import java.util.Vector;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JTree;
import javax.swing.SwingConstants;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.plaf.basic.BasicArrowButton;
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
 * Implementation of the RequirementsAdvancedPanel class.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class RequirementsAdvancedPanel extends JPanel {
  static Logger logger = Logger.getLogger(JDLEditor.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int FIRSTOPERAND = 0;

  static final int SECONDOPERAND = 1;

  String warningMsg = "";

  String errorMsg = "";

  String[] operators = { "AND", "AND NOT", "OR", "OR NOT"
  };

  String[] relations = { "==", "!=", "<", ">", "<=", ">="
  };

  JPanel contentPane;

  JLabel jLabelOperator = new JLabel();

  JLabel jLabelRelation = new JLabel();

  JComboBox jComboBoxAttributesFirstOperand = new JComboBox();

  JComboBox jComboBoxOperators = new JComboBox(operators);

  JComboBox jComboBoxRelations = new JComboBox();

  JTextField jTextFieldValueSecondOperand = new JTextField();

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

  JTextField jTextFieldConditionalCond = new JTextField();

  JLabel jLabelQuestionMark = new JLabel();

  JLabel jLabelColon = new JLabel();

  JTextField jTextFieldConditionalExp1 = new JTextField();

  JTextField jTextFieldConditionalExp2 = new JTextField();

  JRadioButton jRadioButtonAttributeFirstOperand = new JRadioButton();

  JRadioButton jRadioButtonFunctionFirstOperand = new JRadioButton();

  JLabel jLabelExp1 = new JLabel();

  JLabel jLabelExp2 = new JLabel();

  JComboBox jComboBoxFunctionsFirstOperand = new JComboBox();

  JComboBox jComboBoxAttributesSecondOperand = new JComboBox();

  JPanel jPanelFirstOperand = new JPanel();

  JPanel jPanelSecondOperand = new JPanel();

  JRadioButton jRadioButtonAttributeSecondOperand = new JRadioButton();

  JComboBox jComboBoxFirstOperandParam1 = new JComboBox();

  JComboBox jComboBoxFunctionsSecondOperand = new JComboBox();

  JRadioButton jRadioButtonFunctionSecondOperand = new JRadioButton();

  JComboBox jComboBoxSecondOperandParam1 = new JComboBox();

  ButtonGroup buttonGroupFirstOperand = new ButtonGroup();

  ButtonGroup buttonGroupSecondOperand = new ButtonGroup();

  JPanel jPanelClassAdTree = new JPanel();

  ArrayList functionsArrayList = new ArrayList();

  ArrayList attributesArrayList = new ArrayList();

  Vector attribute = new Vector();

  JRadioButton jRadioButtonValueSecondOperand = new JRadioButton();

  JComboBox jComboBoxSecondOperandParam2 = new JComboBox();

  JComboBox jComboBoxSecondOperandParam3 = new JComboBox();

  JComboBox jComboBoxFirstOperandParam2 = new JComboBox();

  JComboBox jComboBoxFirstOperandParam3 = new JComboBox();

  JDLEditorInterface jint;

  JCheckBox jCheckBoxNot = new JCheckBox();

  BasicArrowButton downYear = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton upYear = new BasicArrowButton(BasicArrowButton.NORTH);

  JTextField jTextFieldYear = new JTextField();

  BasicArrowButton downMonth = new BasicArrowButton(BasicArrowButton.SOUTH);

  JTextField jTextFieldMonth = new JTextField();

  BasicArrowButton upMonth = new BasicArrowButton(BasicArrowButton.NORTH);

  JTextField jTextFieldDay = new JTextField();

  BasicArrowButton downDay = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton upDay = new BasicArrowButton(BasicArrowButton.NORTH);

  JTextField jTextFieldHours = new JTextField();

  BasicArrowButton downHours = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton upHours = new BasicArrowButton(BasicArrowButton.NORTH);

  JTextField jTextFieldMinutes = new JTextField();

  BasicArrowButton downMinutes = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton upMinutes = new BasicArrowButton(BasicArrowButton.NORTH);

  JTextField jTextFieldSeconds = new JTextField();

  BasicArrowButton downSeconds = new BasicArrowButton(BasicArrowButton.SOUTH);

  BasicArrowButton upSeconds = new BasicArrowButton(BasicArrowButton.NORTH);

  Calendar calendar = Calendar.getInstance();

  JLabel jLabelDate = new JLabel();

  Vector attributesNameVector = new Vector();

  Vector confFileAttributesVector = new Vector();

  Vector confFileFunctionsVector = new Vector();

  public RequirementsAdvancedPanel(JDLEditorInterface jint) {
    this.jint = jint;
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit();
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
  }

  /**Component initialization*/
  private void jbInit() throws Exception {
    isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG) ? true
        : false;
    setTimeTextFieldVisible(false);
    setDateTextFieldVisible(false);
    confFileAttributesVector = GUIFileSystem.getConfigurationAttributes(this);
    confFileFunctionsVector = GUIFileSystem.getConfigurationFunctions(this);
    attributeInitialize();
    functionInitialize(); //Insert in functionsArrayList all function
    //in array functions.
    blankFirstOperandJCombo();
    blankSecondOperandJCombo();
    GUIListCellTooltipRenderer cellRenderer = new GUIListCellTooltipRenderer();
    jComboBoxAttributesFirstOperand.setRenderer(cellRenderer);
    jComboBoxFunctionsFirstOperand.setRenderer(cellRenderer);
    jComboBoxAttributesSecondOperand.setRenderer(cellRenderer);
    jComboBoxFirstOperandParam1.setRenderer(cellRenderer);
    jComboBoxFirstOperandParam2.setRenderer(cellRenderer);
    jComboBoxFirstOperandParam3.setRenderer(cellRenderer);
    jComboBoxFunctionsSecondOperand.setRenderer(cellRenderer);
    jComboBoxSecondOperandParam1.setRenderer(cellRenderer);
    jComboBoxSecondOperandParam2.setRenderer(cellRenderer);
    jComboBoxSecondOperandParam3.setRenderer(cellRenderer);
    for (int i = 0; i < relations.length; i++) {
      jComboBoxRelations.addItem(relations[i]);
    }
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
    jScrollPaneTreeExpr.getViewport().add(jTreeExpr);
    String relation = "<html><font color=\"#0000AA\">" + " Relation "
        + "</font>";
    String operator = "<html><font color=\"#0000AA\">" + " Operator "
        + "</font>";
    jLabelRelation.setText(relation);
    jLabelOperator.setText(operator);
    jCheckBoxNot.setToolTipText("");
    jCheckBoxNot.setText("Not");
    jLabelDate.setText("Date (ggmmyyyy)");
    jLabelDate.setVisible(false);
    buttonGroupFirstOperand.add(jRadioButtonAttributeFirstOperand);
    buttonGroupFirstOperand.add(jRadioButtonFunctionFirstOperand);
    buttonGroupSecondOperand.add(jRadioButtonAttributeSecondOperand);
    buttonGroupSecondOperand.add(jRadioButtonFunctionSecondOperand);
    buttonGroupSecondOperand.add(jRadioButtonValueSecondOperand);
    jTextFieldYear.setText(Integer.toString(calendar.get(Calendar.YEAR)));
    jTextFieldMonth.setText(Integer.toString(calendar.get(Calendar.MONTH) + 1));
    jTextFieldDay
        .setText(Integer.toString(calendar.get(Calendar.DAY_OF_MONTH)));
    jTextFieldHours.setText(Integer.toString(calendar.get(Calendar.HOUR)));
    jTextFieldMinutes.setText(Integer.toString(calendar.get(Calendar.MINUTE)));
    jTextFieldSeconds.setText(Integer.toString(calendar.get(Calendar.SECOND)));
    jButtonAdd.setText("Add");
    jButtonAdd.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonAddEvent(e);
      }
    });
    jComboBoxAttributesFirstOperand
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jComboBoxAttributesFirstOperandEvent(e);
          }
        });
    jTextFieldValueSecondOperand
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            GraphicUtils.jTextFieldDeselect(jTextFieldValueSecondOperand);
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
    jRadioButtonAttributeFirstOperand.setText("Attribute");
    jRadioButtonAttributeFirstOperand
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jRadioButtonChangeEvent("Attribute", FIRSTOPERAND, e);
          }
        });
    jRadioButtonFunctionFirstOperand.setText("Function");
    jRadioButtonFunctionFirstOperand
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jRadioButtonChangeEvent("Function", FIRSTOPERAND, e);
          }
        });
    jComboBoxFunctionsFirstOperand
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jComboBoxFunctionsFirstOperandEvent(e);
          }
        });
    jComboBoxFunctionsSecondOperand
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jComboBoxFunctionsSecondOperandEvent(e);
          }
        });
    jRadioButtonValueSecondOperand.setSelected(true);
    jRadioButtonAttributeSecondOperand
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jRadioButtonChangeEvent("Attribute", SECONDOPERAND, e);
          }
        });
    jRadioButtonAttributeSecondOperand.setText("Attribute");
    jComboBoxFirstOperandParam1
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jComboBoxFirstOperandParam1Event(e);
          }
        });
    jComboBoxFirstOperandParam1.getEditor().getEditorComponent()
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            ((JTextField) jComboBoxFirstOperandParam1.getEditor()
                .getEditorComponent()).select(0, 0);
          }
        });
    jComboBoxFirstOperandParam1.setEditable(true);
    /*jComboBoxAttributesSecondOperand.addActionListener(new java.awt.event.
     ActionListener() {
     public void actionPerformed(ActionEvent e) {
     //functionsSecondOperandSetJCombo();
     jComboBoxAttributesSecondOperandEvent(e);
     }
     });*/
    jRadioButtonFunctionSecondOperand.setText("Function");
    jRadioButtonFunctionSecondOperand
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jRadioButtonChangeEvent("Function", SECONDOPERAND, e);
          }
        });
    jComboBoxSecondOperandParam1.setEditable(true);
    jComboBoxSecondOperandParam1.getEditor().getEditorComponent()
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            ((JTextField) jComboBoxSecondOperandParam1.getEditor()
                .getEditorComponent()).select(0, 0);
          }
        });
    jPanelClassAdTree.setBorder(new TitledBorder(new EtchedBorder(),
        " Expression Tree ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelClassAdTree.setLayout(null);
    jRadioButtonValueSecondOperand.setText("Value");
    jRadioButtonValueSecondOperand
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jRadioButtonChangeEvent("Value", SECONDOPERAND, e);
          }
        });
    jComboBoxSecondOperandParam2.setEditable(true);
    jComboBoxSecondOperandParam2.getEditor().getEditorComponent()
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            ((JTextField) jComboBoxSecondOperandParam2.getEditor()
                .getEditorComponent()).select(0, 0);
          }
        });
    jComboBoxSecondOperandParam3.setEditable(true);
    jComboBoxSecondOperandParam3.getEditor().getEditorComponent()
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            ((JTextField) jComboBoxSecondOperandParam3.getEditor()
                .getEditorComponent()).select(0, 0);
          }
        });
    jComboBoxFirstOperandParam2.setEditable(true);
    /*jComboBoxFirstOperandParam2.addActionListener(new java.awt.event.
     ActionListener() {
     public void actionPerformed(ActionEvent e) {
     jComboBoxFirstOperandParam2Event(e);
     }
     });*/
    jComboBoxFirstOperandParam2.getEditor().getEditorComponent()
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            ((JTextField) jComboBoxFirstOperandParam2.getEditor()
                .getEditorComponent()).select(0, 0);
          }
        });
    jComboBoxFirstOperandParam3.setEditable(true);
    jComboBoxFirstOperandParam3.getEditor().getEditorComponent()
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            ((JTextField) jComboBoxFirstOperandParam3.getEditor()
                .getEditorComponent()).select(0, 0);
          }
        });
    downYear.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldYear, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.YEAR)), 1970, 9999);
      }
    });
    upYear.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldYear, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.YEAR)), 1970, 9999);
      }
    });
    jTextFieldYear.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldYear.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldYear, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.YEAR)), 1970, 9999);
      }
    });
    downMonth.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldMonth, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.MONTH) + 1), 1, 12);
      }
    });
    jTextFieldMonth.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldMonth, Utils.INTEGER,
            Integer.toString(calendar.get(Calendar.MONTH) + 1), 1, 12);
      }
    });
    jTextFieldMonth.setHorizontalAlignment(SwingConstants.RIGHT);
    upMonth.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldMonth, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.MONTH) + 1), 1, 12);
      }
    });
    jTextFieldDay.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldDay.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldDay, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.DAY_OF_MONTH)), 1, 31);
      }
    });
    downDay.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldDay, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.DAY_OF_MONTH)), 1, 31);
      }
    });
    upDay.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldDay, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.DAY_OF_MONTH)), 1, 31);
      }
    });
    jTextFieldHours.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldHours.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldHours, Utils.INTEGER,
            Integer.toString(calendar.get(Calendar.HOUR)), 0, 23);
      }
    });
    downHours.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldHours, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.HOUR)), 0, 23);
      }
    });
    upHours.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldHours, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.HOUR)), 0, 23);
      }
    });
    jTextFieldMinutes.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldMinutes.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldMinutes, Utils.INTEGER,
            Integer.toString(calendar.get(Calendar.MINUTE)), 0, 59);
      }
    });
    downMinutes.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldMinutes, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.MINUTE)), 0, 59);
      }
    });
    upMinutes.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldMinutes, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.MINUTE)), 0, 59);
      }
    });
    jTextFieldSeconds.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldSeconds.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldSeconds, Utils.INTEGER,
            Integer.toString(calendar.get(Calendar.SECOND)), 0, 59);
      }
    });
    downSeconds.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldSeconds, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.SECOND)), 0, 59);
      }
    });
    upSeconds.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldSeconds, Utils.INTEGER, Integer
            .toString(calendar.get(Calendar.SECOND)), 0, 59);
      }
    });
    GridBagLayout gbl = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.insets = new Insets(3, 3, 3, 3);
    // jPanelFirstOperand
    jPanelFirstOperand.setLayout(gbl);
    jPanelFirstOperand
        .setBorder(new TitledBorder(new EtchedBorder(), " First Operand ", 0,
            0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelFirstOperand.add(jRadioButtonAttributeFirstOperand, GraphicUtils
        .setGridBagConstraints(gbc, 0, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    jPanelFirstOperand.add(jComboBoxAttributesFirstOperand, GraphicUtils
        .setGridBagConstraints(gbc, 1, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jPanelFirstOperand.add(jCheckBoxNot, GraphicUtils.setGridBagConstraints(
        gbc, 2, 0, 1, 1, 0.0, 0.0, GridBagConstraints.WEST,
        GridBagConstraints.NONE, null, 0, 0));
    jPanelFirstOperand.add(jRadioButtonFunctionFirstOperand, GraphicUtils
        .setGridBagConstraints(gbc, 0, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    jPanelFirstOperand.add(jComboBoxFunctionsFirstOperand, GraphicUtils
        .setGridBagConstraints(gbc, 1, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    JPanel jPanelFunctionParamsFirstOperand = new JPanel();
    jPanelFunctionParamsFirstOperand.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelFunctionParamsFirstOperand.add(jComboBoxFirstOperandParam1,
        GraphicUtils.setGridBagConstraints(gbc, 0, 0, 1, 1, 0.3, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jPanelFunctionParamsFirstOperand.add(jComboBoxFirstOperandParam2,
        GraphicUtils.setGridBagConstraints(gbc, 1, 0, 1, 1, 0.3, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jPanelFunctionParamsFirstOperand.add(jComboBoxFirstOperandParam3,
        GraphicUtils.setGridBagConstraints(gbc, 2, 0, 1, 1, 0.3, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jPanelFirstOperand.add(jPanelFunctionParamsFirstOperand, GraphicUtils
        .setGridBagConstraints(gbc, 0, 2, 4, 1, 1.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    // jPanelRelation
    JPanel jPanelRelation = new JPanel();
    ((FlowLayout) jPanelRelation.getLayout()).setAlignment(FlowLayout.LEFT);
    jPanelRelation.add(jLabelRelation);
    jPanelRelation.add(jComboBoxRelations);
    // jPanelSecondOperand
    jPanelSecondOperand.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelSecondOperand
        .setBorder(new TitledBorder(new EtchedBorder(), " Second Operand ", 0,
            0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelSecondOperand.add(jRadioButtonAttributeSecondOperand, GraphicUtils
        .setGridBagConstraints(gbc, 0, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    jPanelSecondOperand.add(jComboBoxAttributesSecondOperand, GraphicUtils
        .setGridBagConstraints(gbc, 1, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jPanelSecondOperand.add(jRadioButtonValueSecondOperand, GraphicUtils
        .setGridBagConstraints(gbc, 2, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.EAST, GridBagConstraints.NONE, null, 0, 0));
    jPanelSecondOperand.add(jTextFieldValueSecondOperand, GraphicUtils
        .setGridBagConstraints(gbc, 3, 0, 1, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            4));
    jPanelSecondOperand.add(jRadioButtonFunctionSecondOperand, GraphicUtils
        .setGridBagConstraints(gbc, 0, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
    jPanelSecondOperand.add(jComboBoxFunctionsSecondOperand, GraphicUtils
        .setGridBagConstraints(gbc, 1, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    JPanel jPanelFunctionParamsSecondOperand = new JPanel();
    jPanelFunctionParamsSecondOperand.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelFunctionParamsSecondOperand.add(jComboBoxSecondOperandParam1,
        GraphicUtils.setGridBagConstraints(gbc, 0, 0, 1, 1, 0.3, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jPanelFunctionParamsSecondOperand.add(jComboBoxSecondOperandParam2,
        GraphicUtils.setGridBagConstraints(gbc, 1, 0, 1, 1, 0.3, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jPanelFunctionParamsSecondOperand.add(jComboBoxSecondOperandParam3,
        GraphicUtils.setGridBagConstraints(gbc, 2, 0, 1, 1, 0.3, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0,
            0));
    jPanelSecondOperand.add(jPanelFunctionParamsSecondOperand, GraphicUtils
        .setGridBagConstraints(gbc, 0, 2, 4, 1, 0.5, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE, null, 0, 0));
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
    this.add(jPanelFirstOperand, GraphicUtils.setGridBagConstraints(gbc, 0, 0,
        1, 1, 1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, new Insets(1, 1, 1, 1), 0, 0));
    this.add(jPanelRelation, GraphicUtils.setGridBagConstraints(gbc, 0, 1, 1,
        1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    this.add(jPanelSecondOperand, GraphicUtils.setGridBagConstraints(gbc, 0, 2,
        1, 1, 1.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    this.add(jPanelClassAdTree, GraphicUtils.setGridBagConstraints(gbc, 0, 3,
        1, 1, 0.0, 1.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, null, 0, 0));
    radioButtonInitialize();
    this.setVisible(true);
  }

  // Show a JOptionPane.
  private int showJOptionPane(String msg, String title) {
    int choice = JOptionPane.showOptionDialog(RequirementsAdvancedPanel.this,
        msg, title, JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE,
        null, null, null);
    return choice;
  }

  boolean isParameterTypeMatch(int paramNumber, String param, int operand) {
    String item;
    String selectedFunctionItem = "";
    param = param.toUpperCase();
    switch (operand) {
      case FIRSTOPERAND:
        selectedFunctionItem = jComboBoxFunctionsFirstOperand.getSelectedItem()
            .toString();
      break;
      case SECONDOPERAND:
        selectedFunctionItem = jComboBoxFunctionsSecondOperand
            .getSelectedItem().toString();
      break;
    }
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
        if (!param.equals("TRUE") && !param.equals("FALSE")) {
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

  void jButtonAddEvent(ActionEvent e) {
    DefaultMutableTreeNode selectedNode = (DefaultMutableTreeNode) jTreeExpr
        .getLastSelectedPathComponent();
    DefaultMutableTreeNode parentNode = null;
    DefaultMutableTreeNode previousNode = null;
    DefaultMutableTreeNode nextNode = null;
    DefaultMutableTreeNode lastNode = null;
    DefaultMutableTreeNode operatorNode = null;
    Object selectedRelation = jComboBoxRelations.getSelectedItem();
    Object selectedOperator = jComboBoxOperators.getSelectedItem();
    String selectedOperatorSubText = selectedOperator.toString()
        .substring(0, 2);
    //Object selectedAttributeFirstOperand = jComboBoxAttributesFirstOperand.getSelectedItem();
    Object selectedFunctionFirstOperand = jComboBoxFunctionsFirstOperand
        .getSelectedItem();
    //Object selectedAttributeFunction = jComboBoxFirstOperandParam1.getSelectedItem();
    //int firstOperandType = -1;
    int firstOperandType = Utils.UNKNOWN;
    String nodeTextFirstOperand = "";
    String firstSpace = " ";
    if (jRadioButtonAttributeFirstOperand.isSelected()) {
      nodeTextFirstOperand += "other."
          + jComboBoxAttributesFirstOperand.getSelectedItem().toString(); // + selectedRelation.toString() + value;
      firstOperandType = getAttributeType(jComboBoxAttributesFirstOperand
          .getSelectedItem().toString());
    } else if (jRadioButtonFunctionFirstOperand.isSelected()) {
      String selectedFunctionTextFirstOperand = selectedFunctionFirstOperand
          .toString();
      Vector functionParamsVector = getFunctionParameterTypes(selectedFunctionTextFirstOperand);
      int functionTextFirstOperandLength = selectedFunctionTextFirstOperand
          .length();
      int index = selectedFunctionTextFirstOperand.indexOf("(");
      String functionNameFirstOperand = selectedFunctionTextFirstOperand
          .substring(0, index + 1);
      //nodeTextFirstOperand = functionNameFirstOperand + jComboBoxFirstOperandParam1.getSelectedItem().toString(); // + ")";
      firstOperandType = getFunctionType(selectedFunctionTextFirstOperand);
      nodeTextFirstOperand += functionNameFirstOperand;
      int selectedFunctionFirstParameterType = Utils.UNKNOWN;
      if (jComboBoxFirstOperandParam1.isVisible()) {
        selectedFunctionFirstParameterType = Integer.parseInt(
            functionParamsVector.get(0).toString(), 10);
      }
      if (jComboBoxFirstOperandParam1.isVisible()
          && selectedFunctionFirstParameterType == Utils.LIST) {
        //String param1 = jComboBoxFirstOperandParam1.getSelectedItem().toString().trim();
        String param1 = jComboBoxFirstOperandParam1.getEditor().getItem()
            .toString().trim();
        if (param1.equals("")) {
          showJOptionPane(
              "First Operand: first parameter field cannot be blank",
              Utils.ERROR_MSG_TXT);
          if (jComboBoxFirstOperandParam1.getItemCount() != 0) {
            jComboBoxFirstOperandParam1.showPopup();
          }
          return;
        }
        logger.debug("attributesNameVector: " + attributesNameVector);
        if (!attributesNameVector.contains(param1)) {
          nodeTextFirstOperand += param1;
        } else {
          nodeTextFirstOperand += "other." + param1;
        }
        if (jComboBoxFirstOperandParam2.isVisible()) {
          //String param2 = jComboBoxFirstOperandParam2.getModel().getSelectedItem().toString().trim();
          String param2 = jComboBoxFirstOperandParam2.getEditor().getItem()
              .toString().trim();
          if (param2.equals("")) {
            showJOptionPane(
                "First Operand: second parameter field cannot be blank",
                Utils.ERROR_MSG_TXT);
            if (jComboBoxFirstOperandParam2.getItemCount() != 0) {
              jComboBoxFirstOperandParam2.showPopup();
            }
            return;
          }
          if (!attributesNameVector.contains(param1)) {
            if (!attributesNameVector.contains(param2)) {
              nodeTextFirstOperand += ", " + param2;
            } else {
              nodeTextFirstOperand += ", " + "other." + param2;
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
                  nodeTextFirstOperand += ", " + "\"" + param2 + "\"";
                } else {
                  nodeTextFirstOperand += ", " + param2;
                }
              } else {
                nodeTextFirstOperand += ", " + "other." + param2;
              }
            } else {
              showJOptionPane(
                  "First Operand: second parameter field type mismatch",
                  Utils.ERROR_MSG_TXT);
              if (jComboBoxFirstOperandParam2.getItemCount() != 0) {
                jComboBoxFirstOperandParam2.showPopup();
              }
              return;
            }
          }
        }
        if (jComboBoxFirstOperandParam3.isVisible()) {
          //String param3 = jComboBoxFirstOperandParam3.getSelectedItem().toString().trim();
          String param3 = jComboBoxFirstOperandParam3.getEditor().getItem()
              .toString().trim();
          if (param3.equals("")) {
            showJOptionPane(
                "First Operand: third parameter field cannot be blank",
                Utils.ERROR_MSG_TXT);
            if (jComboBoxFirstOperandParam3.getItemCount() != 0) {
              jComboBoxFirstOperandParam3.showPopup();
            }
            return;
          }
          if (!attributesNameVector.contains(param1)) {
            if (!attributesNameVector.contains(param3)) {
              nodeTextFirstOperand += ", " + param3;
            } else {
              nodeTextFirstOperand += ", " + "other." + param3;
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
                  nodeTextFirstOperand += ", " + "\"" + param3 + "\"";
                } else {
                  nodeTextFirstOperand += ", " + param3;
                }
              } else {
                nodeTextFirstOperand += ", " + "other." + param3;
              }
            } else {
              showJOptionPane(
                  "First Operand: third parameter field type mismatch",
                  Utils.ERROR_MSG_TXT);
              if (jComboBoxFirstOperandParam3.getItemCount() != 0) {
                jComboBoxFirstOperandParam3.showPopup();
              }
              return;
            }
          }
        }
        nodeTextFirstOperand += ")";
        // This line is added to invert the parameters order of the function
        // when it is Member() or isMember() (first parameter is of LIST type).
        // A new utility method invertFunctionParameter() is necessary.
        if (selectedFunctionFirstParameterType == Utils.LIST) {
          nodeTextFirstOperand = Utils
              .swapFunctionParameters(nodeTextFirstOperand);
        }
      } else {
        if (jComboBoxFirstOperandParam1.isVisible()) {
          //String param1 = jComboBoxFirstOperandParam1.getSelectedItem().toString().trim();
          String param1 = jComboBoxFirstOperandParam1.getEditor().getItem()
              .toString().trim();
          if (param1.equals("")) {
            showJOptionPane(
                "First Operand: first parameter field cannot be blank",
                Utils.ERROR_MSG_TXT);
            if (jComboBoxFirstOperandParam1.getItemCount() != 0) {
              jComboBoxFirstOperandParam1.showPopup();
            }
            return;
          } else {
            if (!isParameterTypeMatch(0, param1, FIRSTOPERAND)) {
              showJOptionPane(
                  "Firts Operand: first parameter field type mismatch",
                  Utils.ERROR_MSG_TXT);
              if (jComboBoxFirstOperandParam1.getItemCount() != 0) {
                jComboBoxFirstOperandParam1.showPopup();
              }
              return;
            }
            Vector jComboBoxFirstOperandParam1Vector = new Vector();
            for (int i = 0; i < jComboBoxFirstOperandParam1.getModel()
                .getSize(); i++) {
              jComboBoxFirstOperandParam1Vector
                  .add((String) jComboBoxFirstOperandParam1.getModel()
                      .getElementAt(i));
            }
            if (jComboBoxFirstOperandParam1Vector.contains(param1)) {
              nodeTextFirstOperand += "other." + param1;
            } else if (Integer.parseInt(functionParamsVector.get(0).toString(),
                10) == Utils.STRING) {
              nodeTextFirstOperand += "\"" + param1 + "\"";
            } else {
              nodeTextFirstOperand += parseNumericValue(param1);
            }
          }
        }
        if (jComboBoxFirstOperandParam2.isVisible()) {
          //String param2 = jComboBoxFirstOperandParam2.getSelectedItem().toString().trim();
          String param2 = jComboBoxFirstOperandParam2.getEditor().getItem()
              .toString().trim();
          if (param2.equals("")) {
            showJOptionPane(
                "First Operand: second parameter field cannot be blank",
                Utils.ERROR_MSG_TXT);
            if (jComboBoxFirstOperandParam2.getItemCount() != 0) {
              jComboBoxFirstOperandParam2.showPopup();
            }
            return;
          } else {
            if (!isParameterTypeMatch(1, param2, FIRSTOPERAND)) {
              showJOptionPane(
                  "First Operand: second parameter field type mismatch",
                  Utils.ERROR_MSG_TXT);
              if (jComboBoxFirstOperandParam2.getItemCount() != 0) {
                jComboBoxFirstOperandParam2.showPopup();
              }
              return;
            }
            Vector jComboBoxFirstOperandParam2Vector = new Vector();
            for (int i = 0; i < jComboBoxFirstOperandParam2.getModel()
                .getSize(); i++) {
              jComboBoxFirstOperandParam2Vector
                  .add((String) jComboBoxFirstOperandParam2.getModel()
                      .getElementAt(i));
            }
            if (jComboBoxFirstOperandParam2Vector.contains(param2)) {
              nodeTextFirstOperand += ", other." + param2;
            } else if (Integer.parseInt(functionParamsVector.get(1).toString(),
                10) == Utils.STRING) {
              nodeTextFirstOperand += ", " + "\"" + param2 + "\"";
            } else {
              nodeTextFirstOperand += ", " + parseNumericValue(param2);
            }
          }
        }
        if (jComboBoxFirstOperandParam3.isVisible()) {
          //String param3 = jComboBoxFirstOperandParam3.getSelectedItem().toString().trim();
          String param3 = jComboBoxFirstOperandParam3.getEditor().getItem()
              .toString().trim();
          if (param3.equals("")) {
            showJOptionPane(
                "First Operand: third parameter field cannot be blank",
                Utils.ERROR_MSG_TXT);
            if (jComboBoxFirstOperandParam3.getItemCount() != 0) {
              jComboBoxFirstOperandParam3.showPopup();
            }
            return;
          } else {
            if (!isParameterTypeMatch(2, param3, FIRSTOPERAND)) {
              showJOptionPane(
                  "First Operand: third parameter field type mismatch",
                  Utils.ERROR_MSG_TXT);
              if (jComboBoxFirstOperandParam3.getItemCount() != 0) {
                jComboBoxFirstOperandParam3.showPopup();
              }
              return;
            }
            Vector jComboBoxFirstOperandParam3Vector = new Vector();
            for (int i = 0; i < jComboBoxFirstOperandParam3.getModel()
                .getSize(); i++) {
              jComboBoxFirstOperandParam3Vector
                  .add((String) jComboBoxFirstOperandParam3.getModel()
                      .getElementAt(i));
            }
            if (jComboBoxFirstOperandParam3Vector.contains(param3)) {
              nodeTextFirstOperand += ", other." + param3;
            } else if (Integer.parseInt(functionParamsVector.get(2).toString(),
                10) == Utils.STRING) {
              nodeTextFirstOperand += ", " + "\"" + param3 + "\"";
            } else {
              nodeTextFirstOperand += ", " + parseNumericValue(param3);
            }
          }
        }
        nodeTextFirstOperand += ")";
      }
    }
    if (jCheckBoxNot.isSelected()) {
      nodeTextFirstOperand = "!" + nodeTextFirstOperand;
      jCheckBoxNot.setSelected(false);
    }
    String nodeTextSecondOperand = "";
    Object selectedFunctionSecondOperand = jComboBoxFunctionsSecondOperand
        .getSelectedItem();
    String selectedAttributeItem = jComboBoxAttributesFirstOperand
        .getSelectedItem().toString();
    String selectedFunctionItem = jComboBoxFunctionsFirstOperand
        .getSelectedItem().toString();
    if (jRadioButtonAttributeFirstOperand.isSelected()
        && (getAttributeType(selectedAttributeItem) == Utils.BOOLEAN)) {
      selectedRelation = "";
      nodeTextSecondOperand = "";
      firstSpace = "";
    } else if (jRadioButtonFunctionFirstOperand.isSelected()
        && (getFunctionType(selectedFunctionItem) == Utils.BOOLEAN)) {
      selectedRelation = "";
      nodeTextSecondOperand = "";
      firstSpace = "";
    } else if (jRadioButtonValueSecondOperand.isSelected()) {
      if (firstOperandType == Utils.ABS_TIME) {
        int day = Integer.parseInt(jTextFieldDay.getText(), 10);
        int month = Integer.parseInt(jTextFieldMonth.getText(), 10) - 1;
        int year = Integer.parseInt(jTextFieldYear.getText(), 10);
        int hours = Integer.parseInt(jTextFieldHours.getText(), 10);
        int minutes = Integer.parseInt(jTextFieldMinutes.getText(), 10);
        int seconds = Integer.parseInt(jTextFieldSeconds.getText(), 10);
        calendar.set(year, month, day, hours, minutes, seconds);
        long millisecondsDate = calendar.getTimeInMillis();
        Date date = new Date(millisecondsDate);
        SimpleDateFormat simpleDateFormat = new SimpleDateFormat(
            "EEE MMM dd HH:mm:ss yyyy '(UTC) 00:00'");
        simpleDateFormat.setTimeZone(new SimpleTimeZone(0, "GMT"));
        StringBuffer stringBuffer = new StringBuffer(simpleDateFormat
            .format(date));
        if (stringBuffer.charAt(8) == '0') {
          stringBuffer.setCharAt(8, ' ');
        }
        nodeTextSecondOperand = "'" + stringBuffer.toString() + "'";
        //nodeTextSecondOperand = "\"" + stringBuffer.toString() + "\"";
      } else if (firstOperandType == Utils.REL_TIME) {
        String hours = jTextFieldHours.getText();
        String minutes = jTextFieldMinutes.getText();
        String seconds = jTextFieldSeconds.getText();
        nodeTextSecondOperand = "'" + hours + ":" + minutes + ":" + seconds
            + "'";
      } else {
        String value = jTextFieldValueSecondOperand.getText().trim();
        if (value.equals("")) {
          jTextFieldValueSecondOperand.grabFocus();
          showJOptionPane("Second Operand: value field cannot be blank",
              Utils.ERROR_MSG_TXT);
          jTextFieldValueSecondOperand.selectAll();
          return;
        } else {
          int valueType = Utils.getValueType(value);
          if (firstOperandType != Utils.STRING) {
            if (((firstOperandType == Utils.INTEGER)
                || (firstOperandType == Utils.FLOAT)
                || (firstOperandType == Utils.MEMORY) || (firstOperandType == Utils.SECONDS))
                && ((valueType == Utils.FLOAT) || (valueType == Utils.INTEGER))) {
              value = parseNumericValue(value);
            } else if (firstOperandType != valueType) {
              jTextFieldValueSecondOperand.grabFocus();
              showJOptionPane(
                  "Second Operand: value field must be same type as first operand type",
                  Utils.ERROR_MSG_TXT);
              jTextFieldValueSecondOperand.selectAll();
              return;
            }
          }
          if (firstOperandType == Utils.STRING) {
            nodeTextSecondOperand = "\"" + value + "\"";
          } else {
            nodeTextSecondOperand = value;
          }
        }
      }
    } else if (jRadioButtonAttributeSecondOperand.isSelected()) {
      nodeTextSecondOperand = "other."
          + jComboBoxAttributesSecondOperand.getSelectedItem().toString();
    } else if (jRadioButtonFunctionSecondOperand.isSelected()) {
      String selectedFunctionTextSecondOperand = selectedFunctionSecondOperand
          .toString();
      Vector functionParamsVector = getFunctionParameterTypes(selectedFunctionTextSecondOperand);
      int functionTextSecondOperandLength = selectedFunctionTextSecondOperand
          .length();
      int index = selectedFunctionTextSecondOperand.indexOf("(");
      String functionNameSecondOperand = selectedFunctionTextSecondOperand
          .substring(0, index + 1);
      nodeTextSecondOperand = functionNameSecondOperand;
      if (jComboBoxSecondOperandParam1.isVisible()) {
        String param1 = jComboBoxSecondOperandParam1.getEditor().getItem()
            .toString().trim();
        if (param1.equals("")) {
          showJOptionPane(
              "Second Operand: first parameter field cannot be blank",
              Utils.ERROR_MSG_TXT);
          if (jComboBoxSecondOperandParam1.getItemCount() != 0) {
            jComboBoxSecondOperandParam1.showPopup();
          }
          return;
        } else {
          if (!isParameterTypeMatch(0, param1, SECONDOPERAND)) {
            showJOptionPane(
                "Second Operand: first parameter field type mismatch",
                Utils.ERROR_MSG_TXT);
            if (jComboBoxSecondOperandParam1.getItemCount() != 0) {
              jComboBoxSecondOperandParam1.showPopup();
            }
            return;
          }
          Vector jComboBoxSecondOperandParam1Vector = new Vector();
          for (int i = 0; i < jComboBoxSecondOperandParam1.getModel().getSize(); i++) {
            jComboBoxSecondOperandParam1Vector
                .add((String) jComboBoxSecondOperandParam1.getModel()
                    .getElementAt(i));
          }
          if (jComboBoxSecondOperandParam1Vector.contains(param1)) {
            nodeTextSecondOperand += "other." + param1;
          } else if (Integer.parseInt(functionParamsVector.get(0).toString(),
              10) == Utils.STRING) {
            nodeTextSecondOperand += "\"" + param1 + "\"";
          } else {
            nodeTextSecondOperand += parseNumericValue(param1);
          }
        }
      }
      if (jComboBoxSecondOperandParam2.isVisible()) {
        String param2 = jComboBoxSecondOperandParam2.getSelectedItem()
            .toString().trim();
        if (param2.equals("")) {
          showJOptionPane(
              "Second Operand: second parameter field cannot be blank",
              Utils.ERROR_MSG_TXT);
          if (jComboBoxSecondOperandParam2.getItemCount() != 0) {
            jComboBoxSecondOperandParam2.showPopup();
          }
          return;
        } else {
          if (!isParameterTypeMatch(1, param2, SECONDOPERAND)) {
            showJOptionPane(
                "Second Operand: second parameter field: type mismatch",
                Utils.ERROR_MSG_TXT);
            if (jComboBoxSecondOperandParam2.getItemCount() != 0) {
              jComboBoxSecondOperandParam2.showPopup();
            }
            return;
          }
          Vector jComboBoxSecondOperandParam2Vector = new Vector();
          for (int i = 0; i < jComboBoxSecondOperandParam2.getModel().getSize(); i++) {
            jComboBoxSecondOperandParam2Vector
                .add((String) jComboBoxSecondOperandParam2.getModel()
                    .getElementAt(i));
          }
          if (jComboBoxSecondOperandParam2Vector.contains(param2)) {
            nodeTextSecondOperand += ", other." + param2;
          } else if (Integer.parseInt(functionParamsVector.get(1).toString(),
              10) == Utils.STRING) {
            nodeTextSecondOperand += ", " + "\"" + param2 + "\"";
          } else {
            nodeTextSecondOperand += ", " + parseNumericValue(param2);
          }
        }
      }
      if (jComboBoxSecondOperandParam3.isVisible()) {
        String param3 = jComboBoxSecondOperandParam3.getEditor().getItem()
            .toString().trim();
        if (param3.equals("")) {
          showJOptionPane(
              "Second Operand: third parameter field cannot be blank",
              Utils.ERROR_MSG_TXT);
          if (jComboBoxSecondOperandParam3.getItemCount() != 0) {
            jComboBoxSecondOperandParam3.showPopup();
          }
          return;
        } else {
          if (!isParameterTypeMatch(2, param3, SECONDOPERAND)) {
            showJOptionPane(
                "Second Operand: third parameter field type mismatch",
                Utils.ERROR_MSG_TXT);
            if (jComboBoxSecondOperandParam3.getItemCount() != 0) {
              jComboBoxSecondOperandParam3.showPopup();
            }
            return;
          }
          Vector jComboBoxSecondOperandParam3Vector = new Vector();
          for (int i = 0; i < jComboBoxSecondOperandParam3.getModel().getSize(); i++) {
            jComboBoxSecondOperandParam3Vector
                .add((String) jComboBoxSecondOperandParam3.getModel()
                    .getElementAt(i));
          }
          if (jComboBoxSecondOperandParam3Vector.contains(param3)) {
            nodeTextSecondOperand += ", other." + param3;
          } else if (Integer.parseInt(functionParamsVector.get(2).toString(),
              10) == Utils.STRING) {
            nodeTextSecondOperand += ", " + "\"" + param3 + "\"";
          } else {
            nodeTextSecondOperand += ", " + parseNumericValue(param3);
          }
        }
      }
      nodeTextSecondOperand += ")";
    }
    String nodeText = nodeTextFirstOperand + firstSpace
        + selectedRelation.toString() + " " + nodeTextSecondOperand;
    TreePath selectedNodePath = jTreeExpr.getSelectionPath();
    // Check if selected node is a logical operator node (you can't add anything to this node).
    String selectedNodeText = new String("");
    if (selectedNode != null) {
      selectedNodeText = selectedNode.getUserObject().toString();
    }
    if ((selectedNodeText.equals("AND"))
        || (selectedNodeText.equals("AND NOT"))
        || (selectedNodeText.equals("OR"))
        || (selectedNodeText.equals("OR NOT"))) {
      return;
    }
    // If there's no selection, default node is root node.
    if (selectedNodePath == null) {
      selectedNode = rootNode;
      // If statements must be in this order!
    }
    if (rootNode.getChildCount() == 0) {
      // Adding first attribute.
      addNode(rootNode, nodeText, true);
    } else if ((rootNode.getChildCount() == 1)
        && (selectedNode.isRoot() || (selectedNode == rootNode.getFirstChild()))) {
      // Selected node is root node or the only child node of the root node. Adding second attribute.
      addNode(rootNode, selectedOperator, true);
      addNode(rootNode, nodeText, true);
      jTreeExpr.setShowsRootHandles(true); // Add the handle (open, close tree) to root node.
      jTreeExpr.setRootVisible(true);
    } else if (selectedNode.isRoot() || selectedNodeText.equals(subExp)) {
      // Selected node is rootNode (Exp) or a Sub-Exp node.
      // Getting tree level operator (maybe you can associate operator to parent node).
      operatorNode = (DefaultMutableTreeNode) selectedNode.getChildAt(1); //Second child.
      String operatorNodeSubText = operatorNode.getUserObject().toString()
          .substring(0, 2);
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
      String operatorNodeSubText = operatorNode.getUserObject().toString()
          .substring(0, 2);
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
      String operatorNodeSubText = operatorNode.getUserObject().toString()
          .substring(0, 2);
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
    if (jRadioButtonValueSecondOperand.isSelected()) {
      jTextFieldValueSecondOperand.grabFocus();
      jTextFieldValueSecondOperand.selectAll();
    }
  }

  void jButtonRemoveEvent(ActionEvent e) {
    DefaultMutableTreeNode selectedNode = (DefaultMutableTreeNode) jTreeExpr
        .getLastSelectedPathComponent();
    DefaultMutableTreeNode operatorNode = null;
    TreePath selectedNodePath = jTreeExpr.getSelectionPath();
    // Check if selected node is a logical operator node (you can't directly remove this node).
    String selectedNodeText = new String("");
    if (selectedNode != null) {
      selectedNodeText = selectedNode.getUserObject().toString();
    }
    if ((selectedNodeText.equals("AND"))
        || (selectedNodeText.equals("AND NOT"))
        || (selectedNodeText.equals("OR"))
        || (selectedNodeText.equals("OR NOT"))) {
      return;
    }
    if (selectedNodePath == null) {
      int choice = JOptionPane.showOptionDialog(RequirementsAdvancedPanel.this,
          "You have to select a node first", Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
          null, null);
    } else if (selectedNode.isRoot()) {
      int choice = JOptionPane.showOptionDialog(RequirementsAdvancedPanel.this,
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
    } else {
      parentNode = (DefaultMutableTreeNode) selectedNode.getParent();
      if (selectedNode == parentNode.getChildAt(0)) {
        // Selected node is the first Attribute in the tree.
        operatorNode = (DefaultMutableTreeNode) parentNode
            .getChildAfter(selectedNode);
        removeNode(selectedNode);
        removeNode(operatorNode);
      } else {
        operatorNode = (DefaultMutableTreeNode) parentNode
            .getChildBefore(selectedNode);
        removeNode(operatorNode);
        removeNode(selectedNode);
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
            //for(int i = 0; i < childrenNumber; i++)
            //            jTreeExpr.scrollPathToVisible(new TreePath(children[i].getPath()));
          }
          expandTree(parentNode);
        }
        removeNode(childNode);
      }
    }
    if (rootNode.getChildCount() == 1) {
      // Root node has only a child.
      DefaultMutableTreeNode firstChild = (DefaultMutableTreeNode) rootNode
          .getFirstChild();
      if (firstChild.getUserObject().toString().equals(subExp)) {
        // The only child of root node is a "Sub-Exp" node. This will change childcount value!
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

  private void jRadioButtonChangeEvent(String choice, int operand, ActionEvent e) {
    boolean attribute = false, function = false, value = false;
    if (choice.equals("Attribute")) {
      attribute = true;
      if (operand == FIRSTOPERAND) {
        attributesFirstOperandSetJCombo();
        attributesFirstOperandSetJRadio();
        //jComboBoxFunctionsSecondOperandEvent(null);
      }
    } else if (choice.equals("Function")) {
      function = true;
      if (operand == FIRSTOPERAND) {
        functionsFirstOperandSetJCombo();
        functionsFirstOperandSetJRadio();
        functionsFirstOperandSetParamJText();
        //jComboBoxFunctionsSecondOperandEvent(null);
      }
      /* else if(operand == SECONDOPERAND) {
       if(jComboBoxFunctionsSecondOperand.getItemCount() == 0)
       jRadioButtonChangeEvent("Value", SECONDOPERAND, null);
       jRadioButtonFunctionSecondOperand.setEnabled(false);
       }*/
    } else if (choice.equals("Value")) {
      value = true;
    }
    if (!GUIFileSystem.isConfFileError) { // If isConfFileError AdvancedPanel must be disabled.
      switch (operand) {
        case FIRSTOPERAND:
          // Attribute
          jComboBoxAttributesFirstOperand.setEnabled(attribute);
          // END Attribute
          // Function
          jComboBoxFunctionsFirstOperand.setEnabled(function);
          jComboBoxFirstOperandParam1.setEnabled(function);
          jComboBoxFirstOperandParam2.setEnabled(function);
          jComboBoxFirstOperandParam3.setEnabled(function);
        // END Function
        break;
        case SECONDOPERAND:
          // Value
          jTextFieldValueSecondOperand.setEnabled(value);
          setTimeTextFieldEnabled(value);
          setDateTextFieldEnabled(value);
          // END Value
          // Attribute
          jComboBoxAttributesSecondOperand.setEnabled(attribute);
          // END Attribute
          // Function
          jComboBoxFunctionsSecondOperand.setEnabled(function);
          jComboBoxSecondOperandParam1.setEnabled(function);
          jComboBoxSecondOperandParam2.setEnabled(function);
          jComboBoxSecondOperandParam3.setEnabled(function);
        // END Function
        break;
      }
    }
  }

  String getErrorMsg() {
    return errorMsg;
  }

  String getWarningMsg() {
    return warningMsg;
  }

  private void jButtonClearEvent(ActionEvent e) {
    int choice = JOptionPane.showOptionDialog(RequirementsAdvancedPanel.this,
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
      } else if (childNodeText.equals("AND")) {
        expr = expr + " && ";
      } else if (childNodeText.equals("AND NOT")) {
        expr = expr + " &&! "; // Maybe &&(!(
      } else if (childNodeText.equals("OR")) {
        expr = expr + " || ";
      } else if (childNodeText.equals("OR NOT")) {
        expr = expr + " ||! ";
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
    if (childText.equals("AND") || childText.equals("AND NOT")
        || childText.equals("OR") || childText.equals("OR NOT")) {
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
    operatorNodeText = operatorNode.getUserObject().toString().substring(0, 2);
    parentNodeParent = (DefaultMutableTreeNode) parentNode.getParent();
    if (parentNodeParent != null) {
      parentNodeParentOperator = (DefaultMutableTreeNode) parentNodeParent
          .getChildAt(1);
      parentNodeParentOperatorText = parentNodeParentOperator.getUserObject()
          .toString().substring(0, 2);
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

  // Represent an expression with a tree.
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
      return;
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
    String currentOperator2 = null;
    String currentOperator3 = null;
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
        if ((i + 3) < length) {
          currentOperator2 = expr.substring(i + 1, i + 3);
        } else {
          currentOperator2 = "";
        }
        if ((i + 5) < length) {
          currentOperator3 = expr.substring(i + 1, i + 5);
        } else {
          currentOperator3 = "";
        }
        if (currentOperator3.equals("&&(!")) {
          subExpr1 = expr.substring(0, i + 1);
          subExpr2 = expr.substring(i + 5, length - 1);
          operatorType = "AND NOT";
          haveToAdd = true;
        } else if (currentOperator3.equals("||(!")) {
          subExpr1 = expr.substring(0, i + 1);
          subExpr2 = expr.substring(i + 5, length - 1);
          operatorType = "OR NOT";
          haveToAdd = true;
        } else if (currentOperator2.equals("&&")) {
          subExpr1 = expr.substring(0, i + 1);
          subExpr2 = expr.substring(i + 3, length);
          operatorType = "AND";
          haveToAdd = true;
        } else if (currentOperator2.equals("||")) {
          subExpr1 = expr.substring(0, i + 1);
          subExpr2 = expr.substring(i + 3, length);
          operatorType = "OR";
          haveToAdd = true;
        }
        if (haveToAdd) {
          /// Changing panel
          subExpr1 = subExpr1.trim();
          subExpr2 = subExpr2.trim();
          ///
          DefaultMutableTreeNode expr1Node = addNode(parentNode, subExpr1, true);
          addNode(parentNode, operatorType, true);
          DefaultMutableTreeNode expr2Node = addNode(parentNode, subExpr2, true);
          if (parentNode != rootNode) {
            parentNode.setUserObject(subExp);
          }
          haveToAdd = false;
          createTreeFromExpr(subExpr1, expr1Node);
          createTreeFromExpr(subExpr2, expr2Node);
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

  private void functionsSecondOperandSetJCombo() {
    if (jComboBoxFunctionsSecondOperand.getItemCount() != 0) {
      String selectedFunctionItem = jComboBoxFunctionsSecondOperand
          .getSelectedItem().toString(); // Function name.
      int selectedFunctionType = getFunctionType(selectedFunctionItem); // Function type.
      Vector selectedFunctionParamTypes = getFunctionParameterTypes(selectedFunctionItem); // Function parameters.
      // Setting items in second operand, first, second, third parameter attributes combo boxes.
      // These items must have the same type as second operand function combo box selected item.
      jComboBoxSecondOperandParam1.removeAllItems();
      jComboBoxSecondOperandParam2.removeAllItems();
      jComboBoxSecondOperandParam3.removeAllItems();
      String functionName;
      for (int j = 0; j < confFileFunctionsVector.size(); j++) {
        functionName = ((Function) confFileFunctionsVector.get(j)).getName();
        if (selectedFunctionItem.equals(functionName)) {
          int parameterCount = ((Function) confFileFunctionsVector.get(j))
              .getParameterCount();
          for (int i = 0; i < attributesArrayList.size(); i++) {
            if (parameterCount >= 1) {
              if (getAttributeType(i) == Integer.parseInt(
                  selectedFunctionParamTypes.get(0).toString(), 10)) {
                if (!getAttributeIsMultivalued(i)) {
                  jComboBoxSecondOperandParam1.addItem(getAttributeName(i));
                }
              }
            }
            if (parameterCount >= 2) {
              if (getAttributeType(i) == Integer.parseInt(
                  selectedFunctionParamTypes.get(1).toString(), 10)) {
                if (!getAttributeIsMultivalued(i)) {
                  jComboBoxSecondOperandParam2.addItem(getAttributeName(i));
                }
              }
            }
            if (parameterCount == 3) {
              if (getAttributeType(i) == Integer.parseInt(
                  selectedFunctionParamTypes.get(2).toString(), 10)) {
                if (!getAttributeIsMultivalued(i)) {
                  jComboBoxSecondOperandParam3.addItem(getAttributeName(i));
                }
              }
            }
          }
          //functionsSetParamJText(parameterCount);
          blankSecondOperandJCombo();
          return;
        }
      }
    }
  }

  private void attributesFirstOperandSetJCombo() {
    if (jComboBoxAttributesFirstOperand.getItemCount() != 0) {
      String selectedAttributeItem = jComboBoxAttributesFirstOperand
          .getSelectedItem().toString(); // Attribute name.
      int selectedAttributeType = getAttributeType(selectedAttributeItem); // Attribute type.
      // Setting items in second operand attributes combo box.
      // These items must have the same type as first operand attribute
      // combo box selected item.
      jComboBoxAttributesSecondOperand.removeAllItems();
      for (int i = 0; i < attributesArrayList.size(); i++) {
        if (getAttributeType(i) == selectedAttributeType) {
          if (!getAttributeName(i).equals(selectedAttributeItem)
              && !getAttributeIsMultivalued(i)) {
            jComboBoxAttributesSecondOperand.addItem(getAttributeName(i));
          }
        }
      }
      // Setting items in second operand functions combo box.
      // These items must have the same type as first operand attribute
      // combo box selected item.
      jComboBoxFunctionsSecondOperand.removeAllItems();
      for (int i = 0; i < confFileFunctionsVector.size(); i++) {
        if (getFunctionType(i) == selectedAttributeType) {
          jComboBoxFunctionsSecondOperand.addItem(getFunctionName(i));
        }
      }
      //!!!
      functionsSecondOperandSetJCombo();
      //jComboBoxFunctionsSecondOperandEvent(null);
      functionsSecondOperandSetJCombo();
      functionsSecondOperandSetJRadio();
      //functionsFirstOperandSetJCombo(); //21/04/2004 added
      //functionsFirstOperandSetParamJText(); //21/04/2004 added
      //blankFirstOperandJCombo();
      //blankSecondOperandJCombo();
      switch (selectedAttributeType) {
        case Utils.INTEGER:
        case Utils.FLOAT:
        case Utils.ABS_TIME:
        case Utils.REL_TIME:
        case Utils.MEMORY:
        case Utils.SECONDS:
          jCheckBoxNot.setSelected(false);
          jCheckBoxNot.setEnabled(false);
          jComboBoxRelations.setEnabled(true);
          jComboBoxRelations.removeAllItems();
          for (int i = 0; i < relations.length; i++) {
            jComboBoxRelations.addItem(relations[i]);
          }
        break;
        case Utils.STRING:
          jCheckBoxNot.setSelected(false);
          jCheckBoxNot.setEnabled(false);
          jComboBoxRelations.setEnabled(true);
          jComboBoxRelations.removeAllItems();
          jComboBoxRelations.addItem("==");
          jComboBoxRelations.addItem("!=");
        break;
        case Utils.BOOLEAN:
          jCheckBoxNot.setEnabled(true);
          jComboBoxRelations.setEnabled(false);
        break;
      }
    }
  }

  private void functionsFirstOperandSetJCombo() {
    if (jComboBoxFunctionsFirstOperand.getItemCount() != 0) {
      String selectedFunctionItem = jComboBoxFunctionsFirstOperand
          .getSelectedItem().toString(); // Function name.
      int selectedFunctionType = getFunctionType(selectedFunctionItem); // Function type.
      Vector selectedFunctionParamTypes = getFunctionParameterTypes(selectedFunctionItem); // Function parameters.
      // Setting items in first operand, first, second, third parameter attributes combo boxes.
      // These items must have the same type as first operand function combo box selected item.
      jComboBoxFirstOperandParam1.removeAllItems();
      jComboBoxFirstOperandParam2.removeAllItems();
      jComboBoxFirstOperandParam3.removeAllItems();
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
                  jComboBoxFirstOperandParam1.addItem(getAttributeName(i));
                }
                jComboBoxFirstOperandParam2.addItem(getAttributeName(i)); // add all attributes in the arraylist.
              }
            } else {
              for (int i = 0; i < attributesArrayList.size(); i++) {
                if (parameterCount >= 1) {
                  if (getAttributeType(i) == Integer.parseInt(
                      selectedFunctionParamTypes.get(0).toString(), 10)) {
                    if (!getAttributeIsMultivalued(i)) {
                      jComboBoxFirstOperandParam1.addItem(getAttributeName(i));
                    }
                  }
                }
                if (parameterCount >= 2) {
                  if (getAttributeType(i) == Integer.parseInt(
                      selectedFunctionParamTypes.get(1).toString(), 10)) {
                    if (!getAttributeIsMultivalued(i)) {
                      jComboBoxFirstOperandParam2.addItem(getAttributeName(i));
                    }
                  }
                }
                if (parameterCount == 3) {
                  if (getAttributeType(i) == Integer.parseInt(
                      selectedFunctionParamTypes.get(2).toString(), 10)) {
                    if (!getAttributeIsMultivalued(i)) {
                      jComboBoxFirstOperandParam3.addItem(getAttributeName(i));
                    }
                  }
                }
              }
            }
          }
        }
      }
      //blankFirstOperandJCombo();
      // Setting items in second operand attributes combo box.
      // These items must have the same type as first operand function combo box selected item.
      jComboBoxAttributesSecondOperand.removeAllItems();
      for (int i = 0; i < attributesArrayList.size(); i++) {
        if (getAttributeType(i) == selectedFunctionType) {
          jComboBoxAttributesSecondOperand.addItem(getAttributeName(i));
        }
      }
      // Setting items in second operand functions combo box.
      // These items must have the same type as first operand function combo box selected item.
      jComboBoxFunctionsSecondOperand.removeAllItems();
      for (int i = 0; i < confFileFunctionsVector.size(); i++) {
        if (getFunctionType(i) == selectedFunctionType) {
          jComboBoxFunctionsSecondOperand.addItem(getFunctionName(i));
        }
      }
      blankFirstOperandJCombo();
      blankSecondOperandJCombo();
      functionsSecondOperandSetJCombo();
      //jComboBoxFunctionsSecondOperandEvent(null);
      functionsSecondOperandSetJCombo();
      functionsSecondOperandSetJRadio();
      switch (selectedFunctionType) {
        case Utils.INTEGER:
        case Utils.FLOAT:
        case Utils.ABS_TIME:
        case Utils.REL_TIME:
        case Utils.MEMORY:
        case Utils.SECONDS:
          jCheckBoxNot.setSelected(false);
          jCheckBoxNot.setEnabled(false);
          jComboBoxRelations.setEnabled(true);
          jComboBoxRelations.removeAllItems();
          for (int i = 0; i < relations.length; i++) {
            jComboBoxRelations.addItem(relations[i]);
          }
        break;
        case Utils.STRING:
          jCheckBoxNot.setSelected(false);
          jCheckBoxNot.setEnabled(false);
          jComboBoxRelations.setEnabled(true);
          jComboBoxRelations.removeAllItems();
          jComboBoxRelations.addItem("==");
          jComboBoxRelations.addItem("!=");
        break;
        case Utils.BOOLEAN:
          jCheckBoxNot.setEnabled(true);
          jComboBoxRelations.setEnabled(false);
      }
    }
  }

  void jComboBoxAttributesFirstOperandEvent(ActionEvent e) {
    attributesFirstOperandSetJCombo();
    attributesFirstOperandSetJRadio();
  }

  private void attributesFirstOperandSetJRadio() {
    if (jComboBoxAttributesFirstOperand.getItemCount() != 0) {
      //boolean bool = true;
      if (getAttributeType(jComboBoxAttributesFirstOperand.getSelectedItem()
          .toString()) == Utils.BOOLEAN) {
        //bool = false;
        jRadioButtonValueSecondOperand.setEnabled(false);
        jTextFieldValueSecondOperand.setEnabled(false);
        jRadioButtonAttributeSecondOperand.setEnabled(false);
        jComboBoxAttributesSecondOperand.setEnabled(false);
        jRadioButtonFunctionSecondOperand.setEnabled(false);
        jComboBoxFunctionsSecondOperand.setEnabled(false);
        jComboBoxSecondOperandParam1.setEnabled(false);
        jComboBoxSecondOperandParam2.setEnabled(false);
        jComboBoxSecondOperandParam3.setEnabled(false);
        jTextFieldValueSecondOperand.setVisible(true);
        setTimeTextFieldVisible(false);
        setDateTextFieldVisible(false);
        jLabelDate.setVisible(false);
        jRadioButtonValueSecondOperand.setText("Value");
      } else if (getAttributeType(jComboBoxAttributesFirstOperand
          .getSelectedItem().toString()) == Utils.ABS_TIME) {
        jTextFieldValueSecondOperand.setVisible(false);
        setTimeTextFieldVisible(true);
        setDateTextFieldVisible(true);
        jLabelDate.setVisible(true);
        jRadioButtonValueSecondOperand.setText("Time (hhmmss)");
        jRadioButtonValueSecondOperand.setEnabled(true);
        if (jComboBoxAttributesSecondOperand.getItemCount() == 0) {
          jRadioButtonAttributeSecondOperand.setEnabled(false);
          jRadioButtonValueSecondOperand.setSelected(true);
        } else {
          jRadioButtonAttributeSecondOperand.setEnabled(true);
        }
        if (jComboBoxFunctionsSecondOperand.getItemCount() == 0) {
          jRadioButtonFunctionSecondOperand.setEnabled(false);
          jRadioButtonValueSecondOperand.setSelected(true);
        } else {
          jRadioButtonFunctionSecondOperand.setEnabled(true);
        }
        if (jRadioButtonValueSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Value", SECONDOPERAND, null);
        } else if (jRadioButtonAttributeSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Attribute", SECONDOPERAND, null);
        } else if (jRadioButtonFunctionSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Function", SECONDOPERAND, null);
        }
      } else if (getAttributeType(jComboBoxAttributesFirstOperand
          .getSelectedItem().toString()) == Utils.REL_TIME) {
        jTextFieldValueSecondOperand.setVisible(false);
        setTimeTextFieldVisible(true);
        setDateTextFieldVisible(false);
        jLabelDate.setVisible(false);
        jRadioButtonValueSecondOperand.setText("Time (hhmmss)");
        jRadioButtonValueSecondOperand.setEnabled(true);
        if (jComboBoxAttributesSecondOperand.getItemCount() == 0) {
          jRadioButtonAttributeSecondOperand.setEnabled(false);
          jRadioButtonValueSecondOperand.setSelected(true);
        } else {
          jRadioButtonAttributeSecondOperand.setEnabled(true);
        }
        if (jComboBoxFunctionsSecondOperand.getItemCount() == 0) {
          jRadioButtonFunctionSecondOperand.setEnabled(false);
          jRadioButtonValueSecondOperand.setSelected(true);
        } else {
          jRadioButtonFunctionSecondOperand.setEnabled(true);
        }
        if (jRadioButtonValueSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Value", SECONDOPERAND, null);
        } else if (jRadioButtonAttributeSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Attribute", SECONDOPERAND, null);
        } else if (jRadioButtonFunctionSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Function", SECONDOPERAND, null);
        }
      } else {
        jTextFieldValueSecondOperand.setVisible(true);
        setTimeTextFieldVisible(false);
        setDateTextFieldVisible(false);
        jLabelDate.setVisible(false);
        jRadioButtonValueSecondOperand.setText("Value");
        jRadioButtonValueSecondOperand.setEnabled(true);
        if (jComboBoxAttributesSecondOperand.getItemCount() == 0) {
          jRadioButtonAttributeSecondOperand.setEnabled(false);
          jRadioButtonValueSecondOperand.setSelected(true);
        } else {
          jRadioButtonAttributeSecondOperand.setEnabled(true);
          jComboBoxAttributesSecondOperand.setEnabled(true);
        }
        if (jComboBoxFunctionsSecondOperand.getItemCount() == 0) {
          jRadioButtonFunctionSecondOperand.setEnabled(false);
          jRadioButtonValueSecondOperand.setSelected(true);
        } else {
          jRadioButtonFunctionSecondOperand.setEnabled(true);
          jComboBoxFunctionsSecondOperand.setEnabled(false);
        }
        if (jRadioButtonValueSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Value", SECONDOPERAND, null);
        } else if (jRadioButtonAttributeSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Attribute", SECONDOPERAND, null);
        } else if (jRadioButtonFunctionSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Function", SECONDOPERAND, null);
        }
      }
    }
    //jRadioButtonChangeEvent("Value", SECONDOPERAND, null);
  }

  private void jComboBoxFunctionsFirstOperandEvent(ActionEvent e) {
    functionsFirstOperandSetJCombo();
    functionsFirstOperandSetJRadio();
    functionsFirstOperandSetParamJText();
    functionsSecondOperandSetJCombo();
    functionsSecondOperandSetJRadio();
    //jComboBoxFunctionsSecondOperandEvent(null);
  }

  private void functionsFirstOperandSetJRadio() {
    if (jComboBoxFunctionsFirstOperand.getItemCount() != 0) {
      //jComboBoxAttributesFirstOperand.setToolTipText(jComboBoxAttributesFirstOperand.getSelectedItem().toString());
      String firstOperandSelectedFunction = jComboBoxFunctionsFirstOperand
          .getSelectedItem().toString();
      boolean bool = true;
      if (getFunctionType(firstOperandSelectedFunction) == Utils.BOOLEAN) {
        bool = false;
        jRadioButtonValueSecondOperand.setEnabled(bool);
        jTextFieldValueSecondOperand.setEnabled(bool);
        jRadioButtonAttributeSecondOperand.setEnabled(bool);
        jComboBoxAttributesSecondOperand.setEnabled(bool);
        jRadioButtonFunctionSecondOperand.setEnabled(bool);
        jComboBoxFunctionsSecondOperand.setEnabled(bool);
        jComboBoxSecondOperandParam1.setEnabled(bool);
        jComboBoxSecondOperandParam2.setEnabled(bool);
        jComboBoxSecondOperandParam3.setEnabled(bool);
        jTextFieldValueSecondOperand.setVisible(true);
        setTimeTextFieldVisible(false);
        setDateTextFieldVisible(false);
        jLabelDate.setVisible(false);
        jRadioButtonValueSecondOperand.setText("Value");
      } else if (getFunctionType(firstOperandSelectedFunction) == Utils.ABS_TIME) {
        jTextFieldValueSecondOperand.setVisible(false);
        setTimeTextFieldVisible(true);
        setDateTextFieldVisible(true);
        jLabelDate.setVisible(true);
        jRadioButtonValueSecondOperand.setText("Time (hhmmss)");
        jRadioButtonValueSecondOperand.setEnabled(bool);
        if (jComboBoxAttributesSecondOperand.getItemCount() == 0) {
          jRadioButtonAttributeSecondOperand.setEnabled(false);
          jRadioButtonValueSecondOperand.setSelected(true);
        } else {
          jRadioButtonAttributeSecondOperand.setEnabled(bool);
        }
        if (jComboBoxFunctionsSecondOperand.getItemCount() == 0) {
          jRadioButtonFunctionSecondOperand.setEnabled(false);
          jRadioButtonValueSecondOperand.setSelected(true);
        } else {
          jRadioButtonFunctionSecondOperand.setEnabled(bool);
        }
        if (jRadioButtonValueSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Value", SECONDOPERAND, null);
        } else if (jRadioButtonAttributeSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Attribute", SECONDOPERAND, null);
        } else if (jRadioButtonFunctionSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Function", SECONDOPERAND, null);
        }
      } else if (getFunctionType(firstOperandSelectedFunction) == Utils.REL_TIME) {
        jTextFieldValueSecondOperand.setVisible(false);
        setTimeTextFieldVisible(true);
        setDateTextFieldVisible(false);
        jLabelDate.setVisible(false);
        jRadioButtonValueSecondOperand.setText("Time (hhmmss)");
        jRadioButtonValueSecondOperand.setEnabled(bool);
        if (jComboBoxAttributesSecondOperand.getItemCount() == 0) {
          jRadioButtonAttributeSecondOperand.setEnabled(false);
          jRadioButtonValueSecondOperand.setSelected(true);
        } else {
          jRadioButtonAttributeSecondOperand.setEnabled(bool);
        }
        if (jComboBoxFunctionsSecondOperand.getItemCount() == 0) {
          jRadioButtonFunctionSecondOperand.setEnabled(false);
          jRadioButtonValueSecondOperand.setSelected(true);
        } else {
          jRadioButtonFunctionSecondOperand.setEnabled(bool);
        }
        if (jRadioButtonValueSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Value", SECONDOPERAND, null);
        } else if (jRadioButtonAttributeSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Attribute", SECONDOPERAND, null);
        } else if (jRadioButtonFunctionSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Function", SECONDOPERAND, null);
        }
      } else {
        jTextFieldValueSecondOperand.setVisible(true);
        setTimeTextFieldVisible(false);
        setDateTextFieldVisible(false);
        jLabelDate.setVisible(false);
        jRadioButtonValueSecondOperand.setText("Value");
        jRadioButtonValueSecondOperand.setEnabled(bool);
        if (jComboBoxAttributesSecondOperand.getItemCount() == 0) {
          jRadioButtonAttributeSecondOperand.setEnabled(false);
          jRadioButtonValueSecondOperand.setSelected(true);
        } else {
          jRadioButtonAttributeSecondOperand.setEnabled(bool);
        }
        if (jComboBoxFunctionsSecondOperand.getItemCount() == 0) {
          jRadioButtonFunctionSecondOperand.setEnabled(false);
          jRadioButtonValueSecondOperand.setSelected(true);
        } else {
          jRadioButtonFunctionSecondOperand.setEnabled(bool);
        }
        if (jRadioButtonValueSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Value", SECONDOPERAND, null);
        } else if (jRadioButtonAttributeSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Attribute", SECONDOPERAND, null);
        } else if (jRadioButtonFunctionSecondOperand.isSelected()) {
          jRadioButtonChangeEvent("Function", SECONDOPERAND, null);
        }
      }
    }
  }

  private void functionsFirstOperandSetParamJText() {
    if (jComboBoxFunctionsFirstOperand.getItemCount() != 0) {
      String selectedFunctionItem = jComboBoxFunctionsFirstOperand
          .getSelectedItem().toString();
      int parameterCount = 0;
      for (int j = 0; j < confFileFunctionsVector.size(); j++) {
        String functionName = ((Function) confFileFunctionsVector.get(j))
            .getName();
        if (selectedFunctionItem.equals(functionName)) {
          parameterCount = ((Function) confFileFunctionsVector.get(j))
              .getParameterCount();
        }
      }
      //int selectedIndex = jComboBoxFunctionsFirstOperand.getSelectedIndex();
      //int selectedFunctionParamNumber = paramNumber[selectedIndex];
      /* You have to add functions first param type structure!!
       int selectedFunctionParam1Type = param1Type[selectedIndex];
       jComboBoxFunctionsFirstOperand.removeAllItems();
       for(int i = 0; i < attributes.length; i++) {
       if(getAttributeType(i) == selectedFunctionParam1Type) {
       jComboBoxFunctionsFirstOperand.addItem(attributes[i]);
       }
       }
       */
      //switch(selectedFunctionParamNumber) {
      switch (parameterCount) {
        case Utils.INFINITE: //strcat()
        // Show different Frame.
        break;
        case 0: //unixTime()
          jComboBoxFirstOperandParam1.setVisible(false);
          jComboBoxFirstOperandParam2.setVisible(false);
          jComboBoxFirstOperandParam3.setVisible(false);
        break;
        case 1:
          // int(), real(), string(), floor(), ceiling(), round(), timeInterval(), localTimeString(), gmtTimeString()
          jComboBoxFirstOperandParam1.setVisible(true);
          jComboBoxFirstOperandParam2.setVisible(false);
          jComboBoxFirstOperandParam3.setVisible(false);
        break;
        case 2:
          // strcmp(), stricmp(), glob(), iglob()
          jComboBoxFirstOperandParam1.setVisible(true);
          jComboBoxFirstOperandParam2.setVisible(true);
          jComboBoxFirstOperandParam3.setVisible(false);
        break;
        case 3: //substr()
          jComboBoxFirstOperandParam1.setVisible(true);
          jComboBoxFirstOperandParam2.setVisible(true);
          jComboBoxFirstOperandParam3.setVisible(true);
        break;
      }
    }
  }

  private void functionsSecondOperandSetParamJText() {
    if (jComboBoxFunctionsSecondOperand.getItemCount() != 0) {
      String selectedFunctionItem = jComboBoxFunctionsSecondOperand
          .getSelectedItem().toString();
      int parameterCount = 0;
      for (int j = 0; j < confFileFunctionsVector.size(); j++) {
        String functionName = ((Function) confFileFunctionsVector.get(j))
            .getName();
        if (selectedFunctionItem.equals(functionName)) {
          parameterCount = ((Function) confFileFunctionsVector.get(j))
              .getParameterCount();
        }
      }
      //int selectedIndex = jComboBoxFunctionsFirstOperand.getSelectedIndex();
      //int selectedFunctionParamNumber = paramNumber[selectedIndex];
      /* You have to add functions first param type structure!!
       int selectedFunctionParam1Type = param1Type[selectedIndex];
       jComboBoxFunctionsFirstOperand.removeAllItems();
       for(int i = 0; i < attributes.length; i++) {
       if(getAttributeType(i) == selectedFunctionParam1Type) {
       jComboBoxFunctionsFirstOperand.addItem(attributes[i]);
       }
       }
       */
      //switch(selectedFunctionParamNumber) {
      switch (parameterCount) {
        case Utils.INFINITE: //strcat()
        // Show different Frame.
        break;
        case 0: //unixTime()
          jComboBoxSecondOperandParam1.setVisible(false);
          jComboBoxSecondOperandParam2.setVisible(false);
          jComboBoxSecondOperandParam3.setVisible(false);
        break;
        case 1:
          // int(), real(), string(), floor(), ceiling(), round(), timeInterval(), localTimeString(), gmtTimeString()
          jComboBoxSecondOperandParam1.setVisible(true);
          jComboBoxSecondOperandParam2.setVisible(false);
          jComboBoxSecondOperandParam3.setVisible(false);
        break;
        case 2:
          // strcmp(), stricmp(), glob(), iglob()
          jComboBoxSecondOperandParam1.setVisible(true);
          jComboBoxSecondOperandParam2.setVisible(true);
          jComboBoxSecondOperandParam3.setVisible(false);
        break;
        case 3: //substr()
          jComboBoxSecondOperandParam1.setVisible(true);
          jComboBoxSecondOperandParam2.setVisible(true);
          jComboBoxSecondOperandParam3.setVisible(true);
        break;
      }
    }
  }

  private void functionsSecondOperandSetJRadio() {
    if (jComboBoxFunctionsSecondOperand.getItemCount() != 0) {
      String selectedItem = jComboBoxFunctionsSecondOperand.getSelectedItem()
          .toString();
      int selectedFunctionParamCount = getFunctionParameterCount(selectedItem);
      /* You have to add functions first param type structure!!
       int selectedFunctionParam1Type = param1Type[selectedIndex];
       jComboBoxFunctionsSecondOperand.removeAllItems();
       for(int i = 0; i < attributes.length; i++) {
       if(getAttributeType(i) == selectedFunctionParam1Type) {
       jComboBoxFunctionsSecondOperand.addItem(attributes[i]);
       }
       }
       */
      switch (selectedFunctionParamCount) {
        case Utils.INFINITE: //strcat()
        // Show different Frame.
        break;
        case 0: //unixTime()
          jComboBoxSecondOperandParam1.setVisible(false);
          jComboBoxSecondOperandParam2.setVisible(false);
          jComboBoxSecondOperandParam3.setVisible(false);
        break;
        case 1:
          // int(), real(), string(), floor(), ceiling(), round(), timeInterval(), localTimeString(), gmtTimeString()
          jComboBoxSecondOperandParam1.setVisible(true);
          jComboBoxSecondOperandParam2.setVisible(false);
          jComboBoxSecondOperandParam3.setVisible(false);
        break;
        case 2:
          // strcmp(), stricmp(), glob(), iglob()
          jComboBoxSecondOperandParam1.setVisible(true);
          jComboBoxSecondOperandParam2.setVisible(true);
          jComboBoxSecondOperandParam3.setVisible(false);
        break;
        case 3: //substr()
          jComboBoxSecondOperandParam1.setVisible(true);
          jComboBoxSecondOperandParam2.setVisible(true);
          jComboBoxSecondOperandParam3.setVisible(true);
        break;
      }
    }
  }

  private void jComboBoxFunctionsSecondOperandEvent(ActionEvent e) {
    //functionsSecondOperandSetJCombo();
    //functionsSecondOperandSetJRadio();
    functionsSecondOperandSetJCombo();
    functionsSecondOperandSetParamJText();
  }

  private int getAttributeType(String attributeName) {
    for (int i = 0; i < attributesArrayList.size(); i++) {
      if ((((Attribute) attributesArrayList.get(i)).getName())
          .equals(attributeName)) {
        return ((Attribute) attributesArrayList.get(i)).getType();
      }
    }
    return Utils.UNKNOWN;
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
    return Utils.UNKNOWN;
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

  private boolean getAttributeIsMultivalued(String attributeName) {
    for (int i = 0; i < attributesArrayList.size(); i++) {
      if ((((Attribute) attributesArrayList.get(i)).getName())
          .equals(attributeName)) {
        return ((Attribute) attributesArrayList.get(i)).isMultivalued();
      }
    }
    return false;
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
    returnArray.add(new Integer(Utils.UNKNOWN));
    return returnArray;
  }

  private Vector getFunctionParameterTypes(int index) {
    return ((Function) confFileFunctionsVector.get(index))
        .getParameterTypeArray();
  }

  private void functionInitialize() {
    functionsArrayList.clear();
    for (int i = 0; i < confFileFunctionsVector.size(); i++) {
      functionsArrayList.add((Function) confFileFunctionsVector.get(i));
    }
    int size = confFileFunctionsVector.size();
    jComboBoxFunctionsFirstOperand.removeAllItems();
    jComboBoxFunctionsSecondOperand.removeAllItems();
    String functionName;
    for (int i = 0; i < size; i++) {
      functionName = ((Function) confFileFunctionsVector.get(i)).getName();
      jComboBoxFunctionsFirstOperand.addItem(functionName);
      jComboBoxFunctionsSecondOperand.addItem(functionName);
    }
  }

  private void radioButtonInitialize() {
    jRadioButtonAttributeFirstOperand.setSelected(true);
    jRadioButtonChangeEvent("Attribute", FIRSTOPERAND, null);
    jRadioButtonValueSecondOperand.setSelected(true);
    jRadioButtonChangeEvent("Value", SECONDOPERAND, null);
    if (attributesArrayList.size() > 0) {
      functionsFirstOperandSetJCombo();
      functionsFirstOperandSetParamJText();
      if (getAttributeType(0) == Utils.BOOLEAN) {
        boolean bool = false;
        jRadioButtonValueSecondOperand.setEnabled(bool);
        jTextFieldValueSecondOperand.setEnabled(bool);
        jRadioButtonAttributeSecondOperand.setEnabled(bool);
        jComboBoxAttributesSecondOperand.setEnabled(bool);
        jRadioButtonFunctionSecondOperand.setEnabled(bool);
        jComboBoxFunctionsSecondOperand.setEnabled(bool);
        jComboBoxSecondOperandParam1.setEnabled(bool);
        jComboBoxSecondOperandParam2.setEnabled(bool);
        jComboBoxSecondOperandParam3.setEnabled(bool);
      }
    }
  }

  private void attributeInitialize() {
    attributesArrayList.clear();
    for (int i = 0; i < confFileAttributesVector.size(); i++) {
      attributesArrayList.add((Attribute) confFileAttributesVector.get(i));
      //attributesNameVector.add(((Attribute) confFileAttributesVector.get(i)).getName());
    }
    //Collections.sort(attributesArrayList);
    jComboBoxAttributesFirstOperand.removeAllItems();
    jComboBoxAttributesSecondOperand.removeAllItems();
    for (int i = 0; i < attributesArrayList.size(); i++) {
      String currentAttribute = ((Attribute) attributesArrayList.get(i))
          .getName();
      //!!! 1/09 if(!((Attribute) attributesArrayList.get(i)).isMultivalued()) {
      attributesNameVector.add(((Attribute) confFileAttributesVector.get(i))
          .getName());
      jComboBoxAttributesFirstOperand.addItem(currentAttribute);
      jComboBoxAttributesSecondOperand.addItem(currentAttribute);
      //}
    }
    /*
     //Collections.sort(attributesNameVector);
     for(int i = 0; i < attributesNameVector.size(); i++) {
     jComboBoxAttributesFirstOperand.addItem(attributesNameVector.get(i).toString());
     jComboBoxAttributesSecondOperand.addItem(attributesNameVector.get(i).toString());
     }
     */
  }

  void jButtonRequirementsAdvancedPanelResetEvent(ActionEvent e) {
    // Reset initial status.
    functionInitialize();
    attributeInitialize();
    blankFirstOperandJCombo();
    blankSecondOperandJCombo();
    radioButtonInitialize();
    jTextFieldValueSecondOperand.setText("");
    jComboBoxOperators.setSelectedIndex(0);
    // Remove expression tree.
    removeAllChildrenNodes(rootNode);
    jTreeExpr.setShowsRootHandles(false);
    jTreeExpr.setRootVisible(false);
    jCheckBoxNot.setSelected(false);
    jint.setJTextAreaJDL("");
  }

  void blankFirstOperandJCombo() {
    jComboBoxFirstOperandParam1.setSelectedItem("");
    jComboBoxFirstOperandParam2.setSelectedItem("");
    jComboBoxFirstOperandParam3.setSelectedItem("");
  }

  void blankSecondOperandJCombo() {
    jComboBoxSecondOperandParam1.setSelectedItem("");
    jComboBoxSecondOperandParam2.setSelectedItem("");
    jComboBoxSecondOperandParam3.setSelectedItem("");
  }

  String jButtonRequirementsAdvancedPanelViewEvent(boolean showWarningMsg,
      boolean showErrorMsg, ActionEvent e) {
    warningMsg = "";
    errorMsg = "";
    String result = computeExp(rootNode);
    logger.debug("Result: " + result);
    //String defaultRequirements = GUIGlobalVars.getGUIConfVarRequirements();
    if (result.equals("")) { // && defaultRequirements.equals("")) {
      jint.setJTextAreaJDL("");
      return "";
    }
    /*if (result.equals("") || result.equals(defaultRequirements)
     || result.equals("(" + defaultRequirements + ")")) {
     result = Jdl.REQUIREMENTS + " = " + defaultRequirements + ";";
     } else if (!isDefaultValueInAnd(result)) {
     if (!defaultRequirements.equals("")) {
     result = Jdl.REQUIREMENTS + " = " + result + " && "
     + defaultRequirements + ";";
     } else {
     result = Jdl.REQUIREMENTS + " = " + result;
     }

     //!!! Pharenteses inserting.
     ClassAdParser cap = new ClassAdParser("[" + result + "]");
     result = cap.parse().toString();
     int length = result.length();
     result = result.substring(1, length - 1).trim() + ";\n";
     // END
     } else {*/
    result = Jdl.REQUIREMENTS + " = " + result + ";";
    //!!! Pharenteses inserting.
    ClassAdParser cap = new ClassAdParser("[" + result + "]");
    result = cap.parse().toString();
    int length = result.length();
    result = result.substring(1, length - 1).trim() + ";\n";
    // END
    //}
    warningMsg = ExprChecker.checkResult(result,
        Utils.requirementsAttributeArray);
    errorMsg = errorMsg.trim();
    warningMsg = warningMsg.trim();
    if (!errorMsg.equals("") && showErrorMsg) {
      GraphicUtils.showOptionDialogMsg(RequirementsAdvancedPanel.this,
          errorMsg, Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE, null,
          null);
      return "";
    } else {
      if (!warningMsg.equals("") && showWarningMsg) {
        GraphicUtils.showOptionDialogMsg(RequirementsAdvancedPanel.this,
            warningMsg, Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
            null, null);
      }
      jint.setJTextAreaJDL(result);
    }
    return result;
  }

  /**
   * Checks if the expression has the form user expression && default value
   * (level 1 of the tree).
   */
  boolean isDefaultValueInAnd(String expr) {
    String requirementsDefault = GUIGlobalVars.getGUIConfVarRequirements();
    if (requirementsDefault.equals("")) {
      return false;
    }
    int length = expr.length();
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
    String currentOperator2 = null;
    String currentOperator3 = null;
    String operatorType = null;
    logger.debug("requirementsDefault: " + requirementsDefault);
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
        if ((i + 3) < length) {
          currentOperator2 = expr.substring(i + 1, i + 3);
        } else {
          currentOperator2 = "";
        }
        if ((i + 5) < length) {
          currentOperator3 = expr.substring(i + 1, i + 5);
        } else {
          currentOperator3 = "";
        }
        if (currentOperator3.equals("&&(!")) {
          subExpr1 = expr.substring(0, i + 1);
          subExpr2 = expr.substring(i + 5, length - 1);
          operatorType = "AND NOT";
          haveToAdd = true;
        } else if (currentOperator3.equals("||(!")) {
          subExpr1 = expr.substring(0, i + 1);
          subExpr2 = expr.substring(i + 5, length - 1);
          operatorType = "OR NOT";
          haveToAdd = true;
        } else if (currentOperator2.equals("&&")) {
          subExpr1 = expr.substring(0, i + 1);
          subExpr2 = expr.substring(i + 3, length);
          operatorType = "AND";
          haveToAdd = true;
        } else if (currentOperator2.equals("||")) {
          subExpr1 = expr.substring(0, i + 1);
          subExpr2 = expr.substring(i + 3, length);
          operatorType = "OR";
          haveToAdd = true;
        }
        if (haveToAdd) {
          subExpr1 = "(" + subExpr1.trim() + ")";
          subExpr2 = "(" + subExpr2.trim() + ")";
          logger.debug("subExpr1: " + subExpr1);
          logger.debug("subExpr2: " + subExpr2);
          if (operatorType.equals("AND")) {
            if (subExpr1.equals(requirementsDefault)
                || subExpr1.equals("(" + requirementsDefault + ")")) {
              return true;
            }
            if (subExpr2.equals(requirementsDefault)
                || subExpr2.equals("(" + requirementsDefault + ")")) {
              return true;
            }
          }
          return false;
        }
      }
    }
    return false;
  }

  void setDateTextFieldVisible(boolean bool) {
    jTextFieldDay.setVisible(bool);
    upDay.setVisible(bool);
    downDay.setVisible(bool);
    jTextFieldMonth.setVisible(bool);
    upMonth.setVisible(bool);
    downMonth.setVisible(bool);
    jTextFieldYear.setVisible(bool);
    upYear.setVisible(bool);
    downYear.setVisible(bool);
  }

  void setTimeTextFieldVisible(boolean bool) {
    jTextFieldHours.setVisible(bool);
    upHours.setVisible(bool);
    downHours.setVisible(bool);
    jTextFieldMinutes.setVisible(bool);
    upMinutes.setVisible(bool);
    downMinutes.setVisible(bool);
    jTextFieldSeconds.setVisible(bool);
    upSeconds.setVisible(bool);
    downSeconds.setVisible(bool);
  }

  void setDateTextFieldEnabled(boolean bool) {
    jTextFieldDay.setEnabled(bool);
    upDay.setEnabled(bool);
    downDay.setEnabled(bool);
    jTextFieldMonth.setEnabled(bool);
    upMonth.setEnabled(bool);
    downMonth.setEnabled(bool);
    jTextFieldYear.setEnabled(bool);
    upYear.setEnabled(bool);
    downYear.setEnabled(bool);
  }

  void setTimeTextFieldEnabled(boolean bool) {
    jTextFieldHours.setEnabled(bool);
    upHours.setEnabled(bool);
    downHours.setEnabled(bool);
    jTextFieldMinutes.setEnabled(bool);
    upMinutes.setEnabled(bool);
    downMinutes.setEnabled(bool);
    jTextFieldSeconds.setEnabled(bool);
    upSeconds.setEnabled(bool);
    downSeconds.setEnabled(bool);
  }

  /*void jComboBoxAttributesSecondOperandEvent(ActionEvent e) {

   }*/
  //void jComboBoxFirstOperandParam2Event(ActionEvent e) {
  //System.out.println("Validating");
  //jComboBoxFirstOperandParam2.validate();
  //jComboBoxFirstOperandParam2.revalidate();
  //jComboBoxFirstOperandParam2.repaint();
  //}
  void jComboBoxFirstOperandParam1Event(ActionEvent e) {
    if (jComboBoxFunctionsFirstOperand.getItemCount() != 0) {
      String selectedFunctionItem = jComboBoxFunctionsFirstOperand
          .getSelectedItem().toString().trim(); // Function name.
      int selectedFunctionType = getFunctionType(selectedFunctionItem); // Function type.
      Vector selectedFunctionParamTypes = getFunctionParameterTypes(selectedFunctionItem);
      int parameterCount = selectedFunctionParamTypes.size();
      String selectedAttributeParam1 = "";
      if (jComboBoxFirstOperandParam1.getItemCount() != 0) {
        selectedAttributeParam1 = jComboBoxFirstOperandParam1.getEditor()
            .getItem().toString();
      } else {
        return;
      }
      if (Integer.parseInt(selectedFunctionParamTypes.get(0).toString(), 10) == Utils.LIST) {
        int parametersType = getAttributeType(selectedAttributeParam1);
        jComboBoxFirstOperandParam2.removeAllItems();
        jComboBoxFirstOperandParam3.removeAllItems();
        if ((selectedAttributeParam1.equals("") || !attributesNameVector
            .contains(selectedAttributeParam1))) {
          jComboBoxFirstOperandParam2.removeAllItems();
          jComboBoxFirstOperandParam3.removeAllItems();
          for (int i = 0; i < attributesArrayList.size(); i++) {
            if (parameterCount >= 2) {
              jComboBoxFirstOperandParam2.addItem(getAttributeName(i));
            }
            if (parameterCount == 3) {
              jComboBoxFirstOperandParam3.addItem(getAttributeName(i));
            }
          }
        } else {
          for (int i = 0; i < attributesArrayList.size(); i++) {
            if (parameterCount >= 2) {
              if (getAttributeType(i) == parametersType) {
                jComboBoxFirstOperandParam2.addItem(getAttributeName(i));
              }
            }
            if (parameterCount == 3) {
              if (getAttributeType(i) == parametersType) {
                jComboBoxFirstOperandParam3.addItem(getAttributeName(i));
              }
            }
          }
        }
      }
      jComboBoxFirstOperandParam2.setSelectedItem("");
      jComboBoxFirstOperandParam3.setSelectedItem("");
    }
  }

  void setAdvancedPanelEnabled(boolean bool) {
    jCheckBoxNot.setEnabled(bool);
    jRadioButtonAttributeFirstOperand.setEnabled(bool);
    jComboBoxAttributesFirstOperand.setEnabled(bool);
    jRadioButtonFunctionFirstOperand.setEnabled(bool);
    jComboBoxFunctionsFirstOperand.setEnabled(bool);
    jComboBoxFirstOperandParam1.setEnabled(bool);
    jComboBoxFirstOperandParam2.setEnabled(bool);
    jComboBoxFirstOperandParam3.setEnabled(bool);
    jLabelRelation.setEnabled(bool);
    jComboBoxRelations.setEnabled(bool);
    jRadioButtonAttributeSecondOperand.setEnabled(bool);
    jComboBoxAttributesSecondOperand.setEnabled(bool);
    jRadioButtonValueSecondOperand.setEnabled(bool);
    jTextFieldValueSecondOperand.setEnabled(bool);
    jRadioButtonFunctionSecondOperand.setEnabled(bool);
    jComboBoxFunctionsSecondOperand.setEnabled(bool);
    jComboBoxSecondOperandParam1.setEnabled(bool);
    jComboBoxSecondOperandParam2.setEnabled(bool);
    jComboBoxSecondOperandParam3.setEnabled(bool);
    jLabelOperator.setEnabled(bool);
    jComboBoxOperators.setEnabled(bool);
    jButtonAdd.setEnabled(bool);
    jButtonRemove.setEnabled(bool);
    jButtonClear.setEnabled(bool);
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

class Attribute { //implements comparable {
  String name;

  int type = 0;

  boolean isMultivalued;

  Attribute(String name, int type, boolean isMultivalued) {
    this.name = name;
    this.type = type;
    this.isMultivalued = isMultivalued;
  }

  public String getName() {
    return name;
  }

  public int getType() {
    return type;
  }

  public boolean isMultivalued() {
    return isMultivalued;
  }
  /*
   public int compareTo(Object o) {
   String attributeName = ((Attribute) o).getName();
   if (name.equals(attributeName)) {
   return 0;
   } else if (name < attributeName) {
   return -1;
   } else {
   return 1;
   }
   }
   */
}

class Function {
  String name;

  int type = 0;

  int parameterCount = 0;

  Vector parameterTypeArray;

  boolean isMultivalued;

  Function(String name, int type, int parameterCount, Vector parameterTypeArray) {
    this.name = name;
    this.type = type;
    this.parameterCount = parameterCount;
    this.parameterTypeArray = parameterTypeArray;
    this.isMultivalued = isMultivalued;
  }

  public String getName() {
    return name;
  }

  public int getType() {
    return type;
  }

  public int getParameterCount() {
    return parameterCount;
  }

  public Vector getParameterTypeArray() {
    return parameterTypeArray;
  }
}