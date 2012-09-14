/*
 * JobMonitor.java
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
import java.awt.FlowLayout;
import java.awt.Point;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.WindowEvent;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.EOFException;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.URL;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
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
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.KeyStroke;
import javax.swing.ListSelectionModel;
import javax.swing.SwingConstants;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.filechooser.FileFilter;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Ad;
import org.glite.jdl.JobState;
import org.glite.wmsui.apij.Job;
import org.glite.wmsui.apij.JobCollection;
import org.glite.wmsui.apij.JobCollectionException;
import org.glite.wmsui.apij.JobId;
import org.glite.wmsui.apij.Query;
import org.glite.wmsui.apij.Url;
import org.glite.wmsui.apij.UserCredential;
import org.glite.wmsui.apij.UserJobs;

/**
 * Implementation of the JobMonitor class.
 *
 *
 * @ingroup gui
 * @brief Utility class used to store global variables values.
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class JobMonitor extends JFrame implements JobMonitorInterface {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final String TITLE = "Job Monitor";

  static final int JOB_MONITOR_MAIN = 0;

  static final int MULTIPLE_JOB_PANEL = 1;

  static final int JOB_ID_COLUMN_INDEX = 0;

  // Vector used to store the addresses of LB from where you can get jobs
  // (menu File option).
  private Vector menuLBVector = new Vector();

  Map dagMonThreadMap = new HashMap();

  Map dagMonitorMap = new HashMap();

  Map userQueryMap = new HashMap();

  Map userQueryAvailabilityMap = new HashMap();

  JobSubmitter jobSubmitterJFrame;

  JobMonitorPreferences jobMonitorPreferences;

  String selectedPreferences = Utils.DEFAULT_PREFERENCES;

  boolean isJobPresentOnStartup = false;

  private JPopupMenu jPopupMenuTable = new JPopupMenu();

  JMenuItem jMenuItemRemove = new JMenuItem("Remove");

  JMenuItem jMenuItemClear = new JMenuItem("Clear");

  JMenuItem jMenuItemSelectAll = new JMenuItem("Select All");

  JMenuItem jMenuItemSelectNone = new JMenuItem("Select None");

  JMenuItem jMenuItemInvertSelection = new JMenuItem("Invert Selection");

  JMenuItem jMenuItemStatus = new JMenuItem("Job Status");

  JMenuItem jMenuItemRetrieveCheckpointState = new JMenuItem(
      "Retrieve Checkpoint State");

  JMenuItem jMenuItemGetJobIdAllLB = new JMenuItem(
      "     Get Job Ids from All LBs");

  JMenu jMenuItemGetJobIdFromLB = new JMenu("     Get Job Ids from LB");

  JMenu jMenuItemGetJobIdMatchingQuery = new JMenu(
      "     Get Job Ids Matching Query");

  JMenuItem jMenuItemChangeVO = new JMenuItem("Change VO");

  JobTableModel jobTableModel;

  JTable jTableJobs;

  Vector vectorHeader = new Vector();

  JScrollPane jScrollPaneJobTable = new JScrollPane();

  JScrollPane jScrollPaneMain = new JScrollPane();

  JLabel jLabelTotalDisplayed = new JLabel();

  JLabel jLabelTotalDisplayedJobs = new JLabel();

  JTextField jTextFieldUserJobId = new JTextField();

  JButton jButtonClearJobId = new JButton();

  JButton jButtonAddJobId = new JButton();

  JPanel jPanelJobId = new JPanel();

  JPanel jPanelJobIdTable = new JPanel();

  JButton jButtonView = new JButton();

  MultipleJobPanel multipleJobPanel;

  JPanel jPanelMain = new JPanel();

  JLabel jLabelTotalSelected = new JLabel();

  JLabel jLabelTotalSelectedJobs = new JLabel();

  JMenuBar jMenuBar;

  JPanel jPanelButton = new JPanel();

  JPanel jPanelLabel = new JPanel();

  JPanel jPanelJobIdButton = new JPanel();

  /**
   * Constructor.
   */
  public JobMonitor() {
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
    // If a previous session has crashed try to recover it
    //loadConf(getJobMonitorRecoveryFile());
  }

  /**
   * Constructor.
   */
  public JobMonitor(JobSubmitter jobSubmitter) {
    super(TITLE + " - Virtual Organisation: "
        + GUIGlobalVars.getVirtualOrganisation());
    this.jobSubmitterJFrame = jobSubmitter;
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit();
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    setLBQueryMenuItems(loadUserQueriesFromFile());
    // If a previous session has crashed try to recover it
    loadConf(getJobMonitorRecoveryFile());
  }

  private void jbInit() throws Exception {
    // Set application type. The type of application affects on some settings.
    Utils.setApplicationType(Utils.FRAME);
    isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG) ? true
        : false;
    Toolkit toolkit = getToolkit();
    Dimension screenSize = toolkit.getScreenSize();
    this.setSize(new Dimension(
        (int) (screenSize.width * GraphicUtils.SCREEN_WIDTH_PROPORTION),
        (int) (screenSize.height * GraphicUtils.SCREEN_HEIGHT_PROPORTION)));
    jLabelTotalSelected.setText("Total Selected Jobs");
    jLabelTotalSelected.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelTotalSelectedJobs.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelTotalSelectedJobs.setBorder(BorderFactory.createLoweredBevelBorder());
    jLabelTotalSelectedJobs.setText("0");
    jLabelTotalDisplayed.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldUserJobId.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jTextFieldUserJobIdFocusLost(e);
      }
    });
    vectorHeader.add("Job Id");
    jobTableModel = new JobTableModel(vectorHeader, 0);
    jTableJobs = new JTable(jobTableModel);
    jTableJobs.getTableHeader().setReorderingAllowed(false);
    jLabelTotalDisplayed.setText("Total Displayed Jobs");
    jButtonClearJobId.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonClearJobIdEvent(e);
      }
    });
    jButtonClearJobId.setText("Clear");
    jButtonAddJobId.setText("Add");
    jButtonAddJobId.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonAddJobIdEvent(e);
      }
    });
    jPanelJobId.setBorder(new TitledBorder(new EtchedBorder(), " Job Id ", 0,
        0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelJobId.setLayout(new BorderLayout());
    jPanelJobIdTable.setBorder(new TitledBorder(new EtchedBorder(),
        " Job Id Table ", 0, 0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelJobIdTable.setLayout(new BorderLayout());
    jButtonView.setText("Job Status");
    jButtonView.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonStatusEvent(e);
      }
    });
    ListSelectionModel listSelectionModel = jTableJobs.getSelectionModel();
    listSelectionModel.addListSelectionListener(new ListSelectionListener() {
      public void valueChanged(ListSelectionEvent e) {
        if (e.getValueIsAdjusting()) {
          jLabelTotalSelectedJobs.setText(Integer.toString(jTableJobs
              .getSelectedRowCount()));
        }
      }
    });
    jTableJobs.addMouseListener(new MouseAdapter() {
      public void mouseClicked(MouseEvent me) {
        if (me.getClickCount() == 2) {
          Point point = me.getPoint();
          int row = jTableJobs.rowAtPoint(point);
          int column = jTableJobs.columnAtPoint(point);
          Vector jobIdVector = new Vector();
          jobIdVector.add(jTableJobs.getValueAt(row, JOB_ID_COLUMN_INDEX)
              .toString());
          setJobStatusTableJobs(jobIdVector);
        }
      }
    });
    jTableJobs.addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        showJPopupMenuTable(e);
      }

      public void mouseReleased(MouseEvent e) {
        showJPopupMenuTable(e);
      }
    });
    jScrollPaneJobTable.addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        if (jTableJobs.getRowCount() != 0) {
          jMenuSelectNone();
        }
        showJPopupMenuTable(e);
      }

      public void mouseReleased(MouseEvent e) {
        if (jTableJobs.getRowCount() != 0) {
          jMenuSelectNone();
        }
        showJPopupMenuTable(e);
      }
    });
    createJPopupMenu();
    this.menuLBVector = JobMonitorPreferences.loadPrefFileLB();
    setLBMenuItems(this.menuLBVector);
    this.userQueryMap = loadUserQueriesFromFile();
    setLBQueryMenuItems(this.userQueryMap);
    jMenuBar = createMenuBar(JOB_MONITOR_MAIN);
    setJMenuBar(jMenuBar);
    jPanelJobIdButton.setLayout(new BoxLayout(jPanelJobIdButton,
        BoxLayout.X_AXIS));
    jPanelJobIdButton.setBorder(GraphicUtils.SPACING_BORDER);
    jPanelJobIdButton.add(jButtonClearJobId, null);
    jPanelJobIdButton.add(Box.createGlue());
    jPanelJobIdButton.add(jButtonAddJobId, null);
    jTextFieldUserJobId.setPreferredSize(new Dimension(300, 18));
    jPanelJobId.add(jTextFieldUserJobId, BorderLayout.NORTH);
    jPanelJobId.add(jPanelJobIdButton, BorderLayout.SOUTH);
    ((FlowLayout) jPanelButton.getLayout()).setAlignment(FlowLayout.RIGHT);
    jPanelButton.add(jButtonView);
    jScrollPaneJobTable.getViewport().setBackground(Color.white);
    jScrollPaneJobTable
        .setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
    jScrollPaneJobTable.setBorder(null);
    jScrollPaneJobTable.getViewport().add(jTableJobs, null);
    jPanelLabel.setLayout(new BoxLayout(jPanelLabel, BoxLayout.X_AXIS));
    jPanelLabel.setBorder(GraphicUtils.SPACING_BORDER);
    jPanelLabel.add(jLabelTotalDisplayed, null);
    jPanelLabel.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jLabelTotalDisplayedJobs.setText("0");
    jLabelTotalDisplayedJobs.setPreferredSize(new Dimension(50, 18));
    jLabelTotalDisplayedJobs
        .setBorder(BorderFactory.createLoweredBevelBorder());
    jLabelTotalDisplayedJobs.setHorizontalAlignment(SwingConstants.RIGHT);
    jPanelLabel.add(jLabelTotalDisplayedJobs, null);
    jPanelLabel.add(Box.createGlue());
    jPanelLabel.add(jLabelTotalSelected, null);
    jPanelLabel.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jLabelTotalSelectedJobs.setPreferredSize(new Dimension(50, 18));
    jPanelLabel.add(jLabelTotalSelectedJobs, null);
    ((BorderLayout) jPanelJobIdTable.getLayout()).setHgap(GraphicUtils.H_GAP);
    ((BorderLayout) jPanelJobIdTable.getLayout()).setVgap(GraphicUtils.V_GAP);
    jPanelJobIdTable.add(jPanelLabel, BorderLayout.NORTH);
    jPanelJobIdTable.add(jScrollPaneJobTable, BorderLayout.CENTER);
    jPanelMain.setLayout(new BorderLayout());
    jPanelMain.setBorder(GraphicUtils.SPACING_BORDER);
    jPanelMain.add(jPanelJobId, BorderLayout.NORTH);
    jPanelMain.add(jPanelJobIdTable, BorderLayout.CENTER);
    jPanelMain.add(jPanelButton, BorderLayout.SOUTH);
    //this.getContentPane().add(jScrollPaneMain, null);
    //jPanelMain.setPreferredSize(new Dimension(660, 545));
    //jScrollPaneMain.getViewport().add(jPanelMain, null);
    this.getContentPane().setLayout(new BorderLayout());
    ((BorderLayout) getContentPane().getLayout()).setHgap(GraphicUtils.H_GAP);
    ((BorderLayout) getContentPane().getLayout()).setVgap(GraphicUtils.V_GAP);
    this.getContentPane().add(jPanelMain, null);
    JobMonitorPreferences.initializeUserConfiguration();
    Vector lbVectorVOConf = GUIGlobalVars.getLBVector();
    Vector lbVectorMonitor = JobMonitorPreferences.loadPrefFileLB();
    if (lbVectorMonitor.size() != 0) {
      setLBMenuItems(lbVectorMonitor);
      jMenuItemGetJobIdFromLB.setEnabled(true);
    } else if (lbVectorVOConf.size() != 0) {
      setLBMenuItems(lbVectorVOConf);
      jMenuItemGetJobIdFromLB.setEnabled(true);
      JobMonitorPreferences.savePrefFileLB(lbVectorVOConf);
    } else {
      setLBMenuItems(new Vector());
      jMenuItemGetJobIdFromLB.setEnabled(false);
      jMenuItemGetJobIdAllLB.setEnabled(false);
    }
  }

  // Configuration file generator:
  static protected File getJobMonitorRecoveryFile() {
    return new File(GUIFileSystem.getUserHomeDirectory() + File.separator
        + GUIFileSystem.getTemporaryFileDirectory()
        + GUIFileSystem.JOBMONITOR_RECOVERY_FILE + "_"
        + System.getProperty("user.name") + ".recover");
  }

  /**
   * Store the user jobId's information into a configuration file in order to
   * retrieve job in case of session crash
   **/
  protected void saveConf(File conf) {
    try {
      PrintWriter out = new PrintWriter(
          new BufferedWriter(new FileWriter(conf)));
      if (jobTableModel.getRowCount() != 0) {
        for (int i = 0; i < jobTableModel.getRowCount(); i++) {
          out.write(jTableJobs.getValueAt(i, JOB_ID_COLUMN_INDEX).toString()
              .trim()
              + "\n");
        }
      } else {
        out.write("");
      }
      out.flush();
      out.close();
    } catch (Exception exc) {
      if (isDebugging) {
        exc.printStackTrace();
      }
    }
  }

  protected void exit(boolean removeCtx) {
    if (JOptionPane.showOptionDialog(JobMonitor.this,
        "Do you really want to exit?", "Job Monitor - Confirm Exit",
        JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null,
        null) == 0) {
      if (removeCtx) {
        // Remove the context file and exit
        try {
          getJobMonitorRecoveryFile().delete();
        } catch (java.lang.SecurityException exc) {
          //Do nothing, the context is definetely lost
        }
      }
      if (multipleJobPanel != null) {
        multipleJobPanel.exitCurrentTimeThread();
        multipleJobPanel.exitUpdateThread();
        multipleJobPanel.disposeStatusDetails();
        multipleJobPanel.disposeLogInfo();
      }
      closeAllDagMonitorFrames();
      //GUIGlobalVars.isJobMonitorActive = false;
      //if(GUIGlobalVars.isJobSubmitterActive) {
      if ((jobSubmitterJFrame != null) && jobSubmitterJFrame.isVisible()) {
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

  void closeAllDagMonitorFrames() {
    Object[] values = this.dagMonitorMap.values().toArray();
    for (int i = 0; i < values.length; i++) {
      ((MultipleJobFrame) values[i]).exit();
    }
    this.dagMonitorMap.clear();
  }

  /**
   * If a previous session crashed or the user selected to automatically upload the previous context
   * the jobId(s) are to be re-loaded
   */
  protected void loadConf(File conf) {
    if (!conf.isFile()) {
      return;
    }
    try {
      // Reading info from the configuration file
      BufferedReader in = new BufferedReader(new FileReader(conf));
      String s;
      Vector result = new Vector();
      while ((s = in.readLine()) != null) {
        result.add(s);
      }
      in.close();
      if (result.size() != 0) {
        if (JOptionPane.showOptionDialog(JobMonitor.this,
            "A previous working session is available. Do you want to load it?",
            "Job Monitor - Job Monitoring Session Restoring",
            JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null,
            null, null) != 0) {
          conf.delete();
          return;
        }
        for (int i = 0; i < result.size(); i++) {
          Vector rowElement = new Vector();
          rowElement.add((String) result.get(i));
          jobTableModel.addRow(rowElement);
        }
        jLabelTotalDisplayedJobs.setText(Integer.toString(jobTableModel
            .getRowCount()));
        this.isJobPresentOnStartup = true;
      }
      jTextFieldUserJobId.selectAll();
      jTextFieldUserJobId.grabFocus();
    } catch (Exception exc) {
      if (isDebugging) {
        exc.printStackTrace();
      }
    }
  };

  protected JMenuBar createMenuBar(int frame) {
    final JMenuBar jMenuBar = new JMenuBar();
    ActionListener alst = null;
    JMenuItem jMenuItem = null;
    JMenu jMenuFile = new JMenu("Job");
    jMenuFile.setMnemonic('j');
    switch (frame) {
      case JOB_MONITOR_MAIN:
        jMenuItem = new JMenuItem("Open Job Ids List File...");
        URL fileOpenGifUrl = JobMonitor.class.getResource(Utils.ICON_FILE_OPEN);
        if (fileOpenGifUrl != null) {
          jMenuItem.setIcon(new ImageIcon(fileOpenGifUrl));
        }
        jMenuItem.setMnemonic('o');
        jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O,
            GraphicUtils.KEY_EVENT_MASK));
        alst = new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            JFileChooser fileChooser = new JFileChooser();
            fileChooser.setCurrentDirectory(new File(GUIGlobalVars
                .getFileChooserWorkingDirectory()));
            int choice = fileChooser.showOpenDialog(JobMonitor.this);
            if (choice != JFileChooser.APPROVE_OPTION) {
              return;
            } else if (!fileChooser.getSelectedFile().isFile()) {
              String selectedFile = fileChooser.getSelectedFile().toString()
                  .trim();
              JOptionPane.showOptionDialog(JobMonitor.this,
                  "Unable to find file: " + selectedFile, Utils.ERROR_MSG_TXT,
                  JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
                  null, null);
              return;
            } else {
              GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
                  .getCurrentDirectory().toString());
              String selectedFile = fileChooser.getSelectedFile().toString()
                  .trim();
              jMenuFileOpen(selectedFile);
            }
          }
        };
        jMenuItem.addActionListener(alst);
        jMenuFile.add(jMenuItem);
      break;
      default:
      break;
    }
    jMenuItem = new JMenuItem("Save Job Ids List File...");
    URL fileSaveGifUrl = JobMonitor.class.getResource(Utils.ICON_FILE_SAVE);
    if (fileSaveGifUrl != null) {
      jMenuItem.setIcon(new ImageIcon(fileSaveGifUrl));
    }
    jMenuItem.setMnemonic('s');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S,
        GraphicUtils.KEY_EVENT_MASK));
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        JFileChooser fileChooser = new JFileChooser();
        fileChooser.setCurrentDirectory(new File(GUIGlobalVars
            .getFileChooserWorkingDirectory()));
        fileChooser.setDialogTitle("Save Job Ids List File");
        int choice = fileChooser.showSaveDialog(JobMonitor.this);
        if (choice != JFileChooser.APPROVE_OPTION) {
          return;
        } else {
          GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
              .getCurrentDirectory().toString());
          String selectedFile = fileChooser.getSelectedFile().toString();
          jMenuFileSave(selectedFile, false);
        }
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuFile.add(jMenuItem);
    jMenuItem = new JMenuItem("     Save Selected Job Ids List File...");
    //URL fileSaveGifUrl = JobMonitor.class.getResource(Utils.ICON_FILE_SAVE);
    //if(fileSaveGifUrl != null) jMenuItem.setIcon(new ImageIcon(fileSaveGifUrl));
    jMenuItem.setMnemonic('a');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A,
        GraphicUtils.KEY_EVENT_MASK));
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        JTable jTable;
        if ((multipleJobPanel != null) && multipleJobPanel.isVisible()) {
          jTable = multipleJobPanel.jTableJobs;
        } else {
          jTable = jTableJobs;
        }
        if (jTable.getSelectedRowCount() != 0) {
          JFileChooser fileChooser = new JFileChooser();
          fileChooser.setCurrentDirectory(new File(GUIGlobalVars
              .getFileChooserWorkingDirectory()));
          fileChooser.setDialogTitle("Save Selected Job Ids List File");
          int choice = fileChooser.showSaveDialog(JobMonitor.this);
          if (choice != JFileChooser.APPROVE_OPTION) {
            return;
          } else {
            GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
                .getCurrentDirectory().toString());
            String selectedFile = fileChooser.getSelectedFile().toString();
            jMenuFileSave(selectedFile, true);
          }
        } else {
          JOptionPane.showOptionDialog(JobMonitor.this,
              "No selected Job Ids to save",
              "Job Monitor - Save Selected Job Ids List File",
              JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null,
              null, null);
        }
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuFile.add(jMenuItem);
    jMenuFile.addSeparator();
    switch (frame) {
      case JOB_MONITOR_MAIN:
        jMenuItemGetJobIdAllLB.setMnemonic('a');
        alst = new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jMenuAllLBGetJobId(e);
          }
        };
        jMenuItemGetJobIdAllLB.addActionListener(alst);
        jMenuFile.add(jMenuItemGetJobIdAllLB);
        //jMenuItemGetJobIdFromLB = new JMenu("     Get Job Ids from LB");
        //jMenuItem.setIcon(new ImageIcon("file_open.gif"));
        jMenuItemGetJobIdFromLB.setMnemonic('g');
        jMenuItemGetJobIdFromLB.removeAll();
        int menuLBVectorSize = this.menuLBVector.size();
        if (menuLBVectorSize != 0) {
          for (int i = 0; i < menuLBVectorSize; i++) {
            jMenuItem = new JMenuItem(this.menuLBVector.get(i).toString());
            alst = new ActionListener() {
              public void actionPerformed(ActionEvent e) {
                jMenuLBGetJobId(e);
              }
            };
            jMenuItem.addActionListener(alst);
            jMenuItemGetJobIdFromLB.add(jMenuItem);
          }
        } else {
          jMenuItemGetJobIdFromLB.setEnabled(false);
        }
        jMenuFile.add(jMenuItemGetJobIdFromLB);
        jMenuItemGetJobIdMatchingQuery.setMnemonic('q');
        jMenuItemGetJobIdMatchingQuery.removeAll();
        Iterator iterator = this.userQueryMap.keySet().iterator();
        String queryName = "";
        String lbAddress = "";
        Object value = null;
        int notEnabledCount = 0;
        while (iterator.hasNext()) {
          queryName = iterator.next().toString();
          value = userQueryAvailabilityMap.get(queryName);
          logger.debug("lbAddress: " + value);
          if (lbAddress == null) {
            logger.debug("Unable to get lbAddress for query: " + queryName);
            ///!! try to use the Map??
            continue;
          } else {
            lbAddress = value.toString();
          }
          jMenuItem = new JMenuItem(queryName);
          alst = new ActionListener() {
            public void actionPerformed(ActionEvent e) {
              jMenuLBGetMatchingQueryJobId(e);
            }
          };
          jMenuItem.addActionListener(alst);
          if (!lbAddress.equals(QueryPanel.ALL_LB_SERVERS)
              && !this.menuLBVector.contains(lbAddress)) {
            jMenuItem.setEnabled(false);
            notEnabledCount++;
          } else {
            jMenuItem.setEnabled(true);
          }
          jMenuItemGetJobIdMatchingQuery.add(jMenuItem);
        }
        if ((this.menuLBVector.size() == 0) || (this.userQueryMap.size() == 0)
            || (notEnabledCount == this.userQueryMap.size())) {
          jMenuItemGetJobIdMatchingQuery.setEnabled(false);
        } else {
          jMenuItemGetJobIdMatchingQuery.setEnabled(true);
        }
        jMenuFile.add(jMenuItemGetJobIdMatchingQuery);
        jMenuFile.addSeparator();
      break;
      default:
      break;
    }
    jMenuItem = new JMenuItem("     Preferences");
    jMenuItem.setMnemonic('p');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_P,
        GraphicUtils.KEY_EVENT_MASK));
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuFilePreferences();
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuFile.add(jMenuItem);
    jMenuFile.addSeparator();
    jMenuItem = new JMenuItem("     Exit");
    jMenuItem.setMnemonic('x');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_W,
        GraphicUtils.KEY_EVENT_MASK));
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        exit(false);
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuFile.add(jMenuItem);
    jMenuItem = new JMenuItem("     Exit & Clean Context");
    jMenuItem.setMnemonic('l');
    jMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_L,
        GraphicUtils.KEY_EVENT_MASK));
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        exit(true);
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuFile.add(jMenuItem);
    jMenuBar.add(jMenuFile);
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
        GUIUserCredentials credential = new GUIUserCredentials(JobMonitor.this,
            GUIUserCredentials.INFO_MODE);
        credential.setModal(true);
        GraphicUtils.windowCenterWindow(JobMonitor.this, credential);
        credential.setTitle(credential.getTitle() + " Info");
        credential.show();
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuProxy.add(jMenuItem);
    /* VO
     jMenuItem = new JMenuItem("Create");
     //jMenuItem.setIcon(new ImageIcon("file_open.gif"));
     jMenuItem.setMnemonic('c');
     alst = new ActionListener() {
     public void actionPerformed(ActionEvent e) {
     GUIUserCredentials credential = new GUIUserCredentials(JobMonitor.this, GUIUserCredentials.CREATION_MODE);
     credential.setModal(true);
     GraphicUtils.windowCenterWindow(JobMonitor.this, credential);
     credential.setTitle(credential.getTitle() + " Create");
     credential.show();
     }
     };
     jMenuItem.addActionListener(alst);
     jMenuProxy.add(jMenuItem);
     jMenuItem = new JMenuItem("Destroy");
     //jMenuItem.setIcon(new ImageIcon("file_open.gif"));
     jMenuItem.setMnemonic('d');
     alst = new ActionListener() {
     public void actionPerformed(ActionEvent e) {
     int choice = JOptionPane.showOptionDialog(JobMonitor.this,
     "Current Proxy: " + GUIGlobalVars.proxyFilePath + "\nDo you really want to destroy Proxy?",
     "Confirm Credential Destroy",
     JOptionPane.YES_NO_OPTION,
     JOptionPane.QUESTION_MESSAGE,
     null, null, null);
     if (choice == 0) {
     try {
     UserCredential userCredential = new UserCredential(new File(GUIGlobalVars.proxyFilePath));
     userCredential.destroyProxy();
     } catch (Exception ex) {
     JOptionPane.showOptionDialog(JobMonitor.this,
     ex.getMessage(),
     Utils.ERROR_MSG_TXT,
     JOptionPane.DEFAULT_OPTION,
     JOptionPane.ERROR_MESSAGE,
     null, null, null);
     }
     }
     }
     };
     jMenuItem.addActionListener(alst);
     jMenuProxy.add(jMenuItem);
     boolean flag = jMenuItemChangeVO.isEnabled();
     jMenuItemChangeVO = new JMenuItem("Change VO");
     jMenuItemChangeVO.setEnabled(flag);
     //jMenuItem.setIcon(new ImageIcon("file_open.gif"));
     jMenuItemChangeVO.setMnemonic('v');
     alst = new ActionListener() {
     public void actionPerformed(ActionEvent e) {
     GUIUserCredentials credential = new GUIUserCredentials(JobMonitor.this, GUIUserCredentials.CHANGE_VO_MODE);
     credential.setModal(true);
     GraphicUtils.windowCenterWindow(JobMonitor.this, credential);
     credential.setTitle(credential.getTitle() + " Change VO");
     credential.show();
     }
     };
     jMenuItemChangeVO.addActionListener(alst);
     jMenuProxy.add(jMenuItemChangeVO);
     VO */
    // Added to select the proxy file and then the VO to use.
    // Remove it when change VO will be possible from GUI interface.
    boolean flag = jMenuItemChangeVO.isEnabled();
    jMenuItemChangeVO = new JMenuItem("Select");
    jMenuItemChangeVO.setEnabled(flag);
    jMenuItemChangeVO.setMnemonic('s');
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        GUIUserCredentials credential = new GUIUserCredentials(JobMonitor.this,
            GUIUserCredentials.CHANGE_VO_MODE);
        credential.setModal(true);
        GraphicUtils.windowCenterWindow(JobMonitor.this, credential);
        credential.setTitle(credential.getTitle() + " Select");
        credential.show();
      }
    };
    jMenuItemChangeVO.addActionListener(alst);
    jMenuProxy.add(jMenuItemChangeVO);
    jMenuBar.add(jMenuProxy);
    switch (frame) {
      case MULTIPLE_JOB_PANEL:
      break;
      default:
      break;
    }
    JMenu jMenuHelp = new JMenu("Help");
    jMenuHelp.setMnemonic('h');
    jMenuItem = new JMenuItem("Help Topics");
    //jMenuItem.setIcon(new ImageIcon("file_open.gif"));
    jMenuItem.setMnemonic('h');
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
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

  public static void main(String[] args) {
    JFrame frame = new JobMonitor();
    GraphicUtils.screenCenterWindow(frame);
    frame.setVisible(true);
  }

  protected void processWindowEvent(WindowEvent e) {
    super.processWindowEvent(e);
    this.setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
    if (e.getID() == WindowEvent.WINDOW_CLOSING) {
      exit(false);
    }
  }

  void jButtonStatusEvent(ActionEvent e) {
    Vector jobIdVector = new Vector();
    int[] selectedRows = jTableJobs.getSelectedRows();
    if (selectedRows.length != 0) {
      for (int i = 0; i < selectedRows.length; i++) {
        jobIdVector.add(jTableJobs.getValueAt(selectedRows[i],
            JOB_ID_COLUMN_INDEX).toString().trim());
      }
      setJobStatusTableJobs(jobIdVector);
    } else {
      if ((multipleJobPanel == null)
          || (multipleJobPanel.getJobCollectionSize() == 0)) {
        JOptionPane.showOptionDialog(JobMonitor.this,
            "Please select at least a job", "Job Monitor - Job Status",
            JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
            null, null);
      } else {
        jMenuBar = createMenuBar(MULTIPLE_JOB_PANEL);
        setJMenuBar(jMenuBar);
        displayMultipleJobPanel();
        multipleJobPanel.startCurrentTimeThread();
        multipleJobPanel.startUpdateThread();
      }
    }
  }

  void setJobStatusTableJobs(final Vector jobIdVector) {
    final SwingWorker worker = new SwingWorker() {
      public Object construct() {
        // Standard code
        int jobIdVectorSize = jobIdVector.size();
        if (jobIdVectorSize != 0) {
          if (multipleJobPanel != null) {
            int choice = 1;
            if (multipleJobPanel.getJobCollectionSize() != 0) {
              Object[] options = { "Add", "Replace", "Cancel"
              };
              choice = JOptionPane.showOptionDialog(JobMonitor.this,
                  "Add selected job(s) to old ones or Replace them?",
                  "Job Monitor - Job Status", JOptionPane.DEFAULT_OPTION,
                  JOptionPane.QUESTION_MESSAGE, null, options, null);
            }
            JobCollection jobCollection;
            switch (choice) {
              case 0:
                Vector toAddJobIdVector = new Vector();
                String jobIdText = "";
                for (int i = 0; i < jobIdVector.size(); i++) {
                  jobIdText = jobIdVector.get(i).toString().trim();
                  if (!multipleJobPanel.jobTableModel.isElementPresentInColumn(
                      jobIdText, JOB_ID_COLUMN_INDEX)) {
                    toAddJobIdVector.add(jobIdText);
                  }
                }
                jobIdVectorSize = toAddJobIdVector.size();
                logger.debug("toAddJobIdVector: " + toAddJobIdVector);
                int monitoredJobCount = multipleJobPanel.getJobCollectionSize();
                logger.debug("GUIGlobalVars.currentMonitoredJobCount: "
                    + monitoredJobCount);
                int maxMonitoredJobNumber = MultipleJobPanel
                    .getMaxMonitoredJobNumber();
                if (jobIdVectorSize > (maxMonitoredJobNumber - monitoredJobCount)) {
                  if ((maxMonitoredJobNumber - monitoredJobCount) != 0) {
                    JOptionPane.showOptionDialog(JobMonitor.this,
                        "Unable to insert to Job Status Table more than "
                            + (maxMonitoredJobNumber - monitoredJobCount)
                            + " jobs", "Job Monitor - Job Status",
                        JOptionPane.DEFAULT_OPTION,
                        JOptionPane.WARNING_MESSAGE, null, null, null);
                  } else {
                    JOptionPane.showOptionDialog(JobMonitor.this,
                        "Max simultaneously monitored job number reached"
                            + "\nFirst remove one or more jobs from table",
                        "Job Monitor - Job Status", JOptionPane.DEFAULT_OPTION,
                        JOptionPane.WARNING_MESSAGE, null, null, null);
                    jMenuBar = createMenuBar(MULTIPLE_JOB_PANEL);
                    setJMenuBar(jMenuBar);
                    displayMultipleJobPanel();
                    multipleJobPanel.startCurrentTimeThread();
                    multipleJobPanel.startUpdateThread();
                  }
                  return "";
                }
                jobCollection = createJobCollection(toAddJobIdVector);
                multipleJobPanel.addJobStatusTableJobs(jobCollection);
                jMenuBar = createMenuBar(MULTIPLE_JOB_PANEL);
                setJMenuBar(jMenuBar);
                displayMultipleJobPanel();
                multipleJobPanel.startCurrentTimeThread();
                multipleJobPanel.startUpdateThread();
              break;
              case 1:
                if ((MultipleJobPanel.getMaxMonitoredJobNumber() >= jobIdVectorSize)) {
                  multipleJobPanel = new MultipleJobPanel(JobMonitor.this);
                  jMenuBar = createMenuBar(MULTIPLE_JOB_PANEL);
                  setJMenuBar(jMenuBar);
                  displayMultipleJobPanel();
                  jobCollection = createJobCollection(jobIdVector);
                  multipleJobPanel.setJobCollection(jobCollection);
                  multipleJobPanel.setJobStatusTableJobs();
                  multipleJobPanel.startCurrentTimeThread();
                  multipleJobPanel.startUpdateThread();
                } else {
                  JOptionPane.showOptionDialog(JobMonitor.this,
                      "Unable to insert to Job Status Table more than "
                          + MultipleJobPanel.getMaxMonitoredJobNumber()
                          + " jobs", "Job Monitor - Job Status",
                      JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE,
                      null, null, null);
                }
              break;
              case 2:
                return "";
              default:
              break;
            }
          } else {
            JobCollection jobCollection = createJobCollection(jobIdVector);
            int jobCollectionSize = jobCollection.size();
            if ((MultipleJobPanel.getMaxMonitoredJobNumber() >= jobCollectionSize)) {
              multipleJobPanel = new MultipleJobPanel(JobMonitor.this);
              jMenuBar = createMenuBar(MULTIPLE_JOB_PANEL);
              setJMenuBar(jMenuBar);
              displayMultipleJobPanel();
              multipleJobPanel.setJobCollection(jobCollection);
              multipleJobPanel.setJobStatusTableJobs();
              multipleJobPanel.startCurrentTimeThread();
              multipleJobPanel.startUpdateThread();
            } else {
              JOptionPane.showOptionDialog(JobMonitor.this,
                  "Unable to insert to Job Status Table more than "
                      + MultipleJobPanel.getMaxMonitoredJobNumber() + " jobs",
                  "Job Monitor - Job Status", JOptionPane.DEFAULT_OPTION,
                  JOptionPane.WARNING_MESSAGE, null, null, null);
            }
          }
        } else {
          jMenuBar = createMenuBar(MULTIPLE_JOB_PANEL);
          setJMenuBar(jMenuBar);
          displayMultipleJobPanel();
          multipleJobPanel.startCurrentTimeThread();
          multipleJobPanel.startUpdateThread();
        }
        // END Standard code
        return "";
      }
    };
    worker.start();
  }

  void jMenuLBGetMatchingQueryJobId(final ActionEvent ae) {
    final SwingWorker worker = new SwingWorker() {
      public Object construct() {
        // Standard code
        Ad queryAd = (Ad) userQueryMap.get(ae.getActionCommand().toString());
        logger.debug("Selected Query: " + ae.getActionCommand());
        Vector vectorJobId = new Vector();
        if (queryAd != null) {
          vectorJobId = getLBJobId(queryAd);
        } else {
          logger.debug("queryAd = null");
        }
        int jobIdCount = vectorJobId.size();
        if (jobIdCount != 0) {
          String jobIdText = "";
          for (int i = 0; i < jobIdCount; i++) {
            Vector vectorTemp = new Vector();
            jobIdText = ((JobId) vectorJobId.get(i)).toString();
            if (!jobTableModel.isRowPresent(jobIdText)) {
              vectorTemp.addElement(jobIdText);
              jobTableModel.addRow(vectorTemp);
            }
            jLabelTotalDisplayedJobs.setText(Integer.toString(jobTableModel
                .getRowCount()));
          }
        } else {
          JOptionPane.showOptionDialog(JobMonitor.this,
              "No Job Ids found Matching Query: "
                  + ae.getActionCommand().toString(),
              "Job Monitor - Get Job Ids Matching Query",
              JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE,
              null, null, null);
        }
        jLabelTotalDisplayedJobs.setText(Integer.toString(jobTableModel
            .getRowCount()));
        saveConf(getJobMonitorRecoveryFile());
        // END Standard code
        return "";
      }
    };
    worker.start();
  }

  void jMenuAllLBGetJobId(ActionEvent ae) {
    final SwingWorker worker = new SwingWorker() {
      public Object construct() {
        // Standard code
        String errorMsg = "";
        String warningMsg = "";
        String lbURLText = null;
        for (int i = 0; i < JobMonitor.this.menuLBVector.size(); i++) {
          lbURLText = JobMonitor.this.menuLBVector.get(i).toString();
          Vector vectorJobId = new Vector();
          try {
            vectorJobId = getLBJobId(lbURLText);
          } catch (Exception e) {
            if (isDebugging) {
              e.printStackTrace();
            }
            errorMsg += "- " + lbURLText + ": " + e.getMessage() + "\n";
            continue;
          }
          int jobIdCount = vectorJobId.size();
          if (jobIdCount != 0) {
            String jobIdText = "";
            for (int j = 0; j < jobIdCount; j++) {
              Vector vectorTemp = new Vector();
              jobIdText = ((JobId) vectorJobId.get(j)).toString();
              if (!jobTableModel.isRowPresent(jobIdText)) {
                vectorTemp.addElement(jobIdText);
                jobTableModel.addRow(vectorTemp);
              }
              jLabelTotalDisplayedJobs.setText(Integer.toString(jobTableModel
                  .getRowCount()));
            }
          } else {
            warningMsg += "- " + lbURLText + "\n";
          }
        }
        jLabelTotalDisplayedJobs.setText(Integer.toString(jobTableModel
            .getRowCount()));
        saveConf(getJobMonitorRecoveryFile());
        warningMsg = warningMsg.trim();
        if (!warningMsg.equals("")) {
          GraphicUtils.showOptionDialogMsg(JobMonitor.this, warningMsg,
              "Job Monitor - Get Job Ids from LB", JOptionPane.DEFAULT_OPTION,
              JOptionPane.INFORMATION_MESSAGE,
              Utils.MESSAGE_LINES_PER_JOPTIONPANE,
              "No Job Ids found from Logging & Bookkeeping Server(s):", null);
        }
        errorMsg = errorMsg.trim();
        if (!errorMsg.equals("")) {
          GraphicUtils.showOptionDialogMsg(JobMonitor.this, errorMsg,
              "Job Monitor - Get Job Ids from LB", JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
              "Unable to get Job Ids from Logging & Bookkeeping Server(s):",
              null);
        }
        // END Standard code
        return "";
      }
    };
    worker.start();
  }

  void jMenuLBGetJobId(final ActionEvent e) {
    final SwingWorker worker = new SwingWorker() {
      public Object construct() {
        // Standard code
        String lbURLText = e.getActionCommand();
        Vector vectorJobId = new Vector();
        try {
          vectorJobId = getLBJobId(lbURLText);
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
          JOptionPane.showOptionDialog(JobMonitor.this,
              "Unable to get Job Ids from Logging & Bookkeeping Server:\n"
                  + "- " + lbURLText + ": " + e.getMessage(),
              "Job Monitor - Get Job Ids from LB", JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
          return "";
        }
        int jobIdCount = vectorJobId.size();
        if (jobIdCount != 0) {
          String jobIdText = "";
          for (int i = 0; i < jobIdCount; i++) {
            Vector vectorTemp = new Vector();
            jobIdText = ((JobId) vectorJobId.get(i)).toString();
            if (!jobTableModel.isRowPresent(jobIdText)) {
              vectorTemp.addElement(jobIdText);
              jobTableModel.addRow(vectorTemp);
            }
            jLabelTotalDisplayedJobs.setText(Integer.toString(jobTableModel
                .getRowCount()));
          }
        } else {
          JOptionPane.showOptionDialog(JobMonitor.this,
              "No Job Ids found from Logging & Bookkeeping Server:\n" + "- "
                  + lbURLText, "Job Monitor - Get Job Ids from LB",
              JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE,
              null, null, null);
        }
        jLabelTotalDisplayedJobs.setText(Integer.toString(jobTableModel
            .getRowCount()));
        saveConf(getJobMonitorRecoveryFile());
        // END Standard code
        return "";
      }
    };
    worker.start();
  }

  void jMenuFileOpen(String fileName) {
    jobTableModel = new JobTableModel(vectorHeader, 0);
    jTableJobs.setModel(jobTableModel);
    Vector vectorJobId = getJobIdFromFile(fileName);
    int jobIdCount = vectorJobId.size();
    String jobIdText = "";
    String errorMsg = "";
    JobId jobId;
    for (int i = 0; i < jobIdCount; i++) {
      jobIdText = vectorJobId.get(i).toString().trim();
      try {
        jobId = new JobId(jobIdText);
      } catch (Exception e) {
        errorMsg += "- " + jobIdText + "\n";
        continue;
      }
      Vector vectorTemp = new Vector();
      vectorTemp.addElement(jobIdText);
      jobTableModel.addRow(vectorTemp);
    }
    errorMsg = errorMsg.trim();
    if (!errorMsg.equals("")) {
      GraphicUtils.showOptionDialogMsg(JobMonitor.this, errorMsg,
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
          "Unable to add the following Job Id(s):", "Please check format");
    }
    jLabelTotalDisplayedJobs.setText(Integer.toString(jobTableModel
        .getRowCount()));
    saveConf(getJobMonitorRecoveryFile());
  }

  void jMenuFileSave(String fileName, boolean selected) {
    File outputFile = new File(fileName);
    int choice = 0;
    if (outputFile.isFile()) {
      choice = JOptionPane.showOptionDialog(JobMonitor.this,
          "Output file exists. Overwrite?", "Job Monitor - Confirm Save",
          JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null, null,
          null);
    }
    if (choice == 0) {
      try {
        String text = "";
        if (!selected) {
          if ((multipleJobPanel != null) && multipleJobPanel.isVisible()) {
            int jobIdCount = multipleJobPanel.jobTableModel.getRowCount();
            for (int i = 0; i < jobIdCount; i++) {
              text += multipleJobPanel.jobTableModel.getValueAt(i,
                  JOB_ID_COLUMN_INDEX)
                  + "\n";
            }
          } else {
            int jobIdCount = jobTableModel.getRowCount();
            for (int i = 0; i < jobIdCount; i++) {
              text += jobTableModel.getValueAt(i, JOB_ID_COLUMN_INDEX) + "\n";
            }
          }
        } else {
          if ((multipleJobPanel != null) && multipleJobPanel.isVisible()) {
            int[] selectedRows = multipleJobPanel.jTableJobs.getSelectedRows();
            for (int i = 0; i < multipleJobPanel.jTableJobs
                .getSelectedRowCount(); i++) {
              text += multipleJobPanel.jobTableModel.getValueAt(
                  selectedRows[i], JOB_ID_COLUMN_INDEX)
                  + "\n";
            }
          } else {
            int[] selectedRows = jTableJobs.getSelectedRows();
            for (int i = 0; i < jTableJobs.getSelectedRowCount(); i++) {
              text += jobTableModel.getValueAt(selectedRows[i],
                  JOB_ID_COLUMN_INDEX)
                  + "\n";
            }
          }
        }
        GUIFileSystem.saveTextFile(fileName, text);
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        JOptionPane.showOptionDialog(JobMonitor.this, "Unable to save file: "
            + outputFile + ex.getMessage(), Utils.ERROR_MSG_TXT,
            JOptionPane.YES_NO_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
      }
    }
  }

  void jMenuFilePreferences() {
    jobMonitorPreferences = new JobMonitorPreferences(this);
    jobMonitorPreferences.setModal(true);
    GraphicUtils.windowCenterWindow(this, jobMonitorPreferences);
    jobMonitorPreferences.show();
  }

  Vector getJobIdFromFile(String fileName) {
    Vector vectorJobIds = new Vector();
    String inputLine;
    int length = 0;
    int rowNumber = 0;
    try {
      try {
        BufferedReader inputFile = new BufferedReader(new InputStreamReader(
            new FileInputStream(fileName)));
        inputLine = inputFile.readLine().trim();
        while (inputLine != null) {
          inputLine = inputLine.trim();
          length = inputLine.length();
          // Check for blank or comment line.
          String trimmedInputLine = inputLine.trim();
          if ((length == 0) || (trimmedInputLine.charAt(0) == '#')
              || (trimmedInputLine.charAt(0) == '*')
              || (trimmedInputLine.substring(0, 2).equals("//"))) {
            inputLine = inputFile.readLine();
            rowNumber++;
            continue;
          }
          vectorJobIds.add(inputLine);
          inputLine = inputFile.readLine();
          rowNumber++;
        }
      } catch (EOFException eofe) {
        if (isDebugging) {
          eofe.printStackTrace();
        }
      }
    } catch (FileNotFoundException fnfe) {
      if (isDebugging) {
        fnfe.printStackTrace();
      }
    } catch (IOException ioe) {
      JOptionPane.showOptionDialog(JobMonitor.this, "Unable to read file: "
          + fileName, Utils.ERROR_MSG_TXT, JOptionPane.YES_NO_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      if (isDebugging) {
        ioe.printStackTrace();
      }
    }
    return vectorJobIds;
  }

  Vector getLBJobId(Ad queryAd) { //throws Exception {
    Vector jobIdVector = new Vector();
    boolean allLBFlag = false;
    try {
      allLBFlag = ((Boolean) queryAd.getBooleanValue(Utils.ALL_LB_FLAG).get(0))
          .booleanValue();
    } catch (Exception ex) {
      if (isDebugging) {
        // Attribute not present.
        ex.printStackTrace();
      }
    }
    Vector lbURLVector = new Vector();
    if (!allLBFlag) {
      try {
        lbURLVector.add(queryAd.getStringValue(Utils.LB_ADDRESS).get(0)
            .toString());
      } catch (Exception ex) {
        // Attribute not present.
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
    } else {
      //lbURLVector = (Vector) GUIGlobalVars.getLBVector().clone();
      lbURLVector = (Vector) this.menuLBVector.clone();
    }
    Calendar fromDate = Calendar.getInstance();
    Calendar toDate = Calendar.getInstance();
    try {
      if (queryAd.hasAttribute(Utils.FROM_DATE)) {
        fromDate.setTimeInMillis(Long.parseLong(queryAd.getStringValue(
            Utils.FROM_DATE).get(0).toString(), 10));
      } else {
        fromDate = null;
      }
      if (queryAd.hasAttribute(Utils.TO_DATE)) {
        toDate.setTimeInMillis(Long.parseLong(queryAd.getStringValue(
            Utils.TO_DATE).get(0).toString(), 10));
      } else {
        toDate = null;
      }
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
    }
    Ad userTagsAd = new Ad();
    try {
      if (queryAd.hasAttribute(Utils.USER_TAGS)) {
        userTagsAd = (Ad) queryAd.getAdValue(Utils.USER_TAGS).get(0);
        if (userTagsAd.size() == 0) {
          userTagsAd = null;
        }
      }
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
    }
    boolean ownedJobsOnly = true;
    try {
      ownedJobsOnly = ((Boolean) queryAd.getBooleanValue(
          Utils.OWNED_JOBS_ONLY_FLAG).get(0)).booleanValue();
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
    }
    String errorMsg = "";
    for (int i = 0; i < lbURLVector.size(); i++) {
      try {
        UserJobs userJobs = new UserJobs();
        Url lbURL = new Url(lbURLVector.get(i).toString());
        UserCredential userCredential = new UserCredential(new File(
            GUIGlobalVars.proxyFilePath));
        if (userCredential.getX500UserSubject().equals(
            GUIGlobalVars.proxySubject)) {
          logger.info("userJobs.getJobs(" + lbURL + ", " + fromDate + ", "
              + toDate + ", " + userTagsAd + ", " + ownedJobsOnly + ")");


	/** Old approach - deprecated */
	// jobIdVector = userJobs.getJobs(lbURL, fromDate, toDate, userTagsAd, ownedJobsOnly);

	/** Query approach */
	Query datequery = new Query () ;
	datequery.setTimeFrom(fromDate);
	datequery.setTimeTo(toDate);
	datequery.setUserTags(userTagsAd);
	if ( ownedJobsOnly) datequery.setOwned () ;
	jobIdVector = userJobs.getJobs(lbURL, datequery);






        } else {
          JOptionPane.showOptionDialog(JobMonitor.this, Utils.FATAL_ERROR
              + "Proxy file user subject has changed"
              + "\nApplication will be terminated", Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
              null, null);
          System.exit(-1);
        }
      } catch (Exception ex) {
        //throw e;
        if (isDebugging) {
          ex.printStackTrace();
        }
        errorMsg += ex.getMessage() + "\n";
      }
    }
    errorMsg = errorMsg.trim();
    if (!errorMsg.equals("")) {
      JOptionPane.showOptionDialog(JobMonitor.this, errorMsg,
          "Job Monitor - Get Job Ids Matching Query",
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
    }
    return jobIdVector;
  }

  Vector getLBJobId(String lbURLText) throws Exception {
    logger.info("getLBJobId() lbURLText: " + lbURLText);
    Vector jobIdVector = new Vector();
    try {
      UserJobs userJobs = new UserJobs();
      Url lbURL = new Url(lbURLText);
      UserCredential userCredential = new UserCredential(new File(
          GUIGlobalVars.proxyFilePath));
      if (userCredential.getX500UserSubject()
          .equals(GUIGlobalVars.proxySubject)) {
        jobIdVector = userJobs.getJobs(lbURL);
      } else {
        JOptionPane.showOptionDialog(JobMonitor.this, Utils.FATAL_ERROR
            + "Proxy file user subject has changed"
            + "\nApplication will be terminated", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        System.exit(-1);
      }
    } catch (Exception e) {
      throw e;
    }
    return jobIdVector;
  }

  void jButtonClearJobIdEvent(ActionEvent e) {
    jTextFieldUserJobId.setText("");
    jTextFieldUserJobId.grabFocus();
  }

  void jButtonAddJobIdEvent(ActionEvent e) {
    String insertedJobId = jTextFieldUserJobId.getText().trim();
    if (!insertedJobId.equals("")) {
      if (!jobTableModel.isElementPresentInColumn(insertedJobId,
          JOB_ID_COLUMN_INDEX)) {
        try {
          JobId jobId = new JobId(insertedJobId);
          jTextFieldUserJobId.setText("");
        } catch (IllegalArgumentException iae) {
          if (isDebugging) {
            iae.printStackTrace();
          }
          JOptionPane.showOptionDialog(JobMonitor.this,
              "Inserted Job Id has wrong format\nPlease check value",
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
          jTextFieldUserJobId.selectAll();
          jTextFieldUserJobId.grabFocus();
          return;
        } catch (Exception ex) {
          JOptionPane.showOptionDialog(JobMonitor.this, ex.getMessage(),
              Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE, null, null, null);
          if (isDebugging) {
            ex.printStackTrace();
          }
          return;
        }
        Vector rowElement = new Vector();
        rowElement.add(insertedJobId);
        jobTableModel.addRow(rowElement);
        jLabelTotalDisplayedJobs.setText(Integer.toString(jobTableModel
            .getRowCount()));
        saveConf(getJobMonitorRecoveryFile());
      } else {
        JOptionPane.showOptionDialog(JobMonitor.this,
            "Inserted Job Id is already present", Utils.WARNING_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null,
            null, null);
      }
      jTextFieldUserJobId.selectAll();
      jTextFieldUserJobId.grabFocus();
    }
  }

  void jButtonCloseEvent(ActionEvent e) {
    if (jobSubmitterJFrame != null) {
      JobMonitor.this.dispose();
    } else {
      exit(false);
    }
  }

  void jButtonAllEvent(ActionEvent e) {
    jTableJobs.selectAll();
    jLabelTotalSelectedJobs.setText(Integer.toString(jTableJobs
        .getSelectedRowCount()));
  }

  void jButtonNoneEvent(ActionEvent e) {
    jTableJobs.clearSelection();
    jLabelTotalSelectedJobs.setText(Integer.toString(0));
  }

  void jMenuHelpAbout() {
    URL url = JobDef1Panel.class.getResource(Utils.ICON_DATAGRID_LOGO);
    JOptionPane.showOptionDialog(JobMonitor.this,
    //Utils.JOB_MONITOR_ABOUT_MSG,
        "Job Monitor Version " + GUIGlobalVars.getGUIConfVarVersion() + "\n\n"
            + Utils.COPYRIGHT, "Job Monitor - About",
        JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE,
        (url == null) ? null : new ImageIcon(url), null, null);
  }

  /*
   void jMenuSortByJobId() {
   GUIGlobalVars.setMultipleJobPanelSortColumn(MultipleJobPanel.JOB_ID_COLUMN_INDEX);
   multipleJobPanel.sortBy(MultipleJobPanel.JOB_ID_COLUMN_INDEX, true);
   }
   void jMenuSortByJobType() {
   GUIGlobalVars.setMultipleJobPanelSortColumn(MultipleJobPanel.JOB_TYPE_COLUMN_INDEX);
   multipleJobPanel.sortBy(MultipleJobPanel.JOB_TYPE_COLUMN_INDEX, true);
   }
   void jMenuSortByStatus() {
   GUIGlobalVars.setMultipleJobPanelSortColumn(MultipleJobPanel.JOB_STATUS_COLUMN_INDEX);
   multipleJobPanel.sortBy(MultipleJobPanel.JOB_STATUS_COLUMN_INDEX, true);
   }
   void jMenuSortBySubmissionTime() {
   GUIGlobalVars.setMultipleJobPanelSortColumn(MultipleJobPanel.SUBMISSION_TIME_COLUMN_INDEX);
   multipleJobPanel.sortBy(MultipleJobPanel.SUBMISSION_TIME_COLUMN_INDEX, true);
   }
   void jMenuSortByDestination() {
   GUIGlobalVars.setMultipleJobPanelSortColumn(MultipleJobPanel.DESTINATION_COLUMN_INDEX);
   multipleJobPanel.sortBy(MultipleJobPanel.DESTINATION_COLUMN_INDEX, true);
   }
   */
  /*
   void jMenuSortByNetworkServer() {
   GUIGlobalVars.setMultipleJobPanelSortColumn(MultipleJobPanel.NETWORK_SERVER_COLUMN_INDEX);
   multipleJobPanel.sortBy(MultipleJobPanel.NETWORK_SERVER_COLUMN_INDEX, true);
   }
   */
  /*
   void jMenuSortByAddingOrder() {
   GUIGlobalVars.setMultipleJobPanelSortColumn(Utils.NO_SORTING);
   multipleJobPanel.sortBy(Utils.NO_SORTING, true);
   }
   */
  /*
   public void sortBy(int columnIndex, boolean ascending) {
   Vector vectorData = multipleJobPanel.jobTableModel.getDataVector();
   Collections.sort(vectorData, new ColumnSorter(columnIndex, ascending));
   multipleJobPanel.jobTableModel.fireTableStructureChanged();
   }
   */
  void jMenuRemove(ActionEvent e) {
    int[] selectedRows = jTableJobs.getSelectedRows();
    int selectedRowCount = selectedRows.length;
    int choice = JOptionPane.showOptionDialog(JobMonitor.this,
        "Remove selected Job Id(s) from table?",
        "Job Monitor - Confirm Remove", JOptionPane.YES_NO_OPTION,
        JOptionPane.QUESTION_MESSAGE, null, null, null);
    if (choice == 0) {
      for (int i = selectedRowCount - 1; i >= 0; i--) {
        jobTableModel.removeRow(selectedRows[i]);
        // remove element also from MultipleJobPanel table?? YES!!
        // multipleJobPanel.removeJobFromTable(jobTableModel.getValueAt(selectedRow[i],
        //          multipleJobPanel.JOB_ID_COLUMN_INDEX).toString());
        // after removing job is still submitted.
        // N.B. not necessary because jobmonitorjframe and multiplejobpanel are mutually exclusive.
      }
      jLabelTotalSelectedJobs.setText(Integer.toString(jTableJobs
          .getSelectedRowCount()));
      jLabelTotalDisplayedJobs.setText(Integer.toString(jobTableModel
          .getRowCount()));
      saveConf(getJobMonitorRecoveryFile());
    }
  }

  /*
   void showMultipleJobPanel() {
   Vector jobIdVector = new Vector();
   String jobIdText;
   for (int i = 0; i < jobTableModel.getRowCount(); i++) {
   jobIdText = jobTableModel.getValueAt(i, JOB_ID_COLUMN_INDEX).toString().trim();
   jobIdVector.add(jobIdText);
   }
   showMultipleJobPanel(jobIdVector);
   }
   */
  public JobCollection createJobCollection(Vector jobIdVector) {
    JobCollection jobCollection = new JobCollection();
    String jobIdText = "";
    for (int i = 0; i < jobIdVector.size(); i++) {
      jobIdText = jobIdVector.get(i).toString().trim();
      try {
        JobId jobId = new JobId(jobIdText);
        jobCollection.insertId(jobId);
      } catch (IllegalArgumentException iae) {
        if (isDebugging) {
          iae.printStackTrace();
        }
        JOptionPane.showOptionDialog(JobMonitor.this, Utils.UNESPECTED_ERROR
            + iae.getMessage(), Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
      } catch (JobCollectionException jce) {
        if (isDebugging) {
          jce.printStackTrace();
        }
        JOptionPane.showOptionDialog(JobMonitor.this, Utils.UNESPECTED_ERROR
            + jce.getMessage(), Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        JOptionPane.showOptionDialog(JobMonitor.this, ex.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
    }
    return jobCollection;
  }

  void showMultipleJobPanel(Vector jobIdVector) {
    int jobIdVectorSize = jobIdVector.size();
    if (jobIdVectorSize != 0) {
      String warningMsg = "";
      if ((multipleJobPanel != null) && multipleJobPanel.isVisible()) {
        Vector toAddJobIdVector = new Vector();
        String jobIdText;
        for (int i = 0; i < jobIdVector.size(); i++) {
          jobIdText = jobIdVector.get(i).toString().trim();
          if (!multipleJobPanel.jobTableModel.isElementPresentInColumn(
              jobIdText, JOB_ID_COLUMN_INDEX)) {
            toAddJobIdVector.add(jobIdText);
          }
        }
        int toAddJobIdVectorSize = toAddJobIdVector.size();
        if (toAddJobIdVectorSize == 0) {
          return;
        }
        JobCollection jobCollection = createJobCollection(toAddJobIdVector);
        int jobLeft = MultipleJobPanel.getMaxMonitoredJobNumber()
            - multipleJobPanel.getJobCollectionSize();
        if (jobLeft >= toAddJobIdVectorSize) {
          logger.debug("ADDING");
          multipleJobPanel.addJobStatusTableJobs(jobCollection);
        } else {
          if (jobLeft == 0) {
            warningMsg = "Max simultaneously monitored job number reached"
                + "\nFirst remove one or more jobs from table";
          } else {
            warningMsg = "Unable to insert to Job Status Table more than "
                + jobLeft + " jobs";
          }
        }
      } else {
        JobCollection jobCollection = createJobCollection(jobIdVector);
        if (MultipleJobPanel.getMaxMonitoredJobNumber() >= jobIdVectorSize) {
          logger.debug("MultipleJobPanel.getMaxMonitoredJobNumber(): "
              + MultipleJobPanel.getMaxMonitoredJobNumber());
          multipleJobPanel = new MultipleJobPanel(this);
          jMenuBar = createMenuBar(MULTIPLE_JOB_PANEL);
          setJMenuBar(jMenuBar);
          displayMultipleJobPanel();
          multipleJobPanel.setJobCollection(jobCollection);
          final SwingWorker worker = new SwingWorker() {
            public Object construct() {
              // Standard code
              multipleJobPanel.setJobStatusTableJobs();
              // END Standard code
              return "";
            }
          };
          worker.start();
        } else {
          warningMsg = "Unable to insert to Job Status Table more than "
              + MultipleJobPanel.getMaxMonitoredJobNumber() + " jobs";
        }
      }
      if (!warningMsg.equals("")) {
        JOptionPane.showOptionDialog(JobMonitor.this, warningMsg,
            Utils.WARNING_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE, null, null, null);
      }
    }
  }

  void addJobId(String jobId) {
    if (!jobTableModel.isElementPresentInColumn(jobId, JOB_ID_COLUMN_INDEX)) {
      Vector rowElement = new Vector();
      rowElement.add(jobId);
      jobTableModel.addRow(rowElement);
      jLabelTotalDisplayedJobs.setText(Integer.toString(jobTableModel
          .getRowCount()));
      saveConf(getJobMonitorRecoveryFile());
    }
  }

  void addSubmittedJobs(final Vector jobVector) {
    final SwingWorker worker = new SwingWorker() {
      public Object construct() {
        // Standard code
        if (jobVector.size() != 0) {
          String jobIdText;
          for (int i = 0; i < jobVector.size(); i++) {
            jobIdText = jobVector.get(i).toString().trim();
            if (!jobTableModel.isElementPresentInColumn(jobIdText,
                JOB_ID_COLUMN_INDEX)) {
              Vector elementToAdd = new Vector();
              elementToAdd.add(jobIdText);
              jobTableModel.addRow(elementToAdd);
            }
          }
          jLabelTotalDisplayedJobs.setText(Integer.toString(jobTableModel
              .getRowCount()));
          jLabelTotalSelectedJobs.setText(Integer.toString(jTableJobs
              .getSelectedRowCount()));
          saveConf(getJobMonitorRecoveryFile());
        }
        // END Standard code
        return "";
      }
    };
    worker.start();
  }

  protected void renewJPopupMenu() {
    jPopupMenuTable.add(jMenuItemRemove);
    jPopupMenuTable.add(jMenuItemClear);
    jPopupMenuTable.addSeparator();
    jPopupMenuTable.add(jMenuItemSelectAll);
    jPopupMenuTable.add(jMenuItemSelectNone);
    jPopupMenuTable.add(jMenuItemInvertSelection);
    jPopupMenuTable.addSeparator();
    jPopupMenuTable.add(jMenuItemStatus);
    jPopupMenuTable.addSeparator();
    jPopupMenuTable.add(jMenuItemRetrieveCheckpointState);
  }

  protected void createJPopupMenu() {
    ActionListener alst = null;
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuRemove(e);
      }
    };
    jMenuItemRemove.addActionListener(alst);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuClear();
      }
    };
    jMenuItemClear.addActionListener(alst);
    jPopupMenuTable.add(jMenuItemRemove);
    jPopupMenuTable.add(jMenuItemClear);
    jPopupMenuTable.addSeparator();
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuSelectAll();
      }
    };
    jMenuItemSelectAll.addActionListener(alst);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuSelectNone();
      }
    };
    jMenuItemSelectNone.addActionListener(alst);
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuInvertSelection();
      }
    };
    jMenuItemInvertSelection.addActionListener(alst);
    jPopupMenuTable.add(jMenuItemSelectAll);
    jPopupMenuTable.add(jMenuItemSelectNone);
    jPopupMenuTable.add(jMenuItemInvertSelection);
    jPopupMenuTable.addSeparator();
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonStatusEvent(null);
      }
    };
    jMenuItemStatus.addActionListener(alst);
    jPopupMenuTable.add(jMenuItemStatus);
    jPopupMenuTable.addSeparator();
    alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        int[] selectedRows = jTableJobs.getSelectedRows();
        jMenuRetrieveCheckpointState(jobTableModel.getValueAt(selectedRows[0],
            JOB_ID_COLUMN_INDEX).toString(), true);
      }
    };
    jMenuItemRetrieveCheckpointState.addActionListener(alst);
    jPopupMenuTable.add(jMenuItemRetrieveCheckpointState);
  }

  void showJPopupMenuTable(MouseEvent e) {
    if (e.isPopupTrigger()) {
      Point point = e.getPoint();
      int row = jTableJobs.rowAtPoint(point);
      if ((row != -1) && !jTableJobs.isRowSelected(row)) {
        jTableJobs.setRowSelectionInterval(row, row);
      }
      if (jTableJobs.getRowCount() != 0) {
        int selectedRowCount = jTableJobs.getSelectedRowCount();
        if (selectedRowCount == 0) {
          jMenuItemRemove.setEnabled(false);
          jMenuItemStatus.setEnabled(false);
        } else {
          jMenuItemRemove.setEnabled(true);
          jMenuItemStatus.setEnabled(true);
        }
        if (selectedRowCount == 1) {
          jMenuItemRetrieveCheckpointState.setEnabled(true);
        } else {
          jMenuItemRetrieveCheckpointState.setEnabled(false);
        }
      } else {
        jMenuItemRemove.setEnabled(false);
        jMenuItemClear.setEnabled(false);
        jMenuItemSelectAll.setEnabled(false);
        jMenuItemSelectNone.setEnabled(false);
        jMenuItemInvertSelection.setEnabled(false);
        jMenuItemStatus.setEnabled(false);
        jMenuItemRetrieveCheckpointState.setEnabled(false);
      }
      jPopupMenuTable = new JPopupMenu();
      renewJPopupMenu();
      if (jButtonView.isEnabled() == false) {
        jMenuItemStatus.setEnabled(false);
      }
      jPopupMenuTable.show(e.getComponent(), e.getX(), e.getY());
    }
  }

  String getToolTipText() {
    return "";
  }

  protected void addLBMenuItem(String item) {
    this.menuLBVector.add(item);
    JMenuItem jMenuItem = new JMenuItem(item);
    ActionListener alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuLBGetJobId(e);
      }
    };
    jMenuItem.addActionListener(alst);
    jMenuItemGetJobIdFromLB.add(jMenuItem);
  }

  protected void setLBMenuItems(Vector lbVector) {
    int lbVectorSize = lbVector.size();
    if (lbVector == null) {
      return;
    }
    this.menuLBVector = (Vector) lbVector.clone();
    if (lbVectorSize != 0) {
      JMenuItem jMenuItem;
      ActionListener alst;
      jMenuItemGetJobIdFromLB.removeAll();
      for (int i = 0; i < lbVectorSize; i++) {
        jMenuItem = new JMenuItem(lbVector.get(i).toString());
        alst = new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jMenuLBGetJobId(e);
          }
        };
        jMenuItem.addActionListener(alst);
        jMenuItemGetJobIdFromLB.add(jMenuItem);
      }
      jMenuItemGetJobIdFromLB.setEnabled(true);
      jMenuItemGetJobIdAllLB.setEnabled(true);
      jMenuItemGetJobIdMatchingQuery.removeAll();
      Iterator iterator = this.userQueryMap.keySet().iterator();
      String lbAddress = "";
      int notEnabledCount = 0;
      while (iterator.hasNext()) {
        lbAddress = iterator.next().toString();
        logger.debug("lbAddress: " + lbAddress);
        jMenuItem = new JMenuItem(lbAddress);
        alst = new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jMenuLBGetMatchingQueryJobId(e);
          }
        };
        jMenuItem.addActionListener(alst);
        if (!lbAddress.equals(QueryPanel.ALL_LB_SERVERS)
            && !this.menuLBVector.contains(lbAddress)) {
          jMenuItem.setEnabled(false);
          notEnabledCount++;
        } else {
          jMenuItem.setEnabled(true);
        }
        jMenuItemGetJobIdMatchingQuery.add(jMenuItem);
      }
      if ((this.userQueryMap.size() == 0)
          || (notEnabledCount == this.userQueryMap.size())) {
        jMenuItemGetJobIdMatchingQuery.setEnabled(false);
      } else {
        jMenuItemGetJobIdMatchingQuery.setEnabled(true);
      }
    } else {
      jMenuItemGetJobIdFromLB.removeAll();
      jMenuItemGetJobIdFromLB.setEnabled(false);
      jMenuItemGetJobIdAllLB.setEnabled(false);
      jMenuItemGetJobIdMatchingQuery.setEnabled(false);
    }
  }

  Vector getLBMenuItems() {
    return (Vector) this.menuLBVector.clone();
  }

  protected void jMenuClear() {
    int choice = JOptionPane.showOptionDialog(JobMonitor.this,
        "Clear Job Id table?", "Job Monitor - Confirm Clear",
        JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null,
        null);
    if (choice == 0) {
      jobTableModel.removeAllRows();
      jLabelTotalSelectedJobs.setText(Integer.toString(jTableJobs
          .getSelectedRowCount()));
      jLabelTotalDisplayedJobs.setText(Integer.toString(jTableJobs
          .getRowCount()));
      saveConf(getJobMonitorRecoveryFile());
    }
  }

  protected void jMenuSelectAll() {
    jTableJobs.selectAll();
    jLabelTotalSelectedJobs.setText(Integer.toString(jTableJobs
        .getSelectedRowCount()));
  }

  protected void jMenuSelectNone() {
    jTableJobs.clearSelection();
    jLabelTotalSelectedJobs.setText(Integer.toString(0));
  }

  protected void jMenuInvertSelection() {
    int rowCount = jTableJobs.getRowCount();
    int selectedRowCount = jTableJobs.getSelectedRowCount();
    if (selectedRowCount != 0) {
      ListSelectionModel listSelectionModel = jTableJobs.getSelectionModel();
      for (int i = 0; i < rowCount; i++) {
        if (listSelectionModel.isSelectedIndex(i)) {
          listSelectionModel.removeSelectionInterval(i, i);
        } else {
          listSelectionModel.addSelectionInterval(i, i);
        }
      }
    } else {
      jTableJobs.selectAll();
    }
    jLabelTotalSelectedJobs.setText(Integer.toString(jTableJobs
        .getSelectedRowCount()));
  }

  int getJobCount() {
    return jobTableModel.getRowCount();
  }

  public void setMenuBar(int frame) {
    jMenuBar = createMenuBar(frame);
    setJMenuBar(jMenuBar);
  }

  void jTextFieldUserJobIdFocusLost(FocusEvent e) {
    jTextFieldUserJobId.select(0, 0);
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
    int choice = fileChooser.showOpenDialog(JobMonitor.this);
    if (choice != JFileChooser.APPROVE_OPTION) {
      return;
    } else if (!fileChooser.getSelectedFile().isFile()) {
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      JOptionPane.showOptionDialog(JobMonitor.this, "Unable to find file: "
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
        GraphicUtils.showOptionDialogMsg(JobMonitor.this, e.getMessage(),
            "Job Monitor - Open Checkpoint State", JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE,
            null, null);
      }
    }
  }

  void jMenuRetrieveCheckpointState(final String jobIdText,
      final boolean isEditable) {
    final SwingWorker worker = new SwingWorker() {
      public Object construct() {
        // Standard code
        RetrieveCheckpointStateDialog retrieveCheckpointState = new RetrieveCheckpointStateDialog(
            JobMonitor.this, jobIdText, isEditable);
        retrieveCheckpointState.setModal(true);
        GraphicUtils.windowCenterWindow(JobMonitor.this,
            retrieveCheckpointState);
        retrieveCheckpointState.show();
        String jobId = retrieveCheckpointState.getJobId();
        int state = retrieveCheckpointState.getState();
        logger.debug("jobId: " + jobId + " state: " + Integer.toString(state));
        if (jobId != null) {
          jobId = jobId.trim();
          if (!jobId.equals("")) {
            Job job = new Job();
            JobState jobState = new JobState();
            try {
              job = new Job(new JobId(jobId));
            } catch (IllegalArgumentException iae) {
              JOptionPane.showOptionDialog(JobMonitor.this, iae.getMessage(),
                  "Job Monitor - Retrieve Checkpoint State",
                  JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
                  null, null);
              if (isDebugging) {
                iae.printStackTrace();
                //SW return;
              }
              return "";
            } catch (Exception e) {
              JOptionPane.showOptionDialog(JobMonitor.this, e.getMessage(),
                  "Job Monitor - Retrieve Checkpoint State",
                  JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
                  null, null);
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
                JOptionPane.showOptionDialog(JobMonitor.this, Utils.FATAL_ERROR
                    + "Proxy file user subject has changed"
                    + "\nApplication will be terminated", Utils.ERROR_MSG_TXT,
                    JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE,
                    null, null, null);
                System.exit(-1);
              }
              logger.debug("jobState: " + jobState);
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
                int choice = fileChooser.showSaveDialog(JobMonitor.this);
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
                    choice = JOptionPane.showOptionDialog(JobMonitor.this,
                        "Output file exists. Overwrite?",
                        "Job Monitor - Confirm Save",
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
              JOptionPane.showOptionDialog(JobMonitor.this, e.getMessage(),
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

  void setMenuCredentialChangeVOItemEnabled(boolean bool) {
    jMenuItemChangeVO.setEnabled(bool);
  }

  void resetMonitor() {
    jobTableModel.removeAllRows();
    if (multipleJobPanel != null) {
      multipleJobPanel.jobTableModel.removeAllRows();
      multipleJobPanel.clearJobCollection();
      if (MultipleJobPanel.isSingleJobDialogShown) {
        MultipleJobPanel.singleJobDialog.dispose();
        multipleJobPanel.setIsSingleJobDialogShown(false);
      }
      if (MultipleJobPanel.isLogInfoJDialogShown) {
        MultipleJobPanel.logInfoJDialog.dispose();
        multipleJobPanel.setIsLogInfoJDialogShown(false);
      }
      multipleJobPanel.stopUpdateThread();
      multipleJobPanel.stopUpdateThread();
      setMenuBar(JOB_MONITOR_MAIN);
      displayJPanelMain();
    }
  }

  Map loadUserQueriesFromFile() {
    Map userQueryMap = new HashMap();
    File userPrefFile = new File(GUIFileSystem.getUserPrefFile());
    Ad userPrefAd = new Ad();
    if (userPrefFile.isFile()) {
      try {
        userPrefAd.fromFile(userPrefFile.toString());
        logger.debug("userPrefAd: " + userPrefAd);
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        return userQueryMap;
      }
      Ad userPrefJobMonitorAd = new Ad();
      try {
        if (userPrefAd.hasAttribute(Utils.PREF_FILE_JOB_MONITOR)) {
          userPrefJobMonitorAd = userPrefAd.getAd(Utils.PREF_FILE_JOB_MONITOR);
          logger.debug("userPrefJobMonitorAd: " + userPrefJobMonitorAd);
          if (userPrefJobMonitorAd.hasAttribute(Utils.USER_QUERIES)) {
            Vector queriesAdVector = userPrefJobMonitorAd
                .getAdValue(Utils.USER_QUERIES);
            Ad queryAd = new Ad();
            String queryName = "";
            for (int i = 0; i < queriesAdVector.size(); i++) {
              queryAd = (Ad) queriesAdVector.get(i);
              logger.debug("queriesAd: " + queryAd);
              queryName = queryAd.getStringValue(Utils.QUERY_NAME).get(0)
                  .toString().trim();
              userQueryMap.put(queryName, queryAd);
            }
          }
        }
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
    }
    return userQueryMap;
  }

  protected void setLBQueryMenuItems(Map userQueryMap) {
    logger.debug("---- userQueryMap: " + userQueryMap);
    this.userQueryMap = userQueryMap;
    jMenuItemGetJobIdMatchingQuery.removeAll();
    userQueryAvailabilityMap.clear();
    int userQueryMapSize = userQueryMap.size();
    Iterator iterator = userQueryMap.keySet().iterator();
    String queryName = "";
    String lbAddress = "";
    Ad queryAd = new Ad();
    JMenuItem jMenuItem = null;
    ActionListener alst = null;
    int notEnabledCount = 0;
    while (iterator.hasNext()) {
      try {
        queryAd = (Ad) userQueryMap.get(iterator.next());
        queryName = queryAd.getStringValue(Utils.QUERY_NAME).get(0).toString();
        lbAddress = queryAd.getStringValue(Utils.LB_ADDRESS).get(0).toString();
        userQueryAvailabilityMap.put(queryName, lbAddress);
        logger.debug("lbAddress: " + lbAddress);
        jMenuItem = new JMenuItem(queryName);
        alst = new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jMenuLBGetMatchingQueryJobId(e);
          }
        };
        jMenuItem.addActionListener(alst);
        if (!lbAddress.equals(QueryPanel.ALL_LB_SERVERS)
            && !this.menuLBVector.contains(lbAddress)) {
          jMenuItem.setEnabled(false);
          notEnabledCount++;
        }
        jMenuItemGetJobIdMatchingQuery.add(jMenuItem);
        if (notEnabledCount == userQueryMap.size()) {
          jMenuItemGetJobIdMatchingQuery.setEnabled(false);
        } else {
          jMenuItemGetJobIdMatchingQuery.setEnabled(true);
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        continue;
      }
    }
    if ((this.menuLBVector.size() == 0) || (this.userQueryMap.size() == 0)
        || (notEnabledCount == this.userQueryMap.size())) {
      jMenuItemGetJobIdMatchingQuery.setEnabled(false);
    } else {
      jMenuItemGetJobIdMatchingQuery.setEnabled(true);
    }
  }

  protected void addLBQueryMenuItem(String item, String lbAddress) {
    JMenuItem jMenuItem = new JMenuItem(item);
    ActionListener alst = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jMenuLBGetMatchingQueryJobId(e);
      }
    };
    jMenuItem.addActionListener(alst);
    if (!this.menuLBVector.contains(lbAddress)) {
      jMenuItem.setEnabled(false);
    }
    jMenuItemGetJobIdMatchingQuery.add(jMenuItem);
  }

  JobMonitorPreferences getJobMonitorPreferencesReference() {
    return jobMonitorPreferences;
  }

  /*
   *********************
   INTERFACE METHODS
   *********************
   */
  public void displayMultipleJobPanel() {
    this.getContentPane().remove(jPanelMain);
    this.getContentPane().add(multipleJobPanel, BorderLayout.CENTER);
    multipleJobPanel.setVisible(true);
    jPanelMain.setVisible(false);
    validate();
    repaint();
  }

  public void displayJPanelMain() {
    this.getContentPane().remove(multipleJobPanel);
    this.getContentPane().add(jPanelMain, BorderLayout.CENTER);
    jPanelMain.setVisible(true);
    multipleJobPanel.setVisible(false);
    validate();
    repaint();
  }
  // END INTERFACE METHODS
}
