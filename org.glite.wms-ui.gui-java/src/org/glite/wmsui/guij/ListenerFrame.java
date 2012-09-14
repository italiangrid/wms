/*
 * ListenerFrame.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.BufferedWriter;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.text.JTextComponent;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.wmsui.apij.Listener;
import org.glite.wmsui.apij.Shadow;

/**
 * Implementation of the ListenerFrame class.
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Alessandro Maraschini <alessandro.maraschini@datamat.it>
 */
public class ListenerFrame extends JFrame implements Listener, ActionListener {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  BufferedWriter inputBuffer;

  boolean refresh = true;

  // Job job ;
  Shadow shadow;

  static int OUT = 1;

  static int IN = 2;

  static int ERR = 3;

  //JTextArea out;
  JTextField jTextFieldIn = new JTextField();

  JButton jButtonSend = new JButton();

  JButton jButtonClose = new JButton();

  JTextArea jTextAreaOutput = new JTextArea();

  JLabel jLabelJobId = new JLabel();

  JTextField jTextFieldJobId = new JTextField();

  JScrollPane jScrollPaneJobError = new JScrollPane();

  JTextArea jTextAreaError = new JTextArea();

  JLabel jLabelJobError = new JLabel();

  JLabel outLabel = new JLabel("Job output");

  JScrollPane jScrollPaneJobOutput = new JScrollPane(jTextAreaOutput);

  JLabel inLabel = new JLabel("Job Input");

  String jobId = "";

  Thread thIn = null, thOut, thErr; // input writing Thread

  /**Construct the frame*/
  public ListenerFrame() {
    super("Job Interaction");
    try {
      jbInit();
    } catch (Exception e) {
      if (isDebugging)
        e.printStackTrace();
    }
  }

