#include "RandomPulseFunction.h"
#include <fstream>
#include <cmath>        // for log
#include "BRIOParams.h"



//#define DEBUG_INPUT

BOREALIS_NAMESPACE_BEGIN

vector<vector<double> > RandomPulseFunction::_data;


RandomPulseFunction::RandomPulseFunction(
            unsigned int dimension, 
            unsigned int dimension_index, 
            double multiplier,
            double busy_period,
            double idle_peirod):
    _dimension(dimension),
    _dimension_index(dimension_index),
    _multiplier(multiplier),
    _busy_period(busy_period),
    _idle_peirod(idle_peirod),
    _current_index(0),
    _accumulated_busy_period(0),
    _value(0)
{
    if (_dimension_index > _dimension) 
    {
        cerr << "RandomPulseFunction: dimension index can not be larger than dimension." << endl;
        exit(1);
    }
    if (_data.empty())
    {
        // read data size 
        vector<int> data_size;
        ifstream ifs;
        string fn = BRIOParams::input_generator_path +
            "source_data/fs_sequence_size";
        ifs.open(fn.c_str(), ios::in);
        if (!ifs) {
                cerr << "Can not open fs sequence size file " << fn << endl;
                exit(1);
        }
        unsigned int count = 0;
        while (!ifs.eof() && count < 15)
        {
            ++count;
            double a;
            ifs >> a;
            data_size.push_back((int)a);
        }

        ifs.close();

        if (_dimension > count) 
        {
            cerr << "RandomPulseFunction: dimension higher than " << count 
                 << "is not supported." << endl;
            exit(1);
        }

        fstream of;
        ostringstream filename;  
        filename << BRIOParams::input_generator_path 
                 << "source_data/fs_sequence" 
                 << _dimension << + ".dat";
        of.open(filename.str().c_str(), ios::in);
        if (!of)
        {
                cerr << "Can not open fs sequence file " << filename.str() << endl;
                exit(1);
        }
        for (unsigned int i = 0; i < _dimension; ++i)
        {
            _data.push_back(vector<double>());
            for (int j = 0; j < data_size[_dimension - 1]; ++j) 
            {
                double a;
                of >> a;
                _data[i].push_back(a);
            }
        }
        of.close();
    }
}


double RandomPulseFunction::get_next_value()
{
    if (_value == 0) 
    {
        _value = 1.0 / _data[_dimension_index - 1][_current_index] / _multiplier;
        ++_current_index;
        if (_current_index >= _data[_dimension_index - 1].size())
        {
            _current_index = 0;
        }

    }

    if (_busy_period - _accumulated_busy_period < _value / 2.0) {
        double value =  _busy_period - _accumulated_busy_period + _idle_peirod;
        _value = 0;
        _accumulated_busy_period = 0;
        return value;
    }
    else 
    {
        _accumulated_busy_period += _value;
        return _value;
    }
}

double RandomPulseFunction::get_average() const
{
    // TODO never used yet
    return 1;
}



BOREALIS_NAMESPACE_END
