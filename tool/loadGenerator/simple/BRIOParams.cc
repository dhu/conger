#include <sstream>
#include "BRIOParams.h"

BOREALIS_NAMESPACE_BEGIN

string BRIOParams::input_generator_path;
string BRIOParams::rate_multiplier_file_name;
double BRIOParams::rate_multiplier = 1;

BRIOParams* BRIOParams::get_singleton()
{
    static BRIOParams* singleton = 0;
    if (singleton == 0)
    {
        singleton = new BRIOParams();
    }

    return singleton;
}

BRIOParams::BRIOParams() :
    number_of_fields(2),
    batch_interval(0.2),
    uniform_input_distr_prob(0.5),
    random_walk_input_distr_prob(0.5),
    const_input_rate_prob(0.5),
    trace_input_rate_prob(0.5),
    use_tcp_trace(false),
    input_rate_multiplier_lower_bound(1),
    input_rate_multiplier_upper_bound(1),
    batch_size_lower_bound(1),
    batch_size_upper_bound(1),
    stream_rate_mode(0),
    batch_size_increasing_interval(0),
    random_rate_duration(3.0),
    idle_duration(7.0)
{
}

ostream& operator<<(ostream& os, const BRIOParams& ob)
{
    os << "number_of_fields: " << ob.number_of_fields << endl;
    os << "batch_interval: " << ob.batch_interval << endl;
    os << "uniform_input_distr_prob: " << ob.uniform_input_distr_prob << endl;
    os << "random_walk_input_distr_prob: " << ob.random_walk_input_distr_prob << endl;
    os << "const_input_rate_prob: " << ob.const_input_rate_prob << endl;
    os << "trace_input_rate_prob: " << ob.trace_input_rate_prob << endl;
    os << "use_tcp_trace: " << ob.use_tcp_trace << endl;
    os << "rate_multiplier: " << ob.rate_multiplier << endl;
    os << "input_rate_multiplier_lower_bound: " << ob.input_rate_multiplier_lower_bound << endl;
    os << "input_rate_multiplier_upper_bound: " << ob.input_rate_multiplier_upper_bound << endl;
    os << "batch_size_lower_bound: " << ob.batch_size_lower_bound << endl;
    os << "batch_size_upper_bound: " << ob.batch_size_upper_bound << endl;
    os << "stream_rate_mode: " << ob.stream_rate_mode << endl;
    os << "batch_size_increasing_interval: " << ob.batch_size_increasing_interval << endl;
    os << "random_rate_duration: " << ob.random_rate_duration << endl;
    os << "idle_duration: " << ob.idle_duration << endl;
    return os;
};

istream& operator>>(
        istream& is,
        BRIOParams& ob)
{
    char buf[1024];
    while(!is.eof())
    {
        is.getline(buf, sizeof(buf));
        istringstream line(buf);
        line >> buf;

        if (strcmp(buf, "")== 0)
            continue;
        else if (strcmp(buf, "number_of_fields:") == 0)
              line >> ob.number_of_fields;
        else if (strcmp(buf, "batch_interval:") == 0)
              line >> ob.batch_interval;
        else if (strcmp(buf, "uniform_input_distr_prob:") == 0)
              line >> ob.uniform_input_distr_prob;
        else if (strcmp(buf, "random_walk_input_distr_prob:") == 0)
              line >> ob.random_walk_input_distr_prob;
        else if (strcmp(buf, "const_input_rate_prob:") == 0)
              line >> ob.const_input_rate_prob;
        else if (strcmp(buf, "trace_input_rate_prob:") == 0)
              line >> ob.trace_input_rate_prob;
        else if (strcmp(buf, "use_tcp_trace:") == 0)
              line >> ob.use_tcp_trace;
        else if (strcmp(buf, "input_rate_multiplier_lower_bound:") == 0)
              line >> ob.input_rate_multiplier_lower_bound;
        else if (strcmp(buf, "input_rate_multiplier_upper_bound:") == 0)
              line >> ob.input_rate_multiplier_upper_bound;
        else if (strcmp(buf, "batch_size_lower_bound:") == 0)
              line >> ob.batch_size_lower_bound;
        else if (strcmp(buf, "batch_size_upper_bound:") == 0)
              line >> ob.batch_size_upper_bound;
        else if (strcmp(buf, "stream_rate_mode:") == 0)
              line >> ob.stream_rate_mode;
        else if (strcmp(buf, "batch_size_increasing_interval:") == 0)
              line >> ob.batch_size_increasing_interval;
        else if (strcmp(buf, "random_rate_duration:") == 0)
              line >> ob.random_rate_duration;
        else if (strcmp(buf, "idle_duration:") == 0)
              line >> ob.idle_duration;
        else
              cout << "Unknown parameter for BRIOParams: " << buf << endl;
    }
    return is;
};


BOREALIS_NAMESPACE_END
