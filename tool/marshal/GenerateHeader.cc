#include  <fstream>

#include  "GenerateMarshal.h"
#include  "DataType.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
//  Create a header file for the marshaling code.
//
Status  GenerateMarshal::generate_header(// Relative path of the header file.
                                         string  file)
{
    ofstream  header;
    Status    status = true;
//..............................................................................


    NOTICE << "generate: " << file;
    header.open(file.c_str());

    if (header)
    {   header_top(header);

        header_publish(header);
        header_subscribe(header);

        header_bottom(header);
        header.close();
    }
    else
    {   status = "Couldn't open header file:  " + file;
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Write the top common portion of the marshaling header file.
//
void GenerateMarshal::header_top( // An open header file output stream.
                                 ofstream  &header )
{
    string   guard;
//..............................................................................


    guard = _prefix;
    uppercase( guard );
    guard += "MARSHAL_H";

    header << "#ifndef  " << guard << "\n";
    header << "#define  " << guard << "\n\n";

    header << "#include  \"MedusaClient.h\"\n";
    header << "#include  \"TupleHeader.h\"\n\n";

    header << BOX_TOP;
    header << "// Generated Martialing code for the " << _prefix << " program.\n";
    header << BOX_BOTTOM;

    header << "using namespace Borealis;\n\n";

    header << "class " << _prefix << "Marshal\n";
    header << "{\n";
    header << "  public:\n\n";
    header << "    " << _prefix << "Marshal() {};\n";
    header << "   ~" << _prefix << "Marshal() {};\n\n";

    header << "    /// Activate the front-end and subscribe to streams.\n";
    header << "    /// Returns 0 if okay; else an error occured.\n";
    header << "    ///\n";
    header << "    int32  open();\n\n\n";

    header << "    /// Connect to inputs and subscribe to streams only.\n";
    header << "    /// Similar to open() but does not load the query.\n";
    header << "    ///\n";
    header << "    void  openInputOutput();\n\n\n";

    header << "    /// Run the client event loop.\n";
    header << "    /// This does not return unless terminated or on an exception.\n";
    header << "    ///\n";
    header << "    void  runClient();\n\n\n";

    header << "    /// Terminate the client event loop.\n";
    header << "    ///\n";
    header << "    void  terminateClient();\n\n\n";

    header << "    /// Copy a string value to a fixed length array and zero fill.\n";
    header << "    ///\n";
    header << "    static void  setStringField(string  value,\n";
    header << "                                  char  field[],\n";
    header << "                                uint32  length)\n";
    header << "                          throw(AuroraException);\n\n\n";

    header << "    /// Get the timestamp for a tuple.\n";
    header << "    ///\n";
    header << "    static timeval   getTimeValue(uint8  *tuple)\n";
    header << "    {\n";
    header << "       return(*((timeval *)(tuple - HEADER_SIZE)));\n";
    header << "    }\n\n\n";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Write marshaling header file declarations for all input streams.
//
void GenerateMarshal::header_publish(// An open header file output stream.
                                     ofstream  &header )
{
    string   name;
    string   tuple;

    CatalogSchema  *schema;
    SchemaArray    *array;
    CatalogStream  *stream;

    set <string>::iterator         find;
    vector<SchemaField>::iterator  f;

    map<string, GeneratePublish>::iterator  publish;
//..............................................................................


    _tuple.clear();

    // Only Publish streams need an associated node.
    // Subcribe uses the client listener node???
    //
    for (publish  = _generate_publish.begin();
         publish != _generate_publish.end(); publish++)
    {
        if (publish->second._prefix == _prefix)
        {   name = to_string(publish->first);
            stream = get_stream(name);           // Must not be null???

            header << "  public:\n\n";
            schema = stream->get_stream_schema();
            tuple  = to_string(schema->get_schema_name());
            tuple  = _schema_mixed[tuple];

            find = _tuple.find(tuple);     // see if the schema has been generated.

            if (find == _tuple.end())
            {
                header << "    struct " << tuple << "\n";
                schema_structure( header, schema );
                _tuple.insert( tuple );                   // Note already generated.
            }

            proper_case(name);
            DEBUG  << "publish " << name << " for " << tuple;

            header << "    struct  " << name << " : public TupleHeader\n";
            header << "    {\n";
            header << "        " << tuple << "   _data;\n";
            header << "    } __attribute__((__packed__));\n\n";

            header << "    /// The sent" << name << "  method must be defined by the application.\n";
            header << "    /// It is called after send" << name << " is done and a pause.\n";
            header << "    ///\n";
            header << "    void  sent" << name << "();\n\n";

            header << "    /// Enque a " << name << " for input.\n";
            header << "    ///\n";
            header << "    void  batch" << name << "(" << name << "  *tuple";

            // For each variable sized array field, add a max parameter.
            //
            vector<SchemaField>  &field = stream->get_stream_schema()->get_schema_field();

            for (f = field.begin(); f != field.end(); f++)
            {
                array = f->get_array();

                if (array)
                {   if (array->is_variable())
                    {
                        DEBUG << "variable array name=" << f->get_name()
                            << " size=" << array->array_size();

                        header << ", int32  max_" << f->get_name();
                    }
                }
            }

            header << ");\n\n";



            header << "    /// Send enqued " << name << " inputs.\n";
            header << "    ///\n";
            header << "    void  send" << name << "(uint32  sleep);\n\n";

            header << "  private:\n\n";
            header << "    /// Connect the " << name << " input stream.\n";
            header << "    ///\n";
            header << "    void  connect" << name << "();\n\n";

            header << "    ///  Resume here.  Extend with a user callback.\n";
            header << "    void  delay" << name << "();\n\n";
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Write marshaling header file declarations for all output subscriptions.
//
void GenerateMarshal::header_subscribe(// An open header file output stream.
                                       ofstream  &header )
{
    string   stream;
    string   tuple;

    CatalogSchema  *schema;
    CatalogStream  *i;
    SchemaArray    *array;

    set<string>::iterator find;

    vector<SchemaField>::iterator  f;

    set<CatalogSubscription *>::iterator   output;
//..............................................................................


    // Declare a global output handler, for all subscriptions.
    //
    header << "  private:\n\n";
    header << "    /// Handler to dispatch tuples received.\n";
    header << "    ///\n";
    header << "    static Status outputHandler(ptr<StreamEvent> event);\n\n\n";

    for (output  = _subscribe.begin();
         output != _subscribe.end(); output++)
    {
        i = (*output)->get_subscription_stream();

        DEBUG << "Stream name is " << i->get_stream_name();
        DEBUG << "Schema is " << i->get_stream_schema()->get_schema_name();

        header << "  public:\n\n";
        schema = i->get_stream_schema();

        tuple = to_string(schema->get_schema_name());
        tuple = _schema_mixed[tuple];

        find = _tuple.find(tuple);     // see if the schema has been generated.

        if (find == _tuple.end())
        {   header << "    struct " << tuple << "\n";

            schema_structure(header, schema);
            _tuple.insert(tuple);                   // Note already generated.
        }

        stream = to_string(i->get_stream_name());
        proper_case(stream);
        DEBUG  << "subscribe " << stream << " for " << tuple;

        header << "    /// The received" << stream << " method must be defined by the application.\n";
        header << "    /// It is called after a(n) " << tuple << " is received.\n";
        header << "    ///\n";
        header << "    static void  received" << stream << "(" << tuple << "  *tuple";

        // For each variable sized array field, add a max parameter.
        //
        vector<SchemaField>  &field = schema->get_schema_field();

        for (f = field.begin(); f != field.end(); f++)
        {
            array = f->get_array();

            if (array)
            {   if (array->is_variable())
                {
                    DEBUG << "variable array name=" << f->get_name()
                        << " size=" << array->array_size();

                    header << ", int32  max_" << f->get_name();
                }
            }
        }

        header << ");\n\n";

        header << "  private:\n\n";

        header << "    /// Subscribe to the " << stream << " output stream.\n";
        header << "    ///\n";
        header << "    void  subscribe" << stream << "();\n\n";

        header << "    /// Handler to receive tuples from the " << stream << " stream.\n";
        header << "    ///\n";
        header << "    static Status  " << stream << "Handler(ptr<StreamEvent>  event);\n\n";
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  GenerateMarshal::schema_structure( // An open header file output stream.
                                        ofstream       &header,

                                        // Schema containing the fields.
                                        CatalogSchema  *schema )
{
    string   name;
    string   type;

    SchemaField    *schema_field;

    vector<SchemaField>  *field;
//..............................................................................


    header << "    {\n";

    field = &schema->_field;

    for (uint32 j = 0; j < field->size(); ++j)
    {
        schema_field = &((*field)[ j ]);

        name = to_string(schema_field->get_name());
        type = schema_field->get_type().c_type();

        if (schema_field->get_type() == DataType::STRING)
        {   name += "[ " + to_string(schema_field->get_size()) + " ]";
        }
        else if (schema_field->get_type() == DataType::ARRAY)
        {   name = "*" + name;
            type = "uint8";
        }
 
        header << "        " << type << "    " << name << ";\n";
    }

    header << "    } __attribute__((__packed__));\n\n";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Write the bottom common portion of the marshaling header file.
//
void GenerateMarshal::header_bottom( // An open header file output stream.
                                    ofstream  &header )
{
    string   name;
    map<string, GeneratePublish>::iterator  publish;
//..............................................................................


    header << "  private:\n\n";
    header << "    /// Launch the Borealis front-end.\n";
    header << "    ///\n";
    header << "    static int32  launchDiagram(string  xml);  // Path of an initial xml file.\n\n";

    header << "    /// Client connections to Borealis nodes.\n";
    header << "    ///\n";
    header << "    MedusaClient      *_client;\n\n\n";

    header << "    /// Event state for input streams.\n";
    header << "    /// These are declared with a smart pointer as fast_post_event requires it.\n";
    header << "    ///\n";

    for (publish  = _generate_publish.begin();
         publish != _generate_publish.end(); publish++)
    {
        if (publish->second._prefix == _prefix)
        {   name = to_string(publish->first);
            proper_case( name );

            header << "    ptr<StreamEvent>   _event" << name << ";\n";
        }
    }

    header << "};\n\n";

    name = _prefix;
    uppercase( name );
    name += "MARSHAL_H";

    header << "#endif                         // " << name << "\n";

    return;
}


BOREALIS_NAMESPACE_END
