/*
 * LogInfoFrame.java
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
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ComponentEvent;
import java.util.Vector;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextPane;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;

/**
 * Implementation of the LogInfoFrame class.
 *
 *
 * @ingroup gui
 * @brief This class provides some constant values and utility methods.
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class LogInfoFrame extends JFrame {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int FRAME_WIDTH = 620;

  static final int FRAME_HEIGHT = 600;

  LogInfoPanel logInfoJPanel; // = new LogInfoPanel();

  MultipleJobPanel multipleJobPanel;

  Vector eventVector = new Vector();

  JTextPane jTextPane = new JTextPane();

  JPanel jPanelText = new JPanel();

  JPanel jPanelButton = new JPanel();

  JScrollPane jScrollPaneText = new JScrollPane();

  JButton jButtonBack = new JButton();

  JScrollPane jScrollPaneMain = new JScrollPane();

  public LogInfoFrame() {
    setTitle("Job Monitor - Job Logging Information");
    try {
      jbInit();
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
  }

  /**
   * Constructor.
   */
  public LogInfoFrame(Component component, Vector eventVector) {
    this.setTitle("Job Monitor - Job Logging Information");
    this.eventVector = eventVector;
    if (component instanceof MultipleJobPanel) {
      multipleJobPanel = (MultipleJobPanel) component;
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
    Toolkit toolkit = getToolkit();
    Dimension screenSize = toolkit.getScreenSize();
    int width = (int) (screenSize.width * GraphicUtils.SCREEN_WIDTH_PROPORTION * GraphicUtils.SCREEN_WIDTH_INFO_DETAILS_PROPORTION);
    int height = (int) (screenSize.height * GraphicUtils.SCREEN_HEIGHT_PROPORTION);
    this.setSize(new Dimension(width, height));
    this.addComponentListener(new java.awt.event.ComponentListener() {
      public void componentResized(ComponentEvent e) {
        logInfoJPanel.updateValueTableWidth();
      }

      public void componentMoved(ComponentEvent e) {
      }

      public void componentHidden(ComponentEvent e) {
      }

      public void componentShown(ComponentEvent e) {
      }
    });
    logInfoJPanel = new LogInfoPanel(LogInfoFrame.this, eventVector);
    jScrollPaneMain.getViewport().add(logInfoJPanel);
    jPanelText.setLayout(new BorderLayout());
    jButtonBack.setText("  Back  ");
    jButtonBack.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonBackEvent(e);
      }
    });
    jPanelButton.add(jButtonBack);
    jScrollPaneText = new JScrollPane(jTextPane);
    jPanelText.add(jScrollPaneText, BorderLayout.CENTER);
    jPanelText.add(jPanelButton, BorderLayout.SOUTH);
    this.getContentPane().add(jScrollPaneMain, BorderLayout.CENTER);
    logInfoJPanel.setPreferredSize(new Dimension(
            (int) (width - width * GraphicUtils.SCREEN_WIDTH_INFO_DETAILS_PREFERRED_PROPORTION),
            (int) (height - height * GraphicUtils.SCREEN_WIDTH_INFO_DETAILS_PREFERRED_PROPORTION)));
  }  void setTableEvents(Vector eventsVector) {
    logInfoJPanel.setTableEvents(eventsVector);
  }

  void close() {
    multipleJobPanel.setIsLogInfoJDialogShown(false);
    this.dispose();
  }

  void showTextPane(String text) {
    if (!text.trim().equals("")) {
      jTextPane.setText(text);
      this.getContentPane().remove(jScrollPaneMain);
      this.getContentPane().add(jPanelText, BorderLayout.CENTER);
      validate();
      repaint();
    }
  }

  void jButtonBackEvent(ActionEvent e) {
    this.getContentPane().remove(jPanelText);
    this.getContentPane().add(jScrollPaneMain, BorderLayout.CENTER);
    validate();
    repaint();
  }

  String getJobIdShown() {
    return logInfoJPanel.jTextFieldJobId.getText().trim();
  }

  void setJLabelLastUpdate(String timeText) {
    logInfoJPanel.setJLabelLastUpdate(timeText);
  }
}