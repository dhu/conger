#include "HA.h"
#include "QueryProcessor.h"
#include "AggregateQBox.h"
#include "CatalogSubscription.h" // This file has implementation of HA-releated parts in the QueryProcessor class.

#define NUMBER_OF_ITERATION_EPOCH 1


BOREALIS_NAMESPACE_BEGIN

void HAUnit::add(ptr<QBox> box)
{
    ptr<HAUnit> ha_unit = box->get_ha_unit();
    if (ha_unit != NULL)
        ha_unit->remove(box);
    _boxes[box->get_name()] = box;
    box->set_ha_unit(ptr<HAUnit>::unowned(this));
    reset_statistics();
}

void HAUnit::remove(ptr<QBox> box)
{
    Name2QBox::iterator i = _boxes.find(box->get_name());
    if (i != _boxes.end())
    {
        i->second->set_ha_unit(ptr<HAUnit>());
        _boxes.erase(i);
    }
}

double HAUnit::get_staleness() {
    if (_boxes.size() == 0)
        return 0;
    double staleness = 0;
    for (Name2QBox::iterator i = _boxes.begin(); i != _boxes.end(); i++)
    {
        staleness +=  i->second->get_staleness();
    }
    return staleness/_boxes.size();
}


bool HA::random_backup_distribution = false;

bool HA::ready = false;
    
Time HA::last_end_time = Time::now();

HAMethod HA::_HA_method = NO_HA;

CheckpointMethod HA::_checkpoint_method;

double HA::_checkpoint_interval = -1;

double HA::_push_interval = 100;

bool HA::_decremental_checkpoint_interval = false;

int HA::_checkpoint_slack = 50;

vector<MedusaID> HA::_ha_partners;

void HA::set_decremental_checkpoint_interval()
{
    _decremental_checkpoint_interval = true;
}

void HA::set_HA_method(HAMethod HA_method)
{
    _HA_method = HA_method;
}

HAMethod HA::get_HA_method()
{
    return _HA_method;
}

void HA::set_checkpoint_method(CheckpointMethod checkpoint_method)
{
    _checkpoint_method = checkpoint_method;
}

void HA::set_checkpoint_interval(int checkpoint_interval)
{
    _checkpoint_interval = checkpoint_interval;
}

void HA::set_checkpoint_slack(int checkpoint_slack)
{
    _checkpoint_slack = checkpoint_slack;
}

double HA::get_checkpoint_interval()
{
    return _checkpoint_interval;
}

int HA::get_checkpoint_slack()
{
    return _checkpoint_slack;
}



void HA::add_ha_partner(MedusaID ha_partner)
{
    _ha_partners.push_back(ha_partner);
}

// --------------------------------------------------
HA::HA(string id, QueryProcessor& qp)
    : _id(id), 
    _qp(qp),
    _epoch(0),
    _tentative_epoch(0),
    _start_time(Time::now()),
    _next_checkpoint_time(_start_time+Time::secs(10)),
    _current_checkpoint_start_time(_start_time),
    _last_checkpoint_start_time(_start_time),
    _last_push_time(Time::now())
{
    setup_ha();
}

void HA::setup_ha()
{
    NOTICE << "ha is on!!!";
    NOTICE << "HA method is " << hamethod_s[_HA_method] << ".";
    if (_HA_method == PASSIVE_STANDBY)
        NOTICE << "Checkpoint method is " << checkpoint_method_s[_checkpoint_method] << ".";
    if (getenv("BOREALIS_LOGGING") != 0)
    {
        string file_name = _id+".";
        string file_name_statistics = file_name + "statistics.log";
        string file_name_recovery_time = file_name + "recovery_time.log";
        NOTICE << "From now on the statistics of " << _id << " will be text-dumped to file " << file_name_statistics << ".";
        NOTICE << "From now on the recovery time of " << _id << " will be text-dumped to file " << file_name_recovery_time << ".";
        _ostream_statistics.set(new ofstream(file_name_statistics.c_str()));
        (*_ostream_statistics) << "% [time] [ha_unit] [# boxes] [p/b type] [proc. load] [capture cost] [paste cost] [e. c. cost] [e. p. cost] [new wins] [updated wins]" << endl;
        _ostream_recovery_time.set(new ofstream(file_name_recovery_time.c_str()));
    }
}

