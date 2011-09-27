#include "HAUnit.h"
#include "QBox.h"

MEDUSA_NAMESPACE_BEGIN

HAUnit::HAUnit():
_name(""),
_backup_server(MedusaID("")),
_start_time(Time::msecs(0)),
_ready_for_checkpoint(true),
_delta_checkpointing(false),
_last_capturing_start_time(Time::msecs(0)),
_last_effective_capturing_start_time(Time::msecs(0)),
_last_capturing_end_time(Time::msecs(0)),
_last_update_start_time(Time::msecs(0)),
_last_update_end_time(Time::msecs(0)),
_accumulated_capture_cost(Time::msecs(0)),
_accumulated_paste_cost(Time::msecs(0)),
_last_capture_cost(Time::msecs(0)),
_last_paste_cost(Time::msecs(0)),
_accumulated_expected_capture_cost(Time::msecs(0)),
_accumulated_expected_paste_cost(Time::msecs(0)),
_passive(false),
_processing_times(10),
_last_processing_time(Time::msecs(0))
{
}

HAUnit::HAUnit(string name, MedusaID backup_server):
_name(name),
_backup_server(backup_server),
_start_time(Time::msecs(0)),
_ready_for_checkpoint(true),
_delta_checkpointing(false),
_last_capturing_start_time(Time::msecs(0)),
_last_effective_capturing_start_time(Time::msecs(0)),
_last_capturing_end_time(Time::msecs(0)),
_last_update_start_time(Time::msecs(0)),
_last_update_end_time(Time::msecs(0)),
_accumulated_capture_cost(Time::msecs(0)),
_accumulated_paste_cost(Time::msecs(0)),
_accumulated_expected_capture_cost(Time::msecs(0)),
_accumulated_expected_paste_cost(Time::msecs(0)),
_passive(false),
_processing_times(10),
_last_processing_time(Time::msecs(0))
{
}

HAUnit::~HAUnit()
{
}

void HAUnit::set_passive()
{
    _passive = true;
}

Name2QBox& HAUnit::get_boxes()
{
    return _boxes;
}

void HAUnit::reset_statistics()
{
    Time zero = Time::msecs(0);
    _start_time = zero;
    _ready_for_checkpoint = true;
//    _delta_checkpointing = false;
    _last_capturing_start_time = zero;
    _last_effective_capturing_start_time = zero;
    _last_capturing_end_time = zero;
    _last_update_start_time = zero;
    _last_update_end_time = zero;
//    _processing_time = zero;
    _accumulated_capture_cost = zero;
    _accumulated_paste_cost = zero;
    _accumulated_expected_capture_cost = zero;
    _accumulated_expected_paste_cost = zero;
}

string HAUnit::get_name()
{
    return _name;
}

MedusaID HAUnit::get_backup_server()
{
    return _backup_server;
}

RecoveryTime& HAUnit::get_recovery_time()
{
    return _recovery_time;
}

bool HAUnit::is_ready_for_checkpoint()
{
    return _ready_for_checkpoint;
}

Time HAUnit::get_last_capturing_start_time()
{
    return _last_capturing_start_time;
}

void HAUnit::start_capturing()
{
    _ready_for_checkpoint = false;
    _last_capturing_start_time = Time::now();
}

void HAUnit::end_capturing()
{
    _last_capturing_end_time = Time::now();
    put_recovery_time(_last_capturing_end_time);
//    NOTICE << _recovery_time.to_string();
    _last_effective_capturing_start_time = _last_capturing_start_time;
    put_recovery_time(_last_capturing_end_time);
//    NOTICE << _recovery_time.to_string();
}

void HAUnit::start_update()
{
    _last_update_start_time = Time::now();
    put_recovery_time();
}

void HAUnit::end_update()
{
    _last_update_end_time = Time::now();
    put_recovery_time();
}

