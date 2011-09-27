#include "common.h"
#include "util.h"
#include "MedusaID.h"

#define  INIT -1

BOREALIS_NAMESPACE_BEGIN;

// --------------------------------------------------
void usage(const char* prog_name)
{
    cout << prog_name << " " 
         << "[-d [medusaIP]:[medusaPort]] "
         << "[-t runtime(msec)] "
         << "[-c checkpoint_method] " 
         << "[-i checkpoint_interval] "
         << "[-s checkpoint_slack] "
         << "[-p ha_partner] "
         << "[-l nh_load_shedder_mode]"
         << endl;
    exit(0);
}

// --------------------------------------------------
void parse(string temp, string& ip, int& port)
{
    unsigned int pos = temp.find(":",0);
    if (pos != string::npos && pos > 0)
    {
        ip = temp.substr(0,pos);
    }
    port = atoi(temp.substr(pos+1,temp.size()-pos-1).c_str());
}

// --------------------------------------------------
void parse(char* optarg, string& ip, int& port)
{
    parse(string(optarg), ip, port);
}

// --------------------------------------------------
void parse(char* optarg, int& add1, int& add0)
{
    string temp(optarg);
    unsigned int pos = temp.find(".",0);
    if (pos != string::npos && pos > 0)
    {
        add1 = atoi(temp.substr(0,pos).c_str()) ;
    }
    add0 = atoi(temp.substr(pos+1,temp.size()-pos-1).c_str());
}

// --------------------------------------------------
void stdArgs(int argc, const char* argv[], 
             string& ip, int& port,
             string& checkpoint_method, 
             int& checkpoint_interval, 
             int& checkpoint_slack, 
             vector<MedusaID>& ha_partners, 
             int* nh_load_shedder_mode,
             int* run_time = NULL)
{
    // Getopt will modify argv so we have to make a copy. 
    // It won't modify the content, though,
    // just the position of the arguments. So we make a shallow copy.
    //
    const char* argv_copy[argc];
    for (int i = 0; i < argc; i++)
    { 
        argv_copy[i] = argv[i];
    }

    optind = 0; // Reset getopt
    opterr = 0; // Ignore any unknown options rather than return error

    int option_char;
    string ha_partner;
    while ((option_char = getopt (argc,(char**)argv,"?hd:t:f:c:i:p:s:l:")) 
           != EOF)
    {
        switch (option_char)
        {
            case 'd':
                parse(optarg,ip,port);
                break;
            case 't':
                if (run_time)
                {
                    (*run_time) = atoi(optarg);
                }
                break;
            case 'c':
                checkpoint_method = string(optarg);
                break;
            case 'i':
                checkpoint_interval = atoi(optarg);
                break;
            case 's':
                checkpoint_slack = atoi(optarg);
                break;
            case 'p':
                ha_partner = string(optarg);
                if (ha_partner != "")
                {
                    string ha_partner_ip; 
                    int ha_partner_port;
                    parse(ha_partner, ha_partner_ip, ha_partner_port);
                    ha_partner = Util::get_host_address(ha_partner_ip) + ":"
                                 + to_string(ha_partner_port);// canonical form.
                    ha_partners.push_back(MedusaID(ha_partner));
                }
                break;
            case 'h':
                usage(argv[0]);
                break;
            case 'l':
                (*nh_load_shedder_mode) = atoi(optarg);
                break;
            default:
                // do nothing
                break;
        }
    }

    // Reset getopt if user want to do some additional parsing of attributes.
    //
    optind = 0; 
}

BOREALIS_NAMESPACE_END;
