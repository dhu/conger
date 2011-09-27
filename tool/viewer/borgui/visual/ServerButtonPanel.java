/*
 * ServerBottonPanel.java
 *
 * Created on June 8, 2005, 11:33 AM
 *
 * To change this template, choose Tools | Options and locate the template under
 * the Source Creation and Management node. Right-click the template and choose
 * Open. You can then make changes to the template in the Source Editor.
 */

package borgui.visual;

import javax.swing.*;
import java.awt.*;
import java.util.*;

import javax.swing.event.*;
import java.awt.event.*;

import borgui.xml.*;
import borgui.client.*;
import borgui.common.*;
import borgui.graph.*;


/**
 *
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/13/2005
 *
 */
public class ServerButtonPanel extends JPanel {
    
    protected Vector m_serversIpPort = new Vector();
    protected ButtonGroup m_buttonGroup;
    protected PlotColor m_plotColor;
    protected JPanel m_leftPanel = new JPanel();
    protected JPanel m_centerPanel = new JPanel();
    protected JPanel m_rightPanel = new JPanel();
    
    
    /** Creates a new instance of ServerBottonPanel */
    public ServerButtonPanel() {
        
        setBorder(BorderFactory.createLineBorder(Color.black, 1));
        
        m_leftPanel.setLayout(new BoxLayout(m_leftPanel, BoxLayout.Y_AXIS));
        m_leftPanel.setPreferredSize(new Dimension(50, 92));
        m_leftPanel.setMinimumSize(new Dimension(50,92));
        m_leftPanel.setBackground(Color.WHITE);
        
        m_centerPanel.setLayout(new BoxLayout(m_centerPanel, BoxLayout.Y_AXIS));
        m_centerPanel.setPreferredSize(new Dimension(50, 92));
        m_centerPanel.setMinimumSize(new Dimension(50,92));
        m_centerPanel.setBackground(Color.WHITE);
        
        m_rightPanel.setLayout(new BoxLayout(m_rightPanel, BoxLayout.Y_AXIS));
        m_rightPanel.setPreferredSize(new Dimension(50, 92));
        m_rightPanel.setMinimumSize(new Dimension(50,92));
        m_rightPanel.setBackground(Color.WHITE);
        
        this.setBackground(Color.WHITE);
        
        this.add(m_leftPanel, BorderLayout.WEST);
        this.add(m_centerPanel, BorderLayout.CENTER);
        this.add(m_rightPanel, BorderLayout.EAST);
        
    }
    
    
    
    public void setPlotColor(PlotColor plotColor) {
        m_plotColor = plotColor;
    }
    
    public void setServerIpPort(Vector serversIpPort) {
        removeAllServers();
        m_serversIpPort.clear();
        m_serversIpPort = serversIpPort;
        if(m_serversIpPort.size()>6) {
            System.out.println("Server Number is larger than 6");
        }
        addButtons();
     }
    
