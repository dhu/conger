#include "FidComplianceLookthroughAndAgg.h"

BOREALIS_NAMESPACE_BEGIN


void FidComplianceLookthroughAndAgg::setup( const ExprContext  &ctxt )
                                     throw( AuroraException )
{
    if ( num_args() != 1 )
    {    Throw( aurora_typing_exception,
                "FidComplianceLookthroughAndAgg takes only one argument" );
    }

    if ( arg(0)->getType() != DataType::INT )
    {   Throw( aurora_typing_exception,
               "FidComplianceLookthroughAndAgg requires one integer argument" );
    }

    add_int_32output_field();
}

BOREALIS_NAMESPACE_END
