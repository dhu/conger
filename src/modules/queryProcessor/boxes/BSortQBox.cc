#include "BSortQBox.h"

BOREALIS_NAMESPACE_BEGIN

void BSortQBox::setup_impl() throw (AuroraException)
{
    if (get_num_inputs() != 1)
        Throw(aurora_typing_exception,
              "BSort requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");


    // Get the parameters
    string s;
    s = param("order-on", PARAM_NON_EMPTY); // ORDER ON
    _field_att = new FieldExt(s.c_str());
    int i;
    typed_param("slack", i, PARAM_NON_EMPTY); // SLACK
    _slack = i;

    s = param("group-by");
    if (s != "") { // GROUP-BY (optional)
        parse_group_by((char*) s.c_str());
    } else
        parse_group_by(NULL); // needed cuz parseGroupBy has necessary side effects (oxymoron?)

    // Set the output stream description
    set_out_description(0, get_in_stream(0)); // same schema comes out
}


void BSortQBox::init_impl() throw (AuroraException)
{
    // Initialize the buffer hash
    _buffer_hash = new HashForBufferList();
    TupleDescription out_td(get_out_schema(0));
    _output_tuple_size = out_td.get_size();
}



void BSortQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    unsigned int max_tuples = inv.get_max_tuples_to_dequeue(0);
    DeqIterator my_deq_iterator = deq(0);
    EnqIterator my_enq = enq(0);
    bool output_occurred = false;

    //TupleDescription in_desc(getInDescription(0));
    //TupleDescription in_desc(getInDescription(0));

    while (max_tuples-- && my_deq_iterator.avail())
    {
        const void *data = my_deq_iterator.tuple();
        ++my_deq_iterator;

        //Fix up???
        //INFO << "BSortQBox IN: " << in_desc.tupleToString(data);

        // Tuple is copied in order to maintain it in the box's state. It is deleted upon emission.
        _curr_tuple = (char*) malloc(_output_tuple_size);
        memcpy(_curr_tuple, data, _output_tuple_size);

        add_to_buffer(_curr_tuple);

        if (_buffer_hash->get_buffer_list_size(_group_by_values_str) == (_slack + 1))
        { // Then emit the lowest value
            // OPTIMIZATION: You could emit as soon as you are adding (addToBuffer)...
            emit_lowest(_buffer,my_enq); // NOTE. the use of _buffer here depends on addToBuffer setting _buffer
            output_occurred = true;
        }
    }

    // Done, notify if appropriate
    if (output_occurred) get_output(0).notify_enq();
}


/**
 * BSORT
 * All Timestamping stuff commented out Mar 26 03 since it seems we no longer
 * care about any timeouts in BSORT
 */



/**
 * Due to reasons baffled in HashForBufferList.C and below, to create the "group by values string"
 *  used for hashing, one needs to collapse any numerical values into a "string representation"
 *  of it. Why? numericals may use a nul bytes (a byte where all 8 bits are 0), but then if you
 *  use that in the context of the hashing function, the nul byte (say decimal 0) is seen as
 *  the "end of string" destroying our intention of what we want hashed...

 * [original bug mentioning]
 * !BUG HASHNUL! If you just squish the bytes for each group_by value, and use it as a hash
 *  key, you get some region in memory (char*) that has some null bytes in it, and guess what!
 *  The hash function within hash_map stops at the first nul byte! (and in our representation,
 *   an integer, using 4 bytes, is represented as ("4 nul nul nul") so if you combine two values
 *   you get something like ("4 nul nul nul 2 nul nul nul"), supposed to be the key "4-2" but
 *   due to the nul bytes, the hash will only look at "4" so "4-3" hashes to the same place in the buffer!
 *  The real reason, is because __stl_hash_string(char *), obviously, stops at the first nul byte of the char*

 *
 * SOOO Take numericals (floats, ints, anything). Make them "strings". squeeze them together by
 *  putting a 0x01 byte in between each "string". For string, just use their chars.
 */
