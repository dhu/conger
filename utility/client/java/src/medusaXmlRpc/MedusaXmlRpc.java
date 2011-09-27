package medusaXmlRpc;

import org.apache.xmlrpc.*;
import java.util.*;
import java.net.*;


/*
 * Java methods to communicate with Medusa using XML-RPC
 * @author kltang
 * Created: 14 Feb 2003
 * Last Modified: 14 Mar 2003
 */

public class MedusaXmlRpc{

    public MedusaXmlRpc(){
    }

    /* Post an event.  
     * @param serverURL a string representing the URL of the medusa node to post event to.
     *                  The format is "http://beacon.lcs.mit.edu:10001/RPC2"
     * @param method the remote method to invoke
     * @param methodParams the arguments to the method
     */
    public static Object invoke(String serverURL, String method, Vector methodParams) throws java.io.IOException {
    Object result = null;
    try{
            XmlRpcClient server = new XmlRpcClient(serverURL);
            result = server.execute(method, methodParams);
        }catch(java.net.MalformedURLException mue){
            System.out.println("Malformed URL");
            mue.printStackTrace();
            System.exit(1);
        }catch(XmlRpcException xre){
            System.out.println("XmlRpc Exception");
            xre.printStackTrace();
        }
    //}catch(java.io.IOException ioe){
        //  System.out.println("IOException");
        //  ioe.printStackTrace();
    //}
        return result;
    }


    /////////////////// CREATING AND POSTING EVENTS //////////////////////////


    /* Post an event.  
     * @param serverURL a string representing the URL of the medusa node to post event to.
     *                  The format is "http://beacon.lcs.mit.edu:10001/RPC2"
     * @param streamEventVector a vector containing event info in the format that medusa expects.
     */
    public static void postEvent(String serverURL, Vector streamEventVector){
    try{
        XmlRpcClient server = new XmlRpcClient(serverURL);
        server.execute("QueryProcessor.post_event", streamEventVector);
    }catch(java.net.MalformedURLException mue){
        System.out.println("Malformed URL");
        mue.printStackTrace();
        System.exit(1);
    }catch(XmlRpcException xre){
        System.out.println("XmlRpc Exception");
        xre.printStackTrace();
        System.exit(1);
    }catch(java.io.IOException ioe){
        System.out.println("IOException");
        ioe.printStackTrace();
        System.exit(1);
    }
    }

    /* Post an event.  
     * @param hostname the hostname of the medusa node to post event to.
     *                  The format is "beacon.lcs.mit.edu"
     * @param port the port that the XMLRPC server is listening on
     * @param streamEventVector a vector in the format that medusa expects.
     */
    public static void postEvent(String hostname, int port, Vector streamEventVector){
    try{
        XmlRpcClient server = new XmlRpcClient(hostname, port);
        server.execute("QueryProcessor.post_event", streamEventVector);
    }catch(java.net.MalformedURLException mue){
        System.out.println("Malformed URL");
        mue.printStackTrace();
        System.exit(1);
    }catch(XmlRpcException xre){
        System.out.println("XmlRpc Exception");
        xre.printStackTrace();
        System.exit(1);
    }catch(java.io.IOException ioe){
        System.out.println("IOException");
        ioe.printStackTrace();
        System.exit(1);
    }
    }

    /* Post an event.  
     * @param serverURL the URL of the medusa node to post event to.
     * @param streamEventVector a vector in the format that medusa expects.
     */
    public static void postEvent(java.net.URL serverURL, Vector streamEventVector){
    XmlRpcClient server = new XmlRpcClient(serverURL);
    try{
        server.execute("QueryProcessor.post_event", streamEventVector);
    }catch(XmlRpcException xre){
        System.out.println("XmlRpc Exception");
        xre.printStackTrace();
        System.exit(1);
    }catch(java.io.IOException ioe){
        System.out.println("IOException");
        ioe.printStackTrace();
        System.exit(1);
    }
    }

