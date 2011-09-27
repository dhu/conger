/*
 * @(#)RecoveryTime.cpp
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 11/07/2005
 */

#include "RecoveryTime.h"
#include <iostream>
#include <fstream>

BOREALIS_NAMESPACE_BEGIN

#define MAX(A, B) ((A) > (B) ? (A) : (B))

RecoveryTime::RecoveryTime():_start_time(Time::now().to_msecs()*0.001),_printed(false)
{
    _times.push_back(_start_time);
    _recovery_times.push_back(0);
}

void RecoveryTime::set(RecoveryTime& recovery_time)
{
    if (&recovery_time == this)
        return;
    _times.clear();
    _recovery_times.clear();
    for (register unsigned int i = 0; i < recovery_time._times.size(); i++)
    {
        _times.push_back(recovery_time._times[i]);
        _recovery_times.push_back(recovery_time._recovery_times[i]);
    }
    _start_time = recovery_time._start_time;
    _printed = recovery_time._printed;
}

string RecoveryTime::as_string() const
{
    string s = "{";
    for (register unsigned int i = 0; i < _times.size(); i++)
    {
        if (i != 0)
            s += ", ";
        s += ("(" + get_formatted_number(_times[i]-_start_time, 3) + ", " + get_formatted_number(_recovery_times[i], 3) + ")");
    }
    return s + "}";
}

void RecoveryTime::print(ptr<ofstream> os)
{
    register unsigned int i = 0;
    if (!_printed)
    {
        i = 2;
        _printed = true;
    }
    for (; i < _times.size(); i++)
    {
        (*os) << (long long)(_times[i]*1000) << ", " << (long)(_recovery_times[i]*1000) << endl;
    }
}

void RecoveryTime::put_recovery_time(Time t, Time recovery_time)
{
    TIME time = 0.001*t.to_msecs();
    TIME r_time = 0.001*recovery_time.to_msecs();
    if (time != _times[_times.size()-1] || r_time != _recovery_times[_recovery_times.size()-1])
    {
        _times.push_back(time);
        _recovery_times.push_back(r_time);
    }
}

TIME interpolate(TIME x1, TIME y1, TIME x2, TIME y2, TIME x)
{
    TIME r1 = x - x1;
    TIME r2 = x2 - x;
    if (r1 == 0 && r2 == 0)
    {
        r1 = 1;
        r2 = 1;
    }
    return (r1*y2 + r2*y1)/(r1+r2);
};

void RecoveryTime::add(RecoveryTime& recovery_time)
{
    align(recovery_time);
    for (register unsigned int i = 0; i < _times.size(); i++)
    {
        if (_times[i] != recovery_time._times[i])
            exit(0);
        _recovery_times[i] = (_recovery_times[i] + recovery_time._recovery_times[i]);
    }
//    cout << to_string() << endl;
}

void RecoveryTime::max(RecoveryTime& recovery_time)
{
    align(recovery_time);
    for (register unsigned int i = 0; i < _times.size(); i++)
        _recovery_times[i] = MAX(_recovery_times[i], recovery_time._recovery_times[i]);
}
    
TIME RecoveryTime::get_average()
{
    TIME sum_time_spans = 0;
    TIME sum_recovery_times = 0;
    for (register unsigned int i = 1; i < _times.size(); i++)
    {
        TIME time_span = _times[i] - _times[i-1];
        sum_time_spans += time_span;
        sum_recovery_times +=  0.5*(_recovery_times[i-1] + _recovery_times[i])*time_span;
    }
    if (sum_time_spans == 0)
        return 0;
    else
        return sum_recovery_times/sum_time_spans;
}

TIME RecoveryTime::get_time_span()
{
    if (_times.size() == 0)
        return 0;
    else
        return _times[_times.size()-1] - _times[0];
}
    
void RecoveryTime::clear()
{
	if (_times.size() >= 1)
	{
		TIME time = _times[_times.size()-1];
		TIME recovery_time = _recovery_times[_recovery_times.size()-1];
		_times.clear();
		_recovery_times.clear();
		_times.push_back(time);
		_recovery_times.push_back(recovery_time);
	}
	else 
		cout << "What??" << endl;
}

void RecoveryTime::insert(unsigned int i, TIME x)
{
    TIME x1 = _times[i-1];
    TIME y1 = _recovery_times[i-1];
    TIME x2 = _times[i];
    TIME y2 = _recovery_times[i];
    TIME y = interpolate(x1, y1, x2, y2, x);
    insert(i, x, y);
}

void RecoveryTime::erase(unsigned int i)
{
    if (i >= _times.size())
         cout << "What??" << endl;
    _times.erase(_times.begin()+i);
    _recovery_times.erase(_recovery_times.begin()+i);
}

void RecoveryTime::insert(unsigned int i, TIME x, TIME y)
{
    if (i > _times.size())
         cout << "What??" << endl;
    _times.insert(_times.begin()+i, x);
    _recovery_times.insert(_recovery_times.begin()+i, y);
}

void RecoveryTime::truncate_before(TIME time)
{
    register unsigned int i = 0;
    for (; i < _times.size(); i++)
    {
        if (_times[i] > time)
            break;
    }
    if (i == 0) 
        return;
    if (i == _times.size())
    {
        cout << "must not happen!" << endl;
        return;
    }
    TIME t_prev = _times[i-1];
    if (t_prev < time)
    {
        _times[i-1] = time;
        _recovery_times[i-1] = interpolate(t_prev, _recovery_times[i-1], _times[i], _recovery_times[i], time);
    }
    for (register unsigned int k = 0; k < i-1; k++)
        erase(0);
}

void RecoveryTime::align(RecoveryTime& recovery_time)
{
    TIME start_time = MAX(_times[0], recovery_time._times[0]);
    truncate_before(start_time);
    recovery_time.truncate_before(start_time);
    register unsigned int i = 0;
    register unsigned int j = 0;
    while(true)
    {
        if (i+1 == _times.size())
        {
            while ( j+1 < recovery_time._times.size())
                recovery_time.erase(j+1);
            return;
        }
        if (j+1 == recovery_time._times.size())
        {
            while ( i+1 < _times.size())
                erase(i+1);
            return;
        }
        // now i+1 and j+1 are safe indices
        TIME t = _times[i+1];
        TIME t_ = recovery_time._times[j+1];
        if (t < t_)
            recovery_time.insert(j+1, t);
        else if (t > t_)
            insert(i+1, t_);
        i++;
        j++;
//        cout << to_string() << endl;
//        cout << recovery_time.to_string() << endl;
    }
}


int RecoveryTime::get_number_of_drops()
{
    int count = 0;
    TIME prev_val = 0;
    for (register unsigned int i = 1; i < _times.size(); i++)
    {
        if (_times[i-1] == _times[i])
        {
            if (prev_val == 0)
                prev_val = _times[i];
            else
            {
                prev_val = _times[i];
            }

            count++;
        }
    }
    return count;
}

BOREALIS_NAMESPACE_END
