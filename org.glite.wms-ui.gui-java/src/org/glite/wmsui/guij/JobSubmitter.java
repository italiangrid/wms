/*
 * JobSubmitter.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.awt.AWTEvent;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowEvent;
import java.io.File;
import java.net.URL;
import java.util.Date;
import java.util.Vector;
import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.KeyStroke;
import javax.swing.event.MenuEvent;
import javax.swing.event.MenuListener;
import javax.swing.filechooser.FileFilter;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Ad;
import org.glite.jdl.Jdl;
import org.glite.jdl.JobAd;
import org.glite.jdl.JobState;
import org.glite.wmsui.apij.Job;
import org.glite.wmsui.apij.JobCollection;
import org.glite.wmsui.apij.JobCollectionException;
import org.glite.wmsui.apij.JobId;
import org.glite.wmsui.apij.Result;
import org.glite.wmsui.apij.Url;
import org.glite.wmsui.apij.UserCredential;

/**
 * Implementation of the JobSubmitter class.
 * This class implements the main part of the Job Submitter application
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class JobSubmitter extends JFrame implements JobSubmitterInterface {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final String TITLE = "Job Submitter";

  private Vector nsVector = new Vector();

  private Vector nsNameVector = new Vector();

  private Vector menuNSVector = new Vector();

  // Popup menu.
  protected JMenuBar jMenuBar;

  JMenuItem jMenuItemCut = new JMenuItem("Cut");

  JMenuItem jMenuItemCopy = new JMenuItem("Copy");

  JMenuItem jMenuItemPaste = new JMenuItem("Paste");

  protected JMenu jMenuCopyTo = new JMenu("     Copy to");

  protected JMenu jMenuMoveTo = new JMenu("     Move to");

  JMenuItem jMenuItemSelectAll = new JMenuItem("     Select All");

  JMenuItem jMenuItemSelectNone = new JMenuItem("     Select None");

  JMenuItem jMenuItemSelectSubmitted = new JMenuItem("     Select Submitted");

  JMenuItem jMenuItemInvertSelection = new JMenuItem("     Invert Selection");

  JPanel jPanelMain = new JPanel();

  JScrollPane jScrollPaneMain = new JScrollPane();

  JButton jButtonEditor = new JButton();

  JButton jButtonSubmit = new JButton();

  JButton jButtonMonitor = new JButton();

  JPanel jPanelButton = new JPanel();

  JPanel jPanelJTabbedPaneRB = new JPanel();

  JTabbedPane jTabbedPaneRB = new JTabbedPane();

  JobMonitor jobMonitorJFrame;

  JobSubmitterPreferences jobSubmitterPreferences;

  /**
   * Constructor
   */
  public JobSubmitter() {
    super(TITLE + " - Virtual Organisation: "
        + GUIGlobalVars.getVirtualOrganisation());
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit();
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    GraphicUtils.screenCenterWindow(this);
    GUIUserCredentials credential = new GUIUserCredentials(this);
    credential.setModal(true);
    GraphicUtils.screenCenterWindow(credential);
    credential.show();
  }

  private void jbInit() throws Exception {
    isDebugging = isDebugging
        || ((Logger.getRootLogger().getLevel() == Level.DEBUG) ? true : false);
    // Set application type. The type of application affects on some settings.
    Utils.setApplicationType(Utils.FRAME);
    // Set the items selectable from Edit menu.
    /*ImageIcon iconLogo = null;
    URL urlLogo = JobSubmitter.class.getResource(Utils.ICON_DATAGRID_LOGO);
    if (urlLogo != null) {
      iconLogo = new ImageIcon(urlLogo);
    }
    this.setIconImage(iconLogo.getImage());*/
    setJMenuEditItemsEnabled();
    Toolkit toolkit = getToolkit();
    Dimension screenSize = toolkit.getScreenSize();
    this.setSize(new Dimension(
        (int) (screenSize.width * GraphicUtils.SCREEN_WIDTH_PROPORTION),
        (int) (screenSize.height * GraphicUtils.SCREEN_HEIGHT_PROPORTION)));
    jButtonEditor.setText("Editor");
    jButtonEditor.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonEditorEvent(e, true);
      }
    });
    jButtonMonitor.setText("Monitor");
    jButtonMonitor.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonMonitorEvent(e);
      }
    });
    jButtonSubmit.setText("Submit");
    jButtonSubmit.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonSubmitEvent(e);
      }
    });
    jPanelButton.setLayout(new BoxLayout(jPanelButton, BoxLayout.X_AXIS));
    jPanelButton.setBorder(GraphicUtils.SPACING_BORDER);
    jPanelButton.add(jButtonEditor, null);
    jPanelButton.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelButton.add(jButtonMonitor, null);
    jPanelButton.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelButton.add(Box.createGlue());
    jPanelButton.add(jButtonSubmit, null);
    jPanelJTabbedPaneRB.setLayout(new BorderLayout());
    jPanelJTabbedPaneRB.add(jTabbedPaneRB, BorderLayout.CENTER);
    jPanelMain.setLayout(new BorderLayout());
    jPanelMain.add(jPanelJTabbedPaneRB, BorderLayout.CENTER);
    jPanelMain.add(jPanelButton, BorderLayout.SOUTH);
    this.getContentPane().setLayout(new BorderLayout());
    this.getContentPane().add(jPanelMain, null);
    jMenuBar = createMenuBar();
    setJMenuBar(jMenuBar);
  }

  /**
   * Creates the Menu bar for the application.
   */
  protected JMenuBar createMenuBar() {
    final JMenuBar jMenuBar = new JMenuBar();
    ActionListener alst = null;
    JMenuItem jMenuItem = null;
    // Job Menu.
    JMenu jMenuJob = new JMenu("Job");
    jMenuJob.setMnemonic('j');
    ImageIcon iconNew = null;
    URL url = JobSubmitter.class.getResource(Utils.ICON_FILE_NEW);
    if (url != null) {
      iconNew = new ImageIcon(url);
    }
    Action actionNew = new AbstractAction("New", iconNew) {
      public void actionPerformed(ActionEvent e) {
        jMenuFileNew(e);
      }
    };
    jMenuItem = jMenuJob.add(actionNew);
    jMenuItem.setMnemonic('n');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N,
        GraphicUtils.KEY_EVENT_MASK));
    actionNew = new AbstractAction("     New Dag") {
      public void actionPerformed(ActionEvent e) {
        jMenuFileNewDag(e);
      }
    };
    jMenuItem = jMenuJob.add(actionNew);
    jMenuItem.setMnemonic('g');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_G,
        GraphicUtils.KEY_EVENT_MASK));
    jMenuItem = new JMenuItem("Open in Editor...");
    url = JobSubmitter.class.getResource(Utils.ICON_FILE_OPEN);
    if (url != null) {
      jMenuItem.setIcon(new ImageIcon(url));
    }
    jMenuItem.setMnemonic('o');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O,
        GraphicUtils.KEY_EVENT_MASK));
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuFileOpenInEditor();
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuJob.add(jMenuItem);
    jMenuItem = new JMenuItem("     Add...");
    //jMenuItem.setIcon(new ImageIcon("file_open.gif"));
    jMenuItem.setMnemonic('d');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_D,
        GraphicUtils.KEY_EVENT_MASK));
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuJobAddFile();
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuJob.add(jMenuItem);
    jMenuJob.addSeparator();
    jMenuItem = new JMenuItem("Save Job Ids List File...");
    url = JobSubmitter.class.getResource(Utils.ICON_FILE_SAVE);
    if (url != null) {
      jMenuItem.setIcon(new ImageIcon(url));
    }
    jMenuItem.setMnemonic('s');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S,
        GraphicUtils.KEY_EVENT_MASK));
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuSaveJobIdList(e);
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuJob.add(jMenuItem);
    jMenuItem = new JMenuItem("     Save Selected Job Ids List File...");
    jMenuItem.setMnemonic('a');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A,
        GraphicUtils.KEY_EVENT_MASK));
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuSaveSelectedJobIdList(e);
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuJob.add(jMenuItem);
    jMenuJob.addSeparator();
    jMenuItem = new JMenuItem("     Exit");
    jMenuItem.setMnemonic('x');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_W,
        GraphicUtils.KEY_EVENT_MASK));
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuExit(false);
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuJob.add(jMenuItem);
    jMenuItem = new JMenuItem("     Exit & Clean Context");
    jMenuItem.setMnemonic('l');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_L,
        GraphicUtils.KEY_EVENT_MASK));
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuExit(true);
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuJob.add(jMenuItem);
    jMenuBar.add(jMenuJob);
    // Edit Menu.
    JMenu jMenuEdit = new JMenu("Edit");
    jMenuEdit.setMnemonic('e');
    MenuListener ml = new MenuListener() {
      public void menuSelected(MenuEvent e) {
        setNSMenuItems(JobSubmitter.this.nsNameVector);
      }

      public void menuDeselected(MenuEvent e) {
      }

      public void menuCanceled(MenuEvent e) {
      }
    };
    jMenuEdit.addMenuListener(ml);
    url = JobSubmitter.class.getResource(Utils.ICON_CUT);
    if (url != null) {
      jMenuItemCut.setIcon(new ImageIcon(url));
    }
    jMenuItemCut.setMnemonic('t');
    jMenuItemCut.setAccelerator(GraphicUtils.CUT_ACCELERATOR);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuCut();
      }
    };
    jMenuItemCut.addActionListener(alst);
    jMenuEdit.add(jMenuItemCut);
    url = JobSubmitter.class.getResource(Utils.ICON_COPY);
    if (url != null) {
      jMenuItemCopy.setIcon(new ImageIcon(url));
    }
    jMenuItemCopy.setMnemonic('c');
    jMenuItemCopy.setAccelerator(GraphicUtils.COPY_ACCELERATOR);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuCopy();
      }
    };
    jMenuItemCopy.addActionListener(alst);
    jMenuEdit.add(jMenuItemCopy);
    url = JobSubmitter.class.getResource(Utils.ICON_PASTE);
    if (url != null) {
      jMenuItemPaste.setIcon(new ImageIcon(url));
    }
    jMenuItemPaste.setMnemonic('p');
    jMenuItemPaste.setAccelerator(GraphicUtils.PASTE_ACCELERATOR);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuPaste();
      }
    };
    jMenuItemPaste.addActionListener(alst);
    jMenuEdit.add(jMenuItemPaste);
    jMenuEdit.addSeparator();
    jMenuCopyTo.setMnemonic('o');
    jMenuEdit.add(jMenuCopyTo);
    jMenuMoveTo.setMnemonic('v');
    jMenuEdit.add(jMenuMoveTo);
    jMenuEdit.addSeparator();
    //jMenuItem.setIcon(new ImageIcon("copy.gif"));
    jMenuItemSelectAll.setMnemonic('a');
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuSelectAll();
      }
    };
    jMenuItemSelectAll.addActionListener(alst);
    jMenuEdit.add(jMenuItemSelectAll);
    //jMenuItem.setIcon(new ImageIcon("copy.gif"));
    jMenuItemSelectNone.setMnemonic('n');
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonNoneEvent(null);
      }
    };
    jMenuItemSelectNone.addActionListener(alst);
    jMenuEdit.add(jMenuItemSelectNone);
    //jMenuItem.setIcon(new ImageIcon("copy.gif"));
    jMenuItemSelectSubmitted.setMnemonic('s');
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonSubmittedEvent(null);
      }
    };
    jMenuItemSelectSubmitted.addActionListener(alst);
    jMenuEdit.add(jMenuItemSelectSubmitted);
    //jMenuItem.setIcon(new ImageIcon("copy.gif"));
    jMenuItemInvertSelection.setMnemonic('i');
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuInvertSelection();
      }
    };
    jMenuItemInvertSelection.addActionListener(alst);
    jMenuEdit.add(jMenuItemInvertSelection);
    jMenuEdit.addSeparator();
    jMenuItem = new JMenuItem("     Preferences");
    jMenuItem.setMnemonic('r');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_P,
        GraphicUtils.KEY_EVENT_MASK));
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuJobPreferences();
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuEdit.add(jMenuItem);
    jMenuBar.add(jMenuEdit);
    // Checkpoint Menu.
    JMenu jMenuCheckpoint = new JMenu("Checkpoint");
    jMenuCheckpoint.setMnemonic('c');
    jMenuItem = new JMenuItem("Open Checkpoint State...");
    jMenuItem.setMnemonic('o');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_K,
        GraphicUtils.KEY_EVENT_MASK));
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuOpenCheckPointState();
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuCheckpoint.add(jMenuItem);
    jMenuItem = new JMenuItem("Retrieve Checkpoint State...");
    //jMenuItem.setIcon(new ImageIcon("file_open.gif"));
    jMenuItem.setMnemonic('r');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_R,
        GraphicUtils.KEY_EVENT_MASK));
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuRetrieveCheckpointState("", true);
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuCheckpoint.add(jMenuItem);
    jMenuBar.add(jMenuCheckpoint);
    // Proxy Menu.
    JMenu jMenuProxy = new JMenu("Credential");
    jMenuProxy.setMnemonic('r');
    jMenuItem = new JMenuItem("Info");
    //jMenuItem.setIcon(new ImageIcon("file_open.gif"));
    jMenuItem.setMnemonic('i');
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        GUIUserCredentials credential = new GUIUserCredentials(
            JobSubmitter.this, GUIUserCredentials.INFO_MODE);
        credential.setModal(true);
        GraphicUtils.windowCenterWindow(JobSubmitter.this, credential);
        credential.setTitle(credential.getTitle() + " Info");
        credential.show();
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuProxy.add(jMenuItem);
    jMenuItem = new JMenuItem("Select");
    jMenuItem.setMnemonic('s');
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        GUIUserCredentials credential = new GUIUserCredentials(
            JobSubmitter.this, GUIUserCredentials.CHANGE_VO_MODE);
        credential.setModal(true);
        GraphicUtils.windowCenterWindow(JobSubmitter.this, credential);
        credential.setTitle(credential.getTitle() + " Select");
        credential.show();
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuProxy.add(jMenuItem);
    jMenuBar.add(jMenuProxy);
    // Help Menu.
    JMenu jMenuHelp = new JMenu("Help");
    jMenuHelp.setMnemonic('h');
    jMenuItem = new JMenuItem("Help Topics");
    //jMenuItem.setIcon(new ImageIcon("file_open.gif"));
    jMenuItem.setMnemonic('h');
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        //jMenuHelp();
      }
    };
    jMenuItem.addActionListener(alst);
    //!!! To remove when coded.
    jMenuItem.setEnabled(false);
    jMenuHelp.add(jMenuItem);
    jMenuItem = new JMenuItem("About");
    //jMenuItem.setIcon(new ImageIcon("file_open.gif"));
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
   * main method
   * @param args
   */
  public static void main(String[] args) {
    JFrame frame = new JobSubmitter();
  }

  protected void processWindowEvent(WindowEvent e) {
    super.processWindowEvent(e);
    this.setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
    if (e.getID() == WindowEvent.WINDOW_CLOSING) {
      jMenuExit(false);
    }
  }

  void jButtonNoneEvent(ActionEvent e) {
    NSPanel selectedPanel = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    selectedPanel.jMenuSelectNone();
  }

  void jButtonAllEvent(ActionEvent e) {
    NSPanel selectedPanel = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    selectedPanel.jMenuSelectAll();
  }

  void jButtonSubmittedEvent(ActionEvent e) {
    NSPanel selectedPanel = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    selectedPanel.jMenuSelectSubmitted();
  }

  void jMenuFileOpenInEditor() {
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
    int choice = fileChooser.showOpenDialog(JobSubmitter.this);
    if (choice != JFileChooser.APPROVE_OPTION) {
      return;
    } else if (!fileChooser.getSelectedFile().isFile()) {
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      JOptionPane.showOptionDialog(JobSubmitter.this, "Unable to find file: "
          + selectedFile, Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return;
    } else {
      GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
          .getCurrentDirectory().toString());
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      NSPanel selectedRB = (NSPanel) jTabbedPaneRB.getSelectedComponent();
      String rBName = jTabbedPaneRB
          .getTitleAt(jTabbedPaneRB.getSelectedIndex());
      String tempName = GUIFileSystem.getNameNoExtension(selectedFile);
      String keyJobName = selectedRB.getAvailableJobName(tempName);
      JFrame editor;
      try {
        editor = new JDLEditor(rBName, keyJobName, selectedFile,
            JobSubmitter.this);
      } catch (Exception ex) {
        JOptionPane.showOptionDialog(JobSubmitter.this, ex.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        return;
      }
      GUIGlobalVars.openedEditorHashMap
          .put(rBName + " - " + keyJobName, editor);
      GraphicUtils.screenCenterWindow(editor);
      editor.setVisible(true);
    }
  }

  void jMenuFileNew(ActionEvent ae) {
    NSPanel selectedRB = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    String rBName = jTabbedPaneRB.getTitleAt(jTabbedPaneRB.getSelectedIndex());
    String keyJobName = GUIFileSystem.DEFAULT_JDL_EDITOR_SAVE_FILE_NAME
        + selectedRB.getProgressiveJobNumber();
    JFrame editor = new JDLEditor(rBName, keyJobName, JobSubmitter.this);
    GUIGlobalVars.openedEditorHashMap.put(rBName + " - " + keyJobName, editor);
    GraphicUtils.screenCenterWindow(editor);
    editor.setVisible(true);
  }

  void jMenuFileNewDag(ActionEvent ae) {
    NSPanel selectedRB = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    String rBName = jTabbedPaneRB.getTitleAt(jTabbedPaneRB.getSelectedIndex());
    String keyJobName = GUIFileSystem.DEFAULT_JDL_EDITOR_SAVE_FILE_NAME
        + selectedRB.getProgressiveJobNumber();
    JFrame editor = new JDLEditor(rBName, keyJobName, JobSubmitter.this, true);
    GUIGlobalVars.openedEditorHashMap.put(rBName + " - " + keyJobName, editor);
    GraphicUtils.screenCenterWindow(editor);
    editor.setVisible(true);
  }

  void jMenuSaveSelectedJobIdList(ActionEvent ae) {
    NSPanel selectedNS = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    selectedNS.saveSelectedJobIdList();
  }

  void jMenuSaveJobIdList(ActionEvent ae) {
    NSPanel selectedNS = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    selectedNS.saveJobIdList();
  }

  void jMenuExit(boolean removeCtx) {
    int choice = JOptionPane.showOptionDialog(JobSubmitter.this,
        "Do you really want to exit?", "Job Submitter - Confirm Exit",
        JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null,
        null);
    if (choice == 0) {
      if (removeCtx) {
        try {
          GUIFileSystem.removeUserSessionFiles();
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
        }
      }
      if (isJobMonitorActive()) {
        GraphicUtils.closeAllEditorFrames();
        ListmatchFrame.closeAllListmatchFrames();
        jobMonitorJFrame.setMenuCredentialChangeVOItemEnabled(true);
        this.dispose();
      } else {
        try {
          GUIFileSystem.removeUserTempFileDirectory(true);
        } catch (Exception ex) {
          if (isDebugging) {
            ex.printStackTrace();
          }
        }
        System.exit(0);
      }
    }
  }

  void jMenuJobAddFile() {
    JFileChooser fileChooser = new JFileChooser();
    fileChooser.setCurrentDirectory(new File(GUIGlobalVars
        .getFileChooserWorkingDirectory()));
    fileChooser.setApproveButtonText("Add");
    fileChooser.setApproveButtonToolTipText("Add selected file");
    fileChooser.setDialogTitle("Add");
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
    fileChooser.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
    int choice = fileChooser.showOpenDialog(JobSubmitter.this);
    if (choice != JFileChooser.APPROVE_OPTION) {
      return;
    } else if (!fileChooser.getSelectedFile().isFile()
        && !fileChooser.getSelectedFile().isDirectory()) {
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      JOptionPane.showOptionDialog(JobSubmitter.this, "Unable to find file: "
          + selectedFile, Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return;
    } else {
      GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
          .getCurrentDirectory().toString());
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      File file = new File(selectedFile);
      String errorMsg = "";
      if (file.isDirectory()) {
        File files[] = file.listFiles();
        String fileExtension = "";
        for (int i = 0; i < files.length; i++) {
          fileExtension = GUIFileSystem.getFileExtension(files[i])
              .toUpperCase();
          if (files[i].isFile()
              && (fileExtension.equals("JDL") || fileExtension.equals("XML"))) {
            // Add files only, not directories.
            if (!addFileToJobTable(files[i].toString())) {
              errorMsg += "- " + files[i] + "\n";
            }
          }
        }
      } else {
        if (!addFileToJobTable(selectedFile)) {
          errorMsg += "- " + selectedFile;
        }
      }
      errorMsg = errorMsg.trim();
      if (!errorMsg.equals("")) {
        GraphicUtils.showOptionDialogMsg(JobSubmitter.this, errorMsg,
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
            "Cannot add file(s):", "The file(s) cannot be submitted"
                + "\n(Use the editor to correct errors before adding)");
      }
    }
  }

  void jMenuCut() {
    NSPanel selectedPanel = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    selectedPanel.jMenuCut();
  }

  void jMenuCopy() {
    NSPanel selectedPanel = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    selectedPanel.jMenuCopy();
  }

  void jMenuCopyTo(ActionEvent e) {
    NSPanel selectedPanel = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    selectedPanel.jMenuCopyTo(e);
  }

  void jMenuMoveTo(ActionEvent e) {
    NSPanel selectedPanel = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    selectedPanel.jMenuMoveTo(e);
  }

  void jButtonEditorEvent(ActionEvent e, boolean openNew) {
    NSPanel selectedPanel = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    int[] selectedRow = selectedPanel.jTableJobs.getSelectedRows();
    int selectedRowCount = selectedRow.length;
    if (selectedRowCount > 1) {
      JOptionPane.showOptionDialog(JobSubmitter.this,
          "To edit from table select a single item", Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
          null, null);
      jMenuFileNew(null);
    } else if (selectedRowCount == 1) {
      String jobIdText = selectedPanel.jTableJobs.getValueAt(selectedRow[0],
          NSPanel.JOB_ID_COLUMN_INDEX).toString().trim();
      if (jobIdText.equals(Utils.SUBMITTING_TEXT)
          || jobIdText.equals(Utils.LISTMATCHING_TEXT)) {
        String msg = (jobIdText.equals(Utils.SUBMITTING_TEXT)) ? "Cannot edit, the job is in Submitting phase"
            : "Cannot edit, the job is in Searching CE phase";
        JOptionPane.showOptionDialog(JobSubmitter.this, msg
            + ((openNew) ? "\nA new JDL Editor will be opened" : ""),
            Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.INFORMATION_MESSAGE, null, null, null);
        if (openNew) {
          jMenuFileNew(null);
        }
        return;
      }
      String keyJobName = selectedPanel.jTableJobs.getValueAt(selectedRow[0],
          NSPanel.JOB_NAME_COLUMN_INDEX).toString();
      NSPanel selectedRB = (NSPanel) jTabbedPaneRB.getSelectedComponent();
      String rBName = jTabbedPaneRB
          .getTitleAt(jTabbedPaneRB.getSelectedIndex());
      String temporaryPhysicalFileName = GUIFileSystem
          .getJobTemporaryFileDirectory()
          + rBName
          + File.separator
          + keyJobName
          + GUIFileSystem.JDL_FILE_EXTENSION;
      JFrame editor = null;
      if (!GUIGlobalVars.openedEditorHashMap.containsKey(rBName + " - "
          + keyJobName)) {
        String jdleSchema = ((NetworkServer) GUIGlobalVars.nsMap.get(rBName))
            .getJDLESchema();
        Utils.setJDLESchemaFile(jdleSchema);
        try {
          editor = new JDLEditor(rBName, keyJobName, temporaryPhysicalFileName,
              JobSubmitter.this);
        } catch (Exception ex) {
          ex.printStackTrace();
          JOptionPane.showOptionDialog(JobSubmitter.this, ex.getMessage(),
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
          return;
        }
        GUIGlobalVars.openedEditorHashMap.put(rBName + " - " + keyJobName,
            editor);
        GraphicUtils.screenCenterWindow(editor);
        editor.setVisible(true);
      } else {
        JOptionPane.showOptionDialog(JobSubmitter.this,
            "A JDL Editor for the job '" + keyJobName + "'  is already opened",
            Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.INFORMATION_MESSAGE, null, null, null);
        editor = (JDLEditor) GUIGlobalVars.openedEditorHashMap.get(rBName
            + " - " + keyJobName);
        editor.setVisible(false);
        GraphicUtils.deiconifyFrame(editor);
        editor.setVisible(true);
      }
    } else {
      jMenuFileNew(null);
    }
  }

  void jButtonMonitorEvent(ActionEvent e) {
    NSPanel selectedPanel = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    JobTableModel jobTableModel = ((JobTableModel) selectedPanel.jTableJobs
        .getModel());
    int[] selectedRow = selectedPanel.jTableJobs.getSelectedRows();
    int selectedRowCount = selectedRow.length;
    if (isJobMonitorActive()) { // Monitor is shown.
      jobMonitorJFrame.setVisible(false);
      GraphicUtils.deiconifyFrame(jobMonitorJFrame);
      jobMonitorJFrame.setVisible(true);
      jobMonitorJFrame.setMenuCredentialChangeVOItemEnabled(false);
    } else { // Monitor is null or disposed.
      jobMonitorJFrame = new JobMonitor(JobSubmitter.this);
      GraphicUtils.screenCenterWindow(jobMonitorJFrame);
      jobMonitorJFrame.setVisible(true);
      jobMonitorJFrame.setMenuCredentialChangeVOItemEnabled(false);
    }
    if (selectedRowCount != 0) {
      String informationMsg = "";
      Vector newElementVector = new Vector();
      String warningMsg = "";
      String jobName;
      for (int i = 0; i < selectedRowCount; i++) {
        String jobIdValue = selectedPanel.jTableJobs.getValueAt(selectedRow[i],
            NSPanel.JOB_ID_COLUMN_INDEX).toString().trim();
        if (isSubmittedJob(jobIdValue)) {
          newElementVector.add(jobTableModel.getValueAt(selectedRow[i],
              NSPanel.JOB_ID_COLUMN_INDEX));
        } else {
          jobName = jobTableModel.getValueAt(selectedRow[i],
              NSPanel.JOB_NAME_COLUMN_INDEX).toString().trim();
          warningMsg += "- " + jobName + "\n";
        }
      }
      jobMonitorJFrame.addSubmittedJobs(newElementVector);
      if (!jobMonitorJFrame.isJobPresentOnStartup
          || ((jobMonitorJFrame.multipleJobPanel != null) && jobMonitorJFrame.multipleJobPanel
              .isVisible())) {
        jobMonitorJFrame.isJobPresentOnStartup = false;
        jobMonitorJFrame.showMultipleJobPanel(newElementVector);
      }
      warningMsg = warningMsg.trim();
      if (!warningMsg.equals("")) {
        GraphicUtils.showOptionDialogMsg(jobMonitorJFrame, warningMsg,
            Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
            "Cannot monitor job(s):",
            "Not submitted job(s) or submission is on-going");
      }
    }
  }

  void jButtonSubmitEvent(ActionEvent ae) {
    final SwingWorker worker = new SwingWorker() {
      public Object construct() {
        // Standard code
        NSPanel selectedPanel = (NSPanel) jTabbedPaneRB.getSelectedComponent();
        String rBName = selectedPanel.getName();
        JobTableModel jobTableModel = ((JobTableModel) selectedPanel.jTableJobs
            .getModel());
        // Creating Network Server URL.
        Url nsUrl = null;
        String nsUrlText = "";
        try {
          nsUrlText = ((NetworkServer) GUIGlobalVars.nsMap.get(rBName))
              .getAddress();
          nsUrl = new Url(nsUrlText);
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
          JOptionPane.showOptionDialog(JobSubmitter.this, e.getMessage(),
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
          return "";
        }
        logger.info("Starting Submissioning Method ");
        logger.info("Network Server: " + nsUrlText);
        // Creating Logging & Bookkeeping Server URLs.
        Vector lbUrlVector = new Vector();
        Vector lbVector = (Vector) GUIGlobalVars.nsLBMap.get(nsUrlText);
        logger.info("Logging & Bookkeeping Server Vector: " + lbVector);
        if (lbVector == null) {
          // Unexpected Error
          return "";
        }
        try {
          for (int i = 0; i < lbVector.size(); i++) {
            lbUrlVector.add(new Url(lbVector.get(i).toString().trim()));
          }
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
          JOptionPane.showOptionDialog(JobSubmitter.this, e.getMessage(),
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
          return "";
        }
        int[] selectedRows = selectedPanel.jTableJobs.getSelectedRows();
        int selectedRowCount = selectedRows.length;
        if (selectedRowCount != 0) {
          String informationMsg = "";
          Vector selectedJobNameVector = new Vector();
          for (int i = 0; i < selectedRowCount; i++) {
            selectedJobNameVector.add(selectedPanel.jTableJobs.getValueAt(
                selectedRows[i], NSPanel.JOB_NAME_COLUMN_INDEX).toString());
          }
          Date timeText = new Date();
          String jobId;
          String jobIdValue;
          String jobName;
          String jobType;
          String temporaryPhysicalFileName;
          JobAd jobAd = new JobAd();
          Ad ad = new Ad();
          // Creating job collection.
          JobCollection jobCollection = new JobCollection();
          for (int i = 0; i < selectedRowCount; i++) {
            jobIdValue = selectedPanel.jTableJobs.getValueAt(selectedRows[i],
                NSPanel.JOB_ID_COLUMN_INDEX).toString().trim();
            //logger.debug("jobId: " + jobIdValue);
            jobName = selectedPanel.jTableJobs.getValueAt(selectedRows[i],
                NSPanel.JOB_NAME_COLUMN_INDEX).toString().trim();
            if (jobIdValue.equals(Utils.NOT_SUBMITTED_TEXT)) {
              jobType = selectedPanel.jTableJobs.getValueAt(selectedRows[i],
                  NSPanel.JOB_TYPE_COLUMN_INDEX).toString().trim();
              if (GUIGlobalVars.openedEditorHashMap.containsKey(selectedPanel
                  .getName()
                  + " - " + jobName)) {
                int choice = JOptionPane
                    .showOptionDialog(
                        JobSubmitter.this,
                        "A JDL Editor is opened for the job '"
                            + jobName
                            + "'"
                            + "\nDo you want to submit this job anyway?"
                            + "\n(Last modifies will be lost, JDL Editor will be closed)",
                        Utils.WARNING_MSG_TXT, JOptionPane.YES_NO_OPTION,
                        JOptionPane.WARNING_MESSAGE, null, null, null);
                if (choice != 0) {
                  selectedJobNameVector.remove(jobName);
                  continue;
                } else {
                  JFrame editor = (JFrame) GUIGlobalVars.openedEditorHashMap
                      .get(selectedPanel.getName() + " - " + jobName);
                  GUIGlobalVars.openedEditorHashMap.remove(rBName + " - "
                      + jobName);
                  editor.dispose();
                }
              }
              if (GUIGlobalVars.openedListmatchMap.containsKey(selectedPanel
                  .getName()
                  + " - " + jobName)) {
                int choice = JOptionPane
                    .showOptionDialog(
                        JobSubmitter.this,
                        "A Listmatch window is shown for '"
                            + jobName
                            + "' job"
                            + "\nNone of the proposed CEs has been selected for the job"
                            + "\nDo you want to submit this job anyway?"
                            + "\n(Listmatch window will be closed)",
                        Utils.WARNING_MSG_TXT, JOptionPane.YES_NO_OPTION,
                        JOptionPane.WARNING_MESSAGE, null, null, null);
                if (choice != 0) {
                  selectedJobNameVector.remove(jobName);
                  continue;
                } else {
                  ListmatchFrame listmatch = (ListmatchFrame) GUIGlobalVars.openedListmatchMap
                      .get(selectedPanel.getName() + " - " + jobName);
                  GUIGlobalVars.openedListmatchMap.remove(rBName + " - "
                      + jobName);
                  listmatch.dispose();
                }
              }
              try {
                temporaryPhysicalFileName = GUIFileSystem
                    .getJobTemporaryFileDirectory()
                    + rBName
                    + File.separator
                    + jobName
                    + GUIFileSystem.JDL_FILE_EXTENSION;
                if (!jobType.equals(Jdl.TYPE_DAG)) {
                  logger.debug("temporaryPhysicalFileName: "
                      + temporaryPhysicalFileName);
                  jobAd.fromFile(temporaryPhysicalFileName);
                  if (jobType.indexOf(Jdl.JOBTYPE_CHECKPOINTABLE) != -1) {
                    String checkpointStateFile = "";
                    if (jobAd.hasAttribute(Utils.GUI_CHECKPOINT_STATE_FILE)) {
                      checkpointStateFile = jobAd.getStringValue(
                          Utils.GUI_CHECKPOINT_STATE_FILE).get(0).toString();
                    }
                    logger.info("The job is checkpointable - checkpoint file: "
                        + checkpointStateFile);
                    if (!checkpointStateFile.equals("")) {
                      if ((new File(checkpointStateFile)).isFile()) {
                        JobState jobStateAd = new JobState();
                        jobStateAd.fromFile(checkpointStateFile);
                        jobStateAd.check();
                        jobAd.setAttribute(Jdl.CHKPT_JOBSTATE, jobStateAd);
                        jobAd.delAttribute(Utils.GUI_CHECKPOINT_STATE_FILE);
                      } else {
                        if (selectedJobNameVector.contains(jobName)) {
                          selectedJobNameVector.remove(jobName);
                        }
                        JOptionPane.showOptionDialog(JobSubmitter.this,
                            "Unable to find linked file: "
                                + checkpointStateFile + "\nCannot submit job: "
                                + jobName, Utils.ERROR_MSG_TXT,
                            JOptionPane.DEFAULT_OPTION,
                            JOptionPane.ERROR_MESSAGE, null, null, null);
                        continue;
                      }
                    }
                  }
                  // Inserting job into jobCollection.
                  jobCollection.insertAd(jobAd);
                } else {
                  Job job = new Job(temporaryPhysicalFileName);
                  logger.debug("jobCollection.insert(job) - job: "
                      + job.toString());
                  jobCollection.insert(job);
                }
                jobTableModel.setValueAt(Utils.SUBMITTING_TEXT,
                    selectedRows[i], NSPanel.JOB_ID_COLUMN_INDEX);
              } catch (JobCollectionException jce) {
                if (selectedJobNameVector.contains(jobName)) {
                  selectedJobNameVector.remove(jobName);
                }
                if (isDebugging) {
                  jce.printStackTrace();
                }
              } catch (Exception e) {
                if (selectedJobNameVector.contains(jobName)) {
                  selectedJobNameVector.remove(jobName);
                }
                if (isDebugging) {
                  e.printStackTrace();
                }
              }
            } else if (jobIdValue.equals(Utils.LISTMATCHING_TEXT)) {
              selectedJobNameVector.remove(jobName);
              informationMsg += "- The job '" + jobName
                  + "' is in Searching CE phase\n";
            } else if (jobIdValue.equals(Utils.SUBMITTING_TEXT)) {
              selectedJobNameVector.remove(jobName);
              informationMsg += "- The job '" + jobName
                  + "' is already in Submitting phase\n";
            } else {
              selectedJobNameVector.remove(jobName);
              informationMsg += "- The job '" + jobName
                  + "' is already submitted\n";
            }
          }
          if (jobCollection.size() != 0) {
            logger.debug("Job Name(s) to Submit: " + selectedJobNameVector);
            Vector resultVector = new Vector();
            try {
              jobCollection.setLoggerLevel(GUIGlobalVars
                  .getGUIConfVarNSLoggerLevel());
              UserCredential userCredential = new UserCredential(new File(
                  GUIGlobalVars.proxyFilePath));
              if (userCredential.getX500UserSubject().equals(
                  GUIGlobalVars.proxySubject)) {
                logger.info("Submitting Job Collection...\n\t"
                    + " Job Collection Size: " + jobCollection.size()
                    + "\n\t submit(" + nsUrl + ", " + lbUrlVector + ", null)");
                //long startTime = (new Date()).getTime();
                resultVector = jobCollection.submit(nsUrl, lbUrlVector, null);
                //long endTime = (new Date()).getTime();
                //logger.debug("SUBMISSION DONE - elapsed time: " +
                //                 (endTime - startTime));
              } else {
                JOptionPane.showOptionDialog(JobSubmitter.this,
                    Utils.FATAL_ERROR + "Proxy file user subject has changed"
                        + "\nApplication will be terminated",
                    Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                    JOptionPane.ERROR_MESSAGE, null, null, null);
                System.exit(-1);
              }
            } catch (Exception e) {
              if (isDebugging) {
                e.printStackTrace();
              }
              int index = -1;
              for (int i = 0; i < selectedJobNameVector.size(); i++) {
                jobName = selectedJobNameVector.get(i).toString();
                index = selectedPanel.jobTableModel.getIndexOfElementInColumn(
                    selectedJobNameVector.get(i).toString(),
                    NSPanel.JOB_NAME_COLUMN_INDEX);
                if (index == -1) {
                  continue;
                }
                jobTableModel.setValueAt(Utils.NOT_SUBMITTED_TEXT, index,
                    NSPanel.JOB_ID_COLUMN_INDEX);
              }
              JOptionPane.showOptionDialog(JobSubmitter.this, e.getMessage(),
                  Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                  JOptionPane.ERROR_MESSAGE, null, null, null);
            }
            int index = -1;
            int statusCode;
            String jobIdText;
            String type = "";
            Result result;
            logger.debug("resultVector.size(): " + resultVector.size()
                + " selectedJobNameVector.size(): "
                + selectedJobNameVector.size());
            for (int i = 0; i < resultVector.size(); i++) {
              result = (Result) resultVector.get(i);
              if (result != null) {
                statusCode = result.getCode();
              } else {
                statusCode = Result.SUBMIT_FAILURE;
              }
              jobName = selectedJobNameVector.get(i).toString();
              index = selectedPanel.jobTableModel.getIndexOfElementInColumn(
                  selectedJobNameVector.get(i).toString(),
                  NSPanel.JOB_NAME_COLUMN_INDEX);
              if (index == -1) {
                continue;
              }
              logger.debug("statusCode: " + statusCode);
              if ((statusCode != Result.SUBMIT_FAILURE)
                  && (statusCode != Result.SUBMIT_FORBIDDEN)) {
                jobIdText = result.getId();
                selectedPanel.jTableJobs.setValueAt(jobIdText, index,
                    NSPanel.JOB_ID_COLUMN_INDEX);
                selectedPanel.jTableJobs.setValueAt(timeText, index,
                    NSPanel.JOB_SUBMIT_TIME_COLUMN_INDEX);
                temporaryPhysicalFileName = GUIFileSystem
                    .getJobTemporaryFileDirectory()
                    + rBName
                    + File.separator
                    + jobName
                    + GUIFileSystem.JDL_FILE_EXTENSION;
                try {
                  ad.fromFile(temporaryPhysicalFileName);
                  if (ad.hasAttribute(Jdl.TYPE)) {
                    type = ad.getStringValue(Jdl.TYPE).get(0).toString();
                  }
                  if (!type.equals(Jdl.TYPE_DAG)) {
                    jobAd.fromFile(temporaryPhysicalFileName);
                    if (jobAd.hasAttribute(Utils.GUI_JOB_ID_ATTR_NAME)) {
                      jobAd.delAttribute(Utils.GUI_JOB_ID_ATTR_NAME);
                      if (jobAd
                          .hasAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME)) {
                        jobAd.delAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME);
                      }
                    }
                    jobAd.addAttribute(Utils.GUI_JOB_ID_ATTR_NAME, jobIdText);
                    jobAd.addAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME,
                        (new Long(timeText.getTime())).toString()); //!!! timeText or something from result?
                    GUIFileSystem.saveTextFile(temporaryPhysicalFileName, jobAd
                        .toLines());
                  } else { // Dag.
                    if (ad.hasAttribute(Utils.GUI_JOB_ID_ATTR_NAME)) {
                      ad.delAttribute(Utils.GUI_JOB_ID_ATTR_NAME);
                      if (ad.hasAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME)) {
                        ad.delAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME);
                      }
                    }
                    ad.addAttribute(Utils.GUI_JOB_ID_ATTR_NAME, jobIdText);
                    ad.addAttribute(Utils.GUI_SUBMISSION_TIME_ATTR_NAME,
                        (new Long(timeText.getTime())).toString()); //!!! timeText or something from result?
                    GUIFileSystem.saveTextFile(temporaryPhysicalFileName, ad
                        .toString());
                  }
                } catch (java.lang.Exception e) {
                  if (isDebugging) {
                    e.printStackTrace();
                  }
                  JOptionPane.showOptionDialog(JobSubmitter.this, e
                      .getMessage(), Utils.ERROR_MSG_TXT,
                      JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE,
                      null, null, null);
                  continue;
                }
              } else {
                String submissionErrorMsg = "";
                if (result != null) {
                  submissionErrorMsg = ((Exception) result.getResult())
                      .getMessage();
                } else {
                  submissionErrorMsg = "Unable to submit job";
                }
                jobTableModel.setValueAt(Utils.NOT_SUBMITTED_TEXT, index,
                    NSPanel.JOB_ID_COLUMN_INDEX);
                jobName = selectedPanel.jTableJobs.getValueAt(index,
                    NSPanel.JOB_NAME_COLUMN_INDEX).toString();
                informationMsg += "- Submission Failure: " + jobName + ": "
                    + submissionErrorMsg + "\n";
              }
            }
          }
          informationMsg = informationMsg.trim();
          if (!informationMsg.equals("")) {
            GraphicUtils.showOptionDialogMsg(JobSubmitter.this, informationMsg,
                Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
                null, null);
          }
        } else {
          JOptionPane.showOptionDialog(JobSubmitter.this, Utils.SELECT_AN_ITEM,
              Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.INFORMATION_MESSAGE, null, null, null);
        }
        // END Standard code
        return "";
      }
    };
    worker.start();
  }

  /**
   * Loads old working session serching for jobs in the temporary file
   * directory. All temporary files will be inserted, as jobs, in the proper
   * Network Server panel.
   */
  protected void loadOldWorkingSession() {
    String directoryPath = GUIFileSystem.getJobTemporaryFileDirectory();
    File directory = new File(directoryPath);
    for (int i = 0; i < jTabbedPaneRB.getTabCount(); i++) {
      ((NSPanel) jTabbedPaneRB.getComponentAt(i)).removeAllJobs();
    }
    if (directory.isDirectory()) {
      int choice = -1;
      if (GUIGlobalVars.restorePreviousSession) {
        choice = 0;
      }
      Vector nsDirectoryVector = new Vector();
      nsDirectoryVector = JobSubmitterPreferences.loadPrefFileNSNames();
      int errorCounter = 0;
      for (int i = 0; i < nsDirectoryVector.size(); i++) {
        File nsDirectory = new File(directory + File.separator
            + nsDirectoryVector.get(i));
        File[] nsFiles = nsDirectory.listFiles();
        if (nsFiles != null) {
          for (int j = 0; j < nsFiles.length; j++) {
            if (nsFiles[j].isFile()) {
              //String fileExtension = Utils.getExtension(nsFiles[j]);
              String fileExtension = GUIFileSystem.getFileExtension(nsFiles[j]);
              if ((fileExtension != null)
                  && ("." + fileExtension)
                      .equals(GUIFileSystem.JDL_FILE_EXTENSION)) {
                if (choice == -1) {
                  choice = JOptionPane.showOptionDialog(JobSubmitter.this,
                      "A previous working session is available. "
                          + "Do you want to load it?",
                      "Job Submitter - Job Submission Session Restoring",
                      JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE,
                      null, null, null);
                }
                if (choice == 0) {
                  NSPanel rbPanel = getRBPanelReference(this.nsNameVector
                      .get(i).toString());
                  int fileLength = GUIFileSystem.getName(nsFiles[j]).length();
                  if (!rbPanel.restoreJobToTable((GUIFileSystem
                      .getName(nsFiles[j])).substring(0, fileLength - 4),
                      nsFiles[j])) {
                    errorCounter++;
                    try {
                      nsFiles[j].delete();
                    } catch (Exception e) {
                      // Do nothing.
                      if (isDebugging) {
                        e.printStackTrace();
                      }
                    }
                  }
                } else {
                  try {
                    GUIFileSystem.removeDirectoryDescendant(nsDirectory);
                  } catch (Exception e) {
                    // Unable to remove old context. Do nothing.
                    if (isDebugging) {
                      e.printStackTrace();
                    }
                  }
                }
              } else {
                try {
                  nsFiles[j].delete();
                } catch (Exception e) {
                  // Do nothing.
                  if (isDebugging) {
                    e.printStackTrace();
                  }
                }
              }
            }
          }
        }
      }
      if (errorCounter != 0) {
        JOptionPane.showOptionDialog(JobSubmitter.this, "Unable to load "
            + errorCounter + " old context job(s)"
            + "\nCorrupted job(s) will be removed", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
      }
    }
    GUIGlobalVars.restorePreviousSession = true;
  }

  /**
   * Adds the selected file (as a job) into the Job Table
   * @param selectedFile the file to add
   * @return true if it succeed;
   *         false otherwise
   */
  protected boolean addFileToJobTable(String selectedFile) {
    //!!! Check it
    ExprChecker exprChecker = new ExprChecker();
    File file = new File(selectedFile);
    String fileExtension = GUIFileSystem.getFileExtension(file).toUpperCase();
    String parserErrorMsg = "";
    JobAd jobAd = null;
    Ad ad = new Ad();
    String type = "";
    //ClassAdSAXParser classAdSAXParser = new ClassAdSAXParser();
    if (fileExtension.equals("JDL")) {
      try {
        ad.fromFile(file.toString());
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        return false;
      }
      try {
        type = ad.getStringValue(Jdl.TYPE).get(0).toString();
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
      if (!type.equals(Jdl.TYPE_DAG)) {
        NSPanel selectedRB = (NSPanel) jTabbedPaneRB.getSelectedComponent();
        String rBName = jTabbedPaneRB.getTitleAt(jTabbedPaneRB
            .getSelectedIndex());
        String jdleSchema = ((NetworkServer) GUIGlobalVars.nsMap.get(rBName))
            .getJDLESchema();
        String defRequirements = "";
        String defRank = "";
        String defRankMPI = "";
        SchemaRankRequirementsDefault schemaRankRequirementsDefault;
        for (int i = 0; i < GUIGlobalVars.srrDefaultVector.size(); i++) {
          schemaRankRequirementsDefault = (SchemaRankRequirementsDefault) GUIGlobalVars.srrDefaultVector
              .get(i);
          logger.info("addFileToJobTable() - schemaRankRequirementsDefault: "
              + schemaRankRequirementsDefault);
          logger.info("addFileToJobTable() - jdleSchema: " + jdleSchema);
          if (schemaRankRequirementsDefault.getSchema().equals(jdleSchema)) {
            defRequirements = schemaRankRequirementsDefault.getRequirements()
                .trim();
            defRank = schemaRankRequirementsDefault.getRank().trim();
            defRankMPI = schemaRankRequirementsDefault.getRankMPI().trim();
          }
        }
        try {
          // Adds virtual organisation to pass API check() method.
          if (!GUIGlobalVars.jobAdAttributeToAdd
              .hasAttribute(Jdl.VIRTUAL_ORGANISATION)) {
            GUIGlobalVars.jobAdAttributeToAdd.addAttribute(
                Jdl.VIRTUAL_ORGANISATION, GUIGlobalVars
                    .getVirtualOrganisation());
          }
          if (GUIGlobalVars.jobAdAttributeToAdd.hasAttribute(Jdl.RANK)) {
            GUIGlobalVars.jobAdAttributeToAdd.delAttribute(Jdl.RANK);
          }
          if ((defRank != null) && !defRank.equals("")) {
            GUIGlobalVars.jobAdAttributeToAdd.setAttributeExpr(Jdl.RANK,
                defRank);
            logger.info("addFileToJobTable() - jobAdAttributeToAdd defRank: "
                + defRank);
          }
          if (GUIGlobalVars.jobAdAttributeToAdd.hasAttribute(Jdl.REQUIREMENTS)) {
            GUIGlobalVars.jobAdAttributeToAdd.delAttribute(Jdl.REQUIREMENTS);
          }
          if ((defRequirements != null) && !defRequirements.equals("")) {
            GUIGlobalVars.jobAdAttributeToAdd.setAttributeExpr(
                Jdl.REQUIREMENTS, defRequirements);
            logger
                .info("addFileToJobTable() - jobAdAttributeToAdd defRequirements: "
                    + defRequirements);
          }
          if (GUIGlobalVars.jobAdAttributeToAdd.hasAttribute(Jdl.RANK_MPI)) {
            GUIGlobalVars.jobAdAttributeToAdd.delAttribute(Jdl.RANK_MPI);
          }
          if ((defRankMPI != null) && !defRankMPI.equals("")) {
            GUIGlobalVars.jobAdAttributeToAdd.setAttributeExpr(Jdl.RANK_MPI,
                defRankMPI);
            logger
                .info("addFileToJobTable() - jobAdAttributeToAdd defRankMPI: "
                    + defRankMPI);
          }
          if (!GUIGlobalVars.jobAdAttributeToAdd.hasAttribute(Jdl.RETRYCOUNT)) {
            int retryCount = GUIGlobalVars.getGUIConfVarRetryCount();
            if (retryCount != GUIGlobalVars.NO_RETRY_COUNT) {
              GUIGlobalVars.jobAdAttributeToAdd.setAttribute(Jdl.RETRYCOUNT,
                  retryCount);
            }
            logger
                .info("addFileToJobTable() - jobAdAttributeToAdd retryCount: "
                    + retryCount);
          }
          if (!GUIGlobalVars.jobAdAttributeToAdd.hasAttribute(Jdl.HLR_LOCATION)) {
            String hlrLocation = GUIGlobalVars.getHLRLocation();
            if (!hlrLocation.equals("")) {
              GUIGlobalVars.jobAdAttributeToAdd.setAttribute(Jdl.HLR_LOCATION,
                  hlrLocation);
            }
            logger
                .info("addFileToJobTable() - jobAdAttributeToAdd hlrLocation: "
                    + hlrLocation);
          }
          if (!GUIGlobalVars.jobAdAttributeToAdd.hasAttribute(Jdl.MYPROXY)) {
            String myProxyServer = GUIGlobalVars.getMyProxyServer();
            if (!myProxyServer.equals("")) {
              GUIGlobalVars.jobAdAttributeToAdd.setAttribute(Jdl.MYPROXY,
                  myProxyServer);
            }
            logger
                .info("addFileToJobTable() - jobAdAttributeToAdd myProxyServer: "
                    + myProxyServer);
          }
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
          return false;
        }
        try {
          logger
              .info("addFileToJobTable() - GUIGlobalVars.jobAdAttributeToAdd: "
                  + GUIGlobalVars.jobAdAttributeToAdd);
          jobAd = exprChecker.parse(file, true,
              GUIGlobalVars.jobAdAttributeToAdd);
          try {
            // Forces the value of VirtualOrganisation attribute to be the one
            // selected from user at the beginning (Credentials panel).
            if (jobAd.hasAttribute(Jdl.VIRTUAL_ORGANISATION)) {
              jobAd.delAttribute(Jdl.VIRTUAL_ORGANISATION);
            }
            jobAd.addAttribute(Jdl.VIRTUAL_ORGANISATION, GUIGlobalVars
                .getVirtualOrganisation());
          } catch (Exception e) {
            if (isDebugging) {
              e.printStackTrace();
            }
            return false;
          }
          parserErrorMsg = exprChecker.getErrorMsg().trim();
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
          return false;
        }
      } else {
        NSPanel selectedRB = (NSPanel) jTabbedPaneRB.getSelectedComponent();
        String rBName = jTabbedPaneRB.getTitleAt(jTabbedPaneRB
            .getSelectedIndex());
        String tempName = GUIFileSystem.getNameNoExtension(selectedFile);
        String keyJobName = selectedRB.getAvailableJobName(tempName);
        try {
          if (ad.hasAttribute(Jdl.VIRTUAL_ORGANISATION)) {
            ad.delAttribute(Jdl.VIRTUAL_ORGANISATION);
          }
          ad.addAttribute(Jdl.VIRTUAL_ORGANISATION, GUIGlobalVars
              .getVirtualOrganisation());
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
          return false;
        }
        selectedRB.addDagToTable(rBName, keyJobName, ad);
      }
    } else if (fileExtension.equals("XML")) {
      try {
        jobAd = exprChecker.parse(file, true, ExprChecker.XML);
        parserErrorMsg = exprChecker.getErrorMsg();
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
    }
    parserErrorMsg = parserErrorMsg.trim();
    if (!type.equals(Jdl.TYPE_DAG)) {
      if (parserErrorMsg.equals("") && (jobAd != null) && (jobAd.size() != 0)) {
        NSPanel selectedRB = (NSPanel) jTabbedPaneRB.getSelectedComponent();
        String rBName = jTabbedPaneRB.getTitleAt(jTabbedPaneRB
            .getSelectedIndex());
        String tempName = GUIFileSystem.getNameNoExtension(selectedFile);
        String keyJobName = selectedRB.getAvailableJobName(tempName);
        selectedRB.addJobToTable(rBName, keyJobName, jobAd);
      } else {
        return false;
      }
    }
    return true;
  }

  /**
   * Sends all the jobs contained in <code>jobVector</code> to the Job Monitor
   * in order to add them inside the Monitor Job Table
   * @param jobVector the vector containing the jobs
   */
  protected void addSubmittedJobsToMonitor(Vector jobVector) {
    if (isJobMonitorActive()) {
      jobMonitorJFrame.setVisible(false);
      GraphicUtils.deiconifyFrame(jobMonitorJFrame);
      jobMonitorJFrame.setVisible(true);
      jobMonitorJFrame.setMenuCredentialChangeVOItemEnabled(false);
    } else {
      jobMonitorJFrame = new JobMonitor(JobSubmitter.this);
      GraphicUtils.screenCenterWindow(jobMonitorJFrame);
      jobMonitorJFrame.setVisible(true);
      jobMonitorJFrame.setMenuCredentialChangeVOItemEnabled(false);
    }
    jobMonitorJFrame.addSubmittedJobs(jobVector);
    if (!jobMonitorJFrame.isJobPresentOnStartup
        || ((jobMonitorJFrame.multipleJobPanel != null) && jobMonitorJFrame.multipleJobPanel
            .isVisible())) {
      jobMonitorJFrame.isJobPresentOnStartup = false;
      jobMonitorJFrame.showMultipleJobPanel(jobVector);
    }
  }

  public void addJobToTable(String rBName, String keyJobName,
      String currentOpenedFile, Ad jobAd) {
    NSPanel targetNSPanel = (NSPanel) jTabbedPaneRB
        .getComponentAt(jTabbedPaneRB.indexOfTab(rBName));
    targetNSPanel.addJobToTable(rBName, keyJobName, jobAd);
  }

  NSPanel getRBPanelReference(String rbName) {
    return (NSPanel) jTabbedPaneRB.getComponentAt(jTabbedPaneRB
        .indexOfTab(rbName));
  }

  void jMenuPaste() {
    NSPanel selectedPanel = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    selectedPanel.jMenuPaste();
  }

  void jMenuSelectAll() {
    jButtonAllEvent(null);
  }

  void jMenuInvertSelection() {
    NSPanel selectedPanel = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    selectedPanel.jMenuInvertSelection();
  }

  void setJMenuEditItemsEnabled() {
    NSPanel selectedPanel = (NSPanel) jTabbedPaneRB.getSelectedComponent();
    if (selectedPanel != null) {
      int selectedRowCount = selectedPanel.jTableJobs.getSelectedRowCount();
      if (selectedPanel.jTableJobs.getRowCount() == 0) {
        jMenuItemSelectAll.setEnabled(false);
        jMenuItemSelectNone.setEnabled(false);
        jMenuItemSelectSubmitted.setEnabled(false);
        jMenuItemInvertSelection.setEnabled(false);
      } else {
        jMenuItemSelectAll.setEnabled(true);
        jMenuItemSelectNone.setEnabled(true);
        jMenuItemSelectSubmitted.setEnabled(true);
        jMenuItemInvertSelection.setEnabled(true);
      }
      if (GUIGlobalVars.selectedJobNameCopyVector.size() == 0) {
        jMenuItemPaste.setEnabled(false);
      } else {
        jMenuItemPaste.setEnabled(true);
      }
      if (selectedRowCount == 0) {
        jMenuItemCut.setEnabled(false);
        jMenuItemCopy.setEnabled(false);
        jMenuCopyTo.setEnabled(false);
        jMenuMoveTo.setEnabled(false);
      } else {
        jMenuItemCut.setEnabled(true);
        jMenuItemCopy.setEnabled(true);
        jMenuCopyTo.setEnabled(true);
        jMenuMoveTo.setEnabled(true);
      }
      if (this.nsNameVector.size() == 1) {
        jMenuMoveTo.setEnabled(false);
      } else if ((this.nsNameVector.size() > 1) && (selectedRowCount > 0)) {
        jMenuMoveTo.setEnabled(true);
      }
    }
  }

  void jMenuHelpAbout() {
    URL url = JobDef1Panel.class.getResource(Utils.ICON_DATAGRID_LOGO);
    JOptionPane.showOptionDialog(JobSubmitter.this,
    //"Job Submitter" + Utils.JOB_SUBMITTER_ABOUT_MSG,
        "Job Submitter Version " + GUIGlobalVars.getGUIConfVarVersion()
            + "\n\n" + Utils.COPYRIGHT, "Job Submitter - About",
        JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE,
        (url == null) ? null : new ImageIcon(url), null, null);
  }

  void jMenuJobPreferences() {
    jobSubmitterPreferences = new JobSubmitterPreferences(this);
    jobSubmitterPreferences.setModal(true);
    GraphicUtils.windowCenterWindow(this, jobSubmitterPreferences);
    jobSubmitterPreferences.show();
  }

  void jMenuJobPreferences(boolean isCancelButtonVisible) {
    jobSubmitterPreferences = new JobSubmitterPreferences(this);
    jobSubmitterPreferences.setModal(true);
    GraphicUtils.windowCenterWindow(this, jobSubmitterPreferences);
    jobSubmitterPreferences.setCancelButtonVisible(isCancelButtonVisible);
    jobSubmitterPreferences.show();
  }

  void jMenuRetrieveCheckpointState(final String jobIdText,
      final boolean isEditable) {
    final SwingWorker worker = new SwingWorker() {
      public Object construct() {
        // Standard code
        RetrieveCheckpointStateDialog retrieveCheckpointState = new RetrieveCheckpointStateDialog(
            JobSubmitter.this, jobIdText, isEditable);
        retrieveCheckpointState.setModal(true);
        GraphicUtils.windowCenterWindow(JobSubmitter.this,
            retrieveCheckpointState);
        retrieveCheckpointState.show();
        String jobId = retrieveCheckpointState.getJobId();
        int state = retrieveCheckpointState.getState();
        logger.info("jobId: " + jobId + " state: " + Integer.toString(state));
        if (jobId != null) {
          jobId = jobId.trim();
          if (!jobId.equals("")) {
            Job job = new Job();
            JobState jobState = new JobState();
            try {
              job = new Job(new JobId(jobId));
            } catch (IllegalArgumentException iae) {
              JOptionPane.showOptionDialog(JobSubmitter.this, iae.getMessage(),
                  Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                  JOptionPane.ERROR_MESSAGE, null, null, null);
              if (isDebugging) {
                iae.printStackTrace();
              }
              return "";
            } catch (Exception e) {
              JOptionPane.showOptionDialog(JobSubmitter.this, e.getMessage(),
                  Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                  JOptionPane.ERROR_MESSAGE, null, null, null);
              if (isDebugging) {
                e.printStackTrace();
              }
            }
            try {
              UserCredential userCredential = new UserCredential(new File(
                  GUIGlobalVars.proxyFilePath));
              if (userCredential.getX500UserSubject().equals(
                  GUIGlobalVars.proxySubject)) {
                jobState = job.getState(state);
              } else {
                JOptionPane.showOptionDialog(JobSubmitter.this,
                    Utils.FATAL_ERROR + "Proxy file user subject has changed"
                        + "\nApplication will be terminated",
                    Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                    JOptionPane.ERROR_MESSAGE, null, null, null);
                System.exit(-1);
              }
              if (jobState != null) {
                JFileChooser fileChooser = new JFileChooser();
                fileChooser.setDialogTitle("Save Checkpoint State - " + jobId);
                fileChooser.setCurrentDirectory(new File(GUIGlobalVars
                    .getFileChooserWorkingDirectory()));
                String[] extensions = { "CHKPT"
                };
                GUIFileFilter classadFileFilter = new GUIFileFilter("chkpt",
                    extensions);
                fileChooser.addChoosableFileFilter(classadFileFilter);
                int choice = fileChooser.showSaveDialog(JobSubmitter.this);
                if (choice != JFileChooser.APPROVE_OPTION) {
                  return "";
                } else {
                  GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
                      .getCurrentDirectory().toString());
                  File outputFile = fileChooser.getSelectedFile();
                  String selectedFile = outputFile.toString();
                  String extension = GUIFileSystem.getFileExtension(outputFile)
                      .toUpperCase();
                  FileFilter selectedFileFilter = fileChooser.getFileFilter();
                  if (!extension.equals("CHKPT")
                      && selectedFileFilter.getDescription().equals("chkpt")) {
                    selectedFile += ".chkpt";
                  }
                  outputFile = new File(selectedFile);
                  choice = 0;
                  if (outputFile.isFile()) {
                    choice = JOptionPane.showOptionDialog(JobSubmitter.this,
                        "Output file exists. Overwrite?",
                        "Job Submitter - Confirm Save",
                        JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE,
                        null, null, null);
                  }
                  if (choice == 0) {
                    GUIFileSystem.saveTextFile(selectedFile, jobState.toString(
                        true, true));
                  }
                }
              }
            } catch (Exception e) {
              if (isDebugging) {
                e.printStackTrace();
              }
              JOptionPane.showOptionDialog(JobSubmitter.this, e.getMessage(),
                  Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                  JOptionPane.ERROR_MESSAGE, null, null, null);
            }
          }
        }
        // END Standard code
        return "";
      }
    };
    worker.start();
  }

  void jMenuOpenCheckPointState() {
    JFileChooser fileChooser = new JFileChooser();
    fileChooser.setCurrentDirectory(new File(GUIGlobalVars
        .getFileChooserWorkingDirectory()));
    fileChooser.setDialogTitle("Open Checkpoint State");
    String[] extensions = { "CHKPT"
    };
    GUIFileFilter classadFileFilter = new GUIFileFilter("*.chkpt", extensions);
    fileChooser.addChoosableFileFilter(classadFileFilter);
    int choice = fileChooser.showOpenDialog(JobSubmitter.this);
    if (choice != JFileChooser.APPROVE_OPTION) {
      return;
    } else if (!fileChooser.getSelectedFile().isFile()) {
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      JOptionPane.showOptionDialog(JobSubmitter.this, "Unable to find file: "
          + selectedFile, Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return;
    } else {
      GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
          .getCurrentDirectory().toString());
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      JobState jobState = new JobState();
      try {
        jobState.fromFile(selectedFile);
        jobState.check();
        if (GUIGlobalVars.openedCheckpointStateMap.containsKey(selectedFile)) {
          CheckpointStateFrame checkpointState = (CheckpointStateFrame) GUIGlobalVars.openedCheckpointStateMap
              .get(selectedFile);
          checkpointState.setVisible(false);
          GraphicUtils.deiconifyFrame(checkpointState);
          checkpointState.setVisible(true);
        } else {
          CheckpointStateFrame checkpointState = new CheckpointStateFrame(this,
              jobState.toString(true, true), selectedFile);
          GUIGlobalVars.openedCheckpointStateMap.put(selectedFile,
              checkpointState);
          GraphicUtils.screenCenterWindow(checkpointState);
          checkpointState.setVisible(true);
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        GraphicUtils.showOptionDialogMsg(JobSubmitter.this, e.getMessage(),
            "Job Submitter - Open Checkpoint State",
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE,
            Utils.MESSAGE_LINES_PER_JOPTIONPANE, null, null);
      }
    }
  }

  protected void addNSMenuItem(String item) {
    this.menuNSVector.add(item);
    JMenuItem jMenuItem = new JMenuItem(item);
    ActionListener alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuMoveTo(e);
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuMoveTo.add(jMenuItem);
    jMenuItem = new JMenuItem(item);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuCopyTo(e);
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuCopyTo.add(jMenuItem);
    if (this.menuNSVector.size() == 1) {
      jMenuMoveTo.setEnabled(false);
      jMenuCopyTo.setEnabled(false);
    } else {
      jMenuMoveTo.setEnabled(true);
      jMenuCopyTo.setEnabled(true);
    }
  }

  protected void setNSTabbedPanePanels(Vector nsVector) {
    this.nsVector = (Vector) nsVector.clone();
    jTabbedPaneRB.removeAll();
    GUIGlobalVars.nsMap.clear();
    NetworkServer currentNS;
    for (int i = 0; i < nsVector.size(); i++) {
      currentNS = (NetworkServer) nsVector.get(i);
      GUIGlobalVars.nsMap.put(currentNS.getName(), currentNS);
      jTabbedPaneRB.add(currentNS.getName(), new NSPanel(currentNS.getName(),
          JobSubmitter.this));
    }
  }

  protected void setNSMenuItems(Vector nsNamesVector) {
    this.nsNameVector = nsNamesVector;
    int nsVectorSize = nsNamesVector.size();
    if (nsVectorSize != 0) {
      this.menuNSVector = (Vector) nsNamesVector.clone();
      JMenuItem jMenuItem;
      ActionListener alst;
      jMenuMoveTo.removeAll();
      jMenuCopyTo.removeAll();
      String nsName;
      for (int i = 0; i < nsVectorSize; i++) {
        nsName = nsNamesVector.get(i).toString().trim();
        if (!nsName.equals(jTabbedPaneRB.getTitleAt(
            jTabbedPaneRB.getSelectedIndex()).trim())) {
          jMenuItem = new JMenuItem(nsName);
          alst = new ActionListener() {
            public void actionPerformed(ActionEvent e) {
              jMenuMoveTo(e);
            }
          };
          jMenuItem.addActionListener(alst);
          jMenuMoveTo.add(jMenuItem);
        }
        jMenuItem = new JMenuItem(nsName);
        alst = new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jMenuCopyTo(e);
          }
        };
        jMenuItem.addActionListener(alst);
        jMenuCopyTo.add(jMenuItem);
      }
    }
    setJMenuEditItemsEnabled();
    for (int i = 0; i < nsNamesVector.size(); i++) {
      (getRBPanelReference(nsNamesVector.get(i).toString()))
          .setNSMenuItems(nsNamesVector);
    }
  }

  boolean isJobMonitorActive() {
    return ((this.jobMonitorJFrame != null) && this.jobMonitorJFrame
        .isVisible());
  }

  public Vector getNSVector() {
    return (Vector) this.nsVector.clone();
  }

  public Vector getNSNameVector() {
    return (Vector) this.nsNameVector.clone();
  }

  JobSubmitterPreferences getJobSubmitterPreferencesReference() {
    return jobSubmitterPreferences;
  }

  boolean isSubmittedJob(String jobIdText) {
    if (!jobIdText.equals(Utils.NOT_SUBMITTED_TEXT)
        && !jobIdText.equals(Utils.SUBMITTING_TEXT)
        && !jobIdText.equals(Utils.LISTMATCHING_TEXT)) {
      return true;
    }
    return false;
  }
}
/**********************
 *
 * CLASS NetworkServer
 *
 **********************/

class NetworkServer {
  private String name;

  private String address;

  private String jdleSchema;

  public NetworkServer(String name, String address, String jdleSchema) {
    this.name = name;
    this.address = address;
    this.jdleSchema = jdleSchema;
  }

  public String getName() {
    return name;
  }

  public void setName(String name) {
    this.name = name;
  }

  public String getAddress() {
    return address;
  }

  public void setAddress(String address) {
    this.address = address;
  }

  public String getJDLESchema() {
    return jdleSchema;
  }

  public void setJDLESchema(String jdleSchema) {
    this.jdleSchema = jdleSchema;
  }
}