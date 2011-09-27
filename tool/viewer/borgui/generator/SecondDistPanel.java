/*
 * SecondDistPanel.java
 *
 * Created on July 30, 2005, 4:04 PM
 *
 */

package borgui.generator;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;

/**
 * A class that implement the right distribution panel.
 * 
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/12/2005
 * 
 */
public class SecondDistPanel extends GeneralApplyCancelPanel
{
    protected JLabel[]      m_distParamsLabel = new JLabel[5];
    protected JTextField[]  m_distParamsTextField = new JTextField[5];
    protected JComboBox     m_distComboBox = new JComboBox();
    protected JLabel        m_distChooseLabel = new JLabel(" Choose Distribution:");
    protected String        m_paramName = new String("");
    
    protected InputGenPanel      m_inputGenPanel;
    protected GenActionHandler   m_genActionHandler;
    protected ParamDistribution  m_paramDist = new ParamDistribution();
    
    
    /** 
     * Creates a new instance of SecondDistPanel 
     */
    public SecondDistPanel(InputGenPanel inputGenPanel)
    {
        super();
        m_inputGenPanel = inputGenPanel;
        m_genActionHandler = new GenActionHandler(m_inputGenPanel);
        //choosePanel
        setupDistChoosePanel();
        chooseNewDistribution();
        setComponentEnabled(false);
        
        //Button
        m_firstButton.setEnabled(false);
        m_firstButton.setVisible(false);
        m_secondButton.setEnabled(false);
        m_secondButton.setVisible(false);
        m_thirdButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.DIST2_APPLY_ACTION));
        m_fourthButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.DIST2_CANCEL_ACTION));
    }
   
 
    /**
     * Initialize the layout of the distribution panel
     */
    protected void setupDistChoosePanel()
    {
        GridLayout gridLayout = new GridLayout(7,2,0,5);
        m_choosePanel.setLayout(gridLayout);
        m_choosePanel.add(m_distChooseLabel);
        
        m_distComboBox = new JComboBox(new Object[] {"CONSTANT","UNIFORM","NORMAL","EXPONENTIAL","LINEAR","RANDOM WALK","RANDOMDIR WALK"});
        
        m_distComboBox.addActionListener(new DistributionSelectionListener());
        m_choosePanel.add(m_distComboBox);
        
        for (int i=0; i<5; i++)
        {
            m_distParamsLabel[i] = new JLabel(" Label" + i);
            m_distParamsLabel[i].setVisible(false);
            m_choosePanel.add(m_distParamsLabel[i]);
            
            m_distParamsTextField[i] = new JTextField("0.0");
            m_distParamsTextField[i].setEditable(true);
            m_distParamsTextField[i].setVisible(false);
            m_choosePanel.add(m_distParamsTextField[i]);
        }
    }

    
    /**
     * Choose a new Distribution from the combobox.
     */
    public void chooseNewDistribution()
    {
        int loopvar;
        int curDist = m_distComboBox.getSelectedIndex();

        switch (curDist)
        {
            case GenConstants.DIST_TYPE_CONSTANT:
                for (loopvar = 0; loopvar<1; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(true);
                    m_distParamsTextField[loopvar].setVisible(true);
                }

                m_distParamsLabel[0].setText(" "+GenConstants.DIST_CONSTANT_PARAM[0]+":");
                m_distParamsTextField[0].setText("0.0");

                for (loopvar = 1; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(false);
                    m_distParamsTextField[loopvar].setVisible(false);
                }

                for (loopvar = 0; loopvar<5; ++loopvar){
                    m_distParamsLabel[loopvar].setEnabled(true);
                    m_distParamsTextField[loopvar].setEditable(true);
                }

                break;

            case GenConstants.DIST_TYPE_UNIFORM:
                for (loopvar = 0; loopvar<2; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(true);
                    m_distParamsTextField[loopvar].setVisible(true);
                }

                m_distParamsLabel[0].setText(" " + GenConstants.DIST_UNIFORM_PARAM[0]+":");
                m_distParamsLabel[1].setText(" " + GenConstants.DIST_UNIFORM_PARAM[1]+":");
                m_distParamsTextField[0].setText("0.0");
                m_distParamsTextField[1].setText("0.0");

                for (loopvar = 2; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(false);
                    m_distParamsTextField[loopvar].setVisible(false);
                }
                
                for (loopvar = 0; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setEnabled(true);
                    m_distParamsTextField[loopvar].setEditable(true);
                }

                break;

            case GenConstants.DIST_TYPE_NORMAL:
                for (loopvar = 0; loopvar<2; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(true);
                    m_distParamsTextField[loopvar].setVisible(true);
                }

                m_distParamsLabel[0].setText(" " + GenConstants.DIST_NORMAL_PARAM[0]+":");
                m_distParamsLabel[1].setText(" " + GenConstants.DIST_NORMAL_PARAM[1]+":");
                m_distParamsTextField[0].setText("0.0");
                m_distParamsTextField[1].setText("0.0");

                for (loopvar = 2; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(false);
                    m_distParamsTextField[loopvar].setVisible(false);
                }
                
                for (loopvar = 0; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setEnabled(true);
                    m_distParamsTextField[loopvar].setEditable(true);
                }
                
                if(m_paramName.equals("Lower bound")
                || m_paramName.equals("Upper bound")
                || m_paramName.equals("Probability")
                || m_paramName.equals("Mean")
                || m_paramName.equals("Standard deviation"))
                {
                    for (loopvar = 0; loopvar<5; ++loopvar)
                    {
                        m_distParamsLabel[loopvar].setEnabled(false);
                        m_distParamsTextField[loopvar].setEditable(false);
                    }
                }
                
                break;

            case GenConstants.DIST_TYPE_EXPONENTIAL:
                for (loopvar = 0; loopvar<1; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(true);
                    m_distParamsTextField[loopvar].setVisible(true);
                }

                m_distParamsLabel[0].setText(" "+ GenConstants.DIST_EXPONENTIAL_PARAM[0]+":");
                m_distParamsTextField[0].setText(m_paramDist.getParamDistParamValue(0).toString());

                for (loopvar = 1; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(false);
                    m_distParamsTextField[loopvar].setVisible(false);
                }
                
                for (loopvar = 0; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setEnabled(true);
                    m_distParamsTextField[loopvar].setEditable(true);
                }

                if(m_paramName.equals("Lower bound")
                || m_paramName.equals("Upper bound")
                || m_paramName.equals("Probability")
                || m_paramName.equals("Mean")
                || m_paramName.equals("Standard deviation"))
                {
                    for (loopvar = 0; loopvar<5; ++loopvar)
                    {
                        m_distParamsLabel[loopvar].setEnabled(false);
                        m_distParamsTextField[loopvar].setEditable(false);
                    }
                }
                
                break;

            case GenConstants.DIST_TYPE_LINEAR:
                for (loopvar = 0; loopvar<4; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(true);
                    m_distParamsTextField[loopvar].setVisible(true);
                }

                m_distParamsLabel[0].setText(" " + GenConstants.DIST_LINEAR_PARAM[0]+":");
                m_distParamsLabel[1].setText(" " + GenConstants.DIST_LINEAR_PARAM[1]+":");
                m_distParamsLabel[2].setText(" " + GenConstants.DIST_LINEAR_PARAM[2]+":");
                m_distParamsLabel[3].setText(" " + GenConstants.DIST_LINEAR_PARAM[3]+":");
                m_distParamsTextField[0].setText("0.0");
                m_distParamsTextField[1].setText("0.0");
                m_distParamsTextField[2].setText("0.0");
                m_distParamsTextField[3].setText("0.0");

                for (loopvar = 4; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(false);
                    m_distParamsTextField[loopvar].setVisible(false);
                }

                for (loopvar = 0; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setEnabled(true);
                    m_distParamsTextField[loopvar].setEditable(true);
                }

                break;

            case GenConstants.DIST_TYPE_RANDOM_WALK:
                for (loopvar = 0; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(true);
                    m_distParamsTextField[loopvar].setVisible(true);
                }

                m_distParamsLabel[0].setText(" " + GenConstants.DIST_RANDOM_WALK_PARAM[0]+":");
                m_distParamsLabel[1].setText(" " + GenConstants.DIST_RANDOM_WALK_PARAM[1]+":");
                m_distParamsLabel[2].setText(" " + GenConstants.DIST_RANDOM_WALK_PARAM[2]+":");
                m_distParamsLabel[3].setText(" " + GenConstants.DIST_RANDOM_WALK_PARAM[3]+":");
                m_distParamsLabel[4].setText(" " + GenConstants.DIST_RANDOM_WALK_PARAM[4]+":");
                m_distParamsTextField[0].setText("0.0");
                m_distParamsTextField[1].setText("0.0");
                m_distParamsTextField[2].setText("0.0");
                m_distParamsTextField[3].setText("0.0");
                m_distParamsTextField[4].setText("0.0");
                
                for (loopvar = 0; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setEnabled(true);
                    m_distParamsTextField[loopvar].setEditable(true);
                }

                break;

            case GenConstants.DIST_TYPE_RANDOMDIR_WALK:
                for (loopvar = 0; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setVisible(true);
                    m_distParamsLabel[loopvar].setEnabled(true);
                    
                    m_distParamsTextField[loopvar].setVisible(true);
                }

                m_distParamsLabel[0].setText(" " + GenConstants.DIST_RANDOM_WALK_PARAM[0]+":");
                m_distParamsLabel[1].setText(" " + GenConstants.DIST_RANDOM_WALK_PARAM[1]+":");
                m_distParamsLabel[2].setText(" " + GenConstants.DIST_RANDOM_WALK_PARAM[2]+":");
                m_distParamsLabel[3].setText(" " + GenConstants.DIST_RANDOM_WALK_PARAM[3]+":");
                m_distParamsLabel[4].setText(" " + GenConstants.DIST_RANDOM_WALK_PARAM[4]+":");
                m_distParamsTextField[0].setText("0.0");
                m_distParamsTextField[1].setText("0.0");
                m_distParamsTextField[2].setText("0.0");
                m_distParamsTextField[3].setText("0.0");
                m_distParamsTextField[4].setText("0.0");
                
                for (loopvar = 0; loopvar<5; ++loopvar)
                {
                    m_distParamsLabel[loopvar].setEnabled(true);
                    m_distParamsTextField[loopvar].setEditable(true);
                }

                break;
        }
    }

    
    /**
     * Initialize the distribution configuration based on the current XML file
     */
    public void chooseCurrentDistribution()
    {
        int loopvar;
        int curDist = m_distComboBox.getSelectedIndex();

        switch (curDist)
        {
            case GenConstants.DIST_TYPE_CONSTANT:
                m_distParamsTextField[0].setText(m_paramDist.getParamDistParamValue(0).toString());
                break;

            case GenConstants.DIST_TYPE_UNIFORM:
                m_distParamsTextField[0].setText(m_paramDist.getParamDistParamValue(0).toString());
                m_distParamsTextField[1].setText(m_paramDist.getParamDistParamValue(1).toString());
                break;

            case GenConstants.DIST_TYPE_NORMAL:
                m_distParamsTextField[0].setText(m_paramDist.getParamDistParamValue(0).toString());
                m_distParamsTextField[1].setText(m_paramDist.getParamDistParamValue(1).toString());
                break;

            case GenConstants.DIST_TYPE_EXPONENTIAL:
                m_distParamsTextField[0].setText(m_paramDist.getParamDistParamValue(0).toString());
                break;

            case GenConstants.DIST_TYPE_LINEAR:
                m_distParamsTextField[0].setText(m_paramDist.getParamDistParamValue(0).toString());
                m_distParamsTextField[1].setText(m_paramDist.getParamDistParamValue(1).toString());
                m_distParamsTextField[2].setText(m_paramDist.getParamDistParamValue(2).toString());
                m_distParamsTextField[3].setText(m_paramDist.getParamDistParamValue(3).toString());
                break;

            case GenConstants.DIST_TYPE_RANDOM_WALK:
                m_distParamsTextField[0].setText(m_paramDist.getParamDistParamValue(0).toString());
                m_distParamsTextField[1].setText(m_paramDist.getParamDistParamValue(1).toString());
                m_distParamsTextField[2].setText(m_paramDist.getParamDistParamValue(2).toString());
                m_distParamsTextField[3].setText(m_paramDist.getParamDistParamValue(3).toString());
                m_distParamsTextField[4].setText(m_paramDist.getParamDistParamValue(4).toString());
                break;

            case GenConstants.DIST_TYPE_RANDOMDIR_WALK:
                m_distParamsTextField[0].setText(m_paramDist.getParamDistParamValue(0).toString());
                m_distParamsTextField[1].setText(m_paramDist.getParamDistParamValue(1).toString());
                m_distParamsTextField[2].setText(m_paramDist.getParamDistParamValue(2).toString());
                m_distParamsTextField[3].setText(m_paramDist.getParamDistParamValue(3).toString());
                m_distParamsTextField[4].setText(m_paramDist.getParamDistParamValue(4).toString());
                break;
        }
    }

    
    /**
     * Response to the select action of the combobox
     */
    private class DistributionSelectionListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            chooseNewDistribution();
        }
    }
    

    /**
     * Set wheather the components in the right distribution panel are enabled.
     *
     * @param tf true or false
     */
    protected void setComponentEnabled(boolean tf)
    {
        m_thirdButton.setEnabled(tf);
        m_fourthButton.setEnabled(tf);
        
        for (int i=0; i<5; i++)
        {
            m_distParamsLabel[i].setEnabled(tf);
            m_distParamsTextField[i].setEnabled(tf);
        }
        
        m_distComboBox.setEnabled(tf);
        m_distChooseLabel.setEnabled(tf);
    }

    
    /**
     * Restore to the orignal configuration
     */
    protected void restoreInput()
    {
        this.m_distComboBox.setSelectedIndex(m_paramDist.getParamDistType());
    }

    
    /**
     * Check wheather the user input are validate.
     */
    protected boolean validateInput()
    {
        boolean result = true;
        int curDist = this.m_distComboBox.getSelectedIndex();
        Double[] inputValue = new Double[5];

        for(int i = 0; i<5; i++)
        {
            try
            {
                inputValue[i] = new Double(this.m_distParamsTextField[i].getText());
            }
            catch(NumberFormatException e)
            {
                JOptionPane.showConfirmDialog(null,
                        "Your input value must be a number.",
                        "Error", JOptionPane.DEFAULT_OPTION,
                        JOptionPane.ERROR_MESSAGE);
                return false;
            }
        }

        switch (curDist)
        {
            case GenConstants.DIST_TYPE_UNIFORM:
                if (inputValue[0] > inputValue[1])
                {
                    result = false;

                    JOptionPane.showConfirmDialog(null,
                            "Lower bound must be less than upper bound.",
                            "Error", JOptionPane.DEFAULT_OPTION,
                            JOptionPane.ERROR_MESSAGE);
                }

                break;

            case GenConstants.DIST_TYPE_NORMAL:
                if (inputValue[1] < 0)
                {
                    result=false;

                    JOptionPane.showConfirmDialog(null,
                            "Standard deviation must be greater than 0.",
                            "Error", JOptionPane.DEFAULT_OPTION,
                            JOptionPane.ERROR_MESSAGE);
                }

                break;

            case GenConstants.DIST_TYPE_EXPONENTIAL:
                if (inputValue[0] < 0)
                {
                    result=false;
                    JOptionPane.showConfirmDialog(null,
                            "Mean must be greater than 0.",
                            "Error", JOptionPane.DEFAULT_OPTION,
                            JOptionPane.ERROR_MESSAGE);
                }

                break;
                
            case GenConstants.DIST_TYPE_LINEAR:
                if (inputValue[2] > inputValue[3])
                {
                    result = false;

                    JOptionPane.showConfirmDialog(null,
                            "Lower bound must be less than upper bound.",
                            "Error", JOptionPane.DEFAULT_OPTION,
                            JOptionPane.ERROR_MESSAGE);
                }
                break;

            case GenConstants.DIST_TYPE_RANDOM_WALK:
            case GenConstants.DIST_TYPE_RANDOMDIR_WALK:
                if (inputValue[3] > inputValue[4])
                {
                    result = false;

                    JOptionPane.showConfirmDialog(null,
                            "Lower bound must be less than upper bound.",
                            "Error", JOptionPane.DEFAULT_OPTION,
                            JOptionPane.ERROR_MESSAGE);
                }

                if ((inputValue[2] > 1)||(inputValue[2] < 0))
                {
                    result=false;

                    JOptionPane.showConfirmDialog(null,
                            "Probability must be between 0 and 1.",
                            "Error", JOptionPane.DEFAULT_OPTION,
                            JOptionPane.ERROR_MESSAGE);
                }

                break;
        }
        
        return result;
    }

    
    /**
     * Create ParamDistribution based on the current input
     */
    protected ParamDistribution createParamDist()
    {
        
        int curDist = this.m_distComboBox.getSelectedIndex();
        ParamDistribution result = new ParamDistribution(curDist);
        int paramsNum = GenConstants.DIST_PARAM_NUM[curDist];
        
        for(int i = 0; i<paramsNum; i++)
        {
            result.setParamDistParam(i, new Double(this.m_distParamsTextField[i].getText()));
        }

        m_paramDist = result;
        return result;
    }

    
    /**
     * Initialize the distribution configuration based on the current XML file
     */
    protected void setParamDist(ParamDistribution paramDist)
    {
        m_paramDist = paramDist;
        this.m_distComboBox.setSelectedIndex(m_paramDist.getParamDistType());
        chooseCurrentDistribution();
    }

    
    /**
     * Cancel the current change.
     */
    public void cancel()
    {
        setComponentEnabled(false);
        m_inputGenPanel.getFistDistPanel().setChooseDistEnabled(true);
        
        m_inputGenPanel.getFistDistPanel().m_firstButton.setEnabled(true);
        m_inputGenPanel.getFistDistPanel().m_secondButton.setEnabled(true);
        m_inputGenPanel.m_treePanel.m_elementTree.setEnabled(false);
        restoreInput();

        if(m_inputGenPanel.getFistDistPanel().getCurrentType() == GenConstants.INPUT_NAME)
        {
            m_inputGenPanel.getFistDistPanel().setRateEnabled(true);
        }
    }
    

    /**
     * Set the current parameter's name
     *
     * @name the name of the parameter
     */
    public void setParamName(String name)
    {
        m_paramName = name;
    }
}


///////////////////////////  end  SecondDistPanel.java  ////////////////////////
