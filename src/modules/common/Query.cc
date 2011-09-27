#include "Query.h"

BOREALIS_NAMESPACE_BEGIN



////////////////////////////////////////////////////////////////////////////////
//
//  When adding a box to the query network, must re-compute
//  the input, output, and intermediate streams
//
void  Query::add_box(Box b)
{
    Box::StreamBindings::const_iterator  i;

    const Box::StreamBindings  inputs  = b.get_inputs();
    const Box::StreamBindings  outputs = b.get_outputs();
//..............................................................................


    for (i = inputs.begin(); i != inputs.end(); ++i)
    {
        StreamID stream_id = i->second;
        if ( ! find(stream_id.get_name(),_input_streams)
         && ! find(stream_id.get_name(),_output_streams)
             && ! find(stream_id.get_name(),_intermediate_streams) )
             {
            _input_streams.push_back(stream_id.get_name());
            _stream_defs[stream_id.get_name()] = ptr<Object>(new StreamDef(stream_id));
        }

        // Perform checks on both inputs and outputs since we're not given guarantees on
        // the order of calls to add_box.
        //
        vector<Name>::iterator where =
            std::find(_output_streams.begin(), _output_streams.end(), stream_id.get_name());

        if ( where != _output_streams.end() )
        {
            _intermediate_streams.push_back(*where);
            _output_streams.erase(where);
        }
    }

    for (i = outputs.begin(); i != outputs.end(); ++i)
    {
        Name stream_name = i->second.get_name();

        vector<Name>::const_iterator where = std::find(_input_streams.begin(),
                                                       _input_streams.end(),
                                                       stream_name);

        if (where != _input_streams.end())
        {   _intermediate_streams.push_back(stream_name);
        }
        else
        {    _output_streams.push_back(stream_name);
        }
    }

    _boxes.push_back(b);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
bool  Query::used_by_box(Name stream_name)
{
    vector<Box>::iterator box_it = _boxes.begin();
    vector<Box>::iterator box_end = _boxes.end();
//..............................................................................


    for (; box_it != box_end; ++box_it)
    {
        const Box::StreamBindings inputs = box_it->get_inputs();
        Box::StreamBindings::const_iterator input_it = inputs.begin();

        for (; input_it != inputs.end(); ++input_it)
        {
            if ( stream_name == input_it->second.get_name() )
            return true;
        }
    }

    return (false);
}



////////////////////////////////////////////////////////////////////////////////
//
void  Query::remove_box(Name  box_name)
{
    vector<Box>::iterator box_it  = _boxes.begin();
    vector<Box>::iterator box_end = _boxes.end();
//..............................................................................


    for (; box_it != box_end; ++box_it)
    {
        if (box_it->get_name() == box_name)
        {
            const Box::StreamBindings inputs = box_it->get_inputs();
            const Box::StreamBindings outputs = box_it->get_outputs();

            Box::StreamBindings::const_iterator input_it = inputs.begin();

            for (; input_it != inputs.end(); ++input_it)
            {
                // Check for intermediates -> outputs
                vector<Name>::iterator in_intermediates =
                    std::find(_intermediate_streams.begin(), _intermediate_streams.end(), input_it->second.get_name());

                if ( in_intermediates != _intermediate_streams.end() )
                {
                    if ( !used_by_box(input_it->second.get_name()) )
                    {
                        _output_streams.push_back(input_it->second.get_name());
                        _intermediate_streams.erase(in_intermediates);
                    }
                }
            }

            Box::StreamBindings::const_iterator output_it = outputs.begin();

            for (; output_it != outputs.end(); ++output_it)
            {
                // Check for intermediates -> inputs
                vector<Name>::iterator in_intermediates =
                    std::find(_intermediate_streams.begin(), _intermediate_streams.end(), output_it->second.get_name());

                if ( in_intermediates != _intermediate_streams.end() )
                {
                    _input_streams.push_back(output_it->second.get_name());
                    _intermediate_streams.erase(in_intermediates);
                }
                else
                {   // Erase outputs
                    vector<Name>::iterator in_outputs =
                        std::find(_output_streams.begin(), _output_streams.end(), output_it->second.get_name());

                    if ( in_outputs != _output_streams.end() )
                    {
                        _output_streams.erase(in_outputs);
                        _stream_defs.erase(output_it->second.get_name());
                    }
                }
            }

            _boxes.erase(box_it);
            break;
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  Query::add_table(TableDescription t)
{
//..............................................................................


    _tables.push_back(t);
}



////////////////////////////////////////////////////////////////////////////////
//
bool  Query::find(Name what, vector<Name> where) const
{
//..............................................................................


    for (vector<Name>::const_iterator i = where.begin(); i != where.end(); ++i)
    {
        if ((*i) == what)  return true;
    }

    return (false);
}



////////////////////////////////////////////////////////////////////////////////
//
const vector<Name>  Query::get_non_input_streams() const
{
        vector<Name> result;
//..............................................................................


    copy(_output_streams.begin(),
         _output_streams.end(),
         back_inserter(result));

    copy(_intermediate_streams.begin(),
         _intermediate_streams.end(),
         back_inserter(result));

    return result;
}


////////////////////////////////////////////////////////////////////////////////
//
const vector<Name>  Query::get_all_streams() const
{
    vector<Name> result;
//..............................................................................

    copy(_input_streams.begin(), _input_streams.end(), back_inserter(result));
    copy(_output_streams.begin(), _output_streams.end(), back_inserter(result));
    copy(_intermediate_streams.begin(), _intermediate_streams.end(), back_inserter(result));

    return result;
}



////////////////////////////////////////////////////////////////////////////////
//
// Make a deep copy of the Query.  (Normally, copying makes only a shallow copy.)
//
Query  Query::clone() const
{
    StreamDefs::const_iterator i;
//..............................................................................


    Query q(get_name());
    q._owner = _owner;
    q._boxes = _boxes;
    q._tables = _tables;
    q._input_streams = _input_streams;
    q._output_streams = _output_streams;
    q._intermediate_streams = _intermediate_streams;


    for (i = _stream_defs.begin(); i != _stream_defs.end(); ++i)
    {   *(q._stream_defs[i->first]) = *(i->second);
    }

    q._snapshot = _snapshot;

    return q;
}



////////////////////////////////////////////////////////////////////////////////
//
// Split query
//
vector<Query>  Query::split_query(Query   query_to_split,
              map< Name, vector<Name> >   boxes_per_hunk)
{

    vector<Query>   split_queries;
//..............................................................................


    vector<Box> existing_boxes = query_to_split.get_boxes();

    map< Name, vector<Name> >::iterator i;

    for (i = boxes_per_hunk.begin(); i != boxes_per_hunk.end(); i++)
    {
        Query new_hunk(i->first);

        vector<Name>::iterator j;

        for (j = i->second.begin(); j != i->second.end(); j++)
        {
            for(vector<Box>::iterator k = existing_boxes.begin(); k != existing_boxes.end(); k++)
            {
                if (k->get_name() == *j)
                {
                    new_hunk.add_box(*k);
                    break;
                }
            }
        }

        for (j = new_hunk._input_streams.begin(); j != new_hunk._input_streams.end(); j++)
        {
            new_hunk.set_stream_def(*j, query_to_split.get_stream_def(*j));
        }

        for (j = new_hunk._output_streams.begin(); j != new_hunk._output_streams.end(); j++)
        {
            new_hunk.set_stream_def(*j, query_to_split.get_stream_def(*j));
        }

        for(j = new_hunk._intermediate_streams.begin(); j != new_hunk._intermediate_streams.end(); j++)
        {   new_hunk.set_stream_def(*j, query_to_split.get_stream_def(*j));
        }

        split_queries.push_back(new_hunk);
    }

    return (split_queries);
}


////////////////////////////////////////////////////////////////////////////////
//
// Merge query
//
Query  Query::merge_query(vector<Query> queries, Name new_query_name)
{
//..............................................................................


    Query new_query(new_query_name);

    for(vector<Query>::iterator i = queries.begin(); i != queries.end(); i++)
    {
        vector<Box> hunk_boxes = i->get_boxes();

        for (vector<Box>::iterator j = hunk_boxes.begin(); j != hunk_boxes.end(); j++)
        {
            new_query.add_box(*j);

            vector<Name>::iterator j;

            for(j = i->_input_streams.begin(); j != i->_input_streams.end(); j++)
            {
                new_query.set_stream_def(*j, i->get_stream_def(*j));
            }

            for(j = i->_output_streams.begin(); j != i->_output_streams.end(); j++)
            {   new_query.set_stream_def(*j, i->get_stream_def(*j));
            }

            for(j = i->_intermediate_streams.begin(); j != i->_intermediate_streams.end(); j++)
            {   new_query.set_stream_def(*j, i->get_stream_def(*j));
            }
        }
    }

    return (new_query);
}


BOREALIS_NAMESPACE_END
