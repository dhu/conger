#include "CombineQBox.h"

BOREALIS_NAMESPACE_BEGIN

using namespace __gnu_cxx;


////////////////////////////////////////////////////////////////////////////////
//
void CombineQBox::setup_impl()
                       throw(AuroraException)
{
    ExprContext ctxt;
//..............................................................................


    if (get_num_inputs() < 1)
    {   Throw(aurora_typing_exception, "combine requires at least one input");
    }

    _final_bitmask = (1 << get_num_inputs()) - 1;

    string key_field = param("key-field", PARAM_NON_EMPTY);

    _data_offsets.push_back(0);

    for (unsigned int i = 0; i < get_num_inputs(); ++i)
    {
        //TupleDescription desc = getInputDescription(i);
        //int index = desc.indexOfField(key_field);
        CatalogSchema  *desc = get_in_schema(i);
        int32 index = desc->index_of_field( key_field );

        if (index < 0)
        {   Throw(aurora_typing_exception,
                  "input " + to_string(i) + " to combine lacks key field " + key_field);
        }

        //TupleDescription::Field field = desc.getField(index);
        //_key_field_offsets.push_back( field.getOffset() );
        //SchemaField  *field = &desc->_field[ index ];
        const SchemaField  *field = desc->get_schema_field( index );
        _key_field_offsets.push_back( field->get_offset() );

        if ( i == 0 )
        {   _key_field_type = field->get_type();
            _key_field_size = field->get_size();
        }
        else
        {   if ( _key_field_type != field->get_type() )
            {   Throw(aurora_typing_exception, "key field of input " + to_string(i) + " does not match other keys' types");
            }

            if (_key_field_size != field->get_size())
            {   Throw(aurora_typing_exception, "key field of input " + to_string(i) + " does not match other keys' lengths");
            }
        }

        _data_offsets.push_back(_data_offsets.back() + desc->get_size());
        //ctxt.setNamedTupleInfo(getInput(i).getName(), desc, i);
        ctxt.set_named_schema_info(get_input(i).get_name(), desc, i);
    }

    vector<SchemaField> output_fields;

    for ( uint32  i = 0; ; ++i )
    {
        string expr = param("expression." + to_string(i));
        if (expr.empty()) break;

        string expr_name = param("output-field-name." + to_string(i), PARAM_NON_EMPTY);
        _expressions.push_back(Expression::parse(expr, ctxt));
        output_fields.push_back(_expressions.back()->to_field(expr_name));
    }

    _ectxt.set(new EvalContext(get_num_inputs()));
    set_out_description(0, TupleDescription(output_fields.begin(), output_fields.end()));
}



////////////////////////////////////////////////////////////////////////////////
//
string CombineQBox::bits_to_string(unsigned int value, int bits)
{
    string out;
//..............................................................................


    for (unsigned int mask = (1 << bits) >> 1; mask; mask >>= 1)
    {    out.push_back(value & mask ? '1' : '0');
    }

    return(out);
}



////////////////////////////////////////////////////////////////////////////////
//
void CombineQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    EnqIterator ei = enq(0);
    bool output = false;

    TupleDescription out_td(get_out_schema(0));
//..............................................................................


    for (unsigned int i = 0; i < get_num_inputs(); ++i)
    {
        DeqIterator di = deq(i);
        TupleDescription i_td(get_in_schema(i));

        while (inv.continue_dequeue_on(di, i))
        {
            const char *tuple = static_cast<const char*>(di.tuple());

            DEBUG << "CombineQBox in on #" << i << ": " << i_td.tuple_to_string(tuple);

            string key(tuple + _key_field_offsets[i], _key_field_size);
            DEBUG << "Key is " << to_escaped_string(key);

            pair<int,string>& value = _data[key];
            DEBUG << " - old bitmask was " << bits_to_string(value.first, get_num_inputs());

            if (value.first == 0)
            {
                DEBUG << "   - first one!";

                // Set size to "as big as we'll need it"
                value.second.resize(_data_offsets.back());
            }

            copy(tuple,
                 tuple + (_data_offsets[i+1] - _data_offsets[i]),
                 value.second.begin() + _data_offsets[i]);

            for (unsigned int j = 0; j < get_num_inputs(); ++j)
            {
                TupleDescription j_td(get_in_schema(j));

                DEBUG << "   - now data[" << j << "] is "
                      << j_td.tuple_to_string(value.second.data() + _data_offsets[j]);
            }

            value.first |= (1 << i);
            DEBUG << "   - now bitmask is " << bits_to_string(value.first, get_num_inputs());

            if (value.first == _final_bitmask)
            {
                DEBUG << "   - evicting!";

                for (unsigned int j = 0; j < get_num_inputs(); ++j)
                {    _ectxt->set_tuple(j, value.second.data() + _data_offsets[j]);
                }

                // Copy header of *this* tuple
                __builtin_memcpy(ei.tuple(), di.tuple(), HEADER_SIZE);

                Expression::eval_vector_into(_expressions,
                                             static_cast<char*>(ei.tuple()) + HEADER_SIZE,
                                             *_ectxt);

                DEBUG << "   - out: " << out_td.tuple_to_string(ei.tuple());
                ++ei;
                _ectxt->reset();

                _data.erase(key);
                output = true;
            }

            ++di;
        }
    }

    if (output)
    {   get_output(0).notify_enq();
    }
}


BOREALIS_NAMESPACE_END