    /* Makes a vector of the format that the xmlrpc implementation for medusa expects as a parameter for a stream event
     * @param host the participant.  for example "nms.lcs.mit.edu" for medusa://nms.lcs.mit.edu/foobar
     * @param path the path of the stream.  for example "foobar" for medusa://nms.lcs.mit.edu/foobar
     * @param events a vector of events to be put on the stream
     * @param deletedEvents a vector of events to be deleted
     * @param streamClosed indicate whether the stream should be closed
     * @param injectEvent indicate whether event should be injected to the stream
     */
    public static StreamEventVector makeStreamEventVector(String host, String path, Vector events, Vector deletedEvents, Boolean streamClosed, Boolean injectEvent){

    StreamEventVector sev = new StreamEventVector(host, path, events, deletedEvents, streamClosed, injectEvent);

    return sev;

    }


    /* Makes an event given information about what should be posted
     * Overloaded method.  medusaID and timeStamp are taken in as Strings, not vectors
     * @param columns a Vector of columns to be posted
     * @param medusaID the medusaID of the node posting the event
     * @param timeStamp time of event posting
     * @param rowID the id of the row
     */
    public static MedusaEvent makeMedusaEvent(Vector columns, String medusaID, Long timeStamp, Integer rowID){

        Vector medusaIDVector = new Vector();
        Vector timeStampVector = new Vector();

    String timeStampString = timeStamp.toString();
    String rowIDstring = rowID.toString();

    medusaIDVector.add(medusaID);
    timeStampVector.add(timeStampString);

    MedusaEvent event = new MedusaEvent(columns, medusaIDVector, timeStampVector, rowIDstring);
    return event;
    }

    /* Makes an event given information about what should be posted.  
     * Overloaded method - medusaID and timeStamp are taken in as vectors
     * @param columns a Vector of columns to be posted
     * @param medusaIDVector a Vector containing the medusaID of the node posting the event
     * @param timeStampVector a Vector containing time of event posting
     * @param rowID the id of the row
     */
    public static MedusaEvent makeMedusaEvent(Vector columns, Vector medusaIDVector, Vector timeStampVector, Integer rowID){

    String rowIDstring = rowID.toString();
    MedusaEvent event = new MedusaEvent(columns, medusaIDVector, timeStampVector, rowIDstring);

    return event;
    }


    ////////////////////  CREATING SCHEMAS //////////////////////////////////////


    /* Create a schema.
     * @param serverURL a string representing the URL of the medusa node to post event to.
     *                  The format is "http://beacon.lcs.mit.edu:10000/RPC2"
     * @param schemaVector a vector containg schema event info in the format medusa expects
     *
     */
    public static void createSchema(String serverURL, SchemaVector schemaVector){
    try{
        XmlRpcClient server = new XmlRpcClient(serverURL);
        server.execute("Brain.create_schema", schemaVector);
    }catch(java.net.MalformedURLException mue){
        System.out.println("Malformed URL");
        mue.printStackTrace();
        System.exit(1);
    }catch(XmlRpcException xre){
        System.out.println("XmlRpc Exception");
        xre.printStackTrace();
            // Do not quit when encoutering XML_RPC excetptions, just print the error
        //System.exit(1);
    }catch(java.io.IOException ioe){
        System.out.println("IOException");
        ioe.printStackTrace();
        System.exit(1);
    }

    }

    /* Makes a vector of the format that the xmlrpc implementation for medusa expects as a parameter for creating a schema
     * @param host the host [nms.lcs.mit.edu]
     * @param schemaName the name of the schema
     * @param schemaFieldsVector a vector containing the fields
     */
    public static SchemaVector makeSchemaVector(String host, String schemaName, Vector schemaFieldsVector){
    SchemaVector sv = new SchemaVector(host, schemaName, schemaFieldsVector);

    return sv;
    }


