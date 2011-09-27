#include "PseudoScheduler.h"
#include "PseudoSchedulerMonitor.h"
#include "AuroraNode.h"
#include "QBox.h"
#include "LockHolder.h"
#include "TSStats.h"
#include "Name.h"

#include <fstream>

BOREALIS_NAMESPACE_BEGIN

// Is this the same as the one defined in AuroraNodeMonitor.h???
#define COLLECT_STATISTICS

// This needs to be 1.0 because it is used to compute the adjustment
// setting it to different value will mess up the adjustment calculation
// in constructor.
//
double Scheduler::ticks_adj_factor = 1.0;


PseudoScheduler::PseudoScheduler() :
    Scheduler(true, true),
    _die(false),
    _draining(false),
    _perf_stats(false)
{
    _wait_mutex = new pthread_mutex_t;
    pthread_mutex_init(_wait_mutex, NULL); // -- tatbul

    _wait_cond = new pthread_cond_t;
    pthread_cond_init(_wait_cond, NULL); // -- tatbul

    _monitor._use_shadow_boxmap = 0;
    _box_counter = 0;
}


void PseudoScheduler::start()
{
    _monitor._last_selectivity_update 
        = _monitor._last_qlength_update
        = _monitor._last_boxcost_update
        = _monitor._last_stream_rate_update 
        = _monitor._last_irate_update 
        = _monitor._init_time 
        = Scheduler::ticks();

    setup_listeners();

    unsigned long before = Scheduler::ticks();
    sleep(1);
    unsigned long after = Scheduler::ticks();
    Scheduler::ticks_adj_factor = (after - before) 
                                  * Scheduler::seconds_per_tick();
    
    NOTICE << "Launching pseudo-scheduler with " << node().repr();

    if (_perf_stats)
    {
        NOTICE << "Enabling performance stats";
    }

    // Start the work thread.
    //
    pthread_create(&_ps_thread, 0, &PseudoScheduler::launch, this);

    NOTICE << "Started pseudo-scheduler";
}

void PseudoScheduler::setup_listeners()
{
    const CatalogStream *input;
    set<ptr<QBox> >::const_iterator i;

    // Set up listeners.
    //
    set<ptr<QBox> > boxes = node().get_boxes();

    for (i = boxes.begin(); i != boxes.end(); ++i)
    {
        QBox *box = i->get();

        if (!_setup_boxes.insert(box).second)
        {
            ERROR << "setupListeners was called on a box with already " 
                  << "existing listener. "
                  << "By new rules this should NOT be happening";

           continue;
        }

        // ok, this call is strange. TODO:
        // works on destination to which you move but not on source?
        // setupListeners is not used when adding boxes??
        //
        INFO << " Adding a box mapping for " << box->get_name();

        // #ifdef COLLECT_STATISTICS  // 1
        _monitor.setup_listeners_statistics(box);

        for (uint32 j = 0; j < box->get_num_inputs(); ++j)
        {
            DEBUG << "Adding listener to " << box->get_name() << " #" << j;
            TupleQueue *q = box->get_input_queue(j);

            input = box->get_input(j).get_local_stream();
            DEBUG << " - its input stream is " << input->get_stream_name();

            if (!input->is_input_stream())
            {
                DEBUG << "   - which is from box " << box->get_name();

                //ptr<MyBoxTupleQueueListener> 
                //    tql(new MyBoxTupleQueueListener);
                MyBoxTupleQueueListener *tql = new MyBoxTupleQueueListener;

                tql->_ps = this;
                tql->set_box(box);
                tql->resume();
                _tqlisteners.push_back(tql);
                //q->addListener(tql.get());
                q->add_listener(tql);
            }
            else
            {
                DEBUG << "   - which is an input stream";

                //ptr<MyInputTupleQueueListener> 
                //    tql(new MyInputTupleQueueListener);
                MyInputTupleQueueListener *tql = new MyInputTupleQueueListener;

                tql->_ps = this;
                tql->set_box(box);
                tql->resume();
                _tqlisteners.push_back(tql);
                //q->addListener(tql.get());
                q->add_listener(tql);
            }
        }
    }
}



void PseudoScheduler::delete_scheduler_statistics(string statistics_name,
                                                  vector<string> &ids)
{
    _monitor.delete_scheduler_statistics(statistics_name, ids);

    return;
}



void PseudoScheduler::shutdown()
{
    if (_ps_thread)
    {
        NOTICE << "Terminating scheduler thread...";

        {
            _die = true;
            LockHolder hold(_ps_lock);
            _ps_condition.signal();
        }

        pthread_join(_ps_thread, 0);

        NOTICE << "Scheduler thread has terminated.  Hasta la vista, baby.";
    }
}

