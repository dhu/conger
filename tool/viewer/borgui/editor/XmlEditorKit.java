package borgui.editor;

import javax.swing.text.*;

/**
 * This is the set of things needed by a text component to be a reasonably 
 * functioning editor for some type  of text document. This implementation 
 * provides a default implementation which treats text as styled text and 
 * provides a minimal set of actions for editing styled text.
 */
public class XmlEditorKit extends StyledEditorKit {
 
    /**
     * Constructor.
     */
    public XmlEditorKit() {
        super();
    }//XMLEditorKit()
    
    /**
     * Overwrites super.
     */
    public String getContentType() {
        return "text/xml";
    }//getContentType()
    
    /**
     * Overwrites super.
     */
    public Document createDefaultDocument() {
        return new XmlDocument();
    }//createDefaultDocument()
    
}//XMLEditorKit
