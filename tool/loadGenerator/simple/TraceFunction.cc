#include "TraceFunction.h"
#include <fstream>
#include <cmath>        // for log
#include "BRIOParams.h"



//#define DEBUG_INPUT

BOREALIS_NAMESPACE_BEGIN

vector<double> TraceFunction::_data;
int TraceFunction::_shift = 0;

TraceFunction::TraceFunction(): _unit(5000)
{
    init();
}

TraceFunction::TraceFunction(int unit): _unit(unit)
{
    init();
}


void TraceFunction::init()
{
    _current_index = -1;
    if (_data.empty())
    {
        fstream of;
        string filename = BRIOParams::input_generator_path + 
                      "source_data/TraceData.dat";
        of.open(filename.c_str(), ios::in);
        if (!of)
        {
                cerr << "can't open file " << filename << endl;
                exit(1);
        }
        while (!of.eof())
        {
            double a;
            of >> a;
            _data.push_back(a);
        }
    }
    _start_index = _shift;
    _shift += _unit;
    if (_shift + _unit >= (int)_data.size())
        _shift = 0;
}

double TraceFunction::get_next_value()
{
    ++_current_index;
    if (_current_index + _start_index >= (int)_data.size())
        _current_index = -_start_index;

    return _data[_current_index + _start_index];
}

double TraceFunction::get_average() const
{
    return 0.8999;
}



BOREALIS_NAMESPACE_END