void HA::form_ha_unit(ptr<QBox> box, bool primary)
{
    locking(_lock)
    {
        if (!box) return;
        string box_name = box->get_name();
        string ha_unit_name;
        if (_checkpoint_method == WHOLE)
            ha_unit_name = box_name[0];
        else
            ha_unit_name = box_name.substr(0, box_name.size()-1);
        ptr<HAUnit> ha_unit;
        if (primary)
        {
            if (_ha_units_primary.find(ha_unit_name) != _ha_units_primary.end())
            {   // if find
                ha_unit = _ha_units_primary[ha_unit_name];
            }
            else
            {
                ha_unit = create_ha_unit(ha_unit_name);
            }
            ha_unit->add(box);
        }
        else // if backup
        {
            if (_ha_units_backup.find(ha_unit_name) != _ha_units_backup.end())
            {   // if find
                ha_unit = _ha_units_backup[ha_unit_name];
            }
            else
            {
                ha_unit = ptr<HAUnit>(new HAUnit(ha_unit_name, _id));
                _ha_units_backup[ha_unit_name] =  ha_unit;
            }
            ha_unit->add(box);
        }
    }
/*
    vector<CatalogBox*> boxes = box->get_neighboring_boxes();
    if (boxes.size() == 0)
    {
        if (!(box->get_ha_unit()))
        {
            string name = box->get_box_name().as_string();
            ptr<HAUnit> ha_unit = create_ha_unit(name);
            ha_unit->add(box);
        }
    }
    else
    {
        HAUnit* ha_unit = boxes[0]->get_ha_unit();
        ha_unit->add(box);
        for (register unsigned int i = 1; i < boxes.size(); i++)
        {
            HAUnit* h_unit = boxes[i]->get_ha_unit();
            if (ha_unit != h_unit)
                merge(ha_unit, h_unit);
        }
    }
*/
}

void HA::enqueue(ptr<CheckpointMessage> cm)
{
    // must not be interrupted!!! Maybe I need some thread safety stuff this one with the takeover precedure which happens on secondary : JH
    ptr<HAUnit> ha_unit = cm->get_ha_unit();
    string ha_unit_name = ha_unit->get_name();
    string primary_server = cm->get_primary_server();
    locking(_lock)
    {
        if (_backup_segments.find(primary_server) == _backup_segments.end())
        {
            ptr<HASegment> ha_backup_segments(new HASegment(primary_server));
            _backup_segments[primary_server] = ha_backup_segments;
        }
    }
    ptr<HASegment> backup_segment = _backup_segments[primary_server];

    HAUnits& ha_units = backup_segment->get_ha_units();
    if (ha_units.find(ha_unit_name) == ha_units.end())  // currently cannot handle update of HA Units.
    {
        instantiate_boxes(cm);
        locking(_lock)
        {
            backup_segment->add(ha_unit);
            if (_HA_method == PASSIVE_STANDBY)
                ha_unit->set_passive();
        }
    }
    locking(_lock)
    {
        ha_units[ha_unit_name]->set(ha_unit);
        _pending_checkpoint_messages.push_back(cm);
        
 /////////////// used for debugging

//        NOTICE << get_formatted_number((Time::now() - _start_time).to_msecs()*0.001,2) << ": enqueued from " << primary_server << " HA unit " << ha_unit_name << ".";
    
        map<string, Time> recovery_times;
        Time t = Time::now();
        for (unsigned int i = 0; i < _pending_checkpoint_messages.size(); i++)
        {
            ptr<CheckpointMessage> cm = _pending_checkpoint_messages[i];
            string primary_server = cm->get_primary_server();
            if (recovery_times.find(primary_server) == recovery_times.end())
            {
                ptr<HASegment> backup_segment = _backup_segments[primary_server];
                Time recovery_time = backup_segment->get_expected_recovery_time(ptr<HAUnit>(), t);;
  //              NOTICE << "<-" << primary_server << " " << recovery_time.to_msecs() << " " << backup_segment->get_expected_staleness(ptr<HAUnit>(), t).to_msecs();
                recovery_times[primary_server] = recovery_time;
            }
        }
//        NOTICE << " ";
    }
    _qp.wake_up_scheduler();
}

void HA::handle_idle_time()
{
    Time start_time = Time::now();
    Time push_interval = Time::msecs(_push_interval);
    if (start_time > _last_push_time + push_interval)
    {
        while (_last_push_time <= start_time)
            _last_push_time += push_interval;
        push_processing_times();
    }
    if (start_time < _next_checkpoint_time)
    {
//        _qp.schedule_wake_up(_next_checkpoint_time);
        return;
    }
    if (!ready)
    {
        return;
    }
    if (_checkpoint_interval >= 1)
    {
        do
        {
            if (_checkpoint_interval < 1)
                break;
            _next_checkpoint_time = _next_checkpoint_time + Time::msecs(_checkpoint_interval);
            if (_decremental_checkpoint_interval)
                _checkpoint_interval = _checkpoint_interval*0.99;
        } while (_next_checkpoint_time < start_time);
    }
    locking(_lock)
    {
        do_ha_task();
    }
}

void HA::checkpoint_message_enqueued(MedusaID backup)
{
// Now has nothing to do
/*    locking(_lock)
    {
        ptr<HAUnit> ha_unit = cm->get_ha_unit();
        string name = ha_unit->get_name();

        if (_ha_units_primary.find(name) != _ha_units_primary.end())
        {
            NOTICE << "///////////";
            NOTICE << ha_unit->get_recovery_time();
            NOTICE << _ha_units_primary[name]->get_recovery_time();
        }
    }
*/
}

