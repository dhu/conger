#ifndef MEDUSA_STREAMSTATE_H
#define MEDUSA_STREAMSTATE_H

#include "common.h"
#include "Objects.h"
#include "StreamBuffer.h"
#include "Stats.h"

BOREALIS_NAMESPACE_BEGIN;

class StreamState
{

 private:

    /// The name of the associated stream
    Name _name;

    /// The definition of the associated stream
    ptr<StreamDef> _stream_def;

 public:

    // Stream classification.
    enum Class
    {
        INJECT,
        OTHER
    };

    int _class;
    bool is_injected_stream() { return _class == INJECT; }

    // List of subscriptions
    vector<Subscription> _fast_subs; // List of clients subscribed to this stream

    // If not a local stream, have I asked to receive events on this stream?
    bool _subscribed;
    pair<MedusaID,Subscription> _sub; // Remember our subscription so we can remove it later

    // How many local queries are connected to this stream on either end
    int _nb_queries;

    StreamBuffer _buffer; // Buffers tuples on this stream
    //bool m_delete;

    // For stats purposes
    double _tuples;
    double _bytes;

    StreamState(Name stream_name, ptr<StreamDef> stream_def, Class clazz) :
        _name(stream_name), _stream_def(stream_def), _class(clazz),
        _subscribed(false), _nb_queries(0),
        _buffer(stream_name),
        //m_delete(false),
        _tuples(0.0), _bytes(0.0)
        {
    }

    string as_string() const
    {
        string out = string("StreamState{") + _name;
        if (_stream_def)
            out << "; def=" << *(_stream_def);

    out << "; class=";
        switch (_class)
        {
        case INJECT: out << "INJECT"; break;
        default: out << "OTHER";
    };

        if (_subscribed)
        {
            out << "; subscribed";
            out << " to " << _sub.first;
        }
        out << "; nb_queries is " << _nb_queries;
        out << "; buffer is " << _buffer;
        out << "}";

        return out;
    }

    string repr() const { return as_string(); }

    ptr<StreamDef> get_stream_def() const { return _stream_def; }
    void set_definition(ptr<StreamDef> str) { _stream_def = str; }

    const Name& get_name() const { return _name; }
    Schema get_schema() const { return _stream_def->get_schema(); }

    // This will go away once we have a good StatsManager!!!
    // Rates of tuples and bytes on this stream
    struct stream_stats
    {
        double _tuple_rate;
        double _running_tuple_rate;
        int    _count;
        double _byte_rate;
        stream_stats() : _tuple_rate(0.0), _running_tuple_rate(0.0),
                         _count(0), _byte_rate(0.0) {}
    };
    stream_stats _stream_stats;
    void incr_stats(double tuples, double bytes)
    {
        _tuples += tuples;
        _bytes += bytes;
    }
    void end_stat_period(double delta_time_msecs)
    {
        //NOTICE << m_name << ": m_tuples = " << m_tuples
        //     << ", delta_time_msecs = " << delta_time_msecs;
        _stream_stats._tuple_rate
            = Stats::average(_stream_stats._tuple_rate,MSECS*_tuples/delta_time_msecs);
        _stream_stats._running_tuple_rate
            = Stats::running_average(_stream_stats._running_tuple_rate, &(_stream_stats._count), MSECS*_tuples/delta_time_msecs);
        _stream_stats._byte_rate
            = Stats::average(_stream_stats._byte_rate,MSECS*_bytes/delta_time_msecs);
        _tuples = _bytes = 0.0;
    }
    stream_stats get_stream_stats()
    {
        return _stream_stats;
    }

    double n_tuples() {return _tuples;};


};

BOREALIS_NAMESPACE_END


#endif
