/*
 * QueryPreferencesPanel.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.awt.AWTEvent;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.util.Vector;
import javax.swing.JButton;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.table.TableColumn;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.glite.jdl.Ad;

/**
 * Implementation of the QueryPreferencesPanel class.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class QueryPreferencesPanel extends JPanel {
  static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int QUERY_NAME_COLUMN_INDEX = 0;

  static final int LB_SERVER_COLUMN_INDEX = 1;

  static final int OWNED_COLUMN_INDEX = 2;

  static final String NOT_AVAILABLE_LB = " (NA)";

  JPanel jPanelQuery = new JPanel();

  JScrollPane jScrollPaneTags = new JScrollPane();

  JTable jTableQueries;

  JobTableModel jobTableModel;

  Vector vectorHeader = new Vector();

  JButton jButtonNew = new JButton();

  JButton jButtonEdit = new JButton();

  JButton jButtonRemove = new JButton();

  JobMonitorPreferences jobMonitorPreferences;

  JobMonitor jobMonitorJFrame;

  /**
   * Constructor.
   */
  public QueryPreferencesPanel(JobMonitorPreferences jobMonitorPreferences,
      JobMonitor jobMonitorJFrame) {
    this.jobMonitorPreferences = jobMonitorPreferences;
    this.jobMonitorJFrame = jobMonitorJFrame;
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
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
  public QueryPreferencesPanel() {
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
    vectorHeader.addElement("Query Name");
    vectorHeader.addElement("LB Server");
    vectorHeader.addElement("Owned Jobs Only");
    jobTableModel = new JobTableModel(vectorHeader, 0);
    jTableQueries = new JTable(jobTableModel);
    jTableQueries.getTableHeader().setReorderingAllowed(false);
    //jTableQueries.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
    TableColumn col = jTableQueries.getColumnModel().getColumn(
        QUERY_NAME_COLUMN_INDEX);
    col.setCellRenderer(new GUITableTooltipCellRenderer());
    col = jTableQueries.getColumnModel().getColumn(LB_SERVER_COLUMN_INDEX);
    col.setCellRenderer(new GUITableTooltipCellRenderer());
    col.setPreferredWidth(200);
    col = jTableQueries.getColumnModel().getColumn(OWNED_COLUMN_INDEX);
    col.setCellRenderer(new GUICheckBoxCellRenderer());
    col.setMinWidth(110);
    col.setMaxWidth(110);
    //jTableQueries.setAutoResizeMode(JTable.AUTO_RESIZE_ALL_COLUMNS);
    jPanelQuery.add(jButtonRemove, null);
    jPanelQuery.add(jScrollPaneTags, null);
    jPanelQuery.add(jButtonNew, null);
    jPanelQuery.add(jButtonEdit, null);
    jScrollPaneTags.getViewport().add(jTableQueries, null);
    this.add(jPanelQuery, null);
    this.setLayout(null);
    this.setSize(new Dimension(547, 381));
    jScrollPaneTags.getViewport().setBackground(Color.white);
    jScrollPaneTags.setBounds(new Rectangle(12, 20, 507, 179));
    jButtonNew.setBounds(new Rectangle(12, 208, 85, 25));
    jButtonNew.setText("New");
    jButtonEdit.setBounds(new Rectangle(103, 208, 85, 25));
    jButtonEdit.setText("Edit");
    jButtonRemove.setBounds(new Rectangle(434, 208, 85, 25));
    jButtonRemove.setText("Remove");
    jPanelQuery.setBorder(new TitledBorder(new EtchedBorder(),
        " Queries List Table ", 0, 0, null,
        GraphicUtils.TITLED_ETCHED_BORDER_COLOR));
    jPanelQuery.setBounds(new Rectangle(2, 3, 530, 246));
    jPanelQuery.setLayout(null);
    jButtonNew.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonNewEvent(e);
      }
    });
    jButtonRemove.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonRemoveEvent(e);
      }
    });
    jButtonEdit.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButtonEditEvent(e);
      }
    });
    jTableQueries.addMouseListener(new MouseAdapter() {
      public void mouseClicked(MouseEvent me) {
        if (me.getClickCount() == 2) {
          Point point = me.getPoint();
          int row = jTableQueries.rowAtPoint(point);
          int column = jTableQueries.columnAtPoint(point);
          jButtonEditEvent(null);
        }
      }
    });
    //loadPreferencesFromFile();
  }

  void jButtonRemoveEvent(ActionEvent e) {
    int[] selectedRow = jTableQueries.getSelectedRows();
    int selectedRowCount = selectedRow.length;
    if (selectedRowCount != 0) {
      for (int i = selectedRowCount - 1; i >= 0; i--) {
        jobTableModel.removeRow(selectedRow[i]);
      }
      if (jobTableModel.getRowCount() != 0) {
        int selectableRow = selectedRow[selectedRowCount - 1] + 1
            - selectedRowCount; // Next.
        if (selectableRow > jobTableModel.getRowCount() - 1) {
          selectableRow--; // Prev. (selectedRow[selectedRowCount - 1] - selectedRowCount).
        }
        jTableQueries.setRowSelectionInterval(selectableRow, selectableRow);
      }
    } else {
      JOptionPane.showOptionDialog(QueryPreferencesPanel.this,
          Utils.SELECT_AN_ITEM, Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
          null, null);
    }
  }

  void jButtonEditEvent(ActionEvent e) {
    if (jTableQueries.getSelectedRowCount() == 0) {
      JOptionPane.showOptionDialog(QueryPreferencesPanel.this,
          "Please first select a table row to Edit", Utils.INFORMATION_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null,
          null, null);
      return;
    } else if (jTableQueries.getSelectedRowCount() != 1) {
      JOptionPane.showOptionDialog(QueryPreferencesPanel.this,
          "Please select a single table row to Edit",
          Utils.INFORMATION_MSG_TXT, JOptionPane.DEFAULT_OPTION,
          JOptionPane.INFORMATION_MESSAGE, null, null, null);
      return;
    }
    int selectedRow = jTableQueries.getSelectedRow();
    String selectedQuery = jobTableModel.getValueAt(selectedRow,
        QUERY_NAME_COLUMN_INDEX).toString().trim();
    QueryPanel queryPanel = new QueryPanel(this, jobMonitorJFrame,
        (Ad) jobMonitorJFrame.userQueryMap.get(selectedQuery), true);
    GraphicUtils.windowCenterWindow(jobMonitorPreferences, queryPanel);
    queryPanel.show();
  }

  void jButtonNewEvent(ActionEvent e) {
    QueryPanel queryPanel = new QueryPanel(this, jobMonitorJFrame);
    GraphicUtils.windowCenterWindow(jobMonitorPreferences, queryPanel);
    queryPanel.show();
  }

  int jButtonApplyEvent(ActionEvent ae) {
    File userPrefFile = new File(GUIFileSystem.getUserPrefFile());
    Ad userPrefAd = new Ad();
    if (userPrefFile.isFile()) {
      try {
        userPrefAd.fromFile(userPrefFile.toString());
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        return Utils.FAILED;
      }
    }
    Ad userPrefJobMonitorAd = new Ad();
    try {
      if (userPrefAd.hasAttribute(Utils.PREF_FILE_JOB_MONITOR)) {
        userPrefJobMonitorAd = userPrefAd.getAd(Utils.PREF_FILE_JOB_MONITOR);
        userPrefAd.delAttribute(Utils.PREF_FILE_JOB_MONITOR);
      }
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
    }
    try {
      if (userPrefJobMonitorAd.hasAttribute(Utils.USER_QUERIES)) {
        userPrefJobMonitorAd.delAttribute(Utils.USER_QUERIES);
      }
      int rowCount = jTableQueries.getRowCount();
      for (int i = 0; i < rowCount; i++) {
        try {
          userPrefJobMonitorAd.addAttribute(Utils.USER_QUERIES,
              (Ad) jobMonitorJFrame.userQueryMap.get(jobTableModel.getValueAt(
                  i, QUERY_NAME_COLUMN_INDEX)));
        } catch (Exception e) {
          if (isDebugging) {
            e.printStackTrace();
          }
          return Utils.FAILED;
        }
      }
      userPrefAd
          .setAttribute(Utils.PREF_FILE_JOB_MONITOR, userPrefJobMonitorAd);
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
    }
    try {
      GUIFileSystem.saveTextFile(userPrefFile, userPrefAd.toString(true, true));
    } catch (Exception ex) {
      if (isDebugging) {
        ex.printStackTrace();
      }
      JOptionPane.showOptionDialog(QueryPreferencesPanel.this,
          "Unable to save preferences file", Utils.ERROR_MSG_TXT,
          JOptionPane.DEFAULT_OPTION, JOptionPane.ERROR_MESSAGE, null, null,
          null);
    }
    jobMonitorJFrame.setLBQueryMenuItems(jobMonitorJFrame
        .loadUserQueriesFromFile());
    return Utils.SUCCESS;
  }

  void addQueryToTable(String queryName, String lbAddress,
      Boolean ownedJobsOnly, Ad userQueryAd) {
    Vector elementToAddVector = new Vector();
    elementToAddVector.add(queryName);
    if (!lbAddress.equals(QueryPanel.ALL_LB_SERVERS)) {
      if (!jobMonitorJFrame.getLBMenuItems().contains(lbAddress)) {
        lbAddress += NOT_AVAILABLE_LB;
      }
    }
    elementToAddVector.add(lbAddress);
    elementToAddVector.add(ownedJobsOnly);
    jobMonitorJFrame.userQueryMap.put(queryName, userQueryAd);
    jobTableModel.addRow(elementToAddVector);
  }

  void setQueryToTable(String queryName, String lbAddress,
      Boolean ownedJobsOnly, Ad userQueryAd) {
    if (!lbAddress.equals(QueryPanel.ALL_LB_SERVERS)) {
      if (!jobMonitorJFrame.getLBMenuItems().contains(lbAddress)) {
        lbAddress += NOT_AVAILABLE_LB;
      }
    }
    int index = jobTableModel.getIndexOfElementInColumn(queryName,
        QUERY_NAME_COLUMN_INDEX);
    if (index != -1) {
      jobTableModel.setValueAt(lbAddress, index, LB_SERVER_COLUMN_INDEX);
      jobTableModel.setValueAt(ownedJobsOnly, index, OWNED_COLUMN_INDEX);
      jobMonitorJFrame.userQueryMap.remove(queryName);
      jobMonitorJFrame.userQueryMap.put(queryName, userQueryAd);
    } else {
      // ERROR
    }
  }

  void loadPreferencesFromFile() {
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
        return;
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
            Boolean allLBFlag = new Boolean(false);
            String lbAddress = QueryPanel.ALL_LB_SERVERS;
            Boolean ownedJobsOnlyFlag = new Boolean(false);
            Ad userTagsAd = new Ad();
            for (int i = 0; i < queriesAdVector.size(); i++) {
              queryAd = (Ad) queriesAdVector.get(i);
              logger.debug("queriesAd: " + queryAd);
              Vector elementToAddVector = new Vector();
              queryName = queryAd.getStringValue(Utils.QUERY_NAME).get(0)
                  .toString().trim();
              elementToAddVector.add(queryName);
              allLBFlag = (Boolean) queryAd.getBooleanValue(Utils.ALL_LB_FLAG)
                  .get(0);
              if (!allLBFlag.booleanValue()) {
                if (!jobMonitorJFrame.getLBMenuItems().contains(lbAddress)) {
                  lbAddress = queryAd.getStringValue(Utils.LB_ADDRESS).get(0)
                      .toString();
                  lbAddress += NOT_AVAILABLE_LB;
                }
              } else {
                lbAddress = QueryPanel.ALL_LB_SERVERS;
              }
              elementToAddVector.add(lbAddress);
              ownedJobsOnlyFlag = (Boolean) queryAd.getBooleanValue(
                  Utils.OWNED_JOBS_ONLY_FLAG).get(0);
              elementToAddVector.add(ownedJobsOnlyFlag);
              jobTableModel.addRow(elementToAddVector);
              jobMonitorJFrame.userQueryMap.put(queryName, queryAd);
              userTagsAd = new Ad();
            }
          }
        }
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
    }
  }

  Ad getQueryFromMap(String queryName) {
    return (Ad) jobMonitorJFrame.userQueryMap.get(queryName);
  }
}