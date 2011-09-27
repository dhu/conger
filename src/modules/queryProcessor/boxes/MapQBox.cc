#include "MapQBox.h"
#include "tupleGenerator.h"

BOREALIS_NAMESPACE_BEGIN

void MapQBox::setup_impl() throw (AuroraException)
{
    if (get_num_inputs() != 1)
    {
        Throw(aurora_typing_exception,
              "Map requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }

    for (unsigned int i = 0; ; ++i)
    {
        string expr = param("expression." + to_string(i));
        if (expr.empty())
        {
            if (i == 0)
            {
                Throw(aurora_typing_exception,
                      "Map requires at least one expression parameter. \
                      (\"expression\" is deprecated)");
            }
            else
            {
                break;
            }
        }

        ExprContext ctxt;
        //ctxt.setTupleDescription(TupleDescription(getInDescription(0)));
        ctxt.set_context_schema(*get_in_schema(0));
        _expressions.push_back(Expression::parse(expr, ctxt));
    }

    vector<string> field_names;
    for (unsigned int i = 0; i < _expressions.size(); ++i)
    {
        field_names.push_back(param("output-field-name." + to_string(i), 
                                    PARAM_NON_EMPTY));
    }

    // Build the output TupleDescription based on the expressions
    //
    // NOTE: Expression::getReturnTypes() returns a vector of DataTypes, but
    //       map function can *only* return one type. This, we assume to be 
    //       true (any other return types would mess things up due to 
    //       getReturnedSize forcing the copy of more than needed)

    vector<SchemaField> fields;
    for (unsigned int i = 0; i < _expressions.size(); ++i)
    {
        string name = param("output-field-name." + to_string(i),
                            PARAM_NON_EMPTY);
        fields.push_back(_expressions[i]->get_schema_field(name));
    }

    TupleDescription t(fields.begin(), fields.end());
    // Set output description (there's only one output stream)
    set_out_description(0, t);
}

void MapQBox::init_impl() throw (AuroraException)
{
}

void MapQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{

    DeqIterator di = deq(0);
    EnqIterator ei = enq(0);
    bool outputted = false;

    TupleDescription in_desc(  get_in_schema( 0 ));
    TupleDescription out_desc( get_out_schema( 0 ));

    while (inv.continue_dequeue_on(di, 0))
    {
        const void *data = di.tuple();
        Tuple input_tuple = Tuple::factory(data);
        char *out = (char*)ei.tuple();

        DEBUG << "MapQBox " << get_name()
            << " IN: " << in_desc.tuple_to_string(data);

        if (input_tuple.get_fake_flag()) // if a fake tuple, just pass along
        {
            memcpy(out, data, HEADER_SIZE);
            char *p = out + HEADER_SIZE;
            _ctxt.reset();
            _ctxt.set_tuple((const char *)data);
            int32 data_size = get_out_schema(0)->get_size();
            memset(p, 0, data_size);
        }
        else                           // otherwise, evaluate the function
        {   
            memcpy(out, data, HEADER_SIZE);
            char *p = out + HEADER_SIZE;
            _ctxt.reset();
            _ctxt.set_tuple((const char *)data);

            // If tuple is a BOUNDARY or a RECONCILIATION_DONE, 
            // keep the header, set all output fields to zero and propagate
            if ((input_tuple.get_tuple_type() == TupleType::BOUNDARY) ||
               (input_tuple.get_tuple_type() == TupleType::RECONCILIATION_DONE))
            { 
                int32 data_size = get_out_schema(0)->get_size(); 
                                        // Size of data part of output tuple
                memset(p,0,data_size);
            }
            else // Otherwise, perform actual mapping
            { 
                Expression::eval_vector_into(_expressions, p, _ctxt);
            }
        }

        // Lovely, pass along...
        DEBUG << "MapQBox " << get_name()
            << " OUT: " << out_desc.tuple_to_string(ei.tuple());
        outputted = true;
        ++ei;
        ++di;
    }

    if (outputted)
    get_output(0).notify_enq();
}

BOREALIS_NAMESPACE_END
