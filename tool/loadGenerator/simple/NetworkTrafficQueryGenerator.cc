#include "NetworkTrafficQueryGenerator.h"
#include "NetworkTrafficQueryParams.h"
#include <sstream>

BOREALIS_NAMESPACE_BEGIN

const string IND = "    ";

string to_string(int a)
{
    char x[256];
    sprintf(x, "%d", a);
    return string(x);
}

string NetworkTrafficQueryGenerator::get_query_xml()
{
    //generateSchema(sizeof(TupleData) / sizeof(int));
    generate_schema();
    for (unsigned int i = 1; 
         i <= NetworkTrafficQueryParams::get_singleton()->number_of_queries;
         ++i)
    {
        generate_query(to_string(i));
    }
    generate_iostreams();

    ostringstream oss;
    oss << "<?xml version=\"1.0\"?>" << endl << endl;
    oss << "<borealis>" << endl << endl;
    oss << _schema.str() << endl;
    oss << _input_streams.str() << endl;
    oss << _output_streams.str() << endl;
    oss << _boxes.str() << endl;
    oss << "</borealis>" << endl;
    return oss.str();
}

void NetworkTrafficQueryGenerator::generate_schema()
{
    _schema 
        << IND << "<schema name=\"input_schema\">" << endl
        << IND << IND << "<field name=\"timestamp\" type=\"int\"/>" << endl
        << IND << IND << "<field name=\"source\" type=\"int\"/>" << endl
        << IND << IND << "<field name=\"src_to_dest\" type=\"int\"/>" << endl
        << IND << IND << "<field name=\"packet_size\" type=\"int\"/>" << endl
        << IND << "</schema>" << endl << endl;

//        << IND << IND << "<field name=\"destination\" type=\"int\"/>" << endl
//        << IND << IND << "<field name=\"src_dest_pair\" type=\"int\"/>" << endl
//        << IND << IND << "<field name=\"real_time\" type=\"int\"/>" << endl

    generate_agg_schema("count", "", "source");
    generate_agg_schema("avg", "packet_size", "source");
    generate_agg_schema("count", "", "src_to_dest");
    generate_agg_schema("avg", "packet_size", "src_to_dest");

}

void NetworkTrafficQueryGenerator::generate_agg_schema(
        string function_name,
        string attribute_name,
        string group_by)
{
    _schema 
        << IND << "<schema name=\"" << function_name << "_" << attribute_name 
               << "_" << group_by << "_schema\">" << endl
        << IND << IND << "<field name=\"" << group_by 
                      << "\" type=\"int\"/>" << endl
        << IND << IND << "<field name=\"timestamp\" type=\"int\"/>" << endl
        << IND << IND << "<field name=\"" << function_name 
                      << "\" type=\"int\"/>" << endl
        << IND << "</schema>" << endl << endl;
//        << IND << IND << "<field name=\"real_time\" type=\"int\"/>" << endl
}

void NetworkTrafficQueryGenerator::generate_query(string index)
{
    // First box accept input stream
    _boxes 
        << IND << "<box name=\"union" << index << "\" type=\"union\">" << endl
        << IND << IND << "<in stream=\"input" << index << "\"/>" << endl
        << IND << IND << "<out stream=\"in" << index << "\"/>" << endl
        << IND << "</box>" << endl;

    _input_stream_names.push_back(string("input") + index);

    _boxes << endl;

    unsigned box_index = 1;

    for (unsigned group_by_index = 1; 
         group_by_index <= NetworkTrafficQueryParams::get_singleton()->number_of_group_by_values;
         ++group_by_index)
    {
        string group_by = "source";
        if (group_by_index == 2)
        {
            group_by = "src_to_dest";
        }

        for (unsigned int i = 0; 
           i < NetworkTrafficQueryParams::get_singleton()->number_of_count_box_per_query;
           ++i)
        {
            generate_agg_box(
                    index, 
                    to_string(box_index),
                    to_string(
                        NetworkTrafficQueryParams::get_singleton()->initial_window_size 
                        + i * 
                        NetworkTrafficQueryParams::get_singleton()->window_size_step),
                    "VALUES",
                    to_string(
                        NetworkTrafficQueryParams::get_singleton()->initial_advance
                        + i * 
                        NetworkTrafficQueryParams::get_singleton()->advance_step),
                    "count",
                    "",
                    group_by);
            ++box_index;
        }
 
        for (unsigned int i = 0; 
           i < NetworkTrafficQueryParams::get_singleton()->number_of_avg_box_per_query;
           ++i)
        {
            generate_agg_box(
                    index, 
                    to_string(box_index),
                    to_string(
                        NetworkTrafficQueryParams::get_singleton()->initial_window_size 
                        + i * 
                        NetworkTrafficQueryParams::get_singleton()->advance_step),
                    "VALUES",
                    to_string(
                        NetworkTrafficQueryParams::get_singleton()->initial_advance
                        + i * 
                        NetworkTrafficQueryParams::get_singleton()->advance_step),
                    "avg",
                    "packet_size",
                    group_by);
            ++box_index;
        }

        for (unsigned int i = 0; 
           i < NetworkTrafficQueryParams::get_singleton()->number_of_count_sliding_window_boxes;
           ++i)
        {
            generate_agg_box(
                    index, 
                    to_string(box_index),
                    to_string(
                        NetworkTrafficQueryParams::get_singleton()->initial_sliding_window_size 
                        + i * 
                        NetworkTrafficQueryParams::get_singleton()->sliding_window_size_step),
                    "VALUES",
                    to_string(1),
                    "count",
                    "",
                    group_by);
            ++box_index;
        }
 
        for (unsigned int i = 0; 
           i < NetworkTrafficQueryParams::get_singleton()->number_of_avg_sliding_window_boxes;
           ++i)
        {
            generate_agg_box(
                    index, 
                    to_string(box_index),
                    to_string(
                        NetworkTrafficQueryParams::get_singleton()->initial_sliding_window_size 
                        + i * 
                        NetworkTrafficQueryParams::get_singleton()->sliding_window_size_step),
                    "VALUES",
                    to_string(1),
                    "avg",
                    "packet_size",
                    group_by);
            ++box_index;
        }
    }

}

