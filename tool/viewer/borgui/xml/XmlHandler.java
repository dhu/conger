package borgui.xml;

import org.jdom.*;
import org.jdom.filter.*;
import org.jdom.output.XMLOutputter;
import org.jdom.output.Format;
import org.jdom.input.SAXBuilder;
import org.xml.sax.InputSource;

import java.io.File;
import java.io.FileReader;
import java.io.*;

import java.util.*;

import borgui.graph.*;
import borgui.common.DebugTrace;


/**
 * Class with convenience methods for dealing with jdom documents as
 * well as encoding and decoding graphical state into and out of a document
 *
 */
public class XmlHandler implements XmlConstants
{
    public final static Namespace GUI_NAMESPACE = Namespace.getNamespace(GUI_NAMESPACEALIAS, GUI_NAMESPACEURI);


    /**
     * Creates a new instance of XmlHandler.
     *
     */
    public XmlHandler() {}


    /**
     * Takes an well-formed xml input source and builds a jdom Document. If a
     * Document cannot be built, prints an error and returns null
     *
     *@param filename the name of the file which is to be build into jdom Document
     *@return jdom Document representing the xml data.
     */
    public static Document  xmlToDoc(File  file)
                    throws  Exception
    {
        // command line should offer URIs or file names
        DebugTrace.trace(file.getAbsolutePath());

        try
        {   FileReader reader = new FileReader(file.getAbsolutePath());
            File    directory = file.getParentFile();  // Directory with the file.
            String       user = System.getProperty("user.dir");

            // Parse relative to the directory containing the XML file.
            //
            System.setProperty("user.dir", directory.getAbsolutePath());

            SAXBuilder builder = new SAXBuilder(false);
            Document  document = builder.build(new InputSource(reader));

            // Return to the original directory.
            //
            System.setProperty("user.dir", user);
            return  document;
        }
        catch (Exception e)
        {   DebugTrace.trace(e.getMessage());
            throw e;
        }
    }


    /**
     * Takes an well-formed xml input source and builds a jdom Document. If a
     * Document cannot be built, prints an error and returns null
     *
     *@param filename the name of the file which is to be build into jdom Document
     *@return jdom Document representing the xml data.
     */
    public static Document  xmlToDoc(String  xml)
                    throws  Exception
    {
        try
        {   StringReader reader   = new StringReader(xml);
            SAXBuilder   builder  = new SAXBuilder(false);
            Document     document = builder.build(new InputSource(reader));

            return  document;
        }
        catch (Exception e)
        {   DebugTrace.trace(e.getMessage());
            throw e;
        }
    }



    /**
     * Takes a jdom Document and writes it out prettly to given OutputStream.
     * If the document cannot be outputted, prints and error and returns false
     *
     * @param doc jdom Document to output
     * @param output stream to output the doc to
     * @return true if document was successfully outputted and false otherwise
     */
    public static boolean docToXml(Document doc, OutputStream output)
    {
        try
        {
            XMLOutputter outputter = new XMLOutputter(Format.getPrettyFormat());
            outputter.output(doc, output);
        }
        catch (java.io.IOException e)
        {
            e.printStackTrace();
            return false;
        }

        return true;
    }


    /**
     * Takes a jdom Element and writes it out prettly to given OutputStream.
     * If the element cannot be outputted, prints and error and returns false
     *
     * @param elem jdom Element to output
     * @param output stream to output the element to
     * @return true if element was successfully outputted and false otherwise
     */
    public static boolean elementToXml(Element element, OutputStream output)
    {
        try
        {
            XMLOutputter outputter = new XMLOutputter(Format.getPrettyFormat());
            outputter.output(element, output);
        }
        catch (java.io.IOException e)
        {
            e.printStackTrace();
            return false;
        }

        return true;
    }


    /**
     * Takes a jdom Element and writes it out prettly to given OutputStream.
     * If the element cannot be outputted, prints and error and returns false
     *
     * @param element jdom Element to output
     * @param output stream to output the element to
     * @return true if element was successfully outputted and false otherwise
     */
    public static String elementToXml(Element element)
    {
        ByteArrayOutputStream stream = new ByteArrayOutputStream();

        if (XmlHandler.elementToXml(element, stream))
        {
            return stream.toString();
        }

        return null;
    }


    /*******************************
    public static void mergeElements(Element element1, Element element2) {}
    *****************************/


    /**
     * Takes a Document and searches for an element named elementName,
     * which an attribute named elementAttrName that has the value
     * elementAttrValue
     *
     * @param elementTop Element to search
     * @param elementName name of elements to consider
     * @param elementAttrName name of attribute to match
     * @param elementAttrValue value of named attribute to match
     * @return Eleement if a match is found and null otherwise
     */

    public static Element findElement(Element  elementTop,
                                      String   elementName,
                                      String   elementAttrName,
                                      String   elementAttrValue)
    {
        Element element = null;

        // get all elements of this name
        ElementFilter filter = new ElementFilter(elementName);
        List elementList = elementTop.getContent(filter);

        // look through all these elements
        for(Iterator i = elementList.iterator(); i.hasNext() && element == null; )
        {
            Element e = (Element) i.next();
            // check to see if it has the correct attribute value
            String value = e.getAttributeValue(elementAttrName);

            if (value != null)
            {
                if (elementAttrValue.equals(value))
                {
                    element = e;
                }
            }
        }

        return element;

    }