  /**Component initialization*/
  private void jbInit() throws Exception {
    isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG) ? true
        : false;
    setSize(new Dimension(500, 546));
    getContentPane().setLayout(null);
    setResizable(false);
    //Adding JLabel outLabel
    outLabel.setText("Job Standard Output");
    //outLabel.setLocation(10,10);
    outLabel.setBounds(new Rectangle(16, 45, 140, 18));
    // out
    jTextAreaOutput = new JTextArea();
    //jTextAreaOutput.setLocation(10,30);
    jTextAreaOutput.setEditable(false);
    // Scroll Pane
    // JScrollPane scrollPane = new JScrollPane( jTextAreaOutput , JScrollPane.VERTICAL_SCROLLBAR_ALWAYS  ,  JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
    //jScrollPaneJobOutput.setLocation(10,30);
    jScrollPaneJobOutput.setBounds(new Rectangle(16, 62, 461, 215));
    //Adding JLabel inLabel
    //inLabel.setLocation(10,450);
    inLabel.setBounds(new Rectangle(16, 444, 64, 18));
    jTextFieldIn.setBounds(new Rectangle(16, 462, 359, 20));
    // in
    // Close
    jButtonClose = new JButton("Close");
    jButtonClose.addActionListener(this);
    jButtonClose.setBounds(new Rectangle(16, 490, 85, 25));
    // Send
    jButtonSend = new JButton("Send");
    jButtonSend.addActionListener(this);
    jButtonSend.setBounds(new Rectangle(392, 462, 85, 25));
    //jButtonSend.setLocation ( 350, 480 );
    jLabelJobId.setText("Job Id");
    jLabelJobId.setBounds(new Rectangle(16, 17, 39, 18));
    jTextFieldJobId.setEditable(false);
    jTextFieldJobId.setBackground(Color.white);
    jTextFieldJobId.setBounds(new Rectangle(58, 16, 419, 20));
    jTextAreaOutput.setText("");
    jScrollPaneJobError.setBounds(new Rectangle(16, 306, 461, 127));
    jLabelJobError.setText("Job Standard Error");
    jLabelJobError.setBounds(new Rectangle(16, 288, 149, 18));
    jTextAreaError.setText("");
    this.getContentPane().add(jTextFieldJobId, null);
    this.getContentPane().add(jLabelJobId, null);
    this.getContentPane().add(jLabelJobError, null);
    this.getContentPane().add(jScrollPaneJobOutput);
    this.getContentPane().add(jScrollPaneJobError, null);
    this.getContentPane().add(outLabel);
    this.getContentPane().add(inLabel);
    this.getContentPane().add(jTextFieldIn);
    this.getContentPane().add(jButtonSend);
    this.getContentPane().add(jButtonClose);
    jScrollPaneJobError.getViewport().add(jTextAreaError, null);
    jScrollPaneJobOutput.getViewport().add(jTextAreaOutput, null);
    //setVisible(true);
    // Window closing listener
    addWindowListener(new WindowAdapter() {
      public void windowClosing(WindowEvent e) {
        refresh = false;
      }
    });
  }

  public void setJobIdTextField(String jobId) {
    jTextFieldJobId.setText(jobId);
  }

  public void run(Shadow sh) {
    shadow = sh;
    jTextFieldJobId.setText(sh.getJobId().toString());
    validate();
    repaint();
    setVisible(true);
    String outMsg = null; //TBD not needed
    thOut = new Thread(new ReadingPipe(jTextAreaOutput, sh, 0));
    // thErr = new Thread ( new ReadingPipe ( jTextAreaError  , sh , 1)  ) ;
    thIn = new Thread(new ReadingPipe(jTextFieldIn, shadow, 2));
    thOut.setDaemon(true);
    thOut.start();
    while (refresh) { /*  Do nothing. Wait for refresh to stop*/
    }
    ;
    // if ( thIn.isAlive() ) System.out.println("***Warning: Last input message was not completely send to the remote listener") ;
    stop();
  }

  public void stop() {
    //Stop and close the window
    refresh = false;
    /** Flush output /  error Streams */
    try {
      thOut.join(1);
      // thErr.join( 1  ) ;
      thIn.join(1);
      shadow.detach();
    } catch (Exception exc) {
      if (isDebugging)
        exc.printStackTrace();
      this.dispose();
    }
    // System.out.println( "Bye Bye" ) ;
    //System.exit(0) ;
    if (GUIGlobalVars.openedListenerFrameMap.containsKey(this)) {
      GUIGlobalVars.openedListenerFrameMap.remove(this);
    }
    this.dispose();
  }

  /*  write the input information into the pipe*/
  void send(String msg) {
    if (thIn.isAlive()) {
      // Check The previous Thread
      String errMsg = "\n*** Error: Unable to send the message '"
          + msg
          + "'.\nStill waiting for the previous message to be read by input stream";
      jTextAreaError.setEditable(true);
      jTextAreaError.append(errMsg);
      jTextAreaError.setEditable(false);
      return;
    }
    // Write the text into the Output Area
    jTextAreaOutput.setEditable(true);
    jTextAreaOutput.append("\n$> " + msg + "\n");
    jTextAreaOutput.setEditable(false);
    // Launch the Thread which writes into the input pipe
    thIn.setDaemon(true);
    thIn.start();
    return;
  }

  // Listener Implementation
  public void actionPerformed(ActionEvent actionEvent) {
    Object source = actionEvent.getSource();
    String command = actionEvent.getActionCommand();
    if ((source == jButtonSend) || (source == jTextFieldIn)) {
      send(jTextFieldIn.getText());
      // jTextFieldIn.setText("");
    } else if (source == jButtonClose)
      refresh = false;
  }

  /*
   *  MAIN method
   */
  public static void main(String[] args) throws Exception {
    ListenerFrame ls = new ListenerFrame();
    // ls.run( new Job() , null , null, null , null );
  };
} //End ListenerFrame class

class ReadingPipe implements Runnable {
  public ReadingPipe(JTextComponent text, Shadow shadow, int level) {
    lev = level;
    txt = text;
    sh = shadow;
  };

  public void run() {
    String str = null;
    while (true)
      try {
        switch (lev) {
          case 0: /*  Read output pipe*/
            str = sh.emptyOut();
            if (!str.equals("")) {
              txt.setEditable(true);
              txt.setText(txt.getText() + str);
              txt.setEditable(false);
            } else
              Thread.sleep(1000);
          break;
          /*  case 1:   Read Error pipe
           str = sh.emptyErr()  ;
           if ( !str.equals("")   ){
           txt.setEditable( true) ;
           txt.setText( txt.getText( )  +  str  );
           txt.setEditable( false) ;
           } else Thread.sleep ( 1000  ) ;
           break;
           */
          default: /*  Write input pipe*/
            str = txt.getText();
            // System.out.println( "removing Selection... and writing: "  + str) ;
            if (!str.equals("")) {
              // Remove the entry
              txt.selectAll();
              txt.replaceSelection("");
              sh.writeIn(str);
              return;
            } else
              Thread.sleep(1000);
          break;
        }
      } catch (Exception exc) { /* Do nothing*/
      }
  }

  /** Private Members*/
  private JTextComponent txt = null;

  private Shadow sh = null;

  private int lev;
};
