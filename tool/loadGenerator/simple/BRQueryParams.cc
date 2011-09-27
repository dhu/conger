#include <sstream>
#include "BRQueryParams.h"

BOREALIS_NAMESPACE_BEGIN

BRQueryParams::BRQueryParams() :
    number_of_fields(2),
    number_of_boxes(40),
    prob_stop(0.2),
    prob_reuse_stream(0.4),
    prob_box_delay(1.0),
    prob_box_filter(0.0),
    prob_box_union(0.0),
    delay_min(1),
    delay_max(10),
    delay_selectivity_percentage(0.5),
    delay_selectivity_min(0.1),
    delay_selectivity_max(1),
    filter_min(10),
    filter_max(90)
{
    max_number_of_boxes_per_query_tree = number_of_boxes;
}

ostream& operator<<(ostream& os, const BRQueryParams& ob)
{
    os << "number_of_fields: " << ob.number_of_fields << endl;
    os << "number_of_boxes: " << ob.number_of_boxes << endl;
    os << "max_number_of_boxes_per_query_tree: " << ob.max_number_of_boxes_per_query_tree << endl;
    os << "prob_stop: " << ob.prob_stop << endl;
    os << "prob_reuse_stream: " << ob.prob_reuse_stream << endl;
    os << "prob_box_delay: " << ob.prob_box_delay << endl;
    os << "prob_box_filter: " << ob.prob_box_filter << endl;
    os << "prob_box_union: " << ob.prob_box_union << endl;
    os << "delay_min: " << ob.delay_min << endl;
    os << "delay_max: " << ob.delay_max << endl;
    os << "delay_selectivity_percentage: " << ob.delay_selectivity_percentage << endl;
    os << "delay_selectivity_min: " << ob.delay_selectivity_min << endl;
    os << "delay_selectivity_max: " << ob.delay_selectivity_max << endl;
    os << "filter_min: " << ob.filter_min << endl;
    os << "filter_max: " << ob.filter_max << endl;
    return os;
};

istream& operator>>(
        istream& is,
        BRQueryParams& ob)
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
        else if (strcmp(buf, "number_of_boxes:") == 0)
              line >> ob.number_of_boxes;
        else if (strcmp(buf, "max_number_of_boxes_per_query_tree:") == 0)
              line >> ob.max_number_of_boxes_per_query_tree;
        else if (strcmp(buf, "prob_stop:") == 0)
              line >> ob.prob_stop;
        else if (strcmp(buf, "prob_reuse_stream:") == 0)
              line >> ob.prob_reuse_stream;
        else if (strcmp(buf, "prob_box_delay:") == 0)
              line >> ob.prob_box_delay;
        else if (strcmp(buf, "prob_box_filter:") == 0)
              line >> ob.prob_box_filter;
        else if (strcmp(buf, "prob_box_union:") == 0)
              line >> ob.prob_box_union;
        else if (strcmp(buf, "delay_min:") == 0)
              line >> ob.delay_min;
        else if (strcmp(buf, "delay_max:") == 0)
              line >> ob.delay_max;
        else if (strcmp(buf, "delay_selectivity_percentage:") == 0)
              line >> ob.delay_selectivity_percentage;
        else if (strcmp(buf, "delay_selectivity_min:") == 0)
              line >> ob.delay_selectivity_min;
        else if (strcmp(buf, "delay_selectivity_max:") == 0)
              line >> ob.delay_selectivity_max;
        else if (strcmp(buf, "filter_min:") == 0)
              line >> ob.filter_min;
        else if (strcmp(buf, "filter_max:") == 0)
              line >> ob.filter_max;
        else
              cout << "Unknown parameter for BRQueryParams: " << buf << endl;
    }
    return is;
};


BOREALIS_NAMESPACE_END
