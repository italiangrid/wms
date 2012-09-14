/*
 * JobSubmitterInterface.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import org.glite.jdl.Ad;

/**
 * Implementation of the JobSubmitterInterface interface.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public interface JobSubmitterInterface {
  void addJobToTable(String rBName, String keyJobName,
      String currentOpenedFile, Ad jobAd);
}