    public void addButtons() {
        int buttonNumber = m_serversIpPort.size();
        String nodeIpPort;
        if(buttonNumber>=1) {
            nodeIpPort = (String)m_serversIpPort.get(0);
            JButton server1Button = new JButton("");
            server1Button.setBackground(m_plotColor.getNodeColor(nodeIpPort));
            server1Button.setName(nodeIpPort);
            server1Button.setIcon(new ImageIcon("images/laptop.gif"));
            server1Button.setToolTipText(nodeIpPort);
            m_leftPanel.add(server1Button);
        }
        if(buttonNumber>=2) {
            nodeIpPort = (String)m_serversIpPort.get(1);
            
            JButton server2Button = new JButton("");
            server2Button.setBackground(m_plotColor.getNodeColor(nodeIpPort));
            server2Button.setName(nodeIpPort);
            server2Button.setIcon(new ImageIcon("images/laptop.gif"));
            server2Button.setToolTipText(nodeIpPort);
            m_leftPanel.add(server2Button);
        }
        if(buttonNumber>=3) {
            nodeIpPort = (String)m_serversIpPort.get(2);
            JButton server3Button = new JButton("");
            server3Button.setBackground(m_plotColor.getNodeColor(nodeIpPort));
            server3Button.setName(nodeIpPort);
            server3Button.setIcon(new ImageIcon("images/laptop.gif"));
            server3Button.setToolTipText(nodeIpPort);
            m_centerPanel.add(server3Button);
        }
        if(buttonNumber>=4) {
            nodeIpPort = (String)m_serversIpPort.get(3);
            JButton server4Button = new JButton("");
            server4Button.setBackground(m_plotColor.getNodeColor(nodeIpPort));
            server4Button.setName(nodeIpPort);
            server4Button.setIcon(new ImageIcon("images/laptop.gif"));
            server4Button.setToolTipText(nodeIpPort);
            m_centerPanel.add(server4Button);
        }
        if(buttonNumber>=5) {
            nodeIpPort = (String)m_serversIpPort.get(4);
            JButton server5Button = new JButton("");
            server5Button.setBackground(m_plotColor.getNodeColor(nodeIpPort));
            server5Button.setName(nodeIpPort);
            server5Button.setIcon(new ImageIcon("images/laptop.gif"));
            server5Button.setToolTipText(nodeIpPort);
            m_rightPanel.add(server5Button);
        }
        
        if(buttonNumber>=6) {
            nodeIpPort = (String)m_serversIpPort.get(5);
            JButton server6Button = new JButton("");
            server6Button.setBackground(m_plotColor.getNodeColor(nodeIpPort));
            server6Button.setName(nodeIpPort);
            server6Button.setIcon(new ImageIcon("images/laptop.gif"));
            server6Button.setToolTipText(nodeIpPort);
            m_rightPanel.add(server6Button);
        }
        
    }
    
    public void removeAllServers() {
        m_leftPanel.removeAll();
        m_centerPanel.removeAll();
        m_rightPanel.removeAll();
        repaint();
    }
    
    public void deleteServer(String ipPort) {
        try{
            for(int i = 0; i<m_leftPanel.getComponentCount(); i++) {
                if(m_leftPanel.getComponent(i).getName().equals(ipPort)) {
                    m_leftPanel.remove(i);
                }
            }
            for(int i = 0; i<m_centerPanel.getComponentCount(); i++) {
                if(m_centerPanel.getComponent(i).getName().equals(ipPort)) {
                    m_centerPanel.remove(i);
                }
            }
            for(int i = 0; i<m_rightPanel.getComponentCount(); i++) {
                if(m_rightPanel.getComponent(i).getName().equals(ipPort)) {
                    m_rightPanel.remove(i);
                }
            }
            
            repaint();
        }catch(Exception e) {
            System.out.println("from deleteServer: cannot delete server");
        }
    }
    
     public void changeServerButtonTip(String oldIpPort, String newIpPort) {
        try{
            for(int i = 0; i<m_leftPanel.getComponentCount(); i++) {
                if(m_leftPanel.getComponent(i).getName().equals(oldIpPort)) {
                    ((JButton)m_leftPanel.getComponent(i)).setToolTipText(newIpPort);
                }
            }
            for(int i = 0; i<m_centerPanel.getComponentCount(); i++) {
                if(m_centerPanel.getComponent(i).getName().equals(oldIpPort)) {
                   ((JButton)m_centerPanel.getComponent(i)).setToolTipText(newIpPort);
                }
            }
            for(int i = 0; i<m_rightPanel.getComponentCount(); i++) {
                if(m_rightPanel.getComponent(i).getName().equals(oldIpPort)) {
                   ((JButton)m_rightPanel.getComponent(i)).setToolTipText(newIpPort);
                }
            }
            
            repaint();
        }catch(Exception e) {
            System.out.println("from deleteServer: cannot delete server");
        }
    }
     
}

