/**
 * GenXmlHandler.java
 *
 * Created on August 3, 2005, 11:20 AM
 *
 */

package borgui.generator;

import java.util.Iterator;
import java.util.List;
import org.jdom.*;
import org.jdom.filter.*;

import borgui.xml.*;
import java.util.Date;
import java.util.Vector;

/**
 * A class that wrap the distribution XML document.
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/14/2005
 */
public class GenXmlHandler
{
    
    Document m_doc = new Document();
    
    /** 
     * Creates a new instance of GenXmlHandler 
     */
    public GenXmlHandler() {}
    
    /**
     * Create distribution node, and add it to corresponding field node.
     *
     * @param doc The distribution XML document
     * @param schemaName the name of the schema which the field belong to
     * @param fieldName the name of the field
     * @param distribution the Distribution that will be set to the field
     */
    public static void setFieldElement(Document doc, String schemaName, String fieldName, Distribution distribution)
    {
        Element  root = doc.getRootElement();

        Element  schemaElement = findFirstElementChild(root,
                                                       XmlConstants.SCHEMA_ELEMENT,
                                                       XmlConstants.NAME_ATTRIBUTE,
                                                       schemaName);

        Element  fieldElement  = findFirstElementChild(schemaElement,
                                                       XmlConstants.FIELD_ELEMENT,
                                                       XmlConstants.NAME_ATTRIBUTE,
                                                       fieldName);

        Element        distElement = createDistElement(distribution);
        ElementFilter  distFilter  = new ElementFilter(GenConstants.DIST_ELEM_NAME);

        fieldElement.removeContent(distFilter);
        fieldElement.addContent(distElement);
    }

    
    /**
     * Create distribution node, and add it to corresponding input stream node.
     *
     * @param doc The distribution XML document
     * @param inputName the name of the input stream
     * @param distribution the Distribution that will be set to the field
     */
    public static void setInputElement(Document      doc,
                                       String        inputName,
                                       Distribution  distribution)
    {
        Element  root = doc.getRootElement();

        Element  inputElement = findFirstElementChild(root,
                                                      XmlConstants.INPUT_ELEMENT,
                                                      XmlConstants.INPUT_STREAM_ATTRIBUTE,
                                                      inputName);

        Element        distElement = createDistElement(distribution);
        ElementFilter  distFilter  = new ElementFilter(GenConstants.DIST_ELEM_NAME);

        inputElement.removeContent(distFilter);
        inputElement.addContent(distElement);
    }

    
    /**
     * Locate the child element by the given parent node. It must have a given attribute name and value.
     *
     * @param elementTop this method will locate child element under this elementTop
     * @param elementName The name of the element that be located
     * @param elementAttrName the given attribute name
     * @param elementAttrValue the given attribute value
     */
    protected static Element findFirstElementChild(Element elementTop, String elementName, String elementAttrName, String elementAttrValue)
    {
        Element result = null;

        if (elementTop!=null)
        {
            // get all elements of this name
            ElementFilter filter = new ElementFilter(elementName);
            List elementList = elementTop.getContent(filter);
            
            // look through all these elements
            for(Iterator i = elementList.iterator(); i.hasNext() && result == null; )
            {
                Element e = (Element) i.next();
                // check to see if it has the correct attribute value
                String val = e.getAttributeValue(elementAttrName);
                
                if (val != null)
                {
                    if (elementAttrValue.equals(val))
                    {
                        result = e;
                    }
                }
            }
        }

        return result;
    }
    
    /**
     * Create a distribution node by the given distribution information
     *
     * @param distribution the given distribution information
     */
    public static Element createDistElement(Distribution distribution)
    {
        if (distribution!=null)
        {
            Element result = new Element(GenConstants.DIST_ELEM_NAME);
            String attrValue = GenConstants.DIST_NAME[distribution.m_type];
            Attribute attr = new Attribute(GenConstants.DIST_TYPE_ATTR, attrValue);

            result.setAttribute(attr);
            
            int paramsNum = GenConstants.DIST_PARAM_NUM[distribution.m_type];

            for (int i = 0; i<paramsNum; i++)
            {
                Element paramDistElement = createParamDistElement(GenConstants.DIST_PARAM_MATRIX[distribution.m_type][i],
                        distribution.getParamDist(i));
                result.addContent(paramDistElement);
            }
            
            return result;
        }
        else
        {   return new Element(GenConstants.DIST_ELEM_NAME);
        }
    }
   
