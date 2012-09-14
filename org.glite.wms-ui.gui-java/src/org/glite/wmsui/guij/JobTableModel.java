/*
 * JobTableModel.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.awt.Component;
import java.util.Collections;
import java.util.Comparator;
import java.util.Vector;
import javax.swing.DefaultListCellRenderer;
import javax.swing.JCheckBox;
import javax.swing.JList;
import javax.swing.JTable;
import javax.swing.SwingConstants;
import javax.swing.border.Border;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableCellRenderer;
import javax.swing.table.TableColumn;
import javax.swing.table.TableColumnModel;

/**
 * Implementation of the JobTableModel class
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class JobTableModel extends DefaultTableModel {
  //static Logger logger = Logger.getLogger(GUIUserCredentials.class.getName());
  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  private String[] header = {};

  private String[] values;

  private String[][] cell;

  private Vector isColumnEditableVector = new Vector();

  /**
   * Constructor.
   */
  public JobTableModel(Vector columnNames, int rowCount) {
    setDataVector(newVector(rowCount), columnNames);
    initIsColumnEditableVector();
  }

  /**
   * Constructor.
   */
  public JobTableModel(Vector data, Vector columnNames) {
    setDataVector(data, columnNames);
    initIsColumnEditableVector();
  }

  private void initIsColumnEditableVector() {
    for (int i = 0; i < this.getColumnCount(); i++) {
      isColumnEditableVector.add(new Boolean(false));
    }
  }

  Vector newVector(int size) {
    Vector v = new Vector(size);
    v.setSize(size);
    return v;
  }

  /**
   * Removes all table rows.
   */
  public void removeAllRows() {
    int rowCount = this.getRowCount();
    for (int i = rowCount - 1; i >= 0; i--) {
      this.removeRow(i);
    }
  }

  /**
   * Checks if the specified table cell is editable or not.
   *
   * @param row cell row index
   * @param column cell column index
   * @return true if the cell is editable, false otherwise or if row or column
   * of the specified cell is out of bounds
   */
  public boolean isCellEditable(int row, int column) {
    if ((row >= this.getRowCount()) || (column >= this.getColumnCount())) {
      return false;
    }
    return (((Boolean) isColumnEditableVector.get(column)).booleanValue()) ? true
        : false;
  }

  /**
   * Sets whether or not specified column is editable.
   *
   * @param index column index
   * @param bool true if this component should be editable, false otherwise
   */
  public void setColumnEditable(int index, boolean bool) {
    isColumnEditableVector.setElementAt(new Boolean(bool), index);
  }

  /**
   * Checks if the specified row is present in the table.
   *
   * @param rowElement string concatenation of the table row cells
   * @return true if the specified row is present, false otherwise
   */
  public boolean isRowPresent(String rowElement) {
    int rowCount = this.getRowCount();
    int columnCount = this.getColumnCount();
    if (rowCount != 0) {
      for (int i = 0; i < rowCount; i++) {
        String tableRowElement = "";
        for (int j = 0; j < columnCount; j++) {
          tableRowElement += this.getValueAt(i, j).toString().trim();
        }
        if (rowElement.equals(tableRowElement)) {
          return true;
        }
      }
    }
    return false;
  }

  /**
   * Checks if the specified row is present in the table and returns the index of the
   * first occurence of the row.
   *
   * @param rowElement string concatenation of the table row cells
   * @return the index of the row if present, -1 otherwise
   */
  public int getIndexOfRow(String rowElement) {
    int rowCount = this.getRowCount();
    int columnCount = this.getColumnCount();
    if (rowCount != 0) {
      for (int i = 0; i < rowCount; i++) {
        String tableRowElement = "";
        for (int j = 0; j < columnCount; j++) {
          tableRowElement += this.getValueAt(i, j).toString().trim();
        }
        if (rowElement.equals(tableRowElement)) {
          return i;
        }
      }
    }
    return -1;
  }

  /**
   * Checks if the specified element is present in the specified column.
   *
   * @param element element to find
   * @param column column index
   * @return true if the element is present, false otherwise
   */
  public boolean isElementPresentInColumn(String element, int column) {
    element = element.trim();
    int rowCount = this.getRowCount();
    int columnCount = this.getColumnCount();
    if (rowCount != 0 && !(column < 0) && !(column > columnCount - 1)) {
      String tableElement = "";
      for (int i = 0; i < rowCount; i++) {
        tableElement = this.getValueAt(i, column).toString().trim();
        if (element.equals(tableElement)) {
          return true;
        }
      }
    }
    return false;
  }

  /**
   * Checks if the specified element is present in the specified column. The search
   * for element is case insensitive.
   *
   * @param element element to find
   * @param column column index
   * @return true if the element is present, false otherwise
   */
  public boolean isElementPresentInColumnCi(String element, int column) {
    element = element.toUpperCase().trim();
    int rowCount = this.getRowCount();
    int columnCount = this.getColumnCount();
    if (rowCount != 0 && !(column < 0) && !(column > columnCount - 1)) {
      String tableElement = "";
      for (int i = 0; i < rowCount; i++) {
        tableElement = this.getValueAt(i, column).toString().toUpperCase()
            .trim();
        if (element.equals(tableElement)) {
          return true;
        }
      }
    }
    return false;
  }

  /**
   * Checks if the specified element is present in the specified column and returns
   * the index of the first occurence of the element.
   *
   * @param element element to find
   * @param column column index
   * @return the table row index where the element is present, if one, -1 otherwise
   */
  public int getIndexOfElementInColumn(String element, int column) {
    int rowCount = this.getRowCount();
    int columnCount = this.getColumnCount();
    if (rowCount != 0 && !(column < 0) && !(column > columnCount - 1)) {
      String tableElement = "";
      for (int i = 0; i < rowCount; i++) {
        tableElement = this.getValueAt(i, column).toString().trim();
        if (element.equals(tableElement)) {
          return i;
        }
      }
    }
    return -1;
  }

  /**
   * Checks if the specified element is present in the specified column and returns
   * the index of the first occurence of the element. The search for the element is
   * case insensitive.
   *
   * @param element element to find
   * @param column column index
   * @return the table row index where the element is present, if one, -1 otherwise
   */
  public int getIndexOfElementInColumnCi(String element, int column) {
    element = element.toUpperCase().trim();
    int rowCount = this.getRowCount();
    int columnCount = this.getColumnCount();
    if (rowCount != 0 && !(column < 0) && !(column > columnCount - 1)) {
      String tableElement = "";
      for (int i = 0; i < rowCount; i++) {
        tableElement = this.getValueAt(i, column).toString().toUpperCase()
            .trim();
        if (element.equals(tableElement)) {
          return i;
        }
      }
    }
    return -1;
  }

  /**
   * Checks if the specified element is present in the specified column and returns
   * the number of the element occurences.
   *
   * @param element element to find
   * @param column column index
   * @return number of occurences of the element in the specified table column
   */
  public int getElementPresentInColumnCount(String element, int column) {
    int rowCount = this.getRowCount();
    int columnCount = this.getColumnCount();
    int count = 0;
    element = element.trim();
    if (rowCount != 0 && !(column < 0) && !(column > columnCount - 1)) {
      String tableElement = "";
      for (int i = 0; i < rowCount; i++) {
        tableElement = this.getValueAt(i, column).toString().trim();
        if (element.equals(tableElement)) {
          count++;
        }
      }
    }
    return count;
  }

  /**
   * Checks if the specified element is present in the specified column and returns
   * the number of the element occurences. The search of the element is case insensitive.
   *
   * @param element element to find
   * @param column column index
   * @return number of occurences of the element in the specified table column
   */
  public int getElementPresentInColumnCountCi(String element, int column) {
    int rowCount = this.getRowCount();
    int columnCount = this.getColumnCount();
    int count = 0;
    element = element.trim().toUpperCase();
    if (rowCount != 0 && !(column < 0) && !(column > columnCount - 1)) {
      String tableElement = "";
      for (int i = 0; i < rowCount; i++) {
        tableElement = this.getValueAt(i, column).toString().trim()
            .toUpperCase();
        if (element.equals(tableElement)) {
          count++;
        }
      }
    }
    return count;
  }

  /**
   * Gets the column preferred width necessary to show the widest element present
   * in the column.
   *
   * @param   table the table containing the column
   * @param   column table column
   * @return the column preferred width
   */
  public int getColumnPreferredWidth(JTable table, TableColumn column) {
    int headerColumnWidth = getColumnHeaderWidth(table, column);
    int columnWidth = getWidestColumnCell(table, column);
    return headerColumnWidth > columnWidth ? headerColumnWidth : columnWidth;
  }

  private int getColumnHeaderWidth(JTable table, TableColumn column) {
    /*
     TableCellRenderer tableCellRenderer = column.getHeaderRenderer();
     System.out.println("renderer: " + tableCellRenderer);
     System.out.println("table, column: " + table + "----"
     + column.getHeaderValue());
     Component component = tableCellRenderer.getTableCellRendererComponent(table,
     column.getHeaderValue(),
     false, false, 0, 0);
     return component.getPreferredSize().width;
     */
    return 0;
  }

  private int getWidestColumnCell(JTable table, TableColumn column) {
    int columnIndex = column.getModelIndex();
    int width = 0;
    int maxWidth = 0;
    for (int row = 0; row < table.getRowCount(); row++) {
      TableCellRenderer tableCellRenderer = table.getCellRenderer(row,
          columnIndex);
      Component component = tableCellRenderer.getTableCellRendererComponent(
          table, table.getValueAt(row, columnIndex), false, false, row,
          columnIndex);
      width = component.getPreferredSize().width;
      maxWidth = width > maxWidth ? width : maxWidth;
    }
    return maxWidth;
  }

  void sortBy(JTable table, int columnIndex, boolean ascending) {
    int[] selectedRows = table.getSelectedRows();
    Vector selectedJobIdVector = new Vector();
    JobTableModel jobTableModel = (JobTableModel) table.getModel();
    TableColumnModel columnModel = table.getColumnModel();
    if (columnIndex == Utils.NO_SORTING) { // Table Adding Order.
      TableColumn column = null;
      for (int i = 0; i < table.getColumnCount(); i++) {
        column = columnModel.getColumn(i);
        column.setHeaderValue(yieldColumnName(table, column.getModelIndex(),
            false, ascending));
      }
      table.getTableHeader().repaint();
    } else {
      TableColumn column = null;
      for (int i = 0; i < table.getColumnCount(); i++) {
        column = columnModel.getColumn(i);
        if (column.getModelIndex() == columnIndex) {
          column.setHeaderValue(yieldColumnName(table, column.getModelIndex(),
              true, ascending));
        } else {
          column.setHeaderValue(yieldColumnName(table, column.getModelIndex(),
              false, ascending));
        }
      }
      table.getTableHeader().repaint();
      Vector vectorData = jobTableModel.getDataVector();
      Collections.sort(vectorData, new ColumnSorter(columnIndex, ascending));
    }
    jobTableModel.fireTableStructureChanged();
    table.repaint();
  }

  private String yieldColumnName(JTable table, int column, boolean addSign,
      boolean ascending) {
    String name = table.getTableHeader().getColumnModel().getColumn(column)
        .getHeaderValue().toString();
    int index = name.lastIndexOf("«");
    int index2 = name.lastIndexOf("»");
    if (index2 != -1) {
      index = index2;
    }
    if (index != -1) {
      name = name.substring(0, index).trim();
    }
    if (addSign) {
      if (ascending) {
        name += " «";
      } else {
        name += " »";
      }
    }
    return name;
  }
}
/*
 ************************
 CLASS HeaderListener
 ************************
 */
