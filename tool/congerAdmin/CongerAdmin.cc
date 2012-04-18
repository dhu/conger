/**
 * CongerAdmin.cc
 *
 *  Created on: Apr 16, 2012
 *      Author: hellojinjie
 */
#include "common.h"
#include "NMSTL/debug"
#include "NMSTL/ptr"
#include "MedusaClient.h"
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

using namespace Borealis;
using namespace std;

void start(string conger_string)
{
    ptr<MedusaClient> client = ptr<MedusaClient>(
            new MedusaClient(InetAddress("127.0.0.1", 15000)));
    client->add_conger_string(conger_string);
}

int main(int argc, char** argv)
{
    for (int i = 1; i < argc; i++)
    {
        struct stat file_stat;
        if (stat(argv[i], &file_stat) == -1)
        {
            continue;
        }
        if (file_stat.st_mode & S_IFMT != S_IFREG)
        {
            continue;
        }
        fstream is;
        is.open(argv[i], ios::in);
        char tmp[102400];
        is.read(tmp, 102400);
        start(string(tmp));
        cout << "send file: " << argv[i] << endl;
        return(EXIT_SUCCESS);
    }
    return(EXIT_FAILURE);
}


