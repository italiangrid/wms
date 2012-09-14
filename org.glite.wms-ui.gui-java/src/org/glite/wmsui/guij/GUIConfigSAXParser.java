/*
 * GUIConfigSAXParser.java
 *
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://public.eu-egee.org/partners/ for details on the copyright holders.
 * For license conditions see the license file or http://www.eu-egee.org/license.html
 *
 */

package org.glite.wmsui.guij;

import java.io.FileInputStream;
import java.io.InputStream;
import java.util.Vector;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.SAXParseException;
import org.xml.sax.helpers.DefaultHandler;

/**
 * Implementation of the GUIConfigSAXParser class.
 *
 *
 * @ingroup gui
 * @brief
 * @version 1.0
 * @date 8 may 2002
 * @author Giuseppe Avellino <giuseppe.avellino@datamat.it>
 */
public class GUIConfigSAXParser extends DefaultHandler {
  static Logger logger = Logger.getLogger(JDLEditor.class.getName());

  static final boolean THIS_CLASS_DEBUG = false;

  static boolean isDebugging = THIS_CLASS_DEBUG || Utils.GLOBAL_DEBUG;

  static JDLConfigSAXHandler handler = null;

  public GUIConfigSAXParser() {
    super();
    isDebugging |= (Logger.getRootLogger().getLevel() == Level.DEBUG) ? true
        : false;
  }

  public Vector getReplicaCatalogVector() {
    return handler.getReplicaCatalogVector();
  }

  public Vector getDataAccessProtocolVector() {
    return handler.getDataAccessProtocolVector();
  }

  public Vector getAttributeDataVector() {
    Vector attributeDataVector = new Vector();
    attributeDataVector.add(handler.getAttributeNameVector());
    attributeDataVector.add(handler.getAttributeTypeVector());
    return attributeDataVector;
  }

  public Vector getFunctionDataVector() {
    Vector functionDataVector = new Vector();
    functionDataVector.add(handler.getFunctionNameVector());
    functionDataVector.add(handler.getFunctionTypeVector());
    functionDataVector.add(handler.getFunctionParamsVector());
    return functionDataVector;
  }

  public Vector getAttribute() {
    Vector attributeNameVector = handler.getAttributeNameVector();
    Vector attributeTypeVector = handler.getAttributeTypeVector();
    Vector attributeMultiVector = handler.getAttributeMultiVector();
    Vector attribute = new Vector();
    Integer tempInteger = new Integer(0);
    String isMultivaluedText = "";
    boolean isMultivalued = false;
    for (int i = 0; i < attributeNameVector.size(); i++) {
      isMultivaluedText = attributeMultiVector.get(i).toString();
      if (isMultivaluedText.equals("true"))
        isMultivalued = true;
      else
        isMultivalued = false;
      attribute.add(new Attribute(attributeNameVector.get(i).toString(),
          Integer.parseInt(attributeTypeVector.get(i).toString(), 10),
          isMultivalued));
    }
    return attribute;
  }

  public Vector getFunction() {
    Vector functionNameVector = handler.getFunctionNameVector();
    //System.out.println("*****NAME " + functionNameVector.size());
    Vector functionTypeVector = handler.getFunctionTypeVector();
    //System.out.println("*****TYPE " + functionTypeVector.size());
    Vector functionParamsVector = handler.getFunctionParamsVector();
    //System.out.println("*****PARAMS " + functionParamsVector.size());
    Vector function = new Vector();
    Integer tempInteger = new Integer(0);
    for (int i = 0; i < functionNameVector.size(); i++) {
      //System.out.println("DIMENSION:" + ((Vector) functionParamsVector.get(i)).size());
      function.add(new Function(functionNameVector.get(i).toString(), Integer
          .parseInt(functionTypeVector.get(i).toString(), 10),
          ((Vector) functionParamsVector.get(i)).size(),
          (Vector) functionParamsVector.get(i)));
    }
    return function;
  }

  public java.util.jar.Attributes getExprAttributesName() {
    return handler.getExprAttributesName();
  }