void BSortQBox::add_to_buffer( char  *tuple )
{
    // deal with the sorting attribute first
    //  int att_size = _field_att->getReturnedSize();
    char *att_value;
    char _att_type = _field_att->getType();

    if (_att_type == 's')
    {
        att_value = _field_att->evaluate(tuple); // deleted in emitLowest()
    }
    else
    {
        att_value = _field_att->evaluate(tuple);  // deleted in emitLowest()
    }

    // WARNING: _field_att->evaluate() for a string returns
    //  a null-terminated string (so its actual bytes is getReturnedSize() + 1)
    //   SOOOO I fixed it -- see the above two calls to memcpy
    // NOTE!!!!!!!!!!! THIS, I THINK, REALLY SHOULD BE FIXED BY MAKING FIELDEXT
    //  RETURN (SIZE + 1) WHEN FIELDEXT IS DEALING WITH A STRING!!!
    // if this "fix" is done, rewrite the stuff above to just have the one call below
    //  - eddie
    //memcpy(att_value, _field_att->evaluate(tuple), att_size);
    BufferList::node new_node = {att_value, tuple};

    // NEW _group_by_values_str: holds as explained before function, char representation of all
    //  values (numericals are made into chars by sprintf), with 0x01 in between, ending in \0
    //   Why the temp buffer starts at (64 + 1) * num of attribs? See FieldExt.C
    _group_by_values_str = (char*) realloc(_group_by_values_str,(65 * _num_group_by_atts) + 1);
    char* _group_by_values_str_i = _group_by_values_str; // temporary pointer
    int32 bytes_needed = 0; int bytes_needed_total = 0;

    for (int ii = 0; ii < _num_group_by_atts; ii++)
    {
        if (ii > 0)
        {   // put the 0x01 chararacter between values
            const char sep = 0x01;
            memcpy(_group_by_values_str_i,&sep,1);
            ++_group_by_values_str_i; ++bytes_needed_total;
        }

        //_field_group[ii]->evaluateAsChar(tuple,bytesNeeded);
        // Careful here -- this is exploiting the fact that first the evaluateAsChar is evaluated,
        //  setting bytesNeeded which THEN is read for memcpy which does its job copying the memory
        //  and only AFTER will the rvalue returned by evaluateAsChar get destroyed
        //  WARNING -- the paragraph above describes something that ISNOT TRUE.
        //              You need a temp, that you clean up right away anyways
        char* temp_group_by_values_str = _field_group[ii]->evaluate_as_char(tuple,bytes_needed);
        memcpy(_group_by_values_str_i,temp_group_by_values_str,bytes_needed);
        free(temp_group_by_values_str);
        _group_by_values_str_i += bytes_needed;
        bytes_needed_total += bytes_needed;
    }

    // Squeeze it back down, putting the \0 at the end too
    _group_by_values_str = (char*) realloc(_group_by_values_str,bytes_needed_total+1);
    _group_by_values_str[bytes_needed_total] = '\0';

    if (_buffer_hash->is_empty() ||
        _buffer_hash->get_buffer_list(_group_by_values_str) == NULL)   // nothing in the hash
    {
        _buffer = new BufferList(_group_by_values_str, bytes_needed_total+1);
        // I SWAPPED THE POSITION OF THE NEXT TWO LINES -eddie
        //   because, amongst other things, I think the hash_map internally
        //   just created a copy? which would meant the push_front wouldn't do much, but perhaps I'm wrong
        _buffer->push_front(new_node);   // add node to BufferList
        _buffer_hash->add_buffer_list(_buffer);  // finally add the BufferList to hash
        // NOTE: Tempting to add _group_by_values_str to addBufferList call, but see comment in HashForBufferList
    }
    else
    {
        if (_buffer_hash->get_buffer_list_size(_group_by_values_str) == 0)
        {
            _buffer = _buffer_hash->get_buffer_list(_group_by_values_str);
            _buffer->push_front(new_node);     // nothing else in this list
        }
        else
        {
            // This whole "different adding needed depending on the attribute type
            //  is only here because we need to compare the values of the types (which must be
            //  cast from the (char*) buffer into whatever it needs to be - eddie
            switch (_att_type)
            {
              case 'f':
                add_single_to_buffer( att_value, new_node );
                break;

              case '1':
                add_single_to_buffer( att_value, new_node );
                break;

              case '2':
                add_double_to_buffer( att_value, new_node );
                break;

              case 'i':
                add_int_32to_buffer( att_value, new_node );
                break;

              case 'L':
                  add_int_64to_buffer( att_value, new_node );
                  break;

              case 'S':
                  add_int_16to_buffer( att_value, new_node );
                  break;

              case 'B':
                  add_int_8to_buffer( att_value, new_node );
                  break;

              case 's':
                 add_string_to_buffer( att_value, new_node );
                 break;
            }
        }
    }
}


