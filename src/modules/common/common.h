#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <queue>
#include <list>
#include <vector>
#include <NMSTL/ptr>
#include <NMSTL/debug>
#include <NMSTL/serial>
#include <NMSTL/ntime>

#include "config.h"

// Braindead autoconf
#undef const

// This file contains default values for various parameters used in Borealis

#define INIT -1

#define SMALL_BUF 128
#define BIG_BUF 16384
#define HUGE_BUF 520000
//#define HUGE_BUF (20*4800)

#define MSECS 1000
#define USECS 1000000

//#define TUPLE_PROCESSING_LIMIT 500
#define TUPLE_PROCESSING_LIMIT 10   // -- tatbul
//#define TUPLE_PROCESSING_LIMIT 100    // -- magda

#ifdef  DOXYGEN_SKIP
#define BOREALIS_NAMESPACE_BEGIN
#define BOREALIS_NAMESPACE_END
#else
#define BOREALIS_NAMESPACE_BEGIN namespace std {}; namespace NMSTL {}; namespace Borealis { using namespace ::std; using namespace ::NMSTL;
#define BOREALIS_NAMESPACE_END };
#endif


// For legacy Medusa code to still work
#define MEDUSA_NAMESPACE_BEGIN  BOREALIS_NAMESPACE_BEGIN
#define MEDUSA_NAMESPACE_END    BOREALIS_NAMESPACE_END


BOREALIS_NAMESPACE_BEGIN

#define  DEFAULT_BOREALIS_PORT  15000
#define  DEFAULT_MONITOR_PORT   25000
#define  DEFAULT_HEAD_PORT      35000
#define  DEFAULT_REGION_PORT    45000
#define  DEFAULT_GLOBAL_PORT    55000

// xml rpc
#define XML_RPC_PORT_OFFSET 1

// Fast data path
#define DATA_PORT_OFFSET 2

// Checks to see that ouput has not run away in DataHandler::send_data
// This limit may be exceeded for one StreamEvent (batch of tuples),
// but not for several.
//
#define MAX_SOCKET_BUFFER 64000

// HA default values can be overridden in config file
#define DEFAULT_NODE_MONITOR_INTERVAL 1000
#define DEFAULT_NODE_MONITOR_START 5000

// Load management
#define STATS_UPDATE_INTERVAL 0
//#define STATS_UPDATE_INTERVAL 10000 // -- tatbul
#define PERFMON_INTERVAL 1000
#define SHED_LOAD_PERIODS 7
//#define PER_TUPLE_COST 0.00002 For idsgen queries
#define PER_TUPLE_COST 0.00008
// Note: A unit of load is one percent of CPU or one percent of MAX_BW
#define MAX_BW 1000000
#define MAX_UTILIZATION 0.99

//  These portability declarations need to be changed depending on the compiler.
//  This version works with gcc.
//
typedef    signed char            int8;
typedef    signed short           int16;
typedef    signed long            int32;
typedef    signed long long int   int64;

typedef  unsigned char            uint8;
typedef  unsigned short           uint16;
typedef  unsigned long            uint32;
typedef  unsigned long long int   uint64;

typedef           float           single;
typedef           char            Boolean;

#define  True     1
#define  False    0


// Declare a 64 bit integer constant (e.g. x = L64(0xFFFF0000FFFF0000)).
//
#define  L64(constant)      constant##ULL


// Print format for a 64 bit integer(e.g.  printf( "%18" I64(x) "\n", i )).
//
#define  I64(format)        "L" #format

// added by Jeong-Hyon. Those functions print numbers in a pretty format.
string get_formatted_number(double number, int width, int precision);

string get_formatted_number(double number, int precision);

string get_formatted_number(int number, int width);

BOREALIS_NAMESPACE_END

#endif                      // COMMON_H
