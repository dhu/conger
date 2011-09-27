/**
 * DistributionPanel.java
 *
 * Created on July 23, 2005, 9:51 PM
 *
 */

package borgui.generator;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.border.*;
import org.jdom.*;


/**
 * A class that implement the left distribution panel.
 * It can display the distribution of the selected input stream or field.
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/10/2005
 *
 */
public class FirstDistPanel extends GeneralApplyCancelPanel
{
    //title
    protected TitledBorder m_firstDistTitledBorder;

    //choosePanel
    protected JLabel[] m_distParamsLabel = new JLabel[5];
    protected JToggleButton[] m_distParamsButton = new JToggleButton[5];
    protected JComboBox m_distComboBox = new JComboBox();
    protected JLabel m_distChooseLabel = new JLabel(" Choose Distribution:");

    //middlePanel
    protected JLabel m_streamLabel = new JLabel(" Stream Number:   ");
    protected JTextField m_streamNumTextField = new JTextField("1");
    protected JLabel m_tupleLabel = new JLabel(" Tuple Number:   ");
    protected JTextField m_tupleNumTextField = new JTextField("-1");
    
    //other
    protected InputGenPanel m_inputGenPanel;
    protected GenActionHandler m_genActionHandler;
    protected String m_currentType = new String("");
    protected String m_currentSchema = new String("");
    protected String m_currentTypeName = new String("");
    protected String m_currentStream = new String("");
    protected Distribution m_distribution = new Distribution(GenConstants.DIST_TYPE_CONSTANT);
    protected Document m_doc = new Document();

    
    /**
     * Creates a new instance of DistributionPanel
     *
     * @param inputGenPanel the constructor needs the instance of the InputGenPanel to get access to its fields and methonds.
     */
    public FirstDistPanel(InputGenPanel inputGenPanel)
    {
        super();
        m_inputGenPanel = inputGenPanel;
        m_genActionHandler = new GenActionHandler(m_inputGenPanel);
        
        //title
        Border loweredetched = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
        m_firstDistTitledBorder = BorderFactory.createTitledBorder(loweredetched,"Distribution Config");
        setBorder(m_firstDistTitledBorder);
        
        //choosePanel
        setupDistChoosePanel();
        
        comboxChooseDistribution();
        setChooseDistEnabled(false);
        setRateEnabled(false);
        
        m_firstButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.DIST1_REVIEW_ACTION));
        m_secondButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.DIST1_APPLY_TO_ACTION));
        m_thirdButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.DIST1_APPLY_ACTION));
        m_fourthButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.DIST1_CANCEL_ACTION));
        
        m_firstButton.setEnabled(false);
        m_secondButton.setEnabled(false);
    }

    
    /**
     * Initialize the layout of the distribution choose panel.
     * This method only be called in the constructor.
     */
    protected void setupDistChoosePanel()
    {
        GridLayout gridLayout = new GridLayout(7,2,0,5);
        m_choosePanel.setLayout(gridLayout);
        m_choosePanel.add(m_distChooseLabel);
        
        m_distComboBox = new JComboBox(new Object[] {"CONSTANT","UNIFORM","NORMAL","EXPONENTIAL","LINEAR","RANDOM WALK","RANDOMDIR WALK"});
        
        m_distComboBox.addActionListener(new DistributionSelectionListener());
        m_choosePanel.add(m_distComboBox);
        
        ButtonGroup buttonGroup = new ButtonGroup();

        for (int i=0; i<5; i++)
        {
            m_distParamsLabel[i] = new JLabel(" Label" + i);
            m_distParamsLabel[i].setVisible(false);
            m_choosePanel.add(m_distParamsLabel[i]);
            
            m_distParamsButton[i] = new JToggleButton("Choose Distribution");
            m_distParamsButton[i].addActionListener(m_genActionHandler.getAction(GenActionHandler.DIST1_CHOOSEDIST_ACTION));
            m_distParamsButton[i].setVisible(false);
            buttonGroup.add(m_distParamsButton[i]);
            m_choosePanel.add(m_distParamsButton[i]);
        }
        
        //for rate stream number and tuple number
        
        JPanel streamPanel = new JPanel();
        streamPanel.setLayout((new BoxLayout(streamPanel, BoxLayout.X_AXIS)));
        streamPanel.add(m_streamLabel);
        streamPanel.add(m_streamNumTextField);
        
        JPanel tuplePanel = new JPanel();
        tuplePanel.setLayout((new BoxLayout(tuplePanel, BoxLayout.X_AXIS)));
        tuplePanel.add(m_tupleLabel);
        tuplePanel.add(m_tupleNumTextField);
        
        m_choosePanel.add(streamPanel);
        m_choosePanel.add(tuplePanel);
    }

    
    /**
     * Display the parameter depending on the selected distribution type.
     */
    protected void comboxChooseDistribution()
    {
        int loopvar;
        int curDist = m_distComboBox.getSelectedIndex();

        switch (curDist)
        {
            case GenConstants.DIST_TYPE_CONSTANT:
                for (loopvar = 0; loopvar<1; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(true);
                    m_distParamsButton[loopvar].setVisible(true);
                }

                m_distParamsButton[0].setActionCommand("0"+GenConstants.DIST_CONSTANT_PARAM[0]);
                m_distParamsLabel[0].setText(" "+GenConstants.DIST_CONSTANT_PARAM[0]+":");

                for (loopvar = 1; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(false);
                    m_distParamsButton[loopvar].setVisible(false);
                }

                break;

            case GenConstants.DIST_TYPE_UNIFORM:
                for (loopvar = 0; loopvar<2; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(true);
                    m_distParamsButton[loopvar].setVisible(true);
                }

                m_distParamsButton[0].setActionCommand("0"+GenConstants.DIST_UNIFORM_PARAM[0]);
                m_distParamsButton[1].setActionCommand("1"+GenConstants.DIST_UNIFORM_PARAM[1]);
                m_distParamsLabel[0].setText(" " + GenConstants.DIST_UNIFORM_PARAM[0]+":");
                m_distParamsLabel[1].setText(" " + GenConstants.DIST_UNIFORM_PARAM[1]+":");

                for (loopvar = 2; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(false);
                    m_distParamsButton[loopvar].setVisible(false);
                }

                break;

            case GenConstants.DIST_TYPE_NORMAL:
                for (loopvar = 0; loopvar<2; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(true);
                    m_distParamsButton[loopvar].setVisible(true);
                }

                m_distParamsButton[0].setActionCommand("0"+GenConstants.DIST_NORMAL_PARAM[0]);
                m_distParamsButton[1].setActionCommand("1"+GenConstants.DIST_NORMAL_PARAM[1]);
                m_distParamsLabel[0].setText(" " + GenConstants.DIST_NORMAL_PARAM[0]+":");
                m_distParamsLabel[1].setText(" " + GenConstants.DIST_NORMAL_PARAM[1]+":");

                for (loopvar = 2; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(false);
                    m_distParamsButton[loopvar].setVisible(false);
                }

                break;

            case GenConstants.DIST_TYPE_EXPONENTIAL:
                for (loopvar = 0; loopvar<1; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(true);
                    m_distParamsButton[loopvar].setVisible(true);
                }

                m_distParamsButton[0].setActionCommand("0"+GenConstants.DIST_EXPONENTIAL_PARAM[0]);
                m_distParamsLabel[0].setText(" "+ GenConstants.DIST_EXPONENTIAL_PARAM[0]+":");

                for (loopvar = 1; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(false);
                    m_distParamsButton[loopvar].setVisible(false);
                }

                break;

            case GenConstants.DIST_TYPE_LINEAR:
                for (loopvar = 0; loopvar<4; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(true);
                    m_distParamsButton[loopvar].setVisible(true);
                }

                m_distParamsButton[0].setActionCommand("0"+GenConstants.DIST_LINEAR_PARAM[0]);
                m_distParamsButton[1].setActionCommand("1"+GenConstants.DIST_LINEAR_PARAM[1]);
                m_distParamsButton[2].setActionCommand("2"+GenConstants.DIST_LINEAR_PARAM[2]);
                m_distParamsButton[3].setActionCommand("3"+GenConstants.DIST_LINEAR_PARAM[3]);
                m_distParamsLabel[0].setText(" " + GenConstants.DIST_LINEAR_PARAM[0]+":");
                m_distParamsLabel[1].setText(" " + GenConstants.DIST_LINEAR_PARAM[1]+":");
                m_distParamsLabel[2].setText(" " + GenConstants.DIST_LINEAR_PARAM[2]+":");
                m_distParamsLabel[3].setText(" " + GenConstants.DIST_LINEAR_PARAM[3]+":");

                for (loopvar = 4; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(false);
                    m_distParamsButton[loopvar].setVisible(false);
                }

                break;

            case GenConstants.DIST_TYPE_RANDOM_WALK:
                for (loopvar = 0; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(true);
                    m_distParamsButton[loopvar].setVisible(true);
                }

                m_distParamsButton[0].setActionCommand("0"+GenConstants.DIST_RANDOM_WALK_PARAM[0]);
                m_distParamsButton[1].setActionCommand("1"+GenConstants.DIST_RANDOM_WALK_PARAM[1]);
                m_distParamsButton[2].setActionCommand("2"+GenConstants.DIST_RANDOM_WALK_PARAM[2]);
                m_distParamsButton[3].setActionCommand("3"+GenConstants.DIST_RANDOM_WALK_PARAM[3]);
                m_distParamsButton[4].setActionCommand("4"+GenConstants.DIST_RANDOM_WALK_PARAM[4]);
                m_distParamsLabel[0].setText(" " + GenConstants.DIST_RANDOM_WALK_PARAM[0]+":");
                m_distParamsLabel[1].setText(" " + GenConstants.DIST_RANDOM_WALK_PARAM[1]+":");
                m_distParamsLabel[2].setText(" " + GenConstants.DIST_RANDOM_WALK_PARAM[2]+":");
                m_distParamsLabel[3].setText(" " + GenConstants.DIST_RANDOM_WALK_PARAM[3]+":");
                m_distParamsLabel[4].setText(" " + GenConstants.DIST_RANDOM_WALK_PARAM[4]+":");
                break;

            case GenConstants.DIST_TYPE_RANDOMDIR_WALK:
                for (loopvar = 0; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(true);
                    m_distParamsLabel[loopvar].setEnabled(true);
                    m_distParamsButton[loopvar].setVisible(true);
                }

                m_distParamsButton[0].setActionCommand("0"+GenConstants.DIST_RANDOMDIR_WALK_PARAM[0]);
                m_distParamsButton[1].setActionCommand("1"+GenConstants.DIST_RANDOMDIR_WALK_PARAM[1]);
                m_distParamsButton[2].setActionCommand("2"+GenConstants.DIST_RANDOMDIR_WALK_PARAM[2]);
                m_distParamsButton[3].setActionCommand("3"+GenConstants.DIST_RANDOMDIR_WALK_PARAM[3]);
                m_distParamsButton[4].setActionCommand("4"+GenConstants.DIST_RANDOMDIR_WALK_PARAM[4]);
                m_distParamsLabel[0].setText(" " + GenConstants.DIST_RANDOMDIR_WALK_PARAM[0]+":");
                m_distParamsLabel[1].setText(" " + GenConstants.DIST_RANDOMDIR_WALK_PARAM[1]+":");
                m_distParamsLabel[2].setText(" " + GenConstants.DIST_RANDOMDIR_WALK_PARAM[2]+":");
                m_distParamsLabel[3].setText(" " + GenConstants.DIST_RANDOMDIR_WALK_PARAM[3]+":");
                m_distParamsLabel[4].setText(" " + GenConstants.DIST_RANDOMDIR_WALK_PARAM[4]+":");

                break;
        }
    }

    
    /**
     * A class that respond to the user's selection in the distribution combox
     */
    private class DistributionSelectionListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            restoreButtonNames();
            comboxChooseDistribution();
            int currentIndex = m_distComboBox.getSelectedIndex();

            if(m_distribution.getDistType()!=currentIndex)
            {
                m_distribution.setDistType(m_distComboBox.getSelectedIndex());
            }
        }
    }
    

    /**
     * Set wheather the components in the left distribution panel are enabled
     *
     * @param tf true or false
     */
    public void setChooseDistEnabled(boolean tf)
    {
        m_firstButton.setEnabled(tf);
        m_secondButton.setEnabled(tf);
        m_thirdButton.setEnabled(tf);
        m_fourthButton.setEnabled(tf);
        //m_firstButton.setEnabled(tf);
        
        for (int i=0; i<5; i++)
        {
            m_distParamsLabel[i].setEnabled(tf);
            m_distParamsButton[i].setEnabled(tf);
        }
        
        m_distComboBox.setEnabled(tf);
        m_distChooseLabel.setEnabled(tf);
    }

    
    /**
     * Set wheather the rate configuration is enabled
     *
     * @param tf true of false
     */
    public void setRateEnabled(boolean tf)
    {
        m_streamLabel.setEnabled(tf);
        m_streamNumTextField.setEnabled(tf);
        m_tupleLabel.setEnabled(tf);
        m_tupleNumTextField.setEnabled(tf);
    }

    
    /**
     * Restore the button names to be "Choose Distribution".
     */
    protected void restoreButtonNames()
    {
        for (int i=0; i<5; i++)
        {
            m_distParamsButton[i].setText("Choose Distribution");
        }
    }
    

    /**
     * Return the current distribution type
     *
     * @return the current distribution type
     */
    public String getCurrentType()
    {
        return m_currentType;
    }
    
    
    /**
     * Return the current distribution type name
     *
     * @return the current distirbuiton type name
     */
    public String getCurrentTypeName()
    {
        return m_currentTypeName;
    }
   
 
    /**
     * Return the distribution of the current input stream of field
     *
     * @return the distribution of the current input stream of field
     */
    public Distribution getDistribution()
    {
        return m_distribution;
    }
   
 
    /**
     * Return the stream number that user input.
     * @retrun the stream number that user input.
     */
    public String getStreamNum()
    {
        return m_streamNumTextField.getText();
    }
   
 
    /**
     * Return the tuple number that user input.
     * @retrun the tuple number that user input.
     */
    public String getTupleNum()
    {
        return this.m_tupleNumTextField.getText();
    }
   
 
    /**
     * Restore the former distribution configuration
     */
    protected void restoreChoose()
    {
        this.m_distComboBox.setSelectedIndex(m_distribution.getDistType());
        restoreButtonNames();
    }
   
 
    /**
     * Set the distribution XML document as the given document
     *
     * @param doc the given XML document
     */
    public void setDoc(Document doc)
    {
        m_doc = doc;
    }
    

    /**
     * Set the given distribution
     *
     * @param distribuiton the given distribution
     */
    public void setDistribution(Distribution distribution)
    {
        m_distribution = distribution;
        m_distComboBox.setSelectedIndex(distribution.m_type);
    }

    
    /**
     * Validate the user input
     */
    protected boolean validateInput()
    {
        boolean result = true;
        Integer streamNumValue;
        Integer tupleNumValue;

        try
        {
            streamNumValue = new Integer(this.m_streamNumTextField.getText());
            tupleNumValue = new Integer(this.m_tupleNumTextField.getText());
        }
        catch(NumberFormatException e)
        {
            JOptionPane.showConfirmDialog(null,
                    "Your input value must be a integer.",
                    "Error", JOptionPane.DEFAULT_OPTION,
                    JOptionPane.ERROR_MESSAGE);
            return false;
        }

        if (streamNumValue <= 0)
        {
            JOptionPane.showConfirmDialog(null,
                    "Stream Number must be greater that 0.",
                    "Error", JOptionPane.DEFAULT_OPTION,
                    JOptionPane.ERROR_MESSAGE);

            return false;
        }
        
        if (tupleNumValue <= 0 && tupleNumValue!=-1)
        {
            JOptionPane.showConfirmDialog(null,
                    "Tuple Number must be greater that 0 or equal to -1.  '-1' means infinite number of tuples.",
                    "Error", JOptionPane.DEFAULT_OPTION,
                    JOptionPane.ERROR_MESSAGE);

            return false;
        }
        
        return result & validateDistValue(m_distribution);
    }

    
    /**
     * Validate the distribution value
     *
     * @param distribution the current distribution configuration
     */
    protected boolean validateDistValue(Distribution distribution)
    {
        double min, max, mean, deviation, probability;

        switch (distribution.getDistType())
        {
            case GenConstants.DIST_TYPE_UNIFORM:
                min = distribution.m_paramsDist[0].getMaxParamValue();
                max = distribution.m_paramsDist[1].getMinParamValue();

                if (min>max)
                {
                    JOptionPane.showConfirmDialog(null,
                            "Lower bound must be less than Upper bound.",
                            "Error", JOptionPane.DEFAULT_OPTION,
                            JOptionPane.ERROR_MESSAGE);
                    return false;
                }

                break;

            case GenConstants.DIST_TYPE_NORMAL:
                mean = distribution.m_paramsDist[0].getMinParamValue();
                deviation = distribution.m_paramsDist[1].getMinParamValue();
                
                if(mean<0 || deviation<0)
                {
                    JOptionPane.showConfirmDialog(null,
                            "Mean or Standard Deviation must greater than 0.",
                            "Error", JOptionPane.DEFAULT_OPTION,
                            JOptionPane.ERROR_MESSAGE);
                    
                    return false;
                }

                break;

            case GenConstants.DIST_TYPE_EXPONENTIAL:
                mean = distribution.m_paramsDist[0].getMinParamValue();

                if(mean<0)
                {
                    JOptionPane.showConfirmDialog(null,
                            "Mean must greater than 0.",
                            "Error", JOptionPane.DEFAULT_OPTION,
                            JOptionPane.ERROR_MESSAGE);
                    
                    return false;
                    
                }

                break;

            case GenConstants.DIST_TYPE_LINEAR:
                min = distribution.m_paramsDist[2].getMaxParamValue();
                max = distribution.m_paramsDist[3].getMinParamValue();

                if(min>max)
                {
                    JOptionPane.showConfirmDialog(null,
                            "Lower bound must be less than Upper bound.",
                            "Error", JOptionPane.DEFAULT_OPTION,
                            JOptionPane.ERROR_MESSAGE);
                    return false;
                }

                break;

            case GenConstants.DIST_TYPE_RANDOM_WALK:
                min = distribution.m_paramsDist[3].getMaxParamValue();
                max = distribution.m_paramsDist[4].getMinParamValue();

                if(min>max)
                {
                    JOptionPane.showConfirmDialog(null,
                            "Lower bound must lower than Upper bound.",
                            "Error", JOptionPane.DEFAULT_OPTION,
                            JOptionPane.ERROR_MESSAGE);
                    return false;
                }
                
                probability = distribution.m_paramsDist[2].getMaxParamValue();

                if(probability>1)
                {
                    JOptionPane.showConfirmDialog(null,
                            "Probability must between 0 and 1.",
                            "Error", JOptionPane.DEFAULT_OPTION,
                            JOptionPane.ERROR_MESSAGE);
                    
                    return false;
                }

                probability = distribution.m_paramsDist[2].getMinParamValue();

                if(probability<0)
                {
                    JOptionPane.showConfirmDialog(null,
                            "Probability must between 0 and 1.",
                            "Error", JOptionPane.DEFAULT_OPTION,
                            JOptionPane.ERROR_MESSAGE);
                    
                    return false;
                }

                break;
        }

        return true;
    }

    
    /**
     * Save the distribution configuration for the current input stream of field to XML document.
     */
    protected void saveDistToXmlDoc()
    {
        boolean validate = validateInput();

        if(validate)
        {
            m_inputGenPanel.m_treePanel.m_elementTree.setEnabled(true);
            m_firstButton.setEnabled(true);
            m_fourthButton.setEnabled(false);
            
            if(m_currentType.equals(GenConstants.FIELD_NAME))
            {
                GenXmlHandler.setFieldElement(m_inputGenPanel.m_genDistdoc,
                        m_currentSchema,
                        m_currentTypeName,
                        m_distribution);
                
            }
            else if(m_currentType.equals(GenConstants.INPUT_NAME) )
            {
                GenXmlHandler.setInputGenInfoElement(m_inputGenPanel.m_genDistdoc,
                        m_currentTypeName,
                        m_streamNumTextField.getText(),
                        m_tupleNumTextField.getText());
                GenXmlHandler.setInputElement(m_inputGenPanel.m_genDistdoc,
                        m_currentTypeName,
                        m_distribution);
            }
        }

        m_inputGenPanel.saveToTempFile();
    }

    
    /**
     * This method will be called when user click the "Cancel" button in the left distribution panel
     */
    public void cancel()
    {
        restoreChoose();
        m_inputGenPanel.m_treePanel.m_elementTree.setEnabled(true);
        setChooseDistEnabled(false);
        m_firstButton.setEnabled(false);
        m_secondButton.setEnabled(false);
        this.m_streamLabel.setEnabled(false);
        this.m_streamNumTextField.setEnabled(false);
        this.m_tupleLabel.setEnabled(false);
        this.m_tupleNumTextField.setEnabled(false);
    }
    
    
    /**
     * When user click the field in the tree panel, this method will be called.
     * It import the distribution configuration for the current field.
     * If the current field has no distribution configuration, it will be configured as "Constant".
     *
     * @param fieldName the name of the field
     * @param distribution distribution configuration for the current field to be import
     * @param schemaName the name of the schema which this field belongs
     * @param nameAttrValue the name of the input stream whose schema has this field.
     */
    public void initFieldConfig(String fieldName, Distribution distribution, String schemaName, String nameAttrValue)
    {
        m_firstDistTitledBorder.setTitle("Distribution of Field: '"+ fieldName +"'");

        setRateEnabled(false);
        setChooseDistEnabled(true);
        restoreButtonNames();

        m_firstButton.setEnabled(true);
        m_secondButton.setEnabled(true);
        m_currentType = GenConstants.FIELD_NAME;
        
        setDistribution(distribution);
        m_currentSchema = schemaName;
        
        m_currentTypeName = nameAttrValue;
        m_currentStream = "";
        
        setChooseDistEnabled(true);
    }

    
    /**
     * When user click the input stream in the tree panel, this method will be called.
     * It import the distribution configuration for the current input stream.
     * If the current input stream has no distribution configuration, it will be configured as "Constant".
     *
     * @param streamName the name of the input stream
     * @param distribution distribution configuration for the current input stream to be import
     * @param schemaName the name of the schema which this input stream has
     * @param streamAttrValue the name of the input stream
     * @param genInfoStreamNum the stream number that will be generated
     * @param genInfoTupleNum the tuple number that will be generated
     */
    public void initInputStreamConfig(
            String streamName,
            Distribution distribution,
            String schemaName,
            String streamAttrValue,
            String genInfoStreamNum,
            String genInfoTupleNum)
    {
        m_firstDistTitledBorder.setTitle("Distribution of Input: '"+ streamName+"'");
        
        setRateEnabled(true);
        setChooseDistEnabled(true);
        restoreButtonNames();
        m_firstButton.setEnabled(true);
        m_secondButton.setEnabled(true);
        m_currentType = GenConstants.INPUT_NAME;
        
        
        m_currentStream = streamName;
        setDistribution(distribution);
        m_currentSchema = schemaName;
        m_currentTypeName = streamAttrValue;
        
        m_streamNumTextField.setText(genInfoStreamNum);
        m_tupleNumTextField.setText(genInfoTupleNum);
        
        setChooseDistEnabled(true);
    }

    
    /**
     * When user click the botton "Choose Distribution", this method will be called.
     * It import the distribution configuration for the current ParamDistribution.
     * If the current ParamDistribution has no distribution configuration, it will be configured as "Constant".
     *
     * @param selectedParamIndex the index of the current selected paramDistribution
     * @param selectedParam the parameter name of the selected paramDistribution
     */
    public void chooseParmDist(String selectedParamIndex, String selectedParam)
    {
        ParamDistribution paramDist = m_distribution.getParamDist(new Integer(selectedParamIndex));
        
        setChooseDistEnabled(false);
        setRateEnabled(false);
        m_firstButton.setEnabled(false);
        m_secondButton.setEnabled(false);
        
        m_inputGenPanel.getSecondDistTitledBorder().setTitle("Distribution of Param: '"+ selectedParam+"'");
        m_inputGenPanel.getSecondDistPanel().setParamName(selectedParam);
        
        //Enabled
        m_inputGenPanel.getSecondDistPanel().setComponentEnabled(true);
        m_inputGenPanel.getSecondDistPanel().setParamDist(paramDist);
        
        m_inputGenPanel.m_treePanel.m_elementTree.setEnabled(false);
        m_inputGenPanel.repaint();
    }
   
 
    /**
     * When user click the botton "Apply" on the SecondDistPanel, this method will be called.
     * It create ParamDistribution based on the input information
     *
     * @param validate weather the input on the SecondDistPanel is validate
     */
    public void applyParamDist(boolean validate)
    {
        if(validate)
        {
            setChooseDistEnabled(true);
            m_firstButton.setEnabled(true);
            m_secondButton.setEnabled(true);
            
            for(int i = 0; i<5; i++)
            {
                if(m_distParamsButton[i].isSelected())
                {
                    m_distParamsButton[i].setText("Change");
                    ParamDistribution paramDist = m_inputGenPanel.getSecondDistPanel().createParamDist();
                    m_distribution.setParamDist(i, paramDist);
                }
            }

            m_inputGenPanel.m_treePanel.m_elementTree.setEnabled(false);
            if(getCurrentType() == GenConstants.INPUT_NAME) {
                setRateEnabled(true);
            }
        }
    }
}


////////////////////////  end  FirstDistPanel.java  ////////////////////////////