    /**
     * Create a ParamDistribution node by the given ParamDistribution information
     *
     * @param paramDistribution the given distribution information
     */
    public static Element createParamDistElement(String paramName, ParamDistribution paramDistribution)
    {
        Element result = new Element(GenConstants.PARAM_DIST_ELEM_NAME);

        if (paramDistribution!=null)
        {
            String nameAttrValue = paramName;
            Attribute nameAttr = new Attribute(GenConstants.DIST_NAME_ATTR, nameAttrValue);
            result.setAttribute(nameAttr);
            
            String typeAttrValue = GenConstants.DIST_NAME[paramDistribution.m_type];
            Attribute typeAttr = new Attribute(GenConstants.DIST_TYPE_ATTR,typeAttrValue);
            result.setAttribute(typeAttr);
            
            int paramsNum = GenConstants.DIST_PARAM_NUM[paramDistribution.m_type];

            for (int i = 0; i<paramsNum; i++)
            {
                Element param = new Element(GenConstants.PARAM_ELEM_NAME);
                
                String name = GenConstants.DIST_PARAM_MATRIX[paramDistribution.m_type][i];
                Attribute attr1 = new Attribute(GenConstants.DIST_NAME_ATTR, name);
                param.setAttribute(attr1);
                
                String value = paramDistribution.getParamDistParamValue(i).toString();
                Attribute attr2 = new Attribute(GenConstants.PARAM_VALUE_ATTR,value);
                param.setAttribute(attr2);
                
                result.addContent(param);
            }
        }

        return result;
    }
    
    /**
     * Return the field element by given the XML document, the field name and the schema name that the field belong to.
     *
     * @param doc The distribution XML document
     * @param schemaName the name of the schema which the field belong to
     * @param fieldName the name of the field
     */
    public static Element getFieldDistElement(Document doc, String schemaName, String fieldName)
    {
        Element result = null;
        Element root = doc.getRootElement();

        Element schemaElement = findFirstElementChild(root,
                                         XmlConstants.SCHEMA_ELEMENT,
                                         XmlConstants.NAME_ATTRIBUTE,
                                                      schemaName);

        Element fieldElement = findFirstElementChild(schemaElement,
                                        XmlConstants.FIELD_ELEMENT,
                                        XmlConstants.NAME_ATTRIBUTE,
                                                     fieldName);

        Element distElement = fieldElement.getChild(GenConstants.DIST_ELEM_NAME);
        result = distElement;
        return result;
    }

    
    /**
     * Return the input stream element by given the XML document, the stream name.
     *
     * @param doc The distribution XML document
     * @param streamName the name of the stream
     */
    public static Element getInputDistElement(Document doc, String streamName)
    {
        Element result = null;
        Element root = doc.getRootElement();

        Element inputElement = findFirstElementChild(root,
                                        XmlConstants.INPUT_ELEMENT,
                                        XmlConstants.INPUT_STREAM_ATTRIBUTE,
                                                     streamName);

        Element distElement = inputElement.getChild(GenConstants.DIST_ELEM_NAME);
        result = distElement;

        return result;
    }

   
    /**
     * Create a distribution by the given distribution element
     * 
     * @param distElem the given distribution element
     */
    public static Distribution createDist(Element distElement)
    {
        Distribution result = new Distribution();
        int type = getDistTypeInt(distElement);
        result.setDistType(type);

        for (int i = 0; i<GenConstants.DIST_PARAM_NUM[type]; i++)
        {
            result.m_paramsDist[i] = createParamDist(distElement, i);
        }

        return result;
    }

    
    /**
     * Return the distribution type (integer) by the given distribution element
     *
     * @param distElement the given distribution element
     */
    public static int getDistTypeInt(Element distElement)
    {
        int result = 0;

        if (distElement!=null)
        {
            String type = distElement.getAttributeValue(GenConstants.DIST_TYPE_ATTR).toUpperCase();
            Vector<String> distNameVector = new Vector<String>();

            for (int i = 0; i < GenConstants.TOTAL_DIST_NUM; i++)
            {
                distNameVector.add(GenConstants.DIST_NAME[i]);
            }

            result = distNameVector.indexOf(type);
        }

        return result;
    }

    
    /**
     * Create the ParamDistribution by the given distribution element and the index
     *
     * @param distElement the given distribution element
     * @param index the sequence number of the ParamDistribution in its distribution
     */
    public static ParamDistribution createParamDist(Element distElement, int index)
    {
        ParamDistribution result = new ParamDistribution();

        if (distElement!=null)
        {
            Element paramDistElement = (Element)(distElement.getChildren().get(index));
            String type = paramDistElement.getAttributeValue(GenConstants.DIST_TYPE_ATTR).toUpperCase();
            Vector<String>  distNameVector = new Vector<String>();

            for (int i = 0; i<GenConstants.TOTAL_DIST_NUM; i++)
            {
                distNameVector.add(GenConstants.DIST_NAME[i]);
            }

            int typeInt = distNameVector.indexOf(type);
            result.setParamDistType(typeInt);
            
            int paramNum = GenConstants.DIST_PARAM_NUM[typeInt];

            for (int i = 0; i<paramNum; i++)
            {
                Element paramI =(Element)(paramDistElement.getChildren().get(i));
                result.m_params[i] =new Double(paramI.getAttributeValue(GenConstants.PARAM_VALUE_ATTR));
            }
        }

        return result;
    }
    
