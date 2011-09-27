
#include "Exceptions.h"
#include "StackTrace.h"

BOREALIS_NAMESPACE_BEGIN;

AuroraException::AuroraException(string message, string file, unsigned int line,
                                 const char *type) :
    _message(message), _file(file), _line(line), _type(type), _trace(StackTrace::save(1))
{
}

AuroraException::~AuroraException() throw()
{
}

string AuroraException::get_stack_trace() const
{
    return _trace ? _trace->repr() : "(no stack trace)";
}


string AuroraException::as_string() const
{
    string out = _type;
    if (!_file.empty())
    {
        out << " (";

        string::size_type last_slash = _file.find_last_of('/');
        if (last_slash == string::npos)
            out << _file;
        else
            out.append(_file, last_slash + 1, string::npos);

        out << ":" << _line << ")";
    }

    out << ": " << _message;
    return out;
}

BOREALIS_NAMESPACE_END;
