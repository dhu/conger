#include "FidAlarmCounterFunc.h"

BOREALIS_NAMESPACE_BEGIN



////////////////////////////////////////////////////////////////////////////////
//
ptr<Expression> FidAlarmCounterFunc::makeExpression( const NArgs  &args )
                                              throw( ExprException )
{
//..............................................................................


    if ( args.size() != 2 )
    {   Throw( aurora_typing_exception,
               "FidAlarmCounterFunc requires two arguments" );
    }

    if (( args[0]->getType() != DataType::INT )  ||
        ( args[1]->getType() != DataType::INT ))
    {
        Throw( aurora_typing_exception,
               "FidAlarmCounterFunc requires integer arguments" );
    }

    return( ptr<Expression>( new Impl( args[0], args[1] )));
}



////////////////////////////////////////////////////////////////////////////////
//
int32 FidAlarmCounterFunc::Impl::evalImpl( EvalAs<int32>,
                                           const EvalContext  &ctxt
                                         )
                                    throw( EvalException )
{
    // This second parameter tells me what the "threshold" is
    int32  threshold = _threshold->eval<int32>( ctxt );

    /*
     * 1 - REUTERS
     * 2 - NY
     * 3 - NA
     * 4 - COMSTOCK
     */

    int   alarm_level;
    bool  quenched = false;
    bool  is_first = false;
    void* addr = NULL;

    // Checkout the shared memory to see if we are in "quench" mode
    int shm_key = 313372; // 31337 was used by the previous fidelity demo
    int seg_id = shmget(shm_key, 4, IPC_CREAT | S_IRUSR | S_IWUSR);
//..............................................................................


    // One more alarm...
    ++_num_alarms;

    if ( seg_id == -1 )
    {   perror("FidAlarmCounterUDF shmget failed, cannot determine quenching status");
    }
    else
    {
        // Determine if this is the first access (if so, initialize to 0, no-quenching)
        struct shmid_ds buf;
        bzero(&buf, sizeof(buf));
        int ret = shmctl(seg_id,IPC_STAT,&buf);

        if ( ret != 0 )
        {   perror( "Problem with shmctl" );
        }

        if ( buf.shm_nattch == 0 )
        {   is_first = true;
        }

        addr = shmat( seg_id, NULL, 0 );

        if ( *(int*)addr == -1 )
        {   perror( "FidAlarmCounterUDF shmat failed, cannot determine quenching status" );
            addr = NULL;
        }
        else
        {   if ( is_first )  // First time, initialize to 0
            {
                int val = 0;
                memcpy( addr, &val, sizeof(int) );
            }
            else
            {
                // Not first, proceed to read status
                int val;
                val = (*((int*)addr));
                if (val == 1) quenched = true;
            }
        }
    }

    // When we become unquenched, reset _numAlarms.
    if (!quenched && _was_quenched)
    {
      //NOTICE << "Resetting alarm counter.";
      _num_alarms = 1;
      _was_quenched = false;
    }

    // Determine alarm level (lo, but every 'threshold' alarms output high alarm)

    if (_num_alarms % threshold != 0)  // lo alarm
    {    if (quenched)
         {
            _was_quenched = true;
            alarm_level = 0;
         }
         else
         {
            alarm_level = 1;
         }
    }
    else
    {
        // hi alarm, tell others that we are now in a high alarm
        // position (to quench any lo alarms anywhere)
        if (addr != NULL)
        {
            int val = 1;
            memcpy( addr,&val,sizeof(int) );
        }
        alarm_level = 2;

    }

    // 0 is a lo alarm, currently quenched
    // 1 is a lo alarm, not quenched
    // 2 is a hi alarm (always gets through)

    DEBUG << "Counter: called (quenched " << quenched
          << ", alarm level " << alarm_level << ", counter " << _num_alarms << ")";

    return( alarm_level );
}


BOREALIS_NAMESPACE_END
