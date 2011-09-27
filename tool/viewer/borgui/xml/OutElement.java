package borgui.xml;


import org.jdom.Element;
import java.util.List;
import borgui.common.*;


public class OutElement  extends BorealisElement
                      implements StreamAttributeElement
{
      public static final String NAME = XmlConstants.OUT_ELEMENT;


      public OutElement(String stream)
      {
          super(NAME);
          this.setStream(stream);
      }


      public OutElement(Element element)
      {
          super(NAME, element);
      }


      public String getStream()
      {
          return this.getAttributeValue(XmlConstants.OUT_STREAM_ATTRIBUTE);
      }


      public void setStream(String value)
      {
          this.setAttribute(XmlConstants.OUT_STREAM_ATTRIBUTE, value);
      }
}


///////////////////////  end  OutElement.java  /////////////////////////////////
