
#include "LoadSheddingControl.h"
#include "MedusaClient.h"
#include <NMSTL/xmlrpc>
#include <NMSTL/tcprpc>

#define DEFAULT_INITIAL_WARM_UP    45000 // msec
#define DEFAULT_OFFLINE_DURATION   60000 // msec
#define DEFAULT_PERCENT_FIT_ERROR  10
#define DEFAULT_LS_PERIOD          10000 // msec
#define DEFAULT_QUALITY_METRIC     0     // throughput       
#define DEFAULT_MODE               0     // plan-based
#define DEFAULT_REGION_SERVER_PORT 40000
#define INIT                       -1

using namespace Borealis;
using namespace NMSTL;

void usage(const char* prog_name)
{
    cout << prog_name << endl
         << "  [-d region_server_port]" << endl
         << "  [-p ls_period (msec)]" << endl
         << "  [-q ls_quality_metric]" << endl
         << "  [-m ls_mode]" << endl
         << "  [-w warmup_time (msec)]" << endl
         << "  [-o offline_duration (msec)]" << endl
         << "  [-e percent_fit_error]" << endl
         << "  [-t run_time (msec)]" << endl
         << endl;
    exit(0);
}

void stdArgs(int argc, const char *argv[],
             int &region_server_port,
             int &ls_period,
             int &ls_quality_metric,
             int &ls_mode,
             int &warmup_time,
             int &offline_duration,
             int &percent_fit_error,
             int &run_time)
{
    const char *argv_copy[argc];

    for (int i = 0; i < argc; i++)
    {
        argv_copy[i] = argv[i];
    }

    optind = 0;
    opterr = 0;

    int option_char;

    while ((option_char = getopt(argc, (char**)argv,"?hd:p:q:m:w:o:e:t:")) 
           != EOF)
    {
        switch (option_char)
        {
            case 'h':
                usage(argv[0]);
                break;
            case 'd':
                region_server_port = atoi(optarg);
                break;
            case 'p':
                ls_period = atoi(optarg);
                break;
            case 'q':
                ls_quality_metric = atoi(optarg);
                break;
            case 'm':
                ls_mode = atoi(optarg);
                break;
            case 'w':
                warmup_time = atoi(optarg);
                break;
            case 'o':
                offline_duration = atoi(optarg);
                break;
            case 'e':
                percent_fit_error = atoi(optarg);
                break;
            case 't':
                run_time = atoi(optarg);
                break;
            default:
                break;
        }
    }

    optind = 0;
}

int main(int argc, const char* argv[])
{
    int region_server_port = DEFAULT_REGION_SERVER_PORT;
    int ls_period          = DEFAULT_LS_PERIOD;
    int ls_quality_metric  = DEFAULT_QUALITY_METRIC;
    int ls_mode            = DEFAULT_MODE;
    int warmup_time        = DEFAULT_INITIAL_WARM_UP;
    int offline_duration   = DEFAULT_OFFLINE_DURATION;
    int percent_fit_error  = DEFAULT_PERCENT_FIT_ERROR;
    int run_time           = INIT;

    stdArgs(argc, argv, 
            region_server_port, 
            ls_period, 
            ls_quality_metric, 
            ls_mode, 
            warmup_time,
            offline_duration,
            percent_fit_error,
            run_time);

    ptr<LoadSheddingControl> ls_control;

    ls_control.set(new LoadSheddingControl(region_server_port,
                                           ls_period,
                                           ls_quality_metric,
                                           ls_mode,
                                           warmup_time,
                                           offline_duration,
                                           percent_fit_error,
                                           run_time));

    ls_control->start_load_shedder();

    ls_control->start_region_server();

    NOTICE << "Terminating the LoadSheddingTool...";
    
    return 0;
}
