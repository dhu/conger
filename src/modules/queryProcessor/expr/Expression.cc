#include "Expression.h"
#include "ExprUtil.h"
#include "ExprLexer.hpp"
#include "ExprParser.hpp"
#include "ExprWalker.hpp"
#include "antlr/AST.hpp"
#include "antlr/CommonAST.hpp"
#include "EString.h"

BOREALIS_NAMESPACE_BEGIN

NMSTL_SERIAL_DEFINE(Expression, 20);


ptr<Expression> Expression::parse( string expr, const ExprContext  &ctxt )
                            throw( ExprException )
{
    try
    {
        DEBUG << "Parsing expression {" << expr << "}";
        istringstream  iss( expr );
        ExprLexer      lexer( iss );
        ExprParser     parser( lexer );
        antlr::ASTFactory ast_factory;

        parser.initializeASTFactory( ast_factory );
        parser.setASTFactory( &ast_factory );

        parser.expr();
        ASSERT( parser.getAST() );

        DEBUG << "AST: " << parser.getAST()->toStringList();
        string theParse = parser.getAST()->toStringList();
        

        ExprWalker walker;
        
        ptr<Expression> expr = walker.expr( parser.getAST(), ctxt );
        if(theParse.find("->") < theParse.size())
        	expr->_array_flag = true;
        else 
        	expr->_array_flag = false; 
        
        ASSERT( expr );

        //DEBUG << "Expression:\n" << indent( to_string( *expr ), 4 );
        return( expr );
    }
    catch ( antlr::ANTLRException  &e )
    {   Throw( ExprException, e.getMessage() );
    }
}


SchemaField Expression::get_schema_field( string  name ) const
                                 throw( ExprException )
{
    int32 length = getLength();

    if ( length < 0 )
    {   Throw( ExprException,
               "Cannot create field from variable-length expression" );
    }

    if ( _type == DataType::BOOL )
    {   Throw( ExprException, "Cannot create field from bool" );
    }

    return( SchemaField( name, _type, length ));
}


void Expression::eval_vector_into( const vector<ptr<Expression> >  &exprs,
                                                           char  *buf,
                                              const EvalContext  &ctxt
                               )
                          throw( EvalException )
{
    vector<ptr<Expression> >::const_iterator i;

    for ( i = exprs.begin(); i != exprs.end(); ++i )
    {
        Expression &e = **i;

        // Reset the pool, leave the tuple intact.
        e.eval_into( buf, ctxt );
        ASSERT( e.getLength() != -1 );
        buf += e.getLength();
    }
}

