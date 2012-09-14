/*
 * UnknownPanel.java
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
import java.awt.event.ActionEvent;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextPane;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Ad;
import org.glite.jdl.JobAd;
import org.glite.jdl.JobAdException;

public class UnknownPanel extends JPanel {
  static Logger logger = Logger.getLogger(JDLEditor.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  private String errorMsg = "";

  private String warningMsg = "";

  JPanel jPanelUnknown = new JPanel();

  JScrollPane jScrollPaneUnknown = new JScrollPane();

  JTextPane jTextPaneUnknown = new JTextPane();

  JDLEditorInterface jint;

  public UnknownPanel(Component component) {
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
      if (isDebugging)
        e.printStackTrace();
    }
  }

  private void jbInit() throws Exception {
    isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG) ? true
        : false;
    this.setLayout(new BorderLayout());
    jPanelUnknown.setLayout(new BorderLayout());
    jTextPaneUnknown.setBorder(null);
    //jTextPaneUnknown.setEditable(false);
    this.add(jPanelUnknown, BorderLayout.CENTER);
    jPanelUnknown.add(jScrollPaneUnknown, BorderLayout.CENTER);
    jScrollPaneUnknown.getViewport().add(jTextPaneUnknown, null);
    this.setVisible(true);
  }

  public void setUnknownText(String text) {
    JobAd jobAd = new JobAd();
    try {
      jobAd = new JobAd("[" + text + "]");
    } catch (JobAdException jae) {
      if (isDebugging)
        jae.printStackTrace();
    } catch (Exception e) {
      if (isDebugging)
        e.printStackTrace();
      JOptionPane.showOptionDialog(UnknownPanel.this, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
    }
    if (jobAd != null) {
      text = jobAd.toLines().trim();
      text = text.substring(2);
      int length = text.length();
      text = text.substring(0, length - 2) + ";";
      jTextPaneUnknown.setText(text);
    }
  }

  public String getUnknownText() {
    return jTextPaneUnknown.getText().trim();
  }

  void jButtonUnknownClearEventNoMsg(ActionEvent e) {
    jTextPaneUnknown.setText("");
    jint.setJTextAreaJDL("");
  }

  void jButtonUnknownClearEvent(ActionEvent e) {
    int choice = JOptionPane.showOptionDialog(UnknownPanel.this,
        "Unknown attributes will be cancelled.\nContinue?", "Confirm Clear",
        JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null, null,
        null);
    if (choice == 0) {
      jTextPaneUnknown.setText("");
      jint.setJTextAreaJDL("");
    }
  }

  String getErrorMsg() {
    return this.errorMsg;
  }

  String getWarningMsg() {
    return this.warningMsg;
  }

  String jButtonUnknownViewEvent(boolean showWarningMsg, boolean showErrorMsg,
      ActionEvent ae) {
    this.errorMsg = "";
    this.warningMsg = "";
    Ad adCheck = new Ad();
    try {
      adCheck.fromString(getUnknownText());
      for (int i = 0; i < Utils.guiAttributesArray.length; i++) {
        if (adCheck.hasAttribute(Utils.guiAttributesArray[i])) {
          this.errorMsg += "- " + Utils.GUI_PANEL_NAMES[7]
              + " panel cannot define '" + Utils.guiAttributesArray[i]
              + "' attribute\n";
        }
      }
      for (int i = 0; i < Utils.guiTemporarlyAddedAttributeArray.length; i++) {
        if (adCheck.hasAttribute(Utils.guiTemporarlyAddedAttributeArray[i])) {
          this.errorMsg += "- " + Utils.GUI_PANEL_NAMES[7]
              + " panel cannot define '"
              + Utils.guiTemporarlyAddedAttributeArray[i] + "' attribute\n";
        }
      }
    } catch (Exception e) {
      if (isDebugging)
        e.printStackTrace();
      this.errorMsg += "Edited jdl text contains syntax error(s)";
    }
    this.errorMsg = errorMsg.trim();
    if (!this.errorMsg.equals("") && showErrorMsg) {
      GraphicUtils.showOptionDialogMsg(UnknownPanel.this, errorMsg,
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, Utils.MESSAGE_LINES_PER_JOPTIONPANE, null,
          null);
      jint.setJTextAreaJDL(this.errorMsg);
    } else {
      jint.setJTextAreaJDL(this.warningMsg);
    }
    return getUnknownText();
  }
}