#include "SOutputQBox.h"

BOREALIS_NAMESPACE_BEGIN

// --------------------------------------------------
void SOutputQBox::setup_impl() throw (AuroraException)
{
    SControlQBox::setup_impl();

    // SOUtput has 2 outputs. See SControlQBox for other output

    // set the output stream's type, same as input
    set_out_description(DATA_STREAM, get_in_stream(0));

}


// --------------------------------------------------
void SOutputQBox::init_impl() throw (AuroraException)
{

    SControlQBox::init_impl();

    _last_stable_tuple = ptr<dynbuf>(new dynbuf(_output_tuple_size[DATA_STREAM]));
    memset(_last_stable_tuple->data(),0,_output_tuple_size[DATA_STREAM]);

    _current_state = NORMAL;
    _produced_stables = false;
    _produced_tentatives = false;
    _init = false;

    _produce_rec_done = false;
    _rec_done_tuple = ptr<dynbuf>(new dynbuf(_output_tuple_size[DATA_STREAM]));
    memset(_last_stable_tuple->data(),0,_output_tuple_size[DATA_STREAM]);

}


// --------------------------------------------------
void SOutputQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{

    //static int iteration = 0;
    //iteration++;
    //if ( ( iteration % 100 ) == 0)
    //{ WARN << "SOutput iteration " << iteration;
    //}

    _init = true;

    DeqIterator my_deq_iterator = deq(0);
    
    // We don't want to create queues in fron of the SOutput operator
    //while (my_deq_iterator.avail())
    int nb_tuples = 0;
    static int MAX = 100;

    while ( inv.continue_dequeue_on(my_deq_iterator, 0) && ( nb_tuples < MAX) )
    {
        nb_tuples++;
        const Tuple src_tuple = Tuple::factory(my_deq_iterator.tuple());

        if ( _produce_rec_done )
        { Tuple::factory(_rec_done_tuple->data()).set_tuple_stime(src_tuple.get_tuple_stime());
        }

        TupleType tuple_type = src_tuple.get_tuple_type();
        DEBUG << "Output box got tuple " << src_tuple.get_tuple_id() << ": " << tuple_type;


        if ( src_tuple.get_tuple_type() == TupleType::RECONCILIATION_DONE)
        { WARN << "Processing a REC_DONE inside SOutput at time" << Timestamp::now();   
        }


        // Garbage below unless _produced_stables is set to true
        Tuple last_stable_tuple = Tuple::factory(_last_stable_tuple->data());

        switch (_current_state)
        {

        case NORMAL:
            if ( tuple_type == TupleType::INSERTION)
            { 
                save_last_stable_tuple(src_tuple);
                _produced_stables = true;
            }
            if ( tuple_type == TupleType::TENTATIVE)
            { 
                if ( ! _produced_tentatives )
                { INFO << "First tentative";
                }
                _produced_tentatives = true;
            }            
            emit_tuple(src_tuple); // propagate BOUNDARY, INSERTION, or TENTATIVE
            break;


        case DROP_DUPLICATES:

            if (src_tuple.get_tuple_id() == last_stable_tuple.get_tuple_id())
            {
                INFO << "Done dropping duplicates";
                if ( _produced_tentatives )
                { 
                    INFO << "Emitting undo tuple";
                    emit_undo_tuple(last_stable_tuple); 
                }
                _produced_stables = false;
                _produced_tentatives = false;
                _current_state = FINISH_RECONCILIATION;
            }
            break;
            

        case FINISH_RECONCILIATION:

            if ( tuple_type == TupleType::INSERTION)
            { 
                save_last_stable_tuple(src_tuple);
                _produced_stables = true;
            }
            if ( tuple_type == TupleType::TENTATIVE)
            { 
                emit_control_tuple(DATA_STREAM,TupleType::RECONCILIATION_DONE,src_tuple.get_tuple_stime());
                INFO << "Got tentative so producing a REC_DONE.. tuple is " << src_tuple.get_tuple_id() << " " << src_tuple.get_tuple_stime();
                _produced_tentatives = true;
                _current_state = NORMAL;
            }            
            if ( tuple_type == TupleType::RECONCILIATION_DONE)
            { 
                INFO << "Just saw a REC_DONE go by";
                _current_state = NORMAL;                
                _produce_rec_done = true;
                Tuple::factory(_rec_done_tuple->data()).copy_tuple(src_tuple,_output_tuple_size[DATA_STREAM]);
            }

            if ( tuple_type != TupleType::RECONCILIATION_DONE)
            { emit_tuple(src_tuple); // propagate BOUNDARY, INSERTION, or TENTATIVE
            }
            break;

        default:
            ERROR << "SOutputQBox is not in a valid state";

        }

        ++my_deq_iterator;
    }

      // If there are no tuples available for processing, that means the queues are finally empty
    if ( (!my_deq_iterator.avail() && _produce_rec_done ) )
    {
        _produce_rec_done = false;

        Tuple rec_done_tuple = Tuple::factory(_rec_done_tuple->data());

        // Send the REC_DONE downstream
        emit_tuple(rec_done_tuple);

        // Send the REC_DONE on the control stream too
        emit_control_tuple(rec_done_tuple.get_tuple_type(),rec_done_tuple.get_tuple_stime());

    }


    notify_outputs();
}

