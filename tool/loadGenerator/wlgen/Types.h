#ifndef Types_H
#define Types_H

#include <sys/types.h>          // for u_int32_t
#include <list>             // for list
#include <map>              // for pair
#include <vector>           // for vector
#include <string>           // for string
#include <common.h>

BOREALIS_NAMESPACE_BEGIN


typedef int source_stream_id_t;
typedef vector<int> stream_ids_t;
typedef vector<source_stream_id_t> stream_ids_t;
typedef vector<source_stream_id_t>::iterator stream_ids_iter_t;

const int MAX_STRING_ID_LEN = 128;

struct AggStreamID
{
	source_stream_id_t   numberID;
	char         stringID[MAX_STRING_ID_LEN];
};

typedef struct AggStreamID agg_stream_id_t;

typedef enum
{
	INTEGER,
	FLOAT,
	DOUBLE,
	STRING
} attr_type_t;

struct DatatypeStruct
{
	attr_type_t  type;
	int      length;
};

typedef struct DatatypeStruct datatype_t;

typedef enum
{
	CONSTANT,
	UNIFORM,
	NORMAL,
	EXPONENTIAL,
	LINEAR,
	RANDOMWALK,
	RANDOMDIRECTIONWALK,
	TIMESTAMP,
	INPUTFILE,
	FILENAME,
	OTHER
} distributiontype_t;

typedef list<double> parlist_t;
typedef list<double>::iterator parlist_it_t;
typedef pair<distributiontype_t, parlist_t> distribution_t;


typedef vector<datatype_t> datatype_list_t;
typedef map<source_stream_id_t, datatype_list_t> schema_t;

const int MAX_PARAMETER_NUMBER = 20;
const int MAX_DISTRIBUTION_TYPE_NUMBER = 20;
const int MAX_ATTRIBUTE_NUMBER = 1000;
const int MAX_DATA_TYPES = 10;
const int NEVER_USE_FOR_NUMBER_ID = -1000000;
const char NEVER_USE_FOR_STRING_ID[] = "STRING ID UNSPECIFIED";
const int MAX_TUPLE_SIZE = 1024 * 1024;

BOREALIS_NAMESPACE_END
#endif
