/*
 * GraphicUtils.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Frame;
import java.awt.GridBagConstraints;
import java.awt.Insets;
import java.awt.Toolkit;
import java.awt.Window;
import java.awt.event.FocusEvent;
import java.awt.event.KeyEvent;
import java.util.Vector;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JTextField;
import javax.swing.KeyStroke;
import javax.swing.SwingConstants;
import javax.swing.border.EmptyBorder;

/**
 * Implementation of the GraphicUtils class.
 * This class provides graphic utility methods.
 *
 * @ingroup gui
 * @brief This class provides some graphic utility methods.
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class GraphicUtils {
  //static final int STARTUP_SELECTED_TABBED_PANE_INDEX = 1;

  // Screen proportion.
  static final double SCREEN_WIDTH_PROPORTION = 0.8;
  
  static final double SCREEN_WIDTH_INFO_DETAILS_PROPORTION = 0.66;
  
  static final double SCREEN_WIDTH_INFO_DETAILS_PREFERRED_PROPORTION = 0.33;

  static final double SCREEN_HEIGHT_PROPORTION = 0.75;

  static final int NS_NAME_MAX_CHAR_NUMBER = 8;

  static final int KEY_EVENT_MASK = KeyEvent.CTRL_MASK;

  static final KeyStroke CUT_ACCELERATOR = KeyStroke.getKeyStroke(
      KeyEvent.VK_X, KeyEvent.CTRL_MASK | KeyEvent.SHIFT_DOWN_MASK);

  static final KeyStroke COPY_ACCELERATOR = KeyStroke.getKeyStroke(
      KeyEvent.VK_C, KeyEvent.CTRL_MASK | KeyEvent.SHIFT_DOWN_MASK);

  static final KeyStroke PASTE_ACCELERATOR = KeyStroke.getKeyStroke(
      KeyEvent.VK_V, KeyEvent.CTRL_MASK | KeyEvent.SHIFT_DOWN_MASK);

  static final Color TITLED_ETCHED_BORDER_COLOR = new Color(0, 0, 170);

  static final Color JTEXTFIELD_DEFAULT_BACK = new Color(200, 224, 216);

  //static final Color LIGHT_YELLOW = new Color(255, 255, 220);
  static final Color LIGHT_YELLOW = new Color(210, 210, 210); //light grey 192 192 192

  static final Font BOLD_FONT = new Font("Dialog", 1, 12);

  static final int H_GAP = 3;

  static final int V_GAP = 3;

  static final int STRUT_GAP = 3;

  static final EmptyBorder SPACING_BORDER = new EmptyBorder(4, 4, 4, 4);

  static final Dimension NUMERIC_TEXT_FIELD_DIMENSION = new Dimension(35, 28);

  static final Dimension ARROW_BUTTON_DIMENSION = new Dimension(30, 30);

  /**********
   * METHODS
   **********/
  static int showOptionDialogMsg(Component component, String inputMsg,
      String title, int optionType, int messageType, int lineNumber,
      String firstMessageLine, String lastMessageLine)
      throws IllegalArgumentException {
    if ((optionType != JOptionPane.DEFAULT_OPTION)
        && (optionType != JOptionPane.YES_NO_OPTION)) {
      throw new IllegalArgumentException("Illegal optionType argument");
    }
    if (firstMessageLine == null) {
      firstMessageLine = "";
    }
    if (lastMessageLine == null) {
      lastMessageLine = "";
    }
    Vector messageVector = new Vector();
    String message = "";
    int j = 0;
    int k = 0;
    int l = 0;
    char currentChar;
    for (int i = 0; i < inputMsg.length(); i++) {
      currentChar = inputMsg.charAt(i);
      if (currentChar == '\n') {
        k = i;
        message += inputMsg.substring(j, k).trim() + "\n";
        j = i;
        l++;
      }
      if (l == lineNumber) {
        messageVector.add(message);
        l = 0;
        message = "";
      }
    }
    message += inputMsg.substring(j, inputMsg.length()).trim() + "\n";
    if (!message.trim().equals("")) {
      messageVector.add(message);
    }
    int messageVectorSize = messageVector.size();
    if (messageVectorSize > 0) {
      JOptionPane jOptionPane = new JOptionPane();
      JLabel jLabelCount = new JLabel();
      jLabelCount.setPreferredSize(new Dimension(160, 26));
      jLabelCount.setHorizontalAlignment(SwingConstants.RIGHT);
      JLabel jLabelFill = new JLabel();
      jLabelFill.setPreferredSize(new Dimension(2, 26));
      jLabelFill.setHorizontalAlignment(SwingConstants.RIGHT);
      JButton jButtonPrev = new JButton("Prev");
      jButtonPrev.setEnabled(false);
      JButton jButtonNext = new JButton("Next");
      jButtonNext.setEnabled(false);
      Object[] buttons11 = { "Ok"
      };
      Object[] buttons12 = { "Yes", "No"
      };
      Object[] buttons21 = { "Ok", jLabelCount, jLabelFill, jButtonPrev, "Next"
      };
      Object[] buttons22 = { "Yes", "No", jLabelCount, jLabelFill, jButtonPrev,
          "Next"
      };
      if (!firstMessageLine.equals("")) {
        firstMessageLine = firstMessageLine + "\n";
      }
      if (!lastMessageLine.equals("")) {
        lastMessageLine = "\n" + lastMessageLine;
      }
      if (messageVectorSize == 1) {
        int choice = JOptionPane.showOptionDialog(component, firstMessageLine
            + messageVector.get(0).toString() + lastMessageLine, title,
            optionType, messageType, null,
            (optionType == JOptionPane.DEFAULT_OPTION) ? buttons11 : buttons12,
            null);
        return choice;
      } else {
        switch (optionType) {
          case JOptionPane.DEFAULT_OPTION: {
            int choice = 3;
            int index = 0;
            String count = "";
            while ((choice == 3) || (choice == 4)) {
              count = "Message Page: " + (index + 1) + "/"
                  + messageVector.size();
              count = "<html><font color=\"#602080\">" + count + "</font>";
              jLabelCount.setText(count);
              choice = JOptionPane.showOptionDialog(component, firstMessageLine
                  + messageVector.get(index).toString() + lastMessageLine,
                  title, optionType, messageType, null, buttons21, null);
              if ((choice == 3) && (index > 0)) {
                index--;
                buttons21[4] = "Next";
                if ((index) == 0) {
                  buttons21[3] = jButtonPrev;
                } else {
                  buttons21[3] = "Prev";
                }
              }
              if ((choice == 4) && (index < messageVector.size() - 1)) {
                index++;
                buttons21[3] = "Prev";
                if ((index) == messageVector.size() - 1) {
                  buttons21[4] = jButtonNext;
                } else {
                  buttons21[4] = "Next";
                }
              }
            }
            return choice;
          }
          case JOptionPane.YES_NO_OPTION: {
            int choice = 4;
            int index = 0;
            String count = "";
            while ((choice == 4) || (choice == 5)) {
              count = "Message Page: " + (index + 1) + "/"
                  + messageVector.size();
              count = "<html><font color=\"#602080\">" + count + "</font>";
              jLabelCount.setText(count);
              choice = JOptionPane.showOptionDialog(component, firstMessageLine
                  + messageVector.get(index).toString() + lastMessageLine,
                  title, optionType, messageType, null, buttons22, null);
              if ((choice == 4) && (index > 0)) {
                index--;
                buttons22[5] = "Next";
                if ((index) == 0) {
                  buttons22[4] = jButtonPrev;
                } else {
                  buttons22[4] = "Prev";
                }
              }
              if ((choice == 5) && (index < messageVector.size() - 1)) {
                index++;
                buttons22[4] = "Prev";
                if ((index) == messageVector.size() - 1) {
                  buttons22[5] = jButtonNext;
                } else {
                  buttons22[5] = "Next";
                }
              }
            }
            return choice;
          }
        }
      }
    }
    return -1;
  }

  static void jTextFieldDeselect(JComponent jComponent) {
    JTextField jTextField = null;
    if (jComponent instanceof JTextField) {
      jTextField = (JTextField) jComponent;
    } else {
      System.exit(-1);
    }
    if (jTextField.getText().trim().equals("")) {
      jTextField.setText("");
    }
    jTextField.select(0, 0);
  }

  static void jTextFieldFocusLost(JComponent jComponent, int Type,
      String defaultValue, float minValue, double maxValue) {
    if (maxValue == Utils.INFINITE) {
      maxValue = Double.MAX_VALUE;
    }
    JTextField jTextField = (JTextField) jComponent;
    jTextField.select(0, 0); // Deselect.
    boolean result = Utils.verify(jTextField, Type, minValue, maxValue);
    if (!result) {
      jTextField.setText(defaultValue);
      jTextField.grabFocus();
      jTextField.selectAll();
    }
  }

  static void jTextFieldFocusLost(FocusEvent e) {
    jTextFieldDeselect((JTextField) e.getSource());
  }

  public static void deiconifyFrame(Frame frame) {
    int state = frame.getExtendedState();
    state &= ~Frame.ICONIFIED;
    frame.setExtendedState(state);
  }

  public static void screenCenterWindow(Window window) {
    Toolkit toolkit = Toolkit.getDefaultToolkit();
    int screenHeight = toolkit.getScreenSize().height;
    int screenWidth = toolkit.getScreenSize().width;
    int height = window.getBounds().height;
    int width = window.getBounds().width;
    if ((screenHeight < height) || (screenWidth < width)) {
      return;
    }
    window.setLocation((screenWidth - width) / 2, (screenHeight - height) / 2);
  }

  public static void windowCenterWindow(Window parentWindow, Window window) {
    int parentX = parentWindow.getBounds().x;
    int parentY = parentWindow.getBounds().y;
    int parentHeight = parentWindow.getBounds().height;
    int parentWidth = parentWindow.getBounds().width;
    int windowX = parentWindow.getBounds().x;
    int windowY = parentWindow.getBounds().y;
    int windowHeight = window.getBounds().height;
    int windowWidth = window.getBounds().width;
    int x = (parentWidth > windowWidth) ? (parentWidth - windowWidth) / 2
        + parentX : windowX - (windowWidth - parentWidth) / 2;
    int y = (parentHeight > windowHeight) ? (parentHeight - windowHeight) / 2
        + parentY : windowY - (windowHeight - parentHeight) / 2;
    if (x < 0) {
      x = 0;
    }
    if (y < 0) {
      y = 0;
    }
    window.setLocation(x, y);
  }

  static void closeAllEditorFrames() {
    Object[] values = GUIGlobalVars.openedEditorHashMap.values().toArray();
    for (int i = 0; i < values.length; i++) {
      ((JDLEditor) values[i]).dispose();
    }
    GUIGlobalVars.openedEditorHashMap.clear();
  }

  static boolean comboBoxContains(JComboBox combo, String string) {
    for (int i = 0; i < combo.getItemCount(); i++) {
      if (combo.getItemAt(i).toString().equals(string)) {
        return true;
      }
    }
    return false;
  }

  static GridBagConstraints setDefaultGridBagConstraints(GridBagConstraints gbc) {
    return setGridBagConstraints(gbc, 0, 0, 1, 1, 0.0, 0.0,
        GridBagConstraints.CENTER, GridBagConstraints.NONE, null, 0, 0);
  }

  static GridBagConstraints setGridBagConstraints(GridBagConstraints gbc,
      int gridx, int gridy, int gridwidth, int gridheight, double weightx,
      double weighty, int anchor, int fill, Insets insets, int ipadx, int ipady) {
    gbc.gridx = gridx;
    gbc.gridy = gridy;
    gbc.gridwidth = gridwidth;
    gbc.gridheight = gridheight;
    gbc.weightx = weightx;
    gbc.weighty = weighty;
    gbc.anchor = anchor;
    gbc.fill = fill;
    if (insets != null) {
      gbc.insets = insets;
    }
    gbc.ipadx = ipadx;
    gbc.ipady = ipady;
    return gbc;
  }
}