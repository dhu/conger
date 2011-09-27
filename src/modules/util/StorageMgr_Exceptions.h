#ifndef StorageMgr_Exceptions_H
#define StorageMgr_Exceptions_H

#include <assert.h>

#include <string>
#include <exception>

class SmException : public std::exception
{
public:
  // Creates a new SmException.
  //
  // Parameters:
  //   location - An optional indication of which source file contains the code
  //      that generated the exception.
  //   lineNum - An optional indication of which line number in the specified
  //      source file has the code that generated the exception.
  //   description - An optional specification of supplementary information
  //      regarding the exception.
  SmException(const std::string location, int line_num, const std::string description);


  // Produces an SmException with an identical name and field definitions to those
  // of the supplied parameter.
  SmException(const SmException &src);

  // Sets the left hand side SmException object have a name and field definitions
  // that are identical to those of the right-hand-side operand.
  SmException &operator=(const SmException & rhs);

  // (No externally visible behavior.)
  virtual ~SmException() throw();

  const char *what() const throw();

  std::string getLocation() const;

  int get_line_num() const;

  std::string get_description() const;

private:
  SmException() : _location(std::string("")), _line_num(0), _description(std::string("")) {assert(false);} // This should never be called.
  std::string _location;
  int _line_num;
  std::string _description;
  std::string _what_value;
};

//========================================================================================

class SmIllegalParameterValueException : public SmException
{
public:
  SmIllegalParameterValueException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmBufferTooSmallException : public SmException
{
public:
  SmBufferTooSmallException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmTooManyFieldsException : public SmException
{
public:
  SmTooManyFieldsException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmIllegalFieldNumException : public SmException
{
public:
  SmIllegalFieldNumException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmDatabaseLockedException : public SmException
{
public:
  SmDatabaseLockedException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmDatabaseExistsException : public SmException
{
public:
  SmDatabaseExistsException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmDatabaseNotFoundException : public SmException
{
public:
  SmDatabaseNotFoundException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmFilesystemException : public SmException
{
public:
  SmFilesystemException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmInternalException : public SmException
{
public:
  SmInternalException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmNoFieldDefinedException : public SmException
{
public:
  SmNoFieldDefinedException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmWrongFieldTypeException : public SmException
{
public:
  SmWrongFieldTypeException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmIllegalFieldLengthException : public SmException
{
public:
  SmIllegalFieldLengthException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmDatabaseClosedException : public SmException
{
public:
  SmDatabaseClosedException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmObjectAlreadyDefinedException : public SmException
{
public:
  SmObjectAlreadyDefinedException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmObjectNotDefinedException : public SmException
{
public:
  SmObjectNotDefinedException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmAlreadyUnpinnedException : public SmException
{
public:
  SmAlreadyUnpinnedException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmDifferentRecordTypesException : public SmException
{
public:
  SmDifferentRecordTypesException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmPageListAlreadyActiveException : public SmException
{
public:
  SmPageListAlreadyActiveException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmQBaseNotHistoricalException : public SmException
{
public:
  SmQBaseNotHistoricalException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmFieldNumUndefinedException : public SmException
{
public:
  SmFieldNumUndefinedException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmFieldNumInUseException : public SmException
{
public:
  SmFieldNumInUseException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmQueueClosedException : public SmException
{
public:
  SmQueueClosedException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmObjectNotLockedException : public SmException
{
public:
  SmObjectNotLockedException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmIllegalFrameNumException : public SmException
{
public:
  SmIllegalFrameNumException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmInvalidUserLockStateException : public SmException
{
public:
  SmInvalidUserLockStateException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmEmptyContainerException : public SmException
{
public:
  SmEmptyContainerException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmClosedException : public SmException
{
public:
  SmClosedException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmTooFullException : public SmException
{
public:
  SmTooFullException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmNotAllocatedException : public SmException
{
public:
  SmNotAllocatedException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmFileSystemFullException : public SmException
{
public:
  SmFileSystemFullException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmFileFullException : public SmException
{
public:
  SmFileFullException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

class SmTrainStepSuspendedException : public SmException
{
public:
  SmTrainStepSuspendedException(const std::string location, int line_num, const std::string description);
};

//========================================================================================

#endif