void HAUnit::end_checkpoint()
{
    _delta_checkpointing= true;
    _ready_for_checkpoint = true;
}


Time HAUnit::get_expected_staleness(Time target_time, Time capturing_start_time)
{
    if (capturing_start_time == Time::secs(-1))
        capturing_start_time = _last_effective_capturing_start_time;
    if (capturing_start_time == Time::msecs(0))
        capturing_start_time = _start_time;
    Time staleness = target_time - capturing_start_time;
    return staleness;
}

Time HAUnit::get_expected_recovery_time(Time target_time, Time capturing_start_time)
{
    if (capturing_start_time == Time::secs(-1))
        capturing_start_time = _last_effective_capturing_start_time;
    Time staleness = get_expected_staleness(target_time, capturing_start_time);
    Time extra = Time::msecs(0);

    Time update_cost = get_expected_paste_cost();
    if (capturing_start_time > _last_update_start_time)
        extra = update_cost;
    else if (target_time > _last_update_start_time + update_cost)
        extra = Time::msecs(0);
    else
        extra = _last_update_start_time + update_cost - target_time;

    Time recovery_time = Time::msecs(staleness.to_msecs() * get_processing_load()) + extra;
//    NOTICE << _name << " " << recovery_time.to_msecs()*0.001;
//    recovery_time = recovery_time + extra;
    return recovery_time;
}

void HAUnit::add_processing_time(Time processing_time)
{
    locking(_lock)
    {
        Time current = Time::now();
        if (_start_time == Time::msecs(0))
        {
            _start_time = current;
            _last_processing_time = _start_time;
        }
        while (current > _last_processing_time + Time::secs(1))
        {
            _processing_times.slide();
            _last_processing_time = _last_processing_time + Time::secs(1);
        }
        _processing_times.addSample(processing_time.to_usecs());
    }
}

void HAUnit::set_processing_times(SlidingWindowStatistics& processing_times)
{
    locking(_lock)
    {
        _processing_times.set(processing_times);
    }
}

ptr<SlidingWindowStatistics> HAUnit::get_processing_times()
{
    return ptr<SlidingWindowStatistics>(new SlidingWindowStatistics(_processing_times));
}

Time HAUnit::get_last_capture_cost()
{
    return _last_capture_cost;
}

Time HAUnit::get_last_paste_cost()
{
    return _last_paste_cost;
}

Time HAUnit::get_expected_capture_cost()
{
    Time cost = Time::msecs(0);
    for (Name2QBox::iterator i = _boxes.begin(); i != _boxes.end(); i++)
    {
        cost +=  i->second->get_expected_capture_cost();
    }
    return cost;
}

Time HAUnit::get_expected_paste_cost()
{
    Time cost = Time::msecs(0);
    for (Name2QBox::iterator i = _boxes.begin(); i != _boxes.end(); i++)
    {
        cost +=  i->second->get_expected_paste_cost();
    }
    return cost;
}

void HAUnit::add_capture_cost(Time capture_cost)
{
    _accumulated_capture_cost += capture_cost;
    _last_capture_cost = capture_cost;
}

void HAUnit::add_paste_cost(Time paste_cost)
{
    _accumulated_paste_cost += paste_cost;
    _last_paste_cost = paste_cost;
}

void HAUnit::add_expected_capture_cost(Time cost)
{
    _accumulated_expected_capture_cost += cost;
}

void HAUnit::add_expected_paste_cost(Time cost)
{
    _accumulated_expected_paste_cost += cost;
}

