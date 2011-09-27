#include "JoinStatic.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * This box joins streamed data with a static table
 *
*/

const int JoinStatic::LEFT = 0;
const int JoinStatic::RIGHT = 1;



////////////////////////////////////////////////////////////////////////////////
//
JoinStatic::JoinStatic() : _ctxt(2)
{
    int i;
//..............................................................................


    maxfilesize = 4096;
    maxlinesize = 256;

    filebuffer = new char[maxfilesize];
    selarraysize = 64;
    selarray = new int[selarraysize];

    for (i = 0; i < selarraysize; i++)
    {
        selarray[i] = 0;
    }

    currindex = 0;
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
JoinStatic::~JoinStatic()
{
//..............................................................................


    delete[] filebuffer;
}



////////////////////////////////////////////////////////////////////////////////
//
void JoinStatic::setup_impl() throw (AuroraException)
{
    ExprContext ctxt;
//..............................................................................


    if (get_num_inputs() != 2)
    {   Throw(aurora_typing_exception,
              "Join requires exactly 2 inputs (not " +
              to_string(get_num_inputs()) + ")");
    }

    // The predicate.
    ctxt.set_named_schema_info("left", get_in_schema(LEFT), LEFT);
    ctxt.set_named_schema_info("right", get_in_schema(RIGHT), RIGHT);
    string pred = param("predicate", PARAM_NON_EMPTY);
    _predicate = Expression::parse(pred, ctxt);

    // Create the buffers based on parameters.
    //_buffer[LEFT] = createBufferByParameter(LEFT);
    //_buffer[RIGHT] = createBufferByParameter(RIGHT);

    // output fields
    vector<SchemaField> output_fields;
    unsigned int field_num = 0;

    while (true)
    {
        string field_str = param("out-field." + to_string(field_num));

        if (field_str.empty()) break;

        ptr<Expression> field_exp = Expression::parse(field_str, ctxt);
        _just_concat = false;
        _output_expressions.push_back(field_exp);

        // Use the string as the name, which is fine if it is a field
        // and icky if it is not, but good for now.
        output_fields.push_back(field_exp->get_schema_field(field_str));
        ++field_num;
    }

    if (output_fields.empty())
    {
        _just_concat = true;
        const vector<SchemaField> &left_fields =
        get_in_schema(LEFT)->get_schema_field();
        const vector<SchemaField> &right_fields =
        get_in_schema(RIGHT)->get_schema_field();

        copy(left_fields.begin(), left_fields.end(),
        back_inserter(output_fields));
        copy(right_fields.begin(), right_fields.end(),
        back_inserter(output_fields));
    }

    TupleDescription td(output_fields.begin(), output_fields.end());
    DEBUG << get_name() << " output description: " << td;
    set_out_description(0, td);
    _node->selval=0;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void JoinStatic::init_impl() throw (AuroraException)
{
    int currel;
    int currplace = 0;
 //..............................................................................


    TupleDescription out_td(get_out_schema(0));
    TupleDescription left_td(get_in_schema(LEFT));
    TupleDescription right_td(get_in_schema(RIGHT));

    _output_tuple_size = out_td.get_size();
    _left_body_size = (left_td.get_size() - HEADER_SIZE);
    _right_body_size = (right_td.get_size() - HEADER_SIZE);

    ifstream statictable ("../modules/optimizer/tinydbwraper/fac2.txt"); //note: make this a parameter later

    //char buffer[maxlinesize];
    currline = 0;

    if (! statictable.is_open())
    {   NOTICE << "Error opening file"; exit (1);
    }

    //statictable.getline (buffer,maxlinesize);
    //NOTICE << buffer;
    //statictable.getline (buffer,maxlinesize);
    //NOTICE << buffer;

    while (! statictable.eof() )
    {
        //statictable.getline (buffer,maxlinesize);
        //NOTICE << buffer;
        Timestamp temp = Timestamp::now();
        memcpy (filebuffer + currplace, &temp, sizeof(Timestamp));
        currplace+=sizeof(Timestamp);
        memcpy (filebuffer + currplace, &temp, sizeof(int));
        currplace+=sizeof(int);
        statictable >> currel;

        if (statictable.eof())
        {   break;
        }

        memcpy (filebuffer + currplace, &currel, sizeof(int));
        currplace+=sizeof(int);
        statictable >> currel;
        memcpy (filebuffer + currplace, &currel, sizeof(int));
        currplace+=sizeof(int);

        //statictable >> currel;
        //memcpy (filebuffer + currplace, &currel, sizeof(int));
        //currplace+=sizeof(int);
        //statictable >> currel;
        //memcpy (filebuffer + currplace, &currel, sizeof(int));
        //currplace+=sizeof(int);

        currline++;
        DEBUG << "Static Table IN: " << right_td.tuple_to_string(filebuffer + ((currline-1) * (right_td.get_size()) ));
    }

    NOTICE << currline;
    statictable.close();
}



////////////////////////////////////////////////////////////////////////////////
//
void JoinStatic::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    bool outputted = false;
    int count;
    DeqIterator my_deq_iterator = deq(0);

    TupleDescription left_td(get_in_schema(LEFT));
    TupleDescription right_td(get_in_schema(RIGHT));
//..............................................................................


    // Do stuff...
    while (inv.continue_dequeue_on(my_deq_iterator, 0))
    {
        const void *data = my_deq_iterator.tuple();
        count = 0;
        INFO << "JoinStatic IN: " << left_td.tuple_to_string(data);

        _ctxt.reset();
        _ctxt.set_tuple(0, (const char *)data);

        for (int i = 0; i < currline; i++)
        {
            _ctxt.set_tuple(1, (const char *)filebuffer + ((i) * (right_td.get_size())));

            if (_predicate->eval<bool>(_ctxt))
            {
                emit_tuple(_ctxt);
                count++;
            }

            NOTICE << "just emitted " << i;
            _ctxt.reset();
        }

        //memcpy(enq(0).tuple(), data, _output_tuple_size);
        //++ (enq(0));

        if (count != 0) //added - is this a mistake
        {   outputted = true;
        }

        selarray[currindex] = count;
        currindex++;

        if (currindex == selarraysize)
        {   currindex = 0;
        }

        if (currindex % 16 == 0)
        {   _node->selval=get_sel_est();
        }

        // neeeeeeeeeeext!
        ++my_deq_iterator;
    }

    // Done, notify if there was output
    if (outputted)
    {   get_output(0).notify_enq();
    }
}


////////////////////////////////////////////////////////////////////////////////
//
double JoinStatic::get_sel_est()
{
    int i, count;
//..............................................................................


    count = 0;

    for (i = 0; i < selarraysize; i++)
    {
        count += selarray[i];
    }

    return (((double)count)/((double)selarraysize));
}



////////////////////////////////////////////////////////////////////////////////
//
void JoinStatic::emit_tuple(EvalContext &ctxt)
{
    EnqIterator ei = enq(0);
//..............................................................................


    const Tuple left = Tuple::factory(ctxt.getTuple(LEFT));
    const Tuple right = Tuple::factory(ctxt.getTuple(RIGHT));
    Tuple output = Tuple::factory(ei.tuple());

    TupleDescription out_td(get_out_schema(0));

    // Header.
    //output.setTimestamp(left.getTimestamp());
    output.copy_header(left);

    if (_just_concat)
    {
        uint8  *pos = output.get_body();
        memcpy(pos, left.get_body(), _left_body_size);
        pos += _left_body_size;
        memcpy(pos, right.get_body(), _right_body_size);
    }
    else
    {   Expression::eval_vector_into(_output_expressions,
                                     (char *)output.get_body(), ctxt);
    }

    NOTICE << get_name() << " OUT: " << out_td.tuple_to_string(output);
    ++ei;
}

BOREALIS_NAMESPACE_END