/*
 class HeaderListener extends MouseAdapter {
 protected JTable table;
 protected JobTableModel jobTableModel;
 protected MultipleJobPanel multipleJobPanel;
 public HeaderListener(MultipleJobPanel multipleJobPanel, JTable table) {
 this.table = table;
 this.jobTableModel = jobTableModel;
 this.multipleJobPanel = multipleJobPanel;
 }
 public void mouseClicked(MouseEvent me) {
 TableColumnModel columnModel = table.getColumnModel();
 int columnIndex = columnModel.getColumnIndexAtX(me.getX());
 int modelIndex = columnModel.getColumn(columnIndex).getModelIndex();
 if (modelIndex < 0) {
 return;
 }
 multipleJobPanel.sortingColumn = columnIndex;
 //if (
 for (int i = 0; i < table.getColumnCount(); i++) {
 TableColumn column = columnModel.getColumn(i);
 column.setHeaderValue(yieldColumnName(column.getModelIndex()));
 }
 table.getTableHeader().repaint();
 //multipleJobPanel.sortBy(modelIndex, true);
 jobTableModel.sortBy(multipleJobPanel, table, modelIndex, true);
 }
 }
 */
/*
 *******************************
 CLASS GUITableCellRenderer
 *******************************
 */
/*
 class GUITableCellRenderer extends DefaultTableCellRenderer  {
 MultipleJobPanel multipleJobPanel;
 public GUITableCellRenderer(Component component) {
 this.multipleJobPanel = (MultipleJobPanel) component;
 }
 // This method is called each time a cell in a column
 // using this renderer needs to be rendered.
 public Component getTableCellRendererComponent(JTable table, Object value,
 boolean isSelected, boolean hasFocus, int row, int column) {
 super.getTableCellRendererComponent(table, value, isSelected, hasFocus, row, column);
 setToolTipText(null);
 if (value instanceof String) {
 String text = value.toString().trim();
 if (!value.equals("")) {
 setToolTipText(text);
 }
 }
 if (isSelected) {
 setForeground(table.getSelectionForeground());
 super.setBackground(table.getSelectionBackground());
 if (column == MultipleJobPanel.JOB_STATUS_COLUMN_INDEX) {
 setFont(Utils.BOLD_FONT);
 }
 } else {
 setForeground(table.getForeground());
 if (column == MultipleJobPanel.JOB_STATUS_COLUMN_INDEX) {
 setBackground(table.getBackground());
 String jobIdText = table.getValueAt(row, MultipleJobPanel.JOB_ID_COLUMN_INDEX).toString();
 setHorizontalAlignment(SwingConstants.CENTER);
 setFont(Utils.BOLD_FONT);
 String state = table.getValueAt(row, MultipleJobPanel.JOB_STATUS_COLUMN_INDEX).toString();
 //System.out.println("RED" + Color.lightGray.getRed());
 //System.out.println("GREEN" + Color.lightGray.getGreen());
 //System.out.println("BLUE" + Color.lightGray.getBlue());
 if (state.indexOf(JobStatus.code[JobStatus.ABORTED]) != -1) {
 setBackground(new Color(255, 80, 100)); // 204, 50, 75; 225, 50, 75
 } else if (state.indexOf(JobStatus.code[JobStatus.SUBMITTED]) != -1) {
 setBackground(new Color(255, 212, 136)); // 255, 212, 136
 } else if (state.indexOf(JobStatus.code[JobStatus.WAITING]) != -1) {
 setBackground(new Color(255, 247, 178)); // 255, 255, 220
 } else if (state.indexOf(JobStatus.code[JobStatus.READY]) != -1) {
 setBackground(new Color(150, 200, 200)); // 255, 153, 0; 90, 250, 180
 } else if (state.indexOf(JobStatus.code[JobStatus.RUNNING]) != -1) {
 setBackground(new Color(140, 228, 185)); // 0, 208, 188
 } else if (state.indexOf(JobStatus.code[JobStatus.SCHEDULED]) != -1) {
 //setBackground(new Color(255, 247, 178));
 setBackground(new Color(255, 179, 128));
 } else if (state.indexOf(JobStatus.code[JobStatus.DONE]) != -1) {
 setBackground(new Color(204, 255, 255)); // 0, 162, 232
 if (state.indexOf(Utils.STATE_EXIT_CODE_NOT_ZERO) != -1) {
 //setForeground(new Color(255, 0, 0));
 setForeground(new Color(128, 0, 128));
 } else {
 setForeground(Color.black);
 }
 } else if (state.indexOf(JobStatus.code[JobStatus.CANCELLED]) != -1) {
 //setBackground(Color.lightGray);
 setBackground(new Color(176, 201, 201));
 } else if (state.indexOf(JobStatus.code[JobStatus.CLEARED]) != -1) {
 //setBackground(Color.lightGray);
 setBackground(new Color(153, 182, 204));
 } else if (state.indexOf(JobStatus.code[JobStatus.PURGED]) != -1) {
 //setBackground(Color.lightGray);
 setBackground(new Color(223, 223, 227));
 } else if (state.indexOf(JobStatus.code[JobStatus.UNDEF]) != -1) {
 setBackground(Color.lightGray);
 } else if (state.indexOf(JobStatus.code[JobStatus.UNKNOWN]) != -1) {
 //setBackground(Color.lightGray);
 setBackground(new Color(105, 109, 109));
 } else {
 setBackground(Color.white);
 }
 } else {
 setBackground(table.getBackground());
 }
 }
 return this;
 }
 }
 */
