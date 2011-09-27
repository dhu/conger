/**
 * InputGenPanel.java
 *
 * Created on July 20, 2005, 4:16 PM
 *
 */

package borgui.generator;

import javax.swing.*;
import java.awt.*;
import java.io.*;
import org.jdom.*;
import org.xml.sax.*;
import javax.swing.border.*;

import borgui.xml.*;
import borgui.common.SwingPanel;

import org.jdom.output.*;

/**
 * A class that implement the main panel of the input Generator
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/11/2005
 *
 */
public class InputGenPanel extends JPanel implements Generator
{
    //toolbar
    protected JToolBar      m_jToolBar       = new JToolBar();
    protected JButton       m_loadDistButton = new JButton();
    protected JButton       m_saveDistButton = new JButton();
    protected JButton       m_socketDistStartButton = new JButton();

    protected JButton       m_loadTupleButton = new JButton();
    protected JButton       m_saveTupleButton = new JButton();
    protected JButton       m_socketTupleStartButton = new JButton();

    protected JButton       m_socketStopButton = new JButton();

    //left
    protected TreePanel     m_treePanel = new TreePanel(this);

    //rightTop
    protected FirstDistPanel   m_firstDistPanel  = new FirstDistPanel(this);
    protected SecondDistPanel  m_secondDistPanel = new SecondDistPanel(this);
    protected TitledBorder     m_secondDistTitledBorder;

    //rightMiddle
    protected JPanel        m_rightMiddlePanel;
    protected JScrollPane   m_twoPanelView;
    protected ReviewPanel   m_reviewPanel  = new ReviewPanel();
    protected ApplyToPanel  m_applyToPanel = new ApplyToPanel(this);
    protected TitledBorder  m_middleTitledBorder;

    //rightBottom
    protected JPanel        m_rightBottomPanel;
    protected TitledBorder  m_bottomTitledBorder;
    protected RatePanel     m_ratePanel = new RatePanel(this);


    //other
    protected GenActionHandler    m_genActionHandler;
    protected Document            m_genDistdoc    = new Document();
    protected String              m_tupleFileName = GenConstants.TUPLE_FILE_NAME;


