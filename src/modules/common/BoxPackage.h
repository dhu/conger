#ifndef BOREALIS_BOX_PACKAGE_H
#define BOREALIS_BOX_PACKAGE_H

#include <string>
#include <vector>
#include <map>
#include <NMSTL/serial>
#include <NMSTL/ptr>

#include "common.h"

BOREALIS_NAMESPACE_BEGIN;

class AbstractQueueState
{
 public:
    AbstractQueueState(){}
    virtual ~AbstractQueueState(){}

    unsigned int number_of_queues() { return _serialized_queues.size(); }
    string operator[](string stream_name) { return _serialized_queues[stream_name]; }

    bool contains_stream(string stream_name)
    {
    return _serialized_queues.find(stream_name) != _serialized_queues.end();
    }

    unsigned int number_of_tuples(string stream_name, unsigned int tuple_size);

    bool get_tuple_from_queue(char* dest, string stream_name, unsigned int tuple_size);
    bool get_tuple_batch_from_queue(char* dest, string stream_name, unsigned int tuple_size, unsigned int num_tuples);

    bool getTuple(char* dest, string& serialized_queue, unsigned int tuple_size);

 protected:
    static const int TUPLE_SERIAL_HEADER = 4;
    typedef map<string, string> packed_queues;
    typedef map<string, string>::iterator queue_iterator;
    packed_queues _serialized_queues;

 private:
    typedef map<string, ptr<ISerialString> > batch_inputs;
    batch_inputs _batch_inputs;

 public:
    NMSTL_SERIAL_BASE(AbstractQueueState, long, << _serialized_queues);
};

class AbstractBoxState
{
 public:
    AbstractBoxState(){}
    virtual ~AbstractBoxState(){}

    string as_string() const
    {
    string out("AbstractBoxState{");
    out += _serialized_state;
    out += "}";
    return out;
    }
    string _serialized_state;

 protected:

 public:
    NMSTL_SERIAL_BASE(AbstractBoxState, long, << _serialized_state);
};

class BoxPackage
{
    typedef pair< ptr<AbstractQueueState>, ptr<AbstractBoxState> >BoxState;
    typedef map<string, BoxState> Boxes;
    typedef Boxes::iterator box_iterator;
    typedef Boxes::const_iterator const_box_iterator;

    Boxes _boxes;

 public:
    BoxPackage() {}
    ~BoxPackage() {}

    box_iterator begin () { return _boxes.begin(); }
    box_iterator end () { return _boxes.end(); }

    int size()
    {
        return _boxes.size();
    }

    void add_box_with_queue_only(string boxName, ptr<AbstractQueueState> boxQueue)
    {
    //AbstractBoxState boxState;
    _boxes[boxName] = BoxState(boxQueue, ptr<AbstractBoxState>());
    }

    void add_box(string boxName, ptr<AbstractQueueState> boxQueue, ptr<AbstractBoxState> box_state)
    {
    _boxes[boxName] = BoxState(boxQueue, box_state);
    }

    const ptr<BoxState> find_box(string boxName)
    {
    const_box_iterator box = _boxes.find(boxName);
    if ( box == _boxes.end() )
        return ptr<BoxState>();
    else
        return ptr<BoxState>(new BoxState(box->second));
    }


    string as_string() const
    {
    string out("BoxPackage{");
    for (const_box_iterator i = _boxes.begin(); i != _boxes.end(); i++)
        out += i->first;
    out += "}";
    return out;
    }

    NMSTL_SIMPLY_SERIALIZABLE(BoxPackage, << _boxes);
};

BOREALIS_NAMESPACE_END;

#endif
