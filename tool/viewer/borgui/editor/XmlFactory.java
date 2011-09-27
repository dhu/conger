package borgui.editor;

import java.awt.*;
import java.util.*;
import borgui.common.*;
import borgui.xml.*;

import org.jdom.*;

public class XmlFactory implements XmlConstants
{

    public static Element createQueryElement(Name name, String node)
    {
        Element element = new Element(QUERY_ELEMENT);
            element.setAttribute(NAME_ATTRIBUTE, name.toString());
        return element;
    }


    public static Element createBoxElement(Name name, String type)
    {
        Element element = new Element(BOX_ELEMENT);

        element.setAttribute(NAME_ATTRIBUTE, name.toString());
        element.setAttribute(BOX_TYPE_ATTRIBUTE, name.toString());

        return element;
    }

    
    public static Element createInputElement(Name     name,
                                             String   stream,
                                             String   schema,
                                             int      node)
    {
        Element element = new Element(INPUT_ELEMENT);

        element.setAttribute(NAME_ATTRIBUTE, name.toString());
        element.setAttribute(INPUT_STREAM_ATTRIBUTE, stream);
        element.setAttribute(INPUT_SCHEMA_ATTRIBUTE, schema);
        element.setAttribute(INPUT_NODE_ATTRIBUTE, Integer.toString(node));

        return element;
    }


    public static Element createOutputElement(Name    name,
                                              String  stream,
                                              String  schema)
    {
        Element element = new Element(OUTPUT_ELEMENT);

        element.setAttribute(NAME_ATTRIBUTE, name.toString());
        element.setAttribute(OUTPUT_STREAM_ATTRIBUTE, stream);
        element.setAttribute(OUTPUT_SCHEMA_ATTRIBUTE, schema);

        //element.setAttribute(OUTPUT_ENDPOINT_ATTRIBUTE, Integer.toString(endpoint));

        return element;
    }


    public static Element createSchemaElement(Name name)
    {
        Element element = new Element(SCHEMA_ELEMENT);

        element.setAttribute(NAME_ATTRIBUTE, name.toString());

        return element;
    }


    public static Element createFieldElement(Name name, String type)
    {
        Element element = new Element(FIELD_ELEMENT);

        element.setAttribute(NAME_ATTRIBUTE, name.toString());
        element.setAttribute(FIELD_TYPE_ATTRIBUTE, type);

        return element;
    }


    public static Element createParameterElement(Name name, String value)
    {
        Element element = new Element(PARAMETER_ELEMENT);

        element.setAttribute(NAME_ATTRIBUTE, name.toString());
        element.setAttribute(PARAMETER_VALUE_ATTRIBUTE, value);

        return element;
    }


    public static Element createInElement(String stream)
    {
        Element element = new Element(IN_ELEMENT);

        element.setAttribute(IN_STREAM_ATTRIBUTE, stream);

        return element;
    }


    public static Element createOutElement(String stream)
    {
        Element element = new Element(OUT_ELEMENT);

        element.setAttribute(OUT_STREAM_ATTRIBUTE, stream);

        return element;
    }
}


////////////////////////  end  XmlFactory.java  ////////////////////////////////
