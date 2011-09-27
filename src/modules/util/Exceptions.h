#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>
#include <exception>

#include <nmstl_util.h>
#include <StorageMgr_Exceptions.h>
#include "StackTrace.h"

BOREALIS_NAMESPACE_BEGIN;

#define Throw(type, message) throw type(message, __FILE__, __LINE__)

/// The base class for all AuroraExceptions.  Currently defined exceptions
/// include
///
/// - AuroraNoSuchEntityException: requested an entity which doesn't exist
/// - AuroraBadXmlException: non-well-formed (i.e., unparseable) XML
/// - AuroraBadEntityException: semantically invalid entity description (e.g., box doesn't have a type)
/// - AuroraTypingException: typechecking failed
/// - AuroraIllegalArgumentException: miscellaneous illegal argument provided
class AuroraException : public exception
{
  public:
    /// Constructor.  An AuroraException should never be thrown directly; use the
    /// Throw macro instead.
    ///
    /// @param message the exception message
    /// @param file the source file in which the exception is being thrown
    /// @param line the line in the source file in which the exception is being thrown
    /// @param type the type of the exception
    AuroraException(string message, string file = string(), unsigned int line = 0,
                    const char *type = "AuroraException");

    /// Destructor.
    virtual ~AuroraException() throw();

    /// Returns the message as a C string.  The string is owned by this
    /// object, so the pointer becomes invalid upon description of the
    /// AuroraException.
    virtual const char *what() const throw() { return _message.c_str(); }

    /// Returns the message.
    string getMessage() const { return _message; }

    /// Returns the source file in which the exception was thrown (or
    /// an empty string if unknown).
    string get_file() const { return _file; }

    /// Returns the line number in the source file in which the exception
    /// was thrown (or an empty string if unknown).
    unsigned int get_line() const { return _line; }

    /// Returns the type of the exception (e.g., "AuroraException" or
    /// "AuroraNoSuchEntityException").
    const char *getType() const { return _type; }

    /// Returns a description containing all known information about the message.
    string as_string() const;

    /// Returns a stack trace describing where the exception was
    /// thrown.  Uses external programs; may be very slow.
    string get_stack_trace() const;

  private:
    string _message;
    string _file;
    unsigned int _line;
    const char *_type;
    ptr<StackTrace> _trace;
};

#define AURORA_EXCEPTION_TYPE(cl) \
class cl : public AuroraException { \
  public: \
    cl(string message, string file = string(), unsigned int line = 0) : \
        AuroraException(message, file, line, #cl) {} \
};

// Requested an entity which doesn't exist
AURORA_EXCEPTION_TYPE(aurora_no_such_entity_exception);

// Non-well-formed XML
AURORA_EXCEPTION_TYPE(AuroraBadXmlException);

// Semantically invalid XML (e.g., box doesn't have a type)
AURORA_EXCEPTION_TYPE(AuroraBadEntityException);

// Typechecking failed
AURORA_EXCEPTION_TYPE(aurora_typing_exception);

// Miscellaneous illegal argument
AURORA_EXCEPTION_TYPE(aurora_illegal_argument_exception);

// A stream has been closed, and thus means this method should either
// unblock immediately, or is now illegal given the closed state.
AURORA_EXCEPTION_TYPE(aurora_closed_exception);

// No more data could be written to a file for some reason
AURORA_EXCEPTION_TYPE(aurora_file_full_exception);

// The filesystem is out of space when a write attempt occured
AURORA_EXCEPTION_TYPE(aurora_file_system_full_exception);

BOREALIS_NAMESPACE_END;

#endif
