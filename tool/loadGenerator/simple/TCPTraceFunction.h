#ifndef TCPTraceFunction_H
#define TCPTraceFunction_H

#include "Function.h"
#include <fstream>
#include <deque>

BOREALIS_NAMESPACE_BEGIN

/**
 * The TCPTraceFunction class returns values from a web trace
 */
struct TCPPacket
{
    int timestamp;
    int source;
//    int destination;
//    int source_destination_pair;
    int source_to_destination;
    int packet_size;
    double double_timestamp;
};

istream& operator>>(istream& is, TCPPacket& packet);
ostream& operator<<(ostream& os, const TCPPacket& packet);

class TCPTraceFunction: public Function
{
public:

    /**
     * Constructors
     */
    TCPTraceFunction();

    TCPTraceFunction(unsigned int index);

    /**
     * Returns the next value
     */
    double get_next_value();


    /**
     * Returns the average value
     */
    double get_average() const;

    /**
     * Returns the address of the data generated
     */
    virtual const char* get_data_handle() const 
    {  
        return (char*)&(_data[_trace_index][1]);
    }

    /**
     * Returns the size of the data generated
     */
    virtual unsigned int get_data_size() const
    { 
        return _packet_size;
    }

    /**
     * Initialize the function
     */
    static void init();

private:

    unsigned int _trace_index;

    unsigned int _current_index;

    static unsigned int _packet_size;

    static vector< deque<TCPPacket> > _data;

    static ifstream _data_ifs;

    /*
     * Read data from file and buffer them
     */
    static bool recharge_data();
};


BOREALIS_NAMESPACE_END
#endif