  public int parse(String fileName) {
    //System.out.println("file: " + fileName);
    try {
      handler = new JDLConfigSAXHandler();
      SAXParserFactory parserFactory = SAXParserFactory.newInstance();
      parserFactory.setValidating(true);
      parserFactory.setNamespaceAware(true);
      //URL url = new URL("jdleconfig.xml");
      // First used before Demo
      ////InputStream inputStream = GUIConfigSAXParser.class.getResourceAsStream(fileName);
      InputStream inputStream = new FileInputStream(fileName);
      InputSource inputSource = new InputSource(inputStream);
      SAXParser saxParser = parserFactory.newSAXParser();
      saxParser.parse(inputSource, handler);
    } catch (Exception e) {
      if (isDebugging)
        e.printStackTrace();
      return -1;
    }
    /*
     Vector attributeNameVector = handler.getAttributeNameVector();
     Vector attributeTypeVector = handler.getAttributeTypeVector();
     Vector functionNameVector = handler.getFunctionNameVector();
     Vector functionTypeVector = handler.getFunctionTypeVector();
     Vector functionParamsVector = handler.getFunctionParamsVector();
     Vector attributeMultiVector = handler.getAttributeMultiVector();

     Enumeration attributeNameEnumeration = attributeNameVector.elements();
     Enumeration attributeTypeEnumeration = attributeTypeVector.elements();
     Enumeration functionNameEnumeration = functionNameVector.elements();
     Enumeration functionTypeEnumeration = functionTypeVector.elements();
     Enumeration functionParamsEnumeration = functionParamsVector.elements();
     */
    /*
     for (; attributeNameEnumeration.hasMoreElements() ;) {
     System.out.println("AttributeName: " + attributeNameEnumeration.nextElement());
     }
     for (; attributeTypeEnumeration.hasMoreElements() ;) {
     System.out.println("AttributeType: " + attributeTypeEnumeration.nextElement());
     }
     for (; functionNameEnumeration.hasMoreElements() ;) {
     System.out.println("FunctionName: " + functionNameEnumeration.nextElement());
     }
     for (; functionTypeEnumeration.hasMoreElements() ;) {
     System.out.println("FunctionType: " + functionTypeEnumeration.nextElement());
     }
     for (; functionParamsEnumeration.hasMoreElements() ;) {
     System.out.println("FunctionParams0");
     Enumeration singleFunctionParamsEnumeration = ((Vector) functionParamsEnumeration.nextElement()).elements();
     for (; singleFunctionParamsEnumeration.hasMoreElements() ;) {
     System.out.println("FunctionParams: " + singleFunctionParamsEnumeration.nextElement());
     }
     }
     */
    return 0;
  }

  public static void main(String args[]) {
    try {
      InputStream inputStream = new FileInputStream(args[0]);
      handler = new JDLConfigSAXHandler();
      SAXParserFactory parserFactory = SAXParserFactory.newInstance();
      parserFactory.setValidating(true);
      parserFactory.setNamespaceAware(true);
      InputSource inputSource = new InputSource(inputStream);
      SAXParser saxParser = parserFactory.newSAXParser();
      saxParser.parse(inputSource, handler);
    } catch (Exception e) {
      if (isDebugging)
        e.printStackTrace();
    }
    /*
     Vector attributeNameVector = handler.getAttributeNameVector();
     Vector attributeTypeVector = handler.getAttributeTypeVector();
     Vector functionNameVector = handler.getFunctionNameVector();
     Vector functionTypeVector = handler.getFunctionTypeVector();
     Vector functionParamsVector = handler.getFunctionParamsVector();
     Vector attributeMultiVector = handler.getAttributeMultiVector();

     Enumeration attributeNameEnumeration = attributeNameVector.elements();
     Enumeration attributeTypeEnumeration = attributeTypeVector.elements();
     Enumeration functionNameEnumeration = functionNameVector.elements();
     Enumeration functionTypeEnumeration = functionTypeVector.elements();
     Enumeration functionParamsEnumeration = functionParamsVector.elements();
     Enumeration attributeMultiEnumeration = attributeMultiVector.elements();
     */
    /*
     for (; attributeNameEnumeration.hasMoreElements() ;) {
     System.out.println("AttributeName: " + attributeNameEnumeration.nextElement());
     }
     for (; attributeTypeEnumeration.hasMoreElements() ;) {
     System.out.println("AttributeType: " + attributeTypeEnumeration.nextElement());
     }
     for (; functionNameEnumeration.hasMoreElements() ;) {
     System.out.println("FunctionName: " + functionNameEnumeration.nextElement());
     }
     for (; functionTypeEnumeration.hasMoreElements() ;) {
     System.out.println("FunctionType: " + functionTypeEnumeration.nextElement());
     }
     for (; functionParamsEnumeration.hasMoreElements() ;) {
     System.out.println("FunctionParams0");
     Enumeration singleFunctionParamsEnumeration = ((Vector) functionParamsEnumeration.nextElement()).elements();
     for (; singleFunctionParamsEnumeration.hasMoreElements() ;) {
     System.out.println("FunctionParams: " + singleFunctionParamsEnumeration.nextElement());
     }
     }
     for (; attributeMultiEnumeration.hasMoreElements() ;) {
     System.out.println("AttributeMulti: " + attributeMultiEnumeration.nextElement());
     }
     */
  }
}