    /**
     * Creates a new instance of InputGenPanel
     */
    public InputGenPanel()
    {
        super();
        m_genActionHandler = new GenActionHandler(this);

        //layout
        JPanel leftPanel     = new JPanel();
        JPanel rightPanel    = new JPanel();
        JPanel rightTopPanel = new JPanel();

        m_rightMiddlePanel = new JPanel();
        m_rightBottomPanel = new JPanel();

        //leftPanel
        leftPanel.setPreferredSize(new Dimension(260, 628));
        JScrollPane treeScrollPane = new JScrollPane(m_treePanel);
        m_treePanel.setBackground(Color.WHITE);
        treeScrollPane.setPreferredSize(new Dimension(260, 628));
        leftPanel.add(treeScrollPane);

        //ToolBar
        m_jToolBar.setFloatable(false);

        m_loadDistButton.setIcon(new ImageIcon("images/loadGenerator/load_D.gif"));
        m_loadDistButton.setToolTipText("Load a Query Network/Distribution File");
        m_loadDistButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.LOAD_DIST_ACTION));
        m_jToolBar.add(m_loadDistButton);

        m_saveDistButton.setIcon(new ImageIcon("images/loadGenerator/save_D.gif"));
        m_saveDistButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.SAVE_DIST_ACTION));
        m_saveDistButton.setToolTipText("Save Current Distribution File");
        m_jToolBar.add(m_saveDistButton);

        m_socketDistStartButton.setIcon(new ImageIcon("images/loadGenerator/socket_D.gif"));
        m_socketDistStartButton.setToolTipText("Sending new tupples to Borealis");
        m_socketDistStartButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.SOCKET_DIST_START_ACTION));
        m_jToolBar.add(m_socketDistStartButton);
        m_socketDistStartButton.setEnabled(false);

        m_jToolBar.addSeparator();
        m_jToolBar.addSeparator();

        m_loadTupleButton.setIcon(new ImageIcon("images/loadGenerator/load_T.gif"));
        m_loadTupleButton.setToolTipText("Load Tuple File");
        m_loadTupleButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.LOAD_TUPLE_ACTION));
        m_jToolBar.add(m_loadTupleButton);

        m_saveTupleButton.setIcon(new ImageIcon("images/loadGenerator/save_T.gif"));
        m_saveTupleButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.SAVE_TUPLE_ACTION));
        m_saveTupleButton.setToolTipText("Save Tupples to a File");
        m_jToolBar.add(m_saveTupleButton);
        m_saveTupleButton.setEnabled(false);

        m_socketTupleStartButton.setIcon(new ImageIcon("images/loadGenerator/socket_T.gif"));
        m_socketTupleStartButton.setToolTipText("Sending exist tupples to Borealis");
        m_socketTupleStartButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.SOCKET_TUPLE_START_ACTION));
        m_jToolBar.add(m_socketTupleStartButton);
        m_socketTupleStartButton.setEnabled(false);

        m_jToolBar.addSeparator();
        m_jToolBar.addSeparator();

        m_socketStopButton.setIcon(new ImageIcon("images/loadGenerator/stopsign.gif"));
        m_socketStopButton.setToolTipText("Stop Sending Tupples");
        m_socketStopButton.addActionListener(m_genActionHandler.getAction(GenActionHandler.SOCKET_DIST_STOP_ACTION));
        m_jToolBar.add(m_socketStopButton);
        m_socketStopButton.setEnabled(false);

        //rightPanel
        rightPanel.setPreferredSize(new Dimension(732, 628));
        rightPanel.setLayout(new BoxLayout(rightPanel, BoxLayout.Y_AXIS));
        rightPanel.add(rightTopPanel);
        rightPanel.add(m_rightMiddlePanel);
        rightPanel.add(m_rightBottomPanel);

        //rightTopPanel
        rightTopPanel.setLayout(new GridLayout(1,2, 5,5));


        m_firstDistPanel.setPreferredSize(new Dimension(366,270));
        rightTopPanel.add(m_firstDistPanel);

        Border loweredetched = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
        m_secondDistTitledBorder = BorderFactory.createTitledBorder(loweredetched,"Distribution of Param");
        m_secondDistPanel.setPreferredSize(new Dimension(366,270));
        m_secondDistPanel.setBorder(m_secondDistTitledBorder);
        rightTopPanel.add(m_secondDistPanel);


        //rightMiddlePanel
        m_rightMiddlePanel.setPreferredSize(new Dimension(700, 200));
        m_middleTitledBorder = BorderFactory.createTitledBorder(loweredetched,"Apply To");
        m_rightMiddlePanel.setBorder(m_middleTitledBorder);

        m_twoPanelView = new JScrollPane(m_reviewPanel);

        m_twoPanelView.setBorder(BorderFactory.createEmptyBorder());
        m_twoPanelView.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);
        m_twoPanelView.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        m_twoPanelView.setPreferredSize(new Dimension(700, 200));

        m_applyToPanel.setPreferredSize(new Dimension(600,170));

        m_reviewPanel.setBackground(new Color(238,238,238));
        m_rightMiddlePanel.add(m_twoPanelView);

        //rightBottomPanel
        m_rightBottomPanel.setPreferredSize(new Dimension(700, 60));
        m_bottomTitledBorder = BorderFactory.createTitledBorder(loweredetched,"Rate");
        m_rightBottomPanel.setBorder(m_bottomTitledBorder);
        m_rightBottomPanel.add(m_ratePanel);

        //Layout
        this.setLayout(new BorderLayout());

        add(leftPanel,BorderLayout.WEST);
        add(rightPanel,BorderLayout.EAST);
        add(m_jToolBar, BorderLayout.NORTH);


        //Enabled
        this.m_ratePanel.setRateEnabled(false);
        m_applyToPanel.setComponentsEnabled(false);
    }


    /**
     * Return the class of the left distribution panel.
     */
    public FirstDistPanel getFistDistPanel()
    {
        return this.m_firstDistPanel;
    }


    /**
     * Return the class of the right distribution panel.
     */
    public SecondDistPanel getSecondDistPanel()
    {
        return this.m_secondDistPanel;
    }


    /**
     * Return the class of the rate panel.
     */
    public RatePanel getRatePanel()
    {
        return this.m_ratePanel;
    }


    /**
     * Return the TitledBorder of the right distribution panel.
     *
     * @param return the TitledBorder of the right distribution panel
     */
    public TitledBorder getSecondDistTitledBorder()
    {
        return m_secondDistTitledBorder;
    }


    /**
     * Load the query network XML file
     */
    public void loadNetwork()
    {
        File file = SwingPanel.showFileDialog("Load Query/Distribution");

        if (file != null)
        {
            try
            {   Document  doc = XmlHandler.xmlToDoc(file);

                m_genDistdoc = doc;
                m_firstDistPanel.setDoc(doc);
                m_treePanel.displayDoc(doc);
                m_treePanel.m_elementTree.setEnabled(true);

                //Enabled
                this.m_ratePanel.setRateEnabled(true);

                saveToDefaultGenXmlFile(doc);
            }
            catch (IOException e)
            {   JOptionPane.showConfirmDialog(null,
                                              e.getMessage(),
                                              "Could not open an XML file.",
                                              JOptionPane.CLOSED_OPTION,
                                              JOptionPane.ERROR_MESSAGE);
            }
            catch (Exception e)
            {   JOptionPane.showConfirmDialog(null,
                                              e.getMessage(),
                                              "XML parse error",
                                              JOptionPane.CLOSED_OPTION,
                                              JOptionPane.ERROR_MESSAGE);
            }
        }

        m_socketDistStartButton.setEnabled(true);
        m_socketStopButton.setEnabled(true);
    }


    /**
     * Create a default distribution XML file, and save.
     */
    protected void saveToDefaultGenXmlFile(Document originalDoc)
    {
        File genDistTempFile = new File("/tmp/GenDistributionTemp.xml");

        if (genDistTempFile != null)
        {
            try
            {   Document resultDoc = GenXmlHandler.createDefaultGenXmlDoc(originalDoc);
                XmlHandler.docToXml(resultDoc, new FileOutputStream(genDistTempFile));

            }
            catch (java.io.FileNotFoundException  e)
            {
                System.err.println(e.getMessage());
            }
        }
    }


    /**
     * Export the current distribution XML file.
     */
    public void exportGenXmlToFile()
    {
        File file = SwingPanel.showFileDialog("Save Distribution File");

        // if a file was chosen to save
        if (file != null)
        {
            try
            {   XmlHandler.docToXml(m_genDistdoc, new FileOutputStream(file));
            }
            catch(java.io.FileNotFoundException e)
            {
                System.err.println(e.getMessage());
            }
        }
    }


    /**
     * Create a temp distribution XML file for sending data by Socket.
     */
    public void saveToTempFile()
    {
        File file = new File("/tmp/GenDistributionTemp.xml");

        if (file != null)
        {
            try
            {   XmlHandler.docToXml(m_genDistdoc, new FileOutputStream(file));
            }
            catch(java.io.FileNotFoundException e)
            {
                System.err.println(e.getMessage());
            }
        }
    }


    /**
     * Show the Review panel
     *
     * @param title the name of the input stream or field that is currently configured.
     */
    public void showReviewView(String title)
    {
        m_twoPanelView.setViewportView(m_reviewPanel);
        m_middleTitledBorder.setTitle("Review of "+"'"+title+"'");

        repaint();
    }


    /**
     * Show the ApplyToPanel
     *
     * @param type it can be input stream or field.
     */

    public void showApplyToView(String type)
    {
        m_twoPanelView.setViewportView(m_applyToPanel);
        m_middleTitledBorder.setTitle("Apply To " + type);

        repaint();
    }


    /**
     * Create Review tree by the current distribution configuration
     */
    public void createReview(String title)
    {
        showReviewView(title);
        Distribution reviewDist = m_firstDistPanel.getDistribution();
        Element reviewElement = GenXmlHandler.createDistElement(reviewDist);
        m_reviewPanel.setElement(reviewElement);
    }


    /**
     * Save all those tupples that have been just sent to Borealis last time.
     */
    public void saveTuplesToFile()
    {
        File newFile = SwingPanel.showFileDialog("Save Tupples");
        File origin  = new File(GenConstants.TUPLE_FILE_NAME);

        // if a file was chosen to save
        if (newFile != null)
        {
            try
            {   FileInputStream  fis = new FileInputStream(origin);
                FileOutputStream fos = new FileOutputStream(newFile);

                byte[] buf = new byte[1024];
                int i = 0;

                while ((i = fis.read(buf))!=-1)
                {
                    fos.write(buf, 0, i);
                }

                fis.close();
                fos.close();
            }
            catch(Exception e)
            {
                System.err.println(e.getMessage());
            }
        }
    }


    /**
     * Load tuple file
     */
    public void loadTupleFile()
    {
        File file = SwingPanel.showFileDialog("Load Tuple File");

        if (file != null)
        {
            m_tupleFileName = file.getAbsolutePath();

            m_socketTupleStartButton.setEnabled(true);
            m_saveTupleButton.setEnabled(true);
            m_socketStopButton.setEnabled(true);
        }
    }


    /**
     * Get the file that save all tuples.
     */
    public String getTupleFileName()
    {
        return m_tupleFileName;
    }


    /**
     * Set the tuple file.
     */
    public void setTupleFile(String tupleFileName)
    {
        m_tupleFileName = tupleFileName;
    }
}


/////////////////////////////  end  InputGenPanel.java  ////////////////////////
