/*
 * GUIUserCredentials.java
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
import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.FocusEvent;
import java.awt.event.WindowEvent;
import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.URL;
import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.SwingConstants;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.glite.jdl.Ad;
import org.glite.jdl.JobAdException;
import org.glite.wmsui.apij.Api;
import org.glite.wmsui.apij.UserCredential;
import org.globus.gsi.GlobusCredentialException;


/**
 * Implementation of the GUIUserCredentials class.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class GUIUserCredentials extends JDialog {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;
  boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  // Reference to GUI Thread.
  static GUIThread guiThreadReference;

  static final int DEFAULT_MODE = 0;
  static final int INFO_MODE = 1;
  static final int CREATION_MODE = 2;
  static final int CHANGE_VO_MODE = 3;

  private String title = "";
  private int mode = DEFAULT_MODE;

  JobSubmitter jobSubmitterJFrame;
  JobMonitor jobMonitorJFrame;
  Component component;

  JLabel jLabelCertFile = new JLabel();
  JTextField jTextFieldCertFile = new JTextField();
  JLabel jLabelCertKey = new JLabel();
  JLabel jLabelProxyFile = new JLabel();
  JLabel jLabelTrustedCertDir = new JLabel();
  JTextField jTextFieldCertKey = new JTextField();
  JTextField jTextFieldProxyFile = new JTextField();
  JTextField jTextFieldTrustedCert = new JTextField();
  JLabel jLabelSubject = new JLabel();
  JTextPane jTextPaneSubject = new JTextPane();
  JButton jButtonOk = new JButton();
  JButton jButtonCertFileChooser = new JButton();
  JButton jButtonCertKeyChooser = new JButton();
  JButton jButtonCertDirChooser = new JButton();
  JButton jButtonProxyFileChooser = new JButton();
  JPanel jPanelCredential = new JPanel();
  JLabel jLabelProxyLifetime = new JLabel();
  JTextField jTextFieldProxyLifetime = new JTextField();
  JLabel jLabelKeyLength = new JLabel();
  JTextField jTextFieldKeyLength = new JTextField();
  JLabel jLabelType = new JLabel();
  JTextField jTextFieldType = new JTextField();
  JLabel jLabelhhmmss = new JLabel();
  JLabel jLabelBits = new JLabel();
  JTextField jTextFieldUnixUser = new JTextField();
  JLabel jLabelUnixUser = new JLabel();
  JButton jButtonDefault = new JButton();
  JLabel jLabelVO = new JLabel();
  JTextField jTextFieldVO = new JTextField();
  JComboBox jComboBoxVO = new JComboBox();
  JButton jButtonExit = new JButton();
  JLabel jLabelVomsExtension = new JLabel();
  JTextField jTextFieldVomsExtension = new JTextField();
  JScrollPane jScrollPaneSubject = new JScrollPane();
  JPanel jPanelButton = new JPanel();

  /**
   * Constructor.
   **/
  public GUIUserCredentials(JobSubmitter jobSubmitter) {
    this(jobSubmitter, DEFAULT_MODE);
  }

  /**
   * Constructor.
   **/
  public GUIUserCredentials(JobMonitor jobMonitor) {
    this(jobMonitor, DEFAULT_MODE);
  }

  /**
   * Constructor.
   **/
  public GUIUserCredentials(JobSubmitter jobSubmitter, int mode) {
    super((JFrame) jobSubmitter);
    this.mode = mode;
    this.component = jobSubmitter;
    jobSubmitterJFrame = jobSubmitter;
    this.title = "Job Monitor - Credentials";
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit();
    } catch (Exception e) {
      e.printStackTrace();
    }
  }

  /**
   * Constructor.
   **/
  public GUIUserCredentials(JobMonitor jobMonitor, int mode) {
    super((JFrame) jobMonitor);
    this.mode = mode;
    this.component = jobMonitor;
    jobMonitorJFrame = jobMonitor;
    this.title = "Job Monitor - Credentials";
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit();
    } catch (Exception e) {
      e.printStackTrace();
    }
  }

  private void jbInit() throws Exception {
    if (this.mode == DEFAULT_MODE) {
      String log4JConfFile = GUIFileSystem.getLog4JConfFile();
      File file = new File(log4JConfFile);
      if (file.isFile()) {
        PropertyConfigurator.configure(log4JConfFile);
      } else {
        logger.setLevel(Level.FATAL);
        Logger.getRootLogger().setLevel(Level.FATAL);
      }
      isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG)
          ? true : false;

      loadUIConfFile();
      setEnvironmentVariables();
    } else {
      isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG)
          ? true : false;
    }

    jLabelCertFile.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelCertFile.setText("Cert File Path");
    jLabelCertFile.setBounds(new Rectangle(12, 43, 101, 14));
    jTextFieldCertFile.setText("");
    jTextFieldCertFile.setBounds(new Rectangle(115, 40, 381, 20));
    jTextFieldCertFile.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(e);
      }
    });
    jLabelCertKey.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelCertKey.setText("Cert Key Path");
    jLabelCertKey.setBounds(new Rectangle(12, 68, 101, 14));
    jLabelProxyFile.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelProxyFile.setText("Proxy File Path");
    jLabelProxyFile.setBounds(new Rectangle(8, 62, 101, 20));
    jLabelTrustedCertDir.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelTrustedCertDir.setText("Trusted Cert Dir");
    jLabelTrustedCertDir.setBounds(new Rectangle(8, 37, 101, 20));
    jTextFieldCertKey.setText("");
    jTextFieldCertKey.setBounds(new Rectangle(115, 65, 381, 20));
    jTextFieldCertKey.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(e);
      }
    });
    jTextFieldProxyFile.setText("");
    jTextFieldProxyFile.setBounds(new Rectangle(112, 62, 373, 20));
    jTextFieldProxyFile.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jTextFieldProxyFileFocusLost(e);
      }
    });
    jTextFieldTrustedCert.setText("");
    jTextFieldTrustedCert.setBounds(new Rectangle(112, 37, 373, 20));
    jTextFieldTrustedCert.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(e);
      }
    });
    jLabelSubject.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelSubject.setText("Subject");
    jLabelSubject.setBounds(new Rectangle(7, 128, 101, 14));
    jTextPaneSubject.setText("");
    jTextPaneSubject.setEditable(false);
    jTextPaneSubject.setBackground(Color.white);
    jTextPaneSubject.setBorder(null);
    jTextPaneSubject.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jTextPaneSubject.select(0, 0);
      }
    });
    jButtonOk.setText("   Ok   ");
    java.awt.event.ActionListener action = new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        switch (GUIUserCredentials.this.mode) {
          case DEFAULT_MODE:
          case CHANGE_VO_MODE:
            jButtonOkEvent(e);
            break;
          case INFO_MODE:
          case CREATION_MODE:
            jButtonOkEventSetMode(e);
            break;
          default:
            break;
        }
      }
    };
    jButtonOk.addActionListener(action);
    jButtonDefault.setText("Default");
    jButtonDefault.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonDefaultEvent(e);
      }
    });

    URL fileOpenGifUrl = JobDef1Panel.class.getResource(Utils.ICON_FILE_OPEN);
    if (fileOpenGifUrl != null) {
      jButtonCertFileChooser.setIcon(new ImageIcon(fileOpenGifUrl));
      jButtonCertKeyChooser.setIcon(new ImageIcon(fileOpenGifUrl));
      jButtonCertDirChooser.setIcon(new ImageIcon(fileOpenGifUrl));
      jButtonProxyFileChooser.setIcon(new ImageIcon(fileOpenGifUrl));
    } else {
      jButtonCertFileChooser.setText("...");
      jButtonCertKeyChooser.setText("...");
      jButtonCertDirChooser.setText("...");
      jButtonProxyFileChooser.setText("...");
    }

    jButtonCertFileChooser.setBounds(new Rectangle(494, 40, 22, 20));
    jButtonCertFileChooser.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        String[] filterExtensions = {"PEM"};
        jButtonFileChooserEvent(jTextFieldCertFile, "Cert File Path Selection",
            filterExtensions, ".pem");
      }
    });
    jButtonCertKeyChooser.setBounds(new Rectangle(494, 65, 22, 20));
    jButtonCertKeyChooser.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        String[] filterExtensions = {"PEM"};
        jButtonFileChooserEvent(jTextFieldCertKey, "Cert Key Path Selection",
            filterExtensions, ".pem");
      }
    });
    jButtonCertDirChooser.setBounds(new Rectangle(483, 37, 22, 20));
    jButtonCertDirChooser.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonFileCertDirEvent();
      }
    });
    jButtonProxyFileChooser.setBounds(new Rectangle(483, 62, 22, 20));
    jButtonProxyFileChooser.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonFileProxyFileEvent();
      }
    });
    jPanelCredential.setBorder(BorderFactory.createEtchedBorder());
    jPanelCredential.setBounds(new Rectangle(6, 7, 517, 215));
    jPanelCredential.setLayout(null);
    jLabelProxyLifetime.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelProxyLifetime.setText("Proxy Time Left");
    jLabelProxyLifetime.setBounds(new Rectangle(7, 184, 101, 20));
    jTextFieldProxyLifetime.setText("");
    jTextFieldProxyLifetime.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldProxyLifetime.setBounds(new Rectangle(112, 184, 74, 20));
    jTextFieldProxyLifetime.setEditable(false);
    jTextFieldProxyLifetime.setBackground(Color.white);
    jTextFieldProxyLifetime.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(e);
      }
    });
    jLabelKeyLength.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelKeyLength.setText("Key Length");
    jLabelKeyLength.setBounds(new Rectangle(266, 184, 72, 20));
    jTextFieldKeyLength.setText("");
    jTextFieldKeyLength.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldKeyLength.setBounds(new Rectangle(339, 184, 41, 20));
    jTextFieldKeyLength.setEditable(false);
    jTextFieldKeyLength.setBackground(Color.white);
    jTextFieldKeyLength.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(e);
      }
    });
    jLabelType.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelType.setText("Type");
    jLabelType.setBounds(new Rectangle(419, 184, 35, 20));
    jTextFieldType.setText("");
    jTextFieldType.setBounds(new Rectangle(457, 184, 48, 20));
    jTextFieldType.setEditable(false);
    jTextFieldType.setBackground(Color.white);
    jTextFieldType.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(e);
      }
    });
    jLabelhhmmss.setText("(hh:mm:ss)");
    jLabelhhmmss.setBounds(new Rectangle(189, 184, 74, 20));
    jLabelBits.setText("(bits)");
    jLabelBits.setBounds(new Rectangle(383, 184, 40, 20));
    jTextFieldUnixUser.setText("");
    jTextFieldUnixUser.setBounds(new Rectangle(112, 11, 185, 20));
    jTextFieldUnixUser.setEditable(false);
    jTextFieldUnixUser.setBackground(Color.white);
    jTextFieldUnixUser.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(e);
      }
    });
    jLabelUnixUser.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelUnixUser.setText("User");
    jLabelUnixUser.setBounds(new Rectangle(8, 11, 101, 20));
    jLabelVO.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelVO.setText("Virtual Org");
    jLabelVO.setBounds(new Rectangle(7, 98, 101, 23));
    jTextFieldVO.setText("");
    jTextFieldVO.setBounds(new Rectangle(112, 99, 185, 20));
    jTextFieldVO.setVisible(false);
    jTextFieldVO.setEditable(false);
    jTextFieldVO.setBackground(Color.white);
    jTextFieldVO.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(e);
      }
    });
    jComboBoxVO.setEditable(true);
    jComboBoxVO.setBounds(new Rectangle(112, 98, 185, 23));
    jButtonExit.setText(" Exit ");
    jButtonExit.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        exit();
      }
    });
    jLabelVomsExtension.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelVomsExtension.setText("VOMS Extension Present");
    jLabelVomsExtension.setBounds(new Rectangle(301, 98, 151, 23));
    jTextFieldVomsExtension.setBackground(Color.white);
    jTextFieldVomsExtension.setFont(new java.awt.Font("Dialog", 1, 12));
    jTextFieldVomsExtension.setEditable(false);
    jTextFieldVomsExtension.setText("");
    jTextFieldVomsExtension.setHorizontalAlignment(SwingConstants.LEADING);
    jTextFieldVomsExtension.setBounds(new Rectangle(457, 99, 48, 20));
    jTextFieldVomsExtension.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(e);
      }
    });
    jScrollPaneSubject.setBounds(new Rectangle(112, 128, 393, 49));
    jPanelCredential.add(jTextFieldProxyFile, null);
    jPanelCredential.add(jScrollPaneSubject, null);
    jPanelCredential.add(jTextFieldType, null);
    jPanelCredential.add(jTextFieldProxyFile, null);
    jPanelCredential.add(jButtonProxyFileChooser, null);
    jPanelCredential.add(jLabelTrustedCertDir, null);
    jPanelCredential.add(jButtonCertDirChooser, null);
    jPanelCredential.add(jTextFieldUnixUser, null);
    jPanelCredential.add(jTextFieldTrustedCert, null);
    jPanelCredential.add(jLabelUnixUser, null);
    jPanelCredential.add(jLabelProxyFile, null);
    jPanelCredential.add(jLabelVO, null);
    jPanelCredential.add(jComboBoxVO, null);
    jPanelCredential.add(jLabelVomsExtension, null);
    jPanelCredential.add(jTextFieldVomsExtension, null);
    jPanelCredential.add(jLabelSubject, null);
    jPanelCredential.add(jLabelProxyLifetime, null);
    jPanelCredential.add(jTextFieldProxyLifetime, null);
    jPanelCredential.add(jLabelType, null);
    jPanelCredential.add(jTextFieldVO, null);
    jPanelCredential.add(jLabelBits, null);
    jPanelCredential.add(jLabelKeyLength, null);
    jPanelCredential.add(jTextFieldKeyLength, null);
    jPanelCredential.add(jLabelhhmmss, null);
    jScrollPaneSubject.getViewport().add(jTextPaneSubject, null);
    jPanelButton.setLayout(new BoxLayout(jPanelButton, BoxLayout.X_AXIS));
    jPanelButton.setBorder(GraphicUtils.SPACING_BORDER);
    jPanelButton.add(jButtonExit);
    jPanelButton.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelButton.add(Box.createGlue());
    jPanelButton.add(jButtonDefault);
    jPanelButton.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
    jPanelButton.add(jButtonOk);

    this.setTitle(this.title);
    this.setSize(new Dimension(525, 285));
    this.setResizable(false);
    this.getContentPane().setLayout(new BorderLayout());
    this.getContentPane().add(jPanelCredential, BorderLayout.CENTER);
    this.getContentPane().add(jPanelButton, BorderLayout.SOUTH);

    switch (this.mode) {
      case INFO_MODE:
        setMenuGUIUserCredentials();
        setInfoMode();
        break;
      case CREATION_MODE:
        setMenuGUIUserCredentials();
        setCreationMode();
        break;
      case DEFAULT_MODE:
        setGUIUserCredentials();
        break;
      case CHANGE_VO_MODE:
        setMenuGUIUserCredentials();
        // VO setChangeVOMode();
        setSelectMode(); // VO
      default:
        break;
    }

  }

  /**
   * Creates user temporary file directory
   */
  protected void createUserTemporaryFileDirectory() {
    for (int i = 0; i < Utils.preferencesTypes.length; i++) {
      File directory = new File(GUIFileSystem.getUserHomeDirectory()
          + File.separator + GUIFileSystem.getTemporaryFileDirectory()
          + File.separator + Utils.preferencesTypes[i]);
      if (!directory.isDirectory()) {
        boolean isCreated = directory.mkdirs(); // Create also non-existent ancestor directories.
        if (!isCreated) {
          JOptionPane.showOptionDialog(GUIUserCredentials.this,
              Utils.FATAL_ERROR
              + "some problems occures creating GUI temporary directory"
              + "\nApplication will be terminated",
              Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE,
              null, null, null);
          System.exit( -1);
        }
      }
      directory = new File(GUIFileSystem.getTemporaryCopyFileDirectory());
      if (!directory.isDirectory()) {
        boolean isCreated = directory.mkdirs(); // Create also non-existent ancestor directories.
        if (!isCreated) {
          JOptionPane.showOptionDialog(GUIUserCredentials.this,
              Utils.FATAL_ERROR
              + "some problems occures creating GUI temporary directory"
              + "\nApplication will be terminated",
              Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE,
              null, null, null);
          System.exit( -1);
        }
      }
    }
  }

  protected void processWindowEvent(WindowEvent e) {
    super.processWindowEvent(e);
    this.setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
    if (e.getID() == WindowEvent.WINDOW_CLOSING) {
      exit();
    }
  }

  protected void exit() {
    if (this.mode != DEFAULT_MODE) {
      this.dispose();
    } else {
      int choice = JOptionPane.showOptionDialog(GUIUserCredentials.this,
          "Do you really want to exit?",
          "Confirm Exit",
          JOptionPane.YES_NO_OPTION,
          JOptionPane.QUESTION_MESSAGE,
          null, null, null);
      if (choice == 0) {
        System.exit(0);
      }
    }
  }

  protected void setMenuGUIUserCredentials() {
    jTextFieldProxyFile.setText(GUIGlobalVars.proxyFilePath);
    jTextFieldCertFile.setText(GUIGlobalVars.certFilePath);
    jTextFieldCertKey.setText(GUIGlobalVars.certKeyPath);
    jTextFieldTrustedCert.setText(GUIGlobalVars.trustedCertDir);
    setGUIUserCredentials(new File(GUIGlobalVars.proxyFilePath));
  }

  public boolean setGUIUserCredentials() {
    jTextFieldUnixUser.setText(System.getProperty("user.name"));
    String envVOConfFile = Api.getEnv(Utils.EDG_WL_GUI_CONFIG_VO);
    boolean loadVOFromProxy = false;
    boolean credentialError = false;
    Vector voVector = new Vector();

    String userProxy = UserCredential.getDefaultProxyName();
    logger.debug("UserCredential.getDefaultProxyName(): "
        + UserCredential.getDefaultProxyName());
    if (!userProxy.equals("")) {
      jTextFieldProxyFile.setText(userProxy);
    }

    if ((envVOConfFile != null) && !envVOConfFile.trim().equals("")) {
      if (new File(envVOConfFile).isFile()) {
        jComboBoxVO.setVisible(false);
        jTextFieldVO.setVisible(true);
        Ad confFileAd = new Ad();
        try {
          confFileAd.fromFile(envVOConfFile);
          if (confFileAd.hasAttribute(Utils.CONF_FILE_VIRTUAL_ORGANISATION)) {
            String virtualOrganisation = confFileAd.getStringValue(Utils.
                CONF_FILE_VIRTUAL_ORGANISATION).get(0).toString().trim();
            if (!virtualOrganisation.equals("")) {
              jTextFieldVO.setText(virtualOrganisation);

              UserCredential userCredential = new UserCredential();
              if (userCredential.hasVOMSExtension() &&
                  !userCredential.containsVO(virtualOrganisation)) {
                JOptionPane.showOptionDialog(GUIUserCredentials.this,
                    "Unable to find configuration file Virtual Organisation '"
                    + virtualOrganisation + "'"
                    + "\ninside VOMS proxy certificate"
                    + "\nConfiguration file: " + envVOConfFile
                    + "\nEnvironment variable: " + Utils.EDG_WL_GUI_CONFIG_VO
                    + "\nApplication will be terminated",
                    Utils.ERROR_MSG_TXT,
                    JOptionPane.DEFAULT_OPTION,
                    JOptionPane.ERROR_MESSAGE,
                    null, null, null);
                System.exit( -1);
              }
              GUIGlobalVars.envVOConfFile = envVOConfFile;
            } else {
              JOptionPane.showOptionDialog(GUIUserCredentials.this,
                  "'" + Utils.CONF_FILE_VIRTUAL_ORGANISATION
                  + "' attribute cannot be blank"
                  + "\nConfiguration file: " + envVOConfFile
                  + "\nEnvironment variable: " + Utils.EDG_WL_GUI_CONFIG_VO
                  + "\nApplication will be terminated",
                  Utils.ERROR_MSG_TXT,
                  JOptionPane.DEFAULT_OPTION,
                  JOptionPane.ERROR_MESSAGE,
                  null, null, null);
              System.exit( -1);
            }
          } else {
            JOptionPane.showOptionDialog(GUIUserCredentials.this,
                "Unable to find mandatory attribute '"
                + Utils.CONF_FILE_VIRTUAL_ORGANISATION + "'"
                + "\nConfiguration file: " + envVOConfFile
                + "\nEnvironment variable: " + Utils.EDG_WL_GUI_CONFIG_VO
                + "\nApplication will be terminated",
                Utils.ERROR_MSG_TXT,
                JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE,
                null, null, null);
            System.exit( -1);
          }
        } catch (java.text.ParseException pe) {
          JOptionPane.showOptionDialog(GUIUserCredentials.this,
              pe.getMessage()
              + "\n\nConfiguration file: " + envVOConfFile
              + "\nEnvironment variable: " + Utils.EDG_WL_GUI_CONFIG_VO
              + "\nApplication will be terminated",
              Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE,
              null, null, null);
          System.exit( -1);
        } catch (JobAdException jae) {
          JOptionPane.showOptionDialog(GUIUserCredentials.this,
              jae.getMessage()
              + "\n\nConfiguration file: " + envVOConfFile
              + "\nEnvironment variable: " + Utils.EDG_WL_GUI_CONFIG_VO
              + "\nApplication will be terminated",
              Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE,
              null, null, null);
          System.exit( -1);
        } catch (Exception e) {
          JOptionPane.showOptionDialog(GUIUserCredentials.this,
              "Some problems occures reading Virtual Organisation "
              + "configuration file: " + envVOConfFile
              + "\nEnvironment variable: " + Utils.EDG_WL_GUI_CONFIG_VO
              + "\nApplication will be terminated",
              Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE,
              null, null, null);
          System.exit( -1);
        }
      } else {
        JOptionPane.showOptionDialog(GUIUserCredentials.this,
            "Unable to find Virtual Organisation configuration file: "
            + envVOConfFile
            + "\nEnvironment variable: " + Utils.EDG_WL_GUI_CONFIG_VO
            + "\nApplication will be terminated",
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        System.exit( -1);
      }
    } else {
      loadVOFromProxy = true;
    }

    String credentialErrorMsg = "";
    try {
      String certFile = UserCredential.getDefaultCert().trim();
      if (!certFile.equals("")) {
        jTextFieldCertFile.setText(certFile);
      }
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }

    try {
      String certKey = UserCredential.getDefaultKey().trim();
      if (!certKey.equals("")) {
        jTextFieldCertKey.setText(certKey);
      }
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }

    try {
      String trustedCert = UserCredential.getDefaultDir().trim();
      if (!trustedCert.equals("")) {
        jTextFieldTrustedCert.setText(trustedCert);
      }
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      credentialErrorMsg += "- " + e.getMessage() + "\n";
      credentialError = true;
    }

    credentialErrorMsg = credentialErrorMsg.trim();
    if (!credentialErrorMsg.equals("")) {
      JOptionPane.showOptionDialog(GUIUserCredentials.this,
          credentialErrorMsg,
          Utils.WARNING_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.WARNING_MESSAGE,
          null, null, null);
    }

    UserCredential userCredential = null;
    try {
      userCredential = new UserCredential();

      int timeLeft = 0;
      timeLeft = userCredential.getTimeLeft();
      if (timeLeft <= 0) {
        jTextFieldProxyLifetime.setForeground(Color.red);
        jTextFieldProxyLifetime.setText("Expired");
        credentialError = true;
      } else {
        jTextFieldProxyLifetime.setForeground(Color.black);
        jTextFieldProxyLifetime.setText(Utils.secondsToTime(timeLeft));
      }

      String subject = userCredential.getX500UserSubject();
      if (!subject.equals("")) {
        jTextPaneSubject.setText(subject);
      }

      jTextFieldKeyLength.setText(Integer.toString(userCredential.getStrenght()));

      if (userCredential.getCredType()) {
        jTextFieldType.setText("Full");
      } else {
        jTextFieldType.setText("Limited");
      }

    } catch (GlobusCredentialException gpe) {
      if (isDebugging) {
        gpe.printStackTrace();
      }
      jTextPaneSubject.setText("");
      jTextFieldKeyLength.setText("");
      jTextFieldProxyLifetime.setText("");
      jTextFieldType.setText("");

      JOptionPane.showOptionDialog(GUIUserCredentials.this,
          gpe.getMessage()
          + "\nYou must select or create a valid Proxy "
          + "certificate before proceeding",
          Utils.WARNING_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.WARNING_MESSAGE,
          null, null, null);
      credentialError = true;
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      JOptionPane.showOptionDialog(GUIUserCredentials.this,
          e.getMessage(),
          Utils.WARNING_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.WARNING_MESSAGE,
          null, null, null);
      credentialError = true;
    }
    if (loadVOFromProxy) {
      if (this.mode == DEFAULT_MODE) {
        Vector voProxyVector = new Vector();
        if (userCredential != null) {
          try {
            // VO voProxyVector = userCredential.getVONames();
            // VO
            if (!userCredential.getDefaultVOName().equals("")) {
              voProxyVector.add(userCredential.getDefaultVOName());
            }
            // VO
          } catch (Exception e) {
          // Do nothing. voProxyVector.size() == 0.
          }
        }
        if (voProxyVector.size() != 0) {
          voVector = (Vector) voProxyVector.clone();
          logger.debug("voProxyVector: " + voProxyVector);
        } else {
          voVector = GUIFileSystem.getVirtualOrganisations();
        }
        setVOComboBoxItems(voVector);
      }
    } else {
      voVector = GUIFileSystem.getVirtualOrganisations();
      setVOComboBoxItems(voVector);
    }

    try {
      if (userCredential != null) {
        if (userCredential.hasVOMSExtension()) {
          jTextFieldVomsExtension.setText("True");
          GUIGlobalVars.hasVOMSExtension = true;
          jComboBoxVO.setEditable(false);
        } else {
          jTextFieldVomsExtension.setText("False");
          GUIGlobalVars.hasVOMSExtension = false;
          jComboBoxVO.setEditable(true);
        }
      } else {
        jTextFieldVomsExtension.setText("");
        //GUIGlobalVars.hasVOMSExtension = false;
        //jComboBoxVO.setEditable(true);
      }
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      jTextFieldVomsExtension.setText("");
      //GUIGlobalVars.hasVOMSExtension = false;
      //jComboBoxVO.setEditable(true);
    }

    if (credentialError) {
      return false;
    }
    return true;
  }

  /**
   * Sets user credential with the information contained in the input proxy
   * certificate
   *
   * @param proxyFile input proxy certificate
   */
  protected void setGUIUserCredentials(File proxyFile) {
    jTextFieldUnixUser.setText(System.getProperty("user.name"));
    if (this.mode == DEFAULT_MODE) {
      jComboBoxVO.removeAllItems();
    }
    jTextPaneSubject.setText("");
    jTextFieldKeyLength.setText("");
    jTextFieldProxyLifetime.setText("");
    jTextFieldType.setText("");

    UserCredential userCredential = null;
    try {
      if (proxyFile != null) {
        userCredential = new UserCredential(proxyFile);
      } else {
        userCredential = new UserCredential();
      }

      String subject = userCredential.getX500UserSubject();
      if (!subject.equals("")) {
        jTextPaneSubject.setText(subject);
      }

      int timeLeft = userCredential.getTimeLeft();
      if (timeLeft <= 0) {
        jTextFieldProxyLifetime.setForeground(Color.red);
        jTextFieldProxyLifetime.setText("Expired");
      } else {
        jTextFieldProxyLifetime.setForeground(Color.black);
        jTextFieldProxyLifetime.setText(Utils.secondsToTime(timeLeft));
      }

      jTextFieldKeyLength.setText(Integer.toString(userCredential.getStrenght()));

      if (userCredential.getCredType()) {
        jTextFieldType.setText("Full");
      } else {
        jTextFieldType.setText("Limited");
      }

    } catch (GlobusCredentialException gpe) {
      // Selected file is not a valid Proxy file.
      if (isDebugging) {
        gpe.printStackTrace();
      }
      jTextPaneSubject.setText("");
      jTextFieldKeyLength.setText("");
      jTextFieldProxyLifetime.setText("");
      jTextFieldType.setText("");
      JOptionPane.showOptionDialog(GUIUserCredentials.this,
          gpe.getMessage()
          + "\nYou must select or create a valid Proxy "
          + "certificate before proceeding",
          Utils.WARNING_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.WARNING_MESSAGE,
          null, null, null);
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      JOptionPane.showOptionDialog(GUIUserCredentials.this,
          e.getMessage(),
          Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE,
          null, null, null);
    }
    Vector voVector = new Vector();
    if (this.mode == DEFAULT_MODE) {
      if (userCredential != null) {
        try {
          voVector = userCredential.getVONames();
        } catch (Exception e) {
          // Do nothing. voVector.size() == 0.
          if (isDebugging) {
            e.printStackTrace();
          }
        }
      }
      if (voVector.size() == 0) {
        voVector = GUIFileSystem.getVirtualOrganisations();
      }
      setVOComboBoxItems(voVector);
    } else if (this.mode == CHANGE_VO_MODE) {
      if (userCredential != null) {
        try {
          // VO voVector = userCredential.getVONames();
          // VO
          String defVO = userCredential.getDefaultVOName();
          if (!defVO.equals("")) {
            if (voVector.contains(defVO.toLowerCase())) {
              voVector.remove(defVO.toLowerCase());
            }
            if (!voVector.contains(defVO)) {
              voVector.add(defVO);
            }
          }
          // VO
        } catch (Exception e) {
          // Do nothing. voVector.size() == 0.
          if (isDebugging) {
            e.printStackTrace();
          }
        }
      }
      if (voVector.size() != 0) {
      /* VO
       Vector vomsesVOVector = new Vector();
               try {
        vomsesVOVector = CreateProxyPanel.getVOMSVOVector();
               } catch (Exception e) {
        // Do nothing. vomsesVOVector.size() == 0.
               }
               String vomsesVO = "";
               for (int i = 0; i < vomsesVOVector.size(); i++) {
        vomsesVO = vomsesVOVector.get(i).toString();
        logger.debug("vomsesVO:" + vomsesVO);
        if (!voVector.contains(vomsesVO)) {
          voVector.add(vomsesVO);
        }
               }
               VO */
      } else {
        voVector = GUIFileSystem.getVirtualOrganisations();
        String virtualOrganisation = GUIGlobalVars.getVirtualOrganisation();
        if (!virtualOrganisation.equals("")) {
          logger.debug("virtualOrganisation: " + virtualOrganisation);
          if (voVector.contains(virtualOrganisation.toLowerCase())) {
            voVector.remove(virtualOrganisation.toLowerCase());
          }
          if (!voVector.contains(virtualOrganisation)) {
            voVector.add(0, virtualOrganisation);
          }
        }
      }
      setVOComboBoxItems(voVector);
    }
    try {
      if (userCredential != null) {
        if (userCredential.hasVOMSExtension()) {
          jTextFieldVomsExtension.setText("True");
          GUIGlobalVars.hasVOMSExtension = true;
          jComboBoxVO.setEditable(false);
        } else {
          jTextFieldVomsExtension.setText("False");
          GUIGlobalVars.hasVOMSExtension = false;
          jComboBoxVO.setEditable(true);
        }
      } else {
        jTextFieldVomsExtension.setText("");
        //GUIGlobalVars.hasVOMSExtension = false;
        //jComboBoxVO.setEditable(true);
      }
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      jTextFieldVomsExtension.setText("");
      //GUIGlobalVars.hasVOMSExtension = false;
      //jComboBoxVO.setEditable(true);
    }
  }

  private void jButtonFileChooserEvent(Component component, String title,
      String[] filterExtensions, String filterDescription) {
    JTextField jTextField = new JTextField();
    if (component instanceof JTextField) {
      jTextField = (JTextField) component;
    } else {
      JOptionPane.showOptionDialog(GUIUserCredentials.this,
          Utils.UNESPECTED_ERROR + Utils.WRONG_COMPONENT_ARGUMENT_TYPE,
          Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE,
          null, null, null);
      return;
    }
    JFileChooser fileChooser = new JFileChooser();
    fileChooser.setDialogTitle(title);
    fileChooser.setCurrentDirectory(new File(GUIGlobalVars.
        getFileChooserWorkingDirectory()));
    fileChooser.setFileHidingEnabled(false);
    GUIFileFilter classadFileFilter = new GUIFileFilter(filterDescription,
        filterExtensions);
    fileChooser.addChoosableFileFilter(classadFileFilter);
    int choice = fileChooser.showOpenDialog(GUIUserCredentials.this);
    if (choice != JFileChooser.APPROVE_OPTION) {
      return;
    } else if (!fileChooser.getSelectedFile().isFile()) {
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      JOptionPane.showOptionDialog(GUIUserCredentials.this,
          "Unable to find file: " + selectedFile,
          Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE,
          null, null, null);
      return;
    } else {
      GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser.
          getCurrentDirectory().toString());
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      jTextField.setText(selectedFile);
    }
  }

  private void jButtonFileProxyFileEvent() {
    JFileChooser fileChooser = new JFileChooser();
    fileChooser.setDialogTitle("Proxy File Path Selection");
    fileChooser.setCurrentDirectory(new File(GUIGlobalVars.
        getFileChooserWorkingDirectory()));
    fileChooser.setFileHidingEnabled(false);
    int choice = fileChooser.showOpenDialog(GUIUserCredentials.this);
    if (choice != JFileChooser.APPROVE_OPTION) {
      return;
    } else if (!fileChooser.getSelectedFile().isFile()) {
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      JOptionPane.showOptionDialog(GUIUserCredentials.this,
          "Unable to find file: " + selectedFile,
          Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE,
          null, null, null);
      return;
    } else {
      GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser.
          getCurrentDirectory().toString());
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      jTextFieldProxyFile.setText(selectedFile);
      setGUIUserCredentials(new File(selectedFile));
    }
  }

  private void jButtonFileCertDirEvent() {
    JFileChooser fileChooser = new JFileChooser();
    fileChooser.setDialogTitle("Trusted Cert Dir Selection");
    fileChooser.setCurrentDirectory(new File(GUIGlobalVars.
        getFileChooserWorkingDirectory()));
    fileChooser.setFileHidingEnabled(false);
    fileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);

    int choice = fileChooser.showOpenDialog(GUIUserCredentials.this);

    if (choice != JFileChooser.APPROVE_OPTION) {
      return;
    } else if (!fileChooser.getSelectedFile().isDirectory()) {
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      JOptionPane.showOptionDialog(GUIUserCredentials.this,
          "Unable to find directory: " + selectedFile,
          Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE,
          null, null, null);
      return;
    } else {
      GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser.
          getCurrentDirectory().toString());
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      jTextFieldTrustedCert.setText(selectedFile);
    }
  }

  protected int jButtonOkEventSetMode(ActionEvent e) {
    String userProxy = jTextFieldProxyFile.getText().trim();
    String userCertDir = jTextFieldTrustedCert.getText().trim();
    if (!userProxy.equals("")) {
      if (!(new File(userCertDir).isDirectory())) {
        int choice =
            JOptionPane.showOptionDialog(GUIUserCredentials.this,
            "Unable to find Trusted Cert Dir: "
            + userCertDir
            + "\nDo you want to try to set default value?",
            Utils.ERROR_MSG_TXT,
            JOptionPane.YES_NO_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        if (choice == 0) {
          try {
            userCertDir = UserCredential.getDefaultDir().trim();
            jTextFieldTrustedCert.setText(userCertDir);
          } catch (Exception ex) {
          // Do nothing, userCertDir will be a blank string, not to bad
          // (it will set to default value).
          }
        }
        jTextFieldTrustedCert.selectAll();
        jTextFieldTrustedCert.grabFocus();
        return Utils.FAILED;
      }
      String subject = "";
      try {
        UserCredential userCredential = new UserCredential(new File(userProxy));
        userCredential.checkProxy();
        subject = userCredential.getSubject().trim();
      } catch (GlobusCredentialException gpe) {
        JOptionPane.showOptionDialog(GUIUserCredentials.this,
            gpe.getMessage(),
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        return Utils.FAILED;
      } catch (Exception ex) {
        JOptionPane.showOptionDialog(GUIUserCredentials.this,
            ex.getMessage(),
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        return Utils.FAILED;
      }
      subject = UserCredential.getX500UserSubject(subject);
      String currentSubject = GUIGlobalVars.proxySubject;

      if (!subject.equals(currentSubject)) {
        JOptionPane.showOptionDialog(GUIUserCredentials.this,
            "Selected Proxy file must have the following Subject:\n"
            + GUIGlobalVars.proxySubject,
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        return Utils.FAILED;
      } else {
        if (this.mode != CHANGE_VO_MODE) {
          UserCredential.setDefaultProxy(userProxy);
          setGUIUserCredentials(new File(userProxy));
          GUIGlobalVars.proxyFilePath = userProxy;
          this.dispose();
        }
      }
    } else {
      JOptionPane.showOptionDialog(GUIUserCredentials.this,
          "Please provide a valid Proxy File Path",
          Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE,
          null, null, null);
      jTextFieldProxyFile.grabFocus();
      return Utils.FAILED;
    }
    return Utils.SUCCESS;
  }

  void jButtonOkEvent(ActionEvent e) { // BOGUS
    if (this.mode == CHANGE_VO_MODE) {
      if (jButtonOkEventSetMode(null) == Utils.FAILED) {
        return;
      }
    }
    String userProxy = jTextFieldProxyFile.getText().trim();
    String userCertDir = jTextFieldTrustedCert.getText().trim();
    String virtualOrganisation = "";
    if (jComboBoxVO.isVisible()) {
      if (jComboBoxVO.isEditable()) {
        virtualOrganisation = jComboBoxVO.getEditor().getItem().toString().trim();
      } else {
        if (jComboBoxVO.getSelectedItem() != null) {
          virtualOrganisation = jComboBoxVO.getSelectedItem().toString().trim();
        }
      }
      if (virtualOrganisation.equals("")) {
        JOptionPane.showOptionDialog(GUIUserCredentials.this,
            "Please provide a Virtual Organisation",
            Utils.WARNING_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE,
            null, null, null);
        if (jComboBoxVO.getItemCount() != 0) {
          jComboBoxVO.showPopup();
        }
        jComboBoxVO.grabFocus();
        return;
      } else {
        logger.debug("virtualOrganisation (user input): " + virtualOrganisation);
        logger.debug("GUIGlobalVars.getVirtualOrganisation(): "
            + GUIGlobalVars.getVirtualOrganisation());

        if ((this.mode == CHANGE_VO_MODE)
            && virtualOrganisation.toLowerCase()
            .equals(GUIGlobalVars.getVirtualOrganisation().toLowerCase())) {
          logger.debug("Same virtual organisation choosen");

          // VO
          try {
            UserCredential userCredential = new UserCredential(new File(
                userProxy));
            UserCredential.setDefaultProxy(userProxy);
            GUIGlobalVars.proxyFilePath = userProxy; // VO
          } catch (GlobusCredentialException gpe) {
            if (isDebugging) {
              gpe.printStackTrace();
            }
            jTextPaneSubject.setText("");
            jTextFieldKeyLength.setText("");
            jTextFieldProxyLifetime.setText("");
            jTextFieldType.setText("");
            JOptionPane.showOptionDialog(GUIUserCredentials.this,
                gpe.getMessage()
                + "\nYou must select or create a valid Proxy "
                + "certificate before proceeding",
                Utils.WARNING_MSG_TXT,
                JOptionPane.DEFAULT_OPTION,
                JOptionPane.WARNING_MESSAGE,
                null, null, null);
            return;
          } catch (Exception ex) {
            if (isDebugging) {
              ex.printStackTrace();
            }
            JOptionPane.showOptionDialog(GUIUserCredentials.this,
                ex.getMessage(),
                Utils.ERROR_MSG_TXT,
                JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE,
                null, null, null);
            return;
          }
          // VO

          this.dispose();
          return;
        }

        String location = GUIFileSystem.getConfFileLocation();
        // Virtual Organisation lower case because the names of the VO
        // directories are all lower case.
        if (!(new File(location +
            virtualOrganisation.toLowerCase())).isDirectory()) {
          int choice = JOptionPane.showOptionDialog(GUIUserCredentials.this,
              "Unable to find configuration file for "
              + "the specified Virtual Organisation"
              + "\nDo you want to continue anyway?",
              Utils.WARNING_MSG_TXT,
              JOptionPane.YES_NO_OPTION,
              JOptionPane.WARNING_MESSAGE,
              null, null, null);
          if (choice != 0) {
            return;
          }
        }
      }
    } else {
      virtualOrganisation = jTextFieldVO.getText().toString().trim();
    }

    if ((this.mode == CHANGE_VO_MODE) && (jobSubmitterJFrame != null)) {
      int choice = 0;
      JobMonitor jobMonitor = jobSubmitterJFrame.jobMonitorJFrame;
      /* VO
             if (((jobMonitor != null) && jobMonitor.isVisible())
          || (GUIGlobalVars.openedEditorHashMap.size() != 0)) {
        choice = JOptionPane.showOptionDialog(GUIUserCredentials.this,
            "Do you really want to change Virtual Organisation?"
            + "\n(The Job Monitor and all JDL Editor(s) will be closed)",
            Utils.WARNING_MSG_TXT,
            JOptionPane.YES_NO_OPTION,
            JOptionPane.WARNING_MESSAGE,
            null, null, null);
        if (choice != 0) {
          return;
        } else {
          // Close monitor and jdl editor(s).
          if ((jobMonitor != null) && jobMonitor.isVisible()) {
            jobMonitor.dispose();
            if (jobMonitor.multipleJobPanel != null) {
              jobMonitor.multipleJobPanel.currentTimeThread.stopThread();
              jobMonitor.multipleJobPanel.updateThread.stopThread();
              jobMonitor.multipleJobPanel.disposeStatusDetails();
              jobMonitor.multipleJobPanel.disposeLogInfo();
            }
          }
          GraphicUtils.closeAllEditorFrames();
        }
             }
             VO */
      String msg =
          "This will change Virtual Organisation, do you really want to continue?";
      if (((jobMonitor != null) && jobMonitor.isVisible())
          && (GUIGlobalVars.openedEditorHashMap.size() != 0)) {
        msg += "\n(The Job Monitor and all JDL Editors will be closed)";
      } else if ((jobMonitor != null) && jobMonitor.isVisible()) {
        msg += "\n(The Job Monitor will be closed)";
      } else if (GUIGlobalVars.openedEditorHashMap.size() != 0) {
        msg += "\n(All JDL Editors will be closed)";
      }
      choice = JOptionPane.showOptionDialog(GUIUserCredentials.this,
          msg,
          Utils.WARNING_MSG_TXT,
          JOptionPane.YES_NO_OPTION,
          JOptionPane.WARNING_MESSAGE,
          null, null, null);
      if (choice != 0) {
        return;
      } else {
        // Close monitor and jdl editor(s).
        if ((jobMonitor != null) && jobMonitor.isVisible()) {
          jobMonitor.dispose();
          if (jobMonitor.multipleJobPanel != null) {
            jobMonitor.multipleJobPanel.currentTimeThread.stopThread();
            jobMonitor.multipleJobPanel.updateThread.stopThread();
            jobMonitor.multipleJobPanel.disposeStatusDetails();
            jobMonitor.multipleJobPanel.disposeLogInfo();
          }
        }
        GraphicUtils.closeAllEditorFrames();
      }
    }

    //!!!GUIGlobalVars.proxyFilePath = userProxy;
    if (!userProxy.equals("")) {
      if (userCertDir.equals("")) {
        JOptionPane.showOptionDialog(GUIUserCredentials.this,
            "Trusted Cert Dir field cannot be blank"
            + "\n(application will try to set default value)",
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        try {
          userCertDir = UserCredential.getDefaultDir().trim();
          jTextFieldTrustedCert.setText(userCertDir);
        } catch (Exception ex) {
        // Do nothing, userCertDir will be a blank string, not to bad
        // (it will set to default value).
        }
        jTextFieldTrustedCert.selectAll();
        jTextFieldTrustedCert.grabFocus();
        return;
      }

      if (!(new File(userCertDir).isDirectory())) {
        int choice = JOptionPane.showOptionDialog(GUIUserCredentials.this,
            "Unable to find Trusted Cert Dir: " + userCertDir
            + "\nDo you want to try to set default value?",
            Utils.ERROR_MSG_TXT,
            JOptionPane.YES_NO_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        if (choice == 0) {
          try {
            userCertDir = UserCredential.getDefaultDir().trim();
            jTextFieldTrustedCert.setText(userCertDir);
            //GUIGlobalVars.currentUserCertDir = userCertDir;
          } catch (Exception ex) {
          // Do nothing, userCertDir will be a blank string, not to bad
          // (it will be set to default value).
          }
        }
        jTextFieldTrustedCert.selectAll();
        jTextFieldTrustedCert.grabFocus();
        return;
      }

      try {
        UserCredential userCredential = new UserCredential(new File(userProxy));
        userCredential.checkProxy();
        UserCredential.setDefaultProxy(userProxy);

        GUIGlobalVars.proxyFilePath = userProxy;
        if (GUIGlobalVars.certFilePath.equals("")) {
          GUIGlobalVars.certFilePath = jTextFieldCertFile.getText().trim();
        }
        if (GUIGlobalVars.certKeyPath.equals("")) {
          GUIGlobalVars.certKeyPath = jTextFieldCertKey.getText().trim();
        }
        if (GUIGlobalVars.trustedCertDir.equals("")) {
          GUIGlobalVars.trustedCertDir = jTextFieldTrustedCert.getText().trim();
        }

        GUIGlobalVars.proxySubject = userCredential.getX500UserSubject();
        String oldVirtualOrganisation = GUIGlobalVars.getVirtualOrganisation();
        logger.debug("oldVirtualOrganisation: " + oldVirtualOrganisation);
        GUIGlobalVars.setVirtualOrganisation(virtualOrganisation);
        logger.debug("newVirtualOrganisation: " +
            GUIGlobalVars.getVirtualOrganisation());

        if (this.mode != CHANGE_VO_MODE) {
          if (jobSubmitterJFrame != null) {
            jobSubmitterJFrame.setTitle(JobSubmitter.TITLE
                + " - Virtual Organisation: " + virtualOrganisation);
          } else if (jobMonitorJFrame != null) {
            jobMonitorJFrame.setTitle(JobMonitor.TITLE
                + " - Virtual Organisation: " + virtualOrganisation);
          }
        }

        GUIGlobalVars.userTemporaryFileDirectory =
            computeUserTempFileDirectoryName();
        createUserTemporaryFileDirectory();

        logger.debug("Mode: " + this.mode);
        logger.debug("Thread verify argument: "
            + computeUserTempFileDirectoryName());
        if (threadVerify(computeUserTempFileDirectoryName())) {
          if (this.mode == CHANGE_VO_MODE) {
            if (userCredential.hasVOMSExtension()) {
              CreateProxyPanel createProxyJPanel = new CreateProxyPanel();
              if (!createProxyJPanel.changeProxyVO(virtualOrganisation,
                  GUIUserCredentials.this)) {
                JOptionPane.showOptionDialog(GUIUserCredentials.this,
                    "Unable to change Virtual Organisation",
                    Utils.ERROR_MSG_TXT,
                    JOptionPane.DEFAULT_OPTION,
                    JOptionPane.ERROR_MESSAGE,
                    null, null, null);
                GUIGlobalVars.setVirtualOrganisation(oldVirtualOrganisation);
                oldVirtualOrganisation = "";
                logger.debug("changeVO failed");
                GUIGlobalVars.userTemporaryFileDirectory =
                    computeUserTempFileDirectoryName();
                return;
              }
            }

            logger.debug("Setting thread temp file dir: "
                + computeUserTempFileDirectoryName());
            String directory = computeUserTempFileDirectoryName();
            GUIGlobalVars.userTemporaryFileDirectory = directory;

            logger.debug("setTemporaryDirectory: " + guiThreadReference);
            logger.debug("directory: " + directory);
            logger.debug("oldVirtualOrganisation: " + oldVirtualOrganisation);

            if (!guiThreadReference.setTemporaryDirectory(directory,
                oldVirtualOrganisation)) {
              GUIGlobalVars.setVirtualOrganisation(oldVirtualOrganisation);
              oldVirtualOrganisation = "";
              GUIGlobalVars.userTemporaryFileDirectory =
                  computeUserTempFileDirectoryName();
              logger.debug("setTemporaryDirectory() failed");
              JOptionPane.showOptionDialog(GUIUserCredentials.this,
                  "Unable to change Virtual Oraganisation",
                  Utils.ERROR_MSG_TXT,
                  JOptionPane.DEFAULT_OPTION,
                  JOptionPane.ERROR_MESSAGE,
                  null, null, null);
              return;
            }
            GUIGlobalVars.selectedJobNameCopyVector.clear();
            GUIGlobalVars.selectedRBPanelCopy = "";
            if (jobSubmitterJFrame != null) {
              jobSubmitterJFrame.setTitle(JobSubmitter.TITLE
                  + " - Virtual Organisation: " + virtualOrganisation);
            } else if (jobMonitorJFrame != null) {
              jobMonitorJFrame.setTitle(JobMonitor.TITLE
                  + " - Virtual Organisation: " + virtualOrganisation);
            }
          }
          this.dispose();

          if (GUIGlobalVars.envVOConfFile.equals("") ||
              this.mode == CHANGE_VO_MODE) { // User haven't set environment variable.
            String location = GUIFileSystem.getConfFileLocation();
            GUIGlobalVars.envVOConfFile = location
                + virtualOrganisation.toLowerCase()
                + File.separator + GUIFileSystem.VO_CONF_FILE_NAME;
          }
          String fileNameVO = GUIGlobalVars.envVOConfFile;

          logger.debug("GUIGlobalVars.envVOConfFile: "
              + GUIGlobalVars.envVOConfFile);

          if (!fileNameVO.equals("")) {
            logger.debug("loadVOSpecificConfFile");
            loadVOSpecificConfFile(fileNameVO);
          } //else {
          //clearVOSpecificConfFileVars();
          //}

          // JOB SUBMITTER
          if (jobSubmitterJFrame != null) {
            jobSubmitterJFrame.show();
            boolean isNSPresent = false;
            Vector nsAddressesVector = GUIGlobalVars.getNSVector();
            Vector nsVectorConfFile = JobSubmitterPreferences.loadPrefFileNS();
            Vector toLeaveNSNamesVector = new Vector();
            if (nsVectorConfFile.size() != 0) {
              for (int i = 0; i < nsVectorConfFile.size(); i++) {
                toLeaveNSNamesVector.add(((NetworkServer) nsVectorConfFile.get(
                    i)).getName());
              }
              jobSubmitterJFrame.setNSTabbedPanePanels(nsVectorConfFile);
              jobSubmitterJFrame.setNSMenuItems(JobSubmitterPreferences.
                  loadPrefFileNSNames());
              isNSPresent = true;

            } else if (nsAddressesVector.size() != 0) {
              Vector nsVector = new Vector();
              Vector nsNameVector = new Vector();
              Vector nsSchemaVector = new Vector();
              String nsName;
              for (int i = 0; i < nsAddressesVector.size(); i++) {
                nsName = Utils.DEFAULT_NS_NAME + Integer.toString(i + 1);
                nsNameVector.add(nsName);
                nsVector.add(new NetworkServer(nsName,
                    nsAddressesVector.get(i).toString(),
                    Utils.DEFAULT_INFORMATION_SCHEMA));
                nsSchemaVector.add(Utils.DEFAULT_INFORMATION_SCHEMA);
                toLeaveNSNamesVector.add(nsName);
              }
              JobSubmitterPreferences.savePrefFileNS(nsNameVector,
                  nsAddressesVector, nsSchemaVector);
              jobSubmitterJFrame.setNSTabbedPanePanels(nsVector);
              jobSubmitterJFrame.setNSMenuItems(nsNameVector);
              isNSPresent = true;
            } else {
              jobSubmitterJFrame.setNSTabbedPanePanels(new Vector());
              jobSubmitterJFrame.setNSMenuItems(new Vector());
            }
            removeNSTempDirectories(toLeaveNSNamesVector);

            boolean isLBPresent = false;
            Map nsLBMapVOConf = Utils.cloneMap(GUIGlobalVars.defaultNSLBMap);
            Map nsLBMap = new HashMap();
            try {
              nsLBMap = JobSubmitterPreferences.loadNSLBMap(new File(
                  GUIFileSystem.
                  getUserPrefFile()));
            } catch (Exception exc) {
              isNSPresent = false;
              JOptionPane.showOptionDialog(jobSubmitterJFrame,
                  exc.getMessage(),
                  Utils.ERROR_MSG_TXT,
                  JOptionPane.DEFAULT_OPTION,
                  JOptionPane.ERROR_MESSAGE,
                  null, null, null);
            }
            logger.debug("nsLBMap: " + nsLBMap);
            if (nsLBMap.size() != 0) {
              logger.info("User Pref file Map: " + nsLBMap);
              Iterator iterator = nsLBMap.keySet().iterator();
              isLBPresent = true;
              while (iterator.hasNext()) {
                Vector vector = (Vector) nsLBMap.get(iterator.next());
                if ((vector.size() == 0)
                    || ((vector.size() == 1) &&
                    vector.get(0).toString().trim().equals(""))) {
                  isLBPresent = false;
                  break;
                }
              }
              GUIGlobalVars.nsLBMap = nsLBMap;
            } else if (nsLBMapVOConf.size() != 0) {
              logger.info("VO conf file Map: " + nsLBMapVOConf);
              Iterator iterator = nsLBMapVOConf.keySet().iterator();
              isLBPresent = true;
              while (iterator.hasNext()) {
                Vector vector = (Vector) nsLBMapVOConf.get(iterator.next());
                if ((vector.size() == 0)
                    || ((vector.size() == 1) &&
                    vector.get(0).toString().trim().equals(""))) {
                  isLBPresent = false;
                  break;
                }
              }
              GUIGlobalVars.nsLBMap = Utils.cloneMap(nsLBMapVOConf);
              JobSubmitterPreferences.saveNSLBMap(nsLBMapVOConf);
            } else {
              GUIGlobalVars.nsLBMap = new HashMap();
              for (int i = 0; i < GUIGlobalVars.getNSVector().size(); i++) {
                GUIGlobalVars.nsLBMap.put(GUIGlobalVars.getNSVector().get(i).
                    toString(), new Vector());
              }
              isLBPresent = false;
            }

            String warningMsg = JobSubmitterPreferences.
                initializeUserConfiguration();
            if (!warningMsg.equals("")) {
              JOptionPane.showOptionDialog(GUIUserCredentials.this,
                  "Some problems occurs reading Requirement, "
                  + "Rank and RankMPI values from configuration files:\n"
                  + warningMsg,
                  Utils.WARNING_MSG_TXT,
                  JOptionPane.DEFAULT_OPTION,
                  JOptionPane.WARNING_MESSAGE,
                  null, null, null);
            }

            jobSubmitterJFrame.loadOldWorkingSession();
            if (!isNSPresent) {
              JOptionPane.showOptionDialog(jobSubmitterJFrame,
                  "Please provide at least a Network Server",
                  Utils.INFORMATION_MSG_TXT,
                  JOptionPane.DEFAULT_OPTION,
                  JOptionPane.INFORMATION_MESSAGE,
                  null, null, null);
              jobSubmitterJFrame.jMenuJobPreferences(false); // Show panel without Cancel button.
            } else if (!isLBPresent) {
              JOptionPane.showOptionDialog(jobSubmitterJFrame,
                  "Please provide at least a Logging & Bookkeeping server"
                  + " for all Network Servers",
                  Utils.INFORMATION_MSG_TXT,
                  JOptionPane.DEFAULT_OPTION,
                  JOptionPane.INFORMATION_MESSAGE,
                  null, null, null);
              jobSubmitterJFrame.jMenuJobPreferences(false); // Show panel without Cancel button.
            }

            // JOB MONITOR
          } else if (jobMonitorJFrame != null) {
            jobMonitorJFrame.show();
            Vector lbVectorVOConf = GUIGlobalVars.getLBVector();
            Vector lbVector = JobMonitorPreferences.loadPrefFileLB();
            if (lbVector.size() != 0) {
              jobMonitorJFrame.setLBMenuItems(lbVector);
              jobMonitorJFrame.jMenuItemGetJobIdFromLB.setEnabled(true);
            } else if (lbVectorVOConf.size() != 0) {
              jobMonitorJFrame.setLBMenuItems(lbVectorVOConf);
              jobMonitorJFrame.jMenuItemGetJobIdFromLB.setEnabled(true);
              JobMonitorPreferences.savePrefFileLB(lbVectorVOConf);
            } else {
              jobMonitorJFrame.setLBMenuItems(new Vector());
              jobMonitorJFrame.jMenuItemGetJobIdFromLB.setEnabled(false);
            }
            jobMonitorJFrame.userQueryMap = jobMonitorJFrame.
                loadUserQueriesFromFile();
            jobMonitorJFrame.setLBQueryMenuItems(jobMonitorJFrame.userQueryMap);

            if (this.mode == CHANGE_VO_MODE) {
              jobMonitorJFrame.resetMonitor();
            }

            JobMonitorPreferences.initializeUserConfiguration();

            // Load from preferences file
            jobMonitorJFrame.loadConf(JobMonitor.
                getJobMonitorRecoveryFile());
          }

        } else {
          if (this.mode == CHANGE_VO_MODE) {
            GUIGlobalVars.setVirtualOrganisation(oldVirtualOrganisation);
            oldVirtualOrganisation = "";
            logger.debug("ThreadVerify failed");
            GUIGlobalVars.userTemporaryFileDirectory =
                computeUserTempFileDirectoryName();
          }
        }
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
        JOptionPane.showOptionDialog(GUIUserCredentials.this,
            ex.getMessage(),
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
      }

    } else {
      JOptionPane.showOptionDialog(GUIUserCredentials.this,
          "Please provide a valid Proxy File Path",
          Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE,
          null, null, null);
      jTextFieldProxyFile.grabFocus();
    }
  }

  protected void clearVOSpecificConfFileVars() {
    GUIGlobalVars.setHLRLocation("");
    GUIGlobalVars.setMyProxyServer("");
    GUIGlobalVars.setNSVector(new Vector());
    GUIGlobalVars.setLBVector(new Vector());
    //!!!
    GUIGlobalVars.nsLBMap = new HashMap();
    GUIGlobalVars.tempNSLBMap = new HashMap();
    GUIGlobalVars.defaultNSLBMap = new HashMap();
  }

  protected int loadVOSpecificConfFile(String fileName) {
    logger.debug("VOSpecificConfFile: " + fileName);
    if ((new File(fileName)).isFile()) {
      Ad confAd = new Ad();
      try {
        confAd.fromFile(fileName);
      } catch (java.text.ParseException pe) {
        JOptionPane.showOptionDialog(this.component,
            "Unable to parse file: " + fileName
            + "\nApplication will try to parse next"
            + " VO configuration file, if any",
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        return -1;
      } catch (JobAdException jae) {
        JOptionPane.showOptionDialog(this.component,
            Utils.UNESPECTED_ERROR + jae.getMessage(),
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        return -1;
      } catch (Exception e) {
        JOptionPane.showOptionDialog(this.component,
            e.getMessage(),
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        return -1;
      }

      if (confAd.hasAttribute(Utils.CONF_FILE_VIRTUAL_ORGANISATION)) {
        try {
          String confFileVO = confAd.getStringValue(Utils.
              CONF_FILE_VIRTUAL_ORGANISATION).get(0).toString().trim();
          if (!confFileVO.toLowerCase().equals(GUIGlobalVars.
              getVirtualOrganisation().toLowerCase())) {
            JOptionPane.showOptionDialog(this.component,
                "VO configuration file reports a different "
                + "Virtual Organisation name"
                + "\nplease check configuration file: " + fileName
                + ",\n'" + Utils.CONF_FILE_VIRTUAL_ORGANISATION
                + "' attribute",
                Utils.WARNING_MSG_TXT,
                JOptionPane.DEFAULT_OPTION,
                JOptionPane.WARNING_MESSAGE,
                null, null, null);
          } else {
            GUIGlobalVars.setVirtualOrganisation(confFileVO);
            if (jobSubmitterJFrame != null) {
              jobSubmitterJFrame.setTitle(JobSubmitter.TITLE
                  + " - Virtual Organisation: " + confFileVO);
            } else if (jobMonitorJFrame != null) {
              jobMonitorJFrame.setTitle(JobMonitor.TITLE
                  + " - Virtual Organisation: " + confFileVO);
            }
          }
        } catch (Exception e) {
          JOptionPane.showOptionDialog(this.component,
              e.getMessage(),
              Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE,
              null, null, null);
        }
      } else {
        JOptionPane.showOptionDialog(this.component,
            "VO configuration file: unable to find mandatory attribute "
            + Utils.CONF_FILE_VIRTUAL_ORGANISATION
            + "\nApplication will be terminated",
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        System.exit( -1);
        //!!! check! in case of mode=CHANGE_VO_MODE then a different behaviour is needed.
      }

      try {
        GUIGlobalVars.setHLRLocation(confAd.getStringValue(Utils.
            CONF_FILE_HLRLOCATION).get(0).toString());
      } catch (NoSuchFieldException nsfe) {
        // Attribute not present.
        GUIGlobalVars.setHLRLocation("");
      } catch (Exception e) {
        JOptionPane.showOptionDialog(this.component,
            e.getMessage(),
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
      }

      try {
        GUIGlobalVars.setMyProxyServer(confAd.getStringValue(Utils.
            CONF_FILE_MYPROXYSERVER).get(0).toString());
      } catch (NoSuchFieldException nsfe) {
        // Attribute not present.
        GUIGlobalVars.setMyProxyServer("");
      } catch (Exception e) {
        JOptionPane.showOptionDialog(this.component,
            e.getMessage(),
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
      }

      try {
        GUIGlobalVars.setNSVector(confAd.getStringValue(Utils.
            CONF_FILE_NSADDRESSES));
      } catch (NoSuchFieldException nsfe) {
        GUIGlobalVars.setNSVector(new Vector());
        JOptionPane.showOptionDialog(this.component,
            "VO configuration file: unable to find attribute "
            + Utils.CONF_FILE_NSADDRESSES,
            Utils.WARNING_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.WARNING_MESSAGE,
            null, null, null);

      } catch (Exception e) {
        GUIGlobalVars.setNSVector(new Vector());
        JOptionPane.showOptionDialog(this.component,
            e.getMessage(),
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
      }

      Vector confFileNS = new Vector();
      try {
        confFileNS = confAd.getStringValue(Utils.CONF_FILE_NSADDRESSES);
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        GUIGlobalVars.setLBVector(new Vector());
        JOptionPane.showOptionDialog(this.component,
            e.getMessage(),
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        return -1;
      }

      try {
        Vector confFileLB = confAd.getStringValue(Utils.CONF_FILE_LBADDRESSES);
        Map nsLBMap = JobSubmitterPreferences.yieldNSLBMap(confFileNS,
            confFileLB);
        GUIGlobalVars.nsLBMap = Utils.cloneMap(nsLBMap);
        GUIGlobalVars.defaultNSLBMap = Utils.cloneMap(nsLBMap);

      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        GUIGlobalVars.setLBVector(new Vector());
        JOptionPane.showOptionDialog(this.component,
            e.getMessage(),
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        Map nsLBMap = new HashMap();
        for (int i = 0; i < confFileNS.size(); i++) {
          nsLBMap.put(confFileNS.get(i).toString(), new Vector());
        }
        GUIGlobalVars.nsLBMap = Utils.cloneMap(nsLBMap);
        GUIGlobalVars.defaultNSLBMap = Utils.cloneMap(nsLBMap);
      }
      return 0;
    }
    clearVOSpecificConfFileVars();
    return -1;
  }

  /**
   * Verifies is another instance of the application is running using same
   * proxy subject and Virtual Organisation
   *
   * @param userTemporaryFileDirectory the user temporary file directory used
   *                                   as key to make the check
   * @return true if the verify is all right, false otherwise
   */
  protected boolean threadVerify(String userTemporaryFileDirectory) {
    Socket clientSocket = null;
    PrintWriter out = null;
    BufferedReader in = null;
    try {
      if (!(new File(getThreadConfigurationFilePath()).isFile())) {
        logger.debug("File not found: " + getThreadConfigurationFilePath());
        if (this.mode == CHANGE_VO_MODE) {
          return true;
        }
        GUIThread guiThread = new GUIThread(this, userTemporaryFileDirectory);
        guiThread.start();
        guiThreadReference = guiThread;
        return true;
      }

      int port = Integer.parseInt(GUIFileSystem.readTextFile(new File(
          getThreadConfigurationFilePath())).trim(), 10);
      logger.debug("getThreadConfigurationFilePath(): "
          + getThreadConfigurationFilePath());
      logger.debug("Port: " + Integer.toString(port));
      clientSocket = new Socket(InetAddress.getLocalHost(), port);
      out = new PrintWriter(clientSocket.getOutputStream(), true);
      in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
      String fromServer = "";
      String fromUser;
      out.println(Utils.GUI_SOCKET_HANDSHAKE_MSG);

      // Contacting thread server.
      clientSocket.setSoTimeout(1000);
      fromServer = in.readLine();
      logger.debug("SERVER: " + fromServer);

      if (fromServer.equals(Utils.GUI_SOCKET_HANDSHAKE_MSG)) {
        fromServer = in.readLine();
        logger.debug("mode: " + this.mode);
        logger.debug("GUI Thread SERVER: " + fromServer);
        logger.debug("userTemporaryFileDirectory: "
            + userTemporaryFileDirectory);
        if (fromServer.equals(userTemporaryFileDirectory)) {
          out.close();
          in.close();
          clientSocket.close();
          JOptionPane.showOptionDialog(GUIUserCredentials.this,
              "An instance of the application is running with the same "
              + "Proxy Subject and Virtual Organisation"
              + ((this.mode == DEFAULT_MODE) ?
              "\nPlease select a Proxy with a different Subject "
              + "or select a different Virtual Organisation" :
              "\nPlease select a different Virtual Organisation"),
              Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION,
              JOptionPane.ERROR_MESSAGE,
              null, null, null);
          return false;
        } else {
          if (this.mode == CHANGE_VO_MODE) {
            logger.debug("handshake message VO not equal CHANGE_VO_MODE");
            out.close();
            in.close();
            clientSocket.close();
            return true;
          }
          logger.debug("handshake message VO not equal DEFAULT_MODE");
          GUIThread guiThread = new GUIThread(this, userTemporaryFileDirectory);
          guiThread.start();
          guiThreadReference = guiThread;
          out.close();
          in.close();
          clientSocket.close();
        }
      } else {
        if (this.mode == CHANGE_VO_MODE) {
          logger.debug("no handshake message CHANGE_VO_MODE");
          out.close();
          in.close();
          clientSocket.close();
          return true;
        }
        logger.debug("no handshake message DEFAULT_MODE");
        GUIThread guiThread = new GUIThread(this, userTemporaryFileDirectory);
        guiThread.start();
        guiThreadReference = guiThread;
        out.close();
        in.close();
        clientSocket.close();
      }

    } catch (UnknownHostException uhe) {
      JOptionPane.showOptionDialog(GUIUserCredentials.this,
          Utils.UNESPECTED_ERROR + uhe.getMessage() +
          "\nApplication will be terminated",
          Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE,
          null, null, null);
      System.exit( -1);

    } catch (IOException ioe) {
      // Couldn't get I/O for the connection.
      GUIThread guiThread = new GUIThread(this, userTemporaryFileDirectory);
      guiThread.start();
      guiThreadReference = guiThread;

    } catch (Exception e) {
      JOptionPane.showOptionDialog(GUIUserCredentials.this,
          e.getMessage() + "\nApplication will be terminated",
          Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE,
          null, null, null);
      System.exit( -1);
    }
    return true;
  }

  protected void jButtonDefaultEvent(ActionEvent ae) {
    try {
      GUIGlobalVars.proxyFilePath = UserCredential.getDefaultProxy();
      logger.debug("UserCredential.getDefaultProxy(): " +
          UserCredential.getDefaultProxy());
      jTextFieldProxyFile.setText(GUIGlobalVars.proxyFilePath);
      setGUIUserCredentials(new File(GUIGlobalVars.proxyFilePath));
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      JOptionPane.showOptionDialog(GUIUserCredentials.this,
          "Unable to load default Proxy Certificate",
          Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE,
          null, null, null);
    }
  }

  private void jTextFieldProxyFileFocusLost(FocusEvent e) {
    GraphicUtils.jTextFieldDeselect(jTextFieldProxyFile);
    Object opComp = e.getOppositeComponent();
    if (opComp == null) {
      return; // User has selected a different application window.
    }
    if ((opComp instanceof JButton) && (opComp != jButtonExit)
        && (opComp != jButtonDefault)
        && (opComp != jButtonOk) && (opComp != jButtonCertFileChooser)
        && (opComp != jButtonCertKeyChooser) &&
        (opComp != jButtonCertDirChooser)
        && (opComp != jButtonProxyFileChooser)) {
      return; // User has pressed a window button (e.g. Exit).
    }
    String proxyFile = jTextFieldProxyFile.getText().trim();
    File file = new File(proxyFile);
    if (!file.isFile() && ((opComp == jButtonExit) || (opComp == jButtonDefault)
        || (opComp == jButtonProxyFileChooser))) {
      return;
    }
    if (!proxyFile.equals("")) {
      if (this.mode == INFO_MODE) {
        return;
      }
      setGUIUserCredentials(new File(proxyFile));
    }
  }

  /**
   * Loads configuration values from configuration files
   */
  private void loadUIConfFile() {
    File voConfFile = new File(GUIFileSystem.getGUIConfVOFile());
    logger.debug("GUI conf VO file: " + GUIFileSystem.getGUIConfVOFile());
    Ad voConfAd = new Ad();
    if (voConfFile.isFile()) {
      try {
        voConfAd.fromFile(voConfFile.toString());
        if (voConfAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_DESTINATION)) {
          GUIGlobalVars.setGUIConfVarLoggingDestination(voConfAd.getStringValue(
              Utils.GUI_CONF_VAR_LOGGING_DESTINATION).get(0).toString().trim());
        }
      } catch (Exception ex) {
      // Do nothing. Logging Destination will be "".
      }
    }

    String versionFile = GUIFileSystem.getVersionFile();
    File confFile = new File(versionFile);
    Ad confAd = new Ad();
    if (confFile.isFile()) {
      try {
        confAd.fromFile(versionFile);
        if (confAd.hasAttribute(Utils.GUI_CONF_VAR_VERSION)) {
          GUIGlobalVars.setGUIConfVarVersion(confAd.getStringValue(Utils.
              GUI_CONF_VAR_VERSION).get(0).toString().trim());
        }
      } catch (Exception ex) {
      // Do nothing, Version will be Unavailable.
      }
    }

    String uiConfFileName = GUIFileSystem.getGUIConfVarFile();
    confFile = new File(uiConfFileName);
    confAd = new Ad();
    if (confFile.isFile()) {
      try {
        confAd.fromFile(uiConfFileName);
        if (confAd.hasAttribute(Utils.GUI_CONF_VAR_RETRY_COUNT)) {
          GUIGlobalVars.setGUIConfVarRetryCount((
              (Integer) confAd.getIntValue(Utils.GUI_CONF_VAR_RETRY_COUNT).get(
              0)).intValue());
        }
        if (confAd.hasAttribute(Utils.GUI_CONF_VAR_ERROR_STORAGE)) {
          String errorStorage = confAd.getStringValue(Utils.
              GUI_CONF_VAR_ERROR_STORAGE).get(0).toString().trim();
          File errorStorageFile = new File(errorStorage);
          if (errorStorageFile.isDirectory()) {
            if (errorStorageFile.canWrite()) {
              GUIGlobalVars.setGUIConfVarErrorStorage(errorStorage);
            } else {
              JOptionPane.showOptionDialog(GUIUserCredentials.this,
                  "Configuration file: " + confFile
                  +
                  "\nUnable to set errors log path ('ErrorStorage' attribute), "
                  + ", you do not have authorization to write file in: "
                  + errorStorage + "\nApplication will be terminated",
                  Utils.ERROR_MSG_TXT,
                  JOptionPane.DEFAULT_OPTION,
                  JOptionPane.ERROR_MESSAGE,
                  null, null, null);
              System.exit( -1);
            }
          } else {
            JOptionPane.showOptionDialog(GUIUserCredentials.this,
                "Configuration file: " + confFile
                +
                "\nUnable to set errors log path ('ErrorStorage' attribute), "
                + "unable to find directory: "
                + errorStorage + "\nApplication will be terminated",
                Utils.ERROR_MSG_TXT,
                JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE,
                null, null, null);
            System.exit( -1);
          }
        }

        if (confAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_TIMEOUT)) {
          GUIGlobalVars.setGUIConfVarLoggingTimeout(
              ((Integer) confAd.getIntValue(Utils.GUI_CONF_VAR_LOGGING_TIMEOUT).
              get(0)).intValue());
        }
        if (confAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT)) {
          GUIGlobalVars.setGUIConfVarLoggingSyncTimeout(
              ((Integer) confAd.getIntValue(Utils.
              GUI_CONF_VAR_LOGGING_SYNC_TIMEOUT).get(0)).intValue());
        }
        if (jobSubmitterJFrame != null) {
          if (confAd.hasAttribute(Utils.GUI_CONF_VAR_LOGGING_DESTINATION)) {
            JOptionPane.showOptionDialog(GUIUserCredentials.this,
                "Deprecated attribute "
                + "'" + Utils.GUI_CONF_VAR_LOGGING_DESTINATION
                + "' found in GUI configuration file",
                Utils.WARNING_MSG_TXT,
                JOptionPane.DEFAULT_OPTION,
                JOptionPane.WARNING_MESSAGE,
                null, null, null);
          }
        }
        if (confAd.hasAttribute(Utils.GUI_CONF_VAR_NS_LOGGER_LEVEL)) {
          GUIGlobalVars.setGUIConfVarNSLoggerLevel(
              ((Integer) confAd.getIntValue(Utils.GUI_CONF_VAR_NS_LOGGER_LEVEL).
              get(0)).intValue());
        }

        if (confAd.hasAttribute(Utils.GUI_CONF_VAR_MAX_MONITORED_JOB_NUMBER)) {
          MultipleJobPanel.setMaxMonitoredJobNumber(
              ((Integer) confAd.getIntValue(Utils.
              GUI_CONF_VAR_MAX_MONITORED_JOB_NUMBER).get(0)).intValue());
        }

      } catch (java.text.ParseException pe) {
        JOptionPane.showOptionDialog(GUIUserCredentials.this,
            "Unable to parse configuration file: " + confFile
            + "\nApplication will be terminated",
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        System.exit( -1);

      } catch (JobAdException jae) {
        JOptionPane.showOptionDialog(GUIUserCredentials.this,
            "Configuration file " + confFile + " syntax error(s):\n\n"
            + jae.getMessage() + "\n\nApplication will be terminated",
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        System.exit( -1);

      } catch (IllegalArgumentException iae) {
        JOptionPane.showOptionDialog(GUIUserCredentials.this,
            "Configuration file " + confFile + ":\n\n"
            + iae.getMessage() + "\n\nApplication will be terminated",
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        System.exit( -1);

      } catch (Exception ex) {
        JOptionPane.showOptionDialog(GUIUserCredentials.this,
            ex.getMessage(),
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        ex.printStackTrace();
      }
    }
  }

  /**
   * Sets the environment variables needed
   */
  private void setEnvironmentVariables() {
    int guiConfVarLoggingTimeout = GUIGlobalVars.getGUIConfVarLoggingTimeout();
    if (guiConfVarLoggingTimeout != -1) {
      Api.setEnv(Utils.EDG_WL_LOG_TIMEOUT,
          (new Integer(guiConfVarLoggingTimeout)).toString());
      logger.info(Utils.EDG_WL_LOG_TIMEOUT + " set to: "
          + Api.getEnv(Utils.EDG_WL_LOG_TIMEOUT));
    }
    int guiConfVarLoggingSyncTimeout = GUIGlobalVars.
        getGUIConfVarLoggingSyncTimeout();
    if (guiConfVarLoggingSyncTimeout != -1) {
      Api.setEnv(Utils.EDG_WL_LOG_SYNC_TIMEOUT,
          (new Integer(guiConfVarLoggingSyncTimeout)).toString());
      logger.info(Utils.EDG_WL_LOG_SYNC_TIMEOUT + " set to: "
          + Api.getEnv(Utils.EDG_WL_LOG_SYNC_TIMEOUT));
    }
    String guiConfVarLoggingDestination = GUIGlobalVars.
        getGUIConfVarLoggingDestination();
    Api.setEnv(Utils.EDG_WL_LOG_DESTINATION, guiConfVarLoggingDestination);
    logger.info(Utils.EDG_WL_LOG_DESTINATION + " set to: "
        + Api.getEnv(Utils.EDG_WL_LOG_DESTINATION));
  }

  /**
   * Removes NS temporary directories of the NSs not contained in the input
   * vector
   *
   * @param toLeaveNSNamesVector
   */
  protected void removeNSTempDirectories(Vector toLeaveNSNamesVector) {
    String directoryPath = GUIFileSystem.getJobTemporaryFileDirectory();
    File directory = new File(directoryPath);
    if (directory.isDirectory()) {
      File[] files = directory.listFiles();
      for (int i = 0; i < files.length; i++) {
        if (files[i].isDirectory()
            && !toLeaveNSNamesVector.contains(files[i].getName())) {
          try {
            GUIFileSystem.removeDirectoryTree(files[i]);
          } catch (Exception e) {
          // Do nothing, directories will remain there (not to bad).
          }
        }
      }
    }
  }

  /**
   * Sets the UserCredential panel Creation mode
   */
  protected void setCreationMode() {
    jTextFieldCertFile.setEditable(false);
    jTextFieldCertKey.setEditable(false);
    jTextFieldTrustedCert.setEditable(false);

    jTextFieldCertFile.setBackground(Color.white);
    jTextFieldCertKey.setBackground(Color.white);
    jTextFieldTrustedCert.setBackground(Color.white);

    int textFieldHeight = jTextFieldCertFile.getSize().height;
    Dimension subjectDimension = jScrollPaneSubject.getSize();
    jTextFieldCertFile.setSize(subjectDimension.width, textFieldHeight);
    jTextFieldCertKey.setSize(subjectDimension.width, textFieldHeight);
    jTextFieldProxyFile.setSize(jTextFieldProxyFile.getSize().width,
        textFieldHeight);
    jTextFieldTrustedCert.setSize(subjectDimension.width, textFieldHeight);

    jComboBoxVO.setVisible(false);
    jTextFieldVO.setEditable(false);
    jTextFieldVO.setText(GUIGlobalVars.getVirtualOrganisation());
    jTextFieldVO.setForeground(Color.black);
    jTextFieldVO.setVisible(true);

    jButtonCertFileChooser.setVisible(false);
    jButtonCertKeyChooser.setVisible(false);
    jButtonCertDirChooser.setVisible(false);

    jButtonDefault.setVisible(false);
    jButtonExit.setText("Cancel");
  }

  /**
   * Sets the UserCredential panel Info mode
   */
  protected void setInfoMode() {
    setCreationMode();

    jButtonProxyFileChooser.setVisible(false);
    jTextFieldProxyFile.setEditable(false);
    jTextFieldProxyFile.setBackground(Color.white);
    jTextFieldProxyFile.setSize(jScrollPaneSubject.getSize().width,
        jTextFieldProxyFile.getSize().height);

    jButtonExit.setText("   Ok   ");
    setJPanelButtonLayout(INFO_MODE);
    jButtonOk.setVisible(false);
  }

  /**
   * Sets the UserCredential panel Change VO mode
   */
  protected void setChangeVOMode() {
    setCreationMode();

    jTextFieldVO.setVisible(false);
    jComboBoxVO.setVisible(true);

    jComboBoxVO.setEditable(false);
    jTextFieldProxyFile.setEditable(false);
    jButtonProxyFileChooser.setVisible(false);
    jTextFieldProxyFile.setBackground(Color.white);
    jTextFieldProxyFile.setSize(jScrollPaneSubject.getSize().width,
        jTextFieldProxyFile.getSize().height);
    setJPanelButtonLayout(DEFAULT_MODE);
  }

// VO
  protected void setSelectMode() {
    setCreationMode();
    jButtonDefault.setVisible(true);

    jButtonProxyFileChooser.setVisible(true);
    jTextFieldProxyFile.setEditable(true);
    jTextFieldProxyFile.setBackground(jTextFieldTrustedCert.getBackground());
    jTextFieldProxyFile.setSize(jTextFieldTrustedCert.getSize().width
        - jButtonProxyFileChooser.getSize().width + 2,
        jTextFieldTrustedCert.getSize().height);

    jTextFieldVO.setVisible(false);
    jComboBoxVO.setVisible(true);
    //jComboBoxVO.setEditable(false);
    setJPanelButtonLayout(DEFAULT_MODE);
  }

// VO

  void setJPanelButtonLayout(int mode) {
    switch (mode) {
      case INFO_MODE:
        jPanelButton.removeAll();
        FlowLayout flowLayout = new FlowLayout();
        flowLayout.setHgap(0);
        flowLayout.setVgap(0);
        jPanelButton.setLayout(flowLayout);
        jPanelButton.add(jButtonExit);
        break;
      case DEFAULT_MODE:
        jPanelButton.removeAll();
        jPanelButton.setLayout(new BoxLayout(jPanelButton, BoxLayout.X_AXIS));
        jPanelButton.setBorder(GraphicUtils.SPACING_BORDER);
        jPanelButton.add(jButtonExit, null);
        jPanelButton.add(Box.createGlue());
        jPanelButton.add(jButtonDefault, null);
        jPanelButton.add(Box.createHorizontalStrut(GraphicUtils.STRUT_GAP));
        jPanelButton.add(jButtonOk, null);
        break;
    }
  }

  /**
   * Sets the Items of the VO combo box
   *
   * @param voVector a Vector containing the VOs to set
   */
  protected void setVOComboBoxItems(Vector voVector) {
    jTextFieldVO.setVisible(false);
    jComboBoxVO.setVisible(true);
    jComboBoxVO.removeAllItems();
    for (int i = 0; i < voVector.size(); i++) {
      jComboBoxVO.addItem(voVector.get(i).toString().trim());
    }
    if (jComboBoxVO.getItemCount() != 0) {
      String virtualOrganisation = GUIGlobalVars.getVirtualOrganisation();
      if (virtualOrganisation.equals("") ||
          !voVector.contains(virtualOrganisation)) {
        jComboBoxVO.setSelectedIndex(0);
      } else {
        jComboBoxVO.setSelectedItem(virtualOrganisation);
      }
    }
  }

  /**
   * Gets the combo box selected VO if the combo box is visible, or the VO
   * contained in the text field otherwise
   *
   * @return the name of the VO
   */
  protected String getVOSelectedItem() {
    String selectedItem = "";
    if (jComboBoxVO.isVisible()) {
      if (jComboBoxVO.isEditable()) {
        selectedItem = jComboBoxVO.getEditor().getItem().toString();
      } else {
        selectedItem = jComboBoxVO.getSelectedItem().toString();
      }
    } else {
      selectedItem = jTextFieldVO.getText().trim();
    }
    return (selectedItem == null) ? "" : selectedItem.trim();
  }

  /**
   * Computes user temporary file directory using a function having as input
   * the subject of the proxy certificate. All certificate has a different
   * corresponding directory.
   */
  static String computeUserTempFileDirectoryName() {
    String subject = "";
    try {
      UserCredential userCredential;
      if (!GUIGlobalVars.proxyFilePath.equals("")) {
        userCredential = new UserCredential(new File(GUIGlobalVars.
            proxyFilePath));
      } else {
        userCredential = new UserCredential();
      }
      subject = userCredential.getX500UserSubject();
    } catch (org.globus.gsi.GlobusCredentialException gpe) {
      System.exit( -1);
    } catch (Exception e) {
      System.exit( -1);
    }
    return "_" + GUIFileSystem.stringToDirectoryName(subject)
        + "_" + System.getProperty("user.name")
        // All VO directories are lower case
        + "_" + GUIGlobalVars.getVirtualOrganisation().toLowerCase();
  }

  /**
   * Computes the name of the user temporary file directory
   *
   * @param vo the name of the VO used to compute the name of the directory
   * @return   the name of the directory
   */
  static String computeUserTempFileDirectoryName(String vo) {
    String subject = "";
    try {
      UserCredential userCredential;
      if (!GUIGlobalVars.proxyFilePath.equals("")) {
        userCredential = new UserCredential(new File(GUIGlobalVars.
            proxyFilePath));
      } else {
        userCredential = new UserCredential();
      }
      subject = userCredential.getX500UserSubject();
    } catch (org.globus.gsi.GlobusCredentialException gpe) {
      System.exit( -1);
    } catch (Exception e) {
      System.exit( -1);
    }
    return "_" + GUIFileSystem.stringToDirectoryName(subject)
        + "_" + System.getProperty("user.name")
        // All VO directories are lower case
        + "_" + vo.toLowerCase();
  }

  /**
   * Computes the path where the thread configuration file is stored
   *
   * @param vo the name of the VO used to compute the name of the directory
   * @return   the resulting path
   */
  static String getThreadConfigurationFilePath(String vo) {
    return GUIFileSystem.getUserHomeDirectory()
        + File.separator + GUIFileSystem.TEMPORARY_FILE_DIRECTORY
        + File.separator + computeUserTempFileDirectoryName(vo)
        + File.separator + GUIFileSystem.THREAD_CONFIGURATION_FILE;
  }

  static String getThreadConfigurationFilePath() {
    return GUIFileSystem.getUserHomeDirectory()
        + File.separator + GUIFileSystem.getTemporaryFileDirectory()
        + GUIFileSystem.THREAD_CONFIGURATION_FILE;
  }

}



/*******************
 * class GUIThread *
 *******************/
class GUIThread extends Thread {
  static Logger logger = Logger.getLogger(GUIThread.class);

  static final boolean THIS_CLASS_DEBUG = false;
  private boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  Component component;
  private volatile boolean active = true;
  ServerSocket serverSocket;
  String temporaryDirectory = "";
  int port;

  public GUIThread(Component component, String temporaryDirectory) {
    this.component = component;
    setDaemon(true); // Thread is killed when calling application dies.
    boolean isSuccess = false;
    int port = Utils.GUI_THREAD_INITIAL_PORT;
    int lastPort = Utils.GUI_THREAD_INITIAL_PORT + Utils.GUI_THREAD_RETRY_COUNT;
    this.temporaryDirectory = temporaryDirectory;

    try {
      ServerSocket serverSocket = new ServerSocket();
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }

    while (!isSuccess && (port < lastPort)) {
      logger.info("GUI Thread trying at Port: " + Integer.toString(port));
      this.port = port;
      try {
        serverSocket = new ServerSocket(port);
        isSuccess = true;

        // Writing file containing port number.
        GUIFileSystem.saveTextFile(GUIUserCredentials.getThreadConfigurationFilePath(),
            Integer.toString(port));
        logger.info("GUI Thread Success at port: " + Integer.toString(port));
      } catch (IOException ioe) {
      // The port is already in use, do nothing, you will try another port.
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(component,
            Utils.FATAL_ERROR
            + "Some problems occures initialising thread socket\n"
            + "Application will be terminated",
            Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE,
            null, null, null);
        System.exit( -1);
      }
      port++;
    }
    if (!isSuccess) {
      JOptionPane.showOptionDialog(component,
          Utils.FATAL_ERROR
          + "Some problems occures initialising thread socket\n"
          + "Application will be terminated",
          Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE,
          null, null, null);
      System.exit( -1);
    }
  }

  public void run() {
    Socket clientSocket = null;
    while (active) {
      try {
        clientSocket = serverSocket.accept();
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
      try {
        PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
        BufferedReader in = new BufferedReader(new InputStreamReader(
            clientSocket.getInputStream()));
        String inputLine = "";
        String outputLine = "";
        //serverSocket.setSoTimeout(2000);
        inputLine = in.readLine();
        if (inputLine.equals(Utils.GUI_SOCKET_HANDSHAKE_MSG)) {
          outputLine = Utils.GUI_SOCKET_HANDSHAKE_MSG;
          out.println(outputLine);
          out.println(temporaryDirectory);
        }
        out.close();
        in.close();
        clientSocket.close();
        //serverSocket.close();
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
    }
  }

  public boolean setTemporaryDirectory(String temporaryDirectory, String vo) {
    logger.debug("setTemporaryDirectory() - port: " + this.port);
    this.temporaryDirectory = temporaryDirectory;
    logger.debug("setTemporaryDirectory() - temporaryDirectory: "
        + this.temporaryDirectory);
    try {
      GUIFileSystem.copyFile(new File(GUIUserCredentials.
          getThreadConfigurationFilePath(vo)),
          new File(GUIUserCredentials.getThreadConfigurationFilePath()));
      logger.debug("setTemporaryDirectory() - Source file: "
          + GUIUserCredentials.getThreadConfigurationFilePath(vo));
      logger.debug("setTemporaryDirectory() - Target file: "
          + GUIUserCredentials.getThreadConfigurationFilePath());

    } catch (Exception e) {
      e.printStackTrace();
      return false;
    }
    logger.debug("setTemporaryDirectory() - Delete file: "
        + GUIUserCredentials.getThreadConfigurationFilePath(vo));
    boolean outgoing = (new File(GUIUserCredentials.
        getThreadConfigurationFilePath(vo))).delete();
    // if (!outgoing) Do nothing. Unable to delete the file.
    return true;
  }

  public void stopThread() {
    active = false;
    interrupt();
  }

} // END class GUIThread
