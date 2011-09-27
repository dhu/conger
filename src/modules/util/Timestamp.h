#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <sys/time.h>
#include <sstream>
#include <NMSTL/serial>

#include "common.h"

BOREALIS_NAMESPACE_BEGIN;

class Timestamp : public timeval
{
  private:
    static const long MILLION = 1000000;

  public:
    Timestamp()
    {
        tv_sec = 0;
        tv_usec = 0;
    }
    Timestamp(long sec, long usec)
    {
        tv_sec = sec;
        tv_usec = usec;
    }
    Timestamp(const timeval &t)
    {
        tv_sec = t.tv_sec;
        tv_usec = t.tv_usec;
    }

    static Timestamp now()
    {
        timeval t;
        gettimeofday(&t, NULL);
        return Timestamp(t);
    }

    Timestamp &operator+=(const Timestamp &rhs)
    {
        tv_sec += rhs.tv_sec;
        tv_usec += rhs.tv_usec;
        if (tv_usec >= MILLION)
        {
            tv_sec += 1;
            tv_usec -= MILLION;
        }
        return *this;
    }

    Timestamp operator-(const Timestamp &rhs) const
    {
        long ret_sec = tv_sec - rhs.tv_sec;
        long ret_usec = tv_usec - rhs.tv_usec;
        if (ret_usec < 0)
        {
            ret_sec -= 1;
            ret_usec += MILLION;
        }
        return Timestamp(ret_sec, ret_usec);
    }

    Timestamp &operator-=(const Timestamp &t)
    {
        tv_sec -= t.tv_sec;
        tv_usec -= t.tv_usec;
        if (tv_usec < 0)
        {
            tv_sec -= 1;
            tv_usec += MILLION;
        }
        return *this;
    }

    bool operator==(const Timestamp &t) const
    {
        return ((tv_sec == t.tv_sec) &&
                (tv_usec == t.tv_usec));
    }

    bool operator<(const Timestamp &t) const
    {
        return ((tv_sec < t.tv_sec) ||
                ((tv_sec == t.tv_sec) &&
                 (tv_usec < t.tv_usec)));
    }

    bool operator>(const Timestamp &t) const
    {
        return ((tv_sec > t.tv_sec) ||
                ((tv_sec == t.tv_sec) &&
                 (tv_usec > t.tv_usec)));
    }

    long long to_usecs() const
    {
        return tv_sec * 1000000LL + tv_usec;
    }

    long long to_msecs() const
    {
        return to_usecs()/1000;
    }

    timeval to_timeval() const
    {
        timeval t;
        t.tv_sec = tv_sec;
        t.tv_usec = tv_usec;
        return t;
    }

    double to_double() const
    {
        return ((double) tv_sec) + (((double) tv_usec) / 1000000.0);
    }

    const string as_string() const
    {
        ostringstream oss;
        oss << "(" << tv_sec << ", " << tv_usec << ")";
        return oss.str();
    }

    NMSTL_SIMPLY_SERIALIZABLE(Timestamp, << tv_sec << tv_usec);
};

BOREALIS_NAMESPACE_END;

#endif