/**
 * FOR ALL THE addXXXToBuffer I wonder... couldn't you just insert anywhere then call sort ?
 */

void BSortQBox::add_single_to_buffer( char  *att_value,
                       BufferList::node   new_node )
{
    single curr_att_value = *(single*)att_value;
    single buffer_att_value;

    _buffer = _buffer_hash->get_buffer_list( _group_by_values_str );

    buffer_att_value = *(single*) _buffer->front().att;

    if ( curr_att_value < buffer_att_value )
    {   _buffer->push_front( new_node );
    }
    else
    {
        buffer_att_value = *(single*) _buffer->back().att;

        if ( curr_att_value >= buffer_att_value )
        {
            //cout << "[BSortQBox] Buffer: PUSH BACK " << endl;
            _buffer->push_back( new_node );
        }
        else
        {
            list<BufferList::node>::iterator iter = _buffer->begin();
            ++iter;    // first node already checked

            BufferList::node temp_node;

            while ( iter != _buffer->end() )
            {
                temp_node = *iter;
                buffer_att_value = *(single*) temp_node.att;

                if ( curr_att_value < buffer_att_value )
                {
                    _buffer->insert( iter, new_node );
                    break;
                }

                ++iter;
            }
        }
    }
}


void BSortQBox::add_double_to_buffer( char  *att_value,
                       BufferList::node   new_node )
{
    double curr_att_value = *(double*)att_value;
    double buffer_att_value;

    _buffer = _buffer_hash->get_buffer_list( _group_by_values_str );

    buffer_att_value = *(double*) _buffer->front().att;

    if ( curr_att_value < buffer_att_value )
    {   _buffer->push_front( new_node );
    }
    else
    {
        buffer_att_value = *(double*) _buffer->back().att;

        if ( curr_att_value >= buffer_att_value )
        {
            //cout << "[BSortQBox] Buffer: PUSH BACK " << endl;
            _buffer->push_back( new_node );
        }
        else
        {
            list<BufferList::node>::iterator iter = _buffer->begin();
            ++iter;    // first node already checked

            BufferList::node temp_node;

            while ( iter != _buffer->end() )
            {
                temp_node = *iter;
                buffer_att_value = *(double*) temp_node.att;

                if ( curr_att_value < buffer_att_value )
                {
                    _buffer->insert( iter, new_node );
                    break;
                }

                ++iter;
            }
        }
    }
}


void BSortQBox::add_int_32to_buffer( char  *att_value,
                      BufferList::node   new_node )
{
    int32  curr_att_value = *(int32*) att_value;
    int32  buffer_att_value;

    _buffer = _buffer_hash->get_buffer_list( _group_by_values_str );

    assert( _buffer != NULL );
    assert( _buffer->size() > 0 );
    assert( ! _buffer->is_empty() );

    buffer_att_value = *(int32*) _buffer->front().att;

    if ( curr_att_value < buffer_att_value )
    {    _buffer->push_front( new_node );
    }
    else
    {    buffer_att_value = *(int32*)_buffer->back().att;

         if ( curr_att_value >= buffer_att_value )
         {   _buffer->push_back( new_node );
         }
         else
         {
             list<BufferList::node>::iterator iter = _buffer->begin();
             ++iter;    // first node already checked

             BufferList::node  temp_node;

             while ( iter != _buffer->end() )
             {
                 temp_node = *iter;
                 buffer_att_value = *(int32*)temp_node.att;

                 if ( curr_att_value < buffer_att_value )
                 {   _buffer->insert( iter, new_node );
                     break;
                 }

                 ++iter;
             }
        }
    }
}