void PseudoScheduler::run()
{
    NOTICE << "Pseudo Scheduler running";
    QBoxInvocation inv;
    
    while (1)
    {
        set<QBox*> boxes_to_run; 
        boxes_to_run.clear();
        set<QBox*> ext_boxes_to_run; 
        ext_boxes_to_run.clear();

        Tasks tasks;

        {
            LockHolder hold(_ps_lock);

            while (_boxes_to_run.empty() && 
                   _ext_boxes_to_run.empty() &&
                   _tasks.empty() && 
                   !_die)
            {
                if (_draining)
                {
                    INFO << "Network is drained; exiting";
                    _die = true;
                    break;
                }

                bool wait = true;

                // alexr - This is where I add the box to the things to run
                // and clear out the bit.   Maybe the mapping should be to
                // actual boxes rather than names.
                //
                DEBUG << "Adding the marked ones...";

                // The meanining of 300 and 32 (as Alex says): 
                // The marker array allows at most 300*32 distinct boxes 
                // for a given Borealis server node. -- tatbul
                //
                for (int z = 0; z < 300; z++)
                {
                    int curr = _box_markers[z];

                    // if anything is set
                    //
                    if (curr > 0)
                    {
                        // for all bits in an int
                        //
                        for (int zz = 0; zz < 32; zz++) 
                        {
                            // if the bit is set
                            //
                            if (1 & curr) 
                            {
                                // Boxes that have been disconnected (e.g.,
                                // a drop box) may have remained marked. 
                                // We must ignore them.
                                // -- tatbul
                                //
                                if (((node().get_box(_id_to_box[z * 32 + zz])).
                                                                       get()))
                                {
                                    wait = false;
                                    DEBUG << "Inserting ext_box_to_run : "
                                          << ((node().get_box(_id_to_box[
                                             z * 32 + zz])).get())->get_name();
                                    _ext_boxes_to_run.insert((node().get_box(
                                             _id_to_box[z * 32 + zz])).get());
                                }
                            }

                            curr = (curr >> 1);
                        }
                    }

                    // reset the curr value
                    //
                    _box_markers[z] = 0;
                }
                
                if (wait)  
                {
                    node()._monitor.set_done(true); // -- tatbul
                
                    //////////// This is for HA (begin): Jeong-Hyon
                    //
                    if (_event_handler)
                    {
                        hold.release();
                        _event_handler->handle_idle_time();
                        hold.reacquire();
                    }
                    //
                    //////////// This is for HA (end)

                    _ps_lock.wait_cond(_ps_condition);
                }
            }

            if (_die)
            {
                // Still have to run any outstanding tasks
                //
                DEBUG << "Scheduler: that's all folks";
                for (Tasks::iterator i = _tasks.begin(); 
                     i != _tasks.end(); ++i)
                {
                    DEBUG << "Running a task before dying";
                    (*i)->run();
                }

                _tasks.clear();
        
                return;
            }

            assert(boxes_to_run.empty());
            assert(ext_boxes_to_run.empty());

            boxes_to_run.swap(_boxes_to_run);
            assert(_boxes_to_run.empty());

            tasks.swap(_tasks);

            // Magda: we only process input boxes
            // once there is nothing else to process
            //if (boxes_to_run.empty())
            //{
            // -- but this may allow one input stream to "block" another.

            ext_boxes_to_run.swap(_ext_boxes_to_run);
            assert(_ext_boxes_to_run.empty());

            copy(boxes_to_run.begin(), boxes_to_run.end(),
                 inserter(ext_boxes_to_run, ext_boxes_to_run.end()));
            boxes_to_run.swap(ext_boxes_to_run);

            //copy(ext_boxes_to_run.begin(), ext_boxes_to_run.end(),
            //     inserter(boxes_to_run, boxes_to_run.end()));

            ext_boxes_to_run.clear();
            assert(ext_boxes_to_run.empty());
        
            //copy(_ext_boxes_to_run.begin(), _ext_boxes_to_run.end(),
            //     inserter(boxes_to_run, boxes_to_run.end()));
            //_ext_boxes_to_run.clear();
            //}

        } // Release _ps_lock

        node()._monitor.set_done(false); // -- tatbul

        INFO << "Got some stuff to do...";

        for (set<QBox*>::const_iterator i = boxes_to_run.begin();
             i != boxes_to_run.end(); ++i)
        {
            INFO << " - " << (*i)->get_name();
        }

        DEBUG <<"Boxes to run" << boxes_to_run;

        {
            LockHolder hold(_ps_lock);

            for (set<QBox*>::iterator i = boxes_to_run.begin();
                 i != boxes_to_run.end(); ++i)
            {
                //////// HA: if there is a stale primary, skip active backups: 
                // Jeong-Hyon
                //
                if (_event_handler) // if HA is on
                {
                    if (has_stale_primary(node().get_boxes()) && 
                        ((*i)->is_backup()))
                    {
                        continue; 
                    }
                }
                ////////

                inv._box = *i;

                _monitor.track_queues(*i);

                if (_perf_stats)
                {
                    //  #ifdef COLLECT_STATISTICS  // 3
                    _monitor.run_statistics3(*i, inv);
                }
                else
                {
                    DEBUG << "Scheduler attempting to run box.";
                    DEBUG << "Running " << (*i)->get_name() << "...";

                    // Magda: bug fix We use the same inv for all boxes
                    // so _tuple_limit.size() must be max of inputs to any box
                    // Note: nb_additional_inputs must be a signed integer
                    //
                    int nb_additional_inputs = (*i)->get_num_inputs() -
                                               inv._tuple_limit.size();
                    for (int k = 0; k < nb_additional_inputs; k++)
                    {
                        inv._tuple_limit.push_back(TUPLE_PROCESSING_LIMIT);
                    }

                    /*
                    if (!inv._tuple_limit.size())
                    {
                        for (register uint32 k = 0; 
                             k < (*i)->get_num_inputs(); k++)
                        {
                            inv._tuple_limit.push_back(TUPLE_PROCESSING_LIMIT);
                            WARN << "Added 1 tuple limit for input port " << k;
                          }
                    }
                    */
                
                    //////////// This is for HA (begin): Jeong-Hyon
                    //
                    Time before = Time::now();
                    //
                    //////////// This is for HA (end)

                    //NOTICE << (*i)->get_name();
                    (*i)->run(inv);
                
                    //////////// This is for HA (begin): Jeong-Hyon
                    //
                    Time time_span = Time::now() - before;
                    if (_event_handler)
                    {
                        _event_handler->update_statistics((*i), time_span);
                    }
                    //
                    //////////// This is for HA (end)

                    DEBUG << "Done running " << (*i)->get_name();

                } // not _perf_stats


                uint32 num_inputs = (*i)->get_num_inputs();
                bool already_added = false;

                // Commenting this out in order to be more fair 
                // to the other boxes (esp. when there is overload). 
                // -- tatbul
                //
                for (uint32 j = 0; j < num_inputs; ++j)
                {
                    if ((*i)->get_input_queue(j)->size())
                    {
                        DEBUG << "XXX - input " << j 
                              << " not done; will run again";
                        _boxes_to_run.insert(*i);
                        DEBUG << "Inserting box to run: " << (*i)->get_name();
                        already_added = true;
                    }
                }
        
                // Magda added: check if box wants to run again
                //
                if ((*i)->has_more_processing() && (!already_added))
                {
                    DEBUG << "Box " << (*i)->get_name()
                          << " would like to run again... rescheduling";
                    _boxes_to_run.insert(*i);
                    DEBUG << "Inserting box to run: " << (*i)->get_name();
                }

                // #ifdef COLLECT_STATISTICS        // 4 -6
                _monitor.run_box_statistics(*i);
            }

            // iteration over the boxes ends.
            //DEBUG << "DID the for loop ever get run?";
            // check if this is a good time to update
 
            // #ifdef COLLECT_STATISTICS   // 7 - 11
            _monitor.run_statistics(node());
        }

        DEBUG << "finished stats; actually do the running of tasks";
        
        for (Tasks::iterator i = tasks.begin(); i != tasks.end(); ++i)
        {
            DEBUG << "Running a task " << typeid(**i);
            (*i)->run();
            DEBUG << "Done running task " << typeid(**i);
        }
    }
}

