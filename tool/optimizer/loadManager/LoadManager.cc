#include <rpc_QueryProcessor.h> // have to include this file before other headers
#include <common.h>
#include "BorealisController.h"
#include "LMParams.h"
using namespace std;
using namespace Borealis;
using namespace NMSTL;

double period = LMParams::time_param.load_distribution_interval;
string region_server_address = "127.0.0.1:40000";
string config_filename;
string output_config_filename;
bool start_input_reader = false;

void usage(const char* prog_name)
{
   cout << prog_name << endl
        << "  [-d region_server_address(ip:port)]" << endl
        << "  [-f config_file_name]" << endl
        << "  [-i 1: start input reader; 0: no input reader]" << endl
        << "  [-o output_config_filename]" << endl
        << "  [-p update_period(sec)]" << endl
        << "  [-r random_seed]" << endl
        << "  [-s status_filename]" << endl
        << "  [-t ouput direcory]" << endl
        << "  [-u ouput prefix]" << endl
        << "  [-v load manager path]" << endl
        << "  [-w load distribution starting delay]" << endl
        << "  [-n number of boxes to move (for box migration test)]" << endl
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

   while ((option_char = getopt(argc, (char**)argv, "?ha:p:d:f:i:n:o:r:s:t:u:v:w:")) != EOF)
   {

       switch (option_char)
       {
           case 'd':
               region_server_address = optarg;
               break;
           case 'f':
               config_filename = optarg;
               break;
           case 'h':
               usage(argv[0]);
               break;
           case 'i':
               start_input_reader = (bool)atoi(optarg);
               break;
           case 'n':
               LMParams::runtime_param.number_of_boxes_to_move = atoi(optarg);
               break;
           case 'o':
               output_config_filename = optarg;
               break;
           case 'p':
               period = atof(optarg);
               break;
           case 'r':
                srand48(atoi(optarg));
                break;
           case 's':
               LMParams::runtime_param.status_filename = optarg;
               break;
           case 't':
               LMParams::runtime_param.output_dir = optarg;
               break;
           case 'u':
               LMParams::runtime_param.output_prefix = optarg;
               break;
           case 'v':
               LMParams::runtime_param.load_manager_path = optarg;
               break;
           case 'w':
               LMParams::time_param.load_distribution_starting_delay =
                 atof(optarg);
               break;
           default:
               break;
       }
   }

   optind = 0;
}


int main(int argc, const char* argv[])
{

    stdArgs(argc,argv);

    // read in LMParams
    if (config_filename != "")
    {
        ifstream ifs;
        ifs.open(config_filename.c_str(), ios::in);
        if (!ifs)
        {
            cerr << "Can't open config file: " << config_filename << endl;
            exit(1);
        }
        LMParams::read_params(ifs);
        ifs.close();
    }

    stdArgs(argc,argv);

    // output LMParams
    if (output_config_filename != "")
    {
        ofstream ofs;
        ofs.open(output_config_filename.c_str(), ios::out);
        if (!ofs)
        {
            cerr << "Can't open output config file: " << output_config_filename << endl;
            exit(1);
        }
        LMParams::output_params(ofs);
        exit(0);
    }

    NOTICE << "LoadManager Parameters used: ";
    LMParams::output_params(cout);
    cout << endl;


        ptr<BorealisController> controller;
        NOTICE << "Initializing the BorealisController...";
        controller.set(new BorealisController(period, region_server_address));


        // Start the region server, which will start the event loop.
        //
        if (start_input_reader)
            controller->start_input_reading();
        controller->start_load_manager();
        controller->start_region_server();

        NOTICE << "Terminating the BorealisController...";
        return 0;
}