    /**
     *
     * Create InputGenInfo node, and add it to corresponding input stream node.
     *
     * @param doc The distribution XML document
     * @param inputName the name of the input stream
     * @param inputStreamNum the number of the input stream that user configured in the GUI. 
     * @param inputTupleNum the number of the tuple that user configured in the GUI.
     */
    public static void setInputGenInfoElement(Document doc, String inputName, String inputStreamNum, String inputTupleNum)
    {
        Element root = doc.getRootElement();

        Element inputElement = findFirstElementChild(root,
                                        XmlConstants.INPUT_ELEMENT,
                                        XmlConstants.INPUT_STREAM_ATTRIBUTE,
                                                     inputName);

        ElementFilter streamFilter = new ElementFilter(
                                GenConstants.GEN_INFO_ELEM_NAME);

        inputElement.removeContent(streamFilter);
        
        Element genInfoElement = new Element(GenConstants.GEN_INFO_ELEM_NAME);
        
        Attribute attrStreamNum = new Attribute(
                     GenConstants.GENINFO_STREAM_NUM_ATTR, inputStreamNum);

        genInfoElement.setAttribute(attrStreamNum);
        
        Attribute attrTupleNum = new Attribute(
                       GenConstants.GENINFO_TUPLE_NUM_ATTR,inputTupleNum);

        genInfoElement.setAttribute(attrTupleNum);
        
        Double random = new Double(Math.random()*100000000);
        Long randomStreamID = random.longValue();
        
        Attribute attrStreamID = new Attribute(
                      GenConstants.GENINFO_STREAM_ID_ATTR, randomStreamID.toString());

        genInfoElement.setAttribute(attrStreamID);
        
        inputElement.addContent(genInfoElement);
    }

    
    /**
     * When user load a new query network XML document that has no distribution information, 
     * this method will be called to create default distribution node.
     *
     * @param originalDoc the new query network XML document that has no distribuion node.
     */
    public static Document createDefaultGenXmlDoc(Document originalDoc)
    {
        Element root = originalDoc.getRootElement();
        
        ElementFilter schemaFilter = new ElementFilter(GenConstants.SCHEMA_NAME);
        ElementFilter fieldFilter = new ElementFilter(GenConstants.FIELD_NAME);
        
        for (Iterator i = root.getContent(schemaFilter).iterator(); i.hasNext();)
        {
            Element schemaElement = (Element)i.next();

            for (Iterator j = schemaElement.getContent(fieldFilter).iterator(); j.hasNext();)
            {
                Element fieldElement = (Element) j.next();
                ElementFilter distFilter = new ElementFilter(GenConstants.DIST_ELEM_NAME);
                
                Iterator k = fieldElement.getContent(distFilter).iterator();

                if (!k.hasNext())
                {
                    Element distElement = new Element(GenConstants.DIST_ELEM_NAME);
                    String attrValue = GenConstants.DIST_NAME[0];

                    Attribute attr = new Attribute(GenConstants.DIST_TYPE_ATTR,
                                                   attrValue);

                    distElement.setAttribute(attr);
                    
                    ParamDistribution paramDist = new ParamDistribution();
                    Element paramDistElement = createParamDistElement(GenConstants.DIST_PARAM_MATRIX[0][0],
                            paramDist);
                    distElement.addContent(paramDistElement);
                    
                    fieldElement.addContent(distElement);
                }
            }
        }
        
        ElementFilter inputFilter = new ElementFilter(GenConstants.INPUT_NAME);
        
        for (Iterator i = root.getContent(inputFilter).iterator(); i.hasNext();)
        {
            Element inputElement = (Element) i.next();
            
            //remove old child Element
            ElementFilter distFilter = new ElementFilter(GenConstants.DIST_ELEM_NAME);
            
            Iterator k = inputElement.getContent(distFilter).iterator();

            if (!k.hasNext())
            {
                //add genInfoElement
                Element genInfoElement = new Element(GenConstants.GEN_INFO_ELEM_NAME);
                Attribute attrStreamNum = new Attribute(GenConstants.GENINFO_STREAM_NUM_ATTR,"1");
                genInfoElement.setAttribute(attrStreamNum);
                Attribute attrTupleNum = new Attribute(GenConstants.GENINFO_TUPLE_NUM_ATTR,"1000");
                genInfoElement.setAttribute(attrTupleNum);
                
                Double random = new Double(Math.random()*100000000);
                Long randomStreamID = random.longValue();
                
                Attribute attrStreamID = new Attribute(GenConstants.GENINFO_STREAM_ID_ATTR,randomStreamID.toString());
                genInfoElement.setAttribute(attrStreamID);
                
                inputElement.addContent(genInfoElement);
                
                //add distribution Element
                Element distElement = new Element(GenConstants.DIST_ELEM_NAME);
                String attrValue = GenConstants.DIST_NAME[0];
                Attribute attr = new Attribute(GenConstants.DIST_TYPE_ATTR, attrValue);
                distElement.setAttribute(attr);
                
                ParamDistribution paramDist = new ParamDistribution();

                Element paramDistElement = createParamDistElement(GenConstants.DIST_PARAM_MATRIX[0][0],
                        paramDist);

                distElement.addContent(paramDistElement);
                inputElement.addContent(distElement);
            }
        }
        
        return originalDoc;
    }
    
