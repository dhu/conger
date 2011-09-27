#include "BRQueryGenerator.h"
#include <sstream>

BOREALIS_NAMESPACE_BEGIN

const string BRQueryGenerator::SCHEMA_NAME = "intstuple";
const string INDENT = "    ";

string BRQueryGenerator::get_query_xml()
{
    //generateSchema(sizeof(TupleData) / sizeof(int));
    generate_schema(_params.number_of_fields);
    while (generate_query())
    {
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

void BRQueryGenerator::generate_schema(int number_of_fields)
{
    _schema << INDENT << "<schema name=\"" << SCHEMA_NAME << "\">" << endl;

// not need to have timestamp field anymore
//    _schema << INDENT << INDENT << "<field  name=\"tm\"" 
//                        << INDENT << "type=\"int\" />" << endl;

    for(int i = 0; i < number_of_fields; ++i)
    {
        _schema << INDENT << INDENT << "<field  name=\"f" << i+1 << "\""
                        << INDENT << "type=\"int\" />" << endl;
    }

    _schema << INDENT << "</schema>" << endl;
}

void BRQueryGenerator::generate_iostreams()
{
    for (unsigned int i = 0; i < _input_stream_names.size(); ++i)
    {
        generate_input_stream(_input_stream_names[i]);
    }
    for (unsigned int i = 0; i < _output_stream_names.size(); ++i)
    {
        generate_output_stream(_output_stream_names[i]);
    }
}


void BRQueryGenerator::generate_input_stream(const string& name)
{
    _input_streams << INDENT << "<input  stream=\"" << name  << "\""
            << INDENT << "schema=\"" << SCHEMA_NAME << "\" />" << endl;
}

void BRQueryGenerator::generate_output_stream(const string& name)
{
    _output_streams << INDENT << "<output stream=\"" << name  << "\""
            << INDENT << "schema=\"" << SCHEMA_NAME << "\" />" << endl;
}

bool BRQueryGenerator::generate_tree(
        const string& input_stream,
        vector<string>& output_streams)
{
    vector<string> intermediate_streams;
    string input_stream_name = input_stream;


    int number_of_boxes_in_tree = 0;
    while (1)
    {
        string output_stream_name = get_next_stream_name();

        // check whether generate union
        if (drand48() < _params.prob_box_union && _output_stream_names.size() > 0)
        {
            // generate a union box
            // pick a random output stream as the other output
            int k = static_cast<int>(
                    drand48() * static_cast<double>(_output_stream_names.size()));
            vector<string>::iterator it = _output_stream_names.begin();
            for (int i = 0; i < k; ++i)
                ++it;

            vector<string> input_stream_names;
            input_stream_names.push_back(input_stream_name);
            input_stream_names.push_back(*it);
            generate_box(BT_UNION, input_stream_names, output_stream_name);

            _output_stream_names.erase(it);
        }
        else
        {
            generate_sisobox(input_stream_name, output_stream_name);
        }

        output_streams.push_back(output_stream_name);

        if (_box_index > _params.number_of_boxes)
            return false;

        double more_box = drand48();
        if (more_box < _params.prob_stop)
            return true;

        ++number_of_boxes_in_tree;
        if (number_of_boxes_in_tree == _params.max_number_of_boxes_per_query_tree)
            return true;

        double reuse_stream = drand48();
        if (!intermediate_streams.empty() &&
                reuse_stream < _params.prob_reuse_stream)
        {
            // resue an existing stream as input
            int i = static_cast<int>(drand48() * intermediate_streams.size());
            input_stream_name = intermediate_streams[i];
        }
        else
        {
            // use an output stream as input
            int i = static_cast<int>(drand48() * output_streams.size());
            input_stream_name = output_streams[i];
            intermediate_streams.push_back(input_stream_name);

            vector<string>::iterator it = output_streams.begin();
            for (int j = 0; j < i; ++j)
            {
                ++it;
            }
            output_streams.erase(it);
        }
    }
}

bool BRQueryGenerator::generate_query()
{
    string input_stream_name = get_next_stream_name();

    _input_stream_names.push_back(input_stream_name);

    vector<string> output_stream_name;
    bool more_query = generate_tree(input_stream_name, output_stream_name);

    _output_stream_names.insert(
            _output_stream_names.end(),
            output_stream_name.begin(),
            output_stream_name.end());

    return more_query;
}

void BRQueryGenerator::generate_sisobox(
        const string& input_stream_name,
        const string& output_stream_name)
{
    vector<string> input_streams;
    input_streams.push_back(input_stream_name);

    double type = drand48();
    if (type < _params.prob_box_delay)
    {
        generate_box(
                BT_DELAY,
                input_streams,
                output_stream_name);
    }
    else
    {
        generate_box(
                BT_FILTER,
                input_streams,
                output_stream_name);
    }
}

string BRQueryGenerator::get_next_box_name()
{
    ostringstream oss;
    oss << "b" ;
    if (_box_index < 10)
        oss << "00";
    else if (_box_index < 100)
        oss << "0";
    oss << _box_index;
    ++_box_index;
    return oss.str();
}

void BRQueryGenerator::generate_box(
        BoxType type,
        const vector<string>& input_stream_name,
        const string& output_stream_name)
{
    // set box name
    _boxes << INDENT << "<box name=\"" << get_next_box_name() << "\"";

    // set box type
    _boxes << INDENT << "type = \"";
    switch(type)
    {
        case BT_FILTER:
            _boxes << "filter";
            break;
        case BT_UNION:
            _boxes << "union";
            break;
        case BT_DELAY:
            _boxes << "delay";
            break;
        default:
            _boxes << "union";
    }
    _boxes << "\">" << endl;

    // set input streams
    for (unsigned int i = 0; i < input_stream_name.size(); ++i)
    {
        _boxes << INDENT << INDENT << "<in "
                << INDENT << "stream=\"" << input_stream_name[i] << "\" />" << endl;
    }

    // set ouput streams
    _boxes << INDENT << INDENT << "<out"
                << INDENT << "stream=\""
                << output_stream_name
                <<  "\" />"
                << endl;

    // set parameter for filter box
    if (type == BT_FILTER)
    {
        string compare;
        if (drand48() > 0.5)
            compare = "&gt;";
        else
            compare = "&lt;";

        int value = static_cast<int>(drand48() *
                (_params.filter_max - _params.filter_min) + _params.filter_min);

        _boxes << INDENT << INDENT << "<parameter name=\"expression.0\" value=\""
                << "f" << ++_filter_field_index << " " << compare << " "
                << value << "\" />" << endl;

        if (_filter_field_index == _params.number_of_fields)
            _filter_field_index = 0;
    }
    if (type == BT_DELAY)
    {
        double delay = 
                drand48() * (_params.delay_max - _params.delay_min) + 
                _params.delay_min;

        _boxes << INDENT << INDENT << "<parameter name=\"delay-ms\" value=\""
                <<  delay <<  "\" />" << endl;
        _boxes << INDENT << INDENT << "<parameter name=\"churn\" value=\""
                << "1" <<  "\" />" << endl;
        if (drand48() < _params.delay_selectivity_percentage)
        {
            double selectivity = 
                drand48() * (_params.delay_selectivity_max - 
                             _params.delay_selectivity_min) +
                _params.delay_selectivity_min;
            _boxes << INDENT << INDENT << "<parameter name=\"selectivity\" value=\""
                <<  selectivity <<  "\" />" << endl;
        }
    }

    _boxes << INDENT << "</box>" << endl;
}


BOREALIS_NAMESPACE_END
