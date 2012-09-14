/*
 * JDLEditor.java 
 * 
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 * 
 */

package org.glite.wmsui.guij;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentEvent;
import java.awt.event.WindowEvent;
import java.io.File;
import java.io.StringWriter;
import java.net.URL;
import java.util.Iterator;
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTabbedPane;
import javax.swing.JTextPane;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.event.CaretEvent;
import javax.swing.event.CaretListener;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.filechooser.FileFilter;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.glite.jdl.Ad;
import org.glite.jdl.Jdl;
import org.glite.jdl.JobAd;
import org.glite.jdl.JobAdException;
import condor.classad.ClassAdWriter;
import condor.classad.RecordExpr;

/**
 * Implementation of the JDLEditor class. This class implements the JDL Editor
 * in a simple JFrame. The JFrame is a standalone application you can use to
 * create a new classad file or edit an existing one.
 * 
 * @ingroup gui
 * @brief class to implement a standalone application for classad file editing.
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class JDLEditor extends JFrame implements JDLEditorInterface {
  static Logger logger = Logger.getLogger(JDLEditor.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int FRAME_WIDTH = 640;

  static final int FRAME_HEIGHT = 720;

  static final int MAX_FRAME_WIDTH = FRAME_WIDTH + 120;

  Component startUpSelectedTab;

  JTabbedPane jTabbedJDL = new JTabbedPane();

  JScrollPane jScrollPaneTabbedJDL = new JScrollPane();

  JScrollPane jScrollPaneTextPaneJDL = new JScrollPane();

  JTextPane jTextAreaJDL = new JTextPane();

  JSplitPane splitPane = new JSplitPane();

  JPanel jPanelDesktop = new JPanel();

  JPanel jPanelJDLText = new JPanel();

  JPanel jPanelButtonPane = new JPanel();

  JPanel jPanelButtonAll = new JPanel();

  JPanel jPanelMain = new JPanel();

  JButton jButtonReset = new JButton();

  JButton jButtonView = new JButton();

  JButton jButtonOk = new JButton();

  JButton jButtonClose = new JButton();

  JButton jButtonResetAll = new JButton();

  JButton jButtonViewAll = new JButton();

  JButton jButtonCheck = new JButton();

  JButton jButtonBack = new JButton();

  JobDef1Panel jobDef1 = null;

  JobDef2Panel jobDef2 = null;

  JobInputDataPanel dataReq = null;

  JobOutputDataPanel jobOutputData = null;

  RequirementsPanel requirements = null;

  RankPanel rank = null;

  JobTypePanel checkpoint = null;

  TagPanel tags = null;

  UnknownPanel unknown = null;

  PartitionablePanel partitionable = null;

  JMenuItem jMenuItemFileParsingError = new JMenuItem("");

  JTextPane jTextPane = new JTextPane();

  JScrollPane jScrollPaneText = new JScrollPane();

  JPanel jdlTextPanel;

  JPanel jPanelFirst = new JPanel();

  JPanel unknownPanel = new JPanel();

  JPanel partitionablePanel = new JPanel();

  JMenuBar jMenuBar;

  JobAd jobAd = new JobAd();

  Ad ad = new Ad();

  JobAd jobAdGlobal = new JobAd();

  String parserErrorMsg = "";

  String errorMsg = "";

  String warningMsg = "";

  final String FRAME_TITLE = "JDL Editor";

  Vector guiAttributesVector = new Vector();

  String currentOpenedFile = "";

  String rBName = "";

  String keyJobName = "";

  String userWorkingDirectory = "";

  int savedFileCount = 0;

  boolean isJobSubmitterCalling = false;

  boolean isJobSubmitted = false;

  String jobType = Jdl.TYPE_JOB;

  JobSubmitterInterface jintSub;

  JPanelStateBar jPanelStateBar;

  JPanel jPanelState = new JPanel();

  JPanel jPanelSouth = new JPanel();

  /**
   * Constructor
   */
  public JDLEditor() {
    super("JDL Editor");
    jbInit();
  }

  /**
   * Constructor.
   */
  public JDLEditor(String rBName, String keyJobName, Component component) {
    JDLEditor.this.setTitle("JDL Editor - " + rBName + " - " + keyJobName);
    isJobSubmitterCalling = true;
    this.rBName = rBName;
    this.keyJobName = keyJobName;
    this.jintSub = (JobSubmitterInterface) component;
    jbInit();
    jobDef1.jTextFieldVO.setText(GUIGlobalVars.getVirtualOrganisation());
  }

  /**
   * Constructor.
   */
  public JDLEditor(String rBName, String keyJobName, Component component,
      boolean isDag) {
    this(rBName, keyJobName, component);
    if (isDag) {
      jobType = Jdl.TYPE_DAG;
      try {
        ad.setAttribute(Jdl.TYPE, Jdl.TYPE_DAG);
      } catch (Exception ex) {
        // Do nothing.
      }
      displayJDLText(ad.toString(true, true));
      jTextPane.grabFocus();
      jButtonBack.setEnabled(false);
      jButtonCheck.setEnabled(false);
    }
  }

  /**
   * Constructor.
   */
  public JDLEditor(String rBName, String keyJobName, String fileName,
      Component component) throws java.text.ParseException, Exception {
    this.currentOpenedFile = keyJobName;
    this.rBName = rBName;
    this.keyJobName = keyJobName;
    this.jintSub = (JobSubmitterInterface) component;
    JDLEditor.this.setTitle("JDL Editor - " + rBName + " - " + keyJobName);
    isJobSubmitterCalling = true;
    ExprChecker exprChecker = new ExprChecker();
    //ClassAdSAXParser classAdSAXParser = new ClassAdSAXParser();
    File file = new File(fileName);
    String fileExtension = GUIFileSystem.getFileExtension(file).toUpperCase();
    if (fileExtension.equals("XML")) {
      // XML file
      try {
        jobAd = exprChecker.parse(file, true, ExprChecker.XML);
        parserErrorMsg = exprChecker.getErrorMsg();
      } catch (Exception ex) {
        ex.printStackTrace();
      }
      jbInit();
      addAttributesFromJobAd(jobAd, true);
      jobDef1.jTextFieldVO.setText(GUIGlobalVars.getVirtualOrganisation());
    } else {
      // JDL file
      try {
        ad.fromFile(fileName);
        if (ad.hasAttribute(Jdl.TYPE)) {
          this.jobType = ad.getStringValue(Jdl.TYPE).get(0).toString();
        } else {
          this.jobType = Jdl.TYPE_JOB;
        }
      } catch (NoSuchFieldException nsfe) {
        // Do nothing.
      } catch (java.text.ParseException pe) {
        if (isDebugging) {
          pe.printStackTrace();
        }
        throw pe;
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        throw e;
      }
      if (this.jobType.equals(Jdl.TYPE_DAG)) { // Dag
        jMenuItemFileParsingError.setEnabled(false);
        try {
          ad.fromFile(fileName);
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
          throw new java.text.ParseException("", 0);
        }
        if (ad != null) {
          if (ad.hasAttribute(Utils.GUI_JOB_ID_ATTR_NAME)) {
            try {
              ad.delAttribute(Utils.GUI_JOB_ID_ATTR_NAME);
              if (ad.hasAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME)) {
                ad.delAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME);
              }
              isJobSubmitted = true;
            } catch (Exception ex) {
              if (isDebugging) {
                ex.printStackTrace();
              }
            }
          }
          jbInit();
          displayJDLText(ad.toString(true, true));
          jButtonBack.setEnabled(false);
          jButtonCheck.setEnabled(false);
        } else {
          //throw new UncorrectJDLFileException();
          //throw new NotAJDLFileException();
        }
      } else {
        // Normal Job
        try {
          jobAd = exprChecker.parse(file);
          parserErrorMsg = exprChecker.getErrorMsg();
        } catch (java.text.ParseException pe) {
          throw pe;
        }
        parserErrorMsg = parserErrorMsg.trim();
        if (!parserErrorMsg.equals("")) {
          jMenuItemFileParsingError.setEnabled(true);
        } else {
          jMenuItemFileParsingError.setEnabled(false);
        }
        if (jobAd != null) {
          if (jobAd.hasAttribute(Utils.GUI_JOB_ID_ATTR_NAME)) {
            try {
              jobAd.delAttribute(Utils.GUI_JOB_ID_ATTR_NAME);
              if (jobAd.hasAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME)) {
                jobAd.delAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME);
              }
              isJobSubmitted = true;
            } catch (Exception ex) {
              if (isDebugging) {
                ex.printStackTrace();
              }
            }
          }
          if (jobAd.hasAttribute(Utils.GUI_CHECKPOINT_STATE_FILE)) {
            try {
              jobAd.delAttribute(Utils.GUI_CHECKPOINT_STATE_FILE);
            } catch (Exception ex) {
              if (isDebugging) {
                ex.printStackTrace();
              }
            }
          }
          jbInit();
          addAttributesFromJobAd(jobAd, true);
          jobDef1.jTextFieldVO.setText(GUIGlobalVars.getVirtualOrganisation());
        } else {
          //throw new UncorrectJDLFileException();
          //throw new NotAJDLFileException();
          jobAd = new JobAd();
        }
      }
    }
  }

  void jbInit() {
    // Set application type. The type of application effects on some settings.
    Utils.setApplicationType(Utils.FRAME);
    String log4JConfFile = GUIFileSystem.getLog4JConfFile();
    File file = new File(log4JConfFile);
    if (file.isFile()) {
      PropertyConfigurator.configure(log4JConfFile);
    } else {
      logger.setLevel(Level.FATAL);
      Logger.getRootLogger().setLevel(Level.FATAL);
    }
    isDebugging = isDebugging
        || ((Logger.getRootLogger().getLevel() == Level.DEBUG) ? true : false);
    if (!jobType.equals(Jdl.TYPE_DAG)) {
      File confFile = new File(GUIFileSystem.getGUIConfVarFile());
      JobAd confAd = new JobAd();
      if (confFile.isFile()) {
        try {
          confAd.fromFile(confFile.toString());
          if (!isJobSubmitterCalling) {
            if (confAd.hasAttribute(Utils.GUI_CONF_VAR_JDLE_DEFAULT_SCHEMA)) {
              String schema = confAd.getStringValue(
                  Utils.GUI_CONF_VAR_JDLE_DEFAULT_SCHEMA).get(0).toString()
                  .trim().toUpperCase();
              for (int i = 0; i < Utils.jdleSchemaNameConfFile.length; i++) {
                if (schema.equals(Utils.jdleSchemaNameConfFile[i][0])) {
                  Utils.setJDLESchemaFile(schema);
                }
              }
            }
          } else {
            String userPrefFile = GUIFileSystem.getUserPrefFile();
            if (new File(userPrefFile).isFile()) {
              confAd.fromFile(userPrefFile);
            }
          }
          if (confAd.hasAttribute(Utils.confFileSchema)) {
            JobAd schemaAd = new JobAd();
            schemaAd = confAd.getJobAdValue(Utils.confFileSchema);
            if (schemaAd.hasAttribute(Jdl.RANK)) {
              GUIGlobalVars.setGUIConfVarRank(schemaAd
                  .getAttributeExpr(Jdl.RANK));
              logger.debug("setting rank default: "
                  + schemaAd.getAttributeExpr(Jdl.RANK));
            }
            if (schemaAd.hasAttribute(Jdl.REQUIREMENTS)) {
              GUIGlobalVars.setGUIConfVarRequirements(schemaAd
                  .getAttributeExpr(Jdl.REQUIREMENTS));
              logger.debug("setting requirements default: "
                  + schemaAd.getAttributeExpr(Jdl.REQUIREMENTS));
            }
            if (schemaAd.hasAttribute(Utils.GUI_CONF_VAR_RANKMPI)) {
              GUIGlobalVars.setGUIConfVarRankMPI(schemaAd
                  .getAttributeExpr(Utils.GUI_CONF_VAR_RANKMPI));
            }
          }
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
        }
      }
      // guiAttributesVector initialize. This attributes are those
      // application recognizes.
      for (int i = 0; i < Utils.guiAttributesArray.length; i++) {
        guiAttributesVector.addElement(Utils.guiAttributesArray[i]);
      }
    }
    // initializing JDL Editor panels
    jobDef1 = new JobDef1Panel(this);
    jobDef2 = new JobDef2Panel(this);
    dataReq = new JobInputDataPanel(this);
    jobOutputData = new JobOutputDataPanel(this);
    requirements = new RequirementsPanel(this);
    rank = new RankPanel(this);
    checkpoint = new JobTypePanel(this);
    tags = new TagPanel(this);
    unknown = new UnknownPanel(this);
    partitionable = new PartitionablePanel(this);
    JDLEditor.this.setSize(FRAME_WIDTH, FRAME_HEIGHT);
    this.addComponentListener(new java.awt.event.ComponentListener() {
      public void componentResized(ComponentEvent e) {
        int xPosition = getBounds().x;
        int yPosition = getBounds().y;
        int height = getBounds().height;
        int width = getBounds().width;
        if (width > MAX_FRAME_WIDTH) {
          JDLEditor.this.setBounds(new Rectangle(xPosition, yPosition,
              MAX_FRAME_WIDTH, height));
        } else {
          repaint();
        }
      }

      public void componentMoved(ComponentEvent e) {
      }

      public void componentHidden(ComponentEvent e) {
      }

      public void componentShown(ComponentEvent e) {
      }
    });
    jTabbedJDL.setPreferredSize(new Dimension(610, 525));
    checkpoint.jComboBoxJobTypeEvent(null);
    // Menu initialize.
    jMenuBar = createMenuBar();
    setJMenuBar(jMenuBar);
    jPanelDesktop.setLayout(new BorderLayout());
    // Initialize tabbed pane panels.
    Object[][] element = { { Utils.GUI_PANEL_NAMES[0], checkpoint
    }, { Utils.GUI_PANEL_NAMES[1], jobDef1
    }, { Utils.GUI_PANEL_NAMES[2], jobDef2
    }, { Utils.GUI_PANEL_NAMES[3], dataReq
    }, { Utils.GUI_PANEL_NAMES[4], jobOutputData
    }, { Utils.GUI_PANEL_NAMES[5], requirements
    }, { Utils.GUI_PANEL_NAMES[6], rank
    }, { Utils.GUI_PANEL_NAMES[8], tags
    //}, { Utils.GUI_PANEL_NAMES[9], partitionable
        }, { Utils.GUI_PANEL_NAMES[7], unknown
        }
    };
    //partitionable.setVisible(false);
    for (int i = 0; i < element.length; i++) {
      jTabbedJDL.addTab((String) element[i][0], (JPanel) element[i][1]);
    }
    //partitionablePanel = (JPanel) partitionable;
    jTabbedJDL.setTabLayoutPolicy(JTabbedPane.SCROLL_TAB_LAYOUT);
    //jTabbedJDL
    //  .setSelectedIndex(GraphicUtils.STARTUP_SELECTED_TABBED_PANE_INDEX);
    //String panelName = jTabbedJDL.getTitleAt(jTabbedJDL.getSelectedIndex());
    startUpSelectedTab = jobDef1;
    jTabbedJDL.setSelectedComponent(startUpSelectedTab);
    /*logger.debug("--------------" + panelName);
     if (panelName.equals(Utils.GUI_PANEL_NAMES[9])) {
     jButtonView.setText("  Check  ");
     } else {
     jButtonView.setText("   View   ");
     }*/
    jobDef1.jTextFieldExecutable.grabFocus();
    jScrollPaneTabbedJDL = new JScrollPane(jTabbedJDL);
    jButtonView.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonViewEvent();
      }
    });
    jButtonView.setText("   View   ");
    jButtonReset.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonResetEvent();
      }
    });
    jButtonReset.setText("  Reset  ");
    jButtonOk.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonOkEvent();
      }
    });
    jButtonOk.setText("    Ok    ");
    jButtonClose.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonCloseEvent();
      }
    });
    jButtonClose.setText("  Close   ");
    jButtonResetAll.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        int choice = JOptionPane.showOptionDialog(JDLEditor.this,
            "Do you really want to reset all?", "Confirm Reset All",
            JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null,
            null, null);
        if (choice == 0) {
          resetAll();
        }
      }
    });
    jButtonResetAll.setText("Reset All");
    jButtonViewAll.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        viewAll();
      }
    });
    jButtonViewAll.setText("View All ");
    jButtonCheck.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonCheckEvent(e);
      }
    });
    jButtonCheck.setText(" Check  ");
    jButtonBack.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        displayJPanelDesktopCheck();
      }
    });
    jButtonBack.setText("  Back   ");
    jTextAreaJDL.setEditable(false);
    jTextPane.setEditable(true);
    jTextPane.setFont(new Font("Monospaced", Font.PLAIN, 12));
    jScrollPaneTextPaneJDL = new JScrollPane(jTextAreaJDL);
    jPanelJDLText.setLayout(new BorderLayout());
    jPanelMain.setLayout(new BorderLayout());
    jPanelButtonPane.setBounds(new Rectangle(0, 0, 700, 50));
    jPanelButtonPane.setBorder(BorderFactory.createRaisedBevelBorder());
    jPanelButtonAll.setBorder(BorderFactory.createRaisedBevelBorder());
    jPanelButtonPane.add(jButtonReset);
    jPanelButtonPane.add(jButtonView);
    if (isJobSubmitterCalling) {
      if (isJobSubmitted) {
        jButtonOk.setEnabled(false);
      }
      jPanelButtonAll.add(jButtonClose);
      jPanelButtonAll.add(jButtonOk);
      jobDef1.setJTextFieldVOEditable(false);
      //!!!
      //jobDef1.jTextFieldVO.setText("to read from certificate");
    }
    jPanelButtonAll.add(jButtonResetAll);
    jPanelButtonAll.add(jButtonViewAll);
    jButtonBack.setVisible(false);
    jButtonCheck.setVisible(false);
    jPanelButtonAll.add(jButtonCheck);
    jPanelButtonAll.add(jButtonBack);
    jPanelJDLText.add(jScrollPaneTextPaneJDL, BorderLayout.CENTER);
    //jPanelJDLText.add(jPanelButtonAll, BorderLayout.SOUTH);
    jPanelMain.add(jScrollPaneTabbedJDL, BorderLayout.CENTER);
    jPanelMain.add(jPanelButtonPane, BorderLayout.SOUTH);
    //Create a split pane with two scroll panes inside.
    splitPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT, jPanelMain,
        jPanelJDLText);
    splitPane.setOneTouchExpandable(true);
    splitPane.setDividerLocation(570); // old value 600
    jPanelDesktop.add(splitPane);
    //jdlTextPanel = (JPanel) jdlText;
    jdlTextPanel = new JPanel();
    jdlTextPanel.setLayout(new BorderLayout());
    jScrollPaneText.getViewport().add(jTextPane, null);
    jdlTextPanel.add(jScrollPaneText, BorderLayout.CENTER);
    jPanelDesktop.setVisible(true);
    jdlTextPanel.setVisible(true);
    jPanelFirst.setLayout(new BorderLayout());
    getContentPane().setLayout(new BorderLayout());
    getContentPane().add(jPanelFirst, BorderLayout.CENTER);
    jPanelFirst.add(jPanelDesktop, BorderLayout.CENTER);
    //jPanelState.setBorder(new EtchedBorder());
    jPanelState.setBorder(BorderFactory.createRaisedBevelBorder());
    jPanelSouth.setLayout(new BorderLayout());
    jPanelSouth.add(jPanelState, BorderLayout.EAST);
    jPanelSouth.add(jPanelButtonAll, BorderLayout.CENTER);
    jPanelFirst.add(jPanelSouth, BorderLayout.SOUTH);
    JPanel newPanel = new JPanel();
    if (GUIFileSystem.getIsConfFileError()) {
      rank.showAdvancedPanelOnly();
      requirements.showAdvancedPanelOnly();
    }
    jobDef1.jTextFieldExecutable.grabFocus();
    jTabbedJDL.addChangeListener(new ChangeListener() {
      public void stateChanged(ChangeEvent e) {
        String panelName = jTabbedJDL.getTitleAt(jTabbedJDL.getSelectedIndex());
        if (panelName.equals(Utils.GUI_PANEL_NAMES[7])
            || panelName.equals(Utils.GUI_PANEL_NAMES[9])) {
          jButtonView.setText("  Check  ");
        } else {
          jButtonView.setText("   View   ");
        }
      }
    });
    jPanelStateBar = new JPanelStateBar(jTextPane);
    jPanelState.setVisible(false);
    jTextPane.addCaretListener(jPanelStateBar);
    ((FlowLayout) jPanelStateBar.getLayout()).setAlignment(FlowLayout.RIGHT);
    jPanelState.add(jPanelStateBar);
  }

  /**
   * Creates the menu bar of the JDL Editor
   * 
   * @return the JMenuBar object
   */
  protected JMenuBar createMenuBar() {
    final JMenuBar jMenuBar = new JMenuBar();
    ActionListener alst = null;
    JMenuItem jMenuItem = null;
    JMenu jMenuFile = null;
    if (isJobSubmitterCalling) {
      jMenuFile = new JMenu("Job");
      jMenuFile.setMnemonic('j');
      jMenuItem = new JMenuItem("     Save To File...");
    } else {
      jMenuFile = new JMenu("File");
      jMenuFile.setMnemonic('f');
      jMenuItem = new JMenuItem("Open File...");
      URL fileOpenGifUrl = JobDef1Panel.class.getResource(Utils.ICON_FILE_OPEN);
      if (fileOpenGifUrl != null) {
        jMenuItem.setIcon(new ImageIcon(fileOpenGifUrl));
      }
      jMenuItem.setMnemonic('o');
      alst = new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          jMenuFileOpen();
        }
      };
      jMenuItem.addActionListener(alst);
      jMenuFile.add(jMenuItem);
      jMenuItem = new JMenuItem("Save File");
      URL fileSaveGifUrl = JobDef1Panel.class.getResource(Utils.ICON_FILE_SAVE);
      if (fileSaveGifUrl != null) {
        jMenuItem.setIcon(new ImageIcon(fileSaveGifUrl));
      }
      jMenuItem.setMnemonic('s');
      alst = new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          jMenuFileSaveAs(currentOpenedFile);
        }
      };
      jMenuItem.addActionListener(alst);
      jMenuFile.add(jMenuItem);
      jMenuItem = new JMenuItem("     Save File As...");
    }
    jMenuItem.setMnemonic('v');
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuFileSaveAs("");
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuFile.add(jMenuItem);
    jMenuFile.addSeparator();
    jMenuItemFileParsingError = new JMenuItem("     File Parsing Error(s)");
    jMenuItemFileParsingError.setMnemonic('e');
    jMenuItemFileParsingError.setEnabled(false);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        showErrors();
      }
    };
    jMenuItemFileParsingError.addActionListener(alst);
    jMenuFile.add(jMenuItemFileParsingError);
    jMenuFile.addSeparator();
    if (isJobSubmitterCalling) {
      jMenuItem = new JMenuItem("     Close");
      jMenuItem.setMnemonic('c');
      alst = new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          int choice = JOptionPane.showOptionDialog(JDLEditor.this,
              "Do you really want to close editor?", "Confirm Close",
              JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null,
              null, null);
          if (choice == 0) {
            GUIGlobalVars.openedEditorHashMap.remove(rBName + " - "
                + keyJobName);
            JDLEditor.this.dispose();
          }
        }
      };
    } else {
      jMenuItem = new JMenuItem("     Exit");
      jMenuItem.setMnemonic('x');
      alst = new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          int choice = JOptionPane.showOptionDialog(JDLEditor.this,
              "Do you really want to exit?", "Confirm Exit",
              JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null,
              null, null);
          if (choice == 0) {
            System.exit(0);
          }
        }
      };
    }
    jMenuItem.addActionListener(alst);
    jMenuFile.add(jMenuItem);
    jMenuBar.add(jMenuFile);
    JMenu jMenuHelp = new JMenu("Help");
    jMenuHelp.setMnemonic('h');
    jMenuItem = new JMenuItem("Help Topics");
    jMenuItem.setMnemonic('h');
    //!!! To remove when coded.
    jMenuItem.setEnabled(false);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuHelp.add(jMenuItem);
    jMenuItem = new JMenuItem("About");
    jMenuItem.setMnemonic('a');
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuHelpAbout();
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuHelp.add(jMenuItem);
    jMenuBar.add(jMenuHelp);
    return jMenuBar;
  }

  /**
   * Performs Check button action
   */
  protected void jButtonCheckEvent(ActionEvent ae) {
    logger.debug("----- jTextPane text: " + jTextPane.getText().trim());
    JobAd jobAd = checkJDLText(jTextPane.getText().trim());
    logger.debug("----- jobAd: " + jobAd);
  }

  /**
   * Checks job represented by input String displaying warning/error messages to
   * the user
   * 
   * @param text
   *          the String representing the job
   * @return a JobAd containing the description of the job; <code>null</code>
   *         in case of warning or error
   */
  protected JobAd checkJDLText(String jdlText) {
    /*ExprChecker exprChecker = new ExprChecker();
     //(*)JobAd jobAd = exprChecker.parseLineByLine(jdlText);
     exprChecker.parseLineByLine(jdlText);
     String errorMsg = exprChecker.getErrorMsg().trim();
     if (!errorMsg.equals("")) {
     String firstLine = "<html><font color=\"#800080\">"
     + "JDL text contains error(s)" + "</font>" + "\n";
     GraphicUtils.showOptionDialogMsg(JDLEditor.this, errorMsg,
     Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
     JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
     firstLine, null);
     return null;
     }*/
    JobAd jobAd = new JobAd();
    logger.debug("jdlText: " + jdlText);
    try {
      jobAd.fromString(jdlText); //remove if you use (*)
      jobAd.checkAll();
    } catch (JobAdException jae) {
      String firstLine = "<html><font color=\"#800080\">"
          + "JDL file cannot be submitted" + "</font>" + "\n";
      GraphicUtils.showOptionDialogMsg(JDLEditor.this, jae.getMessage(),
          Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
          firstLine, null);
      return jobAd;
    } catch (Exception e) {
      String firstLine = "<html><font color=\"#800080\">"
          + "JDL file cannot be submitted" + "</font>" + "\n";
      GraphicUtils.showOptionDialogMsg(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
          firstLine, null);
      return null;
    }
    return jobAd;
  }

  /**
   * Displays the desktop panel (tabbed pane) after making a check of the jdl
   * text contained in the text area
   */
  void displayJPanelDesktopCheck() {
    JobAd jobAd = checkJDLText(jTextPane.getText().trim());
    if (jobAd == null) {
      return;
    }
    // clears all jdl editor panel
    resetAll();
    // inserts all the attribute in the jdl editor panels from the JobAd
    addAttributesFromJobAd(jobAd, false);
    if (!jobAd.hasAttribute(Jdl.RETRYCOUNT)) {
      GUIGlobalVars.setGUIConfVarRetryCount(GUIGlobalVars.NO_RETRY_COUNT);
    }
    if (isJobSubmitterCalling) {
      String workingVirtualOrganisation = GUIGlobalVars
          .getVirtualOrganisation();
      if (!jobDef1.getVOText().toUpperCase().equals(
          workingVirtualOrganisation.toUpperCase())) {
        JOptionPane.showOptionDialog(JDLEditor.this,
            "Cannot change 'VirtualOrganisation' attribute value"
                + "\nPrevious value '" + workingVirtualOrganisation
                + "' will be restored", Utils.WARNING_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null,
            null, null);
        jobDef1.setVOText(workingVirtualOrganisation);
      }
    }
    displayJPanelDesktop();
  }

  /**
   * Displays jdl text area (textual editor)
   */
  void displayJDLText(String result) {
    jPanelFirst.remove(jPanelDesktop);
    jButtonResetAll.setVisible(false);
    jButtonViewAll.setVisible(false);
    jButtonCheck.setVisible(true);
    jButtonBack.setVisible(true);
    jPanelState.setVisible(true);
    jPanelFirst.add(jdlTextPanel, BorderLayout.CENTER);
    jTextPane.setText(result);
    validate();
    repaint();
  }

  /**
   * Performs OK button action
   */
  void jButtonOkEvent() {
    if (!jButtonBack.isVisible()) {
      boolean showWarningMsg = false;
      boolean showErrorMsg = true;
      String jdlText = yieldJDLText(showWarningMsg, showErrorMsg);
      if (errorMsg.equals("")) {
        if (!warningMsg.equals("")) {
          GraphicUtils.showOptionDialogMsg(JDLEditor.this, warningMsg,
              Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
              null, null);
        } else {
          JobAd jobAd = new JobAd();
          try {
            jobAd.fromString(jdlText);
            jintSub.addJobToTable(rBName, keyJobName, currentOpenedFile, jobAd);
            GUIGlobalVars.openedEditorHashMap.remove(rBName + " - "
                + keyJobName);
            JDLEditor.this.dispose();
          } catch (Exception e) {
            if (isDebugging) {
              e.printStackTrace();
            }
            GraphicUtils.showOptionDialogMsg(JDLEditor.this, e.getMessage(),
                Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
                null, null);
            return;
          }
        }
      }
    } else {
      if (!jobType.equals(Jdl.TYPE_DAG)) {
        JobAd jobAd = checkJDLText(jTextPane.getText().trim());
        if (jobAd == null) {
          return;
        }
        String workingVirtualOrganisation = GUIGlobalVars
            .getVirtualOrganisation();
        String jobAdVirtualOrganisation = "";
        try {
          jobAdVirtualOrganisation = jobAd.getStringValue(
              Jdl.VIRTUAL_ORGANISATION).get(0).toString().trim();
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
          JOptionPane.showOptionDialog(JDLEditor.this, "Unable to Add job",
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
          return;
        }
        if (!jobAdVirtualOrganisation.toUpperCase().equals(
            workingVirtualOrganisation.toUpperCase())) {
          JOptionPane.showOptionDialog(JDLEditor.this,
              "Cannot change 'VirtualOrganisation' attribute value"
                  + "\nPrevious value '" + workingVirtualOrganisation
                  + "' will be restored", Utils.WARNING_MSG_TXT,
              JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null,
              null, null);
          try {
            jobAd.delAttribute(Jdl.VIRTUAL_ORGANISATION);
            jobAd.setAttribute(Jdl.VIRTUAL_ORGANISATION,
                workingVirtualOrganisation);
          } catch (Exception e) {
            if (isDebugging) {
              e.printStackTrace();
            }
            JOptionPane.showOptionDialog(JDLEditor.this, "Unable to Add job",
                Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE, null, null, null);
            return;
          }
        }
        jintSub.addJobToTable(rBName, keyJobName, currentOpenedFile, jobAd);
        GUIGlobalVars.openedEditorHashMap.remove(rBName + " - " + keyJobName);
        JDLEditor.this.dispose();
      } else {
        Ad ad = new Ad();
        try {
          ad.fromString(jTextPane.getText().trim());
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
          JOptionPane.showOptionDialog(JDLEditor.this,
              "Edited Dag contains syntax error(s)", Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
              null, null);
          return;
        }
        String workingVirtualOrganisation = GUIGlobalVars
            .getVirtualOrganisation();
        String jobAdVirtualOrganisation = "";
        try {
          jobAdVirtualOrganisation = ad
              .getStringValue(Jdl.VIRTUAL_ORGANISATION).get(0).toString()
              .trim();
        } catch (Exception e) {
          // Attribute not present, setting default.
          jobAdVirtualOrganisation = workingVirtualOrganisation;
          /*
           * if (isDebugging) e.printStackTrace();
           * JOptionPane.showOptionDialog(JDLEditor.this, "Unable to Add Dag",
           * Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
           * JOptionPane.ERROR_MESSAGE, null, null, null); return;
           */
        }
        if (!jobAdVirtualOrganisation.toUpperCase().equals(
            workingVirtualOrganisation.toUpperCase())) {
          JOptionPane.showOptionDialog(JDLEditor.this,
              "Cannot change 'VirtualOrganisation' attribute value"
                  + "\nPrevious value '" + workingVirtualOrganisation
                  + "' will be restored", Utils.WARNING_MSG_TXT,
              JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null,
              null, null);
          try {
            ad.delAttribute(Jdl.VIRTUAL_ORGANISATION);
            ad.setAttribute(Jdl.VIRTUAL_ORGANISATION,
                workingVirtualOrganisation);
          } catch (Exception e) {
            if (isDebugging) {
              e.printStackTrace();
            }
            JOptionPane.showOptionDialog(JDLEditor.this, "Unable to Add Dag",
                Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE, null, null, null);
            return;
          }
        }
        try {
          if (ad.hasAttribute(Jdl.TYPE)) {
            ad.delAttribute(Jdl.TYPE);
          }
          ad.addAttribute(Jdl.TYPE, Jdl.TYPE_DAG);
        } catch (Exception ex) {
        }
        jintSub.addJobToTable(rBName, keyJobName, currentOpenedFile, ad);
        GUIGlobalVars.openedEditorHashMap.remove(rBName + " - " + keyJobName);
        JDLEditor.this.dispose();
      }
    }
  }

  /**
   * Performs Close button action
   */
  void jButtonCloseEvent() {
    GUIGlobalVars.openedEditorHashMap.remove(rBName + " - " + keyJobName);
    JDLEditor.this.dispose();
  }

  /**
   * Performs View button action
   */
  void jButtonViewEvent() {
    String paneName = jTabbedJDL.getTitleAt(jTabbedJDL.getSelectedIndex());
    if (paneName.equals(Utils.GUI_PANEL_NAMES[0])) {
      checkpoint.jButtonCheckpointViewEvent(true, true, null);
    } else if (paneName.equals(Utils.GUI_PANEL_NAMES[1])) {
      jobDef1.jButtonJobDef1ViewEvent(true, true, null);
    } else if (paneName.equals(Utils.GUI_PANEL_NAMES[2])) {
      jobDef2.jButtonJobDef2ViewEvent(true, true, null);
    } else if (paneName.equals(Utils.GUI_PANEL_NAMES[3])) {
      dataReq.jButtonDataReqViewEvent(true, true, null);
    } else if (paneName.equals(Utils.GUI_PANEL_NAMES[4])) {
      jobOutputData.jButtonJobOutputDataViewEvent(true, true, null);
    } else if (paneName.equals(Utils.GUI_PANEL_NAMES[5])) {
      if (requirements.jButtonAdvanced.getText().equals("Advanced >>")) {
        requirements.jButtonRequirementsViewEvent(true, true, null);
      } else {
        requirements.requirementsAdvancedPanel
            .jButtonRequirementsAdvancedPanelViewEvent(true, true, null);
      }
    } else if (paneName.equals(Utils.GUI_PANEL_NAMES[6])) {
      if (rank.jButtonAdvanced.getText().equals("Advanced >>")) {
        rank.jButtonRankViewEvent(true, true, null);
      } else {
        rank.rankAdvancedPanel.jButtonRankAdvancedPanelViewEvent(true, true,
            null);
      }
    } else if (paneName.equals(Utils.GUI_PANEL_NAMES[7])) {
      unknown.jButtonUnknownViewEvent(true, true, null);
    } else if (paneName.equals(Utils.GUI_PANEL_NAMES[8])) {
      tags.jButtonTagsViewEvent(true, true, null);
    }
  }

  String yieldJDLText(boolean showWarningMsg, boolean showErrorMsg) {
    String result = checkpoint.jButtonCheckpointViewEvent(false, false, null);
    if (checkpoint.getJobTypeValue() == Jdl.JOBTYPE_PARTITIONABLE) {
      result += partitionable.jButtonPartitionableViewEvent(false, false, null);
    }
    result += jobDef1.jButtonJobDef1ViewEvent(false, false, null);
    result += jobDef2.jButtonJobDef2ViewEvent(false, false, null);
    result += dataReq.jButtonDataReqViewEvent(false, false, null);
    result += jobOutputData.jButtonJobOutputDataViewEvent(false, false, null);
    if (rank.jButtonAdvanced.getText().equals("Advanced >>")) {
      result += rank.jButtonRankViewEvent(false, false, null);
    } else {
      result += rank.rankAdvancedPanel.jButtonRankAdvancedPanelViewEvent(false,
          false, null);
    }
    if (requirements.jButtonAdvanced.getText().equals("Advanced >>")) {
      result += requirements.jButtonRequirementsViewEvent(false, false, null);
    } else {
      result += requirements.requirementsAdvancedPanel
          .jButtonRequirementsAdvancedPanelViewEvent(false, false, null);
    }
    //result += unknown.getUnknownText();
    result += tags.jButtonTagsViewEvent(false, false, null);
    result += unknown.jButtonUnknownViewEvent(false, false, null);
    setJTextAreaJDL("");
    String panelNameJobType = "<html><font color=\"#602080\">"
        + Utils.GUI_PANEL_NAMES[0] + ":" + "</font>";
    String panelNameJobDefinition1 = "<html><font color=\"#602080\">"
        + Utils.GUI_PANEL_NAMES[1] + ":" + "</font>";
    String panelNameJobDefinition2 = "<html><font color=\"#602080\">"
        + Utils.GUI_PANEL_NAMES[2] + ":" + "</font>";
    String panelNameJobInputData = "<html><font color=\"#602080\">"
        + Utils.GUI_PANEL_NAMES[3] + ":" + "</font>";
    String panelNameJobOutputData = "<html><font color=\"#602080\">"
        + Utils.GUI_PANEL_NAMES[4] + ":" + "</font>";
    String panelNameRequirements = "<html><font color=\"#602080\">"
        + Utils.GUI_PANEL_NAMES[5] + ":" + "</font>";
    String panelNameRank = "<html><font color=\"#602080\">"
        + Utils.GUI_PANEL_NAMES[6] + ":" + "</font>";
    String panelNameUnknown = "<html><font color=\"#602080\">"
        + Utils.GUI_PANEL_NAMES[7] + ":" + "</font>";
    String panelNameTags = "<html><font color=\"#602080\">"
        + Utils.GUI_PANEL_NAMES[8] + ":" + "</font>";
    String panelNamePartitionable = "<html><font color=\"#602080\">"
        + Utils.GUI_PANEL_NAMES[9] + ":" + "</font>";
    this.errorMsg = "";
    if (!checkpoint.getErrorMsg().equals("")) {
      errorMsg += panelNameJobType + "\n" + checkpoint.getErrorMsg() + "\n";
    }
    if (!partitionable.getErrorMsg().equals("")) {
      errorMsg += panelNamePartitionable + "\n" + partitionable.getErrorMsg()
          + "\n";
    }
    if (!jobDef1.getErrorMsg().equals("")) {
      errorMsg += panelNameJobDefinition1 + "\n" + jobDef1.getErrorMsg() + "\n";
    }
    if (!jobDef2.getErrorMsg().equals("")) {
      errorMsg += panelNameJobDefinition2 + "\n" + jobDef2.getErrorMsg() + "\n";
    }
    if (!dataReq.getErrorMsg().equals("")) {
      errorMsg += panelNameJobInputData + "\n" + dataReq.getErrorMsg() + "\n";
    }
    if (!jobOutputData.getErrorMsg().equals("")) {
      errorMsg += panelNameJobOutputData + "\n" + jobOutputData.getErrorMsg()
          + "\n";
    }
    if (requirements.jButtonAdvanced.getText().equals("Advanced >>")) {
      if (!requirements.getErrorMsg().equals("")) {
        errorMsg += panelNameRequirements + "\n" + requirements.getErrorMsg()
            + "\n";
      }
    } else {
      if (!requirements.getErrorMsg().equals("")) {
        errorMsg += panelNameRequirements + "\n"
            + requirements.requirementsAdvancedPanel.getErrorMsg() + "\n";
      }
    }
    if (rank.jButtonAdvanced.getText().equals("Advanced >>")) {
      if (!rank.getErrorMsg().equals("")) {
        errorMsg += panelNameRank + "\n" + rank.getErrorMsg() + "\n";
      }
    } else {
      if (!rank.getErrorMsg().equals("")) {
        errorMsg += panelNameRank + "\n" + rank.rankAdvancedPanel.getErrorMsg()
            + "\n";
      }
    }
    if (!tags.getErrorMsg().equals("")) {
      errorMsg += panelNameTags + "\n" + tags.getErrorMsg() + "\n";
    }
    if (!unknown.getErrorMsg().equals("")) {
      errorMsg += panelNameUnknown + "\n" + unknown.getErrorMsg() + "\n";
    }
    errorMsg = errorMsg.trim();
    if (!errorMsg.equals("") && showErrorMsg) {
      GraphicUtils.showOptionDialogMsg(JDLEditor.this, errorMsg,
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE, null,
          null);
      return null;
    } else {
      ExprChecker exprChecker = new ExprChecker();
      String defaultRank = "";
      if (checkpoint.getJobTypeValue().equals(Jdl.JOBTYPE_MPICH)
          && !GUIGlobalVars.getGUIConfVarRankMPI().equals("")) {
        defaultRank = GUIGlobalVars.getGUIConfVarRankMPI();
      } else {
        defaultRank = GUIGlobalVars.getGUIConfVarRank();
      }
      JobAd jobAd = exprChecker.parse("[" + result + "]", true, GUIGlobalVars
          .getGUIConfVarRequirements(), defaultRank);
      warningMsg = "";
      {
        Vector errorAttributeVector = exprChecker.getErrorAttributeVector();
        Vector errorTypeMsgVector = exprChecker.getErrorTypeMsgVector();
        String jobTypeWarningMsg = "";
        Vector jobTypeAttributeVector = new Vector();
        for (int i = 0; i < Utils.jobTypeAttributeArray.length; i++) {
          jobTypeAttributeVector.add(Utils.jobTypeAttributeArray[i]);
        }
        String partitionableWarningMsg = "";
        Vector partitionableAttributeVector = new Vector();
        for (int i = 0; i < Utils.partitionableAttributeArray.length; i++) {
          partitionableAttributeVector
              .add(Utils.partitionableAttributeArray[i]);
        }
        String jobDefinition1WarningMsg = "";
        Vector jobDefinition1AttributeVector = new Vector();
        for (int i = 0; i < Utils.jobDefinition1AttributeArray.length; i++) {
          jobDefinition1AttributeVector
              .add(Utils.jobDefinition1AttributeArray[i]);
        }
        String jobDefinition2WarningMsg = "";
        Vector jobDefinition2AttributeVector = new Vector();
        for (int i = 0; i < Utils.jobDefinition2AttributeArray.length; i++) {
          jobDefinition2AttributeVector
              .add(Utils.jobDefinition2AttributeArray[i]);
        }
        String jobInputDataWarningMsg = "";
        Vector jobInputDataAttributeVector = new Vector();
        for (int i = 0; i < Utils.jobInputDataAttributeArray.length; i++) {
          jobInputDataAttributeVector.add(Utils.jobInputDataAttributeArray[i]);
        }
        String jobOutputDataWarningMsg = "";
        Vector jobOutputDataAttributeVector = new Vector();
        for (int i = 0; i < Utils.jobOutputDataAttributeArray.length; i++) {
          jobOutputDataAttributeVector
              .add(Utils.jobOutputDataAttributeArray[i]);
        }
        String requirementsWarningMsg = "";
        Vector requirementsAttributeVector = new Vector();
        for (int i = 0; i < Utils.requirementsAttributeArray.length; i++) {
          requirementsAttributeVector.add(Utils.requirementsAttributeArray[i]);
        }
        String rankWarningMsg = "";
        Vector rankAttributeVector = new Vector();
        for (int i = 0; i < Utils.rankAttributeArray.length; i++) {
          rankAttributeVector.add(Utils.rankAttributeArray[i]);
        }
        String tagsWarningMsg = "";
        Vector tagsAttributeVector = new Vector();
        for (int i = 0; i < Utils.tagsAttributeArray.length; i++) {
          tagsAttributeVector.add(Utils.tagsAttributeArray[i]);
        }
        String currentErrorAttribute = "";
        String currentErrorTypeMsg = "";
        for (int i = 0; i < errorAttributeVector.size(); i++) {
          currentErrorAttribute = errorAttributeVector.get(i).toString();
          currentErrorTypeMsg = errorTypeMsgVector.get(i).toString();
          if (jobTypeAttributeVector.contains(currentErrorAttribute)) {
            jobTypeWarningMsg += currentErrorTypeMsg + "\n";
          } else if (partitionableAttributeVector
              .contains(currentErrorAttribute)) {
            partitionableWarningMsg += currentErrorTypeMsg + "\n";
          } else if (jobDefinition1AttributeVector
              .contains(currentErrorAttribute)) {
            jobDefinition1WarningMsg += currentErrorTypeMsg + "\n";
          } else if (jobDefinition2AttributeVector
              .contains(currentErrorAttribute)) {
            jobDefinition2WarningMsg += currentErrorTypeMsg + "\n";
          } else if (jobInputDataAttributeVector
              .contains(currentErrorAttribute)) {
            jobInputDataWarningMsg += currentErrorTypeMsg + "\n";
          } else if (jobOutputDataAttributeVector
              .contains(currentErrorAttribute)) {
            jobOutputDataWarningMsg += currentErrorTypeMsg + "\n";
          } else if (requirementsAttributeVector
              .contains(currentErrorAttribute)) {
            requirementsWarningMsg += currentErrorTypeMsg; //!!! + "\n";
          } else if (rankAttributeVector.contains(currentErrorAttribute)) {
            rankWarningMsg += currentErrorTypeMsg + "\n";
          } else if (tagsAttributeVector.contains(currentErrorAttribute)) {
            tagsWarningMsg += currentErrorTypeMsg + "\n";
          }
        }
        if (!jobTypeWarningMsg.equals("")) {
          warningMsg += panelNameJobType + "\n" + jobTypeWarningMsg;
        }
        if (!partitionableWarningMsg.equals("")) {
          warningMsg += panelNamePartitionable + "\n" + partitionableWarningMsg;
        }
        if (!jobDefinition1WarningMsg.equals("")) {
          warningMsg += panelNameJobDefinition1 + "\n"
              + jobDefinition1WarningMsg;
        }
        if (!jobDef1.getGUIWarningMsg().equals("")) {
          jobDefinition1WarningMsg += jobDef1.getGUIWarningMsg();
        }
        if (!jobDefinition2WarningMsg.equals("")) {
          warningMsg += panelNameJobDefinition2 + "\n"
              + jobDefinition2WarningMsg + "\n";
        }
        if (!jobInputDataWarningMsg.equals("")) {
          warningMsg += panelNameJobInputData + "\n" + jobInputDataWarningMsg
              + "\n";
        }
        if (!jobOutputDataWarningMsg.equals("")) {
          warningMsg += panelNameJobOutputData + "\n" + jobOutputDataWarningMsg
              + "\n";
        }
        if (!requirementsWarningMsg.equals("")) {
          warningMsg += panelNameRequirements + "\n" + requirementsWarningMsg
              + "\n";
        }
        if (!rankWarningMsg.equals("")) {
          warningMsg += panelNameRank + "\n" + rankWarningMsg + "\n";
        }
        if (!tagsWarningMsg.equals("")) {
          warningMsg += panelNameTags + "\n" + tagsWarningMsg + "\n";
        }
      }
      System.gc();
      if (jobAd != null) {
        result = jobAd.toLines();
      }
      warningMsg = warningMsg.trim();
      if (!warningMsg.equals("") && showWarningMsg) {
        String firstLine = "<html><font color=\"#800080\">"
            + "JDL file cannot be submitted." + "</font>" + "\n";
        int choice = GraphicUtils.showOptionDialogMsg(JDLEditor.this,
            warningMsg, Utils.WARNING_MSG_TXT, JOptionPane.YES_NO_OPTION,
            JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
            firstLine, "Do you want to see it anyway?");
        if (choice != 0) {
          return null;
        }
      }
      return result;
    }
  }

  void showErrors() {
    setJTextAreaJDL(parserErrorMsg);
  }

  /**
   * Performs Reset button action
   */
  void jButtonResetEvent() {
    JPanel selectedPane = (JPanel) jTabbedJDL.getSelectedComponent();
    String paneName = jTabbedJDL.getTitleAt(jTabbedJDL.getSelectedIndex());
    int choice = JOptionPane.showOptionDialog(JDLEditor.this,
        "Do you really want to Reset '" + paneName + "' panel?",
        "Confirm Reset", JOptionPane.YES_NO_OPTION,
        JOptionPane.QUESTION_MESSAGE, null, null, null);
    if (choice == 0) {
      if (paneName.equals(Utils.GUI_PANEL_NAMES[0])) {
        checkpoint.jButtonCheckpointResetEvent(null);
      } else if (paneName.equals(Utils.GUI_PANEL_NAMES[9])) {
        partitionable.jButtonPartitionableResetEvent(null);
      } else if (paneName.equals(Utils.GUI_PANEL_NAMES[1])) {
        jobDef1.jButtonJobDef1ResetEvent(null);
      } else if (paneName.equals(Utils.GUI_PANEL_NAMES[2])) {
        jobDef2.jButtonJobDef2ResetEvent(null);
      } else if (paneName.equals(Utils.GUI_PANEL_NAMES[3])) {
        dataReq.jButtonDataReqResetEvent(null);
      } else if (paneName.equals(Utils.GUI_PANEL_NAMES[4])) {
        jobOutputData.jButtonJobOutputDataResetEvent(null);
      } else if (paneName.equals(Utils.GUI_PANEL_NAMES[5])) {
        if (requirements.jButtonAdvanced.getText().equals("Advanced >>")) {
          requirements.jButtonRequirementsResetEvent(null);
        } else {
          requirements.requirementsAdvancedPanel
              .jButtonRequirementsAdvancedPanelResetEvent(null);
        }
      } else if (paneName.equals(Utils.GUI_PANEL_NAMES[6])) {
        if (rank.jButtonAdvanced.getText().equals("Advanced >>")) {
          rank.jButtonRankResetEvent(null);
        } else {
          rank.rankAdvancedPanel.jButtonRankAdvancedPanelResetEvent(null);
        }
      } else if (paneName.equals(Utils.GUI_PANEL_NAMES[7])) {
        unknown.jButtonUnknownClearEvent(null);
      } else if (paneName.equals(Utils.GUI_PANEL_NAMES[8])) {
        tags.jButtonTagsResetEvent(null);
      }
    }
  }

  void resetAll() {
    jobDef1.jButtonJobDef1ResetEvent(null);
    if (isJobSubmitterCalling) {
      jobDef1.jTextFieldVO.setText(GUIGlobalVars.getVirtualOrganisation());
      String hlrLocation = GUIGlobalVars.getHLRLocation();
      jobDef2.setHLRLocationText(hlrLocation);
      String myProxyServer = GUIGlobalVars.getMyProxyServer();
      jobDef2.setMyProxyServerText(myProxyServer);
    }
    partitionable.jButtonPartitionableResetEventNoMsg(null);
    jobDef2.jButtonJobDef2ResetEvent(null);
    dataReq.jButtonDataReqResetEvent(null);
    jobOutputData.jButtonJobOutputDataResetEvent(null);
    requirements.jButtonRequirementsResetEvent(null);
    requirements.requirementsAdvancedPanel
        .jButtonRequirementsAdvancedPanelResetEvent(null);
    rank.jButtonRankResetEvent(null);
    rank.rankAdvancedPanel.jButtonRankAdvancedPanelResetEvent(null);
    checkpoint.jButtonCheckpointResetEvent(null);
    tags.jButtonTagsResetEvent(null);
    unknown.jButtonUnknownClearEventNoMsg(null);
    parserErrorMsg = "";
    jobAdGlobal.clear();
    if (!isJobSubmitterCalling) {
      jobDef1.jTextFieldVO.grabFocus();
    }
    if (!isJobSubmitterCalling) {
      currentOpenedFile = "";
      JDLEditor.this.setTitle(FRAME_TITLE);
    }
    jMenuItemFileParsingError.setEnabled(false);
  }

  void resetAllOpen() {
    partitionable.jButtonPartitionableResetEventNoMsg(null);
    jobDef1.jButtonJobDef1ResetEvent(null);
    jobDef2.jButtonJobDef2ResetEvent(null);
    dataReq.jButtonDataReqResetEvent(null);
    jobOutputData.jButtonJobOutputDataResetEvent(null);
    requirements.jButtonRequirementsResetEvent(null);
    requirements.requirementsAdvancedPanel
        .jButtonRequirementsAdvancedPanelResetEvent(null);
    rank.jButtonRankResetEvent(null);
    rank.rankAdvancedPanel.jButtonRankAdvancedPanelResetEvent(null);
    checkpoint.jButtonCheckpointResetEvent(null);
    tags.jButtonTagsResetEvent(null);
    unknown.jButtonUnknownClearEventNoMsg(null);
  }

  /*
   * boolean isFilePresentPathName(String element, Vector vector) { File
   * pathName; String fileName = ""; for (int i = 0; i < vector.size(); i++) {
   * pathName = new File(vector.get(i).toString()); fileName =
   * pathName.getName().toString(); if (fileName.equals(element)) { return true; } }
   * return false; }
   */
  boolean isFilePresentNamePath(String element, Vector vector) {
    File pathName;
    String fileName = "";
    for (int i = 0; i < vector.size(); i++) {
      pathName = new File(element);
      fileName = pathName.getName().toString();
      if (vector.get(i).toString().equals(fileName)) {
        return true;
      }
    }
    return false;
  }

  private void addAttributesFromJobAd(JobAd jobAd, boolean setDefault) {
    Vector jobTypesVector = new Vector();
    try {
      jobTypesVector = jobAd.getStringValue(Jdl.JOBTYPE);
      String value = "";
      if (jobTypesVector.size() == 1) {
        value = jobTypesVector.get(0).toString();
      } else if (jobTypesVector.contains(Jdl.JOBTYPE_INTERACTIVE)
          && jobTypesVector.contains(Jdl.JOBTYPE_CHECKPOINTABLE)) {
        value = Jdl.JOBTYPE_CHECKPOINTABLE + Utils.JOBTYPE_LIST_SEPARATOR
            + Jdl.JOBTYPE_INTERACTIVE;
      } else if (jobTypesVector.contains(Jdl.JOBTYPE_INTERACTIVE)
          && jobTypesVector.contains(Jdl.JOBTYPE_MPICH)) {
        value = Jdl.JOBTYPE_CHECKPOINTABLE + Utils.JOBTYPE_LIST_SEPARATOR
            + Jdl.JOBTYPE_MPICH;
      }
      checkpoint.setJobType(value);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    try {
      String value = jobAd.getStringValue(Jdl.VIRTUAL_ORGANISATION).get(0)
          .toString();
      jobDef1.setVOText(value);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    // For "Executable" see below after InputSandbox.
    try {
      String value = jobAd.getStringValue(Jdl.ARGUMENTS).get(0).toString();
      jobDef1.setArgumentsText(value);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    // For "Executable" see below after InputSandbox.
    // For "StdOutput" and "StdError" see below after InputSandbox.
    try {
      Vector itemVector = jobAd.getStringValue(Jdl.OUTPUTSB);
      jobDef1.setOutputSandboxList(itemVector);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    try {
      Vector itemVector = jobAd.getStringValue(Jdl.INPUTSB);
      jobDef1.setInputSandboxList(itemVector);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    try {
      String value = jobAd.getStringValue(Jdl.STDOUTPUT).get(0).toString();
      jobDef1.setStdOutputText(value);
      if (isFilePresentNamePath(value, jobDef1.outputSbVector)) {
        jobDef1.jCheckBoxStdOutput.setSelected(true);
      }
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    try {
      String value = jobAd.getStringValue(Jdl.STDERROR).get(0).toString();
      jobDef1.setStdErrorText(value);
      if (isFilePresentNamePath(value, jobDef1.outputSbVector)) {
        jobDef1.jCheckBoxStdError.setSelected(true);
      }
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    // You must do the set for "Executable" and "StdInput" after "InputSandbox"
    // setting.
    try {
      String value = jobAd.getStringValue(Jdl.EXECUTABLE).get(0).toString();
      jobDef1.setExecutableText(value);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    try {
      String value = jobAd.getStringValue(Jdl.STDINPUT).get(0).toString();
      jobDef1.setStdInputText(value);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    try {
      Vector itemVector = jobAd.getStringValue(Jdl.ENVIRONMENT);
      jobDef2.setEnvironmentList(itemVector);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    try {
      String value = "";
      if (jobAd.hasAttribute(Jdl.MYPROXY)) {
        value = jobAd.getStringValue(Jdl.MYPROXY).get(0).toString().trim();
        if (value.equals("") && isJobSubmitterCalling) {
          value = GUIGlobalVars.getMyProxyServer();
        }
      } else if (isJobSubmitterCalling && setDefault) {
        value = GUIGlobalVars.getMyProxyServer();
      }
      jobDef2.setMyProxyServerText(value);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    try {
      String value = "";
      if (jobAd.hasAttribute(Jdl.HLR_LOCATION)) {
        value = jobAd.getStringValue(Jdl.HLR_LOCATION).get(0).toString().trim();
        if (value.equals("") && isJobSubmitterCalling) {
          value = GUIGlobalVars.getHLRLocation();
        }
      } else if (isJobSubmitterCalling && setDefault) {
        value = GUIGlobalVars.getHLRLocation();
      }
      jobDef2.setHLRLocationText(value);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    if (jobAd.hasAttribute(Jdl.RETRYCOUNT)) {
      String value = (jobAd.lookup(Jdl.RETRYCOUNT).toString());
      jobDef2.setRetryCountValue(value);
    }
    if (jobAd.hasAttribute(Jdl.SHPORT)) {
      if (checkpoint.jComboBoxJobType.getSelectedItem().toString().equals(
          Jdl.JOBTYPE_INTERACTIVE)) {
        String value = (jobAd.lookup(Jdl.SHPORT).toString());
        checkpoint.setListenerPortValue(value);
      }
    }
    try {
      Vector itemVector = jobAd.getStringValue(Jdl.INPUTDATA);
      dataReq.setInputDataList(itemVector);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    try {
      Vector itemVector = jobAd.getStringValue(Jdl.DATA_ACCESS);
      dataReq.setDataAccessProtocolList(itemVector);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    try {
      Vector adVector = jobAd.getAdValue(Jdl.OUTPUTDATA);
      jobOutputData.setOutputData(adVector);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    try {
      String value = jobAd.getStringValue(Jdl.OUTPUT_SE).get(0).toString();
      jobOutputData.setOutputSEText(value);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    ///////////////////////////////////
    // Requirements EXPRESSION TREE. //
    ///////////////////////////////////
    try {
      String value = jobAd.getAttributeExpr(Jdl.REQUIREMENTS).trim();
      requirements.setRequirementsTree(value);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    ///////////////////////////
    // Rank EXPRESSION TREE. //
    ///////////////////////////
    if (jobAd.hasAttribute(Jdl.FUZZY_RANK)) {
      String value = (jobAd.lookup(Jdl.FUZZY_RANK).toString());
      rank.setFuzzyRankValue(value);
    }
    try {
      String value = jobAd.getAttributeExpr(Jdl.RANK).trim();
      if (!value.equals("")) {
        rank.setRankTree(value);
      }
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    //////////////////////
    // Type ATTRIBUTES. //
    //////////////////////
    if (jobTypesVector.contains(Jdl.JOBTYPE_NORMAL)) {
      if (jobAd.hasAttribute(Jdl.NODENUMB)) {
        String value = (jobAd.lookup(Jdl.NODENUMB).toString());
        checkpoint.setNodeNumberValue(value);
        checkpoint.setNodeNumberVisible(true);
      }
    }
    if (jobTypesVector.contains(Jdl.JOBTYPE_INTERACTIVE)) {
      if (jobAd.hasAttribute(Jdl.SHPORT)) {
        String value = (jobAd.lookup(Jdl.SHPORT).toString());
        checkpoint.setListenerPortValue(value);
        checkpoint.setListenerPortVisible(true);
      }
    }
    if (jobTypesVector.contains(Jdl.JOBTYPE_CHECKPOINTABLE)) {
      try {
        int type = jobAd.getType(Jdl.CHKPT_STEPS);
        try {
          if (type == JobAd.TYPE_INTEGER) {
            Vector valueVector = jobAd.getIntValue(Jdl.CHKPT_STEPS);
            checkpoint.setJobStepsValue(valueVector.get(0).toString());
            if (jobAd.hasAttribute(Jdl.CHKPT_CURRENTSTEP)) {
              String value = (jobAd.lookup(Jdl.CHKPT_CURRENTSTEP).toString());
              checkpoint.setCurrentStepValue(value);
            }
            checkpoint.setNumericValueSelected(true);
          } else {
            Vector itemVector = jobAd.getStringValue(Jdl.CHKPT_STEPS);
            checkpoint.setJobStepsList(itemVector);
            if (jobAd.hasAttribute(Jdl.CHKPT_CURRENTSTEP)) {
              String value = (jobAd.lookup(Jdl.CHKPT_CURRENTSTEP).toString());
              checkpoint.setCurrentIndexValue(value);
            }
            checkpoint.setNumericValueSelected(false);
          }
          checkpoint.setJobStepsSelected(true);
        } catch (Exception e) {
          JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
          if (isDebugging) {
            e.printStackTrace();
          }
        }
      } catch (NoSuchFieldException nsfe) {
        // Attribute not present in jobad.
      }
    }
    if (jobTypesVector.contains(Jdl.JOBTYPE_PARTITIONABLE)) {
      setPartitionablePanelVisible(true);
      try {
        if (jobAd.hasAttribute(Jdl.PRE_JOB)) {
          JobAd preJobAd = jobAd.getJobAdValue(Jdl.PRE_JOB);
          if (preJobAd.size() != 0) {
            partitionable.setPartitionablePreText(preJobAd);
          }
        }
        if (jobAd.hasAttribute(Jdl.POST_JOB)) {
          JobAd postJobAd = jobAd.getJobAdValue(Jdl.POST_JOB);
          if (postJobAd.size() != 0) {
            partitionable.setPartitionablePostText(postJobAd);
          }
        }
      } catch (Exception e) {
        JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        if (isDebugging) {
          e.printStackTrace();
        }
      }
      try {
        int type = jobAd.getType(Jdl.CHKPT_STEPS);
        try {
          if (type == JobAd.TYPE_INTEGER) {
            Vector valueVector = jobAd.getIntValue(Jdl.CHKPT_STEPS);
            checkpoint.setJobStepsValue(valueVector.get(0).toString());
            if (jobAd.hasAttribute(Jdl.CHKPT_CURRENTSTEP)) {
              String value = (jobAd.lookup(Jdl.CHKPT_CURRENTSTEP).toString());
              checkpoint.setCurrentStepValue(value);
            }
            checkpoint.setNumericValueSelected(true);
          } else {
            Vector itemVector = jobAd.getStringValue(Jdl.CHKPT_STEPS);
            Vector weightVector = new Vector();
            if (jobAd.hasAttribute(Jdl.STEP_WEIGHT)) {
              weightVector = jobAd.getIntValue(Jdl.STEP_WEIGHT);
              checkpoint.setJobStepsTable(itemVector, weightVector);
            } else {
              checkpoint.setJobStepsTable(itemVector);
            }
            if (jobAd.hasAttribute(Jdl.CHKPT_CURRENTSTEP)) {
              String value = (jobAd.lookup(Jdl.CHKPT_CURRENTSTEP).toString());
              checkpoint.setCurrentIndexValue(value);
            }
            checkpoint.setNumericValueSelected(false);
          }
          checkpoint.setJobStepsSelected(true);
        } catch (Exception e) {
          JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
          if (isDebugging) {
            e.printStackTrace();
          }
        }
      } catch (NoSuchFieldException nsfe) {
        // Attribute not present in jobad.
      }
    }
    //////////////////////
    // Tags ATTRIBUTES. //
    //////////////////////
    try {
      Ad adUserTags = jobAd.getAd(Jdl.USER_TAGS);
      tags.setUserTags(adUserTags);
    } catch (NoSuchFieldException nsfe) {
      // Attribute not present in jobad.
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      JOptionPane.showOptionDialog(JDLEditor.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
    }
    /////////////////////////
    // Unknown ATTRIBUTES. //
    /////////////////////////
    Iterator iterator = jobAd.attributes();
    String element = "";
    String unknownAttributesText = "";
    for (; iterator.hasNext();) {
      element = iterator.next().toString();
      if (!Utils.isInVectorCi(element, guiAttributesVector)) {
        unknownAttributesText += element + " = "
            + jobAd.lookup(element).toString() + ";\n";
      }
    }
    if (!unknownAttributesText.trim().equals("")) {
      unknown.setUnknownText(unknownAttributesText);
      //setUnknownPaneVisible(true);
    }
    if (!parserErrorMsg.equals("")) {
      jMenuItemFileParsingError.setEnabled(true);
      GraphicUtils.showOptionDialogMsg(JDLEditor.this, parserErrorMsg,
          Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
          null, null);
      logger.debug("parserErrorMsg: " + parserErrorMsg);
      writeErrorLogFile(parserErrorMsg);
    } else {
      jMenuItemFileParsingError.setEnabled(false);
    }
  } /*
   * String getLocalExecutableText() { return
   * jobDef1.jTextFieldExecutable.getText().trim(); } String
   * getRemoteExecutableText() { return
   * jobDef1.jTextFieldExecutable.getText().trim(); }
   */

  void jMenuFileOpen(String fileName) throws java.text.ParseException {
    ExprChecker exprChecker = new ExprChecker();
    File file = new File(fileName);
    String fileExtension = GUIFileSystem.getFileExtension(file).toUpperCase();
    this.jobType = Jdl.TYPE_JOB;
    if (fileExtension.equals("XML")) {
      try {
        jobAd = exprChecker.parse(file, true, ExprChecker.XML);
        parserErrorMsg = exprChecker.getErrorMsg();
      } catch (java.text.ParseException pe) {
        throw pe;
      }
      if (jobAd != null) {
        resetAllOpen(); // Clear all fields before inserting new values and
        // remove Unknown pane.
        addAttributesFromJobAd(jobAd, true);
        jButtonBack.setEnabled(true);
        displayJPanelDesktop();
        //jTabbedJDL
        //  .setSelectedIndex(GraphicUtils.STARTUP_SELECTED_TABBED_PANE_INDEX);
        jTabbedJDL.setSelectedComponent(startUpSelectedTab);
        //jobDef1.jTextFieldExecutable.grabFocus();
      }
    } else {
      Ad ad = new Ad();
      try {
        ad.fromFile(fileName);
        if (ad.hasAttribute(Jdl.TYPE)) {
          this.jobType = ad.getStringValue(Jdl.TYPE).get(0).toString();
        }
      } catch (NoSuchFieldException nsfe) {
        // Do nothing.
      } catch (java.text.ParseException pe) {
        if (isDebugging) {
          pe.printStackTrace();
        }
        throw pe;
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
      if (jobType.equals(Jdl.TYPE_DAG)) {
        jMenuItemFileParsingError.setEnabled(false);
        try {
          ad.fromFile(fileName);
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
          throw new java.text.ParseException("", 0);
        }
        if (ad != null) {
          resetAllOpen(); // Clear all fields before inserting new values and
          // remove Unknown pane.
          displayJDLText(ad.toString(true, true));
          jButtonBack.setEnabled(false);
          jButtonCheck.setEnabled(false);
        }
      } else {
        // Normal Job
        try {
          jobAd = exprChecker.parse(file);
          parserErrorMsg = exprChecker.getErrorMsg();
        } catch (java.text.ParseException pe) {
          throw pe;
        }
        parserErrorMsg = parserErrorMsg.trim();
        if (!parserErrorMsg.equals("")) {
          jMenuItemFileParsingError.setEnabled(true);
        } else {
          jMenuItemFileParsingError.setEnabled(false);
        }
        if (jobAd != null) {
          resetAllOpen();
          addAttributesFromJobAd(jobAd, true);
          jButtonBack.setEnabled(true);
          displayJPanelDesktop();
          //jTabbedJDL
          //  .setSelectedIndex(GraphicUtils.STARTUP_SELECTED_TABBED_PANE_INDEX);
          jTabbedJDL.setSelectedComponent(startUpSelectedTab);
          //jobDef1.jTextFieldExecutable.grabFocus();
        }
      }
    }
  }

  void jMenuFileOpen() {
    JFileChooser fileChooser = new JFileChooser();
    fileChooser.setCurrentDirectory(new File(GUIGlobalVars
        .getFileChooserWorkingDirectory()));
    String[] extensionsJDL = { "JDL"
    };
    GUIFileFilter classadFileFilter = new GUIFileFilter("*.jdl", extensionsJDL);
    fileChooser.addChoosableFileFilter(classadFileFilter);
    String[] extensionsXML = { "XML"
    };
    classadFileFilter = new GUIFileFilter("*.xml", extensionsXML);
    fileChooser.addChoosableFileFilter(classadFileFilter);
    String[] extensionsJDLXML = { "JDL", "XML"
    };
    classadFileFilter = new GUIFileFilter("*.jdl, *.xml", extensionsJDLXML);
    fileChooser.addChoosableFileFilter(classadFileFilter);
    int choice = fileChooser.showOpenDialog(JDLEditor.this);
    if (choice != JFileChooser.APPROVE_OPTION) {
      return;
    } else if (!fileChooser.getSelectedFile().isFile()) {
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      JOptionPane.showOptionDialog(JDLEditor.this, "Unable to find file: "
          + selectedFile, Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return;
    } else {
      GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
          .getCurrentDirectory().toString());
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      try {
        jMenuFileOpen(selectedFile);
        currentOpenedFile = selectedFile;
        JDLEditor.this.setTitle("JDL Editor - " + currentOpenedFile);
      } catch (java.text.ParseException pe) {
        JOptionPane.showOptionDialog(JDLEditor.this,
            "Selected file is not a valid classad file or it is empty",
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
    }
  }

  void jMenuFileSaveAs(String selectedFile) {
    boolean showWarningMsg = false;
    boolean showErrorMsg = false;
    String jdlText = "";
    if (!this.jobType.equals(Jdl.TYPE_DAG)) {
      if (!jButtonBack.isVisible()) {
        jdlText = yieldJDLText(showWarningMsg, showErrorMsg);
        if (!errorMsg.equals("")) {
          String msg = "<html><font color=\"#800080\">"
              + "JDL file cannot be saved. It contains error(s):" + "</font>"
              + "\n";
          GraphicUtils.showOptionDialogMsg(JDLEditor.this, errorMsg,
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
              msg, null);
          return;
        } else if (!warningMsg.equals("")) {
          int choice = GraphicUtils.showOptionDialogMsg(JDLEditor.this,
              warningMsg, Utils.WARNING_MSG_TXT, JOptionPane.YES_NO_OPTION,
              JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
              null, "Do you want to save it anyway?");
          if (choice != 0) {
            return;
          }
        }
      } else {
        JobAd jobAd = checkJDLText(jTextPane.getText().trim());
        if (jobAd == null) {
          return;
        }
        jdlText = jobAd.toString(true, true);
      }
    } else {
      Ad ad = new Ad();
      try {
        ad.fromString(jTextPane.getText().trim());
        jdlText = ad.toString(true, true);
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(JDLEditor.this,
            "Dag file cannot be saved. It contains syntax error(s)",
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        return;
      }
    }
    logger.debug("jdlText: " + jdlText);
    if (jdlText != null) {
      File outputFile = null;
      int choice = -1;
      if (selectedFile.equals("")) {
        JFileChooser fileChooser = new JFileChooser();
        fileChooser.setCurrentDirectory(new File(GUIGlobalVars
            .getFileChooserWorkingDirectory()));
        String[] extensions1 = { "XML"
        };
        GUIFileFilter classadFileFilter = new GUIFileFilter("xml", extensions1);
        fileChooser.addChoosableFileFilter(classadFileFilter);
        String[] extensions2 = { "JDL"
        };
        classadFileFilter = new GUIFileFilter("jdl", extensions2);
        fileChooser.addChoosableFileFilter(classadFileFilter);
        savedFileCount++;
        if (currentOpenedFile.equals("")) {
          fileChooser
              .setSelectedFile(new File(
                  GUIFileSystem.DEFAULT_JDL_EDITOR_SAVE_FILE_NAME
                      + savedFileCount));
        } else {
          String fileName = (new File(currentOpenedFile)).getName().toString();
          String extension = GUIFileSystem.getFileExtension(new File(fileName));
          if (extension.length() != 0) {
            fileChooser.setSelectedFile(new File(fileName.substring(0, fileName
                .length()
                - (extension.length() + 1)))); // Remove extension (and dot).
          } else {
            fileChooser.setSelectedFile(new File(fileName.substring(0, fileName
                .length())));
          }
        }
        choice = fileChooser.showSaveDialog(JDLEditor.this);
        if (choice != JFileChooser.APPROVE_OPTION) {
          savedFileCount--;
          return;
        } else {
          GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
              .getCurrentDirectory().toString());
          File file = fileChooser.getSelectedFile();
          selectedFile = file.toString();
          String extension = GUIFileSystem.getFileExtension(file).toUpperCase();
          FileFilter selectedFileFilter = fileChooser.getFileFilter();
          if (!extension.equals("JDL")
              && selectedFileFilter.getDescription().equals("jdl")) {
            selectedFile += ".jdl";
          } else if (!extension.equals("XML")
              && selectedFileFilter.getDescription().equals("xml")) {
            selectedFile += ".xml";
          }
        }
        outputFile = new File(selectedFile);
        if (outputFile.isFile()) {
          choice = JOptionPane.showOptionDialog(JDLEditor.this,
              "Output file exists. Overwrite?", "Confirm Save",
              JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null,
              null, null);
        }
      } else {
        choice = 0;
        outputFile = new File(selectedFile);
      }
      if (choice == 0) {
        try {
          ExprChecker exprChecker = new ExprChecker();
          String fileExtension = GUIFileSystem.getFileExtension(outputFile)
              .toUpperCase();
          if (fileExtension.equals("XML")) {
            JobAd jobAd = exprChecker.parse(jdlText);
            jobAd.toLines();
            RecordExpr recordExpr = jobAd.copyAd();
            StringWriter stringWriter = new StringWriter();
            ClassAdWriter classAdWriter = new ClassAdWriter(stringWriter,
                ClassAdWriter.XML);
            classAdWriter.enableFormatFlags(ClassAdWriter.MULTI_LINE_LISTS);
            classAdWriter.enableFormatFlags(ClassAdWriter.MULTI_LINE_ADS);
            classAdWriter.print(recordExpr);
            classAdWriter.close();
            String xmlText = stringWriter.toString().trim();
            GUIFileSystem.saveTextFile(outputFile, xmlText);
          } else {
            //!!!GUIFileSystem.saveTextFile(outputFile, jdlText, true);
            GUIFileSystem.saveTextFile(outputFile, jdlText);
          }
          if (!isJobSubmitterCalling) {
            currentOpenedFile = selectedFile;
            JDLEditor.this.setTitle("JDL Editor - " + currentOpenedFile);
          }
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
          JOptionPane.showOptionDialog(JDLEditor.this, ex.getMessage(),
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
        }
      } else {
        savedFileCount--;
      }
    }
  }

  void jMenuHelpAbout() {
    URL url = JobDef1Panel.class.getResource(Utils.ICON_DATAGRID_LOGO);
    JOptionPane.showOptionDialog(JDLEditor.this,
    //Utils.JDL_EDITOR_ABOUT_MSG,
        "JDL Editor Version " + GUIGlobalVars.getGUIConfVarVersion() + "\n\n"
            + Utils.COPYRIGHT, "About", JOptionPane.DEFAULT_OPTION,
        JOptionPane.INFORMATION_MESSAGE, (url == null) ? null : new ImageIcon(
            url), null, null);
  }

  /**
   * Sets if the Unknown panel must be visible or not
   * 
   * @param bool
   */
  protected void setUnknownPaneVisible(boolean bool) {
    if (bool) {
      jTabbedJDL.addTab(Utils.GUI_PANEL_NAMES[7], unknownPanel);
    } else {
      //jTabbedJDL
      //  .setSelectedIndex(GraphicUtils.STARTUP_SELECTED_TABBED_PANE_INDEX);
      jTabbedJDL.setSelectedComponent(startUpSelectedTab);
      jTabbedJDL.remove(unknownPanel);
    }
  }

  /**
   * main() method
   * 
   * @param args
   *          command line arguments
   */
  public static void main(String[] args) {
    JFrame frame = new JDLEditor();
    GraphicUtils.screenCenterWindow(frame);
    frame.show();
  }

  protected void processWindowEvent(WindowEvent e) {
    super.processWindowEvent(e);
    this.setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
    if (e.getID() == WindowEvent.WINDOW_CLOSING) {
      if (isJobSubmitterCalling) {
        int choice = JOptionPane.showOptionDialog(JDLEditor.this,
            "Do you really want to close editor?", "Confirm Close",
            JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null,
            null, null);
        if (choice == 0) {
          GUIGlobalVars.openedEditorHashMap.remove(rBName + " - " + keyJobName);
          JDLEditor.this.dispose();
        }
      } else {
        int choice = JOptionPane.showOptionDialog(JDLEditor.this,
            "Do you really want to exit?", "Confirm Exit",
            JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null,
            null, null);
        if (choice == 0) {
          System.exit(0);
        }
      }
    }
  }

  void writeErrorLogFile(String parserErrorMsg) {
    try {
      String fileName = GUIGlobalVars.getGUIConfVarErrorStorage()
          + File.separator + GUIFileSystem.ERROR_LOG_FILE_NAME + "_"
          + System.getProperty("user.name") + System.currentTimeMillis()
          + GUIFileSystem.ERROR_LOG_EXTENSION;
      String text = "\nJDL Editor Version "
          + GUIGlobalVars.getGUIConfVarVersion()
          + " parser error(s) log file.\nThis file contains error(s) "
          + "found during file parsing." + "\n" + "\n" + parserErrorMsg + "\n";
      GUIFileSystem.saveTextFile(fileName, text);
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
      JOptionPane.showOptionDialog(JDLEditor.this,
          "Unable to write error log file:" + ex.getMessage(),
          Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.WARNING_MESSAGE, null, null, null);
    }
  }

  /*****************************************************************************
   * JDLEditorInterface methods implementation
   ****************************************************************************/
  // get, set methods //
  public String getUserWorkingDirectory() {
    return userWorkingDirectory;
  }

  public void setUserWorkingDirectory(String directory) {
    userWorkingDirectory = directory;
  }

  public String getNodeNumberValue() {
    return checkpoint.getNodeNumberValue();
  }

  public String getJobTypeValue() {
    return checkpoint.getJobTypeValue();
  }

  public String getHTMLParameter(String paramName) {
    return "";
  }

  public void setJTextAreaJDL(String jdlText) {
    jTextAreaJDL.setText(jdlText);
  }

  // view, display methods //
  public void viewAll() {
    errorMsg = "";
    warningMsg = "";
    boolean showWarningMsg = true;
    boolean showErrorMsg = true;
    String result = yieldJDLText(showWarningMsg, showErrorMsg);
    if (result != null) {
      displayJDLText(result);
    }
  }

  /**
   * Performs View button action
   */
  public String jButtonDataReqViewEvent(boolean showWarningMsg,
      boolean showErrorMsg, ActionEvent ae) {
    return dataReq.jButtonDataReqViewEvent(false, false, null);
  }

  /**
   * Displays the desktop panel (tabbed pane editor)
   */
  public void displayJPanelDesktop() {
    jPanelFirst.remove(jdlTextPanel);
    jButtonBack.setVisible(false);
    jButtonCheck.setVisible(false);
    jButtonResetAll.setVisible(true);
    jButtonViewAll.setVisible(true);
    jPanelState.setVisible(false);
    jPanelFirst.add(jPanelDesktop, BorderLayout.CENTER);
    validate();
    repaint();
  }

  // enabling methods //
  public void setDef1StandardStreamsEnabled(boolean bool) {
    jobDef1.setStandardStreamsEnabled(bool);
  }

  public void setListenerPortEnabled(boolean bool) {
    checkpoint.setListenerPortEnabled(bool);
  }

  // exit method //
  public void exitApplication() {
    System.exit(-1);
  }

  public void setPartitionablePanelVisible(boolean bool) {
    if (bool) {
      jTabbedJDL.insertTab(Utils.GUI_PANEL_NAMES[9], null, partitionable, null,
          1);
    } else {
      jTabbedJDL.remove(partitionable);
    }
  }
}
/*******************************************************************************
 * CLASS JPanelStateBar
 ******************************************************************************/

class JPanelStateBar extends JPanel implements CaretListener {
  String newline = "\n";

  JTextPane jTextPane;

  JLabel jLabelRow = new JLabel("Line:");

  JLabel jLabelRowValue = new JLabel();

  JLabel jLabelColumn = new JLabel("Column:");

  JLabel jLabelColumnValue = new JLabel();

  public JPanelStateBar(JTextPane jTextPane) {
    this.jTextPane = jTextPane;
    jLabelRowValue.setBorder(BorderFactory.createLoweredBevelBorder());
    jLabelRowValue.setPreferredSize(new Dimension(30, 18));
    jLabelRowValue.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelColumnValue.setBorder(BorderFactory.createLoweredBevelBorder());
    jLabelColumnValue.setPreferredSize(new Dimension(30, 18));
    jLabelColumnValue.setHorizontalAlignment(SwingConstants.RIGHT);
    ((FlowLayout) this.getLayout()).setAlignment(FlowLayout.RIGHT);
    this.add(this.jLabelRow);
    this.add(this.jLabelRowValue);
    this.add(this.jLabelColumn);
    this.add(this.jLabelColumnValue);
  }

  public void caretUpdate(CaretEvent e) {
    displaySelectionInfo(e.getDot(), e.getMark());
  }

  protected void displaySelectionInfo(final int dot, final int mark) {
    SwingUtilities.invokeLater(new Runnable() {
      public void run() {
        String text = jTextPane.getText().substring(0, dot);
        char currentChar;
        int row = 1;
        int column = 1;
        for (int i = 0; i < text.length(); i++) {
          currentChar = text.charAt(i);
          if (currentChar == '\n') {
            row++;
            column = 0;
          }
          column++;
        }
        JPanelStateBar.this.jLabelRowValue.setText("" + row);
        JPanelStateBar.this.jLabelColumnValue.setText("" + column);
      }
    });
  }
}