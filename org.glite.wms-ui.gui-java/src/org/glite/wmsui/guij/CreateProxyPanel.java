/*
 * CreateProxyPanel.java
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
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.FocusEvent;
import java.awt.event.WindowEvent;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.net.URL;
import java.security.GeneralSecurityException;
import java.security.InvalidKeyException;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;
import javax.crypto.BadPaddingException;
import javax.swing.BorderFactory;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.plaf.basic.BasicArrowButton;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.wmsui.apij.Api;
import org.glite.wmsui.apij.UserCredential;
import org.globus.gsi.GlobusCredentialException;

/**
 * Implementation of the CreateProxyPanel class.
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class CreateProxyPanel extends JDialog {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int[] keyLengthArray = { 512, 1024, 2048, 4096
  };

  static final String NO_EXTENSION = "- No Extension -";

  private Map voMap = new HashMap();

  private String pwd = null;

  JPasswordField jPasswordFieldPassphrase = new JPasswordField();

  JLabel jLabelKeyLength = new JLabel();

  JLabel jLabelProxyLifetime = new JLabel();

  JLabel jLabelPassphrase = new JLabel();

  JComboBox jComboBoxKeyLength = new JComboBox();

  JTextField jTextFieldProxyLifetime = new JTextField();

  JPanel jPanelCreateProxy = new JPanel();

  JLabel jLabelProxyFile = new JLabel();

  JButton jButtonCancel = new JButton();

  JButton jButtonOk = new JButton();

  BasicArrowButton upProxyLifetime = new BasicArrowButton(
      BasicArrowButton.NORTH);

  BasicArrowButton downProxyLifetime = new BasicArrowButton(
      BasicArrowButton.SOUTH);

  JLabel jLabelVirtualOrganisation = new JLabel();

  JTextField jTextFieldProxyFile = new JTextField();

  GUIUserCredentials credentialInformation;

  JLabel jLabelHours = new JLabel();

  JLabel jLabelBit = new JLabel();

  JButton jButtonProxyFileChooser = new JButton();

  JComboBox jComboBoxVO = new JComboBox();

  JCheckBox jCheckBoxRegenerate = new JCheckBox();

  /**
   * Constructor
   */
  public CreateProxyPanel() {
  }

  /**
   * Constructor
   */
  public CreateProxyPanel(Component component) {
    super((GUIUserCredentials) component);
    if (component instanceof GUIUserCredentials) {
      credentialInformation = (GUIUserCredentials) component;
    } else {
      JOptionPane.showOptionDialog(CreateProxyPanel.this,
          Utils.UNESPECTED_ERROR + Utils.WRONG_COMPONENT_ARGUMENT_TYPE,
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return;
    }
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit();
    } catch (Exception e) {
      e.printStackTrace();
    }
  }

  private void jbInit() throws Exception {
    isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG) ? true
        : false;
    for (int i = 0; i < keyLengthArray.length; i++) {
      jComboBoxKeyLength.addItem(new Integer(keyLengthArray[i]));
    }
    this.setTitle("Credential - Create Proxy");
    this.setSize(new Dimension(447, 250));
    this.setResizable(false);
    //!!! Remove Regenerate Check Box, the proxy will be always regenerated.
    jCheckBoxRegenerate.setSelected(true);
    jCheckBoxRegenerate.setVisible(false);
    /////
    jButtonCancel.setBounds(new Rectangle(15, 185, 85, 25));
    jButtonCancel.setText("Cancel");
    jButtonCancel.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonCancelEvent(e);
      }
    });
    jButtonOk.setBounds(new Rectangle(340, 185, 85, 25));
    jButtonOk.setText("Ok");
    jButtonOk.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonOkEvent(e);
      }
    });
    upProxyLifetime.setBounds(new Rectangle(141, 40, 16, 16));
    upProxyLifetime.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldProxyLifetime, Utils.INTEGER, Integer
            .toString(Utils.PROXY_LIFETIME_DEF_VAL),
            Utils.PROXY_LIFETIME_MIN_VAL, Utils.PROXY_LIFETIME_MAX_VAL);
      }
    });
    downProxyLifetime.setBounds(new Rectangle(141, 56, 16, 16));
    downProxyLifetime.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldProxyLifetime, Utils.INTEGER, Integer
            .toString(Utils.PROXY_LIFETIME_DEF_VAL),
            Utils.PROXY_LIFETIME_MIN_VAL, Utils.PROXY_LIFETIME_MAX_VAL);
      }
    });
    URL fileOpenGifUrl = JobDef1Panel.class.getResource(Utils.ICON_FILE_OPEN);
    if (fileOpenGifUrl != null) {
      jButtonProxyFileChooser.setIcon(new ImageIcon(fileOpenGifUrl));
    } else {
      jButtonProxyFileChooser.setText("...");
    }
    jButtonProxyFileChooser
        .addActionListener(new java.awt.event.ActionListener() {
          public void actionPerformed(ActionEvent e) {
            jButtonFileChooserEvent(e, jTextFieldProxyFile,
                "Cert File Path Selection");
          }
        });
    jLabelKeyLength.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelKeyLength.setText("Key Length");
    jLabelKeyLength.setBounds(new Rectangle(220, 48, 72, 16));
    jLabelProxyLifetime.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelProxyLifetime.setText("Proxy Lifetime");
    jLabelProxyLifetime.setBounds(new Rectangle(17, 48, 89, 16));
    jLabelPassphrase.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelPassphrase.setText("Passphrase");
    jLabelPassphrase.setBounds(new Rectangle(17, 117, 89, 20));
    jComboBoxKeyLength.setBounds(new Rectangle(296, 46, 64, 20));
    jTextFieldProxyLifetime.setText(Integer
        .toString(Utils.PROXY_LIFETIME_DEF_VAL));
    jTextFieldProxyLifetime.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldProxyLifetime.setBounds(new Rectangle(111, 43, 31, 26));
    jTextFieldProxyLifetime.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        GraphicUtils.jTextFieldFocusLost(jTextFieldProxyLifetime,
            Utils.INTEGER, Integer.toString(Utils.PROXY_LIFETIME_DEF_VAL),
            Utils.PROXY_LIFETIME_MIN_VAL, Utils.PROXY_LIFETIME_MAX_VAL);
      }

      public void focusGained(FocusEvent e) {
      }
    });
    jPasswordFieldPassphrase.setText("");
    jPasswordFieldPassphrase.setBounds(new Rectangle(111, 117, 188, 20));
    jPanelCreateProxy.setBorder(BorderFactory.createEtchedBorder());
    jPanelCreateProxy.setBounds(new Rectangle(15, 15, 410, 152));
    jPanelCreateProxy.setLayout(null);
    jLabelVirtualOrganisation.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelVirtualOrganisation.setText("Virtual Org");
    jLabelVirtualOrganisation.setBounds(new Rectangle(17, 81, 89, 16));
    String proxyFile = credentialInformation.jTextFieldProxyFile.getText()
        .trim();
    if (proxyFile.equals("")) {
      proxyFile = UserCredential.getDefaultProxy();
    }
    jTextFieldProxyFile.setText(proxyFile);
    jTextFieldProxyFile.setBounds(new Rectangle(111, 15, 253, 20));
    jLabelProxyFile.setText("Proxy File Path");
    jLabelProxyFile.setBounds(new Rectangle(4, 17, 102, 16));
    jLabelProxyFile.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelHours.setText("hours");
    jLabelHours.setBounds(new Rectangle(161, 48, 41, 16));
    jLabelBit.setText("bits");
    jLabelBit.setBounds(new Rectangle(364, 48, 26, 17));
    jButtonProxyFileChooser.setBounds(new Rectangle(362, 15, 22, 20));
    jComboBoxVO.setBounds(new Rectangle(111, 79, 188, 21));
    jComboBoxVO.setEditable(true);
    jCheckBoxRegenerate.setText("Regenerate");
    jCheckBoxRegenerate.setBounds(new Rectangle(293, 76, 96, 19));
    jPanelCreateProxy.add(jTextFieldProxyFile, null);
    jPanelCreateProxy.add(jLabelProxyFile, null);
    jPanelCreateProxy.add(jButtonProxyFileChooser, null);
    jPanelCreateProxy.add(jPasswordFieldPassphrase, null);
    jPanelCreateProxy.add(jLabelPassphrase, null);
    jPanelCreateProxy.add(jComboBoxKeyLength, null);
    jPanelCreateProxy.add(jLabelProxyLifetime, null);
    jPanelCreateProxy.add(jTextFieldProxyLifetime, null);
    jPanelCreateProxy.add(downProxyLifetime, null);
    jPanelCreateProxy.add(upProxyLifetime, null);
    jPanelCreateProxy.add(jLabelHours, null);
    jPanelCreateProxy.add(jLabelKeyLength, null);
    jPanelCreateProxy.add(jLabelBit, null);
    jPanelCreateProxy.add(jCheckBoxRegenerate, null);
    jPanelCreateProxy.add(jComboBoxVO, null);
    jPanelCreateProxy.add(jLabelVirtualOrganisation, null);
    this.getContentPane().setLayout(null);
    this.getContentPane().add(jPanelCreateProxy, null);
    this.getContentPane().add(jButtonOk, null);
    this.getContentPane().add(jButtonCancel, null);
    // Read VO from file vomses
    jComboBoxVO.setEditable(false);
    Vector voVector = new Vector();
    voVector.add(getVOMSVOVector().get(0));
    if (voVector.size() == 0) {
      voVector = GUIFileSystem.getVirtualOrganisations();
    }
    setVOComboBoxItem(voVector);
  }

  protected void processWindowEvent(WindowEvent e) {
    super.processWindowEvent(e);
    this.setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
    if (e.getID() == WindowEvent.WINDOW_CLOSING) {
      this.dispose();
    }
  }

  void jButtonCancelEvent(ActionEvent e) {
    this.dispose();
  }

  private void jButtonFileChooserEvent(ActionEvent e, Component component,
      String title) {
    JTextField jTextField = new JTextField();
    if (component instanceof JTextField) {
      jTextField = (JTextField) component;
    } else {
      System.exit(-1);
    }
    JFileChooser fileChooser = new JFileChooser();
    fileChooser.setDialogTitle(title);
    fileChooser.setFileHidingEnabled(false);
    fileChooser.setCurrentDirectory(new File(GUIGlobalVars
        .getFileChooserWorkingDirectory()));
    int choice = fileChooser.showOpenDialog(CreateProxyPanel.this);
    if (choice != JFileChooser.APPROVE_OPTION) {
      return;
    } else if (!fileChooser.getSelectedFile().isFile()) {
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      JOptionPane.showOptionDialog(CreateProxyPanel.this,
          "Unable to find file: " + selectedFile, Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      return;
    } else {
      GUIGlobalVars.setFileChooserWorkingDirectory(fileChooser
          .getCurrentDirectory().toString());
      String selectedFile = fileChooser.getSelectedFile().toString().trim();
      jTextField.setText(selectedFile);
    }
  }

  void jButtonOkEvent(ActionEvent ae) {
    char[] passPhraseArray = jPasswordFieldPassphrase.getPassword();
    String passPhrase = "";
    for (int i = 0; i < passPhraseArray.length; i++) {
      passPhrase += passPhraseArray[i];
    }
    passPhrase = passPhrase.trim();
    String userProxy = jTextFieldProxyFile.getText().trim();
    String userCert = credentialInformation.jTextFieldCertFile.getText().trim();
    String userKey = credentialInformation.jTextFieldCertKey.getText().trim();
    String caCertLocation = credentialInformation.jTextFieldTrustedCert
        .getText().trim();
    int bits = Integer.parseInt(
        jComboBoxKeyLength.getSelectedItem().toString(), 10);
    int hours = Integer.parseInt(jTextFieldProxyLifetime.getText(), 10);
    boolean limited = false;
    if (!(new File(userProxy)).isFile() || jCheckBoxRegenerate.isSelected()) {
      try {
        UserCredential.createProxy(passPhrase, userProxy, userCert, userKey,
            bits, hours, limited);
        credentialInformation.jTextFieldProxyFile.setText(userProxy);
        credentialInformation.setGUIUserCredentials(new File(userProxy));
        // Store user selection to show when he asks for Info or to Create new Proxy.
        GUIGlobalVars.certFilePath = userCert;
        GUIGlobalVars.certKeyPath = userKey;
        GUIGlobalVars.trustedCertDir = caCertLocation;
        GUIGlobalVars.proxyFilePath = userProxy;
      } catch (GlobusCredentialException gpe) {
        if (isDebugging) {
          gpe.printStackTrace();
        }
        JOptionPane.showOptionDialog(CreateProxyPanel.this,
            "Unable to create the Proxy", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        this.dispose();
        return;
        // GeneralSecurityException subclass.
      } catch (BadPaddingException bpe) {
        jPasswordFieldPassphrase.grabFocus();
        JOptionPane.showOptionDialog(CreateProxyPanel.this,
            "Inserted Passphrase is uncorrect", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        jPasswordFieldPassphrase.selectAll();
        return;
      } catch (InvalidKeyException ike) {
        credentialInformation.jTextFieldCertFile.grabFocus();
        JOptionPane
            .showOptionDialog(
                CreateProxyPanel.this,
                "Unable to create the Proxy\nplease check certificate and key files",
                Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE, null, null, null);
        this.dispose();
        credentialInformation.jTextFieldCertFile.selectAll();
        return;
        //
      } catch (GeneralSecurityException gse) {
        if (isDebugging) {
          gse.printStackTrace();
        }
        JOptionPane.showOptionDialog(CreateProxyPanel.this, gse.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        return;
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(CreateProxyPanel.this, e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
        return;
      }
    }
    //String vo = jComboBoxVO.getEditor().getItem().toString().trim();
    String vo = jComboBoxVO.getSelectedItem().toString().trim();
    if (!vo.equals("") && !vo.equals(NO_EXTENSION)) {
      String location = Api.getEnv(GUIFileSystem.EDG_LOCATION);
      if ((location == null) || location.equals("")
          || !(new File(location + "/bin/edg-voms-proxy-init")).isFile()) {
        if ((new File("/opt/edg/bin/edg-voms-proxy-init")).isFile()) {
          location = "/opt/edg/bin/edg-voms-proxy-init";
        } else {
          JOptionPane.showOptionDialog(CreateProxyPanel.this,
              "Unable to add VOMS extension to Proxy", Utils.ERROR_MSG_TXT,
              JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null,
              null, null);
          credentialInformation.setGUIUserCredentials(new File(userProxy));
          this.dispose();
          return;
        }
      } else {
        location += "/bin/edg-voms-proxy-init";
      }
      JOptionPane.showOptionDialog(CreateProxyPanel.this,
          "Press Ok to add Extension", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
      try {
        this.voMap = CreateProxyPanel.getVomsesVOAliasMap();
        vo = this.voMap.get(vo).toString();
        String command = location + " -voms " + vo + " -noregen -q";
        logger.debug("Command: " + command);
        Api.shadow(command);
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(CreateProxyPanel.this,
            "Unable to add VOMS extension to Proxy", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
      }
    }
    credentialInformation.setGUIUserCredentials(new File(userProxy));
    this.dispose();
  }

  protected void setProxyFileEditable(boolean bool) {
    jTextFieldProxyFile.setEditable(bool);
    if (!bool) {
      jTextFieldProxyFile.setBackground(Color.white);
    }
    jButtonProxyFileChooser.setVisible(bool);
  }

  static Map getVomsesVOAliasMap() throws Exception {
    Vector vomsesFileVector = CreateProxyPanel.getVomsesFileLocations();
    Vector voVector = new Vector();
    Map voMap = new HashMap();
    File file;
    for (int i = 0; i < vomsesFileVector.size(); i++) {
      file = new File(vomsesFileVector.get(i).toString());
      logger.debug("Looking vomses file: " + file);
      if (file.isFile()) {
        try {
          BufferedReader in = new BufferedReader(new FileReader(file));
          String line;
          String voAlias;
          String voName;
          int index = -1;
          int length = 0;
          while ((line = in.readLine()) != null) {
            line = line.trim();
            voAlias = line.substring(1);
            index = voAlias.indexOf("\"");
            voAlias = voAlias.substring(0, index).trim();
            length = line.length();
            voName = line.substring(0, length - 1);
            index = voName.lastIndexOf("\"");
            voName = voName.substring(index + 1).trim();
            logger.debug("VO name: " + voName);
            logger.debug("VO alias: " + voAlias);
            if (!voVector.contains(voName)) {
              voVector.add(voName);
              voMap.put(voName, voAlias);
            }
          }
          in.close();
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
          throw e;
        }
      }
    }
    return voMap;
  }

  static Vector getVOMSVOVector() throws Exception {
    Vector vomsesFileVector = CreateProxyPanel.getVomsesFileLocations();
    Vector voVector = new Vector();
    File file;
    for (int i = 0; i < vomsesFileVector.size(); i++) {
      file = new File(vomsesFileVector.get(i).toString());
      logger.debug("Looking vomses file: " + file);
      if (file.isFile()) {
        try {
          BufferedReader in = new BufferedReader(new FileReader(file));
          String line;
          String voAlias;
          String voName;
          int index = -1;
          int length = 0;
          while ((line = in.readLine()) != null) {
            line = line.trim();
            voAlias = line.substring(1);
            index = voAlias.indexOf("\"");
            voAlias = voAlias.substring(0, index).trim();
            length = line.length();
            voName = line.substring(0, length - 1);
            index = voName.lastIndexOf("\"");
            voName = voName.substring(index + 1).trim();
            logger.debug("VO name: " + voName);
            logger.debug("VO alias: " + voAlias);
            if (!voVector.contains(voName)) {
              voVector.add(voName);
            }
          }
          in.close();
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
          throw e;
        }
      }
    }
    return voVector;
  }

  static Vector getVomsesFileLocations() {
    Vector vomsesFileVector = new Vector();
    String edgLocation = Api.getEnv(GUIFileSystem.EDG_LOCATION);
    if ((edgLocation != null) && edgLocation.equals("")) {
      vomsesFileVector.add(edgLocation + "/etc/"
          + GUIFileSystem.VOMSES_FILE_NAME);
    } else {
      vomsesFileVector.add("/opt/edg/etc/" + GUIFileSystem.VOMSES_FILE_NAME);
    }
    String location = "/home/" + System.getProperty("user.name") + "/.edg/"
        + GUIFileSystem.VOMSES_FILE_NAME;
    vomsesFileVector.add(location);
    return vomsesFileVector;
  }

  boolean changeProxyVO(String vo, GUIUserCredentials credentialInformation) {
    String userProxy = credentialInformation.jTextFieldProxyFile.getText()
        .trim();
    String userCert = credentialInformation.jTextFieldCertFile.getText().trim();
    String userKey = credentialInformation.jTextFieldCertKey.getText().trim();
    String caCertLocation = credentialInformation.jTextFieldTrustedCert
        .getText().trim();
    int bits = 512;
    int hours = 12;
    boolean limited = false;
    Vector voVector = new Vector();
    Map voMap = new HashMap();
    try {
      voMap = CreateProxyPanel.getVomsesVOAliasMap();
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      return false;
    }
    try {
      UserCredential userCredential = new UserCredential(new File(userProxy));
      voVector = userCredential.getVONames();
      bits = userCredential.getStrenght();
      limited = userCredential.getCredType();
      hours = userCredential.getTimeLeft();
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      return false;
    }
    if (voVector.contains(vo)) {
      return true;
    }
    return false;
    /*
     voVector.add(vo);
     String location = Api.getEnv(Utils.EDG_LOCATION);
     if ((location == null) || location.equals("") || !(new File(location
     + "/bin/edg-voms-proxy-init")).isFile()) {
     if ((new File("/opt/edg/bin/edg-voms-proxy-init")).isFile()) {
     location = "/opt/edg/bin/edg-voms-proxy-init";
     } else {
     return false;
     }
     } else {
     location += "/bin/edg-voms-proxy-init";
     }
     PasswordDialog passwordDialog = new PasswordDialog(credentialInformation, this);
     passwordDialog.setModal(true);
     Utils.windowCenterWindow(credentialInformation, passwordDialog);
     passwordDialog.setVisible(true);
     if (this.pwd == null) {
     return false;
     }
     String voms = "";
     for (int i = 0; i < voVector.size(); i++) {
     voms += " -voms " + voMap.get(voVector.get(i).toString()).toString();
     }
     String pwdFileName = Utils.getUserHomeDirectory() + File.separator
     + Utils.getTemporaryFileDirectory() + ".tempFile";
     logger.debug("pwdFileName" + pwdFileName);
     File pwdFile = new File(pwdFileName);
     try {
     Utils.saveTextFile(pwdFile, this.pwd);
     } catch (Exception e) {
     if (isDebugging) e.printStackTrace();
     return false;
     }
     this.pwd = null;
     String outFileName = Utils.getUserHomeDirectory() + File.separator
     + Utils.getTemporaryFileDirectory() + "x509_TempProxy";
     File outFile = new File(outFileName);
     //String command = location + voms + " -pwstdin -q < " + pwdFileName;
     //String command = location + voms + " -noregen -q";
     String command = location + voms + " -pwstdin -out " + outFileName + " -q < " + pwdFileName;
     logger.debug("Command: " + command);
     try {
     if (Api.shadow(command) != 0) {
     logger.debug("Api.shadow(command) != 0"); // old code 256
     outFile.delete();
     pwdFile.delete();
     return false;
     }
     } catch (Exception e) {
     if (isDebugging) e.printStackTrace();
     outFile.delete();
     pwdFile.delete();
     return false;
     }
     try {
     Utils.copyFile(outFile, new File(userProxy));
     } catch (Exception e) {
     if (isDebugging) e.printStackTrace();
     outFile.delete();
     pwdFile.delete();
     return false;
     }
     outFile.delete();
     pwdFile.delete();
     return true;
     */
  }

  void setPassword(String pwd) {
    this.pwd = pwd;
  }

  protected void setVOComboBoxItem(Vector voVector) {
    if (voVector == null) {
      return;
    }
    jComboBoxVO.removeAllItems();
    for (int i = 0; i < voVector.size(); i++) {
      jComboBoxVO.addItem(voVector.get(i).toString().trim());
    }
    jComboBoxVO.insertItemAt(NO_EXTENSION, 0);
    String credentialInformationVO = credentialInformation.getVOSelectedItem();
    if (jComboBoxVO.getItemCount() != 0) {
      if (credentialInformationVO.equals("")
          || !voVector.contains(credentialInformationVO)) {
        jComboBoxVO.setSelectedIndex(0);
      } else {
        jComboBoxVO.setSelectedItem(credentialInformationVO);
      }
    }
  }
}