void NetworkTrafficQueryGenerator::generate_agg_box(
        string query_index,
        string box_index,
        string window_size,
        string window_size_by,
        string advance,
        string function_name,
        string attribute_name,
        string group_by)
{
    _boxes
        << IND << "<box name=\"" << function_name << "_" << attribute_name 
               << query_index << "." << box_index
               << "\" type=\"aggregate\">" << endl

        << IND << IND << "<in  stream=\"in" << query_index << "\" />" << endl
        << IND << IND << "<out stream=\"" << function_name << query_index << "." << box_index
               << "\" />" << endl

        << IND << IND << "<parameter   name=\"advance\"               "
                      << "value=\"" << advance << "\" />" << endl
        << IND << IND << "<parameter   name=\"aggregate-function.0\"  "
                      << "value=\"" << function_name 
                      << "(" << attribute_name << ")\" />" << endl

        // add real time field
        << IND << IND << "<parameter   name=\"aggregate-function-output-name.0\""
                      << "  value=\"" << function_name << "\" />" << endl
/*
        << IND << IND << "<parameter   name=\"aggregate-function.1\"  "
                      << "value=\"lastval(real_time)\" />" << endl
        << IND << IND << "<parameter   name=\"aggregate-function-output-name.1\""
                      << "  value=\"real_time\" />" << endl
*/

        << IND << IND << "<parameter   name=\"order-by\"              "
                      << "value=\"FIELD\" />" << endl
        << IND << IND << "<parameter   name=\"order-on-field\"        "
                      << "value=\"timestamp\" />" << endl
        << IND << IND << "<parameter   name=\"group-by\"              "
                      << "value=\"" << group_by << "\" />" << endl
        << IND << IND << "<parameter   name=\"window-size\"           "
                      << "value=\"" << window_size << "\"        />" << endl
        << IND << IND << "<parameter   name=\"window-size-by\"        "
                      << "value=\"" << window_size_by << "\" />" << endl;
//        << IND << IND << "<parameter   name=\"window-size-by\"        "
//                      << "value=\"TUPLES\" />" << endl;

    // add timeout field 
    if (NetworkTrafficQueryParams::get_singleton()->timeout > 0 &&
        window_size_by == "VALUE")
    {
        _boxes 
        << IND << IND << "<parameter   name=\"timeout\"               "
                      << "value=\"" 
                      << NetworkTrafficQueryParams::get_singleton()->timeout
                      << "\" />" << endl;

    }


    _boxes << IND << "</box>" << endl;

    _boxes << endl;

    _output_stream_names.push_back(
            function_name + query_index + "." + box_index); 
    _output_stream_schema.push_back(
            function_name + "_" + attribute_name + "_" + group_by + "_schema");
}


void NetworkTrafficQueryGenerator::generate_iostreams()
{
    for (unsigned int i = 0; i < _input_stream_names.size(); ++i)
    {
        generate_input_stream(_input_stream_names[i]);
    }
    for (unsigned int i = 0; i < _output_stream_names.size(); ++i)
    {
        generate_output_stream(
                _output_stream_names[i],
                _output_stream_schema[i]);
    }
}


void NetworkTrafficQueryGenerator::generate_input_stream(const string& name)
{
    _input_streams << IND << "<input  stream=\"" << name  << "\""
            << IND << "schema=\"input_schema\" />" << endl;
}

void NetworkTrafficQueryGenerator::generate_output_stream(
        const string& name, const string& schema)
{
    _output_streams << IND << "<output stream=\"" << name  << "\""
            << IND << "schema=\"" << schema << "\" />" << endl;
}



BOREALIS_NAMESPACE_END
