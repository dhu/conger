#include <sstream>
#include "NetworkTrafficQueryParams.h"

BOREALIS_NAMESPACE_BEGIN

NetworkTrafficQueryParams* NetworkTrafficQueryParams::get_singleton()
{
    static NetworkTrafficQueryParams* singleton = 0;
    if (singleton == 0)
    {
        singleton = new NetworkTrafficQueryParams();
    }

    return singleton;
}

NetworkTrafficQueryParams::NetworkTrafficQueryParams() :
    number_of_queries(2),
    number_of_group_by_values(2),
    number_of_count_box_per_query(2),
    number_of_avg_box_per_query(2),
    timeout(0),
    initial_window_size(1),
    window_size_step(5),
    initial_advance(1),
    advance_step(5),
    number_of_count_sliding_window_boxes(2),
    number_of_avg_sliding_window_boxes(2),
    initial_sliding_window_size(3),
    sliding_window_size_step(1)
{
}

ostream& operator<<(ostream& os, const NetworkTrafficQueryParams& ob)
{
    os << "number_of_queries: " << ob.number_of_queries << endl;
    os << "number_of_group_by_values: " << ob.number_of_group_by_values << endl;
    os << "number_of_count_box_per_query: " << ob.number_of_count_box_per_query << endl;
    os << "number_of_avg_box_per_query: " << ob.number_of_avg_box_per_query << endl;
    os << "timeout: " << ob.timeout << endl;
    os << "initial_window_size: " << ob.initial_window_size << endl;
    os << "window_size_step: " << ob.window_size_step << endl;
    os << "initial_advance: " << ob.initial_advance << endl;
    os << "advance_step: " << ob.advance_step << endl;
    os << "number_of_count_sliding_window_boxes: " 
       << ob.number_of_count_sliding_window_boxes << endl;
    os << "number_of_avg_sliding_window_boxes: " 
       << ob.number_of_avg_sliding_window_boxes << endl;
    os << "initial_sliding_window_size: " 
       << ob.initial_sliding_window_size << endl;
    os << "sliding_window_size_step: " << ob.sliding_window_size_step << endl;
    return os;
};

istream& operator>>(
        istream& is,
        NetworkTrafficQueryParams& ob)
{
    char buf[1024];
    while(!is.eof())
    {
        is.getline(buf, sizeof(buf));
        istringstream line(buf);
        line >> buf;

        if (strcmp(buf, "")== 0)
            continue;
        else if (strcmp(buf, "number_of_queries:") == 0)
              line >> ob.number_of_queries;
        else if (strcmp(buf, "number_of_group_by_values:") == 0)
              line >> ob.number_of_group_by_values;
        else if (strcmp(buf, "number_of_count_box_per_query:") == 0)
              line >> ob.number_of_count_box_per_query;
        else if (strcmp(buf, "number_of_avg_box_per_query:") == 0)
              line >> ob.number_of_avg_box_per_query;
        else if (strcmp(buf, "timeout:") == 0)
              line >> ob.timeout;
        else if (strcmp(buf, "initial_window_size:") == 0)
              line >> ob.initial_window_size;
        else if (strcmp(buf, "window_size_step:") == 0)
              line >> ob.window_size_step;
        else if (strcmp(buf, "initial_advance:") == 0)
              line >> ob.initial_advance;
        else if (strcmp(buf, "advance_step:") == 0)
              line >> ob.advance_step;
        else if (strcmp(buf, "number_of_count_sliding_window_boxes:") == 0)
              line >> ob.number_of_count_sliding_window_boxes;
        else if (strcmp(buf, "number_of_avg_sliding_window_boxes:") == 0)
              line >> ob.number_of_avg_sliding_window_boxes;
        else if (strcmp(buf, "initial_sliding_window_size:") == 0)
              line >> ob.initial_sliding_window_size;
        else if (strcmp(buf, "sliding_window_size_step:") == 0)
              line >> ob.sliding_window_size_step;
        else
              cout << "Unknown parameter for NetworkTrafficQueryParams: " << buf << endl;
    }

    return is;
};


BOREALIS_NAMESPACE_END
