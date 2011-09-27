#ifndef SOUTPUT_QBOX_H
#define SOUTPUT_QBOX_H

#include "QBox.h"
#include "SControlQBox.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * Pass-through filter. When doing checkpoint recovery
 * this operator produces an undo tuple. This operator can
 * also be used to produce boundary tuples
 */
class SOutputQBox : public SControlQBox
{

    friend class SOutputBoxState;

protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

    // packing and unpacking states
    void set_pending_box_state(ptr<AbstractBoxState> packed_box);
    //ptr<AbstractBoxState> pack_state();
    ptr<AbstractBoxState> pack_state(bool delta_only = false);
    void unpack_state(ptr<AbstractBoxState> box_state);

private:

    /**
     * When unpacking state, we are either initializing the state or 
     * recovering after a failure. Need to distinguish.
     */
    bool _init;

    /// Makes a copy of the given tuple as the last stable INSERTION seen
    void save_last_stable_tuple(const Tuple& tuple);

    /// The current state of the SOutput operator
    typedef enum { NORMAL, DROP_DUPLICATES, FINISH_RECONCILIATION } State;
    State _current_state;

    /// State variables: encode history since last checkpoint
    ptr<dynbuf> _last_stable_tuple;
    bool _produced_stables;
    bool _produced_tentatives;

    /// We store REC_DONE tuples until our own queues clear up
    ptr<dynbuf> _rec_done_tuple;
    bool _produce_rec_done;    
};

/**
 * Captures the state of an soutput operator
 * Currently, we only use this for checkpoint recovery and not to move
 * load, so we only save the content of the last output tuple
 */
class SOutputBoxState : public AbstractBoxState
{
 public:
    SOutputBoxState() {}
    SOutputBoxState(SOutputQBox::State state, 
                    dynbuf& last_stable_tuple,
                    bool produced_tentatives)
        : 
        _current_state(state),
        _tuple(last_stable_tuple.max_length()),
        _produced_tentatives(produced_tentatives)        
    {
        // Make a copy of the last stable buffer seen before checkpoint
        memcpy(_tuple.data(),last_stable_tuple.data(),last_stable_tuple.max_length());
        
    }


    ~SOutputBoxState() {}

    SOutputQBox::State get_state() { return _current_state; }
    dynbuf get_tuple() { return _tuple; }
    bool get_produced_tentatives() { return _produced_tentatives; }

    NMSTL_SERIAL_SUBCLASS(SOutputBoxState, AbstractBoxState, << _tuple);

 private:
    SOutputQBox::State _current_state;
    dynbuf _tuple;
    bool _produced_tentatives;

};

BOREALIS_NAMESPACE_END

#endif
