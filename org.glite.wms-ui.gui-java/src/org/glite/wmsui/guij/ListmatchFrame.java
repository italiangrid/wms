/*
 * ListmatchFrame.java
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
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.FocusEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.WindowEvent;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.ListSelectionModel;
import javax.swing.SwingConstants;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.table.TableColumn;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Jdl;
import org.glite.jdl.JobAd;
import org.glite.wmsui.apij.Job;
import org.glite.wmsui.apij.Result;
import org.glite.wmsui.apij.Url;
import org.glite.wmsui.apij.UserCredential;

/**
 * Implementation of the ListmatchFrame class.
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class ListmatchFrame extends JFrame {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int CE_ID_COLUMN_INDEX = 0;

  static final int RANK_VALUE_COLUMN_INDEX = 1;

  static final String[] stringHeader = { "CE Id", "rank Value"
  };

  private Component component;

  private String nsName;

  private String nsAddress;

  private String keyJobName;

  private String title;

  JTable jTableCEId;

  JobTableModel jobTableModel;

  JPanel jPanelCEId = new JPanel();

  JScrollPane jScrollPaneJListmatchFrame = new JScrollPane();

  JScrollPane jScrollPaneCEId = new JScrollPane();

  JLabel jLabelTotalFoundCEIds = new JLabel();

  JTextField jTextFieldTotalFoundCEIds = new JTextField();

  JButton jButtonSelectCEId = new JButton();

  JLabel jLabelSelectedCEId = new JLabel();

  JTextField jTextFieldSelectedCEId = new JTextField();

  JButton jButtonClose = new JButton();

  JButton jButtonSave = new JButton();

  /**
   * Constructor.
   */
  public ListmatchFrame(Component component, String nsName, String nsAddress,
      String keyJobName, String title) {
    super(title + nsName + " - " + keyJobName);
    this.component = component;
    this.nsName = nsName;
    this.nsAddress = nsAddress;
    this.keyJobName = keyJobName;
    this.title = title;
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
    setSize(new Dimension(525, 343));
    setResizable(false);
    this.getContentPane().setLayout(null);
    Vector vectorHeader = new Vector();
    for (int i = 0; i < stringHeader.length; i++) {
      vectorHeader.addElement(stringHeader[i]);
    }
    jobTableModel = new JobTableModel(vectorHeader, 0);
    jTableCEId = new JTable(jobTableModel);
    jTableCEId.getTableHeader().setReorderingAllowed(false);
    jTableCEId.setAutoCreateColumnsFromModel(false);
    jTableCEId.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    TableColumn col = jTableCEId.getColumnModel().getColumn(CE_ID_COLUMN_INDEX);
    col.setCellRenderer(new GUITableTooltipCellRenderer());
    col = jTableCEId.getColumnModel().getColumn(RANK_VALUE_COLUMN_INDEX);
    col.setCellRenderer(new GUITableTooltipCellRenderer());
    col.setMinWidth(80);
    col.setMaxWidth(80);
    jPanelCEId.setLayout(null);
    jPanelCEId.setBorder(new TitledBorder(new EtchedBorder(),
        " Listmatch CE Ids Table ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jButtonClose.setBounds(new Rectangle(5, 284, 85, 25));
    jButtonClose.setText("Close");
    jButtonClose.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonCloseEvent(e);
      }
    });
    jScrollPaneCEId.getViewport().setBackground(Color.white);
    jScrollPaneCEId.setBounds(new Rectangle(11, 48, 486, 177));
    jPanelCEId.setBounds(new Rectangle(5, 5, 508, 239));
    jLabelTotalFoundCEIds.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelTotalFoundCEIds.setText("Total CE Ids");
    jLabelTotalFoundCEIds.setBounds(new Rectangle(351, 21, 98, 18));
    jTextFieldTotalFoundCEIds.setFont(new java.awt.Font("Dialog", 1, 12));
    jTextFieldTotalFoundCEIds.setBorder(BorderFactory
        .createLoweredBevelBorder());
    jTextFieldTotalFoundCEIds.setText("");
    jTextFieldTotalFoundCEIds.setHorizontalAlignment(SwingConstants.RIGHT);
    jTextFieldTotalFoundCEIds.setEditable(false);
    jTextFieldTotalFoundCEIds.setBounds(new Rectangle(454, 21, 43, 18));
    jButtonSelectCEId.setBounds(new Rectangle(428, 284, 85, 25));
    jButtonSelectCEId.setText("Select");
    jButtonSelectCEId.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonSelectCEIdEvent(e);
      }
    });
    jLabelSelectedCEId.setHorizontalAlignment(SwingConstants.RIGHT);
    jLabelSelectedCEId.setText("CE Id");
    jLabelSelectedCEId.setBounds(new Rectangle(3, 252, 36, 20));
    jTextFieldSelectedCEId.setText("");
    jTextFieldSelectedCEId.setBounds(new Rectangle(45, 252, 468, 20));
    jTextFieldSelectedCEId.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusLost(FocusEvent e) {
        jTextFieldSelectedCEIdFocusLost(e);
      }
    });
    jButtonSave.setBounds(new Rectangle(337, 284, 85, 25));
    jButtonSave.setText("Save");
    jButtonSave.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonSaveEvent(e);
      }
    });
    jPanelCEId.add(jTextFieldTotalFoundCEIds, null);
    jPanelCEId.add(jScrollPaneCEId, null);
    jPanelCEId.add(jLabelTotalFoundCEIds, null);
    jScrollPaneCEId.getViewport().add(jTableCEId, null);
    this.getContentPane().add(jPanelCEId, null);
    this.getContentPane().add(jButtonClose, null);
    this.getContentPane().add(jTextFieldSelectedCEId, null);
    this.getContentPane().add(jButtonClose, null);
    this.getContentPane().add(jButtonSelectCEId, null);
    this.getContentPane().add(jButtonSave, null);
    this.getContentPane().add(jLabelSelectedCEId, null);
    this.getContentPane().setVisible(true);
    jTableCEId.addMouseListener(new MouseAdapter() {
      public void mouseClicked(MouseEvent e) {
        if (e.getClickCount() == 1) {
          Point p = e.getPoint();
          int row = jTableCEId.rowAtPoint(p);
          int column = jTableCEId.columnAtPoint(p);
          String selectedCEId = jTableCEId.getValueAt(row, CE_ID_COLUMN_INDEX)
              .toString().trim();
          jTextFieldSelectedCEId.setText(selectedCEId);
        }
      }
    });
  }

  /**
   * Inserts the CE Ids contained in the Vector argument, in the CE Ids table.
   */
  public void setCEIdTable(Vector ceIdVector) {
    jobTableModel.removeAllRows();
    for (int i = 0; i < ceIdVector.size(); i++) {
      Vector vectorTemp = new Vector();
      vectorTemp.addElement(ceIdVector.get(i));
      vectorTemp.addElement("");
      jobTableModel.addRow(vectorTemp);
    }
    jTextFieldTotalFoundCEIds.setText(Integer.toString(jobTableModel
        .getRowCount()));
    if (jobTableModel.getRowCount() != 0) {
      jTableCEId.setRowSelectionInterval(0, 0);
      jTextFieldSelectedCEId.setText(jobTableModel.getValueAt(0,
          CE_ID_COLUMN_INDEX).toString().trim());
    }
  }

  /**
   * Loads CE Ids and corrisponding rank value from file represented by filePath
   * and inserts the values in the CE Ids table.
   * @throws Exception
   * @returns Utils.SUCCESS or Utils.FAILED depending on the method outcome.
   */
  public int setCEIdTable(String filePath, String keyJobName) {
    JobAd jobAd = new JobAd();
    Job job;
    Result result = null;
    try {
      jobAd.fromFile(filePath);
      job = new Job(jobAd);
      logger.info("setCEIdTable(String file) - jobAd: " + jobAd);
      logger.info("setCEIdTable(String file) - NS Address: " + this.nsAddress);
      UserCredential userCredential = new UserCredential(new File(
          GUIGlobalVars.proxyFilePath));
      if (userCredential.getX500UserSubject()
          .equals(GUIGlobalVars.proxySubject)) {
        result = job.listMatchingCE(new Url(this.nsAddress));
      } else {
        JOptionPane.showOptionDialog(component, Utils.FATAL_ERROR
            + "Proxy file user subject has changed"
            + "\nApplication will be terminated", Utils.ERROR_MSG_TXT,
            JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
            null);
        System.exit(-1);
      }
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      JOptionPane.showOptionDialog(this.component, e.getMessage(),
          Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return Utils.FAILED;
    }
    int code = result.getCode();
    if ((code != Result.LISTMATCH_FAILURE)
        && (code != Result.LISTMATCH_FORBIDDEN)) {
      HashMap resultHashMap = (HashMap) result.getResult();
      Iterator resultIterator = resultHashMap.keySet().iterator();
      String ceId;
      Double rankValue;
      jobTableModel.removeAllRows();
      while (resultIterator.hasNext()) {
        Vector rowToAddVector = new Vector();
        ceId = resultIterator.next().toString();
        rankValue = (Double) resultHashMap.get(ceId);
        rowToAddVector.add(ceId);
        rowToAddVector.add(rankValue);
        //jobTableModel.addRow(rowToAddVector);
        jobTableModel.insertRow(0, rowToAddVector);
      }
    } else {
      JOptionPane.showOptionDialog(this.component, "Job '" + keyJobName
          + "':\n" + ((Exception) result.getResult()).getMessage(),
          "JobSubmitter - Listmatch", JOptionPane.DEFAULT_OPTION,
          JOptionPane.ERROR_MESSAGE, null, null, null);
      return Utils.FAILED;
    }
    jTextFieldTotalFoundCEIds.setText(Integer.toString(jobTableModel
        .getRowCount()));
    if (jobTableModel.getRowCount() != 0) {
      jTableCEId.setRowSelectionInterval(0, 0);
      jTextFieldSelectedCEId.setText(jobTableModel.getValueAt(0,
          CE_ID_COLUMN_INDEX).toString().trim());
    }
    return Utils.SUCCESS;
  }

  void jButtonCloseEvent(ActionEvent e) {
    if (GUIGlobalVars.openedListmatchMap.containsKey(this.nsName + " - "
        + this.keyJobName)) {
      GUIGlobalVars.openedListmatchMap.remove(this.nsName + " - "
          + this.keyJobName);
    }
    this.dispose();
  }

  protected void processWindowEvent(WindowEvent e) {
    super.processWindowEvent(e);
    this.setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
    if (e.getID() == WindowEvent.WINDOW_CLOSING) {
      jButtonCloseEvent(null);
    }
  }

  void jButtonSelectCEIdEvent(ActionEvent ae) {
    String selectedCEId = jTextFieldSelectedCEId.getText().trim();
    if (!selectedCEId.equals("")) {
      String temporaryPhysicalFileName = GUIFileSystem
          .getJobTemporaryFileDirectory()
          + this.nsName
          + File.separator
          + this.keyJobName
          + GUIFileSystem.JDL_FILE_EXTENSION;
      try {
        JobAd jobAd = new JobAd();
        jobAd.fromFile(temporaryPhysicalFileName);
        if (jobAd.hasAttribute(Jdl.CEID)) {
          jobAd.delAttribute(Jdl.CEID);
        }
        jobAd.setAttribute(Jdl.CEID, selectedCEId);
        GUIFileSystem.saveTextFile(temporaryPhysicalFileName, jobAd.toString(
            true, true));
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        JOptionPane.showOptionDialog(ListmatchFrame.this, e.getMessage(),
            Utils.ERROR_MSG_TXT, JOptionPane.DEFAULT_OPTION,
            JOptionPane.ERROR_MESSAGE, null, null, null);
      }
      jButtonCloseEvent(null);
    } else {
      JOptionPane.showOptionDialog(ListmatchFrame.this,
          "Please choose or provide a CE Id before selection",
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
    }
  }

  void jButtonSaveEvent(ActionEvent e) {
    JFileChooser fileChooser = new JFileChooser();
    fileChooser.setDialogTitle("Save CE Ids List File");
    fileChooser.setCurrentDirectory(new File(GUIGlobalVars
        .getFileChooserWorkingDirectory()));
    String[] extensions = { "LST"
    };
    GUIFileFilter classadFileFilter = new GUIFileFilter("*.lst", extensions);
    fileChooser.addChoosableFileFilter(classadFileFilter);
    int choice = fileChooser.showSaveDialog(ListmatchFrame.this);
    if (choice != JFileChooser.APPROVE_OPTION) {
      return;
    } else {
      File outputFile = fileChooser.getSelectedFile();
      String selectedFile = outputFile.toString()
          + GUIFileSystem.CE_ID_LIST_FILE_EXTENSION;
      logger.debug("selectedFile: " + selectedFile);
      choice = 0;
      if (outputFile.isFile()) {
        choice = JOptionPane.showOptionDialog(ListmatchFrame.this,
            "Output file exists. Overwrite?", "Confirm Save",
            JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null,
            null, null);
      }
      if (choice == 0) {
        try {
          PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(
              selectedFile)));
          if (jobTableModel.getRowCount() != 0) {
            for (int i = 0; i < jobTableModel.getRowCount(); i++) {
              out.write(jTableCEId.getValueAt(i, CE_ID_COLUMN_INDEX).toString()
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
    }
  }

  void setJButtonSaveVisible(boolean bool) {
    jButtonSave.setVisible(bool);
  }

  void jTextFieldSelectedCEIdFocusLost(FocusEvent e) {
    GraphicUtils.jTextFieldDeselect(jTextFieldSelectedCEId);
  }

  void setJobName(String name) {
    if (name != null) {
      this.keyJobName = name;
    }
  }

  static void closeAllListmatchFrames() {
    Object[] values = GUIGlobalVars.openedListmatchMap.values().toArray();
    for (int i = 0; i < values.length; i++) {
      ((ListmatchFrame) values[i]).dispose();
    }
    GUIGlobalVars.openedListmatchMap.clear();
  }
}