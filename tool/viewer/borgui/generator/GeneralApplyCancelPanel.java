/**
 * GeneralApplyCancelPanel.java
 *
 * Created on July 30, 2005, 10:53 AM
 */

package borgui.generator;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

/**
 * A class that implements a generally used panel.
 * It is extended by ApplyToPanel, FistDistPanel, and SecondDistPanel
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/11/2005
 */
public class GeneralApplyCancelPanel extends JPanel {
    //top
    protected JPanel m_topPanel = new JPanel();
    protected JPanel m_choosePanel = new JPanel();
    //middle
    protected JPanel m_middlePanel = new JPanel();
    
    //bottom
    protected JPanel m_bottomPanel = new JPanel();
    protected JPanel m_leftBottomPanel = new JPanel();
    //protected JPanel m_rightBottomPanel = new JPanel();
    
    protected JPanel m_fourButtonsPanel = new JPanel();
    protected JButton m_firstButton = new JButton("Review");
    protected JButton m_secondButton = new JButton("ApplyTo");
    protected JButton m_thirdButton = new JButton("Apply");
    protected JButton m_fourthButton = new JButton("Cancel");
    
    
    /** 
     * Creates a new instance of GeneralApplyCancelPanel 
     */
    public GeneralApplyCancelPanel() {
        this.setPreferredSize(new Dimension(366, 270));
        //applyCancelButtonsPanel
        m_fourButtonsPanel.setPreferredSize(new Dimension(345,25));
        m_fourButtonsPanel.setLayout(new GridLayout(1,4,2,5));
        
        m_fourButtonsPanel.add(m_firstButton);
        m_fourButtonsPanel.add(m_secondButton);
        m_fourButtonsPanel.add(m_thirdButton);
        m_fourButtonsPanel.add(m_fourthButton);
        
        //bottomPanel
        m_bottomPanel.setPreferredSize(new Dimension(366, 25));
        m_bottomPanel.setLayout(new BoxLayout(m_bottomPanel, BoxLayout.X_AXIS));
        m_leftBottomPanel.setPreferredSize(new Dimension(0, 25));
        m_bottomPanel.add(m_fourButtonsPanel);
        
        
        //topPanel;
        m_topPanel.setLayout(new BoxLayout(m_topPanel, BoxLayout.X_AXIS));
        m_topPanel.setPreferredSize(new Dimension(366, 220));
        m_choosePanel.setPreferredSize(new Dimension(350, 220));
        m_topPanel.add(m_choosePanel);
        m_topPanel.add(Box.createHorizontalStrut(6));
        
        //Layout
        this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        this.add(m_topPanel);
        m_middlePanel.setPreferredSize(new Dimension(356, 25));
        this.add(m_middlePanel);
        this.add(m_bottomPanel);
        
    }
    
}