void BSortQBox::add_int_16to_buffer( char  *att_value,
                      BufferList::node   new_node )
{
    int16  curr_att_value = *(int16*) att_value;
    int16  buffer_att_value;

    _buffer = _buffer_hash->get_buffer_list( _group_by_values_str );

    assert( _buffer != NULL );
    assert( _buffer->size() > 0 );
    assert( ! _buffer->is_empty() );

    buffer_att_value = *(int16*) _buffer->front().att;

    if ( curr_att_value < buffer_att_value )
    {    _buffer->push_front( new_node );
    }
    else
    {    buffer_att_value = *(int16*)_buffer->back().att;

         if ( curr_att_value >= buffer_att_value )
         {   _buffer->push_back( new_node );
         }
         else
         {
             list<BufferList::node>::iterator iter = _buffer->begin();
             ++iter;    // first node already checked

             BufferList::node  temp_node;

             while ( iter != _buffer->end() )
             {
                 temp_node = *iter;
                 buffer_att_value = *(int16*)temp_node.att;

                 if ( curr_att_value < buffer_att_value )
                 {   _buffer->insert( iter, new_node );
                     break;
                 }

                 ++iter;
             }
        }
    }
}



void BSortQBox::add_int_8to_buffer( char  *att_value,
                      BufferList::node   new_node )
{
    int8  curr_att_value = *(int8*) att_value;
    int8  buffer_att_value;

    _buffer = _buffer_hash->get_buffer_list( _group_by_values_str );

    assert( _buffer != NULL );
    assert( _buffer->size() > 0 );
    assert( ! _buffer->is_empty() );

    buffer_att_value = *(int8*) _buffer->front().att;

    if ( curr_att_value < buffer_att_value )
    {    _buffer->push_front( new_node );
    }
    else
    {    buffer_att_value = *(int8*)_buffer->back().att;

         if ( curr_att_value >= buffer_att_value )
         {   _buffer->push_back( new_node );
         }
         else
         {
             list<BufferList::node>::iterator iter = _buffer->begin();
             ++iter;    // first node already checked

             BufferList::node  temp_node;

             while ( iter != _buffer->end() )
             {
                 temp_node = *iter;
                 buffer_att_value = *(int8*)temp_node.att;

                 if ( curr_att_value < buffer_att_value )
                 {   _buffer->insert( iter, new_node );
                     break;
                 }

                 ++iter;
             }
        }
    }
}



void BSortQBox::add_int_64to_buffer( char  *att_value,
                      BufferList::node   new_node )
{
    int64  curr_att_value = *(int64*) att_value;
    int64  buffer_att_value;

    _buffer = _buffer_hash->get_buffer_list( _group_by_values_str );

    assert( _buffer != NULL );
    assert( _buffer->size() > 0 );
    assert( ! _buffer->is_empty() );

    buffer_att_value = *(int64*) _buffer->front().att;

    if ( curr_att_value < buffer_att_value )
    {    _buffer->push_front( new_node );
    }
    else
    {    buffer_att_value = *(int64*)_buffer->back().att;

         if ( curr_att_value >= buffer_att_value )
         {   _buffer->push_back( new_node );
         }
         else
         {
             list<BufferList::node>::iterator iter = _buffer->begin();
             ++iter;    // first node already checked

             BufferList::node  temp_node;

             while ( iter != _buffer->end() )
             {
                 temp_node = *iter;
                 buffer_att_value = *(int64*)temp_node.att;

                 if ( curr_att_value < buffer_att_value )
                 {   _buffer->insert( iter, new_node );
                     break;
                 }

                 ++iter;
             }
        }
    }
}




