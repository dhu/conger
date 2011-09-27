package borgui.xml;

import org.jdom.Element;
import java.util.List;
import borgui.common.*;


public class TablerefElement  extends BorealisElement
{
      public static final String NAME = XmlConstants.ACCESS_ELEMENT;


      public TablerefElement(String name)
      {
          super(NAME);
          this.setTableAttribute(name);
      }


      public TablerefElement(Element element)
      {
          super(NAME, element);
      }


      public String getTableAttribute()
      {
          return this.getAttributeValue(XmlConstants.ACCESS_TABLE_ATTRIBUTE);
      }


      public void setTableAttribute(String value)
      {
          this.setAttribute(XmlConstants.ACCESS_TABLE_ATTRIBUTE, value);
      }
}


//////////////////////////  end  TablerefElement.java  /////////////////////////