ptr<Expression> Expression::makeArrayReference (string array_name, string field_name, const ExprContext &ctxt)
throw( ExprException )
{
	const CatalogSchema  *desc = NULL;
    unsigned int      field_index;
    unsigned int      tuple_index = 0;
	const SchemaField    *field;
	SchemaArray *array_info;
	unsigned int array_offset = 0;
	DataType field_type;
	
	// expand the pointer and make sure the context makes sense
	DEBUG << "Checking for array " << array_name << " field name " << field_name;
	
    // search for field name
    desc = ctxt.get_context_schema();
    
    field_index = desc->index_of_field(array_name);
    DEBUG << "Field index: " << field_index;
    // may need to add this conditionally for when we have multiple input streams
	//ctxt.find_field_name(array_name, array_name, tuple_index, desc, field_index);

    field = desc->get_schema_field( array_name );
    array_info = field->get_array();
    if(array_info->get_element_type() == DataType::NONE)
    {
    	// search schema for arg2
    	CatalogSchema *tuple = array_info->get_tuple();
    	const SchemaField *accessed_field = tuple->get_schema_field(field_name);
    	if(accessed_field == NULL)
    		Throw( ExprException, "no subfield named \"" + field_name + "\" found." );
    	array_offset = accessed_field->get_offset();
    	DEBUG << "Array offset = " << array_offset;
    	array_offset -= get_tuple_header_size();
    	// TODO: make array offset accommodate sparse array coordinate information
    	field_type = accessed_field->get_type();
    }
    else
    {
    	 
    	if(field_name != array_info->get_element_type().as_string())
    		Throw( ExprException, "incorrect subtype: \"" + field_name + "\"" );
    	
    	// array offset defaults to 0
    	field_type.from_string(field_name); 
    }
    if ( field_type == DataType::INT )
    {   return( ptr<Expression>(
                    new IntFieldExpression( tuple_index,
					    desc->get_field_offset( field_index ), array_name,
					    array_offset, field_name )));
    }
    else if ( field_type == DataType::LONG )
    {   return( ptr<Expression>(
                    new LongFieldExpression( tuple_index,
					     desc->get_field_offset( field_index ), array_name,
					     array_offset, field_name )));
    }
    else if ( field_type == DataType::BYTE )
    {   return( ptr<Expression>(
                    new ByteFieldExpression( tuple_index,
					     desc->get_field_offset( field_index ), array_name,
					     array_offset, field_name )));
    }
    else if ( field_type == DataType::SHORT )
    {   return( ptr<Expression>(
                    new ShortFieldExpression( tuple_index,
					     desc->get_field_offset( field_index ), array_name,
					     array_offset, field_name )));
    }
    else if ( field_type == DataType::SINGLE )
    {
        return( ptr<Expression>(
                    new SingleFieldExpression( tuple_index,
					     desc->get_field_offset( field_index ), array_name,
					     array_offset, field_name )));
    }
    else if ( field_type == DataType::DOUBLE )
    {   return( ptr<Expression>(
                    new DoubleFieldExpression( tuple_index,
					     desc->get_field_offset( field_index ), array_name,
					     array_offset, field_name )));


    }
    else if ( field_type == DataType::STRING )
    {   return( ptr<Expression>(
                    new StringFieldExpression( tuple_index,
                        desc->get_field_offset( field_index ),
                        array_name,
                        field->get_size(), array_offset, field_name )));
    }
    else if ( field_type == DataType::TIMESTAMP )
    {   return( ptr<Expression>(
                    new TimestampFieldExpression( tuple_index,
					     desc->get_field_offset( field_index ), array_name,
					     array_offset, field_name )));
    }
    else
    {   Throw( ExprException, "internal error: bad DataType" );
    }
}
ptr<Expression> Expression::makeFieldExpression( string   tuple_name,
                                                  string   field_name,
                                       const ExprContext  &ctxt
                                                )
                                           throw( ExprException )
{
    //TupleDescription  desc;
    const CatalogSchema  *desc = NULL;
    unsigned int      tuple_index;
    unsigned int      field_index;

    DEBUG << "Making field expr for " << tuple_name << " field "
      << field_name;

    //if ( tuple_name.empty() && !ctxt.getTupleDescription() )
    if ( tuple_name.empty() && !(ctxt.get_context_schema()->_field.size()) )
    {
        // search for field name
        ctxt.find_field_name(field_name, tuple_name, tuple_index, desc, field_index);
    }
    else
    {   if ( tuple_name != "" )
        {   ctxt.get_named_schema_info( tuple_name, desc, tuple_index );
            //ctxt.getNamedTupleInfo( tuple_name, desc, tuple_index );

            if ( !desc )
            {   Throw( ExprException, "no tuple named \"" + tuple_name + "\"" );
            }
        }
        else
        {   tuple_index = 0;
            desc = ctxt.get_context_schema();
            //desc = ctxt.getTupleDescription();

            if ( !desc )
            {   Throw( ExprException, "no tuple description in context" );
            }
        }


        //if ( !desc.getSchema() )
        if ( !desc )
        {   DEBUG << "makeFieldExpression: no catalog schema set!";
        }
        else
        {   DEBUG << "makeFieldExpression: schema  "
                  << desc->as_string();
        }

        int fi = desc->index_of_field( field_name );

        if ( fi < 0 )
        {   if ( tuple_name != "" )
            {   Throw( ExprException, "no such field " + field_name
                                    + " in tuple " + tuple_name );
            }
            else
            {   Throw( ExprException, "no such field " + field_name );
            }
        }

        field_index = fi;
    }

    DEBUG << "Accessing field name " << field_name << " as field " << field_index
      << " at offset " << desc->get_field_offset( field_index )
      << " in " << desc->as_string() << ", tuple " << tuple_index;

    const SchemaField    *field;

    field = desc->get_schema_field( field_index );

    if ( field->get_type() == DataType::INT )
    {   return( ptr<Expression>(
                    new IntFieldExpression( tuple_index,
                        desc->get_field_offset( field_index ),
                        field_name, 0, "" )));
    }
    else if ( field->get_type() == DataType::LONG )
    {   return( ptr<Expression>(
                    new LongFieldExpression( tuple_index,
                         desc->get_field_offset( field_index ),
                         field_name, 0, "" )));
    }
    else if ( field->get_type() == DataType::BYTE )
    {   return( ptr<Expression>(
                    new ByteFieldExpression( tuple_index,
                         desc->get_field_offset( field_index ),
                         field_name, 0, "" )));
    }
    else if ( field->get_type() == DataType::SHORT )
    {   return( ptr<Expression>(
                    new ShortFieldExpression( tuple_index,
                         desc->get_field_offset( field_index ),
                         field_name, 0, "" )));
    }
    else if ( field->get_type() == DataType::SINGLE )
    {
        return( ptr<Expression>(
                    new SingleFieldExpression( tuple_index,
                           desc->get_field_offset( field_index ),
                           field_name, 0, "" )));
    }
    else if ( field->get_type() == DataType::DOUBLE )
    {   return( ptr<Expression>(
                    new DoubleFieldExpression( tuple_index,
                           desc->get_field_offset( field_index ),
                           field_name, 0, "" )));
    }
    else if ( field->get_type() == DataType::STRING )
    {   return( ptr<Expression>(
                    new StringFieldExpression( tuple_index,
                        desc->get_field_offset( field_index ),
                        field_name,
                        field->get_size(), 0, "" )));
    }
    else if ( field->get_type() == DataType::TIMESTAMP )
    {   return( ptr<Expression>(
                    new TimestampFieldExpression( tuple_index,
                          desc->get_field_offset( field_index ),
                          field_name, 0, "" )));
    }
    else
    {   Throw( ExprException, "internal error: bad DataType" );
    }
}


