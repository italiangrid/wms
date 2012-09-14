/*
 * ExprChecker.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.io.*;
import java.util.*;
import condor.classad.*;
import org.glite.jdl.JobAd;
import org.glite.jdl.JobAdException;
import java.text.ParseException;
import javax.naming.directory.InvalidAttributeValueException;
import org.apache.log4j.*;

/**
 * Implementation of the ExprChecker class.
 *
 * This class provides some parsing methods.
 * It is used to check the correctness of a file or string classad
 * representation and to create a JobAd containing attributes with
 * corresponding values.
 *
 * @ingroup gui
 * @brief Parses classad file and insert attributes in a JobAd.
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class ExprChecker {
  static Logger logger = Logger.getLogger(JDLEditor.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static final int NORMAL = 0;

  static final int XML = 1;

  String errorMsg = "";

  String warningMsg = "";

  Vector guiAttributesVector = new Vector();

  Vector errorAttributeVector = new Vector();

  Vector errorTypeMsgVector = new Vector();

  /**
   * Parses a file and creates a JobAd containing the found attributes. Local
   * check will be done.
   *
   * @param file  the file to parse.
   * @return      the jobAd containing attributes names and values, if the
   *              input string represent a correct jdl file and contains at
   *              least one attribute;
   *              a null jobAd otherwise.
   */
  public JobAd parse(File file) throws ParseException {
    return parse(file, true);
  }

  /**
   * Parses a file and creates a JobAd containing the found attributes. Local
   * check will be done depending on the value of localAccess parameter.
   *
   * @param file        the file to parse.
   * @param localAccess a boolean value indicating if local check must be done.
   * @return            the jobAd containing attributes names and values, if
   *                    the input string represent a correct jdl file and
   *                    contains at least one attribute;
   *                    a null jobAd otherwise.
   */
  public JobAd parse(File file, boolean localAccess) throws ParseException {
    return parse(file, true, NORMAL);
  }

  /**
   * Parses a file and creates a JobAd containing the found attributes. Local
   * check will be done depending on the value of <code>localAccess</code>
   * parameter.
   *
   * @param file        the file to parse.
   * @param localAccess a boolean value indicating if local check must be done.
   * @param type        the type of the file to parse: normal or XML
   * @return            the jobAd containing attributes names and values, if
   *                    the input string represent a correct jdl file and
   *                    contains at least one attribute;
   *                    a null jobAd otherwise.
   */
  public JobAd parse(File file, boolean localAccess, int type)
      throws ParseException {
    JobAd jobAd = new JobAd();
    jobAd.setLocalAccess(localAccess);
    if (type == NORMAL) { // JDL file
      try {
        jobAd.fromFile(file.getAbsolutePath());
      } catch (ParseException pe) {
        if (isDebugging) {
          pe.printStackTrace();
        }
        throw pe;
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
        errorMsg += e.getMessage() + "\n";
      }
    } else { // XML file
      FileInputStream fis = null;
      try {
        fis = new FileInputStream(file);
      } catch (Exception ex) {
        ex.printStackTrace();
      }
      ClassAdParser parser = new ClassAdParser(fis, ClassAdParser.XML);
      ByteArrayOutputStream baos = new ByteArrayOutputStream();
      PrintStream ps = new PrintStream(baos);
      parser.setErrorStream(ps);
      parser.setVerbosity(1);
      Expr expr = parser.parse();
      ListExpr listExpr = (ListExpr) expr;
      RecordExpr recordExpr = new RecordExpr();
      if (listExpr != null) {
        recordExpr = (RecordExpr) listExpr.sub(0);
      }
      errorMsg = baos.toString().trim();
      if (errorMsg.equals("null")) {
        errorMsg = "";
      } else {
        errorMsg = "- " + errorMsg;
      }
      String exprTxt = "";
      if (expr != null) {
        exprTxt = expr.toString().trim();
      }
      if (!exprTxt.equals("")) {
        exprTxt = exprTxt.substring(1, exprTxt.length() - 1);
      }
      try {
        //!!! change it when coded: jobAd.copy(recordExpr);
        jobAd.fromString(exprTxt);
      } catch (Exception ex) {
        if (isDebugging) {
          ex.printStackTrace();
        }
      }
    }
    if (errorMsg.equals("")) { // No XML parsing error
      try {
        jobAd.checkAll();
      } catch (JobAdException jae) {
        errorMsg += jae.getMessage();
        if (isDebugging) {
          jae.printStackTrace();
        }
      } catch (Exception e) {
        if (isDebugging) {
          e.printStackTrace();
        }
      }
    }
    parseErrorMsg(errorMsg);
    jobAd.toLines();
    return jobAd;
  }

  /**
   * Parses a file and creates a JobAd containing the found attributes. It also
   * add all the attributes contained in the JobAd passed as argument if
   * they are not already present. Local check will be done depending on the
   * value of <code>localAccess</code> parameter.
   *
   * @param file                 the file to parse.
   * @param localAccess          a boolean value indicating if local check must
   *                             be done.
   * @param jobAdAttributesToAdd a JobAd containing some attributes to add to
   *                             the returned JobAd (a JobAd to merge)
   * @return            the jobAd containing attributes names and values, if
   *                    the input string represent a correct jdl file and
   *                    contains at least one attribute;
   *                    a null jobAd otherwise.
   */
  public JobAd parse(File file, boolean localAccess, JobAd jobAdAttributeToAdd)
      throws ParseException {
    JobAd jobAd = new JobAd();
    jobAd.setLocalAccess(localAccess);
    try {
      jobAd.fromFile(file.getAbsolutePath());
    } catch (ParseException pe) {
      if (isDebugging) {
        pe.printStackTrace();
      }
      throw pe;
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      errorMsg += e.getMessage() + "\n";
    }
    if (jobAdAttributeToAdd != null) {
      Iterator iterator = jobAdAttributeToAdd.attributes();
      String attribute = "";
      for (; iterator.hasNext();) {
        attribute = iterator.next().toString();
        if (!jobAd.hasAttribute(attribute)) {
          try {
            jobAd
                .setAttribute(attribute, jobAdAttributeToAdd.lookup(attribute));
          } catch (Exception e) {
            if (isDebugging) {
              e.printStackTrace();
            }
          }
        }
      }
    }
    try {
      jobAd.checkAll();
    } catch (JobAdException jae) {
      errorMsg += jae.getMessage();
      if (isDebugging) {
        jae.printStackTrace();
      }
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    parseErrorMsg(errorMsg);
    jobAd.toLines();
    return jobAd;
  }

  /**
   * Parses an input string and creates a JobAd containing the found attributes.
   * Local check will be done.
   *
   * @param inputString   the input string to parse.
   * @return              the jobAd containing attributes names and values, if
   *                      the input string represent a correct jdl file and
   *                      contains at least one attribute;
   *                      a null jobAd otherwise.
   */
  public JobAd parse(String inputString) {
    return parse(inputString, true, null, null);
  }

  /**
   * Parses an input string and creates a JobAd containing the found attributes.
   * Local check will be done depending on the value of
   * <code>localAccess</code> parameter.
   *
   * @param inputString          the input string to parse.
   * @param localAccess          a boolean value indicating if local check must
   *                             be done.
   * @param defaultRequirements  requirements attribute default value.
   * @param defaultRank          rank attribute default value.
   * @return                     the jobAd containing attributes names and
   *                             values, if the input string represent a
   *                             correct jdl file and contains at least one
   *                             attribute;
   *                             a null jobAd otherwise.
   */
  public JobAd parse(String inputString, boolean localAccess,
      String defaultRequirements, String defaultRank) {
    // Construct attribute vector to create error message vectors.
    for (int i = 0; i < Utils.guiAttributesArray.length; i++) {
      guiAttributesVector.add(Utils.guiAttributesArray[i]);
    }
    JobAd jobAd = new JobAd();
    if ((defaultRequirements != null) && !defaultRequirements.equals("")) {
      jobAd.setDefaultRequirements(defaultRequirements);
    }
    if ((defaultRank != null) && !defaultRank.equals("")) {
      jobAd.setDefaultRank(defaultRank);
    }
    jobAd.setLocalAccess(localAccess);
    try {
      jobAd.fromString(inputString);
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
      errorMsg += e.getMessage() + "\n";
    }
    try {
      jobAd.checkAll();
    } catch (JobAdException jae) {
      if (isDebugging) {
        jae.printStackTrace();
      }
      errorMsg += jae.getMessage();
    } catch (Exception e) {
      if (isDebugging) {
        e.printStackTrace();
      }
    }
    parseErrorMsg(errorMsg);
    jobAd.toLines();
    return jobAd;
  }

  /**
   * Parses error message to produce a more readable format
   * @param error
   */
  void parseErrorMsg(String error) {
    //!!! Maybe it is better to use Tokenizer class.
    error = error.trim();
    error += "-";
    String singleErrorMsg = "";
    String attribute = "";
    String errorTypeMsg = "";
    int startingIndex = 0;
    int index = 0;
    int firstDash = 0;
    int secondDash = 0;
    for (;;) {
      firstDash = error.indexOf("-", startingIndex);
      secondDash = error.indexOf("-", firstDash + 1);
      if ((firstDash != -1) && (secondDash != -1)) {
        startingIndex = secondDash - 1;
        index = error.indexOf(":", firstDash);
        if (index != -1) {
          attribute = error.substring(firstDash + 2, index).trim();
          int commaIndex = attribute.indexOf(",");
          if (commaIndex != -1) {
            attribute = attribute.substring(0, commaIndex);
          }
          if (guiAttributesVector.contains(attribute)) {
            errorAttributeVector.add(attribute);
            errorTypeMsg = error.substring(firstDash, secondDash).trim();
            errorTypeMsgVector.add(errorTypeMsg);
          }
        }
      } else {
        return;
      }
    }
  }

  /**
   * Returns all parsing errors found during last parse operation.
   * Parsing errors are syntax errors or attribute duplications.
   *
   * @return a string containing all parsing errors found during last parse
   *         operation.
   */
  public String getErrorMsg() {
    return errorMsg;
  }

  public String getWarningMsg() {
    return warningMsg;
  }

  Vector getErrorAttributeVector() {
    return errorAttributeVector;
  }

  Vector getErrorTypeMsgVector() {
    return errorTypeMsgVector;
  }

  void printRecordExpr(JobAd jobAd) {
    logger.debug("");
    logger.debug("Number of attributes: " + jobAd.size() + ".");
    logger.debug("");
    Iterator iterator = jobAd.attributes();
    logger.debug("Attributes names:");
    for (; iterator.hasNext();) {
      logger.debug("  " + iterator.next());
    }
    logger.debug("");
    iterator = jobAd.attributes();
    logger.debug("Attributes values:");
    for (; iterator.hasNext();) {
      logger.debug("  " + jobAd.lookup(iterator.next().toString()));
    }
  }

  /**
   * Parses an input String representing a job. The check of the String is made
   * line by line in order to get all the present syntax errors or attribute
   * duplications
   *
   * @param inputString the String to parse
   * @return the JobAd representation of the job or null in case of error
   */
  public JobAd parseLineByLine(String inputString) {
    String trimmedInputString = inputString.trim();
    String inputExp = new String();
    String currentChar = new String();
    String attribute = new String();
    String value = new String();
    ClassAdParser cap = null;
    Expr valueExpr = null;
    JobAd jobAd = new JobAd();
    Iterator iterator = null;
    Vector rowInputStringVector = new Vector();
    int position = 0;
    int rowCount = 1;
    int rowCountNoComments = 1;
    int errorCount = 0;
    int duplicateAttributeCount = 0;
    int continueRowCount = 0;
    boolean isBraketPresent = false;
    errorMsg = "";
    // Check for squared brakets.
    int inputStringLength = trimmedInputString.length();
    if (inputStringLength == 0) {
      return null;
    }
    if (trimmedInputString.charAt(0) != '[') {
      errorMsg += "Input String should begin with a single '['";
      return null;
    } else {
      trimmedInputString = trimmedInputString.substring(1);
      inputStringLength--;
    }
    if (trimmedInputString.charAt(inputStringLength - 1) != ']') {
      errorMsg += "Input String should end with a single ']'";
      return null;
    } else {
      trimmedInputString = trimmedInputString.substring(0,
          inputStringLength - 1);
      inputStringLength--;
    }
    // Creating a lines vector.
    for (int i = 0; i < inputStringLength; i++) {
      currentChar = trimmedInputString.substring(i, i + 1);
      if (currentChar.equals("\n")) {
        logger.debug("Adding line: "
            + trimmedInputString.substring(position, i + 1));
        rowInputStringVector.addElement(trimmedInputString.substring(position,
            i + 1));
        position = i + 1;
      }
    }
    //rowInputStringVector.addElement(trimmedInputString.substring(position) + ";");
    //logger.debug("Adding line: " + trimmedInputString.substring(position) + "; *");
    //!!! Check it (null element)
    if (rowInputStringVector.size() != 0) {
      rowInputStringVector.addElement(null);
    }
    inputExp = (String) rowInputStringVector.get(0);
    int k = 1;
    while (k < rowInputStringVector.size()) {
      if ((rowCountNoComments >= Utils.ROW_BEFORE_NO_JDL_FILE_ERROR)
          && (jobAd.size() == 0)) {
        return null;
      }
      int i = 0;
      int j = 0;
      String trimmedInputExp = inputExp.trim();
      int length = trimmedInputExp.length();
      int trimmedInputExpLength = trimmedInputExp.length();
      // Check for blank line.
      if (length == 0) {
        logger.debug("I have found a blank line");
        inputExp = (String) rowInputStringVector.get(k);
        k++;
        rowCount++;
        continue;
      }
      // Check for comment line.
      if ((trimmedInputExp.charAt(0) == '#')
          || (length >= 2 && trimmedInputExp.substring(0, 2).equals("//"))) {
        logger.debug("I have found a comment line");
        inputExp = (String) rowInputStringVector.get(k);
        k++;
        rowCount++;
        continue;
      }
      // Check for line continuing in the next one.
      if (trimmedInputExp.charAt(length - 1) == '\\') {
        logger.debug("I have found '\\', expression continues next line.");
        inputExp = rTrim(inputExp);
        inputExp = inputExp.substring(0, inputExp.length() - 1)
            + ((String) rowInputStringVector.get(k));
        k++;
        rowCount++;
        rowCountNoComments++;
        continue;
      }
      if (trimmedInputExp.charAt(length - 1) != ';') {
        String line = (String) rowInputStringVector.get(k);
        logger.debug("line: " + line);
        if (line != null) {
          k++;
          logger.debug("Current line doesn't end with ';', "
              + "expression continues next line.");
          inputExp = inputExp + rTrim(line);
          continueRowCount++;
          rowCountNoComments++;
          logger.debug("Continuing inputExp: " + inputExp);
          continue;
        } else {
          inputExp = rTrim(inputExp) + ";";
          logger.debug("Continuing inputExp: " + inputExp);
        }
      }
      // Check current line for error(s). Check is line by line.
      String inputExpClassad = "[" + inputExp + "]";
      logger.debug("inputExp: " + "[" + inputExp + "]");
      // Added because classad parse method interprets slashes as a special
      // formatting chars. (if it finds '\', it takes the next char and try to
      // format: eg. \n -> "new line")
      //!!! check if needed with ClassAd 2.1!!
      inputExpClassad = Utils.addBackSlashes(inputExpClassad);
      cap = new ClassAdParser(inputExpClassad);
      ByteArrayOutputStream baos = new ByteArrayOutputStream();
      PrintStream ps = new PrintStream(baos);
      cap.setErrorStream(ps);
      cap.setVerbosity(1);
      Expr expr = cap.parse();
      RecordExpr recordExpr = (RecordExpr) expr;
      if (recordExpr == null) {
        // Formatting error messages.
        String tempErrorMsg = baos.toString().trim();
        if (!tempErrorMsg.equals("null")) {
          int linePosition = tempErrorMsg.indexOf("line");
          int columnPosition = tempErrorMsg.indexOf("column");
          if ((linePosition != -1) && (columnPosition != -1)) {
            // The error is like: "syntax error at line..."
            logger.debug("tempErrorMsg: " + tempErrorMsg);
            tempErrorMsg = tempErrorMsg.substring(0, linePosition + 5)
                + (Integer.parseInt(tempErrorMsg.substring(linePosition + 4,
                    columnPosition).trim(), 10) - 1 + rowCount)
                + " column "
                + (Integer.parseInt(tempErrorMsg.substring(columnPosition + 6)
                    .trim(), 10) - 1);
          }
          int openedSquaredBraketPosition = tempErrorMsg.indexOf("[");
          int closedSquaredBraketPosition = tempErrorMsg.indexOf("]");
          if ((openedSquaredBraketPosition != -1)
              && (closedSquaredBraketPosition != -1)) {
            tempErrorMsg = tempErrorMsg.substring(0,
                openedSquaredBraketPosition)
                + " "
                + tempErrorMsg.substring(openedSquaredBraketPosition + 1,
                    closedSquaredBraketPosition)
                + tempErrorMsg.substring(closedSquaredBraketPosition + 1);
          }
          errorMsg += "- " + tempErrorMsg + "\n";
          errorCount++;
        }
        inputExp = (String) rowInputStringVector.get(k);
        k++;
        rowCount += continueRowCount + 1;
        continueRowCount = 0;
        rowCountNoComments++;
        continue;
      }
      // recordExpr != null.
      iterator = recordExpr.attributes();
      for (; iterator.hasNext();) {
        attribute = (String) iterator.next().toString();
        logger.debug("Attribute in RecordExp: " + attribute);
        valueExpr = recordExpr.lookup(attribute);
        logger.debug("Value in RecordExp: " + valueExpr);
        try {
          jobAd.setAttribute(attribute.trim(), valueExpr);
        } catch (IllegalArgumentException iae) {
          duplicateAttributeCount++;
          errorMsg += "- " + iae.getMessage() + " at line " + rowCount + "\n";
        } catch (InvalidAttributeValueException iave) {
          errorMsg += "- " + iave.getMessage() + "\n";
        } catch (Exception e) {
          logger.debug("Exception: " + e.getMessage());
        }
      }
      inputExp = (String) rowInputStringVector.get(k);
      k++;
      rowCount += continueRowCount + 1;
      continueRowCount = 0;
      //rowCount++;
      rowCountNoComments++;
    }
    if (jobAd != null) {
      jobAd.toLines();
    }
    if (isDebugging) {
      printRecordExpr(jobAd);
    }
    return jobAd;
  }

  /**
   * Returns right trimmed input String
   * @param text the input String to trim
   * @return a right trimmed String
   */
  public String rTrim(String text) {
    if (text.equals(text.trim())) {
      return "";
    }
    int count = 0;
    int lastIndex = text.length() - 1;
    String currentChar;
    for (int i = 1; i <= lastIndex; i++) {
      currentChar = text.substring(lastIndex - i, lastIndex - i + 1);
      if (currentChar.equals("") || currentChar.equals("\n")) {
        count++;
      }
    }
    return text.substring(0, lastIndex - count);
  }

  /**
   * Checks a String representing a job. Only the attributes contained in the
   * String array will be checked.
   *
   * @param result            String to check
   * @param attributesToCheck the attributes array to check
   * @return                  a String containing error messages, if any
   */
  public static String checkResult(String result, String[] attributesToCheck) {
    return checkResult(result, attributesToCheck, true);
  }

  /**
   * Checks a String representing a job. Only the attributes contained in the
   * String array will be checked.
   *
   * @param result            String to check
   * @param attributesToCheck the attributes array to check
   * @param localAccess       declares if the local access check has to be made
   *                          or not
   * @return                  a String containing error messages, if any
   */
  public static String checkResult(String result, String[] attributesToCheck,
      boolean localAccess) {
    if (result.trim().equals("")) {
      return "";
    }
    String warningMsg = "";
    try {
      JobAd jobAdCheck = new JobAd(result);
      jobAdCheck.setLocalAccess(localAccess);
      jobAdCheck.checkAll(attributesToCheck);
    } catch (JobAdException jae) {
      warningMsg += jae.getMessage();
      if (isDebugging) {
        jae.printStackTrace();
      }
    } catch (IllegalArgumentException iae) {
      warningMsg += iae.getMessage();
      if (isDebugging) {
        iae.printStackTrace();
      }
    } catch (Exception ex) {
      //!!! Check for warning msg here! remove?
      //warningMsg += ex.getMessage();
      //if (isDebugging) {
      ex.printStackTrace();
      //}
    }
    return warningMsg;
  }
}