    /* Makes a vector of the format that the xmlrpc implementation for medusa expects as a parameter for creating a schema.
     * This is an overloaded version to facilitate easy creation of schemas.  The key parameters are vectorOfFieldTypes and vectorOfFieldNames.  The first String in vectorOfFieldTypes is the type of a field corresponding to the first String in vectorOfFieldNames which is the name of the field.  Because of this relationship, the two vectors obviously must be of the same length.  So an example of the two vectors would be [string string] [tag_id location].  
     * @param host the host [nms.lcs.mit.edu]
     * @param schemaName the name of the schema
     * @param vectorOfFieldTypes a vector containing strings of field types - must be in same order as vectorOfFieldNames
     * @param vectorOfFieldNames a vector containing strings of field names - must be in same order as vectorOfFieldTypes
     */
    public static SchemaVector makeSchemaVector(String host, String schemaName, Vector vectorOfFieldTypes, Vector vectorOfFieldNames){

    Vector schemaFieldsVector = new Vector();

    Iterator ftIter = vectorOfFieldTypes.iterator();
    Iterator fnIter = vectorOfFieldNames.iterator();

    while (ftIter.hasNext()){
        
        String fieldType = (String) ftIter.next();
        String fieldName = (String) fnIter.next();

        Vector schemaField = makeSchemaField(fieldType, fieldName);

        schemaFieldsVector.add(schemaField);

    }

    SchemaVector sv = new SchemaVector(host, schemaName, schemaFieldsVector);

    return sv;

    }

 
    /* Makes a schemafield.  To be put in a schemaFieldsVector.
     * 
     * @param fieldType the type of the field [int, string, bool, etc...]
     * @param fieldName the name of the field [tag_ids, owner_last_name, etc...]
     */
    public static Vector makeSchemaField(String fieldType, String fieldName){
    Vector schemaField = new Vector();

    schemaField.add(fieldType);
    schemaField.add(fieldName);

    return schemaField;
    }




    /////////////////////// CREATING STREAMS ////////////////////////////


    /* Creates a stream
     * @param serverURL a string representing the URL of the medusa node to register stream with.
     *                  The format is "http://beacon.lcs.mit.edu:10001/RPC2"
     * @param streamVector a vector containing the stream information
     */
    public static void createStream(String serverURL, StreamVector streamVector){
    try{
        XmlRpcClient server = new XmlRpcClient(serverURL);
        server.execute("Brain.create_stream", streamVector);
    }catch(java.net.MalformedURLException mue){
        System.out.println("Malformed URL");
        mue.printStackTrace();
        System.exit(1);
    }catch(XmlRpcException xre){
        System.out.println("XmlRpc Exception");
        xre.printStackTrace();
            // Don't exit, just print the error. It might be just a duplicate declaration
        //System.exit(1);
    }catch(java.io.IOException ioe){
        System.out.println("IOException");
        ioe.printStackTrace();
        System.exit(1);
    }
    }


    /* Makes a streamVector
     * @param host the participant.  for example "nms.lcs.mit.edu" for medusa://nms.lcs.mit.edu/foobar
     * @param streamName the name of the stream.  for example "foobar"
     * @param schemaInfoVector vector containing info about the schema. See schemaFormat
     * @param history the number of events to keep in the history.  usually 100
     * @param partitionVector 
     */
    public static StreamVector makeStreamVector(String host, String streamName, Vector schemaInfoVector, Integer history, Vector partitionVector){
    
    StreamVector sv = new StreamVector(host, streamName, schemaInfoVector, history, partitionVector);

    return sv;
    
    }

    /* Makes a streamVector
     * @param host the participant.  for example "nms.lcs.mit.edu" for medusa://nms.lcs.mit.edu/foobar
     * @param streamName the name of the stream.  for example "foobar"
     * @param schemaVector vector containing the schema. See makeSchemaVector
     * @param history the number of events to keep in the history.  usually 100
     * @param partitionVector 
     */
    public static StreamVector makeStreamVector(String host, String streamName, SchemaVector schemaVector, Integer history, Vector partitionVector){

    Vector schemaInfoVector = schemaVector.getSchemaInfoVector();

    StreamVector sv = new StreamVector(host, streamName, schemaInfoVector, history, partitionVector);

    return sv;

    }


    ///////////// SUBSCRIBING TO A STREAM ////////////////////////
    
    /* Subscribe to a stream
     *
     * @param serverURL a string representing the URL of the medusa node to communicate with.
     *                  The format is "http://beacon.lcs.mit.edu:10001/RPC2"
     *
     * @param subscriptionVector a vector containing the subscription info in the format medusa expects
     */
    public static void subscribe(String serverURL, Vector subscriptionVector){
    try {
        XmlRpcClient server = new XmlRpcClient(serverURL);
        //server.execute("QueryProcessor.subscribe", subscriptionVector);
            server.execute("Brain.subscribe", subscriptionVector);
    }catch(java.net.MalformedURLException mue){
        System.out.println("Malformed URL");
        mue.printStackTrace();
        System.exit(1);
    }catch(XmlRpcException xre){
        System.out.println("XmlRpc Exception");
        xre.printStackTrace();
        System.exit(1);
    }catch(java.io.IOException ioe){
        System.out.println("IOException");
        ioe.printStackTrace();
        System.exit(1);
    }
    }