/// Return a field suitable for holding this expression.
SchemaField Expression::to_field( string  name ) const
                          throw( AuroraException )
{
    if ( getType() == DataType::INT )
    {   return( SchemaField( name, DataType::INT, sizeof( int32 )));
    }
    else if ( getType() == DataType::LONG )
    {   return( SchemaField( name, DataType::LONG, sizeof( int64 )));
    }
    else if ( getType() == DataType::BYTE )
    {   return( SchemaField( name, DataType::BYTE, sizeof( int8 )));
    }
    else if ( getType() == DataType::SHORT )
    {   return( SchemaField( name, DataType::SHORT, sizeof( int16 )));
    }
    else if ( getType() == DataType::SINGLE )
    {   return( SchemaField( name, DataType::SINGLE, sizeof( single )));
    }
    else if ( getType() == DataType::DOUBLE )
    {   return( SchemaField( name, DataType::DOUBLE, sizeof( double )));
    }
    else if ( getType() == DataType::TIMESTAMP )
    {   return( SchemaField( name, DataType::TIMESTAMP, sizeof( Timestamp )));
    }
    else if ( getType() == DataType::STRING )
    {   int32 string_length = getStringLength();

        if ( string_length < 0 )
        {   Throw( AuroraException,
                   "Unable to determine string length for output field" );
        }

        return( SchemaField( name, DataType::STRING, string_length ));
    }
    else
    {    Throw(AuroraException, "No field type for boolean expression");
    }
}

BOREALIS_NAMESPACE_END
