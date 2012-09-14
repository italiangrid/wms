/*
 * JobDef1Panel.java
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
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.KeyEvent;
import java.io.File;
import java.net.URL;
import java.util.Vector;
import javax.naming.directory.InvalidAttributeValueException;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.ExtractFiles;
import org.glite.jdl.Jdl;

/**
 * Implementation of the JobDef1Panel class.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class JobDef1Panel extends JPanel {
  static Logger logger = Logger.getLogger(JDLEditor.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  String warningMsg = "";

  String GUIWarningMsg = "";

  String errorMsg = "";

  final int NO_MATCH_ERROR = 0;

  final int MATCH_ERROR_EXECUTABLE_STDINPUT = -1;

  final int MATCH_ERROR_STDOUTPUT_STDERROR = -2;

  final String MATCH_ERROR_EXECUTABLE_STDINPUT_MSG = "- " + Jdl.EXECUTABLE
      + ", " + Jdl.STDINPUT + ": inserted files must be different";

  JPanel contentPane;

  JButton jButtonFileExecutable = new JButton();

  JLabel jLabelExecutable = new JLabel();

  JLabel jLabelArguments = new JLabel();

  JLabel jLabelStdInput = new JLabel();

  JLabel jLabelStdOutput = new JLabel();

  JLabel jLabelStdError = new JLabel();

  JPanel jPanelExecutable = new JPanel();

  JTextField jTextFieldExecutable = new JTextField();

  JTextField jTextFieldStdInput = new JTextField();

  JTextField jTextFieldArguments = new JTextField();

  JPanel jPanelStandardStreams = new JPanel();

  JTextField jTextFieldStdOutput = new JTextField();

  JTextField jTextFieldStdError = new JTextField();

  JPanel jPanelJobDInputSandbox = new JPanel();

  JPanel jPanelJobDOutputSandbox = new JPanel();

  JTextField jTextFieldInputSb = new JTextField();

  JTextField jTextFieldOutputSb = new JTextField();

  JButton jButtonRemoveInputSb = new JButton();

  JButton jButtonAddInputSb = new JButton();

  JButton jButtonClearInputSb = new JButton();

  JButton jButtonRemoveOutputSb = new JButton();

  JButton jButtonAddOutputSb = new JButton();

  JButton jButtonClearOutputSb = new JButton();

  JCheckBox jCheckBoxExecutableRemote = new JCheckBox();

  JCheckBox jCheckBoxStdInputRemote = new JCheckBox();

  JButton jButtonFileStdInput = new JButton();

  JCheckBox jCheckBoxStdOutput = new JCheckBox();

  JCheckBox jCheckBoxStdError = new JCheckBox();

  JButton jButtonFileInputSandbox = new JButton();

  JScrollPane jScrollPaneInputSandbox = new JScrollPane();

  JList jListInputSb = new JList();

  Vector inputSbVector = new Vector();

  Vector outputSbVector = new Vector();

  JScrollPane jScrollPaneOutputSandbox = new JScrollPane();

  JList jListOutputSb = new JList();

  JDLEditorInterface jint;

  Component component;

  boolean isAppletCalling = false;

  JPanel jPanelVO = new JPanel();

  JLabel jLabelVO = new JLabel();

  JTextField jTextFieldVO = new JTextField();

  public JobDef1Panel(Component component) {
    this.component = component;
    if (component instanceof JDLEditor) {
      jint = (JDLEditor) component;
      /*
       } else if (component instanceof JDLEJInternalFrame) {
       jint = (JDLEJInternalFrame) component;
       } else if (component instanceof JDLEJApplet) {
       jint = (JDLEJApplet) component;
       isAppletCalling = true;
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

  /**Component initialization*/
  private void jbInit() throws Exception {
    isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG) ? true
        : false;
    URL url = JobDef1Panel.class.getResource(Utils.ICON_FILE_OPEN);
    if (url != null) {
      jButtonFileExecutable.setIcon(new ImageIcon(url));
    }
    jButtonFileExecutable.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jButtonFileExecutableFocusLost(e);
      }
    });
    jButtonFileExecutable.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonFileExecutableEvent(e);
      }
    });
    //setIconImage(Toolkit.getDefaultToolkit().createImage(Frame1.class.getResource("[Your Icon]")));
    //contentPane = (JPanel) this.getContentPane();
    jLabelExecutable.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelExecutable.setText("Executable");
    jLabelArguments.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelArguments.setText("Arguments");
    jLabelStdInput.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelStdInput.setText("StdInput");
    jLabelStdOutput.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelStdOutput.setText("StdOutput");
    jLabelStdError.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelStdError.setText("StdError");
    jLabelExecutable.setToolTipText("Program Name");
    jTextFieldExecutable.setToolTipText("Program Name");
    jTextFieldExecutable.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jTextFieldExecutableFocusLost(e);
      }

      public void focusGained(FocusEvent e) {
        jTextFieldExecutableFocusGained(e);
      }
    });
    jTextFieldStdInput.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jTextFieldStdInputFocusLost(e);
      }
    });
    jTextFieldArguments.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jTextFieldArgumentsFocusLost(e);
      }
    });
    jTextFieldStdOutput.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jTextFieldStdOutputFocusLost(e);
      }
    });
    jTextFieldStdOutput.addKeyListener(new java.awt.event.KeyAdapter() {
      public void keyTyped(KeyEvent e) {
        jTextFieldStdOutputKeyTyped(e);
      }

      public void keyPressed(KeyEvent e) {
        jTextFieldStdOutputKeyPressed(e);
      }
    });
    jTextFieldStdError.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jTextFieldStdErrorFocusLost(e);
      }
    });
    jTextFieldStdError.addKeyListener(new java.awt.event.KeyAdapter() {
      public void keyTyped(KeyEvent e) {
        jTextFieldStdErrorKeyTyped(e);
      }

      public void keyPressed(KeyEvent e) {
        jTextFieldStdErrorKeyPressed(e);
      }
    });
    jTextFieldInputSb.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jTextFieldInputSbFocusLost(e);
      }
    });
    jTextFieldOutputSb.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jTextFieldOutputSbFocusLost(e);
      }
    });
    jButtonRemoveInputSb.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonRemoveInputSbEvent(e);
      }
    });
    jButtonRemoveInputSb.setText("Remove");
    jButtonAddInputSb.setText("Add");
    jButtonAddInputSb.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonAddInputSbEvent(e);
      }
    });
    jButtonClearInputSb.setText("Clear");
    jButtonClearInputSb.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        if (inputSbVector.size() != 0) {
          int choice = JOptionPane.showOptionDialog(JobDef1Panel.this,
              "Clear Input Sandbox list?", "Confirm Clear",
              JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null,
              null, null);
          if (choice == 0) {
            jButtonClearInputSbEvent(e);
          }
        }
        jTextFieldInputSb.selectAll();
        jTextFieldInputSb.grabFocus();
      }
    });
    jButtonRemoveOutputSb.setText("Remove");
    jButtonRemoveOutputSb
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jButtonRemoveOutputSbEvent(e);
          }
        });
    jButtonAddOutputSb.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonAddOutputSbEvent(e);
      }
    });
    jButtonAddOutputSb.setText("Add");
    jButtonClearOutputSb.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        if (outputSbVector.size() != 0) {
          int choice = JOptionPane.showOptionDialog(JobDef1Panel.this,
              "Clear Output Sandbox list?", "Confirm Clear",
              JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null,
              null, null);
          if (choice == 0) {
            jButtonClearOutputSbEvent(e);
          }
        }
        jTextFieldOutputSb.selectAll();
        jTextFieldOutputSb.grabFocus();
      }
    });
    jButtonClearOutputSb.setText("Clear");
    jCheckBoxExecutableRemote.setText("Remote");
    jCheckBoxExecutableRemote
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            jCheckBoxExecutableRemoteFocusLost(e);
          }
        });
    jCheckBoxExecutableRemote
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jCheckBoxExecutableRemoteEvent(e);
          }
        });
    jCheckBoxStdInputRemote.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jCheckBoxStdInputRemoteFocusLost(e);
      }
    });
    jCheckBoxStdInputRemote
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jCheckBoxStdInputRemoteEvent(e);
          }
        });
    jCheckBoxStdInputRemote.setText("Remote");
    jButtonFileStdInput.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonFileStdInputEvent(e);
      }
    });
    //jButtonFileStdInput.setText("File");
    url = JobDef1Panel.class.getResource(Utils.ICON_FILE_OPEN);
    if (url != null) {
      jButtonFileStdInput.setIcon(new ImageIcon(url));
    }
    //jButtonFileStdInput.setIcon(new ImageIcon("file_open.gif"));
    jButtonFileStdInput.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jButtonFileStdInputFocusLost(e);
      }
    });
    jCheckBoxStdOutput.setText("Add to Output Sandbox");
    jCheckBoxStdOutput.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jCheckBoxStdOutputFocusLost(e);
      }
    });
    jCheckBoxStdOutput.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jCheckBoxStdOutputEvent(e);
      }
    });
    jCheckBoxStdError.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jCheckBoxStdErrorFocusLost(e);
      }
    });
    jCheckBoxStdError.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jCheckBoxStdErrorEvent(e);
      }
    });
    jCheckBoxStdError.setText("Add to Output Sandbox");
    jButtonFileInputSandbox.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonFileInputSbEvent(e);
      }
    });
    url = JobDef1Panel.class.getResource(Utils.ICON_FILE_OPEN);
    if (url != null) {
      jButtonFileInputSandbox.setIcon(new ImageIcon(url));
    }
    jListInputSb.setBackground(Color.white);
    jListInputSb.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jListInputSbFocusLost(e);
      }
    });
    jListOutputSb.setBackground(Color.white);
    jListOutputSb.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jListOutputSbFocusLost(e);
      }
    });
    jTextFieldVO.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jTextFieldVOFocusLost(e);
      }
    });
    jLabelVO.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelVO.setText("Virtual Organisation");
    GridBagLayout gbl = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.insets = new Insets(3, 3, 3, 3);
    // jPanelVO
    jPanelVO.setLayout(gbl);
    jPanelVO.setBorder(new EtchedBorder());
    jPanelVO.add(jLabelVO, setGridBagConstraints(gbc, 0, 0, 1, 1, 0.0, 0.0,
        GridBagConstraints.CENTER, GridBagConstraints.NONE, null, 0, 0));
    jPanelVO.add(jTextFieldVO, setGridBagConstraints(gbc, 1, 0, 1, 1, 1.0, 0.0,
        GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, null, 0, 0));
    // jPanelExecutable
    jPanelExecutable.setLayout(gbl);
    setDefaultGridBagConstraints(gbc);
    jPanelExecutable.setBorder(new EtchedBorder());
    jPanelExecutable.add(jLabelExecutable, setGridBagConstraints(gbc, 0, 0, 1,
        1, 0.0, 0.0, GridBagConstraints.EAST, GridBagConstraints.NONE, null, 0,
        0));
    jPanelExecutable.add(jTextFieldExecutable, setGridBagConstraints(gbc, 1, 0,
        1, 1, 1.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jButtonFileExecutable.setPreferredSize(new Dimension(20, 20));
    jPanelExecutable.add(jButtonFileExecutable, setGridBagConstraints(gbc, 2,
        0, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER, GridBagConstraints.NONE,
        null, 0, 0));
    jPanelExecutable.add(jCheckBoxExecutableRemote, setGridBagConstraints(gbc,
        3, 0, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.NONE, null, 0, 0));
    jPanelExecutable.add(jLabelArguments, setGridBagConstraints(gbc, 0, 1, 1,
        1, 0.0, 0.0, GridBagConstraints.EAST, GridBagConstraints.NONE, null, 0,
        0));
    jPanelExecutable.add(jTextFieldArguments, setGridBagConstraints(gbc, 1, 1,
        1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    // jPanelStandardStreams
    jPanelStandardStreams.setLayout(gbl);
    setDefaultGridBagConstraints(gbc);
    jPanelStandardStreams.setBorder(new TitledBorder(new EtchedBorder(),
        " Standard Streams ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelStandardStreams.add(jLabelStdInput, setGridBagConstraints(gbc, 0, 0,
        1, 1, 0.0, 0.0, GridBagConstraints.EAST, GridBagConstraints.NONE, null,
        0, 0));
    jPanelStandardStreams.add(jTextFieldStdInput, setGridBagConstraints(gbc, 1,
        0, 1, 1, 1.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jButtonFileStdInput.setPreferredSize(new Dimension(20, 20));
    jPanelStandardStreams.add(jButtonFileStdInput, setGridBagConstraints(gbc,
        2, 0, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.NONE, null, 0, 0));
    jPanelStandardStreams.add(jCheckBoxStdInputRemote, setGridBagConstraints(
        gbc, 3, 0, 1, 1, 0.0, 0.0, GridBagConstraints.WEST,
        GridBagConstraints.NONE, null, 0, 0));
    jPanelStandardStreams.add(jLabelStdOutput, setGridBagConstraints(gbc, 0, 1,
        1, 1, 0.0, 0.0, GridBagConstraints.EAST, GridBagConstraints.NONE, null,
        0, 0));
    jPanelStandardStreams.add(jTextFieldStdOutput, setGridBagConstraints(gbc,
        1, 1, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelStandardStreams.add(jCheckBoxStdOutput, setGridBagConstraints(gbc, 2,
        1, 2, 1, 0.0, 0.0, GridBagConstraints.CENTER, GridBagConstraints.NONE,
        null, 0, 0));
    jPanelStandardStreams.add(jLabelStdError, setGridBagConstraints(gbc, 0, 2,
        1, 1, 0.0, 0.0, GridBagConstraints.EAST, GridBagConstraints.NONE, null,
        0, 0));
    jPanelStandardStreams.add(jTextFieldStdError, setGridBagConstraints(gbc, 1,
        2, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelStandardStreams.add(jCheckBoxStdError, setGridBagConstraints(gbc, 2,
        2, 2, 1, 0.0, 0.0, GridBagConstraints.CENTER, GridBagConstraints.NONE,
        null, 0, 0));
    // jPanelJobDInputSandbox
    jPanelJobDInputSandbox.setLayout(gbl);
    jPanelJobDInputSandbox
        .setBorder(new TitledBorder(new EtchedBorder(), " Input Sandbox ", 0,
            0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    JPanel jPanelInputSb = new JPanel();
    jPanelInputSb.setLayout(gbl);
    setDefaultGridBagConstraints(gbc);
    jPanelInputSb.add(jTextFieldInputSb, setGridBagConstraints(gbc, 0, 0, 1, 1,
        1.0, 0.0, GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
        null, 0, 0));
    jButtonFileInputSandbox.setPreferredSize(new Dimension(20, 20));
    jPanelInputSb.add(jButtonFileInputSandbox, setGridBagConstraints(gbc, 1, 0,
        1, 1, 0.0, 0.0, GridBagConstraints.CENTER, GridBagConstraints.NONE,
        null, 0, 0));
    jPanelJobDInputSandbox.add(jPanelInputSb, setGridBagConstraints(gbc, 0, 0,
        1, 1, 1.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelJobDInputSandbox.add(jButtonAddInputSb, setGridBagConstraints(gbc, 1,
        0, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelJobDInputSandbox.add(jScrollPaneInputSandbox, setGridBagConstraints(
        gbc, 0, 1, 1, 2, 0.0, 1.0, GridBagConstraints.CENTER,
        GridBagConstraints.BOTH, null, 0, 0));
    jPanelJobDInputSandbox.add(jButtonRemoveInputSb, setGridBagConstraints(gbc,
        1, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelJobDInputSandbox.add(jButtonClearInputSb, setGridBagConstraints(gbc,
        1, 2, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jScrollPaneInputSandbox.getViewport().add(jListInputSb, null);
    // jPanelJobDOutputSandbox
    jPanelJobDOutputSandbox.setLayout(gbl);
    jPanelJobDOutputSandbox
        .setBorder(new TitledBorder(new EtchedBorder(), " Output Sandbox ", 0,
            0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelJobDOutputSandbox.add(jTextFieldOutputSb, setGridBagConstraints(gbc,
        0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelJobDOutputSandbox.add(jScrollPaneOutputSandbox,
        setGridBagConstraints(gbc, 0, 1, 1, 2, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH, null, 0, 0));
    jPanelJobDOutputSandbox.add(jButtonAddOutputSb, setGridBagConstraints(gbc,
        1, 0, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelJobDOutputSandbox.add(jButtonRemoveOutputSb, setGridBagConstraints(
        gbc, 1, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jPanelJobDOutputSandbox.add(jButtonClearOutputSb, setGridBagConstraints(
        gbc, 1, 2, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
        GridBagConstraints.HORIZONTAL, null, 0, 0));
    jScrollPaneOutputSandbox.getViewport().add(jListOutputSb, null);
    // this
    this.setLayout(gbl);
    setDefaultGridBagConstraints(gbc);
    this.add(jPanelVO, setGridBagConstraints(gbc, 0, 0, 1, 1, 1.0, 0.0,
        GridBagConstraints.FIRST_LINE_START, GridBagConstraints.BOTH,
        new Insets(1, 1, 1, 1), 0, 0));
    this.add(jPanelExecutable, setGridBagConstraints(gbc, 0, 1, 1, 1, 0.0, 0.0,
        GridBagConstraints.FIRST_LINE_START, GridBagConstraints.BOTH, null, 0,
        0));
    this.add(jPanelStandardStreams, setGridBagConstraints(gbc, 0, 2, 1, 1, 0.0,
        0.0, GridBagConstraints.FIRST_LINE_START, GridBagConstraints.BOTH,
        null, 0, 0));
    this.add(jPanelJobDInputSandbox, setGridBagConstraints(gbc, 0, 3, 1, 1,
        0.0, 0.5, GridBagConstraints.FIRST_LINE_START, GridBagConstraints.BOTH,
        null, 0, 0));
    this.add(jPanelJobDOutputSandbox, setGridBagConstraints(gbc, 0, 4, 1, 1,
        0.0, 0.5, GridBagConstraints.FIRST_LINE_START, GridBagConstraints.BOTH,
        null, 0, 0));
    if (isAppletCalling) {
      setAppletObject(false);
    }
  }

  void jButtonFileInputSbEvent(ActionEvent e) {
    JFileChooser fileChooser = new JFileChooser();
    //fileChooser.setCurrentDirectory(new File(jint.getUserWorkingDirectory()));
    fileChooser.setDialogTitle(Jdl.INPUTSB + " File Selection");
    fileChooser.setApproveButtonText("Select");
    fileChooser.setCurrentDirectory(new File(GUIGlobalVars
        .getFileChooserWorkingDirectory()));
    int choice = fileChooser.showOpenDialog(JobDef1Panel.this);
    if (choice != JFileChooser.APPROVE_OPTION) {
      return;
    } else if (!fileChooser.getSelectedFile().isFile()) {
      String inputSbText = fileChooser.getSelectedFile().toString().trim();
      JOptionPane.showOptionDialog(JobDef1Panel.this, "Unable to find file: "
          + inputSbText, Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return;
    } else {
      //jint.setUserWorkingDirectory(fileChooser.getCurrentDirectory().toString());
      GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
          .getCurrentDirectory().toString());
      String inputSbText = fileChooser.getSelectedFile().toString().trim();
      jTextFieldInputSb.setText(inputSbText);
    }
  }

  void jCheckBoxStdOutputEvent(ActionEvent e) {
    String stdOutputText = jTextFieldStdOutput.getText().trim();
    if (stdOutputText.equals("")) {
      jCheckBoxStdOutput.setSelected(false);
    } else {
      if (jCheckBoxStdOutput.isSelected()) {
        if (!outputSbVector.contains(stdOutputText)) {
          try {
            JDLEditorInterface.jobAdGlobal.addAttribute(Jdl.OUTPUTSB,
                stdOutputText);
            outputSbVector.add(0, stdOutputText); // Adding at first position.
          } catch (IllegalArgumentException iae) {
            JOptionPane.showOptionDialog(component, "- " + iae.getMessage()
                + "\n", Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE, null, null, null);
            jCheckBoxStdOutput.setSelected(false);
            return;
          } catch (InvalidAttributeValueException iave) {
            JOptionPane.showOptionDialog(component, "- " + iave.getMessage()
                + "\n", Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE, null, null, null);
            jCheckBoxStdOutput.setSelected(false);
            return;
          } catch (Exception ex) {
            if (isDebugging) {
              ex.printStackTrace();
            }
            JOptionPane.showOptionDialog(component, ex.getMessage(),
                Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE, null, null, null);
            jCheckBoxStdOutput.setSelected(false);
          }
        } else {
          jCheckBoxStdOutput.setSelected(false);
        }
      } else {
        outputSbVector.removeElement(stdOutputText);
      }
      jListOutputSb.setListData(outputSbVector);
    }
    jTextFieldStdOutput.grabFocus();
  }

  void jButtonAddInputSbEvent(ActionEvent e) {
    String inputSbText = jTextFieldInputSb.getText().trim();
    String executableText = jTextFieldExecutable.getText().trim();
    try {
      inputSbText = Utils.solveEnviron(inputSbText);
    } catch (Exception ex) {
      JOptionPane.showOptionDialog(component, ex.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      jTextFieldInputSb.selectAll();
      jTextFieldInputSb.grabFocus();
      return;
    }
    if (!inputSbText.equals("") && (!inputSbVector.contains(inputSbText))) {
      try {
        JDLEditorInterface.jobAdGlobal.addAttribute(Jdl.INPUTSB, inputSbText);
        inputSbVector.add(inputSbText);
        jListInputSb.setListData(inputSbVector);
      } catch (IllegalArgumentException iae) {
        JOptionPane.showOptionDialog(component, "- " + iae.getMessage() + "\n",
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      } catch (InvalidAttributeValueException iave) {
        JOptionPane.showOptionDialog(component, "- InputData: wrong format\n",
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        JOptionPane.showOptionDialog(component, ex.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
    }
    jTextFieldInputSb.selectAll();
    jTextFieldInputSb.grabFocus();
  }

  void jCheckBoxStdErrorEvent(ActionEvent e) {
    String stdErrorText = jTextFieldStdError.getText().trim();
    if (stdErrorText.equals("")) {
      jCheckBoxStdError.setSelected(false);
    } else {
      if (jCheckBoxStdError.isSelected()) {
        if (!outputSbVector.contains(stdErrorText)) {
          try {
            JDLEditorInterface.jobAdGlobal.addAttribute(Jdl.OUTPUTSB,
                stdErrorText);
            int index = 0;
            if (jCheckBoxStdOutput.isSelected()) {
              index = 1;
            }
            outputSbVector.add(index, stdErrorText);
          } catch (IllegalArgumentException iae) {
            JOptionPane.showOptionDialog(component, "- " + iae.getMessage()
                + "\n", Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE, null, null, null);
            jCheckBoxStdError.setSelected(false);
            return;
          } catch (InvalidAttributeValueException iave) {
            JOptionPane.showOptionDialog(component, "- " + iave.getMessage()
                + "\n", Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE, null, null, null);
            jCheckBoxStdError.setSelected(false);
            return;
          } catch (Exception ex) {
            if (isDebugging) {
              ex.printStackTrace();
            }
            JOptionPane.showOptionDialog(component, ex.getMessage(),
                Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE, null, null, null);
            jCheckBoxStdError.setSelected(false);
          }
        } else {
          jCheckBoxStdError.setSelected(false);
        }
      } else {
        outputSbVector.removeElement(stdErrorText);
      }
      jListOutputSb.setListData(outputSbVector);
    }
    jTextFieldStdError.grabFocus();
  }

  void jButtonAddOutputSbEvent(ActionEvent e) {
    String outputSbText = jTextFieldOutputSb.getText().trim();
    if (!outputSbText.equals("")) {
      try {
        JDLEditorInterface.jobAdGlobal.addAttribute(Jdl.OUTPUTSB, outputSbText);
        if (!outputSbVector.contains(outputSbText)) {
          outputSbVector.add(outputSbText);
          jListOutputSb.setListData(outputSbVector);
        }
      } catch (IllegalArgumentException iae) {
        JOptionPane.showOptionDialog(component, "- " + iae.getMessage() + "\n",
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        jTextFieldOutputSb.selectAll();
        return;
      } catch (InvalidAttributeValueException iave) {
        JOptionPane.showOptionDialog(component,
            "- " + iave.getMessage() + "\n", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        jTextFieldOutputSb.selectAll();
        return;
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        JOptionPane.showOptionDialog(component, ex.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
    }
    jTextFieldOutputSb.selectAll();
    jTextFieldOutputSb.grabFocus();
  }

  void jButtonRemoveInputSbEvent(ActionEvent e) {
    int[] selectedItems = jListInputSb.getSelectedIndices();
    int selectedItemsCount = selectedItems.length;
    if (selectedItemsCount != 0) {
      for (int i = selectedItemsCount - 1; i >= 0; i--) {
        inputSbVector.removeElementAt(selectedItems[i]);
      }
      jListInputSb.setListData(inputSbVector);
      /* Old selection method.
       if(jListInputSb.getModel().getSize() != 0) {
       int selectableItem = selectedItems[selectedItemsCount - 1] + 1 - selectedItemsCount;
       if(selectableItem > jListInputSb.getModel().getSize() - 1)
       selectableItem = 0;
       jListInputSb.setSelectedIndex(selectableItem);
       }
       */
      if (jListInputSb.getModel().getSize() != 0) {
        int selectableItem = selectedItems[selectedItemsCount - 1] + 1
            - selectedItemsCount; // Next.
        if (selectableItem > jListInputSb.getModel().getSize() - 1) {
          selectableItem--; // Prev. (selectedItems[selectedItemsCount - 1] - selectedItemsCount).
        }
        jListInputSb.setSelectedIndex(selectableItem);
      }
    } else {
      JOptionPane.showOptionDialog(JobDef1Panel.this, Utils.SELECT_AN_ITEM,
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
    }
    jTextFieldInputSb.selectAll();
    jTextFieldInputSb.grabFocus();
  }

  void jButtonRemoveOutputSbEvent(ActionEvent e) {
    int[] selectedItems = jListOutputSb.getSelectedIndices();
    int selectedItemsCount = selectedItems.length;
    if (selectedItemsCount != 0) {
      String currentElement = "";
      for (int i = selectedItemsCount - 1; i >= 0; i--) {
        currentElement = outputSbVector.get(selectedItems[i]).toString();
        if (currentElement.equals(jTextFieldStdOutput.getText().trim())) {
          jCheckBoxStdOutput.setSelected(false);
        } else if (currentElement.equals(jTextFieldStdError.getText().trim())) {
          jCheckBoxStdError.setSelected(false);
        }
        outputSbVector.removeElementAt(selectedItems[i]);
      }
      jListOutputSb.setListData(outputSbVector);
      if (jListOutputSb.getModel().getSize() != 0) {
        int selectableItem = selectedItems[selectedItemsCount - 1] + 1
            - selectedItemsCount; // Next.
        if (selectableItem > jListOutputSb.getModel().getSize() - 1) {
          selectableItem--; // Prev. (selectedItems[selectedItemsCount - 1] - selectedItemsCount).
        }
        jListOutputSb.setSelectedIndex(selectableItem);
      }
    } else {
      JOptionPane.showOptionDialog(JobDef1Panel.this, Utils.SELECT_AN_ITEM,
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
    }
    jTextFieldOutputSb.selectAll();
    jTextFieldOutputSb.grabFocus();
  }

  private void jTextFieldStdOutputKeyTyped(KeyEvent e) {
    /*
     if(jCheckBoxStdOutput.isSelected()) {
     String stdOutputText = jTextFieldStdOutput.getText().trim();
     if(!stdOutputText.equals(""))
     outputSbVector.removeElement(stdOutputText);
     jListOutputSb.setListData(outputSbVector);
     jCheckBoxStdOutput.setSelected(false);
     }
     */
  }

  private void jTextFieldStdErrorKeyTyped(KeyEvent e) {
    /*
     if(jCheckBoxStdError.isSelected()) {
     String stdErrorText = jTextFieldStdError.getText().trim();
     if(!stdErrorText.equals(""))
     outputSbVector.removeElement(stdErrorText);
     jListOutputSb.setListData(outputSbVector);
     jCheckBoxStdError.setSelected(false);
     }
     */
  }

  void jButtonClearInputSbEvent(ActionEvent e) {
    inputSbVector.removeAllElements();
    jListInputSb.setListData(inputSbVector);
  }

  void jButtonClearOutputSbEvent(ActionEvent e) {
    outputSbVector.removeAllElements();
    jListOutputSb.setListData(outputSbVector);
    jCheckBoxStdOutput.setSelected(false);
    jCheckBoxStdError.setSelected(false);
  }

  void jButtonJobDef1ResetEvent(ActionEvent e) {
    //if(!(jint instanceof JDLEJApplet)) {
    if (jTextFieldVO.isEditable()) {
      jTextFieldVO.setText("");
    }
    jTextFieldExecutable.setText("");
    jButtonFileExecutable.setEnabled(true);
    jCheckBoxExecutableRemote.setSelected(false);
    jTextFieldStdInput.setText("");
    jButtonFileStdInput.setEnabled(true);
    jCheckBoxStdInputRemote.setSelected(false);
    inputSbVector.removeAllElements();
    jListInputSb.setListData(inputSbVector);
    //}
    jTextFieldExecutable.setVisible(true);
    jTextFieldArguments.setText("");
    jTextFieldStdInput.setVisible(true);
    jTextFieldStdOutput.setText("");
    jCheckBoxStdOutput.setSelected(false);
    jTextFieldStdError.setText("");
    jCheckBoxStdError.setSelected(false);
    jTextFieldInputSb.setText("");
    jTextFieldOutputSb.setText("");
    outputSbVector.removeAllElements();
    jListOutputSb.setListData(outputSbVector);
    try {
      JDLEditorInterface.jobAdGlobal.delAttribute(Jdl.INPUTSB);
    } catch (Exception ex) {
      // Do nothing
    }
    try {
      JDLEditorInterface.jobAdGlobal.delAttribute(Jdl.OUTPUTSB);
    } catch (Exception ex) {
      // Do nothing
    }
    jint.setJTextAreaJDL("");
    jTextFieldExecutable.grabFocus();
    if (jTextFieldVO.isEditable()) {
      jTextFieldVO.grabFocus();
    }
  }

  void jCheckBoxExecutableRemoteEvent(ActionEvent e) {
    if (jCheckBoxExecutableRemote.isSelected()) {
      jButtonFileExecutable.setEnabled(false);
    } else {
      jButtonFileExecutable.setEnabled(true);
    }
    jTextFieldExecutable.grabFocus();
  }

  void setExecutableRemoteEnabled(boolean bool) {
    jCheckBoxExecutableRemote.setSelected(bool);
    jButtonFileExecutable.setEnabled(!bool);
    jTextFieldExecutable.grabFocus();
  }

  void setStdInputRemoteEnabled(boolean bool) {
    jCheckBoxStdInputRemote.setSelected(bool);
    jButtonFileStdInput.setEnabled(!bool);
    jTextFieldStdInput.grabFocus();
  }

  void jCheckBoxStdInputRemoteEvent(ActionEvent e) {
    String stdInputText = jTextFieldStdInput.getText().trim();
    if (!stdInputText.equals("")) {
      if (jCheckBoxStdInputRemote.isSelected()) {
        File file = new File(stdInputText);
        ///Win32FileSystem ufs = new Win32FileSystem();
        ///if(!file.isAbsolute()) {
        if (!file.isAbsolute() && !GUIFileSystem.isAbsolute(stdInputText)) {
          JOptionPane
              .showOptionDialog(
                  component,
                  Jdl.STDINPUT
                      + ": you have selected \"Remote\", file name must be specified as an absolute path",
                  Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                  JOptionPane.ERROR_MESSAGE, null, null, null);
          jTextFieldStdInput.selectAll();
          jCheckBoxStdInputRemote.setSelected(false);
        }
      }
    }
    if (!jCheckBoxStdInputRemote.isSelected()) {
      jButtonFileStdInput.setEnabled(true);
    } else {
      jButtonFileStdInput.setEnabled(false);
    }
    jTextFieldStdInput.grabFocus();
  }

  void jButtonFileExecutableEvent(ActionEvent e) {
    //jTextFieldExecutableKeyPressed(null);
    String executableText = jTextFieldExecutable.getText().trim();
    JFileChooser fileChooser = new JFileChooser();
    fileChooser.setDialogTitle(Jdl.EXECUTABLE + " File Selection");
    fileChooser.setApproveButtonText("Select");
    //fileChooser.setCurrentDirectory(new File(jint.getUserWorkingDirectory()));
    fileChooser.setCurrentDirectory(new File(GUIGlobalVars
        .getFileChooserWorkingDirectory()));
    int choice = fileChooser.showOpenDialog(JobDef1Panel.this);
    if (choice != JFileChooser.APPROVE_OPTION) {
      return;
    } else if (!fileChooser.getSelectedFile().isFile()) {
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      JOptionPane.showOptionDialog(JobDef1Panel.this, "Unable to find file: "
          + selectedFile, Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return;
    } else {
      //jint.setUserWorkingDirectory(fileChooser.getCurrentDirectory().toString());
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
          .getCurrentDirectory().toString());
      jTextFieldExecutable.setText(selectedFile);
    }
    jTextFieldExecutable.grabFocus();
  }

  void jButtonFileStdInputEvent(ActionEvent e) {
    //jTextFieldStdInputKeyPressed(null);
    JFileChooser fileChooser = new JFileChooser();
    fileChooser.setDialogTitle(Jdl.STDINPUT + " File Selection");
    fileChooser.setApproveButtonText("Select");
    //fileChooser.setCurrentDirectory(new File(jint.getUserWorkingDirectory()));
    fileChooser.setCurrentDirectory(new File(GUIGlobalVars
        .getFileChooserWorkingDirectory()));
    int choice = fileChooser.showOpenDialog(JobDef1Panel.this);
    if (choice != JFileChooser.APPROVE_OPTION) {
      return;
    } else if (!fileChooser.getSelectedFile().isFile()) {
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      JOptionPane.showOptionDialog(JobDef1Panel.this, "Unable to find file: "
          + selectedFile, Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return;
    } else {
      //jint.setUserWorkingDirectory(fileChooser.getCurrentDirectory().toString());
      GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
          .getCurrentDirectory().toString());
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      jTextFieldStdInput.setText(selectedFile);
    }
    jTextFieldStdInput.grabFocus();
  }

  void jButtonFileOutputSandboxEvent(ActionEvent e) {
    JFileChooser fileChooser = new JFileChooser();
    fileChooser.setDialogTitle(Jdl.OUTPUTSB + " File Selection");
    fileChooser.setApproveButtonText("Select");
    //fileChooser.setCurrentDirectory(new File(jint.getUserWorkingDirectory()));
    fileChooser.setCurrentDirectory(new File(GUIGlobalVars
        .getFileChooserWorkingDirectory()));
    int choice = fileChooser.showOpenDialog(JobDef1Panel.this);
    if (choice != JFileChooser.APPROVE_OPTION) {
      return;
    } else if (!fileChooser.getSelectedFile().isFile()) {
      String outputSbText = fileChooser.getSelectedFile().toString().trim();
      JOptionPane.showOptionDialog(JobDef1Panel.this, "Unable to find file: "
          + outputSbText, Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return;
    } else {
      //jint.setUserWorkingDirectory(fileChooser.getCurrentDirectory().toString());
      GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
          .getCurrentDirectory().toString());
      String outputSbText = fileChooser.getSelectedFile().toString().trim();
      File fileName = new File(outputSbText);
      outputSbText = fileName.getName().toString();
      jTextFieldOutputSb.setText(outputSbText);
    }
  }

  String getGUIWarningMsg() {
    return GUIWarningMsg;
  }

  String jButtonJobDef1ViewEvent(boolean showWarningMsg, boolean showErrorMsg,
      ActionEvent e) {
    String result = "";
    errorMsg = "";
    warningMsg = "";
    GUIWarningMsg = "";
    String workingDir = GUIFileSystem.getDefaultWorkingDirectory();
    String voText = jTextFieldVO.getText().trim();
    String stdInputText = jTextFieldStdInput.getText().trim();
    String executableText = jTextFieldExecutable.getText().trim();
    String stdOutputText = jTextFieldStdOutput.getText().trim();
    String stdErrorText = jTextFieldStdError.getText().trim();
    errorMsg += checkInsertedValues();
    if (!voText.equals("")) {
      result += Jdl.VIRTUAL_ORGANISATION + " = \"" + voText + "\";\n";
    }
    if (jCheckBoxExecutableRemote.isSelected()) {
      if (!executableText.equals("")) {
        result += Jdl.EXECUTABLE + " = \"" + executableText + "\";\n";
      }
    } else {
      if (!executableText.equals("")) {
        Vector fileNameVector = new Vector();
        File file = null;
        String fileNameText = "";
        for (int i = 0; i < inputSbVector.size(); i++) {
          file = new File(inputSbVector.get(i).toString());
          fileNameText = file.getName().toString();
          fileNameVector.add(fileNameText);
        }
        File fileName = null;
        if (executableText.equals((new File(executableText)).getName()
            .toString())) {
          fileName = new File(workingDir + File.separator + executableText);
        } else {
          fileName = new File(executableText);
        }
        result += Jdl.EXECUTABLE + " = \"" + fileName.getName().toString()
            + "\";\n";
      }
    }
    String argumentsText = jTextFieldArguments.getText().trim();
    if (!argumentsText.equals("")) {
      result += Jdl.ARGUMENTS + " = \"" + argumentsText + "\";\n";
    }
    if (jPanelStandardStreams.isEnabled()) {
      if (jCheckBoxStdInputRemote.isSelected()) {
        if (!stdInputText.equals("")) {
          result += Jdl.STDINPUT + " = \"" + stdInputText + "\";\n";
        }
      } else {
        if (!stdInputText.equals("")) {
          Vector fileNameVector = new Vector();
          File file = null;
          String fileNameText = "";
          for (int i = 0; i < inputSbVector.size(); i++) {
            file = new File(inputSbVector.get(i).toString());
            fileNameText = file.getName().toString();
            fileNameVector.add(fileNameText);
          }
          File fileName = null;
          if (stdInputText
              .equals((new File(stdInputText)).getName().toString())) {
            fileName = new File(workingDir + File.separator + stdInputText);
          } else {
            fileName = new File(stdInputText);
          }
          result += Jdl.STDINPUT + " = \"" + fileName.getName().toString()
              + "\";\n";
        }
      }
      if (!stdOutputText.equals("")) {
        File fileName = new File(stdOutputText);
        result += Jdl.STDOUTPUT + " = \"" + fileName.toString() + "\";\n";
      }
      if (!stdErrorText.equals("")) {
        File fileName = new File(stdErrorText);
        result += Jdl.STDERROR + " = \"" + fileName.toString() + "\";\n";
      }
    }
    Vector tempInputSbVector = (Vector) inputSbVector.clone();
    boolean isExecutablePresent = false;
    if (!executableText.equals("") && !jCheckBoxExecutableRemote.isSelected()) {
      if (tempInputSbVector.contains(executableText)) {
        tempInputSbVector.remove(executableText);
      }
      tempInputSbVector.add(0, executableText); // Adding at first position.
      isExecutablePresent = true;
    }
    if (jPanelStandardStreams.isEnabled()) {
      if (!stdInputText.equals("") && !jCheckBoxStdInputRemote.isSelected()) {
        if (!stdInputText.equals(executableText)
            && tempInputSbVector.contains(stdInputText)) {
          tempInputSbVector.remove(stdInputText);
        }
        if (isExecutablePresent) {
          tempInputSbVector.add(1, stdInputText); // Second position.
        } else {
          tempInputSbVector.add(0, stdInputText); // First position.
        }
      }
    }
    int jListInputSbCount = tempInputSbVector.size();
    if (jListInputSbCount != 0) {
      result += Jdl.INPUTSB + " = ";
      if (jListInputSbCount == 1) {
        result += "\"" + tempInputSbVector.get(0) + "\";\n";
      } else {
        result += "{";
        for (int i = 0; i < jListInputSbCount - 1; i++) {
          result += "\"" + tempInputSbVector.get(i) + "\", ";
        }
        result += "\"" + tempInputSbVector.get(jListInputSbCount - 1)
            + "\"};\n";
      }
    }
    int jListOutputSbCount = outputSbVector.size();
    boolean isWildcardPresent = false;
    if (jListOutputSbCount != 0) {
      result += Jdl.OUTPUTSB + " = ";
      if (jListOutputSbCount == 1) {
        String element = outputSbVector.get(0).toString();
        result += "\"" + element + "\";\n";
        if ((element.indexOf("?") != -1) || (element.indexOf("*") != -1)) {
          isWildcardPresent = true;
        }
      } else {
        result += "{";
        String element = "";
        for (int i = 0; i < jListOutputSbCount - 1; i++) {
          element = outputSbVector.get(i).toString();
          result += "\"" + element + "\", ";
          if ((element.indexOf("?") != -1) || (element.indexOf("*") != -1)) {
            isWildcardPresent = true;
          }
        }
        String lastElement = outputSbVector.get(jListOutputSbCount - 1)
            .toString();
        result += "\"" + lastElement + "\"};\n";
        if ((lastElement.indexOf("?") != -1)
            || (lastElement.indexOf("*") != -1)) {
          isWildcardPresent = true;
        }
      }
      if (isWildcardPresent) {
        GUIWarningMsg = "\n- "
            + Jdl.OUTPUTSB
            + ": one or more files contain wildcards, file overwriting is possible";
      }
    }
    //boolean localAccess = (jint instanceof JDLEJApplet) ? false : true;
    boolean localAccess = true;
    warningMsg = ExprChecker.checkResult(result,
        Utils.jobDefinition1AttributeArray, localAccess)
        + GUIWarningMsg;
    errorMsg = errorMsg.trim();
    warningMsg = warningMsg.trim();
    if (!errorMsg.equals("") && showErrorMsg) {
      GraphicUtils.showOptionDialogMsg(JobDef1Panel.this, errorMsg,
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE, null,
          null);
      return "";
    } else {
      if (!warningMsg.trim().equals("") && showWarningMsg) {
        GraphicUtils.showOptionDialogMsg(JobDef1Panel.this, warningMsg,
            Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
            null, null);
      }
      jint.setJTextAreaJDL(result);
    }
    return result;
  }

  String checkInsertedValues() {
    String errorMsg = "";
    String stdInputText = jTextFieldStdInput.getText().trim();
    String executableText = jTextFieldExecutable.getText().trim();
    String stdOutputText = jTextFieldStdOutput.getText().trim();
    String stdErrorText = jTextFieldStdError.getText().trim();
    String executableFileName = GUIFileSystem.getName(executableText);
    if (!executableText.equals("")) {
      if (jCheckBoxExecutableRemote.isSelected()) {
        if (!executableText.equals(executableFileName)
            && (new File(executableText)).isAbsolute()
            && !GUIFileSystem.isAbsolute(executableText)) {
          errorMsg += "- " + Jdl.EXECUTABLE
              + ": you have selected \"Remote\", "
              + "file name must be specified as a single name or as an "
              + "absolute path\n";
        }
        errorMsg += GUIFileSystem.checkFileRemote(Jdl.EXECUTABLE,
            executableText, this);
      } else {
        errorMsg += GUIFileSystem.checkFileLocal(Jdl.EXECUTABLE,
            executableText, this);
      }
    }
    if (!stdInputText.equals("")) {
      if (jCheckBoxStdInputRemote.isSelected()) {
        if (!(new File(stdInputText)).isAbsolute()
            && !GUIFileSystem.isAbsolute(stdInputText)) {
          errorMsg += "- " + Jdl.STDINPUT + ": you have selected \"Remote\", "
              + "file name must be specified as an absolute path\n";
        }
        errorMsg += GUIFileSystem.checkFileRemote(Jdl.STDINPUT, stdInputText,
            this);
      } else {
        errorMsg += GUIFileSystem.checkFileLocal(Jdl.STDINPUT, stdInputText,
            this);
      }
    }
    if (!stdOutputText.equals("")) {
      errorMsg += GUIFileSystem.checkFileRemote(Jdl.STDOUTPUT, stdOutputText,
          this);
    }
    if (!stdErrorText.equals("")) {
      errorMsg += GUIFileSystem.checkFileRemote(Jdl.STDOUTPUT, stdErrorText,
          this);
    }
    errorMsg += getMatchErrorMsg();
    return errorMsg;
  }

  void jTextFieldExecutableFocusLost(FocusEvent e) {
    if (jTextFieldExecutable.getText().trim().equals("")) {
      jTextFieldExecutable.setText("");
    }
    jTextFieldExecutable.select(0, 0); // Deselect.
    /*
     String executableText = jTextFieldExecutable.getText().trim();
     String stdInputText = jTextFieldStdInput.getText().trim();
     //String executableFileName = (new File(executableText)).getName().toString();
     String executableFileName = GUIFileSystem.getName(executableText);
     String stdInputFileName = GUIFileSystem.getName(stdInputText);
     int matchErrorType = getMatchErrorType();
     Component hasFocusComponent = e.getOppositeComponent(); // Get component that "has the focus".
     System.out.println("System: " + System.getProperty("os.name"));
     if(hasFocusComponent == null) { // component == null if you lost focus moving to other application.
     jTextFieldExecutable.selectAll();
     jTextFieldExecutable.grabFocus();
     return;
     }
     if( ((hasFocusComponent == jTextFieldStdInput) || (hasFocusComponent == jCheckBoxExecutableRemote) ||
     (hasFocusComponent == jButtonFileExecutable) || (hasFocusComponent == jCheckBoxStdInputRemote) ||
     (hasFocusComponent == jButtonFileStdInput))
     && (matchErrorType == MATCH_ERROR_EXECUTABLE_STDINPUT) ) {
     jTextFieldExecutable.select(0, 0);
     return;
     }
     if( (hasFocusComponent instanceof javax.swing.JRootPane) ||
     (hasFocusComponent.getParent().getParent() instanceof javax.swing.JOptionPane) ||
     (hasFocusComponent.toString().substring(0, 41).equals(
     "javax.swing.plaf.metal.MetalFileChooserUI")) ) {
     jTextFieldExecutable.select(0, 0);
     return;
     }
     if(matchErrorType == MATCH_ERROR_EXECUTABLE_STDINPUT) {
     JOptionPane.showOptionDialog(component,
     MATCH_ERROR_EXECUTABLE_STDINPUT_MSG,
     "Error Message",
     JOptionPane.DEFAULT_OPTION,
     JOptionPane.ERROR_MESSAGE,
     null, null, null);
     jTextFieldExecutable.selectAll();
     jTextFieldExecutable.grabFocus();
     return;
     }
     if(!executableText.equals("")) {
     if(jCheckBoxExecutableRemote.isSelected()) {
     ///if(!executableText.equals(executableFileName) && !(new File(executableText)).isAbsolute()) {
     if(!executableText.equals(executableFileName) &&
     !(new File(executableText)).isAbsolute() && !GUIFileSystem.isAbsolute(executableText)) {
     JOptionPane.showOptionDialog(JobDef1Panel.this,
     "Executable: you have selected \"Remote\", file name must be specified as a single name or as an absolute path",
     "Error Message",
     JOptionPane.DEFAULT_OPTION,
     JOptionPane.ERROR_MESSAGE,
     null, null, null);
     //jCheckBoxExecutableRemote.setSelected(false);
     jTextFieldExecutable.selectAll();
     jTextFieldExecutable.grabFocus();
     return;
     }
     if(!Utils.isCheckFileRemoteOk("Executable", executableText, this)) {
     jTextFieldExecutable.selectAll();
     jTextFieldExecutable.grabFocus();
     return;
     }
     } else {
     if(!Utils.isCheckFileLocalOk("Executable", executableText, this)) {
     jTextFieldExecutable.selectAll();
     jTextFieldExecutable.grabFocus();
     return;
     }
     }
     }
     */
  }

  void jTextFieldStdInputFocusLost(FocusEvent e) {
    if (jTextFieldStdInput.getText().trim().equals("")) {
      jTextFieldStdInput.setText("");
    }
    jTextFieldStdInput.select(0, 0); // Deselect.
    /*
     String executableText = jTextFieldExecutable.getText().trim();
     String stdInputText = jTextFieldStdInput.getText().trim();
     //String executableFileName = (new File(executableText)).getName().toString();
     String executableFileName = GUIFileSystem.getName(executableText);
     //String stdInputFileName = (new File(stdInputText)).getName().toString();
     String stdInputFileName = GUIFileSystem.getName(stdInputText);
     int matchErrorType = getMatchErrorType();
     Component hasFocusComponent = e.getOppositeComponent(); // Get component that "has the focus".
     if(hasFocusComponent == null) { // component == null if you lost focus moving to other application.
     jTextFieldStdInput.selectAll();
     jTextFieldStdInput.grabFocus();
     return;
     }
     if( ((hasFocusComponent == jTextFieldExecutable) || (hasFocusComponent == jCheckBoxExecutableRemote) ||
     (hasFocusComponent == jButtonFileExecutable) || (hasFocusComponent == jCheckBoxStdInputRemote) ||
     (hasFocusComponent == jButtonFileStdInput))
     && (matchErrorType == MATCH_ERROR_EXECUTABLE_STDINPUT)) {
     jTextFieldStdInput.select(0, 0);
     return;
     }
     if( (hasFocusComponent instanceof javax.swing.JRootPane) ||
     (hasFocusComponent.getParent().getParent() instanceof javax.swing.JOptionPane) ||
     (hasFocusComponent.toString().substring(0, 41).equals(
     "javax.swing.plaf.metal.MetalFileChooserUI")) ) {
     jTextFieldExecutable.select(0, 0);
     return;
     }
     if(matchErrorType == MATCH_ERROR_EXECUTABLE_STDINPUT) {
     errorMsg = "Executable, StdInput: inserted files must be different";
     JOptionPane.showOptionDialog(component,
     "Executable, StdInput: inserted files must be different",
     "Error Message",
     JOptionPane.DEFAULT_OPTION,
     JOptionPane.ERROR_MESSAGE,
     null, null, null);
     jTextFieldStdInput.selectAll();
     jTextFieldStdInput.grabFocus();
     return;
     }
     if(!stdInputText.equals("")) {
     if(jCheckBoxStdInputRemote.isSelected()) {
     ///if(!(new File(stdInputText)).isAbsolute()) {
     if(!(new File(stdInputText)).isAbsolute() && !GUIFileSystem.isAbsolute(stdInputText)) {
     JOptionPane.showOptionDialog(JobDef1Panel.this,
     "StdInput: you have selected \"Remote\", file name must be specified as an absolute path",
     "Error Message",
     JOptionPane.DEFAULT_OPTION,
     JOptionPane.ERROR_MESSAGE,
     null, null, null);
     jTextFieldStdInput.selectAll();
     jTextFieldStdInput.grabFocus();
     return;
     }
     if(!Utils.isCheckFileRemoteOk("StdInput", stdInputText, this)) {
     jTextFieldStdInput.selectAll();
     jTextFieldStdInput.grabFocus();
     return;
     }
     } else {
     if(!Utils.isCheckFileLocalOk("StdInput", stdInputText, this)) {
     jTextFieldStdInput.selectAll();
     jTextFieldStdInput.grabFocus();
     return;
     }
     }
     }
     */
  }

  void jTextFieldStdOutputFocusLost(FocusEvent e) {
    if (jTextFieldStdOutput.getText().trim().equals("")) {
      jTextFieldStdOutput.setText("");
    }
    jTextFieldStdOutput.select(0, 0);
    /*
     String stdOutputText = jTextFieldStdOutput.getText().trim();
     String stdErrorText = jTextFieldStdError.getText().trim();
     //String stdOutputFileName = ( (new File(stdOutputText)).getName().toString() );
     String stdOutputFileName = GUIFileSystem.getName(stdOutputText);
     String stdErrorFileName = GUIFileSystem.getName(stdErrorText);
     int matchErrorType = getMatchErrorType();
     Component hasFocusComponent = e.getOppositeComponent(); // Get component that "has the focus".
     if(hasFocusComponent == null) { // component == null if you lost focus moving to other application.
     jTextFieldStdOutput.selectAll();
     jTextFieldStdOutput.grabFocus();
     return;
     }
     if( ((hasFocusComponent == jTextFieldStdError) || (hasFocusComponent == jCheckBoxStdError)
     || (hasFocusComponent == jCheckBoxStdOutput))
     && (matchErrorType == MATCH_ERROR_STDOUTPUT_STDERROR) ) {
     jTextFieldStdOutput.select(0, 0);
     return;
     }
     if( (hasFocusComponent instanceof javax.swing.JRootPane) ||
     (hasFocusComponent.getParent().getParent() instanceof javax.swing.JOptionPane) ||
     (hasFocusComponent.toString().substring(0, 41).equals(
     "javax.swing.plaf.metal.MetalFileChooserUI")) ) {
     jTextFieldExecutable.select(0, 0);
     return;
     }
     if(matchErrorType == MATCH_ERROR_STDOUTPUT_STDERROR) {
     JOptionPane.showOptionDialog(JobDef1Panel.this,
     "StdOutput, StdError: inserted files can be equal but not in different paths",
     "Error Message",
     JOptionPane.DEFAULT_OPTION,
     JOptionPane.ERROR_MESSAGE,
     null, null, null);
     jTextFieldStdOutput.selectAll();
     jTextFieldStdOutput.grabFocus();
     return;
     }
     if(!stdOutputText.equals("")) {
     if(!Utils.isCheckFileLocalOk("StdOutput", stdOutputText, this)) {
     jTextFieldStdOutput.selectAll();
     jTextFieldStdOutput.grabFocus();
     return;
     }
     }
     */
  }

  /*
   String getJobTypeValue() {
   return jComboBoxJobType.getSelectedItem().toString();
   }
   */
  void jTextFieldStdErrorFocusLost(FocusEvent e) {
    GraphicUtils.jTextFieldDeselect(jTextFieldStdError);
  }

  void jListInputSbFocusLost(FocusEvent e) {
    if (e.getOppositeComponent() != jButtonRemoveInputSb) { // Get component that "has the focus".
      jListInputSb.clearSelection();
    }
  }

  void jListOutputSbFocusLost(FocusEvent e) {
    if (e.getOppositeComponent() != jButtonRemoveOutputSb) { // Get component that "has the focus".
      jListOutputSb.clearSelection();
    }
  }

  String getWarningMsg() {
    return warningMsg;
  }

  String getErrorMsg() {
    return errorMsg;
  }

  String getMatchErrorMsg() {
    String matchErrorMsg = "";
    String executableText = jTextFieldExecutable.getText().trim();
    String stdInputText = jTextFieldStdInput.getText().trim();
    if (!stdInputText.equals("") && !executableText.equals("")) {
      if (!jCheckBoxStdInputRemote.isSelected()
          && !jCheckBoxExecutableRemote.isSelected()) {
        //String executableFileName = ( (new File(executableText)).getName().toString() );
        String executableFileName = GUIFileSystem.getName(executableText);
        //String stdInputFileName = ( (new File(stdInputText)).getName().toString() );
        String stdInputFileName = GUIFileSystem.getName(stdInputText);
        if (stdInputFileName.equals(executableFileName)) {
          matchErrorMsg += MATCH_ERROR_EXECUTABLE_STDINPUT_MSG;
        }
      } else if (jCheckBoxStdInputRemote.isSelected()
          && jCheckBoxExecutableRemote.isSelected()) {
        if (stdInputText.equals(executableText)) {
          matchErrorMsg += MATCH_ERROR_EXECUTABLE_STDINPUT_MSG;
        }
      }
    }
    String stdOutputText = jTextFieldStdOutput.getText().trim();
    String stdErrorText = jTextFieldStdError.getText().trim();
    //String stdOutputFileName = ( (new File(stdOutputText)).getName().toString() );
    String stdOutputFileName = GUIFileSystem.getName(stdOutputText);
    //String stdErrorFileName = ( (new File(stdErrorText)).getName().toString() );
    String stdErrorFileName = GUIFileSystem.getName(stdErrorText);
    if (jCheckBoxStdOutput.isSelected() && jCheckBoxStdError.isSelected()) {
      if (!stdErrorText.equals("") && !stdOutputText.equals("")
          && !stdErrorText.equals(stdOutputText)) {
        if (stdErrorFileName.equals(stdOutputFileName)) {
          matchErrorMsg += "- " + Jdl.STDOUTPUT + ", " + Jdl.STDERROR
              + ": inserted files can be equal but not in different paths\n";
        }
      }
    }
    return matchErrorMsg;
  }

  int getMatchErrorType() {
    String executableText = jTextFieldExecutable.getText().trim();
    String stdInputText = jTextFieldStdInput.getText().trim();
    if (!stdInputText.equals("") && !executableText.equals("")) {
      if (!jCheckBoxStdInputRemote.isSelected()
          && !jCheckBoxExecutableRemote.isSelected()) {
        //String executableFileName = ( (new File(executableText)).getName().toString() );
        String executableFileName = GUIFileSystem.getName(executableText);
        //String stdInputFileName = ( (new File(stdInputText)).getName().toString() );
        String stdInputFileName = GUIFileSystem.getName(stdInputText);
        if (stdInputFileName.equals(executableFileName)) {
          return MATCH_ERROR_EXECUTABLE_STDINPUT;
        }
      } else if (jCheckBoxStdInputRemote.isSelected()
          && jCheckBoxExecutableRemote.isSelected()) {
        if (stdInputText.equals(executableText)) {
          return MATCH_ERROR_EXECUTABLE_STDINPUT;
        }
      }
    }
    String stdOutputText = jTextFieldStdOutput.getText().trim();
    String stdErrorText = jTextFieldStdError.getText().trim();
    //String stdOutputFileName = ( (new File(stdOutputText)).getName().toString() );
    String stdOutputFileName = GUIFileSystem.getName(stdOutputText);
    //String stdErrorFileName = ( (new File(stdErrorText)).getName().toString() );
    String stdErrorFileName = GUIFileSystem.getName(stdErrorText);
    if (jCheckBoxStdOutput.isSelected() && jCheckBoxStdError.isSelected()) {
      if (!stdErrorText.equals("") && !stdOutputText.equals("")
          && !stdErrorText.equals(stdOutputText)) {
        if (stdErrorFileName.equals(stdOutputFileName)) {
          return MATCH_ERROR_STDOUTPUT_STDERROR;
        }
      }
    }
    /*
     Vector checkVector = new Vector();
     File file = null;
     String fileName = "";
     for(int i = 0; i < inputSbVector.size(); i++) {
     file = new File(inputSbVector.get(i).toString());
     fileName = file.getName();
     checkVector.add(fileName);
     }
     //String executableText = jTextFieldExecutable.getText().trim();
     if(!executableText.equals("") && !jCheckBoxExecutableRemote.isSelected()) {
     file = new File(executableText);
     fileName = file.getName();
     if(checkVector.contains(fileName))
     return MATCH_ERROR_EXECUTABLE_INPUTSB; //checkVector.add(fileName);
     }
     //String stdInputText = jTextFieldStdInput.getText().trim();
     if(!stdInputText.equals("") && !jCheckBoxStdInputRemote.isSelected()) {
     file = new File(stdInputText);
     fileName = file.getName();
     if(checkVector.contains(fileName))
     return MATCH_ERROR_STDINPUT_INPUTSB;
     }
     String stdOutputText = jTextFieldStdOutput.getText().trim();
     String stdErrorText = jTextFieldStdError.getText().trim();
     if(!stdOutputText.equals("") && stdOutputText.equals(stdErrorText))
     return MATCH_ERROR_STDOUTPUT_STDERROR;
     */
    return NO_MATCH_ERROR;
  }

  void jButtonFileExecutableFocusLost(FocusEvent e) {
    /*
     if(getMatchErrorType() == MATCH_ERROR_EXECUTABLE_STDINPUT) {
     jTextFieldExecutable.selectAll();
     jTextFieldExecutable.grabFocus();
     }
     */
  }

  void jTextFieldExecutableFocusGained(FocusEvent e) {
    /*int matchErrorType = getMatchErrorType();
     Component hadFocusComponent = e.getOppositeComponent(); // Get component that "had the focus".
     if( ((hadFocusComponent == jButtonFileExecutable) || (hadFocusComponent == jCheckBoxExecutableRemote))
     && (matchErrorType == MATCH_ERROR_EXECUTABLE_STDINPUT) ) {
     JOptionPane.showOptionDialog(JobDef1Panel.this,
     MATCH_ERROR_EXECUTABLE_STDINPUT_MSG,
     "Error Message",
     JOptionPane.DEFAULT_OPTION,
     JOptionPane.ERROR_MESSAGE,
     null, null, null);
     }
     */
  }

  void jCheckBoxExecutableRemoteFocusLost(FocusEvent e) {
    /*
     if(getMatchErrorType() == MATCH_ERROR_EXECUTABLE_STDINPUT) {
     jTextFieldExecutable.selectAll();
     jTextFieldExecutable.grabFocus();
     }
     */
  }

  void jButtonFileStdInputFocusLost(FocusEvent e) {
    /*
     if(getMatchErrorType() == MATCH_ERROR_EXECUTABLE_STDINPUT) {
     jTextFieldStdInput.selectAll();
     jTextFieldStdInput.grabFocus();
     }
     */
  }

  void jCheckBoxStdInputRemoteFocusLost(FocusEvent e) {
    /*
     if(getMatchErrorType() == MATCH_ERROR_EXECUTABLE_STDINPUT) {
     jTextFieldStdInput.selectAll();
     jTextFieldStdInput.grabFocus();
     }
     */
  }

  void jCheckBoxStdOutputFocusLost(FocusEvent e) {
    /*
     if(getMatchErrorType() == MATCH_ERROR_STDOUTPUT_STDERROR) {
     jTextFieldStdOutput.selectAll();
     jTextFieldStdOutput.grabFocus();
     }
     */
  }

  void jCheckBoxStdErrorFocusLost(FocusEvent e) {
    /*
     if(getMatchErrorType() == MATCH_ERROR_STDOUTPUT_STDERROR) {
     jTextFieldStdError.selectAll();
     jTextFieldStdError.grabFocus();
     }
     */
  }

  void jTextFieldArgumentsFocusLost(FocusEvent e) {
    if (jTextFieldArguments.getText().trim().equals("")) {
      jTextFieldArguments.setText("");
    }
    jTextFieldArguments.select(0, 0);
  }

  void jTextFieldInputSbFocusLost(FocusEvent e) {
    if (jTextFieldInputSb.getText().trim().equals("")) {
      jTextFieldInputSb.setText("");
    }
    jTextFieldInputSb.select(0, 0);
  }

  void jTextFieldOutputSbFocusLost(FocusEvent e) {
    if (jTextFieldOutputSb.getText().trim().equals("")) {
      jTextFieldOutputSb.setText("");
    }
    jTextFieldOutputSb.select(0, 0);
  }

  void jTextFieldStdOutputKeyPressed(KeyEvent e) {
    if (jCheckBoxStdOutput.isSelected()) {
      String stdOutputText = jTextFieldStdOutput.getText().trim();
      if (!stdOutputText.equals("")) {
        outputSbVector.removeElement(stdOutputText);
      }
      jListOutputSb.setListData(outputSbVector);
      jCheckBoxStdOutput.setSelected(false);
    }
  }

  void jTextFieldStdErrorKeyPressed(KeyEvent e) {
    if (jCheckBoxStdError.isSelected()) {
      String stdErrorText = jTextFieldStdError.getText().trim();
      if (!stdErrorText.equals("")) {
        outputSbVector.removeElement(stdErrorText);
      }
      jListOutputSb.setListData(outputSbVector);
      jCheckBoxStdError.setSelected(false);
    }
  }

  void setAppletObject(boolean bool) {
    jTextFieldExecutable.setEditable(bool);
    jTextFieldExecutable.setBackground(Color.white);
    jButtonFileExecutable.setEnabled(bool);
    jCheckBoxExecutableRemote.setEnabled(bool);
    jTextFieldStdInput.setEditable(bool);
    jTextFieldStdInput.setBackground(Color.white);
    jButtonFileStdInput.setEnabled(bool);
    jCheckBoxStdInputRemote.setEnabled(bool);
    jTextFieldInputSb.setEditable(bool);
    jTextFieldInputSb.setBackground(Color.white);
    jButtonFileInputSandbox.setEnabled(bool);
    jButtonAddInputSb.setEnabled(bool);
    jButtonRemoveInputSb.setEnabled(bool);
    jButtonClearInputSb.setEnabled(bool);
  }

  void setJTextFieldVOEditable(boolean bool) {
    jTextFieldVO.setEditable(bool);
    if (bool) {
      //jTextFieldVO.setBackground();
    } else {
      jTextFieldVO.setBackground(Color.white);
    }
  }

  void setVOText(String text) {
    jTextFieldVO.setText(text);
  }

  String getVOText() {
    return jTextFieldVO.getText().trim();
  }

  void setArgumentsText(String text) {
    jTextFieldArguments.setText(text);
  }

  public void setExecutableText(String value) {
    // If I find file in InputSandbox then Executable file is local.
    // If I find more than one entries prompt user a warning message.
    ///File file = new File(value);
    ///String fileName = file.getName();
    String fileName = GUIFileSystem.getName(value);
    if (value.substring(0, 2).equals("." + File.separator)
        && value.substring(2).equals(fileName)) { // File of type "./name" or ".\name".
      value = GUIFileSystem.getDefaultWorkingDirectory() + File.separator
          + value.substring(2);
      if (inputSbVector.contains(value)) {
        setExecutableRemoteEnabled(false);
        jTextFieldExecutable.setText(value);
        inputSbVector.remove(value);
      } else {
        //!!! Error to display to user.
      }
    } else if (!value.equals(fileName)
        && ((new File(value)).isAbsolute() || GUIFileSystem.isAbsolute(value))) {
      // Executable is an absolute path name.
      setExecutableRemoteEnabled(true);
      jTextFieldExecutable.setText(value);
    } else if (value.equals(fileName)) {
      String executableAbsoluteFile = getInputSbExecutableAbsoluteFile(value);
      if (!executableAbsoluteFile.equals("")) { // I have found this element in InputSandbox.
        setExecutableRemoteEnabled(false);
        jTextFieldExecutable.setText(executableAbsoluteFile);
        inputSbVector.remove(executableAbsoluteFile);
      } else { // I didn't found the element.
        setExecutableRemoteEnabled(true);
        jTextFieldExecutable.setText(value);
      }
    }
  }

  String getInputSbExecutableAbsoluteFile(String value) {
    String fileFormatErrorMsg = "";
    File file = null;
    String fileName = "";
    String currentElement = "";
    Vector fileVector = (new ExtractFiles(inputSbVector)).getMatchingFiles();
    for (int i = 0; i < fileVector.size(); i++) {
      currentElement = fileVector.get(i).toString();
    }
    for (int i = 0; i < inputSbVector.size(); i++) {
      currentElement = inputSbVector.get(i).toString();
      fileName = GUIFileSystem.getName(currentElement);
      if (fileName == null) {
        //fileFormatErrorMsg += "- InputSandbox: file \"" + currentElement + "\" has wrong format\n";
        //parserErrorMsg += "\n- InputSandbox: file \"" + currentElement + "\" has wrong format";
      } else {
        if (fileName.equals(value)) {
          return currentElement;
        }
      }
    }
    if (!fileFormatErrorMsg.equals("")) {
      JOptionPane.showOptionDialog(JobDef1Panel.this, fileFormatErrorMsg,
          Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.WARNING_MESSAGE, null, null, null);
    }
    return "";
  }

  public void setStdInputText(String value) {
    //File file = new File(value);
    //String fileName = file.getName().toString();
    String fileName = GUIFileSystem.getName(value);
    if (value.substring(0, 2).equals("." + File.separator)
        && value.substring(2).equals(fileName)) { // File of type "./name" or ".\name".
      value = GUIFileSystem.getDefaultWorkingDirectory() + File.separator
          + value.substring(2);
      if (inputSbVector.contains(value)) {
        setStdInputRemoteEnabled(false);
        jTextFieldStdInput.setText(value);
        inputSbVector.remove(value);
      } else {
        //!!! Error to display to user.
      }
    } else if (!value.equals(fileName)
        && ((new File(value)).isAbsolute() || GUIFileSystem.isAbsolute(value))) {
      // StdInput is an absolute path name.
      setStdInputRemoteEnabled(true);
      jTextFieldStdInput.setText(value);
    } else if (value.equals(fileName)) {
      String stdInputAbsoluteFile = getInputSbExecutableAbsoluteFile(value);
      if (!stdInputAbsoluteFile.equals("")) { // I have found this element in InputSandbox.
        setStdInputRemoteEnabled(false);
        jTextFieldStdInput.setText(stdInputAbsoluteFile);
        inputSbVector.remove(stdInputAbsoluteFile);
      } else { // I didn't found the element.
        //!!! Error to dispaly to user.
      }
    }
  }

  public String getLocalStdInputText(String text) {
    return jTextFieldStdInput.getText().trim();
  }

  public String getRemoteStdInputText(String text) {
    return jTextFieldStdInput.getText().trim();
  }

  public void setStdOutputText(String text) {
    File pathName = new File(text);
    jTextFieldStdOutput.setText(pathName.getName().toString());
  }

  public String getStdOutputText(String text) {
    return jTextFieldStdOutput.getText().trim();
  }

  String getStdErrorText(String text) {
    return jTextFieldStdError.getText().trim();
  }

  void setStdErrorText(String text) {
    File pathName = new File(text);
    jTextFieldStdError.setText(pathName.getName().toString());
  }

  void setInputSandboxList(Vector itemVector) {
    String item = "";
    inputSbVector.clear();
    for (int i = 0; i < itemVector.size(); i++) {
      item = itemVector.get(i).toString().trim();
      inputSbVector.add(item);
    }
    jListInputSb.setListData(inputSbVector);
  }

  void setOutputSandboxList(Vector itemVector) {
    File pathName;
    String item = "";
    String fileName = "";
    outputSbVector.clear();
    for (int i = 0; i < itemVector.size(); i++) {
      item = itemVector.get(i).toString().trim();
      pathName = new File(item);
      fileName = pathName.getName().toString();
      outputSbVector.add(fileName);
    }
    jListOutputSb.setListData(outputSbVector);
  }

  Vector getInputSandboxList() {
    return inputSbVector;
  }

  Vector getOutputSandboxList() {
    return outputSbVector;
  }

  void jTextFieldVOFocusLost(FocusEvent e) {
    GraphicUtils.jTextFieldDeselect(jTextFieldVO);
  }

  void setStandardStreamsEnabled(boolean bool) {
    jPanelStandardStreams.setEnabled(bool);
    jLabelStdInput.setEnabled(bool);
    jTextFieldStdInput.setEnabled(bool);
    jButtonFileStdInput.setEnabled(bool);
    jCheckBoxStdInputRemote.setEnabled(bool);
    jLabelStdOutput.setEnabled(bool);
    jTextFieldStdOutput.setEnabled(bool);
    jCheckBoxStdOutput.setEnabled(bool);
    jLabelStdError.setEnabled(bool);
    jTextFieldStdError.setEnabled(bool);
    jCheckBoxStdError.setEnabled(bool);
    String stdOutputText = jTextFieldStdOutput.getText().trim();
    String stdErrorText = jTextFieldStdError.getText().trim();
    if (bool) {
      if (jCheckBoxStdOutput.isSelected()) {
        if (!stdOutputText.equals("")) {
          if (!outputSbVector.contains(stdOutputText)) {
            outputSbVector.add(0, stdOutputText); // Adding at first position.
            jListOutputSb.setListData(outputSbVector);
          }
        }
      }
      if (jCheckBoxStdError.isSelected()) {
        if (!stdErrorText.equals("")) {
          if (!outputSbVector.contains(stdErrorText)) {
            outputSbVector.add(0, stdErrorText); // Adding at first position.
            jListOutputSb.setListData(outputSbVector);
          }
        }
      }
    } else {
      if (jCheckBoxStdOutput.isSelected()) {
        if (!stdOutputText.equals("")) {
          outputSbVector.removeElement(stdOutputText);
          jListOutputSb.setListData(outputSbVector);
        }
      }
      if (jCheckBoxStdError.isSelected()) {
        if (!stdErrorText.equals("")) {
          outputSbVector.removeElement(stdErrorText);
          jListOutputSb.setListData(outputSbVector);
        }
      }
    }
  }

  GridBagConstraints setDefaultGridBagConstraints(GridBagConstraints gbc) {
    return setGridBagConstraints(gbc, 0, 0, 1, 1, 0.0, 0.0,
        GridBagConstraints.CENTER, GridBagConstraints.NONE, null, 0, 0);
  }

  GridBagConstraints setGridBagConstraints(GridBagConstraints gbc, int gridx,
      int gridy, int gridwidth, int gridheight, double weightx, double weighty,
      int anchor, int fill, Insets insets, int ipadx, int ipady) {
    gbc.gridx = gridx;
    gbc.gridy = gridy;
    gbc.gridwidth = gridwidth;
    gbc.gridheight = gridheight;
    gbc.weightx = weightx;
    gbc.weighty = weighty;
    gbc.anchor = anchor;
    gbc.fill = fill;
    if (insets != null) {
      gbc.insets = insets;
    }
    gbc.ipadx = ipadx;
    gbc.ipady = ipady;
    return gbc;
  }
}