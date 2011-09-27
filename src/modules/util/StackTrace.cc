
#include "StackTrace.h"
#include <dlfcn.h>

BOREALIS_NAMESPACE_BEGIN;

StackTrace *StackTrace::save(unsigned int skip_frames)
{
    return new StackTrace(skip_frames, 0);
}
string StackTrace::get(unsigned int skip_frames)
{
    return StackTrace(skip_frames, 0).repr();
}

StackTrace *StackTrace::save(const sigcontext& ctx)
{
    return new StackTrace(0, &ctx);
}
string StackTrace::get(const sigcontext& ctx)
{
    return StackTrace(0, &ctx).repr();
}

StackTrace::StackTrace(unsigned int skip_frames, const sigcontext *ctx)
{
    unsigned int try_size = 128;

    if (ctx)
    {
#if __WORDSIZE == 32
        _frames.push_back(reinterpret_cast<void*>(ctx->eip));
#else  /* __WORDSIZE == 64 */
        _frames.push_back(reinterpret_cast<void*>(ctx->rip));
#endif
    }

    while (1)
    {
        void **frames = new void*[try_size];
        if (!frames)
            return;

        unsigned int size = backtrace(frames, try_size);
        if (size < try_size)
        {
            // Good!
            if (size > skip_frames)
                copy(frames + skip_frames, frames + size,
                     inserter(_frames, _frames.end()));

            delete[] frames;
            return;
        }


        // Not enough space - double and try again
        delete[] frames;
        try_size *= 2;
    }
}

StackTrace::~StackTrace()
{
}

string StackTrace::repr() const
{
    if (!_trace.empty())
        return _trace;

    if (_frames.empty())
        return "Empty stack trace";

    string trace;

    for (vector<void*>::const_iterator i = _frames.begin(); i != _frames.end(); ++i)
    {
        Dl_info info;

        if (!dladdr(*i, &info))
        {
            _trace << "at " << *i << "\n";
            continue;
        }

        // Calculate address by subtracting library's base address
        const void *addr = *i;
        if (info.dli_fbase >= (const void*)0x40000000)
        {    addr = (const char*)addr - (long int)info.dli_fbase;
        }

        char buf[32];
        snprintf(buf, sizeof buf, "%p", addr);

        const char *const argv[] = { "addr2line", "--functions", "--demangle", "--basenames", "-e", info.dli_fname, buf, 0 };
        string addr2line_data = exec(argv);
        string::size_type last_noncr = addr2line_data.find_last_not_of('\n');

        if (last_noncr != string::npos)
        {    addr2line_data.resize(last_noncr + 1);
        }

        string out;
        string::size_type cr = addr2line_data.find('\n');

        if (cr != string::npos)
        {
            string::size_type colon = addr2line_data.find(':', cr + 1);

            if (colon != string::npos)
            {
                string function(addr2line_data, 0, cr);
                string file(addr2line_data, cr + 1, colon - (cr + 1));
                string line(addr2line_data, colon + 1);

                if (file == "StackTrace.H" || file == "StackTrace.C")
                {    continue;
                }

                out = function;

                if (file == "??")
                {    out << " [in " << basename(info.dli_fname) << "]";
                }
                else
                {    out << " [at " << file << ":" << line << "]";
                }
            }
        }
        if (out.empty())
            out << "?? [in " << info.dli_fname << "]";

        _trace << "at " << out << "\n";
    }

    if (_trace.empty())
        _trace = "Empty stack trace";

    return _trace;
}

string StackTrace::exec(const char *const argv[])
{
    // Call ldd
    int fds[2];
    if (pipe(fds))
    {
        WARN << "pipe failed: " << strerror(errno);
        return string();
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        close(fds[0]);
        close(fds[1]);
        WARN << "Fork failed: " << strerror(errno);
        return string();
    }

    if (pid == 0)
    {
        dup2(fds[1], 1);
        close(fds[0]);
        close(fds[1]);

        execvp(const_cast<char*>(argv[0]), const_cast<char* const*>(argv));
        perror("execvp failed");
        abort();
    }

    // In parent; read output from ldd
    close(fds[1]);

    string data;
    char buf[1024];
    while (1)
    {
        int bytes = read(fds[0], buf, sizeof buf);
        if (bytes <= 0) break;

        data.append(buf, bytes);
    }
    close(fds[0]);

    int status;
    waitpid(pid, &status, 0);
    return data;
}

string StackTrace::basename(string path)
{
    string::size_type last_slash = path.rfind('/');

    if (last_slash == string::npos)
    {    return path;
    }

    return path.substr(last_slash + 1);
}

BOREALIS_NAMESPACE_END;
