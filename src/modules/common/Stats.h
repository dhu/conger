#ifndef MEDUSA_STATS_H
#define MEDUSA_STATS_H

#include <sstream>
#include "common.h"
#include "Name.h"

MEDUSA_NAMESPACE_BEGIN

#define ALPHA 0.98

class Stats
{

 public:
    Name _name;
    double _cpu_usage; // Fraction
    double _bw_in;
    double _bw_out;
    double _bw_usage; // Fraction of MAX_BW
    Stats(Name name = Name("default comp"))
        : _name(name), _cpu_usage(0.0), _bw_in(0.0), _bw_out(0.0), _bw_usage(0.0) {}

    string as_string() const
    {
        string out("{Stats:"+ to_string(_name) + "}");
        return out;
    }

    string repr() const
    {
        stringstream out;
        out.precision(3);
        out << "Stats for " << _name << ": CPU usage: " << 100*_cpu_usage
            << "% BW in " << _bw_in << " bytes/sec BW out "
            << _bw_out << " bytes/sec" << " so bw usage: " << 100*_bw_usage
            << "%";
        return out.str();
    }
    NMSTL_SIMPLY_SERIALIZABLE(Stats, << _name << _cpu_usage << _bw_in << _bw_out << _bw_usage);

    static double average(double old_value, double new_value)
    {
        if ( old_value <= 0.0)
            return new_value;
        else return ALPHA * old_value + (1-ALPHA) * new_value;
    }

    static double running_average(double old_value, int *count,
                                  double new_value)
    {
        DEBUG << "old val = " << old_value << ", count = " << *count <<
                 ", new val = " << new_value;
        assert(*count >= 0);
        if (old_value <= 0.0)
            return new_value;
        return (old_value*(*count) + new_value)/(++(*count));
    }
};

NMSTL_TO_STRING(Stats);

MEDUSA_NAMESPACE_END

#endif
