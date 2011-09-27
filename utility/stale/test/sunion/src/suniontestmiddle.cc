#include "suniontest.h"

using namespace Borealis;
using namespace NMSTL;

// --------------------------------------------------
class SUnionTestMiddle : public SUnionTestInBase
{

public:
    SUnionTestMiddle(int argc, const char* argv[])
        : SUnionTestInBase(argc,argv) {}
    void start();


};

// --------------------------------------------------
void
SUnionTestMiddle::start()
{

    // If I'm node nb 2, then I use the official output stream
    string input_stream = "medusa://nms.lcs.mit.edu/uniontest/query/output";

    string middle_base = "medusa://nms.lcs.mit.edu/uniontestmiddle/query/output/";

    // If I'm a subsequent node in a chain then I use the output stream of previous middle app
    if ( _info._nb_nodes > 2 )
    {
        input_stream = middle_base + (_info._nb_nodes-1);
    }

    vector<string> stream_names;
    stream_names.push_back(input_stream);

    string output_stream = middle_base + _info._nb_nodes;
    string query_name = string("medusa://nms.lcs.mit.edu/uniontestmiddle/query/") + _info._nb_nodes;

    create_query(query_name,stream_names,output_stream);

}


// --------------------------------------------------
int main(int argc, const char* argv[])
{

    SUnionTestMiddle client(argc,argv);
    client.start();

}
