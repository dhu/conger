#include "TCPTraceFunction.h"
#include <fstream>
#include <cmath>        // for log
#include <iomanip>
#include "BRIOParams.h"



//#define DEBUG_INPUT

BOREALIS_NAMESPACE_BEGIN

vector< deque<TCPPacket> > TCPTraceFunction::_data;
ifstream TCPTraceFunction::_data_ifs;
unsigned int TCPTraceFunction::_packet_size = sizeof(TCPPacket) - sizeof(double);

istream& operator>>(istream& is, TCPPacket& packet)
{
    int destination;
    int port;
    is  >> packet.double_timestamp 
        >> packet.source 
        >> destination 
        >> port
        >> port
        >> packet.packet_size;

//    packet.source_destination_pair = 
//        packet.source + packet.destination + 
//        packet.source * packet.destination;
    packet.source_to_destination = 
        packet.source * 10000 + destination;

    packet.timestamp = (int)packet.double_timestamp;

    return is;
}

ostream& operator<<(ostream& os, const TCPPacket& packet)
{
    os << setw(5) << packet.timestamp << " "
       << setw(4) << packet.source << " "
//       << setw(4) << packet.destination << " "
//       << setw(5) << packet.source_destination_pair << " "
       << setw(8) << packet.source_to_destination << " "
       << setw(5) << packet.packet_size << " "
       << setw(8) << packet.double_timestamp;

    return os;
}

TCPTraceFunction::TCPTraceFunction():
    _trace_index(0),
    _current_index(0)
{
}

TCPTraceFunction::TCPTraceFunction(unsigned int index):
    _trace_index(index),
    _current_index(0)
{
    if (_data.size() < index + 1)
        _data.push_back(deque<TCPPacket>());
}


void TCPTraceFunction::init()
{
    if (_data.empty())
        _data.push_back(deque<TCPPacket>());

    if (_data[0].empty())
    {
        string filename = BRIOParams::input_generator_path + 
                      "source_data/TCPTrace.dat";
        _data_ifs.open(filename.c_str());
        if (!_data_ifs)
        {
                FATAL << "Can't open file " << filename ;
        }

        recharge_data();
    }
}

bool TCPTraceFunction::recharge_data()
{
    int charge_count = 1000;
    unsigned int n = _data.size();
    while (charge_count > 0 && !_data_ifs.eof())
    {
        TCPPacket a;
        _data_ifs >> a;
        if (_data_ifs.good())
        {
            _data[a.source % n].push_back(a);
            --charge_count;
        }
    }

    if (charge_count < 1000)
    {
        return true;
    }
    else
    {
        return false;
    }
}


double TCPTraceFunction::get_next_value()
{
    bool recharge = true;
    while (_data[_trace_index].size() < 100 && recharge)
    {
        recharge = recharge_data();
    }

    if (_data[_trace_index].size() < 3)
    {
        // no more data available, just return a big number
        return 10000000;
    }

    double inter_arrival_time = 
        _data[_trace_index][1].double_timestamp -
        _data[_trace_index][0].double_timestamp;

    _data[_trace_index].pop_front();


    /*
    ostringstream os;
    os << "_trace_index(" << _trace_index << ") _current_index(" 
          << inter_arrival(" << setprecision(4)
          << inter_arrival_time
          << ")";
    DEBUG << os.str();
    */

    return inter_arrival_time;
}

double TCPTraceFunction::get_average() const
{
    return 0.01;
}



BOREALIS_NAMESPACE_END