void HA::end_checkpoint(ptr<CheckpointMessage> cm) // on primary
{
    locking(_lock)
    {
        ptr<HAUnit> ha_unit = cm->get_ha_unit();
        string name = ha_unit->get_name();
        if (_ha_units_primary.find(name) != _ha_units_primary.end())
        {
            _ha_units_primary[name]->update_statistics(cm->get_ha_unit());
            _ha_units_primary[name]->_recovery_time.put_recovery_time(ha_unit->_last_update_start_time,
                _ha_units_primary[name]->get_expected_recovery_time(ha_unit->_last_update_start_time));
            _ha_units_primary[name]->end_update();
            _ha_units_primary[name]->end_checkpoint();
            if (_remaining_checkpoints.find(name) != _remaining_checkpoints.end())
                if ((--(_remaining_checkpoints[name])) == 0)
                    _remaining_checkpoints.erase(name);
        }
        if (_remaining_checkpoints.size() == 0)
        {
            end_epoch();
        }
    }
}

ptr<HAUnit> HA::create_ha_unit(string name)
{
    MedusaID backup_server;
    if (random_backup_distribution)
        backup_server = _ha_partners[rand() % _ha_partners.size()];
    else
        backup_server = _ha_partners[_ha_units_primary.size() % _ha_partners.size()];
    NOTICE << "ha unit: " << name << " -> " << backup_server;
    ptr<HAUnit> ha_unit(new HAUnit(name, backup_server));
    _ha_units_primary[name] = ha_unit;
    if (_primary_segments.find(backup_server) == _primary_segments.end())
    {
        ptr<HASegment> ha_segment(new HASegment(backup_server));
        _primary_segments[backup_server] = ha_segment;
    }
    _primary_segments[backup_server]->add(ha_unit);
    _remaining_checkpoints[name] = NUMBER_OF_ITERATION_EPOCH;
    return ha_unit;
}

void HA::merge(HAUnit* source, HAUnit* target)
{
    Name2QBox& boxes = source->get_boxes();
    for (Name2QBox::iterator i = boxes.begin(); i != boxes.end(); i++)
        target->add(i->second);
    _ha_units_primary.erase(_ha_units_primary.find(source->get_name()));
}

void HA::update_statistics(QBox* box, Time time_span) 
{ 
    if (!box)
        return;
    ptr<HAUnit> ha_unit = box->get_ha_unit();
    if (ha_unit)
        ha_unit->add_processing_time(time_span);
}

Time _last_time = Time::now();

void HA::do_ha_task()
{
        Time start_time = Time::now();
        do_remote_checkpoint();
        _last_time = Time::now();
        HA::last_end_time = Time::now();
        HA::ready = false;
}

void HA::setup_remaining_checkpoints()
{
   for (map<string,ptr<HAUnit> >::iterator i = _ha_units_primary.begin();
        i != _ha_units_primary.end(); i++)
        {
            _remaining_checkpoints[i->first] = NUMBER_OF_ITERATION_EPOCH;
        }
}

void HA::update_statistics() 
{
    if (_primary_segments.size() == 0)
    {
        return;
    }

    HASegments::iterator i = _primary_segments.begin();
    while (! i->second->has_boxes())
    {
        i->second->update_statistics();
        i++;
    }
    i->second->update_statistics();
    _recovery_time.set(i->second->get_recovery_time());
    i++;
    for(; i != _primary_segments.end(); i++)
    {
        i->second->update_statistics();
        if (i->second->has_boxes())
        {
            _recovery_time.max(i->second->get_recovery_time());
        }
    }
}

void HA::print_statistics_to_file()
{
    if (_ostream_recovery_time)
    {
        if (_HA_method == PASSIVE_STANDBY)
            _recovery_time.print(_ostream_recovery_time);
    }
    _last_checkpoint_start_time = _current_checkpoint_start_time;
}

void HA::do_remote_checkpoint()
{
    _current_checkpoint_start_time = Time::now();
    ptr<CheckpointMessage> cm = form_checkpoint_message();
    if (cm)
    {
        ptr<HAUnit> ha_unit = cm->get_ha_unit();
        ha_unit->end_capturing();
        _qp.send(cm);
        NOTICE << get_formatted_number((Time::now() - _start_time).to_msecs()*0.001,2) << ": captured to " << ha_unit->get_backup_server() << " HA unit " << cm->get_ha_unit()->get_name() << " for " << get_formatted_number((Time::now() - _current_checkpoint_start_time).to_msecs()*0.001,2) << "(sec).";
        NOTICE << " ";
    }
}

ptr<HAUnit> HA::get_best_ha_unit_primary_round_robin(Time& expected_capture_cost, Time& expected_time)
{
    expected_capture_cost = Time::secs(0);
    expected_time = Time::secs(0);

    ptr<HAUnit> ha_unit;
    Time oldest_capturing_start_time;

    bool first = true;
    for (map<string,ptr<HAUnit> >::iterator i = _ha_units_primary.begin();
        i != _ha_units_primary.end(); i++)
    {
        bool ready = false;
        if (_HA_method == PASSIVE_STANDBY)
        {
            ready = i->second->is_ready_for_checkpoint();
        }
        else if (_HA_method == ACTIVE_STANDBY)
        {
            ready = !(i->second->is_delta_checkpointing()) && i->second->is_ready_for_checkpoint();
        }
        if (ready) // one optimization here would be to filter out unchanged HAUnits
        {
            Time capturing_start_time = i->second->get_last_capturing_start_time();
            if (first || capturing_start_time < oldest_capturing_start_time  )
            {
                first = false;
                oldest_capturing_start_time = capturing_start_time;
                ha_unit = i->second;
            }
        }
    }
    return ha_unit;
}