class JDLConfigSAXHandler extends DefaultHandler {
  Vector replicaCatalogVector = new Vector();

  Vector dataAccessProtocolVector = new Vector();

  Vector attributeNameVector = new Vector();

  Vector attributeTypeVector = new Vector();

  Vector functionNameVector = new Vector();

  Vector functionTypeVector = new Vector();

  Vector functionParamsVector = new Vector();

  Vector thisFunctionParamsVector = new Vector();

  Vector attributeMultiVector = new Vector();

  Vector attributeVector = new Vector();

  Vector functionVector = new Vector();

  java.util.jar.Attributes exprAttributesName = new java.util.jar.Attributes();

  static boolean isAttribute = false;

  static boolean isFunction = false;

  static boolean isParams = false;

  static boolean isMultivalued = false;

  String value = "";

  Integer integerValue = new Integer(Utils.INTEGER);

  Integer floatValue = new Integer(Utils.FLOAT);

  Integer stringValue = new Integer(Utils.STRING);

  Integer booleanValue = new Integer(Utils.BOOLEAN);

  Integer absTimeValue = new Integer(Utils.ABS_TIME);

  Integer relTimeValue = new Integer(Utils.REL_TIME);

  Integer memoryValue = new Integer(Utils.MEMORY);

  Integer secondsValue = new Integer(Utils.SECONDS);

  Integer listValue = new Integer(Utils.LIST);

  Integer valueValue = new Integer(Utils.VALUE);

  Vector getReplicaCatalogVector() {
    return replicaCatalogVector;
  }

  Vector getDataAccessProtocolVector() {
    return dataAccessProtocolVector;
  }

  Vector getAttributeNameVector() {
    return attributeNameVector;
  }

  Vector getAttributeTypeVector() {
    return attributeTypeVector;
  }

  Vector getFunctionNameVector() {
    return functionNameVector;
  }

  Vector getFunctionTypeVector() {
    return functionTypeVector;
  }

  Vector getFunctionParamsVector() {
    return functionParamsVector;
  }

  Vector getAttributeMultiVector() {
    return attributeMultiVector;
  }

  java.util.jar.Attributes getExprAttributesName() {
    return exprAttributesName;
  }

  public void error(SAXParseException e) {
    //System.out.println("Inside error: " + e.getMessage());
  }

  public void warning(SAXParseException e) {
    //System.out.println("Inside warning: " + e.getMessage());
  }

  public void startDocument() {
    //System.out.println("Start document");
  }

  public void endDocument() {
    //System.out.println("End document");
  }

  public void startElement(String uri, String name, String qName,
      Attributes atts) {
    if (name.equals("jdleconfig")) {
    } else if (name.equals("attribute")) {
      //System.out.println("attribute");
      isAttribute = true;
    } else if (name.equals("function")) {
      //System.out.println("function");
      isFunction = true;
    } else if (name.equals("params")) {
      //System.out.println("params");
      isParams = true;
    } else if (name.equals("multi")) {
      //System.out.println("multi");
      isMultivalued = true;
    } else if (name.equals("exprAttributeName")) {
      String attributeName;
      for (int i = 0; i < atts.getLength(); i++) {
        attributeName = atts.getQName(i).trim();
        exprAttributesName.putValue(attributeName, atts.getValue(attributeName)
            .trim());
      }
    }
  }

