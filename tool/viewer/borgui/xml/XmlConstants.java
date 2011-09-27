package borgui.xml;

/**
 * Interface which contains constants related to the Xml representation
 * of the borealis query and gui state
 */
public interface XmlConstants
{
    // This is the location where packages are installed at your site.
    // It is used by launchProgram to set up LD_LIBRARY_PATH on a remote host.
    // Alternatively is could be setup in a login shell, but that would
    // be site and user specific.  Another alternative would be to do
    // a fully static build of the target program.  This is currently
    // not part of the Borealis build process.
    //
    // A trailing slash is required.
    // See:  common.HostEndpoint.launchProgram
    //
    public final static String  BOREALIS_SOFTWARE = "/pro/borealis/software/";


    // The default machine and port where the Head is running.
    //
    public final static String  BOREALIS_HEAD = "yang";


    // Polling cycle time in milliseconds for reloading the View graph
    // and sampling statistics.  See:  visual/VisualPanel.jbInit
    //
    public final static int  VIEWER_VIEW  = 1000;
    public final static int  VIEWER_PLOT  = 1000;

    

    // The default port numbers have corresponding C declarations in:
    //     borealis/src/modules/common/common.h
    //
    public final static int  DEFAULT_BOREALIS_PORT = 15000;
    public final static int  DEFAULT_MONITOR_PORT  = 25000;
    public final static int  DEFAULT_HEAD_PORT     = 35000;
    public final static int  DEFAULT_REGION_PORT   = 45000;
    public final static int  DEFAULT_GLOBAL_PORT   = 55000;


    // namespace urls
    public final static String DEFAULT_NAMESPACEURI = new String("http://borealis");
    public final static String GUI_NAMESPACEURI     = new String("http://gui");

    // namespace aliases
    public final static String GUI_NAMESPACEALIAS   = new String("gui");

    // borealis elements
    public final static String NETWORK_ELEMENT      = new String("borealis");

    public final static String INPUT_ELEMENT        = new String("input");
    public final static String OUTPUT_ELEMENT       = new String("output");
    public final static String SCHEMA_ELEMENT       = new String("schema");
    public final static String FIELD_ELEMENT        = new String("field");


    public final static String BOX_ELEMENT          = new String("box");
    public final static String IN_ELEMENT           = new String("in");
    public final static String OUT_ELEMENT          = new String("out");
    public final static String PARAMETER_ELEMENT    = new String("parameter");
    public final static String ACCESS_ELEMENT       = new String("access");

    public final static String QUERY_ELEMENT        = new String("query");

    public final static String TABLE_ELEMENT        = new String("table");
    public final static String KEY_ELEMENT          = new String("key");
    public final static String INDEX_ELEMENT        = new String("index");

    //public final static String PARAMETER_ELEMENT_SIMPLE = new String("param");

    // general borealis element attributes
    public final static String NAME_ATTRIBUTE       = new String("name");
    public final static String UPDATE_ATTRIBUTE     = new String("update_queue");
    public final static String QUEUE_TYPE_ATTRIBUTE = new String("queue_type");
    public final static String KEY_ATTRIBUTE        = new String("key");

    //schema field type
    public final static int SCHEMA_TYPE_ID = 6;

    public final static String SCHEMA_BYTE_TYPE      = new String("byte");
    public final static String SCHEMA_SHORT_TYPE     = new String("short");
    public final static String SCHEMA_INT_TYPE       = new String("int");
    public final static String SCHEMA_LONG_TYPE      = new String("long");
    public final static String SCHEMA_SINGLE_TYPE    = new String("single");
    public final static String SCHEMA_DOUBLE_TYPE    = new String("double");
    public final static String SCHEMA_STRING_TYPE    = new String("string");
    public final static String SCHEMA_TIMESTAMP_TYPE = new String("timestamp");

    // box element attributes
    public final static String BOX_TYPE_ATTRIBUTE = new String("type");
    public final static String BOX_NODE_ATTRIBUTE = new String("node");

    // access  element attributes
    public final static String ACCESS_TABLE_ATTRIBUTE = new String("node");

    // input element attributes
    public final static String INPUT_STREAM_ATTRIBUTE = new String("stream");
    public final static String INPUT_SCHEMA_ATTRIBUTE = new String("schema");
    public final static String INPUT_NODE_ATTRIBUTE   = new String("node");

    // output element attributes
    public final static String OUTPUT_STREAM_ATTRIBUTE = new String("stream");
    public final static String OUTPUT_SCHEMA_ATTRIBUTE = new String("schema");

    // table element attributes
    public final static String TABLE_SCHEMA_ATTRIBUTE = new String("schema");
    public final static String TABLE_STREAM_ATTRIBUTE = new String("stream");
    public final static String TABLE_NODE_ATTRIBUTE   = new String("node");