ptr<HAUnit> HA::get_best_ha_unit_round_robin()
{
    Time expected_capture_cost;
    Time expected_recovery_time;

    ptr<HAUnit> best_primary = get_best_ha_unit_primary_round_robin(expected_capture_cost, expected_recovery_time);

    if (!best_primary)
    {
        if (_pending_checkpoint_messages.size() > 0)
        {
            ptr<CheckpointMessage> cm = _pending_checkpoint_messages[0];
            _pending_checkpoint_messages.erase(_pending_checkpoint_messages.begin());
            consume(cm);
        }
        return best_primary;
    }
    else 
    {
        if (_pending_checkpoint_messages.size() > 0) // if both capture and paste tasks exist.
        {
            ptr<CheckpointMessage> cm = _pending_checkpoint_messages[0];
            ptr<HAUnit> best_backup = cm->get_ha_unit();
            if (best_backup->_last_capturing_end_time < best_primary->_last_update_end_time) // if best paste came eariler than best capture.
            {
                _pending_checkpoint_messages.erase(_pending_checkpoint_messages.begin());
                consume(cm);
                return ptr<HAUnit>(); // don't do a capature
            }
            else
                return best_primary;
        }
        else  // if no paste task
            return best_primary;
    }
    return ptr<HAUnit>(); // give it up!
}

ptr<HAUnit> HA::get_oldest_checkpointable()
{
    ptr<HAUnit> ha_unit;
    Time oldest_time = Time::secs(0);
    for (HAUnits::iterator i = _ha_units_primary.begin(); i != _ha_units_primary.end(); i++)
    {
        if (i->second->is_ready_for_checkpoint())
        {
            Time t = i->second->_last_update_end_time;
            if (oldest_time == Time::secs(0) || t < oldest_time)
            {
                oldest_time = t;
                ha_unit = i->second;
            }
        }
    }
    return ha_unit;
}

string HA::get_checkpointables(ptr<HASegment> segment)
{
    string checkpointables;
    HAUnits& ha_units = segment->get_ha_units();
    for (map<string,ptr<HAUnit> >::iterator i = ha_units.begin(); i != ha_units.end(); i++)
    {
        if (i->second->is_ready_for_checkpoint())
        {
            double load = i->second->get_processing_load()*100.0;
            double staleness = i->second->get_expected_staleness(Time::now()).to_msecs()*0.001;
            double recovery_time = i->second->get_expected_recovery_time(Time::now()).to_msecs()*0.001;
            string checkpointable = i->second->get_name() + ":"
                + get_formatted_number(load, 1) + ":" 
                + get_formatted_number(staleness, 1)+":"
                + get_formatted_number(recovery_time, 1);
            if (checkpointables == "")
                checkpointables += checkpointable;
            else
                checkpointables += (", " + checkpointable);
         }
    }
    return checkpointables;
}

