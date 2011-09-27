
#include <string>

#define  INIT -1

using namespace std;

void usage(const char* prog_name)
{
    cout << prog_name << endl
         << "  [-p update_period(msec)]" << endl
         << "  [-d region_server_port]" << endl
         << "  [-s load_shedding_threshold]" << endl
         << "  [-i load_distribution_ignore_node_name]" << endl
         << "  [-w warmup_delay(sec)]" << endl
         << "  [-m debugging method (0: no action, 1: load shedding 2: load balancing 3: HA)]"
         << endl
         << "  [-q load_shedding_quality_metric (0: throughput, 1: tuple_delivery, 2: value)]"
         << endl
         << "  [-t runtime(msec)]" << endl
         << endl;
    exit(0);
}

void stdArgs(int argc, const char *argv[], int &period,
             int &region_server_port, int &debugging_action,
             double& load_shedding_threshold,
             double& warmup_delay,
             string& load_distribution_ignore_node_name,
             int &load_shedding_quality_metric,
             int &runtime)
{
    const char *argv_copy[argc];

    for (int i = 0; i < argc; i++)
    {
        argv_copy[i] = argv[i];
    }

    optind = 0;
    opterr = 0;

    int option_char;

    while ((option_char = getopt(argc, (char**)argv,"?hp:d:m:i:s:w:q:t:")) 
           != EOF)
    {
        switch (option_char)
        {
            case 'p':
                period = atoi(optarg);
                break;
            case 'd':
                region_server_port = atoi(optarg);
                break;
            case 'm':
                debugging_action = atoi(optarg);
                break;
            case 'i':
                load_distribution_ignore_node_name = optarg;
                break;
            case 's':
                load_shedding_threshold = atof(optarg);
                break;
            case 'w':
                warmup_delay = atof(optarg);
                break;
            case 'h':
                usage(argv[0]);
                break;
            case 'q':
                load_shedding_quality_metric = atoi(optarg);
                break;
            case 't':
                runtime = atoi(optarg);
                break;
            default:
                break;
        }
    }

    optind = 0;
}

