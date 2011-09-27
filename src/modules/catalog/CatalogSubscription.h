#ifndef CATALOGSUBSCRIPTION_H
#define CATALOGSUBSCRIPTION_H

#include <NMSTL/net>

#include "common.h"
#include "CatalogSerialize.h"
#include "Tuple.h"

BOREALIS_NAMESPACE_BEGIN;

class CatalogStream;

class CatalogSubscription
{
  public:

    CatalogSubscription()
    {
        _stream = NULL;
        _sink_flag = false;
    }

    CatalogSubscription(InetAddress  dest,
                               Name  stream_name,
                             string  last_insertion_tuple,
                            Boolean  tentative)
    {
        _dest = dest;
        _stream_name = stream_name;
        _gap  = 0xFFFF;                  // The default gap is unlimited.
        _last_insertion_tuple = last_insertion_tuple;
        _tentative = tentative;
        _stream = NULL;
        _sink_flag = false;
    }


    CatalogSubscription( InetAddress  dest, Name  stream_name )
    {
        _dest = dest;
        _stream_name = stream_name;
        _gap  = 0xFFFF;                  // The default gap is unlimited.
        _last_insertion_tuple = "";      // Test hack???
        _tentative = False;              // Test hack???
        _stream = NULL;
        _sink_flag = false;
    }


    void shallow_copy( const CatalogSubscription  &sub )
    {
        _dest = sub._dest;
        _stream_name = sub._stream_name;
        _stream = NULL;
        _gap  = sub._gap;
        _last_insertion_tuple = sub._last_insertion_tuple;
        _tentative = sub._tentative;
        _sink_flag = sub._sink_flag;
    }


    Name get_subscription_stream_name()
    {   return  _stream_name;
    }


    CatalogStream  *get_subscription_stream() const
    {   return  _stream;
    }


    void set_subscription_stream( CatalogStream  *stream )
    {   _stream = stream;
    }


    InetAddress get_subscription_endpoint()
    {   return  _dest;
    }


    void set_subscription_endpoint( InetAddress dest )
    {   _dest = dest;
    }


    void set_gap( uint16  gap )
    {   _gap = gap;
    }


    uint16  get_gap() const
    {   return  _gap;
    }


    /// Set information about the most recent tuples received so
    /// subscription can start from the correct point in the stream
    ///
    void set_history(string last_insertion_tuple, Boolean tentative)
    {
        _last_insertion_tuple = last_insertion_tuple;
        _tentative = tentative;
    }


    /// @return true if the history information is set
    ///
    bool is_valid_last_insertion_tuple()
    {   return  _last_insertion_tuple.length() > 0;
    }


    /// @return the last insertion tuple received by the destination
    /// That means we need to send only tuples that follow the one requested
    ///
    const Tuple  get_last_insertion_tuple()
    {   return  Tuple::factory(_last_insertion_tuple.data());
    }


    /// @return the last insertion tuple received by the destination
    ///
    string get_last_insertion_tuple_as_string()
    {   return  _last_insertion_tuple;
    }


    /// @return true if the destination got tentative tuples that
    /// must be corrected before we can send any new regular insertion tuples
    ///
    Boolean  tentative_tuple_to_correct()
    {   return  _tentative;
    }


    void set_sink_flag()
    {
        DEBUG << "Setting the sink flag";
        _sink_flag = true;
    }


    bool  get_sink_flag()
    {
        return  _sink_flag;
    }

    string  as_string() const
    {
        string  r;

        r += to_string(_dest);

        return  r;
    }


    string  form_subscribe_xml()
    {
        string  xml;

        xml = "<subscribe  stream=\"" + to_string(_stream_name)
                   + "\" endpoint=\"" + to_string(_dest);

        if (get_gap() != 0xFFFF)
        {   xml += "\" gap=\"" + to_string(get_gap());
        }

        xml += "\" />\n";

        return  xml;
    }


    string  form_publish_xml()
    {
        string  xml;

        xml = "<publish  stream=\"" + to_string(_stream_name)
                     + "\" node=\"" + to_string(_dest);

        xml += "\" />\n";

        return  xml;
    }


    NMSTL_SERIALIZABLE(CatalogSubscription);

    void freeze(::NMSTL::OSerial &out) const
    {
    out && out.begin_container(*this)
        && (out << _dest << _stream_name  << _last_insertion_tuple << _tentative << _sink_flag)
            //      && freeze_ptr_impl(out, _stream)
        && out.end_container(*this);
    }

    void unfreeze(::NMSTL::ISerial &in)
    {
    in && in.begin_container(*this)
        && (::NMSTL::__iserial_adaptor(in) << _dest << _stream_name << _last_insertion_tuple << _tentative << _sink_flag)
            //      && unfreeze_ptr_impl(in, _stream)
        && in.end_container(*this);
    }

    NMSTL_DUMMY_SERIALREG(CatalogSubscription);

    NMSTL_COMPARE_OPS(CatalogSubscription, _dest)

 private:
    InetAddress      _dest;
    Name             _stream_name;
    CatalogStream   *_stream;

    // Number of consecutive tuples that are allowed to be lost.
    // The default value is 0xFFFF; which is unbounded.
    //
    uint16           _gap;

    /// Optional history information. 
    /// What is the last INSERTION tuple that the destination received
    /// The source should start sending all tuples following the one indicated
    string           _last_insertion_tuple;

    /// Did the destination receive TENTATIVE tuples that need to be corrected
    Boolean          _tentative;    


    /// subscription to a sink stream.
    /// HACK:  Work around a bug in move box and the neigborhood load
    ///        shedder???
    ///
    bool             _sink_flag;

};

BOREALIS_NAMESPACE_END

#endif // CATALOGSUBSCRIPTION_H