ptr<HAUnit> HA::get_best_ha_unit_primary_min_max(Time& expected_capture_cost, Time& expected_time, bool try_best)
{
    expected_capture_cost = Time::secs(0);
    expected_time = Time::secs(0);

    vector<ptr<HASegment> > sorted;
    vector<Time > times;
    Time current_time = Time::now();
    ptr<HAUnit> ha_unit;
    for(HASegments::iterator i = _primary_segments.begin(); i != _primary_segments.end(); i++)
    {
        ptr<HASegment> segment = i->second;
        Time recovery_time = segment->get_expected_recovery_time(ptr<HAUnit>(), current_time);
        NOTICE << "-> " << i->first << " " << recovery_time.to_msecs() << " " << segment->get_expected_staleness(ptr<HAUnit>(), current_time).to_msecs()
        << " {" << get_checkpointables(segment) << "}";
        unsigned int j = 0;
        unsigned int size = sorted.size();
        for (; j < size; j++)
        {
            if (times[j] > recovery_time) {
                times.insert(times.begin()+j, recovery_time);
                sorted.insert(sorted.begin()+j, segment);
                break;
            }
        }
        if (j == size)
        {
            times.push_back(recovery_time);
            sorted.push_back(segment);
        }
    }
    for (int j = sorted.size()-1; j >=0; j--) 
    {
        ptr<HASegment> segment = sorted[j];
//        NOTICE << segment->get_backup_server();
        HAUnits& ha_units = segment->get_ha_units();
        double max_reduction = -1000;
        for (map<string,ptr<HAUnit> >::iterator i = ha_units.begin(); i != ha_units.end(); i++)
        {
            if (i->second->is_ready_for_checkpoint())
            {
                ptr<HAUnit> h = i->second;
                Time capture_cost = h->get_expected_capture_cost();
                if (capture_cost == 0)
                    continue; // consider this case later!!!
                Time staleness = Time::now() - h->_last_effective_capturing_start_time;
                double reduction = (staleness + capture_cost).to_msecs()* h->get_processing_load()+h->get_expected_paste_cost().to_msecs();
                reduction = reduction/capture_cost.to_usecs();
                if (reduction > max_reduction)
                {
                    max_reduction = reduction;
                    ha_unit = h;
                }
            }
        }
        if (ha_unit)
        {
            expected_capture_cost = ha_unit->get_expected_capture_cost();
//            expected_time = segment->get_expected_recovery_time(ptr<HAUnit>(), Time::now());
            expected_time = get_expected_recovery_time_primary(ha_unit, Time::now());
            NOTICE << "best primary: " << ha_unit->get_name() << " -> " << ha_unit->get_backup_server() <<  " out of {" << get_checkpointables(segment) << "}";
            return ha_unit;
        }
        else if (!try_best)
        {
            expected_capture_cost = Time::secs(0);
            expected_time = get_expected_recovery_time_primary(ptr<HAUnit>(), Time::now());
            return ha_unit;
        }
    }
    NOTICE << "no best primary:";
    return ha_unit;

/*
    Time start_time = Time::now();
    ptr<HAUnit> ha_unit;
    Time max_reduction = Time::secs(-10000);
    for (map<string,ptr<HAUnit> >::iterator i = _ha_units_primary.begin();
        i != _ha_units_primary.end(); i++)
    {
        if (i->second->is_ready_for_checkpoint())
        {
            expected_capture_cost = i->second->get_expected_capture_cost();
            Time e_time = Time::now() + expected_capture_cost;
            Time expected_recovery_time = get_expected_recovery_time_primary(i->second, e_time);
            Time not_doing = get_expected_recovery_time_primary(ptr<HAUnit>(), e_time);
            Time reduction = not_doing - expected_recovery_time;
            if (reduction > max_reduction)
            {
                max_reduction = reduction;
                ha_unit = i->second;
                expected_time = not_doing;
            }
        }
    }
    return ha_unit;
    Time start_time = Time::now();
    ptr<HAUnit> ha_unit;
    double max_reduction = -10000;
    Time maximum_recovery_time = Time::secs(-10000);
    map<MedusaID, Time> recovery_times;
    ptr<HAUnit> first;
    for (map<string,ptr<HAUnit> >::iterator i = _ha_units_primary.begin();
        i != _ha_units_primary.end(); i++)
    {
        ptr<HAUnit> h = i->second;
        Time capture_cost = h->get_expected_capture_cost();
        if (h->is_ready_for_checkpoint())
        {
            if (!first)
                first = h;
            else if (h->get_last_capturing_start_time() < first->get_last_capturing_start_time())
                first = h;
            MedusaID backup_server = h->get_backup_server();
            ptr<HASegment> segment = _primary_segments[backup_server];
            if (segment->get_expected_recovery_time(ptr<HAUnit>(), start_time+capture_cost)
                >= get_expected_recovery_time_primary(ptr<HAUnit>(), start_time+capture_cost))
            {
                if (capture_cost == 0)
                    continue; // consider this case later!!!
                Time staleness = Time::now() - h->_last_effective_capturing_start_time;
                double reduction = (staleness + capture_cost).to_msecs()* h->get_processing_load()+h->get_expected_paste_cost().to_msecs();
                reduction = reduction/capture_cost.to_msecs();
                if (reduction > max_reduction)
                {
                    max_reduction = reduction;
                    ha_unit = h;
                }
            }
        }
    }
    if (!ha_unit && first)
    {
        ha_unit = first;
    }
    if (ha_unit)
    {
        expected_capture_cost = ha_unit->get_expected_capture_cost();
        expected_time = get_expected_recovery_time_primary(ha_unit, Time::now() + expected_capture_cost);
        NOTICE << expected_time.to_msecs();
    }
    return ha_unit;
*/
}

int HA::find_best_checkpoint_message(Time& max_recovery_time)
{
    max_recovery_time = Time::secs(0);

    map<string, Time> recovery_times;
    int best_index = -1;
    Time t = Time::now();
    max_recovery_time = Time::secs(-1000);
    for (unsigned int i = 0; i < _pending_checkpoint_messages.size(); i++)
    {
        ptr<CheckpointMessage> cm = _pending_checkpoint_messages[i];
        string primary_server = cm->get_primary_server();
        if (recovery_times.find(primary_server) == recovery_times.end())
        {
            ptr<HASegment> backup_segment = _backup_segments[primary_server];
            Time recovery_time = backup_segment->get_expected_recovery_time(ptr<HAUnit>(), t);
            NOTICE << "<-" << primary_server << " " << recovery_time.to_msecs() << " " << backup_segment->get_expected_staleness(ptr<HAUnit>(), t).to_msecs()
            << " {" << get_checkpointables(backup_segment) << "}";
            recovery_times[primary_server] = recovery_time;
            if (recovery_time > max_recovery_time)
            {
                max_recovery_time = recovery_time;
                best_index = i;
            }
        }
    }
    if (best_index >= 0)
    {
        ptr<CheckpointMessage> cm = _pending_checkpoint_messages[best_index];
        NOTICE << "best backup: " << cm->get_ha_unit()->get_name() << " <- " << cm->get_primary_server();
    }
    return best_index;  // cannot return -1 because this method is called only when there are checkpoint messages.
}

