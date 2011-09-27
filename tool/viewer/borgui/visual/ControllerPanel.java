package borgui.visual;

import javax.swing.*;
import java.awt.*;
import java.awt.image.*;
import java.util.*;
import java.awt.event.*;
import javax.swing.event.*;
import borgui.visual.*;
import borgui.editor.*;
//import borgui.graphic.*;
import borgui.xml.*;
import java.io.*;
import org.jdom.*;
import org.xml.sax.InputSource;
import java.text.DateFormat;


/**
 * A class that implement the controller panel which is in the middle of VisualPanel.
 */
public class ControllerPanel extends JPanel
{
    JLabel               m_label;
    ControllerSlider     m_slider;
    JComboBox            m_comboBox;
    JButton              m_realButton;
    JButton              m_playButton;
    JButton              m_pauseButton;
    JButton              m_stopButton;
    int                  m_timerDelay;
    javax.swing.Timer    m_timer;
    Date                 m_startTime;
    Date                 m_endTime;
    int                  m_interval;
    Vector<Integer>      m_speeds;
    Integer              m_speed;
    Vector<Controllable> m_controllables;
    
    protected final static int DEFAULT_MILLISECONDS = 1000 * (60 * 10);
    protected final static int DEFAULT_INTERVAL     = 1000;

//  protected final static ImageIcon SLIDER_MARK_ICON  = new ImageIcon("images/Information16.gif");
    protected final static JLabel    SLIDER_MARK_LABEL = new JLabel(new ImageIcon("images/Information16.gif"));
    
    
    @SuppressWarnings("unchecked")
    public ControllerPanel()
    {
        super();
        
        m_startTime = new Date();
        m_endTime = new Date(m_startTime.getTime() + DEFAULT_MILLISECONDS);
        m_interval = DEFAULT_INTERVAL;
        
        m_controllables = new Vector<Controllable>();
        m_timerDelay = m_interval;
        
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
        
        // timer
        m_timer = new javax.swing.Timer(m_timerDelay,
                                        new ActionListener()
            {
                public void actionPerformed(ActionEvent evt)
                {
                    if (!m_slider.getValueIsAdjusting())
                    {
                        int val = (int) m_slider.getValue();
                        m_slider.setValue(val + 1 * m_speed);
                        updateVisual(val + 1 * m_speed);
                    }
                }
            }
        );


        /// movie button
        m_realButton = new JButton(new ImageIcon("images/movie.gif"));

        m_realButton.addActionListener( new ActionListener()
            {
                public void actionPerformed( ActionEvent evt )
                {
                    m_slider.setValue(m_slider.dateToValue(new Date()));
                    play();
                }
            }
        );
        
        
        /// play button
        m_playButton = new JButton(new ImageIcon("images/play.gif"));

        m_playButton.addActionListener( new ActionListener()
            {
                public void actionPerformed(ActionEvent evt)
                {
                    play();
                }
            }
        );


        // pause button
        m_pauseButton = new JButton(new ImageIcon("images/pause.gif"));
        m_pauseButton.addActionListener( new ActionListener()
            {   public void actionPerformed(ActionEvent evt)
                {   pause();
                }
            }
        );
        
        
        // stop button
        m_stopButton = new JButton(new ImageIcon("images/stop.gif"));

        m_stopButton.addActionListener( new ActionListener()
            {
                public void actionPerformed(ActionEvent evt)
                {
                    stop();
                }
            }
        );
        
        m_speeds = new Vector<Integer>();
        m_speeds.add(1);
        m_speeds.add(2);
        m_speeds.add(5);
        m_speeds.add(10);
        m_speeds.add(20);
        m_speed = 1;
        
        Vector<String> optionStrs = new Vector<String>();

        for(Iterator i = m_speeds.iterator(); i.hasNext();)
        {
            Integer num = (Integer) i.next();
            optionStrs.add(" X" + num.toString() + " ");
        }

        m_comboBox = new JComboBox(optionStrs);
        m_comboBox.addActionListener(new SpeedListener());
        m_comboBox.setMaximumSize(new Dimension(30, 35));
        
        m_label = new JLabel("");
        m_slider = new ControllerSlider(0, 60 * 15);
        m_slider.setValue(0);

        //System.out.println(m_slider.getLabelTable().toString());

        //Hashtable<Integer, JLabel>  table = m_slider.createStandardLabels(5000);

        //Hashtable<Integer, JLabel>  table = new Hashtable<Integer, JLabel>();
        //table.put(new Integer(0), new JLabel(""));
        //m_slider.setLabelTable(table);

        //Hashtable  table = m_slider.createStandardLabels(5000);
        //table.put(new Integer(0), new JLabel(""));
        //m_slider.setLabelTable(table);

        m_slider.setLabelTable(m_slider.createStandardLabels(5000));
        m_slider.getLabelTable().put(new Integer(0), new JLabel(""));
        
        add(m_realButton);
        add(m_playButton);
        add(m_pauseButton);
        add(m_stopButton);
        add(m_comboBox);
        add(m_label);
        add(m_slider);
    }
    
