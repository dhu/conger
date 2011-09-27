
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include "xercesDomUtil.h"

using namespace std;

#include <nmstl_util.h>
#include "StackTrace.h"
#include "Exceptions.h"

#ifdef XERCES_CPP_NAMESPACE
using namespace xercesc;
#endif

NMSTL_NAMESPACE_BEGIN;

using namespace ::Borealis;

string to_string(const XMLCh* ch)
{
    char *cch = XMLString::transcode(ch);
    string s(cch);
    delete[] cch;

    return s;
}

string to_string(const XMLException& e)
{
    return to_string(e.getMessage());
}

string to_string(const DOMException& e)
{
    // Not supported in 2.1.0
    //    XMLCh errText[1024];
    //    if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, sizeof errText - 1))
    //        return "DOMException: " + to_string(errText);

    return "DOMException: #" + e.code;
}

static void my_unexpected_handler()
{
    NOTICE << "Unexpected exception\n" << indent(StackTrace::get(), 8);

    try
    {
        throw;
    } catch (AuroraException& e)
    {
        NOTICE << "Original exception was " << e << "\n" << indent(e.get_stack_trace(), 8);
    } catch (exception& e)
    {
        NOTICE << "Original exception was " << typeid(e) << ": " << e.what();
    }

    COREDUMP << "Aborting.";
}

static std::unexpected_handler setup_unexpected = std::set_unexpected(&my_unexpected_handler);

extern "C" void __assert_fail (const char *assertion, const char *file,
                               unsigned int line, const char *function)
{
    const char *last_slash = strrchr(file, '/');
    if (last_slash != 0)
        file = last_slash + 1;

    ERROR << file << ":" << line << ": " << function << ": Assertion '" << assertion << "' failed";
    ERROR << "\n"
          << indent(StackTrace::get(1), 4);
    COREDUMP << "Aborting.";
    abort();
}

#ifndef DISABLE_CRASH_TRACE
static void crash_handler(int sig, sigcontext ctx)
{
    fprintf(stderr,
            "\n"
            "***********************************\n"
            "Crash (must be cosmic rays)\n\n");
    fprintf(stderr, " - signal = %d\n", sig);
    fprintf(stderr, " - signal description = \"%s\"\n", strsignal(sig));
    fprintf(stderr, " - pid = %d\n", getpid());
    fprintf(stderr, " - thread = %d\n", int(pthread_self()));

#if __WORDSIZE == 32
    fprintf(stderr, " - program counter = %08x\n", (unsigned int)ctx.eip);
#else  /* __WORDSIZE == 64 */
    fprintf(stderr, " - program counter = %08x\n", (unsigned int)ctx.rip);
#endif

    fprintf(stderr, " - backtrace {\n");

    {
        void *frames[128];
        unsigned int size = backtrace(frames, array_size(frames));
        for (unsigned int i = 0; i < size; ++i)
        {
            fprintf(stderr, "       %p\n", frames[i]);
        }
    }

    fprintf(stderr, "   }\n");

    if (!getenv("NO_BT_ON_CRASH"))
    {
        fprintf(stderr, " - pretty backtrace {\n");

        string pretty_backtrace = indent(StackTrace::get(ctx), 7);
        fprintf(stderr, "%s", pretty_backtrace.c_str());
        fprintf(stderr, "   }\n");
    }

    char realname[1024];
    int len = readlink("/proc/self/exe", realname, sizeof realname - 1);
    if (len > 0)
        realname[len] = 0;
    else
        strcpy(realname, "???");
    fprintf(stderr, " - executable = %s\n", realname);

    fprintf(stderr,
            "\n\nSleeping for 5 minutes so you can attach GDB: run\n"
            "\n"
            "    gdb %s %d\n"
            "\n"
            "If this is a threaded app, type \"info threads\" and look for the\n"
            "line containing \"Thread %d\".\n\n",
            realname, getpid(), (int)pthread_self());
    sleep(300);

    // Not caught - pass on the signal
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(sig, &sa, NULL);
    raise(sig);
}

static void trap_segv()
{
    struct sigaction action;
    if (getenv("NO_CRASH_HANDLER")) return;
    /* Set up the structure to specify the new action. */
    action.sa_handler = (void(*)(int))&crash_handler;
    sigemptyset (&action.sa_mask);
    action.sa_flags = SA_RESTART;

    sigaction(SIGSEGV, &action, 0);
    sigaction(SIGILL, &action, 0);
    sigaction(SIGFPE, &action, 0);
    sigaction(SIGBUS, &action, 0);
}

static bool setup_segv = (trap_segv(), true);
#endif

NMSTL_NAMESPACE_END;