void PseudoScheduler::drain()
{
    _draining = true;

    ASSERT(_ps_thread);

    INFO << "Scheduler draining...";
    {
        LockHolder hold(_ps_lock);
        _ps_condition.signal();
    }
    pthread_join(_ps_thread, 0);
    _ps_thread = 0;
}



void PseudoScheduler::remove_from_schedule(QBox* box)
{
    DEBUG << "PseudoScheduler::remove_from_schedule(): " << box->get_name();

    LockHolder hold(_ps_lock);
    set<QBox*>::iterator i,j;

    i = _boxes_to_run.find(box);
    j = _ext_boxes_to_run.find(box);

    if ((i != _boxes_to_run.end()) || (j != _ext_boxes_to_run.end()))
    {
        WARN << "The box \"" << box->get_name() 
             << "\" is about to get deleted, "
             << "how come it is scheduled to run?????";
    }

    if (i != _boxes_to_run.end())
    {
        _boxes_to_run.erase(box);
    }

    if (j != _ext_boxes_to_run.end())
    {
        _ext_boxes_to_run.erase(box);
    }

    _ps_condition.signal();
}



void PseudoScheduler::topology_changed()
{
    INFO << "Topology changed";

    setup_listeners();
}



void PseudoScheduler::MyBoxTupleQueueListener::notify(const TupleQueue&)
{
    if (_suspended)
    {
        return;
    }

    INFO << "XXX Notifying scheduler () that it has to run "
         << _box_to_run->get_name();
    _ps->_boxes_to_run.insert(_box_to_run);
    DEBUG << "Inserting box to run: " << _box_to_run->get_name();
}



void PseudoScheduler::MyInputTupleQueueListener::notify(const TupleQueue&)
{
    if (_suspended)
    {
        DEBUG << "Scheduler is suspended";
        return;
    }

    INFO << "Notifying scheduler (from input) that it has to run "
         << _box_to_run->get_name();

    // Trying to change this call for the better. Notifications stored in a
    // bitmap instead of adding the box at notify time.
    // no notifications are lost as a result (check concurrency issues) -alexr

    LockHolder hold(_ps->_ps_lock, true);

    if (hold.is_held())
    {
        DEBUG << "XXX Inserting box " << _box_to_run->get_name();
        _ps->_ext_boxes_to_run.insert(_box_to_run);
        _ps->_ps_condition.signal();
    } 
    else
    {
        DEBUG << "Failed to add box " << _box_to_run->get_name() 
              << " to scheduler, will mark it.";
        //NOTICE << "marking " << _box_to_run->get_name();

        // If we did not manage to get the lock, we still don't want to lose
        // this event. Currently, we will lose the event and will have to wait
        // until new data arrives.

        // Alternative solution.  Not sure how thread-safe maps are,
        // alternatively we can put a lock around them.    -alexr
        map<string, int>::iterator iter =
            _ps->_box_to_id.find(_box_to_run->get_name());

        if (iter == _ps->_box_to_id.end())
        {
            _ps->_box_to_id[_box_to_run->get_name()] = _ps->_box_counter++;
            _ps->_id_to_box[_ps->_box_counter-1] = _box_to_run->get_name();
            iter = _ps->_box_to_id.find(_box_to_run->get_name());
        }

        //int bit_int = _ps->_box_markers[iter.second/32]; 
        // find the right integer.
        int array_offset = (*iter).second/32;

        _ps->_box_markers[array_offset] = ((_ps->_box_markers[array_offset] &
                                            (~(1 << ((*iter).second -
                                                      array_offset)))) |
                                           (1<<((*iter).second-array_offset)));
        //WARN << " PseudoScheduler, marking position " << (*iter).second
        //     << " ON offset " << array_offset
        //     << " RES " << _ps->_box_markers[array_offset];
    }
}



