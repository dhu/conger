package borgui.xml;

import org.jdom.*;
import java.util.*;

import borgui.graph.*;
import borgui.editor.*;
import borgui.common.DebugTrace;
import borgui.common.HostEndpoint;

import org.jgraph.graph.*;
import org.jgraph.JGraph;


/**
 * A Class with convenience methods for dealing with jdom documents as
 * well as encoding and decoding graphical state into and out of a document
 *
 */
public class BorealisModelXmlHandler
{
    public static JGraph m_localGraph = new JGraph();

    public static org.jgraph.layout.SugiyamaLayoutAlgorithm m_layout =
              new org.jgraph.layout.SugiyamaLayoutAlgorithm();
    
    /**
     * Default Constructor
     */
    public BorealisModelXmlHandler() {}
    
    /*
    public static void borealisModelToXml(BorealisModel model) {}
    
    public static void xmlToBorealisModel(Document doc) {}
    */


    ////////////////////////////////////////////////////////////////////////////
    //
    //  Transform a parsed XML document (referenced in a model) to a graph.
    //  Returns an empty string if all is well.
    //  The first warning encountered is returned if there were warnings.
    //  The only warning is that the graph is incomplete (missing a stream).
    //  If the graph could not be loaded due to an error, an Exception is
    //  thrown and the graph is cleared.
    //  
    public static String  loadBorealisModel(BorealisModel  model)
                  throws  Exception
    {
        String  warning = "";          // Assume no warnings.
    //..........................................................................


        // clear
        model.clearElementMap();
        model.clearCellMap();

        // make a local copy
        BorealisGraphModel  graphModel = new BorealisGraphModel();
        model.setGraphModel(graphModel);
        BorealisDocument doc = model.getDocument();
        
        graphModel.setBuffering(true);
        
        // maps to create connections with
        // map of "stream"(edge name) to the graph cell (to its import port)
        HashMap<String, GraphCell>  edgeToTargetCell =
                                new HashMap<String, GraphCell>();

        // map of "stream"(edge name) to the graph cell (to its output port)
        HashMap<String, GraphCell>  edgeToSourceCell =
                                new HashMap<String, GraphCell>();

        // schemas and queries have no graph representation

        // get all inputs
        List inputElements = doc.getAllInputs();

        // Go through each input element.
        //
        for (Iterator i = inputElements.iterator(); i.hasNext();)
        {
            InputElement inputElem = (InputElement) i.next();

            // create graph object
            InputGraphCell inputCell = new InputGraphCell();
            inputCell.setUserObject(inputElem.getStream());
            graphModel.insertCell(inputCell);

            // add model mapping
            model.addMapping(inputElem, inputCell);

            // add port mapping
            edgeToSourceCell.put(inputElem.getStream(), inputCell);
        }
        
        // Scan all outputs.
        //
        List outputElements = doc.getAllOutputs();

        for (Iterator i = outputElements.iterator(); i.hasNext();)
        {
            OutputElement outputElem = (OutputElement) i.next();

            // create graph object
            OutputGraphCell outputCell = new OutputGraphCell();
            outputCell.setUserObject(outputElem.getStream());
            graphModel.insertCell(outputCell);

            // add model mapping
            model.addMapping(outputElem, outputCell);

            // add port mapping
            edgeToTargetCell.put(outputElem.getStream(), outputCell);
        }
        
        // Scan all tables
        //
        List tableElements = doc.getAllTables();

        for(Iterator i = tableElements.iterator(); i.hasNext();)
        {
            TableElement tableElem = (TableElement) i.next();

            // create graph object
            TableGraphCell tableCell = new TableGraphCell();
            tableCell.setUserObject(tableElem.getNameValue());
            graphModel.insertCell(tableCell);

            // add model mapping
            model.addMapping(tableElem, tableCell);
            
            // add port mapping
            edgeToTargetCell.put(tableElem.getStream(), tableCell);
        }
        
        
        // get all boxes
        List  boxElements = doc.getAllBoxes();
        BorealisGraphCellFactory fact = new BorealisGraphCellFactory();

        // get all outputs
        for (Iterator i = boxElements.iterator(); i.hasNext();)
        {
            BoxElement boxElement = (BoxElement) i.next();

            // create graph object
            BoxGraphCell boxCell = fact.createBox(boxElement);
            boxCell.setUserObject(boxElement.getNameValue());
            graphModel.insertCell(boxCell);

            // add model mapping
            model.addMapping(boxElement, boxCell);
            
            // handle ins
            List ins = boxElement.getAllIns();

            for (Iterator j = ins.iterator(); j.hasNext();)
            {
                InElement inElem = (InElement) j.next();
                edgeToTargetCell.put(inElem.getStream(), boxCell);
            }
            
            // handle outs
            List outs = boxElement.getAllOuts();

            for (Iterator j = outs.iterator(); j.hasNext();)
            {
                OutElement outElem = (OutElement) j.next();
                edgeToSourceCell.put(outElem.getStream(), boxCell);
            }
            
            // ADD TABLE PORT SUPPORT
            TablerefElement refs = boxElement.getTableref();

            if (refs!=null)
            {
                boxCell.setTableBox(true);
                TablerefPort refPort = boxCell.getTablerefPort();
                String tableName = refs.getTableAttribute();
                String streamName = "";

                for (Iterator j = tableElements.iterator(); j.hasNext();)
                {
                    TableElement tableElement = (TableElement) j.next();

                    if (tableElement.getNameValue().equals(tableName))
                    {
                        streamName = tableElement.getStream();
                    }
                }
                
                // add port mapping
                edgeToSourceCell.put(streamName, boxCell);
            }
        }

        // make connections
        // go through all outs and connect
        // ??? fails on an incomplete graph.
        Set streams = edgeToSourceCell.keySet();
        //Set streams = edgeToTargetCell.keySet();

        for (Iterator i = streams.iterator(); i.hasNext(); )
        {
            String  edgeName = (String)i.next();
            
            BorealisGraphCell  targetCell =
                            (BorealisGraphCell)edgeToTargetCell.get(edgeName);

            if (targetCell == null)
            {   if (warning == null)
                {   warning = "Stream ("+ edgeName + ") is not connected.";
                    System.out.println(warning);
                }
            }
            else
            {   BorealisGraphCell sourceCell =
                            (BorealisGraphCell) edgeToSourceCell.get(edgeName);

                if (!(targetCell instanceof TableGraphCell))
                {
                    graphModel.connect(sourceCell.getOutputPort(),
                                       targetCell.getInputPort(), edgeName);
                }
                else
                {   graphModel.connect(((BoxGraphCell)sourceCell).getTablerefPort(),
                                       ((TableGraphCell)targetCell).getTablePort(),
                                       edgeName);
                }
            }
        }

        graphModel.flushBuffer();
        graphModel.setBuffering(false);
        m_layout.setVertical(false);

        m_localGraph.setModel(graphModel);

        try
        {   m_layout.run(m_localGraph, DefaultGraphModel.getAll(graphModel), new Object[0]);
            
        }
        catch(Exception e)
        {   System.out.println("loadBorealisModel:  " + e.getMessage());
            model.setGraphModel(null);

            throw  new Exception(e.getMessage());
        }

        return  warning;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    //  Apply a deployment to the graph.
    //
    public static void  applyDeployment(BorealisModel   model,
                                        DeployDocument  deploy)
                throws  Exception
    {
        String  query;
        String  endpoint;
        String  q;
        int     i;
    //..........................................................................


        // Scan deployment node elements.
        //
        List  list = deploy.getDeploy().getChildren(XmlConstants.NODE_ELEMENT);

        for (Iterator k = list.iterator(); k.hasNext();)
        {
            NodeElement node = (NodeElement)k.next();

            endpoint = HostEndpoint.formEndpoint(node.getEndpoint(),
                                     XmlConstants.DEFAULT_BOREALIS_PORT);

            query = node.getQuery().trim();
            DebugTrace.trace(query + " @ " + endpoint);

            while (!query.equals(""))
            {   i = query.indexOf(' ');

                if (i == -1)
                {   q = query;
                    query = "";
                }
                else
                {   q = query.substring(0, i);
                    query = query.substring(i).trim();
                }

                QueryElement  e = model.getDocument().getQuery(q);

                if (e == null)
                {   
                    BoxElement box = model.getDocument().getBox(q);

                    if (box == null)
                    {   throw new Exception("No such query or box (" + q + ").");
                    }

                    applyBoxNode(model, box, endpoint);
                }
                else
                {   List<BoxElement>  b = e.getAllBoxes();

                    for (Iterator j = b.iterator(); j.hasNext();)
                    {
                        BoxElement box = (BoxElement)j.next();
                        applyBoxNode(model, box, endpoint);
                    }
                }
            }
        }

        return;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    // set up output base on box outs
    //
    public static void  applyBoxNode(BorealisModel  model,
                                     BoxElement     box,
                                     String         node)
    {
        String         stream;
        OutElement     out;
        OutputElement  output;
    //..........................................................................


        DebugTrace.trace(box.getNameValue() + " @ " + node);
        
        box.setNodeAttribute(node);
        List  l = box.getAllOuts();
 
        for (Iterator o = l.iterator(); o.hasNext();)
        {
             out    = (OutElement)o.next();
             stream = out.getStream();
             output = model.getDocument().getOutput(stream);

             if (output != null)
             {   output.setNodeAttribute(node);
                 DebugTrace.trace(stream);
             }
        }

        return;
    }
}


/////////////////////  end BorealisModelXmlHandler.java  ///////////////////////
