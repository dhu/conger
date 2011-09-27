#ifndef CONFIG_H
#define CONFIG_H

#include <sys/types.h>
#include "DataType.h"

typedef unsigned char   BYTE;
typedef int16_t         INT_16;
typedef int32_t         INT_32;
typedef int64_t         INT_64;

typedef enum schema_type     {st_invalid, st_stream, st_relation};
typedef enum usage_type      {ut_invalid, ut_permanent, ut_regenerate};
typedef enum wrapper_state   {ws_invalid, ws_initialized};
typedef enum QueryState     {qs_invalid, qs_prepared, qs_running, qs_stopped};
typedef enum operator_type   {ot_filter, ot_map, ot_union, ot_bsort, ot_aggregate, ot_join, ot_resample, ot_insert, ot_select, ot_update};

// socket commands
#define CMD_PREPQUERY   101
#define CMD_RUNQUERY    102
#define CMD_STOPQUERY   104
#define CMD_SETEPOCHDUR 105
#define CMD_GETCATALOG  110
#define CMD_EXIT        100

// settings of listening sockets
#define DATATHREAD_FIRST_PORT   11111
#define DATATHREAD_NO_OF_PORTS  15

// metrics
#define ESTIMATE_TRANS_LEFT     10000
#define ESTIMATE_TRANSMISSIONS  .1
#define ESTIMATE_TRANS_QUERY    5
#define ESTIMATE_LATENCY        0.1
#define ESTIMATE_THROUGHPUT     5
#define ESTIMATE_SENT           5
#define ESTIMATE_RECEIVED       4.9
#define ESTIMATE_JOINSEL        0.5

// score functions
#define WEIGHT_LIFETIME         1.75
#define WEIGHT_LATENCY          0.75
#define WEIGHT_THROUGHPUT       0.5
#define WEIGHT_QUALITY          0.75

#endif /* CONFIG_H */
