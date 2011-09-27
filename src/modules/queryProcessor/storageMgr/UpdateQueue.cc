
#include <UpdateQueue.h>
#include <TupleQueue.h>
#include <Stream.h>

BOREALIS_NAMESPACE_BEGIN;

void UpdateQueue::enqueue_fifo()
{
    if (!_q)
    {
        WARN << "UpdateQueue has no corresponding TupleQueue!";
        return;
    }

    DEBUG << "Size of the tuple queue is = " << _q->size();

    string key_value;
    if (_key_field != "")
    {
        char *key_location = (char*)_q->_enq_location + _key_offset;
        key_value = string(key_location, _key_size);
    }
    else
    {
        key_value = "none";
    }
    DEBUG << "key value = " << key_value;

    char *window_location = (char*)_q->_enq_location + _window_offset;
    int32 window_value = int32(*((int32 *)window_location));
    DEBUG << "window value = " << window_value;

    char *tuple = (char*)_q->_enq_location;
    Tuple input_tuple = Tuple::factory(tuple);
    int win_spec = input_tuple.get_win_spec();
    DEBUG << "win_spec = " << win_spec;

    // First add the new cell.
    //
    DataCell *cell = new DataCell();
    cell->_data_location = _q->_enq_location;

    if (is_empty())
    {
        DEBUG << "adding to an empty index";
        cell->_prev_cell = NULL;
        cell->_next_cell = NULL;
        _head = cell;
        _tail = cell;
    }
    else
    {
        DEBUG << "adding to a non-empty index";
        assert(_head); 
        assert(_tail);
        cell->_prev_cell = _tail;
        cell->_next_cell = NULL;
        _tail->_next_cell =  cell;
        _tail = cell;
    }

    DEBUG << "newly added cell = " << cell;
    DEBUG << cell->as_string(_q->get_stream()->get_tuple_description());

    // Then update the window queue information.
    //
    if ((_hash_index.find(key_value) == _hash_index.end()) ||
        (_hash_index[key_value] == NULL))
    {
        DEBUG << "window queue for key " << key_value << " does not exist";

        // Initialize the update count if it is the first time we see this key.
        // Create the random number sequences.
        //
        if (_hash_index.find(key_value) == _hash_index.end())
        {
            assert(_update_counts.find(key_value) == _update_counts.end());
            _update_counts[key_value] = 0;
            _win_dequeue_counts[key_value] = 0;
            srand48(time(NULL));
            for (unsigned int i = 0; i < RANDOM_NUMBER_COUNT; i++)
            {
                double d = drand48();
                _random_numbers[key_value].push(d);
            }
        }

        _hash_index[key_value] = new WindowQueue(key_value);

        if (win_spec > 0)
        {
            _hash_index[key_value]->_recent_win_start = cell;
            (_hash_index[key_value]->_arrival_time).tv_sec = 
                                    ((timeval *)cell->_data_location)->tv_sec;
            (_hash_index[key_value]->_arrival_time).tv_usec = 
                                    ((timeval *)cell->_data_location)->tv_usec;

            ++_num_queued_windows;

            // Update UF stats.
            //
            ++_update_counts[key_value];
            ++_total_update_count;
            if (_update_counts[key_value] > _max_update_count)
            {
                _max_update_count = _update_counts[key_value];
            }

            /*
            double current_time = static_cast<double>(Time::now().to_msecs());
            _update_times[key_value].push(current_time);
            double oldest = _update_times[key_value].front();
            while ((current_time - oldest) >= _stats_window)
            {
                _update_times[key_value].pop();
                assert(_update_times[key_value].size() > 0);
                oldest = _update_times[key_value].front();
            }
            */

            /*
            // should we shed load ?
            //
            char *ls = getenv("LOAD_SHEDDING");
            if (ls[0] == '1')
            {
                DEBUG << "load shedding is on";
                if (_num_queued_windows > _max_num_windows)
                {
                    DEBUG << "will shed load";
                    char *mode = getenv("RANDOM_LS");
                    if (mode[0] == '0')
                    {
                        DEBUG << "high-low";
                        string drop_key = pick_drop_key();
                        DEBUG << "key to drop = " << drop_key;
                        drop_window(drop_key);
                    }
                    else if (mode[0] == '1')
                    {
                        DEBUG << "random";
                        string drop_key = pick_random_drop_key();
                        DEBUG << "key to drop = " << drop_key;
                        drop_window(drop_key);
                    }
                }
            }
            */
        }
    }
    else
    {
        DEBUG << "window queue for key " << key_value << " exists";

        if (win_spec > 0)
        {
            if (_hash_index[key_value]->_recent_win_start)
            {
                // Apply early drops.
                //
                int32 win_val;

                DataCell *i = _hash_index[key_value]->_recent_win_start;
                while (i)
                {
                    char *tup = (char*)i->_data_location;
                    char *win_loc = tup + _window_offset;
                    win_val = int32(*((int32 *)win_loc));
                    char *key_loc = tup + _key_offset;
                    string key_val = string(key_loc, _key_size);
                    DEBUG << "win_val = " << win_val;
                    DEBUG << "key_val = " << key_val;

                    if (key_val != key_value)
                    {
                        i = i->_next_cell;
                        continue;
                    }

                    if (win_val >= (_hash_index[key_value]->_keep_until))
                    {
                        break;
                    }

                    // If we are keeping a window starter due to scope,
                    // we must disable its spec.
                    //
                    if (i == _hash_index[key_value]->_recent_win_start)
                    {
                        Tuple disable = Tuple::factory(tup);
                        disable.set_win_spec(0);
                    }

                    i = i->_next_cell;
                }

                bool fake_flag = false;

                while (i && (i != cell))
                {
                    // Drop the data cell if it has the right key.
                    //
                    char *tup = (char*)i->_data_location;
                    char *key_loc = tup + _key_offset;
                    string key_val = string(key_loc, _key_size);
                    Tuple tup_fac = Tuple::factory(tup);
                    fake_flag = tup_fac.get_fake_flag();
                    int w_spec = tup_fac.get_win_spec();

                    DataCell *temp = i->_next_cell;

                    if ((key_val == key_value) && (!fake_flag))
                    {
                        if (i->_prev_cell)
                        {
                            i->_prev_cell->_next_cell = i->_next_cell;
                            i->_next_cell->_prev_cell = i->_prev_cell;
                        }
                        else // i must be the _head.
                        {
                            assert(i == _head);
                            _head = i->_next_cell;
                            i->_next_cell->_prev_cell = NULL;
                        }

                        _garbage.insert(i->_data_location);

                        if (w_spec > 0)
                        {
                            --_num_queued_windows;
                        }

                        delete i;
                    }

                    i = temp;
                }
            }

            _hash_index[key_value]->_recent_win_start = cell;
            (_hash_index[key_value]->_arrival_time).tv_sec = 
                                    ((timeval *)cell->_data_location)->tv_sec;
            (_hash_index[key_value]->_arrival_time).tv_usec = 
                                    ((timeval *)cell->_data_location)->tv_usec;

            ++_num_queued_windows;

            // Update UF stats.
            //
            ++_update_counts[key_value];
            ++_total_update_count;
            if (_update_counts[key_value] > _max_update_count)
            {
                _max_update_count = _update_counts[key_value];
            }

            /*
            double current_time = static_cast<double>(Time::now().to_msecs());
            _update_times[key_value].push(current_time);
            double oldest = _update_times[key_value].front();
            while ((current_time - oldest) >= _stats_window)
            {
                _update_times[key_value].pop();
                assert(_update_times[key_value].size() > 0);
                oldest = _update_times[key_value].front();
            }
            */

            /*
            // should we shed load ?
            //
            char *ls = getenv("LOAD_SHEDDING");
            if (ls[0] == '1')
            {
                DEBUG << "load shedding is on";
                if (_num_queued_windows > _max_num_windows)
                {
                    DEBUG << "will shed load";
                    char *mode = getenv("RANDOM_LS");
                    if (mode[0] == '0')
                    {
                        DEBUG << "high-low";
                        string drop_key = pick_drop_key();
                        DEBUG << "key to drop = " << drop_key;
                        drop_window(drop_key);
                    }
                    else if (mode[0] == '1')
                    {
                        DEBUG << "random";
                        string drop_key = pick_random_drop_key();
                        DEBUG << "key to drop = " << drop_key;
                        drop_window(drop_key);
                    }
                }
            }
            */
        }
    }
}

