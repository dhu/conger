
#ifndef CPU_STATS_H
#define CPU_STATS_H

#include "common.h"
#include "nmstl_util.h"

BOREALIS_NAMESPACE_BEGIN

class CpuStats : public Thread
{
  public:
    CpuStats();

    void terminate();

    /// Returns the CPU usage as a value from 0 to 1.  Uses an EWMA of
    /// CPU usage, updated about every quarter second.  This routine
    /// is very fast.
    double get_cpu_usage() const { return _cpu_usage; }
    string as_string() const;
    void set(ptr<ofstream> os);

  private:
    void run();
    
    Time           _start_time;
    ptr<ofstream>  _os;

    bool    _die_cpu_stats;
    double  _cpu_usage;

    static void get_proc_stats(uint64  &total, uint64  &idle);
};

BOREALIS_NAMESPACE_END
#endif