/*
ptr<HAUnit> HA::get_best_ha_unit_min_max()
{
    Time expected_capture_cost;
    Time expected_recovery_time;
    ptr<HAUnit> best_primary = get_best_ha_unit_primary_min_max(expected_capture_cost, expected_recovery_time);

    if (!best_primary)
    {
//        expected_recovery_time = get_expected_recovery_time(ptr<HAUnit>(), Time::now());
        if (_pending_checkpoint_messages.size() > 0)
        {
            Time recovery_time;
            int i = find_best_checkpoint_message(recovery_time);
            ptr<CheckpointMessage> cm = _pending_checkpoint_messages[i];
            _pending_checkpoint_messages.erase(_pending_checkpoint_messages.begin()+i);
            NOTICE << "::: no primary " << recovery_time.to_msecs();
            consume(cm); // consume the checkpoint message
        }
        return ptr<HAUnit>();
    }
    else // if best_primary exists.
    {
        Time max_recovery_time = Time::secs(0);
        if (_pending_checkpoint_messages.size() > 0) // if both capture and paste tasks exist.
        {
            Time recovery_time_backup;
            int i = find_best_checkpoint_message(recovery_time_backup);
            NOTICE << ":::" << expected_recovery_time.to_msecs() << ", "
                << recovery_time_backup.to_msecs() << ", "
                << _pending_checkpoint_messages.size();
            if (recovery_time_backup > expected_recovery_time) // the backup segment really needs care.
            {
                ptr<CheckpointMessage> cm = _pending_checkpoint_messages[i];
                _pending_checkpoint_messages.erase(_pending_checkpoint_messages.begin()+i);
                consume(cm);    // consume the checkpoint message!
                return ptr<HAUnit>();
            }
            else
            {
                ptr<HASegment> segment = _primary_segments[best_primary->get_backup_server()];
                Time alpha = segment->get_last_effective_capturing_start_time();
                Time current = Time::now();
                Time next = Time::now()+expected_capture_cost;
            //    NOTICE << get_formatted_number((current - _start_time).to_msecs()*0.001,2) << ":"                     << segment->get_expected_recovery_time(ptr<HAUnit>(), current).to_msecs() << ", "                     << segment->get_expected_recovery_time(best_primary, next).to_msecs() << ", "                     << get_expected_recovery_time_primary(best_primary, next).to_msecs();
                return best_primary; // do the best capture
            }
        }
        else  // if no paste task
            return best_primary; // do the best capture
    }
    return ptr<HAUnit>(); // give it up!
}
*/

/*
ptr<HAUnit> HA::get_best_ha_unit_min_max()
{
    Time expected_capture_cost;
    Time expected_recovery_time_primary, expected_recovery_time_backup;
    ptr<HAUnit> best_primary = get_best_ha_unit_primary_min_max(expected_capture_cost, expected_recovery_time_primary, false);

    ptr<HAUnit> oldest = get_oldest_checkpointable();

    int i = find_best_checkpoint_message(expected_recovery_time_backup);
    NOTICE << ":::" << expected_recovery_time_primary.to_msecs() << ", "
        << expected_recovery_time_backup.to_msecs() << ", "
        << _pending_checkpoint_messages.size();
    
    if ((i >= 0 && ( !oldest || expected_recovery_time_primary < expected_recovery_time_backup)))
    {
        ptr<CheckpointMessage> cm = _pending_checkpoint_messages[i];
        _pending_checkpoint_messages.erase(_pending_checkpoint_messages.begin()+i);
        consume(cm);    // consume the checkpoint message!
        return ptr<HAUnit>(); // skip capture
    }
    else
    {
        if (best_primary)
            return best_primary;
        return oldest;
    }
}
*/

ptr<HAUnit> HA::get_best_ha_unit_min_max()
{
    Time expected_capture_cost;
    Time expected_recovery_time_primary, expected_recovery_time_backup;
    ptr<HAUnit> best_primary = get_best_ha_unit_primary_min_max(expected_capture_cost, expected_recovery_time_primary, true);

    int i = find_best_checkpoint_message(expected_recovery_time_backup);
    NOTICE << ":::" << expected_recovery_time_primary.to_msecs() << ", "
        << expected_recovery_time_backup.to_msecs() << ", "
        << _pending_checkpoint_messages.size();
    
    if ((i >= 0 && ( !best_primary || expected_recovery_time_primary < expected_recovery_time_backup)))
    {
        ptr<CheckpointMessage> cm = _pending_checkpoint_messages[i];
        _pending_checkpoint_messages.erase(_pending_checkpoint_messages.begin()+i);
        consume(cm);    // consume the checkpoint message!
        return ptr<HAUnit>(); // skip capture
    }
    else
    {
        return best_primary;
    }
}

ptr<HAUnit> HA::get_best_ha_unit()
{
    if (_checkpoint_method == ROUND_ROBIN || _checkpoint_method == WHOLE  )
    {
        return get_best_ha_unit_round_robin();
    }
    else if (_checkpoint_method == MIN_MAX)
    {
        if ((Time::now() - _start_time) <= Time::secs(60))
            return get_best_ha_unit_round_robin();
        else
            return get_best_ha_unit_min_max();
    }
    return ptr<HAUnit>(); // give it up!
}

