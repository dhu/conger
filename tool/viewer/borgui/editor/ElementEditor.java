package borgui.editor;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import org.jdom.Element;
import org.jdom.Attribute;
import borgui.xml.*;
import borgui.common.*;


/**
 * A class that represents the element editor.
 */
public class ElementEditor extends JPanel
{
    protected Element m_elem;
    protected Vector m_addElems;
    
    protected JPanel m_childPanel;
    protected JPanel m_optionPanel;
    
    /**
     * Default contructor.
     */
    public ElementEditor()
    {
        super();
    }
    
    /**
     * Used by constructor to set up the layout.
     */
    private void setup()
    {
        this.removeAll();
        this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        
        ElementEditorRow topRow = new ElementEditorRow(m_elem);
        // add a row for this element
        this.add(topRow);
        
        m_childPanel = new JPanel();
        m_childPanel.setLayout(new BoxLayout(m_childPanel, BoxLayout.Y_AXIS));
        
        java.util.List elemList = m_elem.getChildren();
        // add a row for each child
        for(Iterator i = elemList.iterator(); i.hasNext(); ) {
            this.addChildElement((Element) i.next());
        }
        
        this.add(m_childPanel);
        
        // options
        m_optionPanel = new JPanel();
        m_optionPanel.setLayout(new BoxLayout(m_optionPanel, BoxLayout.X_AXIS));
        
        JButton addButton = new JButton("Add");
        m_optionPanel.add(addButton);
        
        Vector<String>  optionStrs = new Vector<String>();

        for(Iterator i = m_addElems.iterator(); i.hasNext(); )
        {
            optionStrs.add( ((Element) i.next()).getName() );
        }
        
        JComboBox addCombo = new JComboBox(optionStrs);
        m_optionPanel.add(addCombo);
        
        JButton removeButton = new JButton("Remove");
        m_optionPanel.add(removeButton);
        
        addButton.addActionListener(new AddButtonListener(addCombo));
        
        this.add(m_optionPanel);
    }
    
    /**
     * Return the Borealis Element in the current tree.
     */
    public Element getElement()
    {
        return m_elem;
    }
    
    /**
     * Add a child element
     */
    public void addChildElement(Element child)
    {
        ElementEditorRow childRow = new ElementEditorRow(child);
        m_childPanel.add(childRow);
        repaint();
    }
    
    /**
     * A class that response to add action.
     */
    private class AddButtonListener implements ActionListener
    {
        
        JComboBox m_combo;
        
        /**
         * Constructor.
         */
        public AddButtonListener(JComboBox combo)
        {
            m_combo = combo;
        }
        
        /**
         * Invoked when an action occurs.
         */
        public void actionPerformed(ActionEvent e)
        {
            Element elem = (Element) m_addElems.get(m_combo.getSelectedIndex());
            m_elem.addContent(new BorealisElement(elem.getName(), elem));
            addChildElement(elem);
        }
        
    }
    
    /**
     * A class that represents a option row.
     */
    private class ElementOptionRow extends JPanel implements ActionListener
    {
        
        /**
         * Constructor.
         */
        public ElementOptionRow()
        {
            this.setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
            
            JButton addButton = new JButton("Add");
            this.add(addButton);
            
            JComboBox addCombo = new JComboBox();
            this.add(addCombo);
            
            JButton removeButton = new JButton("Remove");
            this.add(removeButton);
        }
        
        /**
         * Invoked when an action occurs.
         */
        public void actionPerformed(ActionEvent e)
        {
                /*
                JComboBox cb = (JComboBox)e.getSource();
                String petName = (String)cb.getSelectedItem();
                updateLabel(petName);*/
        }
    }
    
    
    /**
     * A class that represents a editor row.
     */
    private class ElementEditorRow extends JPanel
    {
        protected Element m_rowElem;
        
        /**
         * Constructor.
         */
        public ElementEditorRow(Element elem)
        {
            m_rowElem = elem;
            
            this.setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
            
            // filler
            JPanel fillerPanel = new JPanel();
            fillerPanel.setPreferredSize(new Dimension(10,10));
            this.add(fillerPanel);
            
            // add a label for this element
            this.add(new JLabel(elem.getName() + " : "));
            
            java.util.List attrList = m_rowElem.getAttributes();
            // add a cell for each attribute
            for(Iterator i = attrList.iterator(); i.hasNext();)
            {
                this.add(new AttributeEditor((Attribute) i.next()));
            }
        }
    }
    
    /**
     * A class that represents a attribute editor
     */
    private class AttributeEditor extends JPanel implements PropertyChangeListener
    {
        protected Attribute m_attr;
        protected JFormattedTextField m_valueField;
        
        /**
         * Constructor.
         */
        public AttributeEditor(Attribute attr)
        {
            m_attr = attr;
            
            this.setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
            
            // add label for this attribute
            this.add(new JLabel(attr.getName() + " = "));
            
            m_valueField = new JFormattedTextField(attr.getValue());
            m_valueField.addPropertyChangeListener("value", this);
            this.add(m_valueField);
        }
        
        /**
         * Called when a field's "value" property changes.
         */
        public void propertyChange(PropertyChangeEvent e)
        {
            m_attr.setValue(m_valueField.getValue().toString());
                /*
                Object source = e.getSource();
                if (source == amountField)
                {
                        amount = ((Number)amountField.getValue()).doubleValue();
                }
                else if (source == rateField)
                {
                        rate = ((Number)rateField.getValue()).doubleValue();
                } else if (source == numPeriodsField) {
                        numPeriods = ((Number)numPeriodsField.getValue()).intValue();
                }
                 
                double payment = computePayment(amount, rate, numPeriods);
                paymentField.setValue(new Double(payment));*/
        }
    }
}


//////////////////////////  end  ElementEditor.java  ///////////////////////////
