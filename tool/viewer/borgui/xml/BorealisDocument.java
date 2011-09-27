package borgui.xml;

import org.jdom.*;
import java.util.*;
import borgui.common.*;

public class BorealisDocument extends Document
{
    public BorealisDocument()
    {
        super();
        NetworkElement  root = new NetworkElement();
        this.setNetwork(root);
    }

    
    public BorealisDocument(Document doc)
    {
        super();
        
        XmlHandler.jdomDocToBorealisDoc(doc, this);
    }

    
    public void setDocument(BorealisDocument doc)
    {
        NetworkElement root = doc.getNetwork();
        doc.removeContent(root);
        setNetwork(root);
    }

    
    public NetworkElement getNetwork()
    {
        return (NetworkElement) this.getRootElement();
    }

    
    public boolean setNetwork(NetworkElement network)
    {
        this.setRootElement(network);
        return true;
    }

    
    public List getAllSchemas()
    {
        return this.getNetwork().getAllSchemas();
    }
    
    
    public Vector<String>  getAllSchemaNames()
    {
        Vector<String>  names = new Vector<String>();
        List schemas = this.getAllSchemas();

        for (Iterator i = schemas.iterator(); i.hasNext();)
        {
            SchemaElement schema = (SchemaElement)i.next();
            names.add(schema.getNameValue());
        }

        return names;
    }
    
    
    public SchemaElement getSchema(String name)
    {
        return this.getNetwork().getSchema(name);
    }

    
    public boolean addSchema(SchemaElement schema)
    {
        return this.getNetwork().addSchema(schema);
    }

    
    public boolean removeSchema(String name)
    {
        return this.getNetwork().removeSchema(name);
    }
    
    
    public List getAllInputs()
    {
        return this.getNetwork().getAllInputs();
    }

    
    public Vector<String>  getAllInputNames()
    {
        Vector<String>  names = new Vector<String>();
        List inputs = this.getAllInputs();

        for (Iterator i = inputs.iterator(); i.hasNext();)
        {
            InputElement input = (InputElement)i.next();
            names.add(input.getStream());
        }

        return names;
    }

    
    public InputElement getInput(String streamName)
    {
        return this.getNetwork().getInput(streamName);
    }

    
    public boolean addInput(InputElement input)
    {
        return this.getNetwork().addInput(input);
    }

    
    public boolean removeInput(String streamName)
    {
        return this.getNetwork().removeInput(streamName);
    }
    
    
    public List getAllOutputs()
    {
        return this.getNetwork().getAllOutputs();
    }

    
    public Vector<String>  getAllOutputNames()
    {
        Vector<String>  names = new Vector<String>();
        List outputs = this.getAllOutputs();

        for (Iterator i = outputs.iterator(); i.hasNext();)
        {
            OutputElement output = (OutputElement)i.next();
            names.add(output.getStream());
        }

        return names;
    }
    
    
    public OutputElement getOutput(String streamName)
    {
        return this.getNetwork().getOutput(streamName);
    }

    
    public boolean addOutput(OutputElement output)
    {
        return this.getNetwork().addOutput(output);
    }

    
    public boolean removeOutput(String streamName)
    {
        return this.getNetwork().removeOutput(streamName);
    }

    
    public boolean removeOutElement(BoxElement boxElement, String streamName)
    {
        return this.getNetwork().removeOutElement(boxElement, streamName);
    }
    
    
    public List getAllQueries()
    {
        return this.getNetwork().getAllQueries();
    }

    
    public Vector<String>  getAllQueryNames()
    {
        Vector<String>  names = new Vector<String>();
        List queries = this.getAllQueries();

        for (Iterator i = queries.iterator(); i.hasNext();)
        {
            QueryElement query = (QueryElement)i.next();
            names.add(query.getNameValue());
        }

        return names;
    }

    
    public QueryElement getQuery(String name)
    {
        return this.getNetwork().getQuery(name);
    }

    
    public boolean addQuery(QueryElement query)
    {
        return this.getNetwork().addQuery(query);
        
    }

    
    public boolean removeQuery(String name)
    {
        return this.getNetwork().removeQuery(name);
    }

    
    public List<BoxElement>  getAllBoxes()
    {
        List<BoxElement>   boxList = new Vector<BoxElement>();

        // Go through all queries and get boxes from each.
        //
        for (Iterator i = this.getAllQueries().iterator(); i.hasNext();)
        {
            QueryElement  queryElement = (QueryElement)i.next();
            boxList.addAll(queryElement.getAllBoxes());
        }

        // Also add any boxes not wrapped in a query.
        //
        for (Iterator i = this.getNetwork().getAllBoxes().iterator();
                      i.hasNext();)
        {
            boxList.add((BoxElement)i.next());
        }

        return boxList;
    }
    
    
    public Vector<String>  getAllBoxNames()
    {
        Vector<String>  names = new Vector<String>();
        List boxes = this.getAllBoxes();

        for (Iterator i = boxes.iterator(); i.hasNext();)
        {
            BoxElement box = (BoxElement)i.next();
            names.add(box.getNameValue());
        }

        return names;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    //  Find distinct ports in a graph (empty initial set).
    //
    public Vector<String>  getAllDistinctIpPortsString()
    {
    //..........................................................................


        return  getAllDistinctIpPortsString(new HashSet<String>());
    }


 
    ////////////////////////////////////////////////////////////////////////////
    //
    //  Find distinct ports in a graph including an initial set.
    //
    public Vector<String>  getAllDistinctIpPortsString(HashSet<String>  ipPorts)
    {
        String           node;
        Vector<String>   ipPortsVector = new Vector<String>();
    //..........................................................................


        List boxes = getAllBoxes();

        for (Iterator i = boxes.iterator(); i.hasNext();)
        {
            BoxElement box = (BoxElement)i.next();
            node = box.getNodeAttribute();
            DebugTrace.trace("box " + box.getNameValue() + " @ " + node);

            ipPorts.add(HostEndpoint.formEndpoint(node,
                                     XmlConstants.DEFAULT_BOREALIS_PORT));
        }
        
        List tables = getAllTables();

        for (Iterator j = tables.iterator(); j.hasNext();)
        {
            TableElement table = (TableElement)j.next();
            node = table.getNodeAttribute();
            DebugTrace.trace("table " + table.getNameValue() + " @ " + node);

            ipPorts.add(HostEndpoint.formEndpoint(node,
                                     XmlConstants.DEFAULT_BOREALIS_PORT));
        }

        List inputs = getAllInputs();

        for (Iterator k = inputs.iterator(); k.hasNext();)
        {
            InputElement input = (InputElement)k.next();

            // The input node attribute is only present on intermediate streams.
            // It is the sending node.
            //
            node = input.getNodeAttribute();

            if (node != null)
            {   ipPorts.add(HostEndpoint.formEndpoint(node,
                                         XmlConstants.DEFAULT_BOREALIS_PORT));

                DebugTrace.trace("input " + input.getStream() + " @ " + node);
            }
        }

        // Convert the unique names to a vector.
        //
        for (Iterator l = ipPorts.iterator(); l.hasNext();)
        {
            node = (String)l.next();
            ipPortsVector.add(node);
        }

        return  ipPortsVector;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    public void clearAllNodesAttribute()
    {
        List boxes = getAllBoxes();
    //..........................................................................


        for (Iterator i = boxes.iterator(); i.hasNext();)
        {
            BoxElement box = (BoxElement)i.next();
            box.setNodeAttribute("");
        }
        
        List tables = getAllTables();

        for (Iterator j = tables.iterator(); j.hasNext();)
        {
            TableElement table = (TableElement)j.next();
            table.setNodeAttribute("");
        }
        
        List inputs = getAllInputs();

        for (Iterator k = inputs.iterator(); k.hasNext();)
        {
            InputElement input = (InputElement)k.next();
            input.setNodeAttribute("");
        }
    }

 
    ////////////////////////////////////////////////////////////////////////////
    //
    public BoxElement getBox(String name)
    {
        BoxElement box = null;
    //..........................................................................


        // Scan any boxes not wrapped in a query.
        //
        for (Iterator i = this.getNetwork().getAllBoxes().iterator();
                      i.hasNext()  && box == null;)
        {
            box = (BoxElement)i.next();
            //DebugTrace.trace(box.getNameValue() + " name=" + name);

            if (!box.getNameValue().equals(name))
            {   box = null;
            }
        }

        // go through all queries and get boxes from each.
        //
        if (box == null)
        {   for(Iterator i = this.getAllQueries().iterator();
                i.hasNext() && box == null;)
            {
                QueryElement queryElem = (QueryElement) i.next();
                box = queryElem.getBox(name);
            }
        }
        
        return box;
    }
    
    
    public boolean addBox(BoxElement box, QueryElement query)
    {
        return query.addBox(box);
    }

    
    public boolean removeBox(String name)
    {
        BoxElement boxElem = this.getBox(name);
        QueryElement queryElem = (QueryElement) boxElem.getParent();
        return queryElem.removeBox(name);
    }
    
    
    public List<TableElement>  getAllTables()
    {
        List<TableElement>  tableList = new Vector<TableElement>();

        // all queries
        List queryList = this.getAllQueries();

        // go through all queries and get boxes from each
        for (Iterator i = queryList.iterator(); i.hasNext();)
        {
            QueryElement queryElem = (QueryElement) i.next();
            tableList.addAll(queryElem.getAllTables());
        }

        return tableList;
    }
    
    
    public Vector<String>  getAllTableNames()
    {
        Vector<String>  names = new Vector<String>();
        List tables = this.getAllTables();

        for (Iterator i = tables.iterator(); i.hasNext();)
        {
            TableElement table = (TableElement)i.next();
            names.add(table.getNameValue());
        }

        return names;
    }


    public TableElement getTable(String name)
    {
        TableElement table = null;
        List queryList = this.getAllQueries();

        // go through all queries and get boxes from each
        for (Iterator i = queryList.iterator(); i.hasNext() && table == null;)
        {
            QueryElement queryElem = (QueryElement) i.next();
            table = queryElem.getTable(name);
        }
        
        return table;
    }

    
    public boolean addTable(TableElement table, QueryElement query)
    {
        return query.addTable(table);
    }

    
    public boolean removeTable(String name)
    {
        TableElement tableElem = this.getTable(name);
        QueryElement queryElem = (QueryElement) tableElem.getParent();
        return queryElem.removeTable(name);
    }

    
    // Change Element
    //
    public void changeSchemaName(String oldValue, String newValue)
    {
        List inputList = this.getAllInputs();
        List outputList = this.getAllOutputs();
        List tableList = this.getAllTables();

        if (inputList != null)
        {
            for (Iterator i = inputList.iterator(); i.hasNext();)
            {
                InputElement inputElem = (InputElement) i.next();

                if(inputElem.getSchema().equals(oldValue))
                {
                    inputElem.setSchema(newValue);
                }
            }
        }

        if (outputList != null)
        {
            for (Iterator j = outputList.iterator(); j.hasNext();)
            {
                OutputElement outputElem = (OutputElement) j.next();

                if (outputElem.getSchema().equals(oldValue))
                {
                    outputElem.setSchema(newValue);
                }
            }
        }

        if (tableList != null)
        {
            for (Iterator k = tableList.iterator(); k.hasNext();)
            {
                TableElement tableElem = (TableElement) k.next();

                if (tableElem.getSchema().equals(oldValue))
                {
                    tableElem.setSchema(newValue);
                }
            }
        }
    }
}


//////////////////////  end BorealisDocument.java  /////////////////////////////
