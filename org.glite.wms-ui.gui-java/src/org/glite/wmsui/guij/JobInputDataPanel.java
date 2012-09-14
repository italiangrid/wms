/*
 * JobInputDataPanel.java
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
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.FocusEvent;
import java.util.Vector;
import javax.naming.directory.InvalidAttributeValueException;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Jdl;
import org.glite.jdl.JobAd;

/**
 * Implementation of the JobInputDataPanel class.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class JobInputDataPanel extends JPanel {
  static Logger logger = Logger.getLogger(JDLEditor.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final String LFN_PREFIX = "lfn:";

  static final String LCN_PREFIX = "lcn:";

  static final String GUID_PREFIX = "guid:";

  String errorMsg = "";

  String warningMsg = "";

  //String[] dataAccessProtocolProtocol = {"gridftp", "rfio", "file"};
  //String[] replicaCatalogProtocol = {"LDAP"};
  JPanel contentPane;

  //OSE JPanel jPanelDataROSE = new JPanel();
  JPanel jPanelDataRInputData = new JPanel();

  JTextField jTextFieldLFN = new JTextField();

  JButton jButtonRemoveLFN = new JButton();

  JButton jButtonAddLFN = new JButton();

  JButton jButtonClearLFN = new JButton();

  JButton jButtonClearPFN = new JButton();

  JTextField jTextFieldPFN = new JTextField();

  JButton jButtonRemovePFN = new JButton();

  JButton jButtonAddPFN = new JButton();

  JobAd jobAdGlobalJobInputDataPanel = new JobAd();

  //RC Vector confFileReplicaCatalogVector = new Vector();
  Vector confFileDataAccessProtocolVector = new Vector();

  JLabel jLabelLFN = new JLabel();

  JLabel jLabelPFN = new JLabel();

  JScrollPane jScrollPaneLFN = new JScrollPane();

  JList jListLFN = new JList();

  JScrollPane jScrollPanePFN = new JScrollPane();

  JList jListPFN = new JList();

  Vector LFNVector = new Vector();

  Vector PFNVector = new Vector();

  Vector GUIDVector = new Vector();

  Vector dataAccessProtocolVector = new Vector();

  JPanel jPanelDataAccessProtocol = new JPanel();

  //RC JPanel jPanelReplicaCatalog = new JPanel();
  JButton jButtonAddDataAccessProtocol = new JButton();

  JButton jButtonClearDataAccessProtocol = new JButton();

  JButton jButtonRemoveDataAccessProtocol = new JButton();

  JComboBox jComboBoxDataAccessProtocol = new JComboBox();

  //RC JComboBox jComboBoxReplicaCatalogProtocol = new JComboBox();
  //RC JTextField jTextFieldReplicaCatalogAddress = new JTextField();
  //RC JTextField jTextFieldReplicaCatalogPort = new JTextField();
  //RC JTextField jTextFieldReplicaCatalogDn = new JTextField();
  JLabel jLabelSlashes = new JLabel();

  //RC JLabel jLabelReplicaCatalogProtocol = new JLabel();
  //RC JLabel jLabelReplicaCatalogAddress = new JLabel();
  //RC JLabel jLabelReplicaCatalogPort = new JLabel();
  //RC JLabel jLabelReplicaCatalogDn = new JLabel();
  //JLabel jLabelColon = new JLabel();
  //OSE JTextField jTextFieldOutputSE = new JTextField();
  JScrollPane jScrollPaneDataAccessProtocol = new JScrollPane();

  JList jListDataAccessProtocol = new JList();

  //RC JButton jButtonClearReplicaCatalog = new JButton();
  JDLEditorInterface jint;

  Component component;

  JLabel jLabelSlash = new JLabel();

  boolean isApplet = false;

  JLabel jLabelGUID = new JLabel();

  JScrollPane jScrollPaneGUID = new JScrollPane();

  JButton jButtonRemoveGUID = new JButton();

  JButton jButtonClearGUID = new JButton();

  JTextField jTextFieldGUID = new JTextField();

  JList jListGUID = new JList();

  JButton jButtonAddGUID = new JButton();

  public JobInputDataPanel(Component component) {
    this.component = component;
    if (component instanceof JDLEditor) {
      jint = (JDLEditor) component;
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
    confFileDataAccessProtocolVector = GUIFileSystem
        .getConfigurationDataAccessProtocol(this);
    for (int i = 0; i < confFileDataAccessProtocolVector.size(); i++) {
      jComboBoxDataAccessProtocol.addItem(confFileDataAccessProtocolVector
          .get(i));
    }
    jListPFN.setBackground(Color.white);
    jListPFN.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jListPFNFocusLost(e);
      }
    });
    jListLFN.setBackground(Color.white);
    jListLFN.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jListLFNFocusLost(e);
      }
    });
    jButtonAddLFN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonAddLFNEvent(e);
      }
    });
    jButtonAddPFN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonAddPFNEvent(e);
      }
    });
    jButtonClearLFN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonClearLFNEvent(e);
      }
    });
    jButtonClearPFN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonClearPFNEvent(e);
      }
    });
    jButtonRemoveLFN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonRemoveLFNEvent(e);
      }
    });
    jButtonRemovePFN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonRemovePFNEvent(e);
      }
    });
    //RC jPanelReplicaCatalog.setBorder(new TitledBorder(new EtchedBorder(), " Replica Catalog ", 0, 0,
    //RC           null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    //RC jPanelReplicaCatalog.setLayout(null);
    jButtonAddDataAccessProtocol.setText("Add");
    jButtonAddDataAccessProtocol
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jButtonAddDataAccessProtocolEvent(e);
          }
        });
    jButtonClearDataAccessProtocol.setText("Clear");
    jButtonClearDataAccessProtocol
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jButtonClearDataAccessProtocolEvent(e);
          }
        });
    jButtonRemoveDataAccessProtocol.setText("Remove");
    jButtonRemoveDataAccessProtocol
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jButtonRemoveDataAccessProtocolEvent(e);
          }
        });
    jComboBoxDataAccessProtocol.setEditable(true); // Editable combo box, add FocusLost to editor below.
    jComboBoxDataAccessProtocol.getEditor().getEditorComponent()
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            jComboBoxDataAccessProtocolFocusLost(e);
          }
        });
    /* RC
     jComboBoxReplicaCatalogProtocol.setEditable(true);
     jTextFieldReplicaCatalogAddress.addFocusListener(new java.awt.event.FocusAdapter() {
     public void focusLost(FocusEvent e) {
     GraphicUtils.jTextFieldDeselect(jTextFieldReplicaCatalogAddress);
     }
     });
     jTextFieldReplicaCatalogPort.addFocusListener(new java.awt.event.FocusAdapter() {
     public void focusLost(FocusEvent e) {
     GraphicUtils.jTextFieldDeselect(jTextFieldReplicaCatalogPort);
     }
     });
     jTextFieldReplicaCatalogDn.addFocusListener(new java.awt.event.FocusAdapter() {
     public void focusLost(FocusEvent e) {
     GraphicUtils.jTextFieldDeselect(jTextFieldReplicaCatalogDn);
     }
     });
     //jLabelSlashes.setFont(new java.awt.Font("Dialog", 1, 12));
     jLabelSlashes.setText("://");
     jLabelReplicaCatalogProtocol.setText("Protocol");
     jLabelReplicaCatalogAddress.setText("Address");
     jLabelReplicaCatalogPort.setText("Port");
     jLabelReplicaCatalogDn.setText("Dn");
     jLabelColon.setText(":");
     RC */
    /* OSE
     jTextFieldOutputSE.addFocusListener(new java.awt.event.FocusAdapter() {
     public void focusLost(FocusEvent e) {
     GraphicUtils.jTextFieldDeselect(jTextFieldOutputSE);
     }
     });
     OSE */
    //OSE jPanelDataROSE.setLayout(null);
    jTextFieldLFN.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldLFN);
      }
    });
    jButtonRemoveLFN.setText("Remove");
    jButtonAddLFN.setText("Add");
    jButtonClearLFN.setText("Clear");
    jButtonClearPFN.setText("Clear");
    jTextFieldPFN.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldPFN);
      }
    });
    jButtonRemovePFN.setText("Remove");
    jButtonAddPFN.setText("Add");
    jLabelLFN.setText("Logical File Name");
    jLabelPFN.setText("Logical Collection Name");
    //OSE jPanelDataROSE.setBorder(new TitledBorder(new EtchedBorder(), " Output Storage Element ", 0, 0,
    //OSE           null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jListDataAccessProtocol.setBackground(Color.white);
    jListDataAccessProtocol.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jListDataAccessProtocolFocusLost(e);
      }
    });
    /* RC
     jButtonClearReplicaCatalog.addActionListener(new java.awt.event.ActionListener() {
     public void actionPerformed(ActionEvent e) {
     jButtonClearReplicaCatalogEvent(e);
     }
     });
     jButtonClearReplicaCatalog.setText("Clear");
     jLabelSlash.setText("/");
     RC */
    jLabelGUID.setText("Grid Unique IDentifier");
    jButtonRemoveGUID.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonRemoveGUIDEvent(e);
      }
    });
    jButtonRemoveGUID.setText("Remove");
    jButtonClearGUID.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonClearGUIDEvent(e);
      }
    });
    jButtonClearGUID.setText("Clear");
    jTextFieldGUID.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldDeselect(jTextFieldGUID);
      }
    });
    jButtonAddGUID.setText("Add");
    jButtonAddGUID.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonAddGUIDEvent(e);
      }
    });
    jListGUID.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jListGUIDFocusLost(e);
      }
    });
    //CNF jScrollPanePFN.getViewport().add(jListPFN, null);
    //RC this.add(jPanelReplicaCatalog, null);
    //OSE this.add(jPanelDataROSE, null);
    //OSE jPanelDataROSE.add(jTextFieldOutputSE, null);
    //jPanelDataROSE.add(jCheckBoxOutputSE, null);
    /* RC
     jPanelReplicaCatalog.add(jLabelReplicaCatalogProtocol, null);
     jPanelReplicaCatalog.add(jComboBoxReplicaCatalogProtocol, null);
     jPanelReplicaCatalog.add(jLabelSlashes, null);
     jPanelReplicaCatalog.add(jLabelReplicaCatalogAddress, null);
     jPanelReplicaCatalog.add(jTextFieldReplicaCatalogAddress, null);
     jPanelReplicaCatalog.add(jLabelColon, null);
     jPanelReplicaCatalog.add(jTextFieldReplicaCatalogDn, null);
     jPanelReplicaCatalog.add(jLabelReplicaCatalogPort, null);
     jPanelReplicaCatalog.add(jTextFieldReplicaCatalogPort, null);
     jPanelReplicaCatalog.add(jLabelSlash, null);
     jPanelReplicaCatalog.add(jLabelReplicaCatalogDn, null);
     jPanelReplicaCatalog.add(jButtonClearReplicaCatalog, null);
     RC */
    //CNF jPanelDataRInputData.add(jScrollPanePFN, null);
    /* CNF jPanelDataRInputData.add(jButtonRemovePFN, null);
     jPanelDataRInputData.add(jLabelPFN, null);
     jPanelDataRInputData.add(jTextFieldPFN, null);
     jPanelDataRInputData.add(jButtonClearPFN, null);
     jPanelDataRInputData.add(jButtonAddPFN, null);
     */
    // jPanelDataRInputData
    GridBagLayout gbl = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.insets = new Insets(3, 3, 3, 3);
    jPanelDataRInputData.setLayout(gbl);
    jPanelDataRInputData.setBorder(new TitledBorder(new EtchedBorder(),
        " Input Data ", 0, 0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelDataRInputData.add(jLabelLFN, GraphicUtils.setGridBagConstraints(gbc,
        0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.NONE, null, 0, 0));
    jPanelDataRInputData.add(jTextFieldLFN, GraphicUtils.setGridBagConstraints(
        gbc, 1, 0, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelDataRInputData.add(jButtonAddLFN, GraphicUtils.setGridBagConstraints(
        gbc, 2, 0, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jScrollPaneLFN.getViewport().add(jListLFN, null);
    jPanelDataRInputData.add(jScrollPaneLFN, GraphicUtils
        .setGridBagConstraints(gbc, 0, 1, 2, 2, 1.0, 0.5,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.BOTH, null,
            0, 0));
    jPanelDataRInputData.add(jButtonRemoveLFN, GraphicUtils
        .setGridBagConstraints(gbc, 2, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    jPanelDataRInputData.add(jButtonClearLFN,
        GraphicUtils
            .setGridBagConstraints(gbc, 2, 2, 1, 1, 0.0, 0.0,
                GridBagConstraints.NORTH, GridBagConstraints.HORIZONTAL, null,
                0, 0));
    // jPanelDataRInputData
    jPanelDataRInputData.setLayout(gbl);
    jPanelDataRInputData.setBorder(new TitledBorder(new EtchedBorder(),
        " Input Data ", 0, 0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelDataRInputData.add(jLabelGUID, GraphicUtils.setGridBagConstraints(
        gbc, 0, 3, 1, 1, 0.0, 0.0, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.NONE, null, 0, 0));
    jPanelDataRInputData.add(jTextFieldGUID, GraphicUtils
        .setGridBagConstraints(gbc, 1, 3, 1, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    jPanelDataRInputData.add(jButtonAddGUID, GraphicUtils
        .setGridBagConstraints(gbc, 2, 3, 1, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    jScrollPaneGUID.getViewport().add(jListGUID, null);
    jPanelDataRInputData.add(jScrollPaneGUID, GraphicUtils
        .setGridBagConstraints(gbc, 0, 4, 2, 2, 1.0, 0.5,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.BOTH, null,
            0, 0));
    jPanelDataRInputData.add(jButtonRemoveGUID, GraphicUtils
        .setGridBagConstraints(gbc, 2, 4, 1, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    jPanelDataRInputData.add(jButtonClearGUID,
        GraphicUtils
            .setGridBagConstraints(gbc, 2, 5, 1, 1, 0.0, 0.0,
                GridBagConstraints.NORTH, GridBagConstraints.HORIZONTAL, null,
                0, 0));
    // jPanelDataAccessProtocol
    jPanelDataAccessProtocol.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    jPanelDataAccessProtocol.setBorder(new TitledBorder(new EtchedBorder(),
        " Data Access Protocol ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelDataAccessProtocol.add(jComboBoxDataAccessProtocol, GraphicUtils
        .setGridBagConstraints(gbc, 0, 0, 1, 1, 1.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    jPanelDataAccessProtocol.add(jButtonAddDataAccessProtocol, GraphicUtils
        .setGridBagConstraints(gbc, 1, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    jPanelDataAccessProtocol.add(jScrollPaneDataAccessProtocol, GraphicUtils
        .setGridBagConstraints(gbc, 0, 1, 1, 2, 0.0, 1.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.BOTH, null,
            0, 0));
    jScrollPaneDataAccessProtocol.getViewport().add(jListDataAccessProtocol,
        null);
    jPanelDataAccessProtocol.add(jButtonRemoveDataAccessProtocol, GraphicUtils
        .setGridBagConstraints(gbc, 1, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.FIRST_LINE_START, GridBagConstraints.HORIZONTAL,
            null, 0, 0));
    jPanelDataAccessProtocol.add(jButtonClearDataAccessProtocol,
        GraphicUtils
            .setGridBagConstraints(gbc, 1, 2, 1, 1, 0.0, 0.0,
                GridBagConstraints.NORTH, GridBagConstraints.HORIZONTAL, null,
                0, 0));
    // this
    this.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    this.add(jPanelDataRInputData, GraphicUtils.setGridBagConstraints(gbc, 0,
        0, 1, 1, 1.0, 0.8, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, new Insets(1, 1, 1, 1), 0, 0));
    this.add(jPanelDataAccessProtocol, GraphicUtils.setGridBagConstraints(gbc,
        0, 1, 1, 1, 1.0, 0.2, GridBagConstraints.FIRST_LINE_START,
        GridBagConstraints.BOTH, null, 0, 0));
  }

  private void jButtonAddLFNEvent(ActionEvent e) {
    String LFNText = jTextFieldLFN.getText().trim();
    jTextFieldLFN.grabFocus();
    if (!LFNText.equals("")) { // && (!LFNVector.contains(LFNText))) {
      int choice = 0;
      if (LFNVector.contains(LFNText)) {
        choice = JOptionPane.showOptionDialog(JobInputDataPanel.this,
            "Inserted value is already present"
                + "\nDo you want to add it again?", Utils.WARNING_MSG_TXT,
            JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null, null,
            null);
      }
      if (choice == 0) {
        try {
          jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, LFN_PREFIX
              + LFNText);
          LFNVector.add(LFNText);
          jListLFN.setListData(LFNVector);
        } catch (IllegalArgumentException iae) {
          JOptionPane.showOptionDialog(JobInputDataPanel.this, "- "
              + iae.getMessage() + "\n", Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
              null, null);
        } catch (InvalidAttributeValueException iave) {
          JOptionPane.showOptionDialog(JobInputDataPanel.this, "- "
              + Jdl.INPUTDATA + ": wrong format\n", Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
              null, null);
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
          JOptionPane.showOptionDialog(JobInputDataPanel.this, ex.getMessage(),
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
        }
      }
    }
    jTextFieldLFN.selectAll();
  }

  private void jButtonAddPFNEvent(ActionEvent e) {
    String PFNText = jTextFieldPFN.getText().trim();
    jTextFieldPFN.grabFocus();
    if (!PFNText.equals("")) { // && (!PFNVector.contains(PFNText))) {
      int choice = 0;
      if (PFNVector.contains(PFNText)) {
        choice = JOptionPane.showOptionDialog(JobInputDataPanel.this,
            "Inserted value is already present"
                + "\nDo you want to add it again?", Utils.WARNING_MSG_TXT,
            JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null, null,
            null);
      }
      if (choice == 0) {
        try {
          //JobAd globalJobAd = new JobAd();
          jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, LCN_PREFIX
              + PFNText);
          PFNVector.add(PFNText);
          jListPFN.setListData(PFNVector);
        } catch (IllegalArgumentException iae) {
          JOptionPane.showOptionDialog(JobInputDataPanel.this, "- "
              + iae.getMessage() + "\n", Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
              null, null);
        } catch (InvalidAttributeValueException iave) {
          JOptionPane.showOptionDialog(JobInputDataPanel.this, "- "
              + Jdl.INPUTDATA + ": wrong format\n", Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
              null, null);
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
          JOptionPane.showOptionDialog(JobInputDataPanel.this, ex.getMessage(),
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
        }
      }
    }
    jTextFieldPFN.selectAll();
  }

  private void jButtonClearLFNEvent(ActionEvent e) {
    jTextFieldLFN.grabFocus();
    if (LFNVector.size() != 0) {
      int choice = JOptionPane.showOptionDialog(JobInputDataPanel.this,
          "Clear Logical File Name list?", "Confirm Clear",
          JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null,
          null);
      if (choice == 0) {
        LFNVector.removeAllElements();
        jListLFN.setListData(LFNVector);
        try {
          jobAdGlobalJobInputDataPanel.delAttribute(Jdl.INPUTDATA);
          for (int i = 0; i < PFNVector.size(); i++) {
            jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, LCN_PREFIX
                + PFNVector.get(i));
          }
          for (int i = 0; i < GUIDVector.size(); i++) {
            jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA,
                GUID_PREFIX + GUIDVector.get(i));
          }
        } catch (NoSuchFieldException nsfe) {
          // Attribute not present, do nothing.
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
          JOptionPane.showOptionDialog(JobInputDataPanel.this, ex.getMessage(),
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
        }
      }
    }
    jTextFieldLFN.selectAll();
  }

  private void jButtonClearPFNEvent(ActionEvent e) {
    jTextFieldPFN.grabFocus();
    if (PFNVector.size() != 0) {
      int choice = JOptionPane.showOptionDialog(JobInputDataPanel.this,
          "Clear Physical File Name list?", "Confirm Clear",
          JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null,
          null);
      if (choice == 0) {
        PFNVector.removeAllElements();
        jListPFN.setListData(PFNVector);
        try {
          jobAdGlobalJobInputDataPanel.delAttribute(Jdl.INPUTDATA);
          for (int i = 0; i < LFNVector.size(); i++) {
            jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, LFN_PREFIX
                + LFNVector.get(i));
          }
          for (int i = 0; i < GUIDVector.size(); i++) {
            jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA,
                GUID_PREFIX + GUIDVector.get(i));
          }
        } catch (NoSuchFieldException nsfe) {
          // Attribute not present, do nothing.
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
          JOptionPane.showOptionDialog(JobInputDataPanel.this, ex.getMessage(),
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
        }
      }
    }
    jTextFieldPFN.selectAll();
  }

  private void jButtonRemoveLFNEvent(ActionEvent e) {
    int[] selectedItems = jListLFN.getSelectedIndices();
    int selectedItemsCount = selectedItems.length;
    jTextFieldLFN.grabFocus();
    if (selectedItemsCount != 0) {
      for (int i = selectedItemsCount - 1; i >= 0; i--) {
        LFNVector.removeElementAt(selectedItems[i]);
      }
      jListLFN.setListData(LFNVector);
      try {
        jobAdGlobalJobInputDataPanel.delAttribute(Jdl.INPUTDATA);
        for (int i = 0; i < LFNVector.size(); i++) {
          jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, LFN_PREFIX
              + LFNVector.get(i));
        }
        for (int i = 0; i < PFNVector.size(); i++) {
          jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, LCN_PREFIX
              + PFNVector.get(i));
        }
        for (int i = 0; i < GUIDVector.size(); i++) {
          jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, GUID_PREFIX
              + GUIDVector.get(i));
        }
      } catch (IllegalArgumentException iae) {
        JOptionPane.showOptionDialog(JobInputDataPanel.this, Jdl.INPUTDATA
            + ": wrong format error", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        JOptionPane.showOptionDialog(component, ex.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
      if (jListLFN.getModel().getSize() != 0) {
        int selectableItem = selectedItems[selectedItemsCount - 1] + 1
            - selectedItemsCount; // Next.
        if (selectableItem > jListLFN.getModel().getSize() - 1) {
          selectableItem--; // Prev. (selectedItems[selectedItemsCount - 1] - selectedItemsCount).
        }
        jListLFN.setSelectedIndex(selectableItem);
      }
    } else {
      JOptionPane.showOptionDialog(JobInputDataPanel.this, Jdl.INPUTDATA + ": "
          + Utils.SELECT_AN_ITEM, Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
          null, null);
    }
    jTextFieldLFN.selectAll();
  }

  private void jButtonRemovePFNEvent(ActionEvent e) {
    int[] selectedItems = jListPFN.getSelectedIndices();
    int selectedItemsCount = selectedItems.length;
    jTextFieldPFN.grabFocus();
    if (selectedItemsCount != 0) {
      for (int i = selectedItemsCount - 1; i >= 0; i--) {
        PFNVector.removeElementAt(selectedItems[i]);
      }
      jListPFN.setListData(PFNVector);
      try {
        jobAdGlobalJobInputDataPanel.delAttribute(Jdl.INPUTDATA);
        for (int i = 0; i < LFNVector.size(); i++) {
          jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, LFN_PREFIX
              + LFNVector.get(i));
        }
        for (int i = 0; i < PFNVector.size(); i++) {
          jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, LCN_PREFIX
              + PFNVector.get(i));
        }
        for (int i = 0; i < GUIDVector.size(); i++) {
          jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, GUID_PREFIX
              + GUIDVector.get(i));
        }
      } catch (IllegalArgumentException iae) {
        JOptionPane.showOptionDialog(JobInputDataPanel.this, Jdl.INPUTDATA
            + ": wrong format error", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        JOptionPane.showOptionDialog(component, ex.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
      if (jListPFN.getModel().getSize() != 0) {
        int selectableItem = selectedItems[selectedItemsCount - 1] + 1
            - selectedItemsCount; // Next
        if (selectableItem > jListPFN.getModel().getSize() - 1) {
          selectableItem--; // Prev. (selectedItems[selectedItemsCount - 1] - selectedItemsCount).
        }
        jListPFN.setSelectedIndex(selectableItem);
      }
    } else {
      JOptionPane.showOptionDialog(JobInputDataPanel.this, Jdl.INPUTDATA + ": "
          + Utils.SELECT_AN_ITEM, Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
          null, null);
    }
    jTextFieldPFN.selectAll();
  }

  void jButtonDataReqResetEvent(ActionEvent e) {
    jTextFieldLFN.setText("");
    LFNVector.removeAllElements();
    jListLFN.setListData(LFNVector);
    jTextFieldPFN.setText("");
    PFNVector.removeAllElements();
    jListPFN.setListData(PFNVector);
    jTextFieldGUID.setText("");
    GUIDVector.removeAllElements();
    jListGUID.setListData(GUIDVector);
    //OSE jTextFieldOutputSE.setText("");
    dataAccessProtocolVector.removeAllElements();
    jListDataAccessProtocol.setListData(dataAccessProtocolVector);
    if (jComboBoxDataAccessProtocol.getItemCount() != 0) {
      jComboBoxDataAccessProtocol.setSelectedIndex(0);
    }
    /*RC
     jComboBoxReplicaCatalogProtocol.setSelectedIndex(0);
     jTextFieldReplicaCatalogAddress.setText("");
     jTextFieldReplicaCatalogPort.setText("");
     jTextFieldReplicaCatalogDn.setText("");
     RC */
    jint.setJTextAreaJDL("");
    jTextFieldLFN.grabFocus();
    jobAdGlobalJobInputDataPanel.clear();
  }

  void jButtonAddDataAccessProtocolEvent(ActionEvent e) {
    String dataAccessProtocolText = jComboBoxDataAccessProtocol
        .getSelectedItem().toString();
    if (dataAccessProtocolText.equals("")) {
      if (jComboBoxDataAccessProtocol.getItemCount() != 0) {
        jComboBoxDataAccessProtocol.setSelectedIndex(0);
      }
      return;
    } else if (!dataAccessProtocolText.equals("")
        && (!dataAccessProtocolVector.contains(dataAccessProtocolText))) {
      try {
        jobAdGlobalJobInputDataPanel.addAttribute(Jdl.DATA_ACCESS,
            dataAccessProtocolText);
        dataAccessProtocolVector.add(dataAccessProtocolText);
        jListDataAccessProtocol.setListData(dataAccessProtocolVector);
        //jTextFieldDataAccessProtocol.setText("");
      } catch (IllegalArgumentException iae) {
        JOptionPane.showOptionDialog(component, "- " + iae.getMessage() + "\n",
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        return;
      } catch (InvalidAttributeValueException iave) {
        JOptionPane.showOptionDialog(JobInputDataPanel.this, "- "
            + iave.getMessage() + "\n", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        JOptionPane.showOptionDialog(component, ex.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        return;
      }
    }
  }

  void jButtonClearDataAccessProtocolEvent(ActionEvent e) {
    if (dataAccessProtocolVector.size() != 0) {
      int choice = JOptionPane.showOptionDialog(JobInputDataPanel.this,
          "Clear Data Access Protocol list?", "Confirm Clear",
          JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null,
          null);
      if (choice == 0) {
        dataAccessProtocolVector.removeAllElements();
        jListDataAccessProtocol.setListData(dataAccessProtocolVector);
      }
    }
    JTextField jTextField = (JTextField) jComboBoxDataAccessProtocol
        .getEditor().getEditorComponent();
    jTextField.grabFocus();
    jTextField.selectAll();
  }

  void jButtonRemoveDataAccessProtocolEvent(ActionEvent e) {
    int[] selectedItems = jListDataAccessProtocol.getSelectedIndices();
    int selectedItemsCount = selectedItems.length;
    if (selectedItemsCount != 0) {
      for (int i = selectedItemsCount - 1; i >= 0; i--) {
        dataAccessProtocolVector.removeElementAt(selectedItems[i]);
      }
      jListDataAccessProtocol.setListData(dataAccessProtocolVector);
      if (jListDataAccessProtocol.getModel().getSize() != 0) {
        int selectableItem = selectedItems[selectedItemsCount - 1] + 1
            - selectedItemsCount; // Next.
        if (selectableItem > jListDataAccessProtocol.getModel().getSize() - 1) {
          selectableItem--; // Prev. (selectedItems[selectedItemsCount - 1] - selectedItemsCount).
        }
        jListDataAccessProtocol.setSelectedIndex(selectableItem);
      }
    } else {
      JOptionPane.showOptionDialog(JobInputDataPanel.this,
          Utils.SELECT_AN_ITEM, Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
          null, null);
    }
    JTextField jTextField = (JTextField) jComboBoxDataAccessProtocol
        .getEditor().getEditorComponent();
    jTextField.grabFocus();
    jTextField.selectAll();
  }

  String jButtonDataReqViewEvent(boolean showWarningMsg, boolean showErrorMsg,
      ActionEvent e) {
    String result = "";
    warningMsg = "";
    errorMsg = "";
    boolean isWarningMsgInputData = false;
    int jListLFNItemCount = LFNVector.size();
    int jListPFNItemCount = PFNVector.size();
    int jListGUIDItemCount = GUIDVector.size();
    int totItem = jListLFNItemCount + jListPFNItemCount + jListGUIDItemCount;
    JobAd jobAdCheck = new JobAd();
    if (totItem != 0) {
      if (totItem == 1) {
        if (jListLFNItemCount == 1) {
          result += Jdl.INPUTDATA + " = \"" + LFN_PREFIX + LFNVector.get(0)
              + "\";\n";
        } else if (jListPFNItemCount == 1) {
          result += Jdl.INPUTDATA + " = \"" + LCN_PREFIX + PFNVector.get(0)
              + "\";\n";
        } else {
          result += Jdl.INPUTDATA + " = \"" + GUID_PREFIX + GUIDVector.get(0)
              + "\";\n";
        }
      } else {
        result += Jdl.INPUTDATA + " = {";
        for (int i = 0; i < jListLFNItemCount; i++) {
          result += "\"" + LFN_PREFIX + LFNVector.get(i) + "\", ";
        }
        for (int i = 0; i < jListPFNItemCount; i++) {
          result += "\"" + LCN_PREFIX + PFNVector.get(i) + "\", ";
        }
        if (jListGUIDItemCount == 0) {
          result = result.substring(0, result.length() - 2) + "};\n";
        } else {
          for (int i = 0; i < jListGUIDItemCount - 1; i++) {
            result += "\"" + GUID_PREFIX + GUIDVector.get(i) + "\", ";
          }
          result += "\"" + GUID_PREFIX + GUIDVector.get(jListGUIDItemCount - 1)
              + "\"};\n";
        }
      }
    }
    /* RC
     String protocolText = "";
     if(jComboBoxReplicaCatalogProtocol.getEditor().getItem() != null)
     protocolText = jComboBoxReplicaCatalogProtocol.getSelectedItem().toString().trim();
     String addressText = jTextFieldReplicaCatalogAddress.getText().trim();
     String portText = jTextFieldReplicaCatalogPort.getText().trim();
     String dnText = jTextFieldReplicaCatalogDn.getText().trim();
     String tempResult = "";
     if(addressText.equals("") && portText.equals("") && dnText.equals("")) {
     } else {
     if(protocolText.equals("")) errorMsg += "- ReplicaCatalog: protocol field cannot be blank\n";
     else tempResult += protocolText + "://";
     if(addressText.equals("")) errorMsg += "- ReplicaCatalog: address field cannot be blank\n";
     else {
     //int dotNumber = 0;
     //for(int i = 0; i < addressText.length(); i++)
     // if(addressText.charAt(i) == '.') dotNumber++;
     //if(dotNumber < 2) errorMsg += "- ReplicaCatalog: address field has wrong format\n";
     //else tempResult += addressText + ":";
     tempResult += addressText + ":";
     }
     if(portText.equals("")) errorMsg += "- ReplicaCatalog: port field cannot be blank\n";
     else {
     if(Utils.verify(jTextFieldReplicaCatalogPort, Utils.INTEGER))
     tempResult += portText + "/";
     else errorMsg += "- ReplicaCatalog: wrong port number\n";
     }
     if(dnText.equals("")) errorMsg += "- ReplicaCatalog: dn field cannot be blank\n";
     else tempResult += dnText;
     if(!protocolText.equals("") && !addressText.equals("") && !portText.equals("") && !dnText.equals("")) {
     if(checkAttributeSet("ReplicaCatalog", tempResult))
     result += "ReplicaCatalog = \"" + tempResult + "\";\n";
     }
     }
     RC */
    //!!! use following line only during test
    //result += "ReplicaCatalog = \"http://pippo.pluto.it:80/topolino\";\n";
    int jListDataAccessProtocolCount = dataAccessProtocolVector.size();
    if (jListDataAccessProtocolCount == 0) {
      if (LFNVector.size() != 0 || PFNVector.size() != 0) {
        //if(!isWarningMsgInputData)
        //warningMsg += "- InputData, ReplicaCatalog, DataAccessProtocol: attribute must be specified all togheter\n";
      }
    } else {
      result += Jdl.DATA_ACCESS + " = ";
      if (jListDataAccessProtocolCount == 1) {
        result += "\"" + dataAccessProtocolVector.get(0) + "\";\n";
      } else {
        result += "{";
        for (int i = 0; i < jListDataAccessProtocolCount - 1; i++) {
          result += "\"" + dataAccessProtocolVector.get(i) + "\", ";
        }
        result += "\""
            + dataAccessProtocolVector.get(jListDataAccessProtocolCount - 1)
            + "\"};\n";
      }
    }
    /* OSE
     String outputSEText = jTextFieldOutputSE.getText().trim();
     if(!outputSEText.equals("")) {
     if(checkAttributeSet("OutputSE", outputSEText));
     result += "OutputSE = \"" + outputSEText + "\";\n";
     }
     OSE */
    warningMsg = ExprChecker.checkResult(result,
        Utils.jobInputDataAttributeArray);
    errorMsg = errorMsg.trim();
    warningMsg = warningMsg.trim();
    if (!errorMsg.equals("") && showErrorMsg) {
      GraphicUtils.showOptionDialogMsg(JobInputDataPanel.this, errorMsg,
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE, null,
          null);
    } else {
      if (!warningMsg.equals("") && showWarningMsg) {
        GraphicUtils.showOptionDialogMsg(JobInputDataPanel.this, warningMsg,
            Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
            null, null);
      }
      jint.setJTextAreaJDL(result);
    }
    return result;
  }

  boolean checkAttributeSet(String attribute, String value) {
    JobAd jobAdCheck = new JobAd();
    try {
      jobAdCheck.setAttribute(attribute, value);
    } catch (IllegalArgumentException iae) {
      errorMsg += "- " + iae.getMessage() + "\n";
      return false;
    } catch (InvalidAttributeValueException iave) {
      errorMsg += "- " + iave.getMessage() + "\n";
      return false;
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
      JOptionPane.showOptionDialog(component, ex.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
    }
    return true;
  }

  String getErrorMsg() {
    return this.errorMsg;
  }

  String getWarningMsg() {
    return this.warningMsg;
  }

  void jListLFNFocusLost(FocusEvent e) {
    Component hasFocusComponent = e.getOppositeComponent(); // Get component which "has the focus".
    if (hasFocusComponent == jButtonRemoveLFN) {
      return;
    }
    jListLFN.clearSelection();
  }

  void jListPFNFocusLost(FocusEvent e) {
    Component hasFocusComponent = e.getOppositeComponent(); // Get component which "has the focus".
    if (hasFocusComponent == jButtonRemovePFN) {
      return;
    }
    jListPFN.clearSelection();
  }

  void jListDataAccessProtocolFocusLost(FocusEvent e) {
    Component hasFocusComponent = e.getOppositeComponent(); // Get component which "has the focus".
    if (hasFocusComponent == jButtonRemoveDataAccessProtocol) {
      return;
    }
    jListDataAccessProtocol.clearSelection();
  }

  void jListGUIDFocusLost(FocusEvent e) {
    Component hasFocusComponent = e.getOppositeComponent(); // Get component which "has the focus".
    if (hasFocusComponent == jButtonRemoveGUID) {
      return;
    }
    jListGUID.clearSelection();
  }

  void jButtonRemoveGUIDEvent(ActionEvent e) {
    int[] selectedItems = jListGUID.getSelectedIndices();
    int selectedItemsCount = selectedItems.length;
    jTextFieldGUID.grabFocus();
    if (selectedItemsCount != 0) {
      for (int i = selectedItemsCount - 1; i >= 0; i--) {
        GUIDVector.removeElementAt(selectedItems[i]);
      }
      jListGUID.setListData(GUIDVector);
      try {
        jobAdGlobalJobInputDataPanel.delAttribute(Jdl.INPUTDATA);
        for (int i = 0; i < LFNVector.size(); i++) {
          jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, LFN_PREFIX
              + LFNVector.get(i));
        }
        for (int i = 0; i < PFNVector.size(); i++) {
          jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, LCN_PREFIX
              + PFNVector.get(i));
        }
        for (int i = 0; i < GUIDVector.size(); i++) {
          jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, GUID_PREFIX
              + GUIDVector.get(i));
        }
      } catch (IllegalArgumentException iae) {
        JOptionPane.showOptionDialog(JobInputDataPanel.this, Jdl.INPUTDATA
            + ": wrong format error", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        JOptionPane.showOptionDialog(component, ex.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
      if (jListGUID.getModel().getSize() != 0) {
        int selectableItem = selectedItems[selectedItemsCount - 1] + 1
            - selectedItemsCount; // Next
        if (selectableItem > jListGUID.getModel().getSize() - 1) {
          selectableItem--; // Prev. (selectedItems[selectedItemsCount - 1] - selectedItemsCount).
        }
        jListGUID.setSelectedIndex(selectableItem);
      }
    } else {
      JOptionPane.showOptionDialog(JobInputDataPanel.this, Jdl.INPUTDATA + ": "
          + Utils.SELECT_AN_ITEM, Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
          null, null);
    }
    jTextFieldGUID.selectAll();
  }

  void jButtonClearGUIDEvent(ActionEvent e) {
    jTextFieldGUID.grabFocus();
    if (GUIDVector.size() != 0) {
      int choice = JOptionPane.showOptionDialog(JobInputDataPanel.this,
          "Clear Grid Unique IDentifier list?", "Confirm Clear",
          JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null,
          null);
      if (choice == 0) {
        GUIDVector.removeAllElements();
        jListGUID.setListData(GUIDVector);
        try {
          jobAdGlobalJobInputDataPanel.delAttribute(Jdl.INPUTDATA);
          for (int i = 0; i < LFNVector.size(); i++) {
            jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, LFN_PREFIX
                + LFNVector.get(i));
          }
          for (int i = 0; i < PFNVector.size(); i++) {
            jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, LCN_PREFIX
                + PFNVector.get(i));
          }
        } catch (NoSuchFieldException nsfe) {
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
          JOptionPane.showOptionDialog(JobInputDataPanel.this, ex.getMessage(),
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
        }
      }
    }
    jTextFieldGUID.selectAll();
  }

  void jButtonAddGUIDEvent(ActionEvent e) {
    //JobAd jobAdCheck = new JobAd();
    String GUIDText = jTextFieldGUID.getText().trim();
    jTextFieldGUID.grabFocus();
    if (!GUIDText.equals("")) { // && (!GUIDVector.contains(GUIDText))) {
      int choice = 0;
      if (GUIDVector.contains(GUIDText)) {
        choice = JOptionPane.showOptionDialog(JobInputDataPanel.this,
            "Inserted value is already present"
                + "\nDo you want to add it again?", Utils.WARNING_MSG_TXT,
            JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null, null,
            null);
      }
      if (choice == 0) {
        try {
          jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, GUID_PREFIX
              + GUIDText);
          GUIDVector.add(GUIDText);
          jListGUID.setListData(GUIDVector);
        } catch (IllegalArgumentException iae) {
          JOptionPane.showOptionDialog(JobInputDataPanel.this, "- "
              + iae.getMessage() + "\n", Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
              null, null);
        } catch (InvalidAttributeValueException iave) {
          JOptionPane.showOptionDialog(JobInputDataPanel.this, "- "
              + Jdl.INPUTDATA + ": wrong format\n", Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
              null, null);
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
          JOptionPane.showOptionDialog(JobInputDataPanel.this, ex.getMessage(),
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
        }
      }
    }
    jTextFieldGUID.selectAll();
  }

  String setInputDataList(Vector itemVector) {
    String warningMsg = "";
    String item = "";
    LFNVector.clear();
    GUIDVector.clear();
    for (int i = 0; i < itemVector.size(); i++) {
      item = itemVector.get(i).toString().trim();

// WARNING: TEMPORAL PATCH BEGIN!!!
	if (item.startsWith( "si")) {
		warningMsg +="- " + Jdl.INPUTDATA + ": " +item+" prefix not yet supported.\nIt will be converted into a known prefix" ;
		item=item.substring(3);
	}
// WARNING: TEMPORAL PATCH END!!!


      //if(item.substring(0, LFN_PREFIX.length()).toUpperCase().equals(LFN_PREFIX)) {
      if (item.substring(0, LFN_PREFIX.length()).equals(LFN_PREFIX)) {
        LFNVector.add(item.substring(LFN_PREFIX.length()).trim());
      }
      //else if (item.substring(0, LCN_PREFIX.length()).toUpperCase().equals(LCN_PREFIX)) {
      else if (item.substring(0, GUID_PREFIX.length()).equals(GUID_PREFIX)) {
        GUIDVector.add(item.substring(GUID_PREFIX.length()).trim());
      } 

// WARNING: TEMPORAL PATCH BEGIN!!!
      else {
	warningMsg +="- " + Jdl.INPUTDATA + ": " +item+" unable to load known prefix";
      }
// WARNING: TEMPORAL PATCH END!!!

      try {
        jobAdGlobalJobInputDataPanel.addAttribute(Jdl.INPUTDATA, item);
      } catch (IllegalArgumentException iae) {
        warningMsg += "- " + iae.getMessage() + "\n";
      } catch (InvalidAttributeValueException iave) {
        warningMsg += "- " + Jdl.INPUTDATA + ": wrong format\n";
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        JOptionPane.showOptionDialog(JobInputDataPanel.this, ex.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
    }
    jListLFN.setListData(LFNVector);
    jListGUID.setListData(GUIDVector);

    return warningMsg;
  }

  void setDataAccessProtocolList(Vector itemVector) {
    String item = "";
    dataAccessProtocolVector.clear();
    for (int i = 0; i < itemVector.size(); i++) {
      item = itemVector.get(i).toString().trim();
      dataAccessProtocolVector.add(item);
    }
    //dataReq.setDataAccessProtocolEnabled(true);
    jListDataAccessProtocol.setListData(dataAccessProtocolVector);
  }

  void jComboBoxDataAccessProtocolFocusLost(FocusEvent e) {
    ((JTextField) jComboBoxDataAccessProtocol.getEditor().getEditorComponent())
        .select(0, 0);
  }
  /* RC
   void jButtonClearReplicaCatalogEvent(ActionEvent e) {
   jComboBoxReplicaCatalogProtocol.setSelectedIndex(0);
   jTextFieldReplicaCatalogAddress.setText("");
   jTextFieldReplicaCatalogPort.setText("");
   jTextFieldReplicaCatalogDn.setText("");
   jTextFieldReplicaCatalogAddress.grabFocus();
   }
   RC */
  /* RC
   void setReplicaCatalogText(String text) {
   int length = text.length();
   char currentChar;
   String protocol = "";
   String address = "";
   String port = "";
   String dn = "";
   int j = 0;
   boolean firstColonFound = false;
   for(int i = 0; i < length; i++) {
   currentChar = text.charAt(i);
   if(currentChar == ':' && !firstColonFound) {
   firstColonFound = true;
   protocol = text.substring(0, i);
   j = i + 1;
   } else if(currentChar == ':' && firstColonFound) {
   address = text.substring(j + 2, i);
   j = i + 1;
   } else if(currentChar == '/') {
   port = text.substring(j, i);
   dn = text.substring(i + 1, length);
   }
   }
   int dataAccessProtocolCount = confFileDataAccessProtocolVector.size();
   for(int i = 0; i < dataAccessProtocolCount; i++) {
   if(protocol.toUpperCase().equals(confFileDataAccessProtocolVector.get(i).toString().toUpperCase())) {
   jComboBoxReplicaCatalogProtocol.setSelectedIndex(i);
   break;
   }
   }
   //setReplicaCatalogEnabled(true);
   jTextFieldReplicaCatalogAddress.setText(address);
   jTextFieldReplicaCatalogPort.setText(port);
   jTextFieldReplicaCatalogDn.setText(dn);
   }
   RC */
}