// -- tatbul
void PseudoScheduler::choke_inputs(vector<string> input_names)
{
    {
        LockHolder hold(_ps_lock);

        for(vector<string>::iterator i = input_names.begin();
            i != input_names.end(); i++)
        {
            ptr<Stream> s = node().get_stream(*i);
            s->begin_buffering();
        }
    }
}



// -- tatbul
void PseudoScheduler::resume_inputs(vector<string> input_names)
{
    {
        LockHolder hold(_ps_lock);

        for(vector<string>::iterator i = input_names.begin();
            i != input_names.end(); i++)
        {
            ptr<Stream> s = node().get_stream(*i);
            s->stop_buffering();
        }
    }
}



/**
 *  This method will choke a given subnetwork, but stopping all
 *  flow within it. Input streams will be set into buffering mode
 *  to make sure no tuples show up unexpected.
 *
 */
void PseudoScheduler::choke_sub_network(vector<string> boxNames)
{
    {
        LockHolder hold(_ps_lock);

        for (vector<string>::iterator i = boxNames.begin(); 
             i != boxNames.end(); i++)
        {
            string box = (*i);
            //WARN << "Removing box " << box << " total " << boxNames.size();

            // here we'll block incoming streams (i.e. streams that feed one
            // of our "choked" boxes but come from outside).
            //
            ptr<QBox> temp_box = node().get_box(box);
            if (!temp_box)
            {
                DEBUG << "Could not choke box " << box << ", box not found.";
                continue;
            }

            for (uint32 in_port = 0; in_port < temp_box->get_num_inputs();
                 in_port++)
            {
                bool buffer = true;
                Stream& s = temp_box->get_input(in_port);

                //WARN << " Considering choking " <<s<< " on box " << temp_box;

                // see if any box is feeding this stream.
                //
                for (vector<string>::iterator iter = boxNames.begin(); 
                     iter != boxNames.end(); iter++)
                {
                    ptr<QBox> up_box = node().get_box(*iter);
                    if (!up_box)
                    {
                        continue;
                    }

                    for (uint32 out_port = 0;
                         out_port < up_box->get_num_outputs(); out_port++)
                    {
                        if (up_box->get_output(out_port).get_name() ==
                            s.get_name())
                        {
                            buffer = false;
                            break;
                        }
                    }

                }

                if (buffer)
                {
                    DEBUG << "buffering on " << s.get_name();
                    s.begin_buffering();
                }
            }

            // Check the currently scheduled boxes and remove as well.
            //
            for (set<QBox*>::iterator it = _boxes_to_run.begin();
                 it != _boxes_to_run.end(); it++)
            {
                DEBUG << "Checking " << (*it)->get_name() << " for removal...";
                if ((*it)->get_name() == box)
                {
                    DEBUG << "XXX Removing " << box << " from running boxes.";
                    _boxes_to_run.erase(it);
                    break;
                }
            }

            for (set<QBox*>::iterator it = _ext_boxes_to_run.begin();
                 it != _ext_boxes_to_run.end(); it++)
            {
                DEBUG << "Checking ext" << (*it)->get_name() 
                      << " for removal...";
                if ((*it)->get_name() == box)
                {
                    DEBUG << "XXX Removing " << box 
                          << " from ext running boxes.";
                    _ext_boxes_to_run.erase(it);
                    break;
                }
            }

            // Check all listeners, which may have pointers to our box 
            // and suspend them. 
            // Note that none of the listeners are removed, just suspended.
            //
            for (TQListeners::iterator iter = _tqlisteners.begin();
                 iter != _tqlisteners.end(); iter++)
            {
                if ((*iter)->get_box())
                {
                    QBox* b = (*iter)->get_box();
                    if (b->get_name() == box)
                    {
                        DEBUG << " Suspending " << box << " listeners.";
                        (*iter)->suspend();
                    }
                }
            }
        }
    } // Release _ps_lock
}



