#include <StorageMgr_Exceptions.h>
#include <sstream>
#include <iostream>

using namespace std;

//*********************************************************************************
// Exception class implementation
//*********************************************************************************

SmException::SmException(const string location,
             int line_num,
             const string description) :
  exception(),
  _location(location),
  _line_num(line_num),
  _description(description)
{
  try
    {
      ostringstream msg_stream;
      msg_stream << "StorageMgr::SmException-based exception thrown:" << endl
        << "   Location: "    << _location                 << endl
        << "   Line number: " << _line_num                  << endl
        << "   Description: " << _description              << endl;

      _what_value = msg_stream.str();
    }
  catch (...)
    {
      // Things got just too bad, but we don't want to hide this problem either.
      abort();
    }
}

//================================================================================

SmException::SmException(const SmException &src) :
  exception(),
  _location(src._location),
  _line_num(src._line_num),
  _description(src._description),
  _what_value(src._what_value)
{
}

//================================================================================

SmException & SmException::operator=(const SmException & src)
{
  _location = src._location;
  _line_num = src._line_num;
  _description = src._description;
  _what_value = src._what_value;
  return *this;
}

//================================================================================

SmException::~SmException()
  throw()
{
}

//================================================================================

const char * SmException::what()
  const throw()
{
  return _what_value.c_str();
}

//================================================================================

string SmException::getLocation() const
{
  return _location;
}

//================================================================================

int SmException::get_line_num() const
{
  return _line_num;
}

//================================================================================

string SmException::get_description() const
{
  return _description;
}

//****************************************************************************************
// Exception-derived classes implementations
//****************************************************************************************

SmInternalException::SmInternalException(const string location,
                     int line_num,
                     const string description) :
  SmException(location, line_num, description)
{
}

SmFilesystemException::SmFilesystemException(const string location,
                         int line_num,
                         const string description) :
  SmException(location, line_num, description)
{
}

SmIllegalParameterValueException::SmIllegalParameterValueException(const string location,
                                   int line_num,
                                   const string description) :
  SmException(location, line_num, description)
{
}

SmDatabaseExistsException::SmDatabaseExistsException(const string location,
                             int line_num,
                             const string description) :
  SmException(location, line_num, description)
{
}

SmDatabaseClosedException::SmDatabaseClosedException(const string location,
                             int line_num,
                             const string description) :
  SmException(location, line_num, description)
{
}

SmDatabaseNotFoundException::SmDatabaseNotFoundException(const string location,
                             int line_num,
                             const string description) :
  SmException(location, line_num, description)
{
}

SmObjectAlreadyDefinedException::SmObjectAlreadyDefinedException(const string location,
                                 int line_num,
                                 const string description) :
  SmException(location, line_num, description)
{
}

SmObjectNotDefinedException::SmObjectNotDefinedException(const string location,
                             int line_num,
                             const string description) :
  SmException(location, line_num, description)
{
}

SmDifferentRecordTypesException::SmDifferentRecordTypesException(const string location,
                                 int line_num,
                                 const string description) :
  SmException(location, line_num, description)
{
}

SmQueueClosedException::SmQueueClosedException(const string location,
                           int line_num,
                           const string description) :
  SmException(location, line_num, description)
{
}

SmObjectNotLockedException::SmObjectNotLockedException(const string location,
                               int line_num,
                               const string description) :
  SmException(location, line_num, description)
{
}

SmIllegalFrameNumException::SmIllegalFrameNumException(const string location,
                               int line_num,
                               const string description) :
  SmException(location, line_num, description)
{
}

SmInvalidUserLockStateException::SmInvalidUserLockStateException(const string location,
                                 int line_num,
                                 const string description) :
  SmException(location, line_num, description)
{
}

SmEmptyContainerException::SmEmptyContainerException(const string location,
                             int line_num,
                             const string description) :
  SmException(location, line_num, description)
{
}

SmClosedException::SmClosedException(const string location,
                     int line_num,
                     const string description) :
  SmException(location, line_num, description)
{
}

SmTooFullException::SmTooFullException(const string location,
                       int line_num,
                       const string description) :
  SmException(location, line_num, description)
{
}

SmNotAllocatedException::SmNotAllocatedException(const string location,
                         int line_num,
                         const string description) :
  SmException(location, line_num, description)
{
}

SmFileSystemFullException::SmFileSystemFullException(const string location,
                             int line_num,
                             const string description) :
  SmException(location, line_num, description)
{
}

SmFileFullException::SmFileFullException(const string location,
                     int line_num,
                     const string description) :
  SmException(location, line_num, description)
{
}

SmTrainStepSuspendedException::SmTrainStepSuspendedException(const string location,
                                 int line_num,
                                 const string description) :
  SmException(location, line_num, description)
{
}