    // key element attributes
    public final static String KEY_FIELD_ATTRIBUTE = new String("field");

    // index element attributes
    public final static String INDEX_FIELD_ATTRIBUTE = new String("index");

    // schema element attributes
    
    // field element attributes
    public final static String FIELD_TYPE_ATTRIBUTE = new String("type");
    public final static String FIELD_SIZE_ATTRIBUTE = new String("size");

    // parameter element attributes
    public final static String PARAMETER_VALUE_ATTRIBUTE = new String("value");

    // in element attributes
    public final static String IN_STREAM_ATTRIBUTE = new String("stream");

    // out element attributes
    public final static String OUT_STREAM_ATTRIBUTE = new String("stream");

    // box attribute values
    public final static String BOX_TYPE_FILTER     = new String("filter");
    public final static String BOX_TYPE_MAP        = new String("map");
    public final static String BOX_TYPE_AGGREGATE  = new String("aggregate");
    public final static String BOX_TYPE_JOIN       = new String("join");
    public final static String BOX_TYPE_UNION      = new String("union");
    public final static String BOX_TYPE_SELECT     = new String("select");
    public final static String BOX_TYPE_INSERT     = new String("insert");
    public final static String BOX_TYPE_UPDATE     = new String("update");
    public final static String BOX_TYPE_WAITFOR    = new String("waitfor");
    public final static String BOX_TYPE_RANDOMDROP = new String("random_drop");
    public final static String BOX_TYPE_WINDOWDROP = new String("window_drop");

    /***********
    // gui elements
    public final static String GUI_ELEMENT      = new String("gui");
    public final static String SUPERBOX_ELEMENT = new String("superbox");
    public final static String BOX_ELEMENT      = new String("box");
    public final static String INPUT_ELEMENT    = new String("input");
    public final static String OUTPUT_ELEMENT   = new String("output");
    public final static String PORT_ELEMENT     = new String("port");
    public final static String ARC_ELEMENT      = new String("arc");

    // gui attributes general
    public final static String TYPE_GUIATTR   = new String("type");
    public final static String ID_GUIATTR     = new String("id");
    public final static String X_GUIATTR      = new String("x");
    public final static String Y_GUIATTR      = new String("y");
    public final static String WIDTH_GUIATTR  = new String("width");
    public final static String HEIGHT_GUIATTR = new String("height");
    public final static String COLOR1_GUIATTR = new String("color1");
    public final static String COLOR2_GUIATTR = new String("color2");
    ***********/

    //deploy elements
    public final static String DEPLOY_ELEMENT      = new String("deploy");
    public final static String REPLICA_SET_ELEMENT = new String("replica_set");
    public final static String NODE_ELEMENT        = new String("node");
    public final static String REGION_ELEMENT      = new String("region");
    public final static String PUBLISH_ELEMENT     = new String("publish");
    public final static String SUBSCRIBE_ELEMENT   = new String("subscribe");
    public final static String UNSUBSCRIBE_ELEMENT = new String("unsubscribe");
    public final static String CLIENT_ELEMENT      = new String("client");
    public final static String MOVE_ELEMENT        = new String("move");

    // replica_set attribute values
    public final static String REPLICA_QUERY_ATTRIBUTE = new String("filter");

    // node attribute values
    public final static String NODE_ENDPOINT_ATTRIBUTE = new String("endpoint");
    public final static String NODE_QUERY_ATTRIBUTE    = new String("query");

    // region attribute values
    public final static String REGION_ENDPOINT_ATTRIBUTE = new String("endpoint");
    public final static String REGION_NODE_ATTRIBUTE     = new String("node");

    // publish attribute values
    public final static String PUBLISH_STREAM_ATTRIBUTE = new String("stream");
    public final static String PUBLISH_NODE_ATTRIBUTE   = new String("node");

    // subscribe attribute values
    public final static String SUBSCRIBE_STREAM_ATTRIBUTE   = new String("stream");
    public final static String SUBSCRIBE_ENDPOINT_ATTRIBUTE = new String("endpoint");
    public final static String SUBSCRIBE_GAP_ATTRIBUTE      = new String("gap");

    // unsubscribe attribute values
    public final static String UNSUBSCRIBE_STREAM_ATTRIBUTE = new String("stream");

    // move attribute values
    public final static String MOVE_BOX_ATTRIBUTE   = new String("box");
    public final static String MOVE_TABLE_ATTRIBUTE = new String("table");
    public final static String MOVE_NODE_ATTRIBUTE  = new String("node");
}

////////////////////////  end XmlConstants.java  ///////////////////////////////
