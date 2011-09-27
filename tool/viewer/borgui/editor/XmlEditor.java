package borgui.editor;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.text.*;

/**
 * A class that represent the editor for xml file.
 */
public class XmlEditor extends JTextPane
{

      protected XmlDocument m_xmldoc;
      protected XmlEditorKit m_xmlkit;

      /**
       * Constructor.
       */
      public XmlEditor() {
	    super();

	    m_xmldoc = new XmlDocument();
//	    m_xmldoc.setHighlight(false);

	    m_xmlkit = new XmlEditorKit();

	    this.setStyledDocument(m_xmldoc);
	    this.setEditorKit(m_xmlkit);

	    this.setEditable(false);

      }
    
	/* 
     public void setText(String t) {
	   super.setText(t);
	   setCaretPosition(0);
	   m_xmldoc.setHighlight(true);
	   try {
	   	m_xmldoc.insertString(0, t, getCharacterAttributes()); 
	   } catch(BadLocationException e) {
	   }
	   m_xmldoc.setHighlight(false);
	   repaint();
     }*/

}