Time HA::get_expected_recovery_time_primary(ptr<HAUnit> ha_unit, Time target_time)
{
    Time max_recovery_time = Time::secs(-1000);
    for(HASegments::iterator i = _primary_segments.begin(); i != _primary_segments.end(); i++)
    {
        Time recovery_time = i->second->get_expected_recovery_time(ha_unit, target_time);
        if (recovery_time > max_recovery_time)
            max_recovery_time = recovery_time;
    }
    return max_recovery_time;
}

void HA::add_expected_capture_cost(ptr<HAUnit> ha_unit)
{
    if (ha_unit)
        ha_unit->add_expected_capture_cost(ha_unit->get_expected_capture_cost());
}

void HA::add_expected_paste_cost(ptr<HAUnit> ha_unit)
{
    if (ha_unit)
        ha_unit->add_expected_paste_cost(ha_unit->get_expected_paste_cost());
}

ptr<CheckpointMessage> HA::form_checkpoint_message(ptr<HAUnit> ha_unit)
{
    ha_unit->start_capturing();
    add_expected_capture_cost(ha_unit);
    add_expected_paste_cost(ha_unit);
    int new_windows = 0;
    int updated_windows = 0;
    Time expected_capture_cost = ha_unit->get_expected_capture_cost();
    Time expected_paste_cost = ha_unit->get_expected_paste_cost();

    Time start_time = Time::now();

    ptr<CheckpointMessage> cm(new CheckpointMessage(_id, ha_unit));
    Name2QBox& boxes = ha_unit->get_boxes();
    for (Name2QBox::iterator i = boxes.begin(); i != boxes.end(); i++)
    {
        try 
        {
            ptr<AggregateQBox> aggr = i->second.dynamic_cast_to<AggregateQBox>();
            new_windows += aggr->count_new_windows();
            updated_windows += aggr->count_updated_windows();
        }
        catch (...) {}
        string name = i->first.as_string();
        add(name, cm, ha_unit->_delta_checkpointing);
    }
    Time time_span = Time::now() - start_time;
    ha_unit->add_capture_cost(time_span);
    if (_ostream_statistics)
    {
        (*_ostream_statistics) << Time::now().to_msecs() << " " << ha_unit->get_name() 
            << " " << get_formatted_number((int)(ha_unit->get_number_of_boxes()), 2) << " 1 "
            << get_formatted_number(100.0*ha_unit->get_processing_load(), 2) << " "
            << ha_unit->get_last_capture_cost().to_msecs() << " " 
            << -1 << " " // undefined
            << expected_capture_cost.to_msecs() << " " 
            << expected_paste_cost.to_msecs() << " "
            << new_windows << " " << updated_windows << endl;
    }
    return cm;
}

ptr<CheckpointMessage> HA::form_checkpoint_message()
{
    ptr<HAUnit> ha_unit = get_best_ha_unit();
    if (ha_unit)
    {
        return form_checkpoint_message(ha_unit);
    }
    return ptr<CheckpointMessage>();
}

void HA::consume(ptr<CheckpointMessage> cm)
{
    Time start_time = Time::now();
    ptr<HAUnit> ha_unit = cm->get_ha_unit();
    string ha_unit_name = ha_unit->get_name();
    ha_unit->start_update();
    typedef pair< ptr<AbstractQueueState>, ptr<AbstractBoxState> >BoxState;
    ptr<BoxPackage> box_package = cm->get_box_package();
    if (_HA_method == PASSIVE_STANDBY)
    {
        for (map<string, BoxState>::iterator i = box_package->begin(); i != box_package->end(); i++)
        {
            string box_name = i->first;
            BoxState& box_state = i->second;
            ptr<QueueState> qs = box_state.first.dynamic_cast_to<QueueState>();
            ptr<AbstractBoxState> bs = box_state.second;
            _qp.update_box_state(box_name, bs);
            box_state = BoxState(ptr<QueueState>(), ptr<AbstractBoxState>());
        }
    }
    Time delay = Time::now() - start_time;
    ha_unit->add_paste_cost(delay);
    _qp.ack(cm);
    NOTICE << get_formatted_number((Time::now() - _start_time).to_msecs()*0.001,2) << ": pasted HA unit " << cm->get_ha_unit()->get_name() << " for " << get_formatted_number(delay.to_msecs()*0.001,2) << "(sec).";
    NOTICE << " ";
    if (_ostream_statistics)
    {
        ptr<HAUnit> h = _ha_units_backup[ha_unit_name];
        int new_windows = 0;
        int updated_windows = 0;
        Name2QBox& boxes = h->get_boxes();
        for (Name2QBox::iterator i = boxes.begin(); i != boxes.end(); i++)
        {
            try 
            {
                ptr<AggregateQBox> aggr = i->second.dynamic_cast_to<AggregateQBox>();
                new_windows += aggr->count_new_windows();
                updated_windows += aggr->count_updated_windows();
            }
            catch (...) {}
        }
        (*_ostream_statistics) << Time::now().to_msecs() << " " << ha_unit_name 
            << " " << get_formatted_number((int)(box_package->size()), 2) << " 2 "
            << get_formatted_number(100.0*ha_unit->get_processing_load(),2) << " "
            << ha_unit->get_last_capture_cost().to_msecs() << " " 
            << ha_unit->get_last_paste_cost().to_msecs() << " "
            << h->get_expected_capture_cost().to_msecs() << " " 
            << h->get_expected_paste_cost().to_msecs() << " "
            << new_windows << " " << updated_windows << endl;
    }
}