    public void setStartTime(Date time)
    {
        m_startTime = time;
    }
    
    public void setEndTime(Date time)
    {
        m_endTime = time;
    }
    
    @SuppressWarnings("unchecked")
    public void addMark(Date time)
    {
        Dictionary  table = m_slider.getLabelTable();
        table.put(m_slider.dateToValue(time), SLIDER_MARK_LABEL);
        m_slider.setLabelTable(table);

        //m_slider.getLabelTable().put(m_slider.dateToValue(time), SLIDER_MARK_LABEL);

        m_slider.updateUI();
    }
    

    public void setInterval(int seconds)
    {
        m_interval = seconds;
    }

    
    public void addControllable(Controllable c)
    {
        m_controllables.add(c);
    }

    
    public void removeControllable(Controllable c)
    {
        m_controllables.remove(c);
    }

    
    protected void messageControllables(Date time)
    {
        for(Iterator<Controllable> i = m_controllables.iterator(); i.hasNext();)
        {
            Controllable cont = i.next();
            cont.showTime(time);
        }
    }
    
    
    protected void play()
    {
        if (!m_timer.isRunning())
        {
            m_timer.start();
        }
    }

    
    protected void pause()
    {
        if (m_timer.isRunning())
        {
            m_timer.stop();
        }
    }

    
    protected void stop()
    {
        pause();
        m_slider.setValue(m_slider.getMinimum());
    }

    
    protected void updateVisual(int i)
    {
        messageControllables(m_slider.valueToDate(i));
    }
    
    
    private class ControllerSlider extends JSlider implements ChangeListener
    {
        public ControllerSlider(int start, int end)
        {
            super(JSlider.HORIZONTAL, start, end, start);
            
            addChangeListener(this);
            setMajorTickSpacing(60);
            setMinorTickSpacing(20);
            setPaintTicks(true);
            setPaintLabels(true);
        }
        
        public void stateChanged(ChangeEvent e)
        {
            if (!getValueIsAdjusting())
            {
                int val = (int) getValue();
                updateVisual(val);
            }
        }
        
        
        public void setValue(int value)
        {
            super.setValue(value);

            m_label.setText("  Sim Time: "
                          + DateFormat.getTimeInstance(
                                DateFormat.MEDIUM,
                                Locale.UK).format(valueToDate(value))
                          + "  ");
        }
        
        public Date valueToDate(int value)
        {
            return new Date(m_startTime.getTime() + m_interval * (long) value);
        }
        
        public int dateToValue(Date time)
        {
            return (int)((time.getTime() - m_startTime.getTime()) / m_interval);
        }
    }

    
    private class SpeedListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            m_speed = ((Integer) m_speeds.get(((Integer) m_comboBox.getSelectedIndex()).intValue())).intValue();
        }
    }
}


////////////////////////  end  ControllerPanel.java  ///////////////////////////
