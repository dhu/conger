#include  "HAMethod.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
HAMethod string_to_method(string method) throw( AuroraBadEntityException )
{
    HAMethod ha_method;

    if ( method.empty() )
    {    ha_method = NO_HA;
    }
    else if ( method == "amnesia" )
    {    ha_method = AMNESIA;
    }
    else if ( method == "upstream" )
    {   ha_method = UPSTREAM_BACKUP;
    }
    else if ( method == "passive" )
    {   ha_method = PASSIVE_STANDBY;
    }
    else if ( method == "active" )
    {   ha_method = ACTIVE_STANDBY;
    }
    else
    {   DEBUG << "Unrecognize HA method:  " << method;

        Throw( AuroraBadEntityException,
               "Unrecognize HA method:  " + method );
    }

    return( ha_method );
}

CheckpointMethod string_to_checkpoint_method(string method) throw( AuroraBadEntityException )
{
    CheckpointMethod checkpoint_method;

    if ( method.empty() )
    {    checkpoint_method = WHOLE;
    }
    else if ( method == checkpoint_method_s[WHOLE] )
    {    checkpoint_method = WHOLE;
    }
    else if ( method == checkpoint_method_s[ROUND_ROBIN] )
    {   checkpoint_method = ROUND_ROBIN;
    }
    else if ( method == checkpoint_method_s[MIN_MAX] )
    {   checkpoint_method = MIN_MAX;
    }
    else
    {   DEBUG << "Unrecognize Checkpoint Method:  " << method;

        Throw( AuroraBadEntityException,
               "Unrecognize Checkpoint Method:  " + method );
    }

    return( checkpoint_method );
}

    
BOREALIS_NAMESPACE_END

//
////////////////////////////////////////////////////////////////////////////////