void UpdateQueue::enqueue_inplace()
{
    if (!_q)
    {
        WARN << "UpdateQueue has no corresponding TupleQueue!";
        return;
    }

    DEBUG << "Size of the tuple queue is = " << _q->size();
    DEBUG << "Size of the garbage set is = " << _garbage.size();
    DEBUG << "Size of the hash index is = " << _hash_index.size();
    DEBUG << "_q->_enq_location = " << _q->_enq_location;
    DEBUG << "_q->_deq_location = " << _q->_deq_location;

    string key_value;
    if (_key_field != "")
    {
        char *key_location = (char*)_q->_enq_location + _key_offset;
        key_value = string(key_location, _key_size);
    }
    else
    {
        key_value = "none";
    }
    DEBUG << "key value = " << key_value;

    char *window_location = (char*)_q->_enq_location + _window_offset;
    int32 window_value = int32(*((int32 *)window_location));
    DEBUG << "window value = " << window_value;

    char *tuple = (char*)_q->_enq_location;
    Tuple input_tuple = Tuple::factory(tuple);
    int win_spec = input_tuple.get_win_spec();
    DEBUG << "win_spec = " << win_spec;

    // First check if there exists a cell for that key.
    //
    if ((_hash_index.find(key_value) == _hash_index.end()) ||
        (_hash_index[key_value] == NULL))
    {
        DEBUG << "window queue for key " << key_value << " does not exist";

        // We'll create a new cell.
        //
        DataCell *cell = new DataCell();
        cell->_data_location = _q->_enq_location;

        if (is_empty())
        {
            DEBUG << "adding to an empty index";
            cell->_prev_cell = NULL;
            cell->_next_cell = NULL;
            _head = cell;
            _tail = cell;
        }
        else
        {
            DEBUG << "adding to a non-empty index";
            assert(_head); 
            assert(_tail);
            cell->_prev_cell = _tail;
            cell->_next_cell = NULL;
            _tail->_next_cell =  cell;
            _tail = cell;
        }

        DEBUG << "newly added cell = " << cell;
        DEBUG << cell->as_string(_q->get_stream()->get_tuple_description());

        // Initialize the update count if it is the first time we see this key.
        // Create the random number sequences.
        //
        if (_hash_index.find(key_value) == _hash_index.end())
        {
            assert(_update_counts.find(key_value) == _update_counts.end());
            _update_counts[key_value] = 0;
            _win_dequeue_counts[key_value] = 0;
            srand48(time(NULL));
            for (unsigned int i = 0; i < RANDOM_NUMBER_COUNT; i++)
            {
                double d = drand48();
                _random_numbers[key_value].push(d);
            }
        }

        _hash_index[key_value] = new WindowQueue(key_value);

        if (win_spec > 0)
        {
            _hash_index[key_value]->_recent_win_start = cell;
            (_hash_index[key_value]->_arrival_time).tv_sec = 
                                    ((timeval *)cell->_data_location)->tv_sec;
            (_hash_index[key_value]->_arrival_time).tv_usec = 
                                    ((timeval *)cell->_data_location)->tv_usec;

            ++_num_queued_windows;

            // Update UF stats.
            //
            ++_update_counts[key_value];
            ++_total_update_count;
            if (_update_counts[key_value] > _max_update_count)
            {
                _max_update_count = _update_counts[key_value];
            }
        }
    }
    else
    {
        DEBUG << "window queue for key " << key_value << " exists";

        if (_hash_index[key_value]->_recent_win_start)
        {
            // We'll create a new cell if it belongs to the window in the queue.
            // But add it to the end of the queue chunk for that key.
            // Assuming that it is in the scope of the most recent one.
            //
            if (win_spec <= 0)
            {
                DataCell *cell = new DataCell();
                cell->_data_location = _q->_enq_location;

                DEBUG << "adding to a non-empty index";
                assert(_head); assert(_tail);

                // Traverse backwards from 
                // _hash_index[key_value]->_recent_win_start
                // until another key window boundary.
                //
                DataCell *i = _hash_index[key_value]->_recent_win_start;
                bool done = false;
                while ((!done) && (i))
                {
                    char *tup = (char*)i->_data_location;
                    char *key_loc = tup + _key_offset;
                    string key_val = string(key_loc, _key_size);
                    DEBUG << "key_val = " << key_val;
                    if (key_val == key_value)
                    {
                        i = i->_next_cell;
                        continue;
                    }

                    // Insert the new cell between i and i->prev, and break.
                    //
                    cell->_prev_cell = i->_prev_cell;
                    i->_prev_cell->_next_cell = cell;
                    cell->_next_cell = i;
                    i->_prev_cell = cell;
                
                    done = true;
                }
                if (!done)
                {
                    // Insert the new cell to the tail.
                    //
                    cell->_prev_cell = _tail;
                    cell->_next_cell = NULL;
                    _tail->_next_cell =  cell;
                    _tail = cell;
                    done = true;
                }
            }
            // If it's a new window, we'll overwrite the old one, removing the
            // rest of its members. But, don't delete the ones in the scope!
            // And if a window starter is in scope, 
            // don't forget to disable that!
            //
            else if (win_spec > 0)
            {
                // Update UF stats, but don't increase the num_queued_windows.
                //
                ++_update_counts[key_value];
                ++_total_update_count;
                if (_update_counts[key_value] > _max_update_count)
                {
                    _max_update_count = _update_counts[key_value];
                }

                DataCell *i = _hash_index[key_value]->_recent_win_start;

                int32 win_val;
                i = i->_next_cell; // Hold on to the recent for now.
                while (i)
                {
                    char *tup = (char*)i->_data_location;
                    char *win_loc = tup + _window_offset;
                    win_val = int32(*((int32 *)win_loc));
                    char *key_loc = tup + _key_offset;
                    string key_val = string(key_loc, _key_size);
                    Tuple tup_fac = Tuple::factory(tup);
                    bool fake_flag = tup_fac.get_fake_flag();
                    DEBUG << "win_val = " << win_val;
                    DEBUG << "key_val = " << key_val;
                    DEBUG << "fake_flag = " << fake_flag;
                    if (key_val == key_value)
                    {
                        // Remove i only if not in scope and not fake_flag.
                        //
                        if ((win_val >= (_hash_index[key_value]->_keep_until))
                            && (!fake_flag))
                        {
                            _garbage.insert(i->_data_location);

                            if (i == _tail)
                            {
                                _tail = i->_prev_cell;
                                i->_prev_cell->_next_cell = NULL;
                                delete i;
                                i = NULL;
                                break;
                            }
                            else
                            {
                                assert(i->_next_cell);
                                DataCell *temp = i->_next_cell;
                                i->_prev_cell->_next_cell = i->_next_cell;
                                i->_next_cell->_prev_cell = i->_prev_cell;
                                delete i;
                                i = temp;
                            }
                        }
                        else
                        {
                            i = i->_next_cell;
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                char *tup = (char*)_hash_index[key_value]
                                ->_recent_win_start->_data_location;
                char *win_loc = tup + _window_offset;
                win_val = int32(*((int32 *)win_loc));
                Tuple tup_fac = Tuple::factory(tup);
                bool fake_flag = tup_fac.get_fake_flag();
                DEBUG << "win_val = " << win_val;
                DEBUG << "fake_flag = " << fake_flag;

                if (i)
                {
                    if (i->_prev_cell == 
                        _hash_index[key_value]->_recent_win_start)
                    {
                        // Everything inbetween is removed.
                        // Check if recent is in the scope or fake_flag. 
                        // If not, overwrite.
                        //
                        if ((win_val >= (_hash_index[key_value]->_keep_until)) 
                            && (!fake_flag))
                        {
                            _garbage.insert(_hash_index[key_value]
                                        ->_recent_win_start->_data_location);
                            _hash_index[key_value]->_recent_win_start
                                        ->_data_location = _q->_enq_location;
                            (_hash_index[key_value]->_arrival_time).tv_sec = 
                                ((timeval *)_q->_enq_location)->tv_sec;
                            (_hash_index[key_value]->_arrival_time).tv_usec = 
                                ((timeval *)_q->_enq_location)->tv_usec;
                        }
                        else
                        {
                            // If we are keeping a window starter due to scope,
                            // or due to fake_flag, we must disable its spec.
                            //
                            Tuple disable = Tuple::factory(tup);
                            disable.set_win_spec(0);

                            DataCell *cell = new DataCell();
                            cell->_data_location = _q->_enq_location;
                            cell->_prev_cell = i->_prev_cell;
                            assert(cell->_prev_cell 
                                == _hash_index[key_value]->_recent_win_start);
                            cell->_next_cell = i;
                            i->_prev_cell->_next_cell = cell;
                            i->_prev_cell = cell;
                            _hash_index[key_value]->_recent_win_start = cell;
                            (_hash_index[key_value]->_arrival_time).tv_sec = 
                                ((timeval *)cell->_data_location)->tv_sec;
                            (_hash_index[key_value]->_arrival_time).tv_usec = 
                                ((timeval *)cell->_data_location)->tv_usec;
                        }
                    }
                    else
                    {
                        // If we are keeping a window starter due to scope,
                        // or fake_flag, we must disable its spec.
                        //
                        Tuple disable = Tuple::factory(tup);
                        disable.set_win_spec(0);

                        // Some stuff has been kept. Add new cell.
                        //
                        DataCell *cell = new DataCell();
                        cell->_data_location = _q->_enq_location;
                        cell->_prev_cell = i->_prev_cell;
                        cell->_next_cell = i;
                        i->_prev_cell->_next_cell = cell;
                        i->_prev_cell = cell;
                        _hash_index[key_value]->_recent_win_start = cell;
                        (_hash_index[key_value]->_arrival_time).tv_sec = 
                            ((timeval *)cell->_data_location)->tv_sec;
                        (_hash_index[key_value]->_arrival_time).tv_usec = 
                            ((timeval *)cell->_data_location)->tv_usec;
                    }
                }
                else
                {
                    if (_tail == _hash_index[key_value]->_recent_win_start)
                    {
                        // Everything inbetween is removed.
                        // Check if recent is in the scope or fake_flag.
                        // If not, overwrite.
                        //
                        if ((win_val >= (_hash_index[key_value]->_keep_until))
                            && (!fake_flag))
                        {
                            _garbage.insert(_hash_index[key_value]
                                        ->_recent_win_start->_data_location);
                            _hash_index[key_value]->_recent_win_start
                                        ->_data_location = _q->_enq_location;
                            (_hash_index[key_value]->_arrival_time).tv_sec = 
                                ((timeval *)_q->_enq_location)->tv_sec;
                            (_hash_index[key_value]->_arrival_time).tv_usec = 
                                ((timeval *)_q->_enq_location)->tv_usec;
                        }
                        else
                        {
                            // If we are keeping a window starter due to scope,
                            // or due to fake_flag, we must disable its spec.
                            //
                            Tuple disable = Tuple::factory(tup);
                            disable.set_win_spec(0);

                            DataCell *cell = new DataCell();
                            cell->_data_location = _q->_enq_location;
                            _tail->_next_cell = cell;
                            cell->_prev_cell = _tail;
                            cell->_next_cell = NULL;
                            _tail = cell;
                            _hash_index[key_value]->_recent_win_start = cell;
                            (_hash_index[key_value]->_arrival_time).tv_sec = 
                                ((timeval *)cell->_data_location)->tv_sec;
                            (_hash_index[key_value]->_arrival_time).tv_usec = 
                                ((timeval *)cell->_data_location)->tv_usec;
                        }
                    }
                    else
                    {
                        // If we are keeping a window starter due to scope,
                        // or due to fake_flag, we must disable its spec.
                        //
                        Tuple disable = Tuple::factory(tup);
                        disable.set_win_spec(0);

                        // Some stuff has been kept. Add new cell.
                        //
                        DataCell *cell = new DataCell();
                        cell->_data_location = _q->_enq_location;
                        _tail->_next_cell = cell;
                        cell->_prev_cell = _tail;
                        cell->_next_cell = NULL;
                        _tail = cell;
                        _hash_index[key_value]->_recent_win_start = cell;
                        (_hash_index[key_value]->_arrival_time).tv_sec = 
                            ((timeval *)cell->_data_location)->tv_sec;
                        (_hash_index[key_value]->_arrival_time).tv_usec = 
                            ((timeval *)cell->_data_location)->tv_usec;
                    }
                }
            }
        }
        else
        {
            DataCell *cell = new DataCell();
            cell->_data_location = _q->_enq_location;

            // If a new window, add a new cell to the tail.
            //
            if (win_spec > 0)
            {
                if (is_empty())
                {
                    DEBUG << "adding to an empty index";
                    cell->_prev_cell = NULL;
                    cell->_next_cell = NULL;
                    _head = cell;
                    _tail = cell;
                }
                else
                {
                    DEBUG << "adding to a non-empty index";
                    assert(_head); assert(_tail);
                    cell->_prev_cell = _tail;
                    cell->_next_cell = NULL;
                    _tail->_next_cell =  cell;
                    _tail = cell;
                }

                _hash_index[key_value]->_recent_win_start = cell;
                (_hash_index[key_value]->_arrival_time).tv_sec = 
                    ((timeval *)cell->_data_location)->tv_sec;
                (_hash_index[key_value]->_arrival_time).tv_usec = 
                    ((timeval *)cell->_data_location)->tv_usec;

                ++_num_queued_windows; // It was NULL before, so it's new.

                // Update UF stats.
                //
                ++_update_counts[key_value];
                ++_total_update_count;
                if (_update_counts[key_value] > _max_update_count)
                {
                    _max_update_count = _update_counts[key_value];
                }
            }
            // Must find my friends in the queue and attach there.
            // Since windows are processed in chunks, my friends must be at
            // the head of the queue (if there are any).
            //
            else
            {
                if (win_spec < 0)
                {
                    // If I am beyond the scope and not fake_flag, ignore me..
                    //
                    char *tup = (char*)cell->_data_location;
                    char *win_loc = tup + _window_offset;
                    int32 win_val = int32(*((int32 *)win_loc));
                    Tuple tup_fac = Tuple::factory(tup);
                    bool fake_flag = tup_fac.get_fake_flag();
                    DEBUG << "win_val = " << win_val;
                    DEBUG << "fake_flag = " << fake_flag;
                    if ((win_val >= (_hash_index[key_value]->_keep_until)) &&
                        (!fake_flag))
                    {
                        _garbage.insert(cell->_data_location);
                        delete cell;

                        DEBUG << "Index after enqueue_inplace():";
                        DEBUG << as_string();
                        return;
                    }
                }

                if (is_empty())
                {
                    DEBUG << "adding to an empty index";
                    cell->_prev_cell = NULL;
                    cell->_next_cell = NULL;
                    _head = cell;
                    _tail = cell;
                }
                else
                {
                    DEBUG << "adding to a non-empty index";
                    assert(_head); assert(_tail);
                    DataCell *i = _head;
                    bool done = false;
                    while ((!done) && (i))
                    {
                        char *tup = (char*)i->_data_location;
                        char *key_loc = tup + _key_offset;
                        string key_val = string(key_loc, _key_size);
                        DEBUG << "key_val = " << key_val;
                        if (key_val == key_value)
                        {
                            i = i->_next_cell;
                            continue;
                        }

                        // Insert the new cell between i and i->prev, and break.
                        //
                        if (i->_prev_cell == NULL)
                        {
                            assert(i == _head);
                            cell->_prev_cell = NULL;
                            cell->_next_cell = i;
                            i->_prev_cell = cell;
                            _head = cell;
                        }
                        else
                        {
                            cell->_prev_cell = i->_prev_cell;
                            i->_prev_cell->_next_cell = cell;
                            cell->_next_cell = i;
                            i->_prev_cell = cell;
                        }
                
                        done = true;
                    }
                    if (!done)
                    {
                        // Insert the new cell to the tail.
                        //
                        cell->_prev_cell = _tail;
                        cell->_next_cell = NULL;
                        _tail->_next_cell =  cell;
                        _tail = cell;
                        done = true;
                    }
                }
            }
        }
    }

    DEBUG << "Index after enqueue_inplace():";
    DEBUG << as_string();
}

void UpdateQueue::enqueue_linecut()
{
    if (!_q)
    {
        WARN << "UpdateQueue has no corresponding TupleQueue!";
        return;
    }

    DEBUG << "Size of the tuple queue is = " << _q->size();
    DEBUG << "Size of the garbage set is = " << _garbage.size();
    DEBUG << "Size of the hash index is = " << _hash_index.size();
    DEBUG << "_q->_enq_location = " << _q->_enq_location;
    DEBUG << "_q->_deq_location = " << _q->_deq_location;

    string key_value;
    if (_key_field != "")
    {
        char *key_location = (char*)_q->_enq_location + _key_offset;
        key_value = string(key_location, _key_size);
    }
    else
    {
        key_value = "none";
    }
    DEBUG << "key value = " << key_value;

    char *window_location = (char*)_q->_enq_location + _window_offset;
    int32 window_value = int32(*((int32 *)window_location));
    DEBUG << "window value = " << window_value;

    char *tuple = (char*)_q->_enq_location;
    Tuple input_tuple = Tuple::factory(tuple);
    int win_spec = input_tuple.get_win_spec();
    DEBUG << "win_spec = " << win_spec;

    // First check if there exists a cell for that key.
    //
    if ((_hash_index.find(key_value) == _hash_index.end()) ||
        (_hash_index[key_value] == NULL))
    {
        DEBUG << "window queue for key " << key_value << " does not exist";

        // We'll create a new cell.
        //
        DataCell *cell = new DataCell();
        cell->_data_location = _q->_enq_location;

        // Initialize the update count if it is the first time we see this key.
        // Create the random number sequences.
        //
        if (_hash_index.find(key_value) == _hash_index.end())
        {
            assert(_update_counts.find(key_value) == _update_counts.end());
            _update_counts[key_value] = 0;
            _win_dequeue_counts[key_value] = 0;
            srand48(time(NULL));
            for (unsigned int i = 0; i < RANDOM_NUMBER_COUNT; i++)
            {
                double d = drand48();
                _random_numbers[key_value].push(d);
            }
        }

        _hash_index[key_value] = new WindowQueue(key_value);

        if (win_spec > 0)
        {
            _hash_index[key_value]->_recent_win_start = cell;
            (_hash_index[key_value]->_arrival_time).tv_sec = 
                                    ((timeval *)cell->_data_location)->tv_sec;
            (_hash_index[key_value]->_arrival_time).tv_usec = 
                                    ((timeval *)cell->_data_location)->tv_usec;

            ++_num_queued_windows;

            // Update UF stats.
            //
            ++_update_counts[key_value];
            ++_total_update_count;
            if (_update_counts[key_value] > _max_update_count)
            {
                _max_update_count = _update_counts[key_value];
            }
        }

        if (is_empty())
        {
            DEBUG << "adding to an empty index";
            cell->_prev_cell = NULL;
            cell->_next_cell = NULL;
            _head = cell;
            _tail = cell;
        }
        else if (_update_counts[key_value] == 0)
        {
            DEBUG << "adding to a non-empty index";
            assert(_head); 
            assert(_tail);
            DEBUG << "since first arrival, add to the tail"; // ???
            cell->_prev_cell = _tail;
            cell->_next_cell = NULL;
            _tail->_next_cell =  cell;
            _tail = cell;
        }
        else
        {
            DEBUG << "adding to a non-empty index";
            assert(_head); 
            assert(_tail);

            // Decide if it should cut into the front/middle,
            // or simply be added to the tail.
            //
            DataCell *cut;
            if (_type == LINECUT_UF_FRONT)
            {
                cut = linecut_uf_front(key_value);
            }
            else if (_type == LINECUT_UF_MIDDLE)
            {
                cut = linecut_uf_middle(key_value);
            }
            else if (_type == LINECUT_DF_FRONT)
            {
                cut = linecut_df_front(key_value);
            }
            else if (_type == LINECUT_DF_MIDDLE)
            {
                cut = linecut_df_middle(key_value);
            }
            else if (_type == LINECUT_UF_MIDDLE_DETERMINISTIC)
            {
                cut = linecut_uf_middle_deterministic(key_value);
            }
            else if (_type == LINECUT_DF_MIDDLE_DETERMINISTIC)
            {
                cut = linecut_df_middle_deterministic(key_value);
            }

            if (cut == NULL) // cut to the very front
            {
                DEBUG << "cutting into the front";
                _head->_prev_cell = cell;
                cell->_next_cell = _head;
                cell->_prev_cell = NULL;
                _head = cell;
            }
            else if (cut == _tail) // cut to the tail
            {
                DEBUG << "adding to the tail";
                cell->_prev_cell = _tail;
                cell->_next_cell = NULL;
                _tail->_next_cell =  cell;
                _tail = cell;
            }
            else // cut somewhere in the middle
            {
                DEBUG << "cutting in the middle";
                cell->_next_cell = cut->_next_cell;
                cell->_prev_cell = cut;
                cut->_next_cell->_prev_cell = cell;
                cut->_next_cell = cell;
            }
        }

        DEBUG << "newly added cell = " << cell;
        DEBUG << cell->as_string(_q->get_stream()->get_tuple_description());
    }
    else
    {
        DEBUG << "window queue for key " << key_value << " exists";

        if (_hash_index[key_value]->_recent_win_start)
        {
            // We'll create a new cell if it belongs to the window in the queue.
            // But add it to the end of the queue chunk for that key.
            // Assuming that it is in the scope of the most recent one.
            //
            if (win_spec <= 0)
            {
                DataCell *cell = new DataCell();
                cell->_data_location = _q->_enq_location;

                DEBUG << "adding to a non-empty index";
                assert(_head); assert(_tail);

                // Traverse backwards from 
                // _hash_index[key_value]->_recent_win_start
                // until another key window boundary.
                //
                DataCell *i = _hash_index[key_value]->_recent_win_start;
                bool done = false;
                while ((!done) && (i))
                {
                    char *tup = (char*)i->_data_location;
                    char *key_loc = tup + _key_offset;
                    string key_val = string(key_loc, _key_size);
                    DEBUG << "key_val = " << key_val;
                    if (key_val == key_value)
                    {
                        i = i->_next_cell;
                        continue;
                    }

                    // Insert the new cell between i and i->prev, and break.
                    //
                    cell->_prev_cell = i->_prev_cell;
                    i->_prev_cell->_next_cell = cell;
                    cell->_next_cell = i;
                    i->_prev_cell = cell;
                
                    done = true;
                }
                if (!done)
                {
                    // Insert the new cell to the tail.
                    //
                    cell->_prev_cell = _tail;
                    cell->_next_cell = NULL;
                    _tail->_next_cell =  cell;
                    _tail = cell;
                    done = true;
                }
            }
            // If it's a new window, we'll overwrite the old one, removing the
            // rest of its members. 
            // But, Don't delete the ones in the scope!
            // And if a window starter is in scope, 
            // Don't forget to disable that!
            //
            else if (win_spec > 0)
            {
                // Update UF stats, but don't increase the num_queued_windows.
                //
                ++_update_counts[key_value];
                ++_total_update_count;
                if (_update_counts[key_value] > _max_update_count)
                {
                    _max_update_count = _update_counts[key_value];
                }

                DataCell *i = _hash_index[key_value]->_recent_win_start;

                int32 win_val;
                i = i->_next_cell; // Hold on to the recent for now.
                while (i)
                {
                    char *tup = (char*)i->_data_location;
                    char *win_loc = tup + _window_offset;
                    win_val = int32(*((int32 *)win_loc));
                    char *key_loc = tup + _key_offset;
                    string key_val = string(key_loc, _key_size);
                    Tuple tup_fac = Tuple::factory(tup);
                    bool fake_flag = tup_fac.get_fake_flag();
                    DEBUG << "win_val = " << win_val;
                    DEBUG << "key_val = " << key_val;
                    DEBUG << "fake_flag = " << fake_flag;
                    if (key_val == key_value)
                    {
                        // Remove i only if not in scope and not fake_flag.
                        //
                        if ((win_val >= (_hash_index[key_value]->_keep_until))
                            && (!fake_flag))
                        {
                            _garbage.insert(i->_data_location);

                            if (i == _tail)
                            {
                                _tail = i->_prev_cell;
                                i->_prev_cell->_next_cell = NULL;
                                delete i;
                                i = NULL;
                                break;
                            }
                            else
                            {
                                DataCell *temp = i->_next_cell;
                                i->_prev_cell->_next_cell = i->_next_cell;
                                i->_next_cell->_prev_cell = i->_prev_cell;
                                delete i;
                                i = temp;
                            }
                        }
                        else
                        {
                            i = i->_next_cell;
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                char *tup = (char*)_hash_index[key_value]
                                ->_recent_win_start->_data_location;
                char *win_loc = tup + _window_offset;
                win_val = int32(*((int32 *)win_loc));
                Tuple tup_fac = Tuple::factory(tup);
                bool fake_flag = tup_fac.get_fake_flag();
                DEBUG << "win_val = " << win_val;
                DEBUG << "fake_flag = " << fake_flag;

                if (i)
                {
                    if (i->_prev_cell == 
                        _hash_index[key_value]->_recent_win_start)
                    {
                        // Everything inbetween is removed.
                        // Check if recent is in the scope or fake_flag.
                        // If not, overwrite.
                        //
                        if ((win_val >= (_hash_index[key_value]->_keep_until))
                            && (!fake_flag))
                        {
                            _garbage.insert(_hash_index[key_value]
                                        ->_recent_win_start->_data_location);
                            _hash_index[key_value]->_recent_win_start
                                        ->_data_location = _q->_enq_location;
                            (_hash_index[key_value]->_arrival_time).tv_sec = 
                                ((timeval *)_q->_enq_location)->tv_sec;
                            (_hash_index[key_value]->_arrival_time).tv_usec = 
                                ((timeval *)_q->_enq_location)->tv_usec;
                        }
                        else
                        {
                            // If we are keeping a window starter due to scope,
                            // or due to fake_flag, we must disable its spec.
                            //
                            Tuple disable = Tuple::factory(tup);
                            disable.set_win_spec(0);

                            DataCell *cell = new DataCell();
                            cell->_data_location = _q->_enq_location;
                            cell->_prev_cell = i->_prev_cell;
                            assert(cell->_prev_cell 
                                == _hash_index[key_value]->_recent_win_start);
                            cell->_next_cell = i;
                            i->_prev_cell->_next_cell = cell;
                            i->_prev_cell = cell;
                            _hash_index[key_value]->_recent_win_start = cell;
                            (_hash_index[key_value]->_arrival_time).tv_sec = 
                                ((timeval *)cell->_data_location)->tv_sec;
                            (_hash_index[key_value]->_arrival_time).tv_usec = 
                                ((timeval *)cell->_data_location)->tv_usec;
                        }
                    }
                    else
                    {
                        // If we are keeping a window starter due to scope,
                        // or due to fake_flag, we must disable its spec.
                        //
                        Tuple disable = Tuple::factory(tup);
                        disable.set_win_spec(0);

                        // Some stuff has been kept. Add new cell.
                        //
                        DataCell *cell = new DataCell();
                        cell->_data_location = _q->_enq_location;
                        cell->_prev_cell = i->_prev_cell;
                        cell->_next_cell = i;
                        i->_prev_cell->_next_cell = cell;
                        i->_prev_cell = cell;
                        _hash_index[key_value]->_recent_win_start = cell;
                        (_hash_index[key_value]->_arrival_time).tv_sec = 
                            ((timeval *)cell->_data_location)->tv_sec;
                        (_hash_index[key_value]->_arrival_time).tv_usec = 
                            ((timeval *)cell->_data_location)->tv_usec;
                    }
                }
                else
                {
                    if (_tail == _hash_index[key_value]->_recent_win_start)
                    {
                        // Everything inbetween is removed.
                        // Check if recent is in the scope or fake_flag. 
                        // If not, overwrite.
                        //
                        if ((win_val >= (_hash_index[key_value]->_keep_until))
                            && (!fake_flag))
                        {
                            _garbage.insert(_hash_index[key_value]
                                        ->_recent_win_start->_data_location);
                            _hash_index[key_value]->_recent_win_start
                                        ->_data_location = _q->_enq_location;
                            (_hash_index[key_value]->_arrival_time).tv_sec = 
                                ((timeval *)_q->_enq_location)->tv_sec;
                            (_hash_index[key_value]->_arrival_time).tv_usec = 
                                ((timeval *)_q->_enq_location)->tv_usec;
                        }
                        else
                        {
                            // If we are keeping a window starter due to scope,
                            // or due to fake_flag, we must disable its spec.
                            //
                            Tuple disable = Tuple::factory(tup);
                            disable.set_win_spec(0);

                            DataCell *cell = new DataCell();
                            cell->_data_location = _q->_enq_location;
                            _tail->_next_cell = cell;
                            cell->_prev_cell = _tail;
                            cell->_next_cell = NULL;
                            _tail = cell;
                            _hash_index[key_value]->_recent_win_start = cell;
                            (_hash_index[key_value]->_arrival_time).tv_sec = 
                                ((timeval *)cell->_data_location)->tv_sec;
                            (_hash_index[key_value]->_arrival_time).tv_usec = 
                                ((timeval *)cell->_data_location)->tv_usec;
                        }
                    }
                    else
                    {
                        // If we are keeping a window starter due to scope,
                        // or due to fake_flag, we must disable its spec.
                        //
                        Tuple disable = Tuple::factory(tup);
                        disable.set_win_spec(0);

                        // Some stuff has been kept. Add new cell.
                        //
                        DataCell *cell = new DataCell();
                        cell->_data_location = _q->_enq_location;
                        _tail->_next_cell = cell;
                        cell->_prev_cell = _tail;
                        cell->_next_cell = NULL;
                        _tail = cell;
                        _hash_index[key_value]->_recent_win_start = cell;
                        (_hash_index[key_value]->_arrival_time).tv_sec = 
                            ((timeval *)cell->_data_location)->tv_sec;
                        (_hash_index[key_value]->_arrival_time).tv_usec = 
                            ((timeval *)cell->_data_location)->tv_usec;
                    }
                }
            }
        }
        else
        {
            DataCell *cell = new DataCell();
            cell->_data_location = _q->_enq_location;

            // If a new window, add a new cell.
            //
            if (win_spec > 0)
            {
                _hash_index[key_value]->_recent_win_start = cell;
                (_hash_index[key_value]->_arrival_time).tv_sec = 
                                ((timeval *)cell->_data_location)->tv_sec;
                (_hash_index[key_value]->_arrival_time).tv_usec = 
                                ((timeval *)cell->_data_location)->tv_usec;

                ++_num_queued_windows; // It was NULL before, so it's new.

                // Update UF stats.
                //
                ++_update_counts[key_value];
                ++_total_update_count;
                if (_update_counts[key_value] > _max_update_count)
                {
                    _max_update_count = _update_counts[key_value];
                }

                if (is_empty())
                {
                    DEBUG << "adding to an empty index";
                    cell->_prev_cell = NULL;
                    cell->_next_cell = NULL;
                    _head = cell;
                    _tail = cell;
                }
                else if (_update_counts[key_value] == 0)
                {
                    DEBUG << "adding to a non-empty index";
                    assert(_head); 
                    assert(_tail);
                    DEBUG << "since first arrival, add to the tail"; // ???
                    cell->_prev_cell = _tail;
                    cell->_next_cell = NULL;
                    _tail->_next_cell =  cell;
                    _tail = cell;
                }
                else
                {
                    DEBUG << "adding to a non-empty index";
                    assert(_head); assert(_tail);

                    // Decide if it should cut into the front/middle,
                    // or simply be added to the tail.
                    //
                    DataCell *cut;
                    if (_type == LINECUT_UF_FRONT)
                    {
                        cut = linecut_uf_front(key_value);
                    }
                    else if (_type == LINECUT_UF_MIDDLE)
                    {
                        cut = linecut_uf_middle(key_value);
                    }
                    else if (_type == LINECUT_DF_FRONT)
                    {
                        cut = linecut_df_front(key_value);
                    }
                    else if (_type == LINECUT_DF_MIDDLE)
                    {
                        cut = linecut_df_middle(key_value);
                    }
                    else if (_type == LINECUT_UF_MIDDLE_DETERMINISTIC)
                    {
                        cut = linecut_uf_middle_deterministic(key_value);
                    }
                    else if (_type == LINECUT_DF_MIDDLE_DETERMINISTIC)
                    {
                        cut = linecut_df_middle_deterministic(key_value);
                    }

                    if (cut == NULL) // cut to the very front
                    {
                        DEBUG << "cutting into the front";
                        _head->_prev_cell = cell;
                        cell->_next_cell = _head;
                        cell->_prev_cell = NULL;
                        _head = cell;
                    }
                    else if (cut == _tail) // cut to the tail
                    {
                        DEBUG << "adding to the tail";
                        cell->_prev_cell = _tail;
                        cell->_next_cell = NULL;
                        _tail->_next_cell =  cell;
                        _tail = cell;
                    }
                    else // cut somewhere in the middle
                    {
                        DEBUG << "cutting in the middle";
                        cell->_next_cell = cut->_next_cell;
                        cell->_prev_cell = cut;
                        cut->_next_cell->_prev_cell = cell;
                        cut->_next_cell = cell;
                    }
                }
            }
            // Must find my friends in the queue and attach there.
            // Since windows are processed in chunks, my friends must be at
            // the head of the queue (if there are any).
            //
            else
            {
                if (win_spec < 0)
                {
                    // If I am beyond the scope, ignore me..
                    //
                    char *tup = (char*)cell->_data_location;
                    char *win_loc = tup + _window_offset;
                    int32 win_val = int32(*((int32 *)win_loc));
                    Tuple tup_fac = Tuple::factory(tup);
                    bool fake_flag = tup_fac.get_fake_flag();
                    DEBUG << "win_val = " << win_val;
                    DEBUG << "fake_flag = " << fake_flag;
                    if ((win_val >= (_hash_index[key_value]->_keep_until))
                        && (!fake_flag))
                    {
                        _garbage.insert(cell->_data_location);
                        delete cell;

                        DEBUG << "Index after enqueue_linecut():";
                        DEBUG << as_string();
                        return;
                    }
                }

                if (is_empty())
                {
                    DEBUG << "adding to an empty index";
                    cell->_prev_cell = NULL;
                    cell->_next_cell = NULL;
                    _head = cell;
                    _tail = cell;
                }
                else
                {
                    DEBUG << "adding to a non-empty index";
                    assert(_head); assert(_tail);
                    DataCell *i = _head;
                    bool done = false;
                    while ((!done) && (i))
                    {
                        char *tup = (char*)i->_data_location;
                        char *key_loc = tup + _key_offset;
                        string key_val = string(key_loc, _key_size);
                        DEBUG << "key_val = " << key_val;
                        if (key_val == key_value)
                        {
                            i = i->_next_cell;
                            continue;
                        }

                        // Insert the new cell between i and i->prev, and break.
                        //
                        if (i->_prev_cell == NULL)
                        {
                            assert(i == _head);
                            cell->_prev_cell = NULL;
                            cell->_next_cell = i;
                            i->_prev_cell = cell;
                            _head = cell;
                        }
                        else
                        {
                            cell->_prev_cell = i->_prev_cell;
                            i->_prev_cell->_next_cell = cell;
                            cell->_next_cell = i;
                            i->_prev_cell = cell;
                        }
                
                        done = true;
                    }
                    if (!done)
                    {
                        // Insert the new cell to the tail.
                        //
                        cell->_prev_cell = _tail;
                        cell->_next_cell = NULL;
                        _tail->_next_cell =  cell;
                        _tail = cell;
                        done = true;
                    }
                }
            }
        }
    }

    DEBUG << "Index after enqueue_linecut():";
    DEBUG << as_string();
}

void *UpdateQueue::dequeue()
{
    if (!_q)
    {
        WARN << "UpdateQueue has no corresponding TupleQueue!";
        return NULL;
    }

    DEBUG << "Size of the tuple queue is = " << _q->size();

    if (!_head)
    {
        WARN << "Queue was empty";
        return NULL;
    }

    string key_value;
    if (_key_field != "")
    {
        char *key_location = (char*)_head->_data_location + _key_offset;
        key_value = string(key_location, _key_size);
    }
    else
    {
        key_value = "none";
    }
    DEBUG << "key value = " << key_value;

    char *tuple = (char*)_head->_data_location;
    Tuple input_tuple = Tuple::factory(tuple);
    int win_spec = input_tuple.get_win_spec();
    DEBUG << "win_spec = " << win_spec;

    if (_head->_next_cell) 
    {
        DEBUG << "There is enough stuff in the queue";
        if (win_spec > 0)
        {
            assert(_hash_index[key_value]);
            _hash_index[key_value]->_keep_until = win_spec;
            _hash_index[key_value]->_recent_win_start = NULL;
            --_num_queued_windows;
            DEBUG << "_num_queued_windows = " << _num_queued_windows;
            ++_win_dequeue_counts[key_value];
            ++_total_dequeue_count;
            if (_win_dequeue_counts[key_value] > _max_dequeue_count)
            {
                _max_dequeue_count = _win_dequeue_counts[key_value];
            }
        }

        DEBUG << _head->_data_location;

        _garbage.insert(_head->_data_location);

        _head = _head->_next_cell;
        delete _head->_prev_cell;
        _head->_prev_cell = NULL;

        DEBUG << "new _head = " << _head;
        DEBUG << _head->as_string(_q->get_stream()->get_tuple_description());

        DEBUG << "Index after dequeue():";
        DEBUG << as_string();
        return _head->_data_location;
    }

    DEBUG << "Nothing is left in the queue";
    if (win_spec > 0)
    {
        assert(_hash_index[key_value]);
        _hash_index[key_value]->_keep_until = win_spec;
        _hash_index[key_value]->_recent_win_start = NULL;
        --_num_queued_windows;
        DEBUG << "_num_queued_windows = " << _num_queued_windows;
        ++_win_dequeue_counts[key_value];
        ++_total_dequeue_count;
        if (_win_dequeue_counts[key_value] > _max_dequeue_count)
        {
            _max_dequeue_count = _win_dequeue_counts[key_value];
        }
    }

    DEBUG << _head->_data_location;

    _garbage.insert(_head->_data_location);

    delete _head;
    _head = _tail = NULL;

    DEBUG << "Index after dequeue():";
    DEBUG << as_string();
    return NULL;
}

// Dequeue the key entry with the highest priority. Return nothing.
// For now, assume no windows.
//
void UpdateQueue::dequeue_prior()
{
    if (!_q)
    {
        WARN << "UpdateQueue has no corresponding TupleQueue!";
    }

    DEBUG << "Size of the tuple queue is = " << _q->size();

    if (!_prior)
    {
        WARN << "No prior to dequeue !";
    }

    string key_value;
    if (_key_field != "")
    {
        char *key_location = (char*)_prior->_data_location + _key_offset;
        key_value = string(key_location, _key_size);
    }
    else
    {
        key_value = "none";
    }
    DEBUG << "prior key value = " << key_value;

    char *tuple = (char*)_prior->_data_location;
    Tuple input_tuple = Tuple::factory(tuple);
    int win_spec = input_tuple.get_win_spec();
    DEBUG << "win_spec = " << win_spec;

    assert(_hash_index[key_value]);
    _hash_index[key_value]->_keep_until = win_spec;
    _hash_index[key_value]->_recent_win_start = NULL;
    --_num_queued_windows;
    DEBUG << "_num_queued_windows = " << _num_queued_windows;
    ++_win_dequeue_counts[key_value];
    ++_total_dequeue_count;
    if (_win_dequeue_counts[key_value] > _max_dequeue_count)
    {
        _max_dequeue_count = _win_dequeue_counts[key_value];
    }

    DEBUG << _prior->_data_location;

    _garbage.insert(_prior->_data_location);

    if ((_prior == _head) && (_prior->_next_cell))
    {
        _head = _head->_next_cell;
        delete _head->_prev_cell;
        _head->_prev_cell = NULL;
        DEBUG << "new _head = " << _head;
        DEBUG << _head->as_string(_q->get_stream()->get_tuple_description());
        DEBUG << "Index after dequeue():";
        DEBUG << as_string();
    }
    else if (_prior == _head)
    {
        DEBUG << "Nothing is left in the queue";
        assert(_prior == _tail);
        delete _head;
        _head = _tail = NULL;
        DEBUG << "Index after dequeue():";
        DEBUG << as_string();
    }
    else
    {
        _prior->_prev_cell->_next_cell = _prior->_next_cell;
        _prior->_next_cell->_prev_cell = _prior->_prev_cell;
        DEBUG << "Index after dequeue():";
        DEBUG << as_string();
    }

    _prior = NULL;
}

// Traverse the queue to locate the entry with the current largest aw value.
// Assuming no windows for now.
//
const void *UpdateQueue::get_prior_location()
{
    if (!_q)
    {
        WARN << "UpdateQueue has no corresponding TupleQueue!";
        return NULL;
    }

    DEBUG << "Size of the tuple queue is = " << _q->size();

    if (!_head)
    {
        WARN << "Queue was empty";
        return NULL;
    }

    int win;
    DataCell *selected = NULL;
    double priority, max_priority = 0;
    double wait_time;
    timeval arrival_time;
    Time current_time = Time::now();

    DataCell *i = _head;
    while (i)
    {
        char *tup = (char*)i->_data_location;
        char *key_loc = tup + _key_offset;
        string key_val = string(key_loc, _key_size);
        Tuple input_tuple = Tuple::factory(tup);
        int win_spec = input_tuple.get_win_spec();
        assert(win_spec > 0); // for now, since no windows
        if (win_spec > 0)
        {
            DEBUG << "key_val = " << key_val;
            arrival_time.tv_sec = (_hash_index[key_val]->_arrival_time).tv_sec;
            arrival_time.tv_usec = 
                                (_hash_index[key_val]->_arrival_time).tv_usec;
            /*
            in_time.tv_sec = ((timeval *)i->_data_location)->tv_sec;
            in_time.tv_usec = ((timeval *)i->_data_location)->tv_usec;
            */
            wait_time = static_cast<double>((current_time - 
                                             Time(arrival_time)).to_usecs());
            priority = _access_prob[key_val] * wait_time;
            DEBUG << "priority = " << priority;
            if (priority > max_priority)
            {
                max_priority = priority;
                selected = i;
                win = win_spec;
            }

        }

        i = i->_next_cell;
    }

    assert(selected);
    _prior = selected;
    return _prior->_data_location;
}

bool UpdateQueue::is_empty()
{
    if (!_head)
    {
        assert(!_tail);
        return true;
    }
    
    return false;
}

void UpdateQueue::set_field_info(Stream *stream)
{
    if (!stream)
    {
        WARN << "Invalid stream!";
        return;
    }

    const CatalogSchema *schema = stream->get_stream_schema();
    if (!schema)
    {
        WARN << "Invalid schema!";
        return;
    }

    int32 index = schema->index_of_field(_window_field);
    if (index == -1)
    {
        WARN << "Invalid field index!";
        return;
    }

    const SchemaField *win_field = schema->get_schema_field((uint16)index);
    if (!win_field)
    {
        WARN << "Invalid schema field!";
        return;
    }
    
    _window_size = win_field->get_size();
    _window_offset = win_field->get_offset();

    if (_key_field != "")
    {
        index = schema->index_of_field(_key_field);
        if (index == -1)
        {
            WARN << "Invalid field index!";
            return;
        }
        
        const SchemaField *key_field = schema->get_schema_field((uint16)index);
        if (!key_field)
        {
            WARN << "Invalid schema field!";
            return;
        }
        
        _key_size = key_field->get_size();
        _key_offset = key_field->get_offset();
    }
}

double UpdateQueue::get_update_frequency(string key)
{
    assert(_update_counts.find(key) != _update_counts.end());
    assert(_update_counts[key] > 0);
    return(double(_update_counts[key])/double(_total_update_count));

    /*
    if (_update_times.find(key) != _update_times.end())
    {
        if (_update_times[key].size() == 0)
        {
            DEBUG << "Empty _update_time queue for key " << key;
            return 0;
        }

        if (_update_times[key].size() == 1)
        {
            return(1.0/_stats_window);
        }

        if (_update_times[key].size() > 1)
        {
            return(double(_update_times[key].size())/
                   (_update_times[key].back() - _update_times[key].front()));
        }
    }

    DEBUG << "No _update_time queue for key " << key;
    return 0;
    */

    /*
    double current_time = static_cast<double>(Time::now().to_msecs());
    return(double(_update_counts[key])/(current_time - _stats_start_time));
    */
}

// Return the key with the largest update frequency.
//
string UpdateQueue::get_max_freq_key()
{
    double max_update_freq = 0;
    string max_freq_key = "none";
    double frequency;

    DEBUG << "update frequency of things in the window queue";

    for (WindowQueueMap::iterator iter = _hash_index.begin();
         iter != _hash_index.end(); iter++)
    {
        if (iter->second) 
        {
            if (iter->second->_recent_win_start)
            {
                frequency = get_update_frequency(iter->first);
                DEBUG << "key = " << iter->first << " ---> " << frequency;

                if (frequency > max_update_freq)
                {
                    max_update_freq = frequency; 
                    max_freq_key = iter->first;
                }
            }
        }
    }

    DEBUG << "max_freq_key = " << max_freq_key 
          << ", with frequency = " << max_update_freq; 

    return max_freq_key;
}

// Quicksort/partition function.
//
int UpdateQueue::partition(vector<double> *v, vector<string> *s, int p, int r)
{
    double x, temp;
    string stemp;

    x = (*v)[p];

    int i = p-1;
    int j = r+1;

    while (1) 
    {
        do 
        {
            j = j-1;
        } while ((*v)[j] < x);

        do 
        {
            i = i+1;
        } while ((*v)[i] > x);

        if (i < j) 
        {
            temp = (*v)[i]; 
            stemp = (*s)[i];
            (*v)[i] = (*v)[j]; 
            (*s)[i] = (*s)[j];
            (*v)[j] = temp; 
            (*s)[j] = stemp;
        }
        else
        {
            return j;
        }
    }
}

// Quicksort/main function.
//
void UpdateQueue::quick_sort(vector<double> *v, vector<string> *s, int p, int r)
{
    int q;

    if (p < r) 
    {
        q = partition(v, s, p, r);
        quick_sort(v, s, p, q);
        quick_sort(v, s, q+1, r);
    }
}

/*
// Randomly pick one of the keys among the ones with high update frequency.
// For now, we use two freq categories: high and low.
// Top 50% of the frequencies are in the high category.
// With probability _prob_high, we pick a key at random from the high category.
// We assume that _prob_high + _prob_low = 1.0, and _prob_high > _prob_low.
//
string UpdateQueue::pick_drop_key()
{
    // First, get the latest update frequency stats for queue residents.
    //
    vector<double> frequencies;
    vector<string> keys;
    DEBUG << "update frequency of things in the window queue";
    for (WindowQueueMap::iterator iter = _hash_index.begin();
         iter != _hash_index.end(); iter++)
    {
        if (iter->second) 
        {
            if (iter->second->_recent_win_start)
            {
                frequencies.push_back(get_update_frequency(iter->first));
                keys.push_back(iter->first);
                DEBUG << "key = " << keys.back()
                      << " ---> " << frequencies.back();
            }
        }
    }

    // Then, sort them in descending order of their update frequencies.    
    //
    quick_sort(&frequencies, &keys, 0, frequencies.size()-1); 

    
    //DEBUG << "sorted in descending order:";
    //for (unsigned int i = 0; i < frequencies.size(); i++)
    //{
    //    DEBUG << "key = " << keys[i]
    //          << " ---> " << frequencies[i];
    //}
    
    
    // Then, decide if top-half, or bottom-half.
    //
    double p = drand48();
    bool high = false;
    if (p <= _prob_high)
    {
        high = true;
    }
    else
    {
        high = false;
    }
    
    // Finally, pick one from the chosen half randomly.
    //
    int index;

    if (high)
    {
        //DEBUG << "high";
        if ((keys.size())/2*2 != (keys.size()))
        {
            index = random() % ((keys.size())/2+1);
        }
        else
        {
            index = random() % ((keys.size())/2);        
        }
        //DEBUG << index;
        //DEBUG << keys[index];
        return(keys[index]);
    }
    else
    {
        //DEBUG << "low";
        index = random() % ((keys.size())/2);        
        if ((keys.size())/2*2 != (keys.size()))
        {
            index = index + (keys.size())/2+1;
        }
        else
        {
            index = index + (keys.size())/2;
        }
        //DEBUG << index;
        //DEBUG << keys[index];
        return(keys[index]);
    }
}

// Randomly pick a key from the queue.
//
string UpdateQueue::pick_random_drop_key()
{
    // First, get the keys that are available for dropping.
    //
    vector<string> keys;
    DEBUG << "things in the window queue";
    for (WindowQueueMap::iterator iter = _hash_index.begin();
         iter != _hash_index.end(); iter++)
    {
        if (iter->second) 
        {
            if (iter->second->_recent_win_start)
            {
                keys.push_back(iter->first);
                DEBUG << "key = " << keys.back();
            }
        }
    }

    // Then pick a random number between 0 and keys.size()-1.
    //
    int index = random() % (keys.size());    
    DEBUG << index;

    return(keys[index]);
}

void UpdateQueue::drop_window(string key)
{
    assert(_hash_index[key]->_recent_win_start);

    bool fake_flag = false;
    DataCell *i = _hash_index[key]->_recent_win_start;
    while(i)
    {
        // Drop the data cell if it has the right key.
        //
        char *tup = (char*)i->_data_location;
        char *key_loc = tup + _key_offset;
        string key_val = string(key_loc, _key_size);
        Tuple tup_fac = Tuple::factory(tup);
        fake_flag = tup_fac.get_fake_flag();

        DataCell *temp = i->_next_cell;

        if ((key_val == key) && (!fake_flag))
        {
            if (i->_prev_cell)
            {
                if (i->_next_cell)
                {
                    i->_prev_cell->_next_cell = i->_next_cell;
                    i->_next_cell->_prev_cell = i->_prev_cell;
                }
                else // i must be the _tail.
                {
                    assert(i == _tail);
                    _tail = i->_prev_cell;
                    i->_prev_cell->_next_cell = NULL;
                }
            }
            else // i must be the _head.
            {
                assert(i == _head);
                _head = i->_next_cell;
                i->_next_cell->_prev_cell = NULL;
            }

            ++_num_early_drops;
            DEBUG << "Early-dropping: "
                  << i->as_string(
                     _q->get_stream()->get_tuple_description());
            delete i;
        }

        i = temp;
    }

    _hash_index[key]->_recent_win_start = NULL;
    --_num_queued_windows;
}
*/

UpdateQueue::DataCell *UpdateQueue::linecut_uf_front(string key)
{
    DEBUG << "_max_update_count = " << _max_update_count;
    DEBUG << "_update_counts[" << key << "] = " << _update_counts[key];
    double cut_prob = 1 - (double(_update_counts[key])/
                           double(_max_update_count));
    DEBUG << "cut_prob = " << cut_prob;

    assert(_random_numbers[key].size() > 0);
    double p = _random_numbers[key].front();
    _random_numbers[key].pop();
    _random_numbers[key].push(p); // reuse RANDOM_NUMBER_COUNT times later

    if (p < cut_prob)
    {
        DEBUG << "Cutting...";

        // Decide where to linecut in front.
        //
        DataCell *i = _head;
        while (i)
        {
            char *tup = (char*)i->_data_location;
            char *key_loc = tup + _key_offset;
            string key_val = string(key_loc, _key_size);
            Tuple input_tuple = Tuple::factory(tup);
            int win_spec = input_tuple.get_win_spec();
            if (win_spec > 0)
            {
                assert(key_val != key);
                DEBUG << "key_val = " << key_val;
                return (i->_prev_cell);
            }
            else
            {
                i = i->_next_cell;
            }
        }

        if (!i)
        {
            DEBUG << "tail";
            return _tail;
        }
        else
        {
            return (i->_prev_cell);
        }
    }
    else
    {
        DEBUG << "tail";
        return _tail;
    }
}

UpdateQueue::DataCell *UpdateQueue::linecut_uf_middle(string key)
{
    DEBUG << "_max_update_count = " << _max_update_count;
    DEBUG << "_update_counts[" << key << "] = " << _update_counts[key];
    double cut_prob = 1 - (double(_update_counts[key])/
                           double(_max_update_count));
    DEBUG << "cut_prob = " << cut_prob;

    assert(_random_numbers[key].size() > 0);
    double p = _random_numbers[key].front();
    _random_numbers[key].pop();
    _random_numbers[key].push(p); // reuse RANDOM_NUMBER_COUNT times later

    if (p < cut_prob)
    {
        DEBUG << "Cutting...";
        DEBUG << "Will find a slot...";

        DataCell *i = _head;
        while (i)
        {
            char *tup = (char*)i->_data_location;
            char *key_loc = tup + _key_offset;
            string key_val = string(key_loc, _key_size);
            Tuple input_tuple = Tuple::factory(tup);
            int win_spec = input_tuple.get_win_spec();
            if (win_spec > 0)
            {
                assert(key_val != key);
                DEBUG << "key_val = " << key_val;
                if (_update_counts[key_val] <= _update_counts[key])
                {
                    i = i->_next_cell;
                }
                else 
                {
                    return (i->_prev_cell);
                }
            }
            else
            {
                i = i->_next_cell;
            }
        }

        if (!i)
        {
            DEBUG << "tail";
            return _tail;
        }
        else
        {
            return (i->_prev_cell);
        }
    }
    else
    {
        DEBUG << "tail";
        return _tail;
    }
}

UpdateQueue::DataCell *UpdateQueue::linecut_df_front(string key)
{
    DEBUG << "_max_dequeue_count = " << _max_dequeue_count;
    DEBUG << "_win_dequeue_counts[ " << key << " ] = " 
          << _win_dequeue_counts[key];

    double cut_prob;
    if (_max_dequeue_count > 0)
    {
        cut_prob = 1 - (double(_win_dequeue_counts[key])/
                        double(_max_dequeue_count));
    }
    else
    {
        return _tail; 
    }

    DEBUG << "cut_prob = " << cut_prob;

    assert(_random_numbers[key].size() > 0);
    double p = _random_numbers[key].front();
    _random_numbers[key].pop();
    _random_numbers[key].push(p); // reuse RANDOM_NUMBER_COUNT times later

    if (p < cut_prob)
    {
        DEBUG << "Cutting...";

        // Decide where to linecut in front.
        //
        DataCell *i = _head;
        while (i)
        {
            char *tup = (char*)i->_data_location;
            char *key_loc = tup + _key_offset;
            string key_val = string(key_loc, _key_size);
            Tuple input_tuple = Tuple::factory(tup);
            int win_spec = input_tuple.get_win_spec();
            if (win_spec > 0)
            {
                assert(key_val != key);
                DEBUG << "key_val = " << key_val;
                return (i->_prev_cell);
            }
            else
            {
                i = i->_next_cell;
            }
        }

        if (!i)
        {
            DEBUG << "tail";
            return _tail;
        }
        else
        {
            return (i->_prev_cell);
        }
    }
    else
    {
        DEBUG << "tail";
        return _tail;
    }
}

UpdateQueue::DataCell *UpdateQueue::linecut_df_middle(string key)
{
    DEBUG << "_max_dequeue_count = " << _max_dequeue_count;
    DEBUG << "_win_dequeue_counts[ " << key << " ] = " 
          << _win_dequeue_counts[key];

    double cut_prob;
    if (_max_dequeue_count > 0)
    {
        cut_prob = 1 - (double(_win_dequeue_counts[key])/
                        double(_max_dequeue_count));
    }
    else
    {
        return _tail; 
    }

    DEBUG << "cut_prob = " << cut_prob;
    assert(_random_numbers[key].size() > 0);
    double p = _random_numbers[key].front();
    _random_numbers[key].pop();
    _random_numbers[key].push(p); // reuse RANDOM_NUMBER_COUNT times later

    if (p < cut_prob)
    {
        DEBUG << "Cutting...";
        DEBUG << "Will find a slot...";

        DataCell *i = _head;
        while (i)
        {
            char *tup = (char*)i->_data_location;
            char *key_loc = tup + _key_offset;
            string key_val = string(key_loc, _key_size);
            Tuple input_tuple = Tuple::factory(tup);
            int win_spec = input_tuple.get_win_spec();
            if (win_spec > 0)
            {
                assert(key_val != key);
                DEBUG << "key_val = " << key_val;
                if (_win_dequeue_counts[key_val] <= _win_dequeue_counts[key])
                {
                    i = i->_next_cell;
                }
                else 
                {
                    return (i->_prev_cell);
                }
            }
            else
            {
                i = i->_next_cell;
            }
        }

        if (!i)
        {
            DEBUG << "tail";
            return _tail;
        }
        else
        {
            return (i->_prev_cell);
        }
    }
    else
    {
        DEBUG << "tail";
        return _tail;
    }
}

UpdateQueue::DataCell *UpdateQueue::linecut_uf_middle_deterministic(string key)
{
    DEBUG << "_max_update_count = " << _max_update_count;
    DEBUG << "_update_counts[" << key << "] = " << _update_counts[key];

    DEBUG << "Cutting...";
    DEBUG << "Will find a slot...";

    DataCell *i = _head;
    while (i)
    {
        char *tup = (char*)i->_data_location;
        char *key_loc = tup + _key_offset;
        string key_val = string(key_loc, _key_size);
        Tuple input_tuple = Tuple::factory(tup);
        int win_spec = input_tuple.get_win_spec();
        if (win_spec > 0)
        {
            assert(key_val != key);
            DEBUG << "key_val = " << key_val;
            if (_update_counts[key_val] <= _update_counts[key])
            {
                i = i->_next_cell;
            }
            else 
            {
                return (i->_prev_cell);
            }
        }
        else
        {
            i = i->_next_cell;
        }
    }
    if (!i)
    {
        DEBUG << "tail";
        return _tail;
    }
    else
    {
        return (i->_prev_cell);
    }
}

UpdateQueue::DataCell *UpdateQueue::linecut_df_middle_deterministic(string key)
{
    DEBUG << "_max_dequeue_count = " << _max_dequeue_count;
    DEBUG << "_win_dequeue_counts[ " << key << " ] = " 
          << _win_dequeue_counts[key];

    DEBUG << "Cutting...";
    DEBUG << "Will find a slot...";

    DataCell *i = _head;
    while (i)
    {
        char *tup = (char*)i->_data_location;
        char *key_loc = tup + _key_offset;
        string key_val = string(key_loc, _key_size);
        Tuple input_tuple = Tuple::factory(tup);
        int win_spec = input_tuple.get_win_spec();
        if (win_spec > 0)
        {
            assert(key_val != key);
            DEBUG << "key_val = " << key_val;
            if (_win_dequeue_counts[key_val] <= _win_dequeue_counts[key])
            {
                i = i->_next_cell;
            }
            else 
            {
                return (i->_prev_cell);
            }
        }
        else
        {
            i = i->_next_cell;
        }
    }

    if (!i)
    {
        DEBUG << "tail";
        return _tail;
    }
    else
    {
        return (i->_prev_cell);
    }
}

string UpdateQueue::as_string()
{
    string result = string("\n");

    DataCell *cell = _head;
    while (cell)
    {
        char *tup = (char*)cell->_data_location;
        char *key_loc = tup + _key_offset;
        string key_val = string(key_loc, _key_size);
        char *win_loc = tup + _window_offset;
        int32 win_val = int32(*((int32 *)win_loc));
        Tuple tuple = Tuple::factory(tup);
        int win_spec = tuple.get_win_spec();
        result += (_key_field + " = " + key_val + ", "
                   + _window_field + " = " + win_val + ", win_spec = " 
                   + win_spec + "\n");
        cell = cell->_next_cell;
      }

    return result;
}

BOREALIS_NAMESPACE_END;

