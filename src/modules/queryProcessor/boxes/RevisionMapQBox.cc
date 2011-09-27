#include "RevisionMapQBox.h"
#include "tupleGenerator.h"

BOREALIS_NAMESPACE_BEGIN

void RevisionMapQBox::setup_impl() throw (AuroraException)
{
  set_emit_hist( true );

  if (get_num_inputs() != 1)
    Throw(aurora_typing_exception,
          "RevisionMap requires exactly one input stream (not " +
          to_string(get_num_inputs()) + ")");

  for (unsigned int i = 0; ; ++i)
    {
      string expr = param("expression." + to_string(i));
      if (expr.empty())
    {
      if (i == 0)
        Throw(aurora_typing_exception,
          "RevisionMap requires at least one expression parameter. (\"expression\" is deprecated)");
      else
        break;
    }
    
      ExprContext ctxt;
      //ctxt.setTupleDescription( TupleDescription( getInDescription( 0 ) ) );
      ctxt.set_context_schema( *get_in_schema( 0 ));
      _expressions.push_back(Expression::parse(expr, ctxt));
    }

  vector<string> field_names;
  for (unsigned int i = 0; i < _expressions.size(); ++i)
    {
      field_names.push_back(param("output-field-name." + to_string(i), PARAM_NON_EMPTY));
    }

  // Build the output TupleDescription based on the expressions
  //
  // NOTE: Expression::getReturnTypes() returns a vector of DataTypes, but
  //       map function can *only* return one type. This, we assume to be true (any other
  //       return types would mess things up due to getReturnedSize forcing the copy of more than needed)

  // bloot -- Dammit, why is this generated on the fly?
  //          We need a revised_field

  vector<SchemaField> fields;
  for (unsigned int i = 0; i < _expressions.size(); ++i)
    {
      string name = param("output-field-name." + to_string(i),
              PARAM_NON_EMPTY);
      fields.push_back(_expressions[i]->get_schema_field(name));
    }

  // Create the revised field
  int max_size = 0;
  int r_offset = HEADER_SIZE;

  for( unsigned int i = 0; i < fields.size(); i++ )
    {
      SchemaField this_field = fields.at(i);
      max_size = (max_size > this_field.get_size()) ? max_size
    : this_field.get_size();
      r_offset += this_field.get_size();
    }

  DEBUG << "Creating revised_field: size=" << max_size << "; offset=" << r_offset;

  // revised values of field being revised in rev_field (in header)
  SchemaField revised_field;
  revised_field.set_name( "revised_value" );
  revised_field.set_type_size( DataType::STRING, max_size );
  revised_field.set_offset( r_offset );

  fields.push_back( revised_field );

  // Set output description (there's only one output stream)
  TupleDescription t(fields.begin(), fields.end());

  set_out_description(0, t);

  // inverses
  for (unsigned int i = 0; i < _expressions.size(); ++i) 
    {
      string inv_expr = param("inverse." + to_string(i));
      DEBUG << inv_expr;
      if (inv_expr.empty()) 
    {
      DEBUG << get_name() << " map box is UNINVERTIBLE";
      _invertible = false;
      _inverses.clear();   // no need for inverses, clean up if something is there
      break;
    } 
      else 
    {
      ExprContext ctxt;
      ctxt.set_context_schema( *get_out_schema( 0 ));
      _inverses.push_back(Expression::parse(inv_expr, ctxt));
    }
    }
  if (_expressions.size() == _inverses.size()) 
    {
      DEBUG << get_name() << " map box is INVERTIBLE";
      _invertible = true;
    }

  /** MODE OF REVISION PROCESSING **/
  string mode = param("processing-mode", PARAM_NON_EMPTY);
  if (mode == "UPSTREAM") 
    {
      DEBUG << get_name() << " map box in UPSTREAM mode";
      _revision_processing_mode = UPSTREAM;
    } 
  else if (mode == "DOWNSTREAM") 
    {
      DEBUG << get_name() << " map box in DOWNSTREAM mode";
      _revision_processing_mode = DOWNSTREAM;
    }
  else 
    {
      Throw(aurora_typing_exception, "Unsupported RevisionProcessingMode: " + mode);
    }

  // anchors
  string anchor_name = param("anchor");
  split(anchor_name, ',', _anchor);
                
}