double HAUnit::get_processing_load()
{
    locking(_lock)
    {
        if (_processing_times.getNumberOfBins() <= 2)
            return 0;
        else
        {
            double time_span = 1000.0*1000.0*(_processing_times.getNumberOfBins()-1); // because we don't use the last bin.
            double sum_load =  _processing_times.getSum() -  _processing_times.getCurrentSum();
            return sum_load/time_span;
        }
/*    
        if (_last_processing_time == Time::secs(0))
            return 0;
        Time time = Time::now();
        if (_passive)
            time = _last_capturing_start_time;
        double time_span = (time - _last_processing_time).to_usecs();
        while (time_span < 1000.0*1000.0*(_processing_time.getNumberOfBins()-1))
        {
            time_span += 1000.0*1000.0;
        }
        if (time_span)
        {
            double load = _processing_time.getSum()/time_span;
    //        NOTICE << _name << " " << load;
            return load;
        }
        else
            return 0;
*/
    }
}

double HAUnit::get_capture_load()
{
    double time_span = (Time::now() - _start_time).to_msecs();
    if (time_span)
        return _accumulated_capture_cost.to_msecs()/time_span;
    else
        return 0;
}

double HAUnit::get_paste_load()
{
    double time_span = (Time::now() - _start_time).to_msecs();
    if (time_span)
        return _accumulated_paste_cost.to_msecs()/time_span;
    else
        return 0;
}

double HAUnit::get_expected_capture_load()
{
    double time_span = (Time::now() - _start_time).to_msecs();
    if (time_span)
        return _accumulated_expected_capture_cost.to_msecs()/time_span;
    else
        return 0;
}

double HAUnit::get_expected_paste_load()
{
    double time_span = (Time::now() - _start_time).to_msecs();
    if (time_span)
        return _accumulated_expected_paste_cost.to_msecs()/time_span;
    else
        return 0;
}
 
void HAUnit::add(ptr<HAUnit> ha_unit)
{
    for (Name2QBox::iterator i = ha_unit->_boxes.begin(); i != ha_unit->_boxes.end(); i++)
        _boxes[i->first] = i->second;
}

string HAUnit::as_string()
{
    string out;
    for (Name2QBox::iterator i = _boxes.begin(); i != _boxes.end(); i++)
    {
        if (i != _boxes.begin())
            out += ", ";
        out += i->first;
    }    
    return out;
}

void HAUnit::put_recovery_time(Time time)
{
    _recovery_time.put_recovery_time(time, get_expected_recovery_time(time));
}

void HAUnit::update_statistics()
{
//    _accumulated_capture_cost = Time::msecs(0);
//    _accumulated_paste_cost = Time::msecs(0);
//    _accumulated_expected_capture_cost = Time::msecs(0);
//    _accumulated_expected_paste_cost = Time::msecs(0);
    
    _recovery_time.clear();
}

void HAUnit::update_statistics(ptr<HAUnit> ha_unit)
{
    _accumulated_paste_cost = ha_unit->_accumulated_paste_cost;
    _last_update_start_time = ha_unit->_last_update_start_time;
    _last_update_end_time   = ha_unit->_last_update_end_time;
    _last_capture_cost = ha_unit->_last_capture_cost;
    _last_paste_cost = ha_unit->_last_paste_cost;
}

unsigned int HAUnit::get_number_of_boxes()
{
    return _boxes.size();
}

bool HAUnit::is_delta_checkpointing()
{
    return _delta_checkpointing;
}

void HAUnit::set(ptr<HAUnit> ha_unit)
{
    this->_delta_checkpointing = ha_unit->_delta_checkpointing;
    this->_last_capturing_start_time = ha_unit->_last_capturing_start_time;
    this->_last_capturing_end_time = ha_unit->_last_capturing_end_time;
    this->_last_update_start_time = ha_unit->_last_update_start_time;
    this->_last_update_end_time = ha_unit->_last_update_end_time;
    this->_last_effective_capturing_start_time = ha_unit->_last_effective_capturing_start_time;
    this->_accumulated_paste_cost = ha_unit->_accumulated_paste_cost;
    this->_start_time = ha_unit->_start_time;
    this->_processing_times = ha_unit->_processing_times;
    this->_recovery_time.set(ha_unit->_recovery_time);
}

MEDUSA_NAMESPACE_END
