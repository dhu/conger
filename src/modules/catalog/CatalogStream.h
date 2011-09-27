#ifndef  CATALOGSTREAM_H
#define  CATALOGSTREAM_H

#include  "common.h"
#include  "CatalogSchema.h"
#include  "BoxPort.h"
#include  "CatalogSubscription.h"

#include "CatalogSerialize.h"

BOREALIS_NAMESPACE_BEGIN

/// A flow of tuples, all with the same type.  A stream is connected
/// to zero or more TupleQueues and zero or more Boxes.  Whenever a
/// tuple is written to the stream, it is multicasted to each of these
/// TupleQueues and Boxes.
///
/// If the stream source is disconnected the _port will be 0 and the _box
/// pointer is NULL.
///
class CatalogStream
{
  public:
    /// A collection of streams; either inputs or outputs or box ins or outs.
    ///
    typedef  vector<CatalogStream *>   StreamStar;

    CatalogStream():           // Stream content;
        _schema(0),
        _remote_source(0)
    {}


    void shallow_copy(const CatalogStream &stream);


    /// True   - This is an Input stream.
    /// False  - Intermediate (or Output) stream (source is a box output).
    /// The old check was to check:
    /// if (QBoxOutputPort source = input.getSourcePort()) == NULL
    ///     Input stream
    /// else box
    /// QBoxes fill in the schemas on intermediate streams.
    ///  so use BoxPort source = CatalogStream::_source is not used???
    ///
    Boolean  is_input_stream() const
    {   return  _source.get_port_box() == NULL;
    }


    Boolean  is_output_stream() const
    {   
        // yna -- this doesn't handle splits, one of which may be an output.
        //return( _sink.empty());
        return  _sink.empty() || _remote_sink.size() > 0;
    }


    Boolean  is_injected_stream() const
    {   return  is_input_stream() && ( _remote_source == NULL );
    }


    Boolean is_subscribed_stream()
    {   return  _remote_source != NULL;
    }


    BoxPort  *get_stream_source()
    {    return  &_source;
    }


    /// Name of a stream as a string.
    ///
    string  get_stream_name_string() const
    {   return  to_string(_name);
    }


    Name get_stream_name() const 
    {
        return  _name;
    }


    /// Set name of a stream.
    ///
    void  set_stream_name(Name   name)
    {   _name = name;
    }


    /// Name of an input stream's schema as a string; empty if intermediate.
    ///
    string  get_schema_name() const
    {   if ( _schema )
        {   return  _schema->get_schema_name();
        }
        else
        {   return  string();
        }
    }


    CatalogSchema   *get_stream_schema()  const
    {   return  _schema;
    }


    void  set_stream_schema( CatalogSchema   *schema )
    {   _schema = schema;
    }


    void  set_stream_source( CatalogBox  *box,
                               uint16   port )
    {
        _source.set_box_port( box, port );

        if (box)
        {   _remote_source = NULL;
        }
    }


    CatalogBox *get_stream_source_box();


    void set_stream_source_box(CatalogBox *box);


    /// Ports receiving data from a stream.
    ///
    vector<BoxPort>  *get_sink_port()
    {   return(&_sink);
    }


    vector<CatalogBox*> get_stream_sink_boxes();


    /// Get box port at a certain index.
    ///
    BoxPort  *get_sink_box_port(uint16 index)
    {   return(&_sink[index]);
    }


    /// Find a sink connected to a box and port.
    ///
    BoxPort  *find_sink_box_port(CatalogBox  *box, uint16  port);


    void  add_sink_port(BoxPort  port)
    {   _sink.push_back(port);
    }


    CatalogSchema  *get_schema() const
    {   return  _schema;
    }


    void set_schema(CatalogSchema  *schema)
    {   _schema = schema;
    }


    // Find the sink connected to the from box at port
    //void reset_sink(CatalogBox  *from,
    //                CatalogBox  *to,
    //                uint16       port);


    void subscribe_sink(   CatalogSubscription  *remote_sink );
    void unsubscribe_sink( CatalogSubscription  *remote_sink );
    void subscribe_source( CatalogSubscription  *remote_source );


    void unsubscribe_source()
    {   _remote_source = NULL;
    }


    CatalogSubscription* get_source_subscription()
    {    return  _remote_source;
    }


    vector<CatalogSubscription*>  *get_subscribers()
    {   return  &(_remote_sink);
    }


    Boolean  match_stream_schema( // Stream to match up.
                                CatalogStream  *Match );


    string   as_string() const;


    string   form_input_xml()
    {
        return  "<input  stream=\"" + Util::full_name(get_stream_name())
                  + "\"  schema=\"" + Util::full_name(get_schema_name())
                  + "\" />\n";
    }


    string   form_output_xml()
    {
        string  xml;

        xml  = "<output  stream=\"" + Util::full_name(get_stream_name());

        if (_schema)
        {   xml += "\"  schema=\"" + Util::full_name(get_schema_name());
        }

        xml += "\" />\n";

        return  xml;
    }


    NMSTL_SERIALIZABLE(CatalogStream);


    void freeze(::NMSTL::OSerial &out) const
    {
        bool shallow_freeze = false;

        out && out.begin_container(*this)
            && (out << shallow_freeze << _name)
            && freeze_ptr_impl(out, _schema)
            && (out << _source << _sink)
            && out.end_container(*this);
    }


    void shallow_freeze(::NMSTL::OSerial &out) const
    {
        bool shallow_freeze = true;

        out && out.begin_container(*this)
            && (out << shallow_freeze << _name)
            && freeze_ptr_impl(out, _schema)
            && out.end_container(*this);
    }


    void unfreeze(::NMSTL::ISerial &in)
    {
        bool shallow_freeze;

        in && in.begin_container(*this)
            && (::NMSTL::__iserial_adaptor(in) << shallow_freeze << _name)
            && unfreeze_ptr_impl(in, _schema);

        if (!shallow_freeze)
        {
            (::NMSTL::__iserial_adaptor(in) << _source << _sink);
        }

        in.end_container(*this);
    }
    
    NMSTL_DUMMY_SERIALREG(CatalogStream);


  private:

    /// Stream name.
    ///
    Name             _name;


    /// Stream content; Initially NULL if intermediate.
    ///
    CatalogSchema   *_schema;


    /// Stream input (Box out or unassigned for Input stream).
    ///
    BoxPort          _source;


    /// Stream output (Box in).
    ///
    vector<BoxPort>  _sink;


    CatalogSubscription*         _remote_source;
    vector<CatalogSubscription*> _remote_sink;


  public:

    /*
     * TO DO:  May have header dependencies???
     *
    // Is this stream crossing between node boundaries?
    //
    Boolean is_across_stream()
    {
        if (_source.get_port_box() == NULL)
        {
            return false;
        }

        string src_node = (_source.get_port_box())->get_box_node();
        for (unsigned int i = 0; i < _sink.size(); i++)
        {
            if ((_sink[i].get_port_box()) == NULL)
            {
                continue;
            }
            else
            {
                string dest_node = (_sink[i].get_port_box())->get_box_node();
                if (src_node != dest_node)
                {
                    return true;
                }
            }
        }

        return false;
    }
    */
};

BOREALIS_NAMESPACE_END
#endif                       // CATALOGSTREAM_H
