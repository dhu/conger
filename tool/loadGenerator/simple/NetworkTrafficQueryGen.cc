#include <fstream>
#include "NetworkTrafficQueryGenerator.h"
#include "NetworkTrafficQueryParams.h"
using namespace std;
using namespace Borealis;


string query_config_file_name;
string output_xml_file_name;
string output_config_file_name;

void usage(const char* prog_name)
{
   cout << prog_name << endl
        << "  [-f query_config_file_name]" << endl
        << "  [-o output_xml_file_name]" << endl
        << "  [-c output_config_file_name]" << endl
        << "  [-r random_seed]" << endl
        << endl;
   exit(0);
}

void stdArgs(int argc, const char *argv[])
{
    const char *argv_copy[argc];

    for (int i = 0; i < argc; i++)
    {
        argv_copy[i] = argv[i];
    }

    optind = 0;
    opterr = 0;

    int option_char;

    while ((option_char = getopt(argc, (char**)argv, "?hc:f:o:r:")) != EOF)
    {
        switch (option_char)
        {
            case 'f':
                query_config_file_name = optarg;
                break;
            case 'o':
                output_xml_file_name = optarg;
                break;
            case 'c':
                output_config_file_name = optarg;
                break;
            case 'r':
                srand48(atoi(optarg));
                break;
            default:
                usage(argv[0]);
                break;
        }
    }
}


int main(int argc, const char* argv[])
{
    stdArgs(argc,argv);

    if (query_config_file_name != "")
    {
        fstream ifs;
        ifs.open(query_config_file_name.c_str(), ios::in);
        if (!ifs)
        {
            WARN << "Can not open query config file " << query_config_file_name;
        }
        else
        {
            ifs >> *(NetworkTrafficQueryParams::get_singleton());
        }
        ifs.close();
    }

    if (output_config_file_name != "")
    {
        fstream ofs;
        ofs.open(output_config_file_name.c_str(), ios::out);
        if (!ofs)
        {
            WARN << "Can not open output config file " << output_config_file_name;
        }
        else
        {
            ofs << *(NetworkTrafficQueryParams::get_singleton());
        }
        ofs.close();
    }

    NetworkTrafficQueryGenerator bq;

    if (output_xml_file_name != "")
    {
        fstream ofs;
        ofs.open(output_xml_file_name.c_str(), ios::out);
        if (!ofs)
        {
            WARN << "Can not open output config file " << output_xml_file_name;
        }
        else
        {
            ofs << bq.get_query_xml();
        }
        ofs.close();
    }
    else
    {
        cout << bq.get_query_xml();
    }

}