void RevisionMapQBox::init_impl() throw (AuroraException)
{
  // nothing to initialize
}

void RevisionMapQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{

  DeqIterator di = deq(0);
  EnqIterator ei = enq(0);
  bool outputted = false;
  bool need_cleanup = true;

  TupleDescription in_desc(  get_in_schema( 0 ));
  TupleDescription out_desc( get_out_schema( 0 ));

  while (inv.continue_dequeue_on(di, 0))
    {

      const void *data = di.tuple();
      Tuple input_tuple = Tuple::factory(data);
      char *out = (char*)ei.tuple();
      memset( out, ~'\0', out_desc.get_size() );
      bool stepping = // STEPPER: Change this to change paused-on tuples in DEBUG
    (input_tuple.get_tuple_type() == TupleType::REQUEST ||
     input_tuple.get_tuple_type() == TupleType::NOTIFICATION);

      INFO << "RevisionMapQBox " << get_name() << " IN: " << in_desc.tuple_to_string(data);
      if(stepping)
    {
      ////    DEBUG << (char)getchar();
    }

      if (input_tuple.get_fake_flag()) { // if a fake tuple, just pass along
    // to do..

      }

      // otherwise, it's real. Do as appropriate
      else if(    (    input_tuple.get_tuple_type() == TupleType::HISTORICAL
               || input_tuple.get_tuple_type() == TupleType::DELIMITER  )
          && !get_emit_hist() )
        {
      // End of upstream processing. Drop the historical tuples
      // bloot -- Something to DEBUG
      need_cleanup = false;
      ++di;
      NOTICE << "RevisionMapQBOX drop: "
         << in_desc.tuple_to_string( data );
      if(stepping)
        {
          ////        DEBUG << (char)getchar();
        }
        }
      else if ( input_tuple.get_tuple_type() == TupleType::INSERTION
        || input_tuple.get_tuple_type() == TupleType::DELETION
        || input_tuple.get_tuple_type() == TupleType::HISTORICAL
        || input_tuple.get_tuple_type() == TupleType::DELIMITER  )
        {
      // Do the evaluation, pass through
      memcpy(out, data, HEADER_SIZE);
      char *p = out + HEADER_SIZE;
      _ctxt.reset();
      _ctxt.set_tuple((const char *)data);
      Expression::eval_vector_into(_expressions, p, _ctxt);
        }
      else if( input_tuple.get_tuple_type() == TupleType::REPLACEMENT )
        {
      // Replacement tuple. Evaluate as old and new, send out replacements
      // with the differences.
      need_cleanup = false;

      // Create tuple as revised
      char new_data[ in_desc.get_size() ];
      memcpy( new_data, data, in_desc.get_size() );
      Tuple new_input_tuple = Tuple::factory( new_data );

      string revised_val = get_revised_val(new_input_tuple, *(get_in_schema(0)));
      const SchemaField *revised_field
        = in_desc.get_schema_field(new_input_tuple.get_revised());
      revised_val.copy( new_data + revised_field->get_offset(),
                revised_field->get_size() );

      // Set up old and new outputs
      char old_out_data[ out_desc.get_size() ];
      char new_out_data[ out_desc.get_size() ];
      memcpy( old_out_data, data, HEADER_SIZE );
      memcpy( new_out_data, data, HEADER_SIZE );
      char *p = old_out_data + HEADER_SIZE;
      _ctxt.reset();
      _ctxt.set_tuple((const char *)data);
      Expression::eval_vector_into(_expressions, p, _ctxt);
      p = new_out_data + HEADER_SIZE;
      _ctxt.reset();
      _ctxt.set_tuple((const char *)new_data);
      Expression::eval_vector_into(_expressions, p, _ctxt);
      Tuple output_tuple = Tuple::factory(old_out_data);
      Tuple new_output_tuple = Tuple::factory(new_out_data);

      // Do comparisons. On finding something different, create the
      // appropriate replacement tuple
      for( uint16 i = 0; i < out_desc.get_num_fields(); i++ )
            {
          if (out_desc.get_schema_field(i)->get_name()  == "revised_value" )
                {
          continue;
                }
          bool different = false;
          int offset = out_desc.get_offset(i);
          for( unsigned int j = 0;
           j < out_desc.get_schema_field(i)->get_size();
           j++ )
                {
          different = different || ( old_out_data[ offset + j ] != new_out_data[ offset + j ] );
          if( different )
                    {
              break;
                    }
                }
          if( different )
                {
          // bloot
          // We assume here that any field being revised is 4 bytes.
          // Therefore, we can do simple assignment of
          // Tuple::RevisionVal
          // While this is the Wrong Thing, it is what we're doing
          // until a variable-size revision value field gets added
          // into the end of the tuple.

          // Change and output output_tuple, which will therefore be a
          // running version of the tuple as changes are found.
          output_tuple.set_revised( i );
          revised_val.assign( new_out_data + offset,
                      out_desc.get_schema_field(i)->get_size() );
          set_revised_val( output_tuple,
                                   *(out_desc.get_schema()),
                                   revised_val );
          memcpy( out, old_out_data, out_desc.get_size() );

          INFO << "RevisionMapQBox " << get_name() << " OUT: "
                         << out_desc.tuple_to_string(ei.tuple());
          if(stepping)
            {
              ////            DEBUG << (char)getchar();
            }

          ++ei;
          memcpy( old_out_data + offset,
              new_out_data + offset,
              sizeof(int)            );
          outputted = true;
                }
            }
      ++di;
        }
      else if( input_tuple.get_tuple_type() == TupleType::REQUEST )
    {
      // Propogate requests...

      // Create new request tuple (i.e. header)
      uint8  new_header[HEADER_SIZE];
      memcpy( new_header, data, HEADER_SIZE );
      Tuple new_tuple = Tuple::factory(new_header);
      new_tuple.set_revised(new_tuple.get_revision_id()); // old revision id in rev_field
      new_tuple.set_revision_id(_local_boxid);
      // no need to change win_spec and tuple_id

      // Send it both downstream...
      memset( out, '\0', out_desc.get_size() );
      memcpy( out, new_header, HEADER_SIZE );
            
      // ... and to the sweeper
      send_to_sweeper(new_header);
      INFO << "RevisionMapQBox " << get_name() << " OUT to SWEEPER: "
         << out_desc.tuple_to_string( new_header );
      if (stepping)
        {
          ////        DEBUG << getchar();
        }
    }
      else if( input_tuple.get_tuple_type() == TupleType::NOTIFICATION )
    {
      need_cleanup = false;
      // Grab HIS, pass through inverses, send back to sweeper
      // TODO: proper multi-sweeper behavior
      uint8  *hist_tuple = retrieve_historical( (uint8 *)data );
      //char *hist_tuple = _sweeper_box->notify( data );
      //assert( hist_tuple != NULL );
      if( hist_tuple != NULL )
        {
          DEBUG << "HISTORICAL: " << out_desc.tuple_to_string(hist_tuple);
          uint8  inverted_tuple[in_desc.get_size()];
          memcpy( inverted_tuple, hist_tuple, HEADER_SIZE );
          char *p = (char *)inverted_tuple + HEADER_SIZE;
          _ctxt.reset();
          _ctxt.set_tuple(hist_tuple);
          Expression::eval_vector_into( _inverses, p, _ctxt );

          send_to_sweeper(inverted_tuple);
          INFO << "RevisionMapQBox " << get_name() << " OUT to SWEEPER: "
             << in_desc.tuple_to_string( inverted_tuple );
          if (stepping)
        {
          ////        DEBUG << getchar();
        }
          ++di;
        }
    }
      else
        {
      need_cleanup = false;
      NOTICE << "RevisionMapQBox drop unrecognized tuple: "
                 << in_desc.tuple_to_string( data );
      if(stepping)
        {
          ////        DEBUG << (char)getchar();
        }
      ++di;
        }
      if( need_cleanup )
        {
      // Lovely, pass along...
      INFO << "RevisionMapQBox " << get_name() << " OUT: " << out_desc.tuple_to_string(ei.tuple());
      if(stepping)
        {
          ////        DEBUG << (char)getchar();
        }
      outputted = true;
      ++ei;
      ++di;
        }
    }

  if (outputted)
    get_output(0).notify_enq();
}


BOREALIS_NAMESPACE_END
