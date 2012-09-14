/*
 * RetrieveCheckpointStateDialog.java
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
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.plaf.basic.BasicArrowButton;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;

/**
 * Implementation of the RetrieveCheckpointStateDialog class.
 *
 *
 * @ingroup gui
 * @brief This class provides some constant values and utility methods.
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class RetrieveCheckpointStateDialog extends JDialog {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  private String jobId = "";

  private int state = Utils.CHECKPOINT_STATE_DEF_VAL;

  JLabel jLabelJobId = new JLabel();

  JTextField jTextFieldJobId = new JTextField();

  BasicArrowButton upCheckpointState = new BasicArrowButton(
      BasicArrowButton.NORTH);

  BasicArrowButton downCheckpointState = new BasicArrowButton(
      BasicArrowButton.SOUTH);

  JTextField jTextFieldCheckpointState = new JTextField();

  JLabel jLabelCheckpointState = new JLabel();

  JButton jButtonCancel = new JButton();

  JButton jButtonRetrieve = new JButton();

  /**
   *  Constructor.
   */
  public RetrieveCheckpointStateDialog(Component component, String jobIdText,
      boolean isEditable) {
    super((JFrame) component);
    this.jobId = jobIdText;
    if (component instanceof JobSubmitter) {
      setTitle("Job Submitter - Retrieve Checkpoint State");
    } else if (component instanceof JobMonitor) {
      setTitle("Job Monitor - Retrieve Checkpoint State");
    } else {
      System.exit(-1);
    }
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit();
      jTextFieldJobId.setText(jobIdText);
      if (!isEditable) {
        jTextFieldJobId.setEditable(false);
        jTextFieldJobId.setBackground(Color.white);
        jTextFieldJobId.setBorder(BorderFactory.createEtchedBorder());
      } else {
        jTextFieldJobId.selectAll();
      }
    } catch (Exception e) {
      if (isDebugging)
        e.printStackTrace();
    }
  }

  private void jbInit() throws Exception {
    isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG) ? true
        : false;
    setSize(new Dimension(547, 110));
    setResizable(false);
    jLabelJobId.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelJobId.setText("Job Id");
    jLabelJobId.setBounds(new Rectangle(7, 15, 48, 18));
    this.getContentPane().setLayout(null);
    jTextFieldJobId.setText("");
    jTextFieldJobId.setBounds(new Rectangle(58, 14, 374, 20));
    upCheckpointState.setBounds(new Rectangle(507, 8, 16, 16));
    downCheckpointState.setBounds(new Rectangle(507, 24, 16, 16));
    jTextFieldCheckpointState.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldCheckpointState.setBounds(new Rectangle(478, 11, 29, 27));
    jTextFieldCheckpointState
        .addFocusListener(new java.awt.event.FocusAdapter() {
          public void focusLost(FocusEvent e) {
            GraphicUtils.jTextFieldFocusLost(jTextFieldCheckpointState,
                Utils.INTEGER,
                Integer.toString(Utils.CHECKPOINT_STATE_DEF_VAL),
                Utils.CHECKPOINT_STATE_MIN_VAL, Utils.CHECKPOINT_STATE_MAX_VAL);
          }

          public void focusGained(FocusEvent e) {
          }
        });
    jTextFieldCheckpointState.setText(Integer
        .toString(Utils.CHECKPOINT_STATE_DEF_VAL));
    jLabelCheckpointState.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelCheckpointState.setText("State");
    jLabelCheckpointState.setBounds(new Rectangle(439, 15, 36, 18));
    jButtonCancel.setBounds(new Rectangle(192, 48, 85, 25));
    jButtonCancel.setText("Cancel");
    jButtonCancel.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonCancelEvent(e);
      }
    });
    jButtonRetrieve.setBounds(new Rectangle(286, 48, 85, 25));
    jButtonRetrieve.setText("Retrieve");
    jButtonRetrieve.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonRetrieveEvent(e);
      }
    });
    this.getContentPane().add(jLabelJobId, null);
    this.getContentPane().add(jTextFieldJobId, null);
    this.getContentPane().add(downCheckpointState, null);
    this.getContentPane().add(jLabelCheckpointState, null);
    this.getContentPane().add(jTextFieldCheckpointState, null);
    this.getContentPane().add(upCheckpointState, null);
    this.getContentPane().add(jButtonRetrieve, null);
    this.getContentPane().add(jButtonCancel, null);
    upCheckpointState.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.upButtonEvent(jTextFieldCheckpointState, Utils.INTEGER, Integer
            .toString(Utils.CHECKPOINT_STATE_DEF_VAL),
            Utils.CHECKPOINT_STATE_MIN_VAL, Utils.CHECKPOINT_STATE_MAX_VAL);
      }
    });
    downCheckpointState.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Utils.downButtonEvent(jTextFieldCheckpointState, Utils.INTEGER, Integer
            .toString(Utils.CHECKPOINT_STATE_DEF_VAL),
            Utils.CHECKPOINT_STATE_MIN_VAL, Utils.CHECKPOINT_STATE_MAX_VAL);
      }
    });
  }

  public String getJobId() {
    return this.jobId;
  }

  public int getState() {
    return this.state;
  }

  void jButtonCancelEvent(ActionEvent e) {
    this.jobId = null;
    this.dispose();
  }

  void jButtonRetrieveEvent(ActionEvent e) {
    this.jobId = jTextFieldJobId.getText().trim();
    this.state = Integer.parseInt(jTextFieldCheckpointState.getText().trim(),
        10);
    this.dispose();
  }
}