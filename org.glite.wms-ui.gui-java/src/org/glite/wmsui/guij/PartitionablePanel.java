/*
 * PartitionablePanel.java 
 * 
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 * 
 */

package org.glite.wmsui.guij;

import java.awt.AWTEvent;
import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextPane;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Jdl;
import org.glite.jdl.JobAd;

public class PartitionablePanel extends JPanel {
  static Logger logger = Logger.getLogger(JDLEditor.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  private String errorMsg = "";

  private String warningMsg = "";

  JPanel jPanelPre = new JPanel();

  JPanel jPanelPreText = new JPanel();

  JScrollPane jScrollPanePre = new JScrollPane();

  JTextPane jTextPanePre = new JTextPane();

  JPanel jPanelPost = new JPanel();

  JPanel jPanelPostText = new JPanel();

  JScrollPane jScrollPanePost = new JScrollPane();

  JTextPane jTextPanePost = new JTextPane();

  JDLEditorInterface jint;

  public PartitionablePanel(Component component) {
    if (component instanceof JDLEditor) {
      jint = (JDLEditor) component;
      /*
       * } else if (component instanceof JDLEJInternalFrame) { jint =
       * (JDLEJInternalFrame) component; } else if (component instanceof
       * JDLEJApplet) { jint = (JDLEJApplet) component;
       */
    } else {
      System.exit(-1);
    }
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit();
    } catch (Exception e) {
      if (isDebugging)
        e.printStackTrace();
    }
  }

  private void jbInit() throws Exception {
    isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG) ? true
        : false;
    GridBagLayout gbl = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.insets = new Insets(3, 3, 3, 3);
    jPanelPreText.setLayout(new BorderLayout());
    jPanelPre.setLayout(new BorderLayout());
    jPanelPre.add(jPanelPreText, BorderLayout.CENTER);
    jTextPanePre.setBorder(null);
    jPanelPreText.add(jScrollPanePre, BorderLayout.CENTER);
    jScrollPanePre.getViewport().add(jTextPanePre, null);
    jPanelPre.setBorder(new TitledBorder(new EtchedBorder(), " Pre Job JDL ",
        0, 0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelPostText.setLayout(new BorderLayout());
    jPanelPost.setLayout(new BorderLayout());
    jPanelPost.add(jPanelPostText, BorderLayout.CENTER);
    jTextPanePost.setBorder(null);
    jPanelPostText.add(jScrollPanePost, BorderLayout.CENTER);
    jScrollPanePost.getViewport().add(jTextPanePost, null);
    jPanelPost.setBorder(new TitledBorder(new EtchedBorder(), " Post Job JDL ",
        0, 0, null, GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    this.setLayout(gbl);
    GraphicUtils.setDefaultGridBagConstraints(gbc);
    this.add(jPanelPre, GraphicUtils.setGridBagConstraints(gbc, 0, 0, 1, 1,
        1.0, 1.0, GridBagConstraints.FIRST_LINE_START, GridBagConstraints.BOTH,
        new Insets(1, 1, 1, 1), 0, 0));
    this.add(jPanelPost, GraphicUtils.setGridBagConstraints(gbc, 0, 1, 1, 1,
        1.0, 1.0, GridBagConstraints.FIRST_LINE_START, GridBagConstraints.BOTH,
        null, 0, 0));
    this.setVisible(true);
  }

  public void setPartitionablePreText(JobAd jobAd) {
    if (jobAd != null) {
      jTextPanePre.setText(jobAd.toString(true, true));
    }
  }

  public void setPartitionablePostText(JobAd jobAd) {
    if (jobAd != null) {
      jTextPanePost.setText(jobAd.toString(true, true));
    }
  }

  public String getPartitionablePreText() {
    return jTextPanePre.getText().trim();
  }

  public String getPartitionablePostText() {
    return jTextPanePost.getText().trim();
  }

  void jButtonPartitionableResetEventNoMsg(ActionEvent e) {
    jTextPanePre.setText("");
    jTextPanePost.setText("");
    jint.setJTextAreaJDL("");
  }

  void jButtonPartitionableResetEvent(ActionEvent e) {
    int choice = JOptionPane.showOptionDialog(PartitionablePanel.this,
        "Pre and Post Job JDL will be cancelled.\nContinue?", "Confirm Clear",
        JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null, null,
        null);
    if (choice == 0) {
      jTextPanePre.setText("");
      jTextPanePost.setText("");
      jint.setJTextAreaJDL("");
    }
  }

  String getErrorMsg() {
    return this.errorMsg;
  }

  String getWarningMsg() {
    return this.warningMsg;
  }

  String jButtonPartitionableViewEvent(boolean showWarningMsg,
      boolean showErrorMsg, ActionEvent ae) {
    this.errorMsg = "";
    this.warningMsg = "";
    JobAd adCheck = new JobAd();
    try {
      adCheck.fromString(getPartitionablePreText());
      //adCheck.checkAll(); //TBD when coded a check method
    } catch (Exception e) {
      this.errorMsg += Jdl.PRE_JOB + ": " + e.getMessage();
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    try {
      adCheck.fromString(getPartitionablePostText());
      //adCheck.checkAll(); //TBD when coded a check method
    } catch (Exception e) {
      this.errorMsg += Jdl.POST_JOB + ": " + e.getMessage();
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    this.errorMsg = errorMsg.trim();
    if (!this.errorMsg.equals("") && showErrorMsg) {
      GraphicUtils.showOptionDialogMsg(PartitionablePanel.this, errorMsg,
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE, null,
          null);
      jint.setJTextAreaJDL(this.errorMsg);
    } else {
      jint.setJTextAreaJDL(this.warningMsg);
    }
    String result = "";
    if (!getPartitionablePreText().equals("")) {
      result += Jdl.PRE_JOB + " = " + getPartitionablePreText() + ";\n";
    }
    if (!getPartitionablePostText().equals("")) {
      result += Jdl.POST_JOB + " = " + getPartitionablePostText() + ";\n";
    }
    return result;
  }
}