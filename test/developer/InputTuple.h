#ifndef PACKET_TUPLE_H
#define PACKET_TUPLE_H

#include "TupleHeader.h"

using namespace Borealis;
using namespace NMSTL;

const int PROT_SIZE = 4;

// --------------------------------------------------
struct InputTuple : public TupleHeader
{

    // Data
    int     _start_time_sec;
    char    _protocol[PROT_SIZE];

    InputTuple(int start_time_sec, string protocol)
        : TupleHeader(), _start_time_sec(start_time_sec)
        {
        memset(&this->_protocol, 0, sizeof this->_protocol);
        strncpy(this->_protocol, protocol.c_str(), sizeof this->_protocol);
    }

} __attribute__((__packed__));


#endif
