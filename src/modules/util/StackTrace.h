
#ifndef STACK_TRACE_H
#define STACK_TRACE_H

#include "common.h"
#include "nmstl_util.h"
#include "execinfo.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// A very inefficient, but accurate, stack tracer.
//
// Steps:
//
//   1. Call the glibc backtrace() function to get the list of
//      return addresses on the stack
//   2. For each backtrace address,
//
//       - use dladdr1 to determine which shared library (or the main
//         executable) contains the address.  If a shared library,
//         subtract the .so's base address from the backtrace address.
//       - call addr2line to get the function, file name, and line
//         number of the invocation
//
// This all so Linux-and-glibc-dependent it's not funny.

struct sigcontext;

BOREALIS_NAMESPACE_BEGIN;

class StackTrace
{
  public:
    static StackTrace *save(unsigned int skip_frames = 0);
    static string get(unsigned int skip_frames = 0);

    static StackTrace *save(const sigcontext& ctx);
    static string get(const sigcontext& ctx);

    ~StackTrace();

    string as_string() const
    {
        return "StackTrace(" + to_string(_frames.size()) + " frames)";
    }

    string repr() const;

  private:
    static string exec(const char *const argv[]);
    static string basename(string path);

    StackTrace(unsigned int skip_frames, const sigcontext *);

    vector<void*> _frames;
    mutable string _trace;
};

BOREALIS_NAMESPACE_END;
#endif