void PseudoScheduler::suspend_sub_network(vector<string> boxNames)
{
    LockHolder hold(_ps_lock);

    for(vector<string>::iterator i = boxNames.begin(); i !=boxNames.end(); i++)
    {
        string box = (*i);
        //WARN << "Removing box " << box << " total " << boxNames.size();

        // here we'll block incoming boxes, i.e. upstream from choked
        // boxes.
        ptr<QBox> temp_box = node().get_box(box);
        if (!temp_box)
        {
          DEBUG << "Could not find " << box << " to suspend.";
          continue;
        }

        for (uint32 in_port = 0;
             in_port < temp_box->get_num_inputs(); in_port++)
        {
            Stream& s = temp_box->get_input(in_port);

            //WARN << " Considering choking " <<s<< " on box " << temp_box;

            // see if any box is feeding this stream.
            for(vector<string>::iterator iter = boxNames.begin();
                iter !=boxNames.end(); iter++)
            {
                ptr<QBox> up_box = node().get_box(*iter);
                if (!up_box)
                {
                    continue;
                }

                // for every box see if it is upstream from one of the list
                for (uint32 out_port = 0;
                     out_port < up_box->get_num_outputs(); out_port++)
                {
                    // if box is upstream:
                    if (up_box->get_output(out_port).get_name() ==
                         s.get_name())
                    {
                        boxNames.push_back(up_box->get_name());
                    }
                }
            }
        }

        // check the currently scheduled boxes and remove as well
        for (set<QBox*>::iterator it =  _boxes_to_run.begin();
             it != _boxes_to_run.end(); it ++)
        {
            INFO << "Checking " << (*it)->get_name() << " for removal...";
            if ((*it)->get_name() == box)
            {
                DEBUG << "XXX Removing " << box << " from running boxes.";
                _boxes_to_run.erase(it);
                break;
            }
        }

        for (set<QBox*>::iterator it =  _ext_boxes_to_run.begin();
             it != _ext_boxes_to_run.end(); it ++)
        {
            INFO << "Checking ext" << (*it)->get_name() << " for removal...";
            if ((*it)->get_name() == box)
            {
                DEBUG << "XXX Removing " << box << " from ext running boxes.";
                _ext_boxes_to_run.erase(it);
                break;
            }
        }

        // check all listeners, which may have pointers to our box and suspend
        // them. Note that none of the listeners are removed, just suspended.
        /*
        for (TQListeners::iterator iter = _tqlisteners.begin();
             iter != _tqlisteners.end(); iter++)
        {
            if ((*iter)->getBox())
            {
                QBox* b = (*iter)->getBox();
                if (b->get_name() == box)
                {
                    //WARN << " Suspending " << box << " listeners.";
                    (*iter)->suspend();
                }
            }
        }
        */
    }
}

/**
 *  This method will resume operation in given subnetwork.
 *  NOTE: it does not resume listeners. Use resumeBoxesListenersNetwork
 *   if you want to resume listeners!
 */
void PseudoScheduler::resume_box_streams(vector<string> boxNames)
{
    // I am no longer convinced this is a thread safe operation. It definately
    // was before, but now with the additions I made probably is not.
    //
    LockHolder hold(_ps_lock);

    for(vector<string>::iterator i = boxNames.begin(); i != boxNames.end(); i++)
    {
        ptr<QBox> temp_box = node().get_box(*i);
        if (!temp_box)
        {
            DEBUG << "Could not resume box "
                  << *i << "'s streams, box not found.";
            continue;
        }
        
        for (uint32 in_port = 0;
             in_port < temp_box->get_num_inputs(); in_port++)
        {
            temp_box->get_input(in_port).stop_buffering();
        }
        
        for (uint32 out_port = 0;
             out_port < temp_box->get_num_outputs(); out_port++)
        {
            temp_box->get_output(out_port).stop_buffering();
        }

        // check all listeners, that were suspended and resume them.
        // Also make notify calls for non-empty queues once, 
        // to resume operation.
        /*
        for (TQListeners::iterator iter = _tqlisteners.begin();
             iter != _tqlisteners.end(); iter++)
        {
            if ((*iter)->getBox())
            {
                QBox* b = (*iter)->getBox();
                if (b->get_name() == (*i))
                {
                    WARN << " Suspending " << (*i) << " listeners.";
                     (*iter)->suspend();
                }
            }
        }
        */
    }
}

/**
 *  This method will resume operation in given subnetwork.
 *  The only difference with resumeBoxStreams is that it resumes
 *  the listeners as well.
 */
void PseudoScheduler::resume_sub_network(vector<string> boxNames)
{
    LockHolder hold(_ps_lock);

    for(vector<string>::iterator i = boxNames.begin(); i != boxNames.end(); i++)
    {
        ptr<QBox> temp_box = node().get_box(*i);
        if (!temp_box)
        {
            DEBUG << "Could not resume box " << *i << " box not found.";
            continue;
        }

        for (uint32 in_port = 0; in_port < temp_box->get_num_inputs(); 
             in_port++)
        {
            temp_box->get_input(in_port).stop_buffering();
        }
        
        for (uint32 out_port = 0; out_port < temp_box->get_num_outputs();
             out_port++)
        {
            temp_box->get_output(out_port).stop_buffering();
        }
        
        // check all listeners, that were suspended and resume them.
        // Also make notify calls for non-empty queues once, 
        // to resume operation.
        //
        for (TQListeners::iterator iter = _tqlisteners.begin();
             iter != _tqlisteners.end(); iter++)
        {
            if ((*iter)->get_box())
            {
                QBox* b = (*iter)->get_box();
                if (b->get_name() == (*i))
                {
                    DEBUG << " Resuming " << (*i) << " listeners.";
                    (*iter)->resume();
                }
            }
        }
    }
}

/**
 *  Drains the subnetwork of all tuples.
 */
