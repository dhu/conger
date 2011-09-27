package borgui.xml;

import org.jdom.Element;
import java.util.List;
import borgui.common.*;

/**
 * A class that represent a Field Borealis Element
 */
public class FieldElement extends BorealisElement
{
      public static final String NAME = XmlConstants.FIELD_ELEMENT;


      public FieldElement(String name, String type)
      {
          super(NAME);

          this.setNameValue(name);

          setType(type);
      }


      public FieldElement(String name, String type, int size)
      {
          super(NAME);

          this.setNameValue(name);
          setType(type);
          setSize(size);
      }


      public FieldElement(Element element)
      {
          super(NAME, element);
      }


      public String getNameValue()
      {
          return this.getAttributeValue(XmlConstants.NAME_ATTRIBUTE);
      }


      public void setNameValue(String value)
      {
          this.setAttribute(XmlConstants.NAME_ATTRIBUTE, value);
      }


      public String getType()
      {
          return this.getAttributeValue(XmlConstants.FIELD_TYPE_ATTRIBUTE);
      }


      public void setType(String value)
      {
          this.setAttribute(XmlConstants.FIELD_TYPE_ATTRIBUTE, value);
      }


      public Integer getSize()
      {
          return this.getAttributeValueInt(XmlConstants.FIELD_SIZE_ATTRIBUTE);
      }


      public void setSize(int value)
      {
          this.setAttributeInt(XmlConstants.FIELD_SIZE_ATTRIBUTE, value);
      }
}


///////////////////////////  end  FieldElement.java  ///////////////////////////
