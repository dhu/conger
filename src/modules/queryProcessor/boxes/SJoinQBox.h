#ifndef SJOIN_QBOX_H
#define SJOIN_QBOX_H

#include "JoinQBox.h"
#include <fstream>

BOREALIS_NAMESPACE_BEGIN

/**
 * Almost identical to a join operator but assumes that 
 * tuples arrive order by increasing tuple_stime values on each input stream.
 * Interleaves input tuples by increasing tuple_stime values before processing them.
 */
class SJoinQBox : public JoinQBox
{

 public:
    SJoinQBox() {}
    ~SJoinQBox() {}

 protected:
    
    virtual void run_impl(QBoxInvocation&) throw (AuroraException);
    virtual void setup_impl() throw( AuroraException );


    /**
     * Perform the regular processing when preparing a new output tuple
     * Additionally, check if output tuple should be labeled as TENTATIVE
     */
    virtual void produce_dont_emit_tuple(EvalContext &ctxt); 

    void unpack_state(ptr<AbstractBoxState> box_state);

 private:
    void run_streams_in_parallel(QBoxInvocation& inv, bool& outputted);
    void process_data_tuple(const Tuple& tuple, int stream_id, bool& outputted, Boolean do_join); 

    /**
     * SJoinQBox simply propagates the boundary tuples that it receives.
     * This approach relies on two assumptions: 1) the SUnion preceding SJoin
     * in the query diragram propagates BOUNDARY tuples on a single output stream
     * and 2) the parent class JoinQBox assigns as output tuple_stime the 
     * maximum of the two input tuple_stime being joined
     * SJoinQBox also simply propagates RECONCILIATION_DONE tuples
     */
    void process_control_tuple(const Tuple& tuple);

    /// Just some simple utility functions to avoid duplicating code
    void process_control(QBoxInvocation& inv, DeqIterator& di,                            
                         bool& outputted, bool& more_processing,                             
                         Timestamp& last_time, int stream_id);  

    void process_data(QBoxInvocation& inv, DeqIterator& di,                            
                      bool& outputted, bool& more_processing,                             
                      Timestamp& last_time, int stream_id, bool do_join);


    /// @return true if this tuple is a control tuple (e.g., BOUNDARY, RECONCILIATION_DONE)
    bool is_control_tuple(const Tuple& tuple);

    /// Join left tuples against those on the right. Default: true
    Boolean _leftjoin;

    /// Join right tuples against those on the left. Default: true
    Boolean _rightjoin;


    /// State variables to enable processing of tuples by increasing tuple_stime values
    Timestamp _last_left;
    Timestamp _last_right;

    /// We store REC_DONE tuples until our own queues clear up
    ptr<dynbuf> _rec_done_tuple;
    bool _produce_rec_done;    
};


BOREALIS_NAMESPACE_END

#endif // SJOIN_QBOX_H
