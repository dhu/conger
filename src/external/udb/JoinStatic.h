#ifndef  JOINSTATIC_H
#define  JOINSTATIC_H

#include "QBox.h"
#include "AuroraNode.h"
#include "Expression.h"
#include "Tuple.h"
#include <fstream>

BOREALIS_NAMESPACE_BEGIN


/**
 * This box joins streamed data with a static table
 *
*/
class JoinStatic : public QBox
{

  private:
    // Constants for keeping track of the IDs for left and right.
    static const int LEFT;
    static const int RIGHT;

    // Print a tuple by stream id.
    string tuple_to_string(int stream_id, const void *data)
    {
        TupleDescription in_td(get_in_schema(stream_id));
        return in_td.tuple_to_string(data);
    }

    void emit_tuple(EvalContext &ctxt);
    EvalContext _ctxt;
    ptr<Expression> _predicate;

    bool _just_concat;
    vector<ptr<Expression> > _output_expressions;

    // Cached information about tuples
    unsigned int _left_body_size, _right_body_size;
    unsigned int _output_tuple_size;

    char* filebuffer;
    int maxfilesize;
    int maxlinesize;
    int currline;
    int* selarray;
    int selarraysize;
    int currindex;

  protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

  public:
    JoinStatic();
   ~JoinStatic();
    double get_sel_est();
};

BOREALIS_NAMESPACE_END

#endif                     // JOINSTATIC_H
