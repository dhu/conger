package borgui.xml;

import org.jdom.*;
import borgui.common.*;
import java.util.*;


/**
 * A class that wrap Borealis XML Element.
 */
public class BorealisElement  extends Element
{
    /**
     * Create a new BorealisElement with the supplied (local) name and no namespace.
     */
    public BorealisElement(java.lang.String name)
    {
        super(name);
    }
   
 
    /**
     * Create a new BorealisElement with the same content and attributes with the given element.
     */
    public BorealisElement(java.lang.String name, Element element)
    {
        super(name);

        // copy content
        this.addContent(element.cloneContent());

        // copy attributes
        List attrList =  element.getAttributes();

        for(Iterator i = attrList.iterator(); i.hasNext(); )
        {
            Attribute attr = (Attribute) i.next();
            this.setAttribute((Attribute)attr.clone());
        }
    }
    

    /**
     * Creates a new BorealisElement with the supplied (local) name and namespace.
     */
    public BorealisElement(java.lang.String name, Namespace namespace)
    {
        super(name, namespace);
    }
   
 
    /**
     * Creates a new BorealisElement with the supplied (local) name and a namespace given by a URI.
     */
    public BorealisElement(java.lang.String name, java.lang.String uri)
    {
        super(name, uri);
    }
   
 
    /**
     * Creates a new BorealisElement with the supplied (local) name and a namespace given by the supplied prefix and URI combination.
     */
    public BorealisElement(java.lang.String name,
                           java.lang.String prefix,
                           java.lang.String uri)
    {
        super(name, prefix, uri);
    }
    

    /**
     * Return the value of the attribute as a int.
     */
    public Integer getAttributeValueInt(String name)
    {
        Integer value = null;
        String string = this.getAttributeValue(name);
        
        try
        {   value = Integer.valueOf(string);
        }
        catch(java.lang.NumberFormatException e)
        {
            return null;
        }
        
        return value;
    }
    

    /**
     * Set the value of the attribute as a int.
     */
    public void setAttributeInt(String name, int value)
    {
        this.setAttribute(name, Integer.toString(value));
    }
}


/////////////////////////////  end  BorealisElement.java  //////////////////////
