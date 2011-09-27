#ifndef NetworkTrafficQueryGenerator_H
#define NetworkTrafficQueryGenerator_H

#include <common.h>

BOREALIS_NAMESPACE_BEGIN

/**
 * The BRQueryGenerator class generate XML query graphs
 */
class NetworkTrafficQueryGenerator
{
public:
    NetworkTrafficQueryGenerator() {}

    string get_query_xml();

private:

    /*
     * Gernerate an aggregate query
     * @param index the index if this query
     */
    void generate_query(string index);

    /*
     * Gernerate an aggregate box
     */
    void generate_agg_box(
        string query_index,
        string box_index,
        string window_size,
        string window_size_by,
        string advance,
        string function_name,
        string attribute_name,
        string group_by);

    /*
     * Generate schema
     */
    void generate_schema();

    /*
     * Generate schema for aggregate box
     */
    void generate_agg_schema(
        string function_name,
        string attribute_name,
        string group_by);

    /*
     * Generate IO streams
     */
    void generate_iostreams();

    /*
     */
    void generate_input_stream(const string& name);

    /*
     */
    void generate_output_stream(const string& name, const string& schema);

    /*
     * The string stream that stores all the streams
     */
    ostringstream _schema;

    /*
     * The string stream that stores all the input streams
     */
    ostringstream _input_streams;

    /*
     * The string stream that stores all the output streams
     */
    ostringstream _output_streams;

    /*
     * The string stream that stores all the input streams
     */
    ostringstream _boxes;

    /*
     * Input stream names
     */
    vector<string> _input_stream_names;

    /*
     * Output stream names
     */
    vector<string> _output_stream_names;

    /*
     * Output stream schemas
     */
    vector<string> _output_stream_schema;

};

BOREALIS_NAMESPACE_END
#endif

