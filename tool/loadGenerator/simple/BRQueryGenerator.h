#ifndef BRQueryGenerator_H
#define BRQueryGenerator_H

#include <common.h>
#include "BRQueryParams.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * The BRQueryGenerator class generate XML query graphs
 */
class BRQueryGenerator
{
public:
    BRQueryGenerator(BRQueryParams& params):
      _params(params),
      _box_index(1),
      _stream_index(1),
      _filter_field_index(0)
    {
    }

    string get_query_xml();

private:
    typedef enum
    {
      BT_FILTER = 0,
      BT_UNION,
      BT_DELAY
    } BoxType;

    /*
     * A prefix for all names
     */
    static const string PREFIX;


    /*
     * The schema name
     */
    static const string SCHEMA_NAME;

    /*
     * Generatat one schema for the whole query network
     * @param number_of_fields number of fields in the schema
     * (all fields are ints)
     */
    void generate_schema(int number_of_fields);

    /*
     * Gernerate a chain of operators (now all union)
     * @param number_of_boxes number of boxes in the chain
     */
    bool generate_query();

    /*
     * Gernerate a chain of operators (now all union)
     * @param number_of_boxes number of boxes in the chain
     */
    bool generate_tree(
      const string& input_stream_name,
      vector<string>& output_stream_names);

    /*
     * Generate IO streams
     */
    void generate_iostreams();

    /*
     */
    void generate_input_stream(const string& name);

    /*
     */
    void generate_output_stream(const string& name);

    /*
     * Generate a box
     * @param type type of the box
     * @param input_stream_name the names of the input streams
     * @param number_of_output_streams_per_port number of output streams
     * per each output port
     */
    void generate_sisobox(
        const string& input_stream_name,
        const string& output_stream_name);

    /*
     * Generate a box
     * @param type type of the box
     * @param input_stream_name the names of the input streams
     * per each output port
     */
    void generate_box(
        BoxType type,
        const vector<string>& input_stream_name,
        const string& output_stream_name);

    string get_next_box_name();

    string get_next_stream_name()
    {
      ostringstream oss; oss << "s" << _stream_index; ++_stream_index;
      return oss.str();
    }

    /*
     * The parameters used to generate the queries
     */
    BRQueryParams& _params;

    /*
     * Index for box name
     */
    int _box_index;

    /*
     * Index for box name
     */
    int _stream_index;

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
     * Outout stream names
     */
    vector<string> _input_stream_names;
    vector<string> _output_stream_names;

    /*
     * The index for filter field
     */
    int _filter_field_index;

};

BOREALIS_NAMESPACE_END
#endif