    /**
     * Change a jdom Xml Document to Borealis Document
     */
    public static void jdomDocToBorealisDoc(Document          doc,
                                            BorealisDocument  borDoc)
    {

        // need a root
        if (doc.getRootElement() == null)
        {
            return;
        }

        // recursively walk and convert all elements
        borDoc.setRootElement(XmlHandler.jdomDocToBorealisDoc(doc.getRootElement()));
    }


    /**
     * Change a jdom Xml Document to Borealis Document
     */
    public static Element jdomDocToBorealisDoc(Element element)
    {
        // get all immediate child elements
        List childElements = element.getChildren();

        // convert to borealis element
        Element borealisElement = XmlHandler.jdomElementToBorealisElement(element);

        // remove all child elements
        borealisElement.removeContent(new ElementFilter());

        // walk the list of child elements
        for(Iterator i= childElements.iterator(); i.hasNext();)
        {
            Element e = (Element)i.next();

            // recursively convert child and add as child
            borealisElement.addContent(XmlHandler.jdomDocToBorealisDoc(e));
        }

        return borealisElement;
    }


    /**
     * Change a jdom Xml element to Borealis Element
     */
    public static Element jdomElementToBorealisElement(Element element)
    {
        Element borealisElement = null;

        String name = element.getName();

        if (name.equals(BoxElement.NAME))
        {
            borealisElement = new BoxElement(element);
        }
        else if (name.equals(FieldElement.NAME))
        {
            borealisElement = new FieldElement(element);
        }
        else if (name.equals(InElement.NAME))
        {
            borealisElement = new InElement(element);
        }
        else if (name.equals(InputElement.NAME))
        {
            borealisElement = new InputElement(element);
        }
        else if (name.equals(NetworkElement.NAME))
        {
            borealisElement = new NetworkElement(element);
        }
        else if (name.equals(OutElement.NAME))
        {
            borealisElement = new OutElement(element);
        }
        else if (name.equals(OutputElement.NAME))
        {
            borealisElement = new OutputElement(element);
        }
        else if (name.equals(ParameterElement.NAME))
        {
            borealisElement = new ParameterElement(element);
        }
        else if (name.equals(QueryElement.NAME))
        {
            borealisElement = new QueryElement(element);
        }
        else if (name.equals(SchemaElement.NAME))
        {
            borealisElement = new SchemaElement(element);
        }
        else if (name.equals(TablerefElement.NAME) &&
                ((Element)element.getParent()).getName().equals(BoxElement.NAME))
        {
            borealisElement = new TablerefElement(element);
        }
        else if (name.equals(TableElement.NAME))
        {
            borealisElement = new TableElement(element);
        }
        else
        {
            borealisElement = (Element) element.clone();
        }

        return borealisElement;
    }


    public static void jdomDocToDeployDoc(Document        doc,
                                          DeployDocument  deployDoc)
    {
        // need a root
        if (doc.getRootElement() == null)
        {
            return;
        }

        // recursively walk and convert all elements
        deployDoc.setRootElement(XmlHandler.jdomDocToDeployDoc(doc.getRootElement()));
    }


    public static Element jdomDocToDeployDoc(Element element)
    {
        // get all immediate child elements
        List childElements = element.getChildren();

        // convert to borealis element
        Element deployElement = XmlHandler.jdomElementToDeployElement(element);
        // remove all child elements
        deployElement.removeContent(new ElementFilter());

        // walk the list of child elements
        for(Iterator i= childElements.iterator(); i.hasNext(); )
        {
            element = (Element) i.next();

            // recursively convert child and add as child
            deployElement.addContent(XmlHandler.jdomDocToDeployDoc(element));
        }

        return deployElement;
    }


    public static Element jdomElementToDeployElement(Element element)
    {
        Element deployElement = null;

        String name = element.getName();

        if (name.equals(DeployElement.NAME))
        {
            deployElement = new DeployElement(element);
        }
        else if (name.equals(ReplicSetElement.NAME))
        {
            deployElement = new ReplicSetElement(element);
        }
        else if (name.equals(NodeElement.NAME))
        {
            deployElement = new NodeElement(element);
        }
        else if (name.equals(RegionElement.NAME))
        {
            deployElement = new RegionElement(element);
        }
        else if (name.equals(PublishElement.NAME))
        {
            deployElement = new PublishElement(element);
        }
        else if (name.equals(SubscribeElement.NAME))
        {
            deployElement = new SubscribeElement(element);
        }
        else if (name.equals(UnSubscribeElement.NAME))
        {
            deployElement = new UnSubscribeElement(element);
        }
        else if (name.equals(MoveElement.NAME))
        {
            deployElement = new MoveElement(element);
        }
        else
        {
            deployElement = (Element) element.clone();
        }

        return deployElement;
    }
}


///////////////////////////////  end  XmlHandler.java  /////////////////////////
