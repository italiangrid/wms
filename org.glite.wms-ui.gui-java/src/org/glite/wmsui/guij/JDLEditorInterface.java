/*
 * JDLEditorInterface.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.awt.event.ActionEvent;
import org.glite.jdl.JobAd;

/**
 * Implementation of the JDLEditorInterface interface.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public interface JDLEditorInterface {
  JobAd jobAdGlobal = new JobAd();

  //JButton jButtonReset = new JButton();
  //JMenuBar getJMenuBarReference();
  // get, set methods //
  String getUserWorkingDirectory();

  void setUserWorkingDirectory(String directory);

  String getNodeNumberValue();

  //String getJobTypeValue();
  String getHTMLParameter(String paramName);

  void setJTextAreaJDL(String JDLText);

  // view, display methods //
  void viewAll();

  String jButtonDataReqViewEvent(boolean showWarningMsg, boolean showErrorMsg,
      ActionEvent ae);

  void displayJPanelDesktop();

  // enabling methods //
  void setDef1StandardStreamsEnabled(boolean bool);

  void setListenerPortEnabled(boolean bool);

  // exit method //
  void exitApplication();
}