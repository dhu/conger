/**
 * RatePanel.java
 *
 * Created on September 3, 2005, 12:38 PM
 *
 */

package borgui.generator;

import java.awt.*;
import javax.swing.*;
import javax.swing.event.*;
import org.jdom.*;

import borgui.xml.*;
import java.io.*;

/**
 * A class that represent a panel to show the rate configuration
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/11/2005
 *
 */
public class RatePanel extends JPanel {
    
    protected InputGenPanel m_inputGenPanel;
    protected GenActionHandler m_genActionHandler;
    
    protected JLabel m_rateValueLabel = new JLabel(" Rate Value:   ");
    protected JLabel m_rateScaleLabel = new JLabel(" Scale: ");
    protected JTextField m_changeScaleTextField = new JTextField("100");
    protected JButton m_changeScaleButton = new JButton("Change Scale");
    protected RateSlider m_rateSlider = new RateSlider(0,GenConstants.DEFAULT_RATE_SLIDER_SCALE);
    protected JButton m_sendRateButton = new JButton("Send");
    
    /** 
     * Creates a new instance of RatePanel 
     */
    public RatePanel(InputGenPanel inputGenPanel) {
        super();
        m_inputGenPanel = inputGenPanel;
        m_genActionHandler = new GenActionHandler(m_inputGenPanel);
        
        //setup
        m_rateSlider.setValue(1);
        m_changeScaleButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.CHANGE_RATE_SCALE_ACTION));
        
        m_rateSlider.setPreferredSize(new Dimension(320,50));
        m_rateValueLabel.setPreferredSize(new Dimension(120, 15));
        m_changeScaleTextField.setPreferredSize(new Dimension(40,15));
        
        m_sendRateButton.setIcon(new ImageIcon("images/loadGenerator/socket_D.gif"));
        m_sendRateButton.setToolTipText("Sending");
        m_sendRateButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.SOCKET_RATE_ACTION));
        
        //Layout
        this.setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
        
        add(m_changeScaleButton);
        add(Box.createHorizontalStrut(5));
        add(m_changeScaleTextField);
        add(Box.createHorizontalStrut(10));
        add(m_rateSlider);
        add(Box.createHorizontalStrut(5));
        add(m_rateValueLabel);
        add(m_sendRateButton);
        
    }
    
    /**
     * Change the major and miner tick spacking of the slider
     */
    public void changeScale() {
        String scaleString = m_changeScaleTextField.getText();
        int scaleInt = Integer.parseInt(scaleString);
        m_rateSlider.setMaximum(scaleInt);
        m_rateSlider.setMajorTickSpacing(scaleInt/5);
        m_rateSlider.setMinorTickSpacing(scaleInt/20);
    }
    
    /**
     * Get current rate
     */
    public int getRate()
    {
        return m_rateSlider.getValue();                
    }
    
    /**
     * A class that represent the slider for rate
     */
    private class RateSlider extends JSlider implements ChangeListener {
        
        public RateSlider(int start, int end) {
            super(JSlider.HORIZONTAL, start, end, start);
            
            addChangeListener(this);
            setMajorTickSpacing(GenConstants.DEFAULT_RATE_SLIDER_SCALE/5);
            setMinorTickSpacing(GenConstants.DEFAULT_RATE_SLIDER_SCALE/20);
            setPaintTicks(true);
            setPaintLabels(true);
            
        }
        
        public void stateChanged(ChangeEvent e) {
            
            String scaleString = m_changeScaleTextField.getText();
            int scaleInt = Integer.parseInt(scaleString);
            
            m_rateSlider.setMajorTickSpacing(scaleInt/5);
            m_rateSlider.setMinorTickSpacing(scaleInt/20);
            
            
        }
        
        
        public void setValue(int value) {
            super.setValue(value);
            m_rateValueLabel.setText(" Rate Value: " + value);
        }
        
        public int getValue() {
            return super.getValue();
        }
        
        public void setMajorTickSpacing(int n) {
            super.setMajorTickSpacing(n);

        }
        
        public void setMinorTickSpacing(int n) {
            super.setMinorTickSpacing(n);
        }
    }
    
    public void setRateEnabled(boolean tf)
    {
        this.m_changeScaleButton.setEnabled(tf);
        this.m_changeScaleTextField.setEnabled(tf);
        this.m_rateScaleLabel.setEnabled(tf);
        this.m_rateSlider.setEnabled(tf);
        this.m_rateValueLabel.setEnabled(tf);
        this.m_sendRateButton.setEnabled(tf);
    }
}
