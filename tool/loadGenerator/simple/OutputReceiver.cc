#include <common.h>
#include "BROutputReceiver.h"
#include "BRIOParams.h"
using namespace std;
using namespace Borealis;

string deployment_file_name = "deploy.xml";
string address = "127.0.0.1:60000";
double statistics_window_size = 1.0;
string param_file_name;
string output_file_prefix;
string output_config_file_name;

void usage(const char* prog_name)
{
    cout << prog_name << endl
         << "  [-d region_server_address(ip:port)]" << endl
         << "  [-f io_config_file_name]" << endl
         << "  [-p param_file_name]" << endl
         << "  [-o output_file_prefix]" << endl
         << "  [-r random_seed]" << endl
         << "  [-s statistics_window_size]" << endl
         << "  [-w output_config_file_name]" << endl
         << "  [-z path_of_the_source_files]" << endl
         << "  [-m rate_multiplier_filename]" << endl
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

    while ((option_char = getopt(argc, (char**)argv, "?hd:p:m:f:r:w:o:s:z:")) != EOF)
    {
        switch (option_char)
        {
            case 'f':
                deployment_file_name = optarg;
                break;
            case 'd':
                address = optarg;
                break;
            case 'm':
                BRIOParams::rate_multiplier_file_name = optarg;
                break;
            case 'p':
                param_file_name = optarg;
                break;
            case 'o':
                output_file_prefix = optarg;
                break;
            case 'r':
                srand48(atoi(optarg));
                break;
            case 's':
                statistics_window_size = atof(optarg);
                break;
            case 'w':
                output_config_file_name = optarg;
                break;
            case 'z':
                BRIOParams::input_generator_path = optarg;
                break;
            case 'h':
                usage(argv[0]);
                break;
            default:
                usage(argv[0]);
                break;
        }
    }
}


int main(int argc, const char* argv[])
{
    stdArgs(argc, argv);

    cout << "param_file_name " << param_file_name << endl;
    if (param_file_name != "")
    {
        ifstream ifs;
        ifs.open(param_file_name.c_str(), ios::in);
        if (!ifs)
        {
            WARN << "Can not open file " << param_file_name;
        }
        else
        {
            ifs >> *(BRIOParams::get_singleton());
        }
        ifs.close();
    }

    if (output_config_file_name != "")
    {
        ofstream ofs;
        ofs.open(output_config_file_name.c_str(), ios::out);
        if (!ofs)
        {
            WARN << "Can not open file " << output_config_file_name;
        }
        else
        {
            ofs << *(BRIOParams::get_singleton());
        }
        ofs.close();
        exit(0);
    }

    cout << *(BRIOParams::get_singleton());

    NOTICE << "Starting Ourput Reciever on: " << address;

    BROutputReceiver output_receiver(
            deployment_file_name,
            InetAddress(address),
            statistics_window_size,
            output_file_prefix);
    output_receiver.join();
}