void PseudoScheduler::drain_sub_network(vector<string> boxNames)
{
    // Re-add boxes to the scheduling set.
    //
    DEBUG << "Inside drainSubNetwork, about to lock/add boxes "
          << "to ext sched set";

    {
        LockHolder hold(_ps_lock);

        for (vector<string>::iterator i = boxNames.begin(); 
             i != boxNames.end(); i++)
        {
            //QBox *box = (node().getBox(*i)).get();
            string box = (*i);
            //WARN << " Adding " << box->get_name() ;
            //if (box->get_name() == (*i))
            //_ext_boxes_to_run.insert(box);

            // check all listeners, which may have pointers to our box 
            // and resume their operation.
            //
            for (TQListeners::iterator iter = _tqlisteners.begin();
                 iter != _tqlisteners.end(); iter++)
            {
                if ((*iter)->get_box())
                {
                    QBox* b = (*iter)->get_box();
                    if (b->get_name() == box)
                    {
                        //DEBUG << "Resuming " << box 
                        //      << " listeners, input size "
                        //      << (b->get_input_queue(0))->size();

                        //(*iter)->resume();

                        // send a notify enq for non-empty queues to start the
                        // execution.
                        //
                        for (uint32 in_port = 0;
                             in_port < b->get_num_inputs(); in_port++)
                        {
                            if ((b->get_input(in_port)).get_source_port())
                                                                    // box
                            {
                                DEBUG << "XXX Inserting " << b->get_name();
                                _boxes_to_run.insert(b);
                            }
                            else // input
                            {
                                // I know that I hold the lock here, 
                                // so no need to check.
                                //
                                DEBUG << "XXX Signal for " << b->get_name()
                                      << " inserting into _ext_boxes";
                                _ext_boxes_to_run.insert(b);
                                _ps_condition.signal();
                            }
                            //if ((b->get_input_queue(in_port))->size() > 0)
                            //(*iter)->notify(*(b->get_input_queue(in_port)));
                        }
                    }
                }
            }
        }
    } // Release _ps_lock

    // Wait periodically until tuple queues are empty.
    // Not that this part is not locked, since I assume that if tuple is
    // partially processed, then system still counts it in the queue.
    // This might not be true.
    //
    int tuples = 0, old_tuples = -1; //, counter = 0;

    struct timespec to;
    memset(&to, 0, sizeof(to));
    to.tv_sec = time(0);
    to.tv_nsec = 100000000; // 100 msec sleep

    pthread_mutex_lock(_wait_mutex);

    do
    {
        tuples = 0;
        // count all tuples in cut-off area.
        //
        for (vector<string>::iterator i = boxNames.begin(); i != boxNames.end();
             i++)
        {
            ptr<QBox> qbox = node().get_box(*i);
            if (!qbox)
            {
                DEBUG << "Could not drain box " << *i << ", box not found.";
                continue;
            }

            for (uint32 in_port = 0; in_port < qbox->get_num_inputs(); 
                 in_port++)
            {
                DEBUG << " Box " << qbox->get_name() << " has "
                      << (qbox->get_input_queue(in_port))->size();
                tuples += (qbox->get_input_queue(in_port))->size();
            }
        }

        DEBUG << " drainSubNetwork is sleeping until tuples are drained."
              << "Currently " << tuples << " remains\n";
    
        pthread_cond_timedwait(_wait_cond, _wait_mutex, &to);
    
        old_tuples = tuples;

    } while (tuples > 0);

    DEBUG << "tuples = " << tuples;

    // Unset the marks of all the drained boxes that are in the
    // _box_markers array.
    // -- tatbul
    //
    for (vector<string>::iterator i = boxNames.begin(); i != boxNames.end();
         i++)
    {
        ptr<QBox> qbox = node().get_box(*i);
        if (!qbox)
        {
            continue;
        }

        string box_name = qbox->get_name();

        map<string, int>::iterator iter = _box_to_id.find(box_name);

        if (iter != _box_to_id.end())
        {
            int array_offset = (*iter).second/32;
            _box_markers[array_offset] = 0;
            DEBUG << "Unset the marker for " << box_name;
        }
    }

    pthread_mutex_unlock(_wait_mutex);
}

