#include <fstream>
#include <cmath>

#include "CpuStats.h"

BOREALIS_NAMESPACE_BEGIN

CpuStats::CpuStats() :
    _start_time(Time::now()),
    _die_cpu_stats(false),
    _cpu_usage(NAN)
{
}

void CpuStats::set(ptr<ofstream> os)
{
    _start_time = Time::now();
    _os = os;
}

void CpuStats::terminate()
{
    _die_cpu_stats = true;
}

void CpuStats::get_proc_stats(uint64& total, uint64& idle)
{
    string label;
    uint64 user, nice, system;

    ifstream in("/proc/stat");

    if (!(in >> label >> user >> nice >> system >> idle))
    {    FATAL << "Unable to read /proc/stat";
    }

    total = user + nice + system + idle;
    // DEBUG << "total=" << total << " user=" << user << " idle=" << idle;
}

static const Time TIME_BETWEEN_OBSERVATIONS = Time::msecs(500);

// EWMA alpha factor
static const double EWMA_DECAY_FACTOR = 0.5;


////////////////////////////////////////////////////////////////////////////////
//
void CpuStats::run()
{
    uint64  total, idle;
    uint64 last_total, last_idle;
    char buffer[30];
//..............................................................................


    INFO << "Running CPU stats thread";

    get_proc_stats(last_total, last_idle);
    
    Time next_time = _start_time + TIME_BETWEEN_OBSERVATIONS;
    
    while (!_die_cpu_stats)
    {
        Thread::sleep(next_time - Time::now());
        next_time += TIME_BETWEEN_OBSERVATIONS;
        
        get_proc_stats(total, idle);
        
        if (total > last_total)
        {
            double usage_frac = 1.0 - 1.0*(idle - last_idle) / (total - last_total);

            if (_os)
            {
                sprintf(buffer, "%7.3f %9.5f %7" I64(d) " %7" I64(d),
                        ((Time::now()-_start_time).to_msecs()*0.001),
                         usage_frac,
                         total - last_total,
                         idle - last_idle);

                (*_os) << buffer << endl;
            }
            
            //DEBUG << "CPU usage: " << int(100.0*usage_frac) << "%";
    
            if (isnan(_cpu_usage))
            {   _cpu_usage = usage_frac;
            }
            else
            {   _cpu_usage += EWMA_DECAY_FACTOR * (usage_frac - _cpu_usage);
            }
    
            //        DEBUG << "CPU usage EWMA: " << int(100.0*_cpu_usage) << "%";
    
            last_total = total;
            last_idle  = idle;
        }
    }

    INFO << "CPU stats thread dying";
}

BOREALIS_NAMESPACE_END
