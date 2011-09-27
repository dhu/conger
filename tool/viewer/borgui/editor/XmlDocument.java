package borgui.editor;

import javax.swing.text.*;
import java.awt.*;

public class XmlDocument extends DefaultStyledDocument {
 
    private MutableAttributeSet m_openBraceAttrs = null;
    private MutableAttributeSet m_closeBraceAttrs = null;
    private MutableAttributeSet m_equalsAttrs = null;
    private MutableAttributeSet m_elemNameAttrs = null;
    private MutableAttributeSet m_attrNameAttrs = null;
    private MutableAttributeSet m_attrValueAttrs = null;
    private MutableAttributeSet m_contentAttrs = null;
    private boolean m_highlight;
 
    /**
     * Constructor.
     */
    public XmlDocument() {

        m_openBraceAttrs = new SimpleAttributeSet();
        StyleConstants.setForeground(m_openBraceAttrs, Color.magenta);
        //StyleConstants.setBackground(m_openBraceAttrs, Color.black);

        m_closeBraceAttrs = new SimpleAttributeSet();
        StyleConstants.setForeground(m_closeBraceAttrs, Color.magenta);
        //StyleConstants.setBackground(m_closeBraceAttrs, Color.black);

        m_equalsAttrs = new SimpleAttributeSet();
        StyleConstants.setForeground(m_equalsAttrs, Color.black);
        //StyleConstants.setBackground(m_equalsAttrs, Color.black);

        m_elemNameAttrs = new SimpleAttributeSet();
        StyleConstants.setForeground(m_elemNameAttrs, Color.red);
        //StyleConstants.setBackground(m_elemNameAttrs, Color.black);

        m_attrNameAttrs = new SimpleAttributeSet();
        StyleConstants.setForeground(m_attrNameAttrs, Color.blue);
        //StyleConstants.setBackground(m_attrNameAttrs, Color.black);

        m_attrValueAttrs = new SimpleAttributeSet();
        StyleConstants.setForeground(m_attrValueAttrs, Color.cyan.darker());
        //StyleConstants.setBackground(m_attrValueAttrs, Color.black);

        m_contentAttrs = new SimpleAttributeSet();
        StyleConstants.setForeground(m_contentAttrs, Color.black);
        //StyleConstants.setBackground(m_contentAttrs, Color.black);

	m_highlight = true;


    }//XMLDocument()
 
    /**
     * Override AbstractDocument.
     */
    public void insertString(int offset, String str, AttributeSet a) throws BadLocationException {
        super.insertString(offset, str, a);
	if (m_highlight) {
        this.colorize(offset); }
    }//insertString()
 
    /**
     * Override AbstractDocument.
     */
    public void remove(int offset, int length) throws BadLocationException {
        super.remove(offset, length);
	if (m_highlight) {
        this.colorize(0);}
    }//remove()

    public void setElementColor(String elemName, Color elemColor) {
	
    }

    public void highlight() {
	this.setHighlight(true);
	try {
		this.colorize(0);
	} catch (BadLocationException e) {
	}
	this.setHighlight(false);	
    }

    public void setHighlight(boolean on) {
	m_highlight = on;
    }

    private void colorizeElement(String text, int offset) {
	   int pos = 0;

	   int braceLen = 1;

	   // handle end and begin tags
	   if (text.startsWith("</")) {
		braceLen = 2;
	   }

	   // color open brace
           setCharacterAttributes(offset + pos, braceLen, m_openBraceAttrs, false);
	   pos = pos + braceLen;

	   int endPos = text.length() - 1;

	   // handle
	   if (text.endsWith("/>")) {
		endPos = text.length() - 2;
	   }

	   int elemNameEnd = text.indexOf(' ', 0); // end of name
	   if (elemNameEnd == -1) {
		elemNameEnd = endPos; 
	   }
	   elemNameEnd--;

	   // color element name
	   setCharacterAttributes(offset + pos, elemNameEnd - pos + 1, m_elemNameAttrs, false);

	   pos = elemNameEnd + 1;

	   // color all attribute names, "=", and values
	   while(pos < endPos) {

		int equalsPos = text.indexOf('=', pos); // position of next equals
		if (equalsPos == -1) {
			break;
		}

		// everything up until equals is attribute name color
		setCharacterAttributes(offset + pos, equalsPos - pos, m_attrNameAttrs, false);
	
		// equals is equals color
		setCharacterAttributes(offset + equalsPos, 1, m_equalsAttrs, false);

		// move position
		pos = equalsPos + 1;

		int attrValueStart = text.indexOf('"', pos); // position of begin "
	
		if (attrValueStart == -1) {
			break;
		}

		int attrValueEnd = text.indexOf('"', attrValueStart + 1); // position of end "

		if (attrValueEnd == -1){
			break;
		}

		// color attribute value
		setCharacterAttributes(offset + attrValueStart, attrValueEnd - attrValueStart + 1, m_attrValueAttrs, false);

	        // move position
		pos = attrValueEnd + 1;

	   } 
          
           // color everything else 
	   setCharacterAttributes(offset + pos, endPos - pos, m_contentAttrs, false);

	   // color end brace
	   setCharacterAttributes(offset + endPos, text.length() - endPos, m_closeBraceAttrs, false);
    }

 
    /**
     * Called when document content changed to revalidate syntax highlightning.
     * Only works correctly for well-formed XML.
     */
    private void colorize(int offset) throws BadLocationException {

	if (!m_highlight) {
		return;
	}
   
	String text = this.getText(offset, getLength() - offset);

 	int strpos = 0;
	while(strpos < text.length()) {
		
		int openIndex = text.indexOf('<', strpos); // next '<'

		if (openIndex == -1) {
			break;
		}

		
		int closeIndex = text.indexOf('>', openIndex); // next '>'

		if (closeIndex == -1) {
			//break;
			closeIndex = text.length() - 1;
		}

		this.colorizeElement(text.substring(openIndex, closeIndex + 1), offset + openIndex);
		strpos = closeIndex + 1;
	}
	
     }

  
}

