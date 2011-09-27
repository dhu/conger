/**
 * ApplyToPanel.java
 *
 * Created on July 31, 2005, 2:34 PM
 *
 */

package borgui.generator;

import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;

import borgui.xml.*;
import java.util.Iterator;
import org.jdom.filter.ElementFilter;

/**
 * A class that implement a pannel that can apply the current distribution configuration to other input streams or fields.
 * 
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/07/2005
 */
public class ApplyToPanel extends GeneralApplyCancelPanel
{
    //left
    protected TitledBorder m_availableTitledBorder;
    protected DefaultListModel m_availableListModel = new DefaultListModel();
    protected JList m_availableList = new JList(m_availableListModel);
    //center
    protected JButton m_addButton = new JButton("  >  ");
    protected JButton m_deleteButton = new JButton("  <  ");
    //right
    protected TitledBorder m_choosedTitledBorder;
    protected DefaultListModel m_choosedListModel = new DefaultListModel();
    protected JList m_choosedList = new JList(m_choosedListModel);
    
    //other
    protected GenActionHandler m_genActionHandler;
    protected InputGenPanel m_inputGenPanel;
    
    /** 
     * Creates a new instance of ApplyToPanel 
     * @param inputGenPanel the constructor needs the instance of the InputGenPanel to get access to its fields and methonds.
     */
    public ApplyToPanel(InputGenPanel inputGenPanel)
    {
        super();

        m_inputGenPanel = inputGenPanel;
        this.setPreferredSize(new Dimension(700, 200));
        m_firstButton.setVisible(false);
        m_secondButton.setVisible(false);
        m_choosePanel.setPreferredSize(new Dimension(670, 170));
        m_middlePanel.setPreferredSize(new Dimension(680, 15));
        m_leftBottomPanel.setPreferredSize(new Dimension(340, 25));
        m_bottomPanel.add(m_leftBottomPanel,0);
        
        m_genActionHandler = new GenActionHandler(m_inputGenPanel);
        m_thirdButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.APPLY_TO_APPLY_ACTION));
        m_fourthButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.APPLY_TO_CANCEL_ACTION));
        setupChoosePanel();
    }

    
    /**
     * Initialize the layout of the m_choosePanel.
     * This method only be called in the constructor.
     */
    protected void setupChoosePanel()
    {
        //layout
        m_choosePanel.setLayout(new BoxLayout(m_choosePanel, BoxLayout.X_AXIS));
        JPanel leftPanel = new JPanel();
        JPanel centerPanel = new JPanel();
        JPanel rightPanel = new JPanel();
        leftPanel.setPreferredSize(new Dimension(260, 170));
        centerPanel.setPreferredSize(new Dimension(40, 170));
        rightPanel.setPreferredSize(new Dimension(260, 170));
        
        m_choosePanel.add(Box.createHorizontalStrut(15));
        m_choosePanel.add(leftPanel);
        m_choosePanel.add(Box.createHorizontalStrut(15));
        m_choosePanel.add(centerPanel);
        m_choosePanel.add(Box.createHorizontalStrut(10));
        m_choosePanel.add(rightPanel);
        m_choosePanel.add(Box.createHorizontalStrut(15));
        
        //leftPanel
        leftPanel.setLayout(new BoxLayout(leftPanel, BoxLayout.Y_AXIS));
        
        Border empty = BorderFactory.createEmptyBorder();
        m_availableTitledBorder = BorderFactory.createTitledBorder(empty, "Available:");
        m_availableTitledBorder.setTitlePosition(TitledBorder.TOP);
        
        JScrollPane availableListScroller = new JScrollPane(m_availableList);
        availableListScroller.setPreferredSize(new Dimension(260, 130));
        leftPanel.add(availableListScroller);
        leftPanel.setBorder(m_availableTitledBorder);
        m_availableList.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
        m_availableList.setSelectedIndex(0);
        m_availableList.setVisibleRowCount(9);
        
        //centerPanel
        centerPanel.setLayout(new BoxLayout(centerPanel, BoxLayout.Y_AXIS));
        JPanel centerCenterPanel = new JPanel();
        
        m_addButton.addActionListener(new addListener());
        m_deleteButton.addActionListener(new deleteListener());
        
        centerPanel.add(Box.createVerticalStrut(25));
        centerPanel.add(m_addButton);
        centerPanel.add(Box.createVerticalStrut(20));
        centerPanel.add(m_deleteButton);
        centerPanel.add(Box.createVerticalStrut(25));
        
        //rightPanel
        
        rightPanel.setLayout(new BoxLayout(rightPanel, BoxLayout.Y_AXIS));
        
        m_choosedTitledBorder = BorderFactory.createTitledBorder(empty, "Choosed:");
        m_choosedTitledBorder.setTitlePosition(TitledBorder.TOP);
        
        JScrollPane choosedListScroller = new JScrollPane(m_choosedList);
        choosedListScroller.setPreferredSize(new Dimension(260, 130));
        rightPanel.add(choosedListScroller);
        rightPanel.setBorder(m_choosedTitledBorder);
        m_choosedList.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
    }
   
 
    /**
     * set part of the componenents in this panel to be enable or unable.
     * @param tf set to true of false
     */
    protected void setComponentsEnabled(boolean tf)
    {
        m_availableList.setEnabled(tf);
        m_addButton.setEnabled(tf);
        m_deleteButton.setEnabled(tf);
        m_choosedList.setEnabled(tf);
        this.m_thirdButton.setEnabled(tf);
        this.m_fourthButton.setEnabled(tf);
    }

    
    /**
     * Clear the two list: m_availableList and m_choosedList. Delete all the current list items.
     */  
    protected void clear()
    {
        this.m_availableListModel.removeAllElements();
        this.m_choosedListModel.removeAllElements();
    }
   
 
    /**
     * When user click the "ApplyTo" button in the left distribution panel, this method will be called.
     * Initialize the "avalible list" with the current input streams or fields.
     * Initialize the "choosed list" with one item which is what the use current configuring.
     */
    public void initDataInList()
    {
        setComponentsEnabled(true);
        
        BorealisElement rootElementGenTree = (BorealisElement) m_inputGenPanel.m_treePanel.m_rootElementGenTree.clone();
        m_availableListModel.removeAllElements();
        m_choosedListModel.removeAllElements();

        if(m_inputGenPanel.getFistDistPanel().getCurrentType().equals(GenConstants.FIELD_NAME))
        {
            m_inputGenPanel.showApplyToView("FIELD");
            
            ElementFilter fieldFilter = new ElementFilter(XmlConstants.FIELD_ELEMENT);

            for(Iterator i = rootElementGenTree.getDescendants(fieldFilter);i.hasNext();)
            {
                BorealisElement fieldElement = (BorealisElement)i.next();
                String fieldName = fieldElement.getAttributeValue(XmlConstants.NAME_ATTRIBUTE);
                String parentSchemaName = fieldElement.getParentElement().getAttributeValue(XmlConstants.NAME_ATTRIBUTE);
                
                //put current stream item into choosed panel
                if(fieldName==m_inputGenPanel.getFistDistPanel().getCurrentTypeName())
                {
                    m_choosedListModel.addElement(fieldName + " _ ( Schema: " + parentSchemaName + " )");
                }
                else
                {   m_availableListModel.addElement(fieldName + " _ ( Schema: " + parentSchemaName + " )");
                }
            }
        }
        else
        {  m_inputGenPanel.showApplyToView("INPUT");
            
            ElementFilter inputFilter = new ElementFilter(XmlConstants.INPUT_ELEMENT);
            
            for(Iterator j = rootElementGenTree.getDescendants(inputFilter);j.hasNext();)
            {
                BorealisElement inputElement = (BorealisElement)j.next();
                String inputName = inputElement.getAttributeValue(XmlConstants.INPUT_STREAM_ATTRIBUTE);
                String parentSchemaName = inputElement.getParentElement().getAttributeValue(XmlConstants.NAME_ATTRIBUTE);
                
                //put current stream item into choosed panel
                if(inputName==m_inputGenPanel.getFistDistPanel().getCurrentTypeName())
                {
                    m_choosedListModel.addElement(inputName + " _ ( Schema: " + parentSchemaName + " )");
                }
                else
                {   m_availableListModel.addElement(inputName + " _ ( Schema: " + parentSchemaName + " )");
                }
            }
        }

        m_inputGenPanel.getFistDistPanel().setChooseDistEnabled(false);
        m_inputGenPanel.m_treePanel.m_elementTree.setEnabled(false);
        m_inputGenPanel.getFistDistPanel().setRateEnabled(false);
    }

    
    /**
     * When user click the "Apply" button in the ApplyToPanel, this method will be called.
     * Create distribution node and insert it to the XML document.
     */
    public void insertDist()
    {
        String typeString = m_inputGenPanel.getFistDistPanel().getCurrentType();
        DefaultListModel choosedListModel = m_inputGenPanel.m_applyToPanel.m_choosedListModel;

        if(typeString.equals(GenConstants.FIELD_NAME))
        {
            for(int i = 0; i<choosedListModel.size(); i++)
            {
                String choosedFieldName = choosedListModel.getElementAt(i).toString();
                int fieldNameEndIndex = choosedFieldName.indexOf("_ (")-1;
                String fieldName = choosedFieldName.substring(0, fieldNameEndIndex);
                int schemaBeginIndex = choosedFieldName.indexOf("Schema:")+8;
                int schemaEndIndex = choosedFieldName.indexOf(")")-1;
                String schemaName = choosedFieldName.substring(schemaBeginIndex, schemaEndIndex);
                GenXmlHandler.setFieldElement(m_inputGenPanel.m_genDistdoc, schemaName, fieldName, m_inputGenPanel.getFistDistPanel().getDistribution());
            }
        }
        else
        {   if(typeString.equals(GenConstants.INPUT_NAME))
            {
                for(int i = 0; i<choosedListModel.size(); i++)
                {
                    String choosedInputName = choosedListModel.getElementAt(i).toString();
                    int inputNameEndIndex = choosedInputName.indexOf("_ (")-1;
                    String inputName = choosedInputName.substring(0, inputNameEndIndex);
                    
                    GenXmlHandler.setInputGenInfoElement(m_inputGenPanel.m_genDistdoc,
                            inputName,
                            m_inputGenPanel.getFistDistPanel().getStreamNum(),
                            m_inputGenPanel.getFistDistPanel().getTupleNum());
                    
                    GenXmlHandler.setInputElement(m_inputGenPanel.m_genDistdoc,
                            inputName, m_inputGenPanel.getFistDistPanel().getDistribution());
                }
            }
        }
        
        m_inputGenPanel.getFistDistPanel().setChooseDistEnabled(true);
        m_inputGenPanel.m_treePanel.m_elementTree.setEnabled(true);
        setComponentsEnabled(false);
        m_inputGenPanel.saveToTempFile();
        m_inputGenPanel.getFistDistPanel().setRateEnabled(true);
    }

    
    /**
     * When user click the "Cancel" button in the ApplyToPanel, this method will be called.
     * Cancel the current operation. Don't apply the distribution to the current choosed input stream or field. 
     */
    public void cancel()
    {
        m_inputGenPanel.getFistDistPanel().setChooseDistEnabled(true);
        m_inputGenPanel.m_treePanel.m_elementTree.setEnabled(true);
        setComponentsEnabled(false);
        m_inputGenPanel.getFistDistPanel().setRateEnabled(true);
    }
   
 
     /**
      * This class is for responsing to the operation that user add item to the choosed list.
      */
    private class addListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            int[] selectedItemsNum = m_availableList.getSelectedIndices();

            for(int i = 0; i<selectedItemsNum.length; i++)
            {
                String currentItem = (String)m_availableListModel.getElementAt(
                                                   selectedItemsNum[i]);

                m_choosedListModel.addElement(currentItem);
            }

            for(int i = 0; i<selectedItemsNum.length; i++)
            {
                m_availableListModel.remove(selectedItemsNum[i]-i);
            }
        }
    }

    
    /**
     * This class is for responsing to the operation that user delete item from the choosed list.
     */
    private class deleteListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            int[] selectedItemsNum = m_choosedList.getSelectedIndices();

            for(int i = 0; i<selectedItemsNum.length; i++)
            {
                String currentItem = (String)m_choosedListModel.getElementAt(selectedItemsNum[i]);
                m_availableListModel.addElement(currentItem);
            }

            for(int i = 0; i<selectedItemsNum.length; i++)
            {
                m_choosedListModel.remove(selectedItemsNum[i]-i);
            }
        }
    }
}


/////////////////////////  end  ApplyToPanel.java  /////////////////////////////