void HA::instantiate_boxes(ptr<CheckpointMessage> cm)
{
    Time start_time = Time::now();
    vector<string> xml_strings = cm->get_xml_strings();
    for (register unsigned int j = 0; j < xml_strings.size(); j++)
    {
        string s = xml_strings[j];
        _qp.add_xml_string(s);
    }
}

// --------------------------------------------------
void HA::add_xml_strings(string box_name, ptr<CheckpointMessage> cm)
{    
    RPC<vector<string> > result = _qp.get_xml_strings(box_name);
    if (result.valid())
    {
        for (register unsigned int i = 0; i < result->size(); i++)
        {
            cm->add((*result)[i]);
        }
    }
}

void HA::add(string box_name, ptr<CheckpointMessage> cm, bool delta_checkpointing)
{
    if (!delta_checkpointing) add_xml_strings(box_name, cm);
    if (_HA_method == PASSIVE_STANDBY)
    {
        RPC<ptr<AbstractBoxState> > result = _qp.get_box_state(box_name);
        if (result.valid())
        {
            cm->get_box_package()->add_box(box_name, ptr<QueueState>(), *result);
        }
    }
}

void HA::end_epoch()
{
    if (_tentative_epoch == _epoch)
        _tentative_epoch++;

    // ask all others to see if their tentative epoch > _epoch
    if (true) // if so
    {
        update_statistics();
        print_statistics_to_file();
        NOTICE << get_formatted_number((Time::now() - _start_time).to_msecs()*0.001,2) << " end of epoch " << _epoch << ": recovery time = " << get_formatted_number(_recovery_time.get_average(), 3) << ".";
        NOTICE << _recovery_time;
        _recovery_time.clear();
        _epoch++;
        setup_remaining_checkpoints();
    }
    else
    {
    }
} 

double HA::get_backup_staleness()
{
    int size = _ha_units_backup.size();
    if (size == 0)
        return 0;
    double sum_staleness = 0;
    for (map<string,ptr<HAUnit> >::iterator i = _ha_units_backup.begin();
        i != _ha_units_backup.end(); i++)
    {
        double staleness = i->second->get_staleness();
        sum_staleness += staleness;
    }
    long staleness = (long)(sum_staleness/size);
    NOTICE << get_formatted_number((Time::now() - _start_time).to_msecs()*0.001,2) << ", "  << staleness;
    if (_HA_method == ACTIVE_STANDBY)
        (*_ostream_recovery_time) << Time::now().to_msecs() << ", "  << staleness << endl;  
        // this part is not yet complete:
        // 1. the staleness info does not yet reflect the processing load
        // 2. this staleness info has to be sent to the primaries.
    double fake_processing_load = 0.5;

    return staleness*fake_processing_load; //          /2 because only the backups will be executed at failover.
}

void HA::push_processing_times()
{
    HASegments::iterator i, oldest;
    vector<string> names;
    vector<ptr<SlidingWindowStatistics> > processing_times;
    locking(_lock)
    {
        if (_primary_segments.size() == 0)
            return;
        i = _primary_segments.begin();
        oldest = i;
        Time oldest_push_time = i->second->get_last_push_time();
        i++;
        while (i != _primary_segments.end())
        {
            Time last_push_time = i->second->get_last_push_time();
            if (last_push_time < oldest_push_time)
            {
                oldest = i;
                oldest_push_time = last_push_time;
            }
            i++;
        }
        HAUnits& ha_units = oldest->second->get_ha_units();
        for (HAUnits::iterator j = ha_units.begin(); j != ha_units.end(); j++)
        {
            names.push_back(j->first);
            processing_times.push_back(j->second->get_processing_times());
        }
        oldest->second->set_last_push_time();
    }
    _qp.push_processing_times(oldest->first, names, processing_times);
}

void HA::update_processing_times(MedusaID primary, vector<string> ha_units, vector<ptr<SlidingWindowStatistics> > processing_times)
{
    locking(_lock)
    {
        for (register unsigned int i = 0; i < ha_units.size(); i++)
        {
            string ha_unit_name = ha_units[i];
            if (_ha_units_backup.find(ha_unit_name) != _ha_units_backup.end())
            {
                ptr<HAUnit> ha_unit = _ha_units_backup[ha_unit_name];
//                double l1 = ha_unit->get_processing_load();
                ha_unit->set_processing_times(*(processing_times[i]));
//                NOTICE << ":*:" << get_formatted_number(l1, 2) << " " << get_formatted_number(ha_unit->get_processing_load(), 2);
            }
        }
    }
}

BOREALIS_NAMESPACE_END