    /**
     * Return the stream number by given the XML document and stream name
     *
     * @param doc the distribution XML document
     * @param streamName the stream name
     */
    public static String getStreamNum(Document doc, String streamName)
    {
        String result = GenConstants.DEFAULT_STREAM_NUM;
        Element root = doc.getRootElement();

        Element inputElement = findFirstElementChild(root,
                                                     XmlConstants.INPUT_ELEMENT,
                                                     XmlConstants.INPUT_STREAM_ATTRIBUTE,
                                                     streamName);

        if (inputElement!=null)
        {
            Element genInfoElement = inputElement.getChild(GenConstants.GEN_INFO_ELEM_NAME);

            if (genInfoElement!=null)
            {
                result = genInfoElement.getAttributeValue(GenConstants.GENINFO_STREAM_NUM_ATTR);
            }
        }

        return result;
    }

    
    /**
     * Return the tuple number by given the XML document and stream name
     *
     * @param doc the distribution XML document
     * @param streamName the stream name
     */
    public static String getTupleNum(Document doc, String streamName)
    {
        String result = GenConstants.DEFAULT_TUPLE_NUM;
        Element root = doc.getRootElement();

        Element inputElement = findFirstElementChild(root,
                                                     XmlConstants.INPUT_ELEMENT,
                                                     XmlConstants.INPUT_STREAM_ATTRIBUTE,
                                                     streamName);

        if (inputElement!=null)
        {
            Element genInfoElement = inputElement.getChild(GenConstants.GEN_INFO_ELEM_NAME);

            if (genInfoElement!=null)
            {
                result = genInfoElement.getAttributeValue(GenConstants.GENINFO_TUPLE_NUM_ATTR);
            }
        }

        return result;
    }
}


/////////////////////////  end GenXmlHandler.java  /////////////////////////////