/*
 **************************************
 CLASS GUITableTooltipCellRenderer
 **************************************
 */

class GUITableTooltipCellRenderer extends DefaultTableCellRenderer {
  public Component getTableCellRendererComponent(JTable table, Object value,
      boolean isSelected, boolean hasFocus, int row, int column) {
    super.getTableCellRendererComponent(table, value, isSelected, hasFocus,
        row, column);
    setToolTipText(null);
    if (value instanceof String) {
      String text = value.toString().trim();
      if (!value.equals("")) {
        setToolTipText(text);
      }
    }
    return this;
  }
}
/*
 *************************************
 CLASS GUIListCellTooltipRenderer
 *************************************
 */

class GUIListCellTooltipRenderer extends DefaultListCellRenderer {
  public Component getListCellRendererComponent(JList list, Object value,
      int index, boolean isSelected, boolean hasFocus) {
    super
        .getListCellRendererComponent(list, value, index, isSelected, hasFocus);
    setToolTipText(null);
    if (value instanceof String) {
      String text = value.toString().trim();
      if (!value.equals("")) {
        setToolTipText(text);
      }
    }
    return this;
  }
}
/*
 *********************************
 CLASS GUICheckBoxCellRenderer
 *********************************
 */

class GUICheckBoxCellRenderer extends JCheckBox implements TableCellRenderer {
  protected static Border noFocusBorder;