    /* Makes a vector of the format that the xmlrpc implementation for medusa expects as a parameter for a subscription
     * @param host the host [nms.lcs.mit.edu  from medusa://nms.lcs.mit.edu/foobar]
     * @param path the path [foobar   from medusa://nms.lcs.mit.edu/foobar]
     * @param transport [either TCP or XML]
     * @param destination the node to receive events.  ip:port [192.168.0.1:10000]
     * @param objectName the name of the object that the node sending the stream can call.  objectName.handleevent
     * @param addOrRemove indicate whether to be added (1) or removed (0) from subscription
     */
    public static SubscriptionVector makeSubscriptionVector(String host, String path, String transport, String destination, String objectName, Integer addOrRemove){
    
    SubscriptionVector subVec = new SubscriptionVector(host, path, transport, destination, objectName, addOrRemove);
        
    return subVec;

    }


    //////////////SUBMITTING A QUERY///////////////////////////
    /* Submit a query
     * @param serverURL a string representing the URL of the medusa node to communicate with
     *                  The format is "http://beacon.lcs.mit.edu:10001/RPC2"
     *
     * @param query a Vector containing one element - a string in XML format representing the query
     */
    public static void createQuery(String serverURL, Vector query){
    try {
        XmlRpcClient server = new XmlRpcClient(serverURL);
        server.execute("Brain.create_query_xml", query);
    }catch(java.net.MalformedURLException mue){
        System.out.println("Malformed URL");
        mue.printStackTrace();
        System.exit(1);
    }catch(XmlRpcException xre){
        System.out.println("XmlRpc Exception");
        xre.printStackTrace();
            // Don't exit, just print the error. It might be just a duplicate declaration
        //System.exit(1);
    }catch(java.io.IOException ioe){
        System.out.println("IOException");
        ioe.printStackTrace();
        System.exit(1);
    }   
    }

    /* Set the status of a query
     * 0 = NONE
     * 1 = CLOSED
     * 2 = SETUP
     * 3 = RUNNING
     * 4 = STOPPED
     * 5 = DELETED
     *
     * @param serverURL the url of the server
     * @param nameVector the name of the query
     * @param queryStatusVector the status of the query
     */
    public static void setQueryStatus(String serverURL, Vector nameVector, Vector queryStatusVector){
    
    Vector newVector = new Vector();

    newVector.add(nameVector);
    newVector.add(queryStatusVector);

    try {
        XmlRpcClient server = new XmlRpcClient(serverURL);
        server.execute("Brain.set_query_status", newVector);
    }catch(java.net.MalformedURLException mue){
        System.out.println("Malformed URL");
        mue.printStackTrace();
        System.exit(1);
    }catch(XmlRpcException xre){
        System.out.println("XmlRpc Exception");
        xre.printStackTrace();
            // Don't exit, just print the error. It might be ok
        //System.exit(1);
    }catch(java.io.IOException ioe){
        System.out.println("IOException");
        ioe.printStackTrace();
        System.exit(1);
    }       

    }

    /* Set the status of a query
     * 0 = NONE
     * 1 = CLOSED
     * 2 = SETUP
     * 3 = RUNNING
     * 4 = STOPPED
     * 5 = DELETED
     *
     * @param qs the status of the query
     */
    public static Vector makeQueryStatusVector(Integer qs){
    Vector queryStatusVector = new Vector();
    //Vector queryStatusSubVector = new Vector();
    
    //queryStatusSubVector.add(qs);
    //queryStatusVector.add(queryStatusSubVector);
    queryStatusVector.add(qs);

    return queryStatusVector;

    }


    /* Create a name Vector 
     *
     * @param host  the host [nms.lcs.mit.edu]
     * @param path  the path [rfidquery/out]
     */
    public static Vector makeNameVector(String host, String path){
    Vector nameVector = new Vector();
    //Vector nameSubVector = new Vector();

    //nameSubVector.add(host);
    //nameSubVector.add(path);

    //nameVector.add(nameSubVector);

    nameVector.add(host);
    nameVector.add(path);   

    return nameVector;
    }

}
