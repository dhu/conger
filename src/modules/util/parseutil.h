#ifndef PARSEUTIL_H
#define PARSEUTIL_H

#include <string>
#include <exception>
#include <vector>
#include <cctype>              // for toupper
#include <algorithm>

#include "common.h"
#include "Name.h"

#define lowercase(s)  transform( s.begin(), s.end(),                 \
                                 s.begin(), (int(*)(int))tolower );


BOREALIS_NAMESPACE_BEGIN


//  Split space separated names into to a vector.
//
void  split_name(const  string  &source,
                 vector<Name>   &list);


// The following functions attempt to parse the specified string to a particular
// data type. The functions throws an exception if the parse is unsuccessful.
bool string_to_bool(const string & s)
  throw (exception);

size_t string_to_size_t(const string & s)
  throw (exception);

int string_to_int(const string & s)
  throw (exception);

unsigned int string_to_uint(const string & s)
  throw (exception);

long string_to_long(const string & s)
  throw (exception);

unsigned long string_to_ulong(const string & s)
  throw (exception);

unsigned long long string_to_ulong_long(const string & s)
  throw (exception);

double string_to_double(const string & s)
  throw (exception);

BOREALIS_NAMESPACE_END

#endif
