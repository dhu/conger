#ifndef ARGS_H
#define ARGS_H

#include <iostream>
#include <vector>
// For getopt
#include <unistd.h>

using namespace std;

#define INIT -1

// --------------------------------------------------
void parse(const char* optarg, string& ip, int& port)
{

    string temp(optarg);
    unsigned int pos = temp.find(":",0);
    if (pos != string::npos && pos > 0)
        ip = temp.substr(0,pos);
    port = atoi( temp.substr(pos+1,temp.size()-pos-1).c_str());
}

// --------------------------------------------------
void parse(const char* optarg, int& add1, int& add0)
{

    string temp(optarg);
    unsigned int pos = temp.find(".",0);
    if (pos != string::npos && pos > 0)
        add1 = atoi( temp.substr(0,pos).c_str() ) ;
    add0 = atoi( temp.substr(pos+1,temp.size()-pos-1).c_str());
}


// --------------------------------------------------
/// Parses one ip at the time and replaces
void parse_ips(const char* optarg, vector< pair<string, int> >& addresses)
{

    string ip;
    int port;
    int i = 0;
    char* index = strtok((char*)optarg," ");
    while ( (index != NULL) && (i < (int)addresses.size()))
    {
        parse(index,ip,port);
        addresses[i++] = pair<string,int>(ip,port);
        index = strtok(NULL," ");
    }
}


// --------------------------------------------------
void usage(const char* prog_name)
{

    cout << prog_name << " " << "[-d [medusaIP]:[medusaPort]] [-t runtime(msec)]\n";
    exit(0);
}


// --------------------------------------------------
/**
 * The standard arguments for applications are:
 * -d ip:port    of the borealis node to talk to
 * -t time_msecs the time the application should run before stopping itself
 */
void stdArgs(const int argc, const char* argv[], string& ip, int& port, int* run_time = NULL)
{

    // Getopt will modify argv so we have to make a copy. It won't modify the content, though,
    // just the position of the arguments. So we make a shallow copy
    const char* argv_copy[argc];
    for ( int i = 0; i < argc; i++)
    { argv_copy[i] = argv[i];
    }

    int option_char;
    string options("hd:t:");

    optind = 0; // Reset getopt
    opterr = 0; // Ignore any unknown options rather than return error
    while ( (option_char = getopt (argc,(char**)argv_copy,options.c_str())) != EOF )
    {
        switch (option_char)
        {
        case 'd':
            parse(optarg,ip,port);
            break;
        case 't':
            if ( run_time )
                (*run_time) = atoi(optarg);
            break;
        case 'h':
            usage(argv[0]);
            break;
        default:
            // do nothing
            break;
        }
    }

    optind = 0; // Reset getopt if user want to do some additional parsing of attributes

}


// --------------------------------------------------
/**
 * It is often convenient for client apps to use an option to set their own ip and port
 * -a my_ip:my_port
 */
void client_args(const int argc, const char* argv[], string& my_ip, int& my_port)
{
    // Getopt will modify argv so we have to make a copy. It won't modify the content, though,
    // just the position of the arguments. So we make a shallow copy
    const char* argv_copy[argc];
    for ( int i = 0; i < argc; i++)
    { argv_copy[i] = argv[i];
    }

    string extra_args("a:");
    int option_char;
    optind = 0; // Reset parser
    opterr = 0; // Ignore unknown options
    while ( (option_char = getopt (argc,(char**)argv_copy,extra_args.c_str())) != EOF )
    {
        switch (option_char)
        {
        case 'a':
            parse(optarg,my_ip,my_port);
            break;
        default:
            // do nothing
            break;
        }
    }

    optind = 0;
}

#endif