void BSortQBox::add_string_to_buffer(char *att_value, BufferList::node new_node)
{
  // the +1 here is because we will add the \0. WARNING if you change FieldExt
  // semantics to add the 1...
  int att_size = _field_att->get_returned_size() + 1;
  char *buffer_att_value = new char[att_size];

  _buffer = _buffer_hash->get_buffer_list(_group_by_values_str);

  // Can the new string go at the front?
  memcpy(buffer_att_value, _buffer->front().att, att_size);
  // WARNING!!! Shouldnt this check for equality too? I mean if its equal
  //  you could just stick it in front (or rather, right after)
  if (strcmp(att_value, buffer_att_value) < 0)
    {
      _buffer->push_front(new_node);
    }
  else
    {
      // does it go all the way at the back?
      memcpy(buffer_att_value, _buffer->back().att, att_size);
      if (strcmp(att_value, buffer_att_value) >= 0)
    {
      _buffer->push_back(new_node);
    }
      else
    { // goes somewhere in the middle, figure it out
      list<BufferList::node>::iterator iter = _buffer->begin();
      ++iter;    // first node already checked

      BufferList::node temp_node;
      while (iter != _buffer->end())
        {
          temp_node = *iter;
          memcpy(buffer_att_value, temp_node.att, att_size);
          if (strcmp(att_value, buffer_att_value) < 0)
        {
          _buffer->insert(iter, new_node);
          break;
        }
          ++iter;
        }
    }
    }
}


void BSortQBox::emit_lowest( BufferList *buffer,EnqIterator enq )
{
    // Euh... "The c++ stl reference" says pop_front() does NOT return the element...
    //  yet this works.. hmm
    // BAAAAAAAAAA ignore me. buffer isnt a vector, its a BufferList which DOES return it...
    BufferList::node n = buffer->pop_front();

    //TupleDescription out_desc(getOutDescription(0));
    //DEBUG << "BSortQBox OUT: " << out_desc.tupleToString(n.tuple);
    DEBUG << "BSortQBox OUT: fix up???";
    memcpy(enq.tuple(),n.tuple,_output_tuple_size);
    ++enq;

    // cleanup
    delete[] n.att;
    free(n.tuple);
}


// SideEffect: creates _field_group with ptrs to FieldExt's for each groupby
//             and sets _num_group_by_atts. It should also set _group_by_size,
//             the # of bytes needed to store all values of the group by attrs
//             (here, DO NOT INCLUDE THE \0 in counting size of strings)
// example atts = ":0:i:0:4:~:0:i:0:4:~:0:i:0:4:"
//    variables set by parseGroupBy
//                    _field_group (FieldExt)
//                    _group_by_size (int)
//                    _num_group_by_atts (int)
// April 2: GOOD BYE _GROUP_BY_SIZE (want the size? use strlen on _group_by_values_str)
// April 3 - its "," seperated, not ~ anymore
void BSortQBox::parse_group_by(char *atts)
{
    _group_by_values_str = NULL;

    if (atts == NULL || strcmp(atts,"") == 0)
    { // Check both that it is "null" and the empty string (not the same)
        // No group_by attributes means this operator basically groups everyone in the same place
        // OPTIMIZATION OPPORTUNITY: we could then not even use the hash...
        _num_group_by_atts = 0;

        return;
    }

    // We assume atts is null-terminated
    char *p = index(atts, ',');

    if (p == NULL)
    {   // No match for "~" means only one group by
        _num_group_by_atts = 1;
        _field_group = new FieldExt*[1];
        _field_group[0] = new FieldExt(atts);
        return;
    }
    else
    {   // Let's figure out how many group bys
        int num_atts = 2; // At least two already
        p = index(p+1,','); // Resume search after the ~

        while ( p != NULL )
        {   num_atts++;
            p = index(p+1,',');
        }

        _field_group = new FieldExt*[num_atts];

        // Now that we know, create the FieldExts in _field_group
        p = atts;
        // Careful - the loop below goes by num_atts not by what p points to
        //            but that should be equivalent
        for (int i = 0; i < num_atts; i++)
        {
            _field_group[i] = new FieldExt(p);
            p = index(p+1,',');

          if (p != NULL) p++; // To skip the ~ (FieldExt doesnt want it)
        }

        _num_group_by_atts = num_atts;

        return;
    }
}


BOREALIS_NAMESPACE_END