// --------------------------------------------------
void
SOutputQBox::save_last_stable_tuple(const Tuple& src_tuple)
{
    char* tuple_start = _last_stable_tuple->data();
    Tuple dst_tuple = Tuple::factory(tuple_start);
    dst_tuple.copy_tuple(src_tuple,_output_tuple_size[DATA_STREAM]);
}


// --------------------------------------------------
void
SOutputQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    _pending_box_state = packed_box.dynamic_cast_to<SOutputBoxState>();
}

// --------------------------------------------------
ptr<AbstractBoxState>
SOutputQBox::pack_state(bool delta_only)
{

    DEBUG << "At checkpoint produced tentatives is " << _produced_tentatives;
    ptr<SOutputBoxState> union_box_state(new SOutputBoxState(_current_state, 
                                                             *_last_stable_tuple,
                                                             _produced_tentatives));

    if ( (_current_state == NORMAL) ||
         (_current_state == FINISH_RECONCILIATION))
    {
        DEBUG << "Resetting produced stables ";
        _produced_stables = false;
    }

    return union_box_state;
}

// --------------------------------------------------
void
SOutputQBox::unpack_state(ptr<AbstractBoxState> box_state)
{

    ptr<SOutputBoxState> my_state = box_state.dynamic_cast_to<SOutputBoxState>();
    
    Tuple t = Tuple::factory(my_state->get_tuple().data());

    // If we are initializing the state from a checkpoint
    if ( !_init )
    {
        _current_state = my_state->get_state();
        save_last_stable_tuple(t);
        _produced_tentatives = my_state->get_produced_tentatives();
        return;
    }

    INFO << "At recovery produced tentatives is " << _produced_tentatives;    
    switch ( _current_state )
    {
    case NORMAL :  INFO << "Current state is normal"; break;
    case DROP_DUPLICATES :  INFO << "Current state is drop dupes"; break;
    case FINISH_RECONCILIATION :  INFO << "Current state is rec done"; break;
    default: INFO << "Current state is unknown";
    }

    if ( ( _current_state == NORMAL ) || 
         ( _current_state == DROP_DUPLICATES))
    {
        // Need to drop duplicate tuples
        if ( _produced_stables )
        { 
            INFO << "Going to drop duplicate stable tuples... until we see tuple" << Tuple::factory(_last_stable_tuple->data()).get_tuple_id();
            _current_state = DROP_DUPLICATES;
        }

        // It is possible that we only produced TENTATIVE tuples after the checkpoint
        // then we have to produce an UNDO and wait for rec_done
        else if ( _produced_tentatives)
        {            
            INFO << "Emitting undo tuple";
            emit_undo_tuple(t); 
            _produced_stables = false;
            _produced_tentatives = false;
            _current_state = FINISH_RECONCILIATION;   
        }

    }

}

NMSTL_SERIAL_DEFINE(SOutputBoxState, 3002);

BOREALIS_NAMESPACE_END
