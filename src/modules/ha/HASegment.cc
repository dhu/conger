#include "HASegment.h"

MEDUSA_NAMESPACE_BEGIN

HASegment::HASegment(MedusaID backup_server):
_backup_server(backup_server),
_last_push_time(Time::now())
{
}

HAUnits& HASegment::get_ha_units()
{
    return _ha_units;
}

MedusaID HASegment::get_backup_server()
{
    return _backup_server;
}

void HASegment::add(ptr<HAUnit> ha_unit)
{
    string name = ha_unit->get_name();
    if (_ha_units.find(name) == _ha_units.end())
    {
        _ha_units[name] = ha_unit;
    }
}

void HASegment::update_statistics()
{
    if (_ha_units.size() == 0)
    {
//        _recovery_time_statistics.addSample(0);
//        _recovery_time_statistics.slide();
        return;
    }
    HAUnits::iterator i = _ha_units.begin();
    ptr<HAUnit> ha_unit = i->second;
    ha_unit->put_recovery_time();
    _recovery_time.set(ha_unit->get_recovery_time());
    ha_unit->update_statistics();
    i++;
    for(; i != _ha_units.end(); i++)
    {
        ha_unit = i->second;
//        NOTICE << ha_unit->get_name();
//        NOTICE << ha_unit->get_recovery_time().as_string();       
//        NOTICE << _recovery_time.as_string();       
        _recovery_time.add(ha_unit->get_recovery_time());
//        NOTICE << _recovery_time.as_string();       
        ha_unit->update_statistics();
    }
//    TIME recovery_time = _recovery_time.get_average();
//    _recovery_time_statistics.addSample(recovery_time);
//    _recovery_time_statistics.slide();
}

bool HASegment::has_boxes() 
{
    return _ha_units.size() > 0;
}

RecoveryTime& HASegment::get_recovery_time()
{
    return _recovery_time;
}

Time HASegment::get_expected_recovery_time(ptr<HAUnit> ha_unit, Time target_time)
{
    Time recovery_time = (target_time - target_time);
    for (HAUnits::iterator j = _ha_units.begin(); j!= _ha_units.end(); j++)
    {
        if (ha_unit == j->second)
            recovery_time += j->second->get_expected_recovery_time(target_time, Time::now());
        else
            recovery_time += j->second->get_expected_recovery_time(target_time);
    }
    return recovery_time;
}

Time HASegment::get_expected_staleness(ptr<HAUnit> ha_unit, Time target_time)
{
    Time sum_staleness = (target_time - target_time);
    for (HAUnits::iterator j = _ha_units.begin(); j!= _ha_units.end(); j++)
    {
        Time staleness;
        if (ha_unit == j->second)
            staleness = j->second->get_expected_staleness(target_time, Time::now());
        else
            staleness = j->second->get_expected_staleness(target_time);
        sum_staleness += staleness;
    }
    Time staleness = Time::msecs(sum_staleness.to_msecs()/_ha_units.size());;
    return staleness;
}

Time HASegment::get_last_effective_capturing_start_time()
{
    Time last_effective_capture_start_time;
    for (HAUnits::iterator j = _ha_units.begin(); j!= _ha_units.end(); j++)
    {
        Time t = j->second->_last_effective_capturing_start_time;
        if (t > last_effective_capture_start_time)
            last_effective_capture_start_time = t;
    }
    return last_effective_capture_start_time;
}


MEDUSA_NAMESPACE_END