  public void endElement(String uri, String name, String qName) {
    //System.out.println("uri: " + uri + " name: " + name + " qName: " + qName);
    //System.out.println("value: " + value);
    if (name.equals("jdleconfig")) {
    } else if (name.equals("replicacatalog")) {
      replicaCatalogVector.add(value);
    } else if (name.equals("dataaccessprotocol")) {
      dataAccessProtocolVector.add(value);
    } else if (name.equals("attribute")) {
      //System.out.println("attribute");
      if (isMultivalued)
        attributeMultiVector.add("true");
      else
        attributeMultiVector.add("false");
      isMultivalued = false;
      isAttribute = false;
    } else if (name.equals("function")) {
      //System.out.println("function");
      isFunction = false;
      functionParamsVector.add(thisFunctionParamsVector);
      thisFunctionParamsVector = new Vector();
      //Function function = new Function(
    } else if (name.equals("params")) {
      //System.out.println("params");
      isParams = false;
    } else if (name.equals("name")) {
      if (isAttribute) {
        attributeNameVector.add(value);
      } else if (isFunction)
        functionNameVector.add(value);
    } else if (name.equals("INTEGER")) {
      if (isFunction && isParams) {
        thisFunctionParamsVector.add(integerValue);
      } else if (isAttribute) {
        attributeTypeVector.add(integerValue);
      } else if (isFunction) {
        functionTypeVector.add(integerValue);
      }
    } else if (name.equals("BOOLEAN")) {
      if (isFunction && isParams) {
        thisFunctionParamsVector.add(booleanValue);
      } else if (isAttribute) {
        attributeTypeVector.add(booleanValue);
      } else if (isFunction) {
        functionTypeVector.add(booleanValue);
      }
    } else if (name.equals("FLOAT")) {
      if (isFunction && isParams) {
        thisFunctionParamsVector.add(floatValue);
      } else if (isAttribute) {
        attributeTypeVector.add(floatValue);
      } else if (isFunction) {
        functionTypeVector.add(floatValue);
      }
    } else if (name.equals("STRING")) {
      if (isFunction && isParams) {
        thisFunctionParamsVector.add(stringValue);
      } else if (isAttribute) {
        attributeTypeVector.add(stringValue);
      } else if (isFunction) {
        functionTypeVector.add(stringValue);
      }
    } else if (name.equals("ABSTIME")) {
      if (isFunction && isParams) {
        thisFunctionParamsVector.add(absTimeValue);
      } else if (isAttribute) {
        attributeTypeVector.add(absTimeValue);
      } else if (isFunction) {
        functionTypeVector.add(absTimeValue);
      }
    } else if (name.equals("RELTIME")) {
      if (isFunction && isParams) {
        thisFunctionParamsVector.add(relTimeValue);
      } else if (isAttribute) {
        attributeTypeVector.add(relTimeValue);
      } else if (isFunction) {
        functionTypeVector.add(relTimeValue);
      }
    } else if (name.equals("MEMORY")) {
      if (isFunction && isParams) {
        thisFunctionParamsVector.add(memoryValue);
      } else if (isAttribute) {
        attributeTypeVector.add(memoryValue);
      } else if (isFunction) {
        functionTypeVector.add(memoryValue);
      }
    } else if (name.equals("SECONDS")) {
      if (isFunction && isParams) {
        thisFunctionParamsVector.add(secondsValue);
      } else if (isAttribute) {
        attributeTypeVector.add(secondsValue);
      } else if (isFunction) {
        functionTypeVector.add(secondsValue);
      }
    } else if (name.equals("LIST")) {
      if (isFunction && isParams) {
        thisFunctionParamsVector.add(listValue);
      } /*else if(isAttribute) {
       attributeTypeVector.add(listValue);

       } else if(isFunction) {
       functionTypeVector.add(listValue);

       }*/
    } else if (name.equals("VALUE")) {
      if (isFunction && isParams) {
        thisFunctionParamsVector.add(valueValue);
      } /*else if(isAttribute) {
       attributeTypeVector.add(valueValue);

       } else if(isFunction) {
       functionTypeVector.add(valueValue);

       }*/
    }
  }

  // This method is necessary for XML parsing.
  public void characters(char ch[], int start, int length) {
    value = "";
    for (int i = start; i < start + length; i++) {
      switch (ch[i]) {
        case '\\':
        case '"':
        case '\n':
        case '\r':
        case '\t':
        break;
        default:
          value += ch[i];
        break;
      }
    }
  }
}