  public GUICheckBoxCellRenderer() {
    super();
    setOpaque(true);
    setHorizontalAlignment(SwingConstants.CENTER);
  }

  public Component getTableCellRendererComponent(JTable table, Object value,
      boolean isSelected, boolean hasFocus, int row, int column) {
    if (value instanceof Boolean) {
      Boolean bool = (Boolean) value;
      setSelected(bool.booleanValue());
    }
    setBackground(isSelected ? table.getSelectionBackground() : table
        .getBackground());
    setForeground(isSelected ? table.getSelectionForeground() : table
        .getForeground());
    return this;
  }
}
/*
 class RendererDecorator implements TableCellRenderer {
 TableCellRenderer tableRenderer;
 JPanel jPanel;
 URL icon = JobDef1Panel.class.getResource("images/arrow_up.gif");
 JLabel iconLabel = new JLabel(new ImageIcon(icon));
 public RendererDecorator(TableCellRenderer tableRenderer) {
 this.tableRenderer = tableRenderer;
 //iconLabel.setBorder(BorderFactory.createEtchedBorder());
 }
 public Component getTableCellRendererComponent(JTable table, Object value,
 boolean isSelected, boolean hasFocus,
 int row, int col) {
 //Component component = tableRenderer.getTableCellRendererComponent(table,
 //  value, isSelected, hasFocus, row, col);
 Component component = new DefaultTableCellRenderer();
 embellish(component);
 return jPanel;
 }
 private void embellish(Component component) {
 if (jPanel == null) {
 jPanel = new JPanel();
 jPanel.setLayout(new BorderLayout());
 jPanel.add(component, BorderLayout.CENTER);
 jPanel.add(iconLabel, BorderLayout.WEST);
 jPanel.setPreferredSize(new Dimension(20, 20));
 }
 }
 }
 */
/*
 **********************
 CLASS ColumnSorter
 **********************
 */

class ColumnSorter implements Comparator {
  int columnIndex;

  boolean ascending;

  ColumnSorter(int columnIndex, boolean ascending) {
    this.columnIndex = columnIndex;
    this.ascending = ascending;
  }

  public int compare(Object a, Object b) {
    Vector v1 = (Vector) a;
    Vector v2 = (Vector) b;
    Object o1 = v1.get(columnIndex);
    Object o2 = v2.get(columnIndex);
    if (o1 instanceof String && ((String) o1).length() == 0) {
      o1 = null;
    }
    if (o2 instanceof String && ((String) o2).length() == 0) {
      o2 = null;
    }
    if (o1 == null && o2 == null) {
      return 0;
    } else if (o1 == null) {
      return 1;
    } else if (o2 == null) {
      return -1;
    } else if (o1 instanceof Comparable) {
      if (ascending) {
        return ((Comparable) o1).compareTo(o2);
      } else {
        return ((Comparable) o2).compareTo(o1);
      }
    } else {
      if (ascending) {
        return o1.toString().compareTo(o2.toString());
      } else {
        return o2.toString().compareTo(o1.toString());
      }
    }
  }
}