////////////////////////////////////////////////////////////////////////////////
//
void PseudoScheduler::update_listeners(vector<string> boxNames, bool add)
{
    vector<string>::iterator   i;
           CatalogStream      *input;
           CatalogBox         *c_box;
    vector<BoxPort>::iterator  ports;
//
// need to make parameter an action add, remove, delete.
//..............................................................................

    // maybe there is a way to avoid locking the system to make the changes?
    // via the tasks set?
    LockHolder hold(_ps_lock);
    QBox *t_box;
    vector<string> box_names_copy;

    // make a copy for comparacent. If not all of branches need this, it
    // should move into a respective branch
    //
    for (i = boxNames.begin(); i != boxNames.end(); i++)
    {   
        box_names_copy.push_back((*i));
    }

    if (add)
    {
        for (vector<string>::iterator i = boxNames.begin();
             i !=boxNames.end(); i++)
        {
            //for (set<ptr<QBox> >::const_iterator i = boxes.begin(); 
            //     i != boxes.end(); ++i)
            //{
            QBox *box = node().get_box((*i)).get();

            for (uint32 j = 0; j < box->get_num_inputs(); ++j)
            {
                DEBUG << "Adding listener to " << box->get_name() << " #" << j;
                TupleQueue *q = box->get_input_queue(j);

                input = box->get_input(j).get_local_stream();
                DEBUG << " - its input stream is " << input->get_stream_name();

                // #ifdef COLLECT_STATISTICS   // 12
                _monitor.update_listeners_statistics(box, j);

                if (!input->is_input_stream())
                {
                    DEBUG << "   - which is from box " << box->get_name();

                    //ptr<MyBoxTupleQueueListener> 
                    //    tql(new MyBoxTupleQueueListener);
                    MyBoxTupleQueueListener *tql = new MyBoxTupleQueueListener;

                    tql->_ps = this;
                    tql->set_box(box);
                    tql->resume();
                    _tqlisteners.push_back(tql);
                    //q->addListener(tql.get());
                    q->add_listener(tql);
                }
                else
                {
                    DEBUG << "   - which is an input stream";

                    //ptr<MyInputTupleQueueListener> 
                    //    tql(new MyInputTupleQueueListener);
                    MyInputTupleQueueListener *tql =
                        new MyInputTupleQueueListener;

                    tql->_ps = this;
                    tql->set_box(box);
                    tql->resume();
                    _tqlisteners.push_back(tql);
                    //q->addListener(tql.get());
                    q->add_listener(tql);
                }
            }
        }
    }

    // swap input and internal listeners as necessary (both ways)
    //
    for(vector<string>::iterator i = boxNames.begin(); i !=boxNames.end(); i++)
    {
        //WARN << " Scheduler begins with box named as " << (*i);
        string box = (*i);
        ptr<QBox> b = node().get_box(box);

        for (uint32  index = 0; index < b->get_num_outputs(); index++)
        {
            //WARN << " Scheduler continues with box port " << index;
            //Stream& str = b->getOutput(index);
            //vector<QBoxInputPort> d_ports = str.getDestPorts();
            input = b->get_output(index).get_local_stream();
            vector<BoxPort>  *d_ports = input->get_sink_port();

            for (ports  = (*d_ports).begin();
                 ports != (*d_ports).end(); ports ++)
            {
                //WARN << " Scheduler begins with actual port "
                //     << (*ports).getPort();

                bool change = true;

                // there is a box on the output
                //
                c_box = (*ports).get_port_box();

                if (c_box)
                {   
                    for (vector<string>::iterator ic = box_names_copy.begin();
                         ic != box_names_copy.end(); ic++)
                    {
                        if ((*ic) == c_box->get_box_name().as_string())
                        {   
                            change = false; // intermediate box, no action
                        }
                    }
                }

                if (change)
                {
                    t_box = 
                        node().get_box(c_box->get_box_name().as_string()).get();
                    //WARN << " Scheduler detects a change in " 
                    //     << t_box->get_name();
                    TQListeners::iterator  listen_i;

                    // remove the old listener from the TQListeners
                    //
                    for (listen_i  = _tqlisteners.begin();
                         listen_i != _tqlisteners.end(); listen_i++)
                    {
                        if ((*listen_i)->get_box()->get_name()
                             == c_box->get_box_name().as_string())
                        {
                            (t_box->get_input_queue((*ports).get_port()))
                                  ->remove_listener((*listen_i));
                            break;
                        }
                    }

                    _tqlisteners.erase(listen_i);

                    if (add)
                    {
                        MyBoxTupleQueueListener* tql =
                            new MyBoxTupleQueueListener;

                        tql->_ps = this;
                        tql->set_box(t_box);
                        tql->resume();
                        _tqlisteners.push_back(tql);
                        /*
                        (t_box->get_input_queue((*ports).getPort()))->
                            addListener(tql.get());
                        */
                        (t_box->get_input_queue((*ports).get_port()))->
                            add_listener(tql);
                    }
                    else
                    {
                        MyInputTupleQueueListener* tql =
                            new MyInputTupleQueueListener;

                        tql->_ps = this;
                        tql->set_box(t_box);
                        tql->resume();
                        _tqlisteners.push_back(tql);
                        /*
                        (t_box->get_input_queue((*ports).getPort()))->
                            addListener(tql.get());
                        */
                        (t_box->get_input_queue((*ports).get_port()))->
                            add_listener(tql);
                    }

                }

            }

        }
    }
    // this should probably be incorporated into the previous operation.
    // TODO
    //
    if (!add) // oh oh
    {
        WARN << " Scheduler moves on to invalidateBoxes!";
        invalidate_boxes(boxNames);
    }
}


/**
 *  Removes the listeners referring to the box that are no longer in the query.
 *  There might be some other box-related maintenance that will be needed.
 */
void PseudoScheduler::invalidate_boxes(vector<string> boxNames)
{
    //LockHolder hold(_ps_lock);

    for(vector<string>::iterator i = boxNames.begin(); i != boxNames.end(); i++)
    {
         string box = (*i);
        // check all listeners, which may have pointers to our box.
        //
        for (TQListeners::iterator i = _tqlisteners.begin();
             i != _tqlisteners.end();)
        {
            if ((*i)->get_box())
            {
                QBox* b = (*i)->get_box();
                if (b->get_name() == box)
                {
                    INFO << "Removing " << box << " listeners.";
                    for (uint32 j = 0; j < b->get_num_inputs(); ++j)
                    {
                        TupleQueue *q = b->get_input_queue(j);
                        //q->removeListener((*i).get());
                        q->remove_listener((*i));
                    }

                    i = _tqlisteners.erase(i);

                } 
                else 
                { 
                    i++; 
                }
            } 
            else 
            { 
                i++; 
            }
        }
    }
}

