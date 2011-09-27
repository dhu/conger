#include <parseutil.h>
#include <Exceptions.h>
#include <sstream>

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
//  Split space separated names into a vector.
//
void  split_name(const  string   &source,
                 vector<Name>    &list)
{
    string::size_type  index = 0;
    string::size_type  space;
//
//  Lead, trailing and multiple spaces are permitted.
//..............................................................................


    list.clear();

    if (!source.empty())
    {   space = source.find(" ", index);

        while (space != string::npos)
        {   if (space != index)
            {   list.push_back(source.substr(index, space - index));
            }

            index = space + 1;
            space = source.find(" ", index);
        }

        if (index < source.length())
        {   // The substring length is truncated to the end of the string.
            //
            list.push_back(source.substr(index, source.length()));
        }
    }

    return;
}
 

//===============================================================================

bool string_to_bool(const string & s)
              throw(exception)
{
    if ((s == "T") ||
        (s == "t") ||
        (s == "TRUE") ||
        (s == "true"))
        {
            return true;
        }

    if ((s == "F") ||
        (s == "f") ||
        (s == "FALSE") ||
        (s == "false"))
        {
            return false;
        }

    ostringstream os;
    os << "Can't parse \"" << s << "\" as a bool";
    Throw(AuroraException, os.str());
}

//===============================================================================

unsigned int string_to_uint(const string & s)
  throw (exception)
{
    unsigned int return_val;

    {
        istringstream iss(s.c_str());
        iss >> return_val;

        if ((iss.fail()) || (! iss.eof()) || (iss.bad()))
            {
                ostringstream os;
                os << "Tried to parse as an unsigned int but we couldn't: \"" << s << "\"";
                Throw(AuroraException, os.str());
            }
    }

    return return_val;

    /*
  char * endPtr;
  unsigned long ulongVal = strtoul(s.c_str(), & endPtr, 10);

  if (*endPtr != '\0')
    {
      ostringstream os;
      os << "Tried to parse as an unsigned int, but we couldn'tL \"" << s << "\"";
      Throw(AuroraException, os.str());
    }

  if ((ulongVal < numeric_limits<unsigned int>::min()) ||
      (ulongVal > numeric_limits<unsigned int>::max()))
    {
      ostringstream  os;
      os << "Tried to parse as an unsigned int, but it was out of range: \""
         << s << "\"";
      Throw(AuroraException, os.str());
    }

  // I'd like to have this as a single return statement, but new style C++
  // casting:   type-name(value) doesn't really work when type-name has
  // qualifiers such as 'unsigend'. Such a scenario trigger's g++'s
  // "old-style cast" warning... -cjc
  typedef unsigned int uint;

  return uint(ulongVal);
    */
}

//===============================================================================

long string_to_long(const string & s)
  throw (exception)
{
  long long_val;

  {
    istringstream iss(s.c_str());
    iss >> long_val;

    if ((iss.fail()) || (! iss.eof()) || (iss.bad()))
      {
      ostringstream os;
      os << "Tried to parse as a long, but we couldn't: \"" << s << "\"";
      Throw(AuroraException, os.str());
      }
  }

  return long_val;
}

//===============================================================================

unsigned long string_to_ulong(const string & s)
  throw (exception)
{
  unsigned long ulong_val;

  {
    istringstream iss(s.c_str());
    iss >> ulong_val;

    if ((iss.fail()) || (! iss.eof()) || (iss.bad()))
      {
      ostringstream os;
      os << "Tried to parse as an unsigned long, but we couldn't: \"" << s << "\"";
      Throw(AuroraException, os.str());
      }
  }

  return ulong_val;
}

//===============================================================================

unsigned long long string_to_ulong_long(const string & s)
  throw (exception)
{
    unsigned long long ull_val;

    {
        istringstream iss(s.c_str());
        iss >> ull_val;

        if ((iss.fail()) || (! iss.eof()) || (iss.bad()))
            {
                ostringstream os;
                os << "Tried to parse as an unsigned long long, but we couldn't: \"" << s << "\"";
                Throw(AuroraException, os.str());
          }
    }

    return ull_val;
}

//===============================================================================

double string_to_double(const string & s)
  throw (exception)
{
  istringstream iss(s.c_str());

  double v;
  iss >> v;

  if ((iss.fail()) || (! iss.eof()) || (iss.bad()))
    {
      ostringstream os;
      os << "Tried to parse \"" << s << "\" as a double, but parsing failed.";
      Throw(AuroraException, os.str());
    }

  return v;
}

//===============================================================================

size_t string_to_size_t(const string & s)
  throw (exception)
{
  istringstream iss(s.c_str());

  size_t v;
  iss >> v;

  if ((iss.fail()) || (! iss.eof()) || (iss.bad()))
    {
      ostringstream os;
      os << "Tried to parse \"" << s << "\" as a size_t, but parsing failed.";
      Throw(AuroraException, os.str());
    }

  return v;
}

//===============================================================================

int string_to_int(const string & s)
  throw (exception)
{
  istringstream iss(s.c_str());

  int v;
  iss >> v;

  if ((iss.fail()) || (! iss.eof()) || (iss.bad()))
    {
      ostringstream os;
      os << "Tried to parse \"" << s << "\" as a int, but parsing failed.";
      Throw(AuroraException, os.str());
    }

  return v;
}

BOREALIS_NAMESPACE_END
