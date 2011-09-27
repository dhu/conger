package borgui.xml;

import org.jdom.Element;
import java.util.List;
import borgui.common.*;

/**
 * A class that represent a Network Borealis Element
 */

public class NetworkElement extends BorealisElement
{
    public static String NAME = XmlConstants.NETWORK_ELEMENT;


    public NetworkElement()
    {
        super(NAME);
    }


    public NetworkElement(Element element)
    {
        super(NAME, element);
    }


    public List getAllSchemas()
    {
        return this.getChildren(XmlConstants.SCHEMA_ELEMENT);
    }


    public SchemaElement getSchema(String name)
    {
        return (SchemaElement) XmlHandler.findElement(this,
                                         XmlConstants.SCHEMA_ELEMENT,
                                         XmlConstants.NAME_ATTRIBUTE, name);
    }


    public boolean addSchema(SchemaElement schema)
    {
        this.addContent(schema);
        return true;
    }


    public boolean removeSchema(String name)
    {
        Element element = this.getSchema(name);

        if (element == null)
        {   return false;
        }

        return this.removeContent(element);
    }


    public List getAllInputs()
    {
        return this.getChildren(XmlConstants.INPUT_ELEMENT);
    }


    public InputElement getInput(String name)
    {
        return (InputElement) XmlHandler.findElement(this,
                                     XmlConstants.INPUT_ELEMENT,
                                     XmlConstants.INPUT_STREAM_ATTRIBUTE, name);
    }


    public boolean addInput(InputElement input)
    {
        this.addContent(input);
        return true;
    }


    public boolean removeInput(String name)
    {
        Element element = this.getInput(name);

        if (element == null)
        {
            return false;
        }

        return this.removeContent(element);
    }


    public List getAllOutputs()
    {
        return this.getChildren(XmlConstants.OUTPUT_ELEMENT);
    }


    public OutputElement getOutput(String name)
    {
        return (OutputElement) XmlHandler.findElement(this,
                                         XmlConstants.OUTPUT_ELEMENT,
                                         XmlConstants.OUTPUT_STREAM_ATTRIBUTE,
                                                      name);
    }


    public boolean  addOutput(OutputElement output)
    {
        this.addContent(output);
        return true;
    }


    public boolean  removeOutput(String name)
    {
        Element element = this.getOutput(name);

        if (element == null)
        {   return false;
        }

        return this.removeContent(element);
    }


    public boolean  removeOutElement(BoxElement boxElement, String name)
    {
        return boxElement.removeOut(name);
    }


    public List  getAllBoxes()
    {
        return  this.getChildren(XmlConstants.BOX_ELEMENT);
    }


    public List  getAllQueries()
    {
        return this.getChildren(XmlConstants.QUERY_ELEMENT);
    }


    public QueryElement  getQuery(String name)
    {
        return (QueryElement) XmlHandler.findElement(this,
                                        XmlConstants.QUERY_ELEMENT,
                                        XmlConstants.NAME_ATTRIBUTE, name);
    }


    public boolean addQuery(QueryElement query)
    {
        this.addContent(query);
        return true;
    }


    public boolean removeQuery(String name)
    {
        Element element = this.getQuery(name);

        if (element == null)
        {   return false;
        }

        return this.removeContent(element);
    }
}


///////////////////////  end  NetworkElement.java  /////////////////////////////