/*
  Removes a set of boxes from the running network
*/
/*void PseudoScheduler::removeBoxes(vector<string> boxNames)
{
    // check if dynamic removal/addition is supported?

    // this has to change, but for now lock the operation for box movement.
    // problem is, box could be either currently exucution or be in a similar
    // precarious state.
    {
    LockHolder hold(_ps_lock);
    //bool box_exists = false;

    for(vector<string>::iterator i = boxNames.begin(); i !=boxNames.end(); i++)
    {
         string box = (*i);
         INFO << "Removing box " << box;

           box_exists = false;

           for (vector<string>::iterator iter = _disabled_boxes.begin();
                 iter != _disabled_boxes.end(); iter++)
           {
           if ((*iter) == box)
           {
           cerr << "WARNING: disabling an already disabled box " << box;
           box_exists = true;
           break;
           }
           }

           if (!box_exists)
           {
           INFO << "Disabling " << box;
           _disabled_boxes.push_back(box);
           }

         // check the currently scheduled boxes and remove as well
         for (set<QBox*>::iterator it =  _boxes_to_run.begin();
              it != _boxes_to_run.end(); it ++)
         {
             INFO << "Checking " << (*it)->get_name() << " for removal...";
             if ((*it)->get_name() == box)
             {
             INFO << "Removing " << box << " from running boxes.";
             _boxes_to_run.erase(it);
             break;
             }
         }

         for (set<QBox*>::iterator it =  _ext_boxes_to_run.begin();
              it != _ext_boxes_to_run.end(); it ++)
         {
             INFO << "Checking ext" << (*it)->get_name() << " for removal...";
             if ((*it)->get_name() == box)
             {
             INFO << "Removing " << box << " from ext running boxes.";
             _ext_boxes_to_run.erase(it);
             break;
             }
         }

         // check all listeners, which may have pointers to our box.
         for (TQListeners::iterator i = _tqlisteners.begin();
              i != _tqlisteners.end();)
         {
             if ((*i)->getBox())
             {
             QBox* b = (*i)->getBox();
             if (b->get_name() == box)
             {
                 INFO << "Removing " << box << " listeners.";
                 for (uint32 j = 0; j < b->get_num_inputs(); ++j)
                 {
                 TupleQueue *q = b->get_input_queue(j);
                 q->removeListener((*i).get());
                 }
                 i = _tqlisteners.erase(i);
             } else { i++; }
             } else { i++; }
         }
    }
    }
}

*/

/*void PseudoScheduler::activateBoxes()
{
    // for now just call topology changed (and clear the disabled boxes)
    //_disabled_boxes.clear();

    topologyChanged();
}
*/

void PseudoScheduler::schedule_exclusive_task(ptr<SchedulerTask> task)
{
    LockHolder hold(_ps_lock);

    if (task) 
    {
        _tasks.push_back(task);
    }

    _ps_condition.signal();
}

/*******************************************
// this should probably be in utility, but there is no Utility class yet.
// The return value is junk and should be discarded.
// It is only there so the compiler does not optimize it out.
//
double PseudoScheduler::use_cpu_time(double time_to_use)
{
    //struct itimerval first;
    uint32 seed = 2;

    double secs_per_rand = 2.60260e-08;
    //double secs_per_rand = 2.414e-08;
    //double secs_per_rand = 2.78e-08;
    //double secs_per_rand = 1.09111e-07;
    double num_rands = time_to_use/secs_per_rand;

    //double elapsed_time = 0.0;
    //double secs;
    //secs = get_etime(_itimer_start);
    for (int i = 0; i < num_rands; i++)
    {
        time_to_use = (double)rand_r(&seed) / (double)(RAND_MAX);
        //drand48() / (double)(RAND_MAX);
    }
    //elapsed_time = get_etime(_itimer_start) - secs;
    //_measure->incrementTimeSpentExecutingBoxes(elapsed_time);

    return (time_to_use);
}
****************************/


string PseudoScheduler::to_string()
{
    string boxes = "", ext_boxes = "";

    for (set<QBox*>::const_iterator i = _boxes_to_run.begin();
         i != _boxes_to_run.end(); ++i)
    {
        boxes = boxes + (*i)->get_name() + " ";
    }

    for (set<QBox*>::const_iterator i = _ext_boxes_to_run.begin();
         i != _ext_boxes_to_run.end(); ++i)
    {
        ext_boxes = ext_boxes + (*i)->get_name() + " ";
    }

    return string("\nPseudoScheduler:\n") +
           string("is ")+ (_die ? string("") : string("not ")) +
           string("shutting down at the moment\n")+
           string("Current boxes-to-run ") + boxes + string(" end") +
           string("\nCurrent ext-boxes-to-run ") + ext_boxes + string(" end");
}

bool PseudoScheduler::has_stale_primary(set<QBox*> boxes)
{
    for (set<QBox*>::iterator i = boxes.begin(); i != boxes.end(); ++i)
    {
        if (has_stale_primary(*i))
        {
            return true;
        }
    }

    return false;
}

bool PseudoScheduler::has_stale_primary(set<ptr<QBox> > boxes)
{
    for (set<ptr<QBox> >::iterator i = boxes.begin(); i != boxes.end(); ++i)
    {
        if (has_stale_primary(i->get()))
        {
            return true;
        }
    }

    return false;
}

bool PseudoScheduler::has_stale_primary(QBox* box)
{
    if (!box->is_backup() && box->get_staleness() > 90)
    {
        return true;
    }
    else
    {
        return false;
    }
}

BOREALIS_NAMESPACE_END
