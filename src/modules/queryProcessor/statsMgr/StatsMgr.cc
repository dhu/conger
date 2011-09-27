/*
 * @(#)StatsMgr.cpp
 *
 * changed by olga @ 25/04/05. added namespace
 */

#include "StatsMgr.h"

BOREALIS_NAMESPACE_BEGIN

void StatsMgr::write_lock_statistics(StatisticsName name)
{
  read_lock();
  try
  {
    TSStats ts = _stats.get_tsstats(name);
    ts.write_lock();
  }
  catch(...)
  {
    read_unlock();
  }
}

void StatsMgr::write_un_lock_statistics(StatisticsName name)
{
    try
    {
        TSStats ts = _stats.get_tsstats(name);
        ts.write_un_lock();
        read_unlock();
    }
    catch(...)
    {
    }
}

/**
 * Copy the whole Stats structure.
 *
 * (locking/unlocking has been implemented inside the function)
 */
Statistics StatsMgr::copy_stats()
{
    read_lock();
    Statistics s = _stats.copy();
    read_unlock();
    return( s );
}


/**
 * Copy TSStats.
 *
 * If window_size/precision/time_stamp is zero, then the default value
 * is used.  Otherwise, the returned data are resamples according to the
 * given parameters.
 *
 * Statistics::STATISTICS_NAME_NOT_FOUND is thrown if the name is invalid.
 *
 * (locking/unlocking has been implemented inside the function)
 */
TSStats StatsMgr::copy_tsstats(
                      StatisticsName  name,
                               int32  window_size,
                             TimeSec  precision,
                           TimeStamp  time_stamp
                             )
                        throw( Statistics::error )
{
    try
    {
        read_lock();
        TSStats tss = _stats.copy_tsstats( name, window_size,
                                          precision, time_stamp );

        read_unlock();
        return( tss );
    }
    catch (...)
    {
        read_unlock();
        throw;
    }
}


/**
 * Copy TSStats with given object names.
 *
 * If window_size/precision/time_stamp is zero, then the default value
 * is used.  Otherwise, the returned data are resamples according to the
 * given parameters.
 *
 * Stats::STATISTICS_NAME_NOT_FOUND is thrown if the given name is not valid
 * TSStats::ID_NOT_FOUND is thrown if the given id is not valid
 *
 * (locking/unlocking has been implemented inside the function)
 */
TSStats StatsMgr::copy_tsstats(
                      StatisticsName  name,
                              IDVect  ids,
                               int32  window_size,
                             TimeSec  precision,
                           TimeStamp  time_stamp
                             )
                        throw( Statistics::error )
{
    try
    {
        read_lock();
        TSStats tss = _stats.copy_tsstats( name, ids, window_size,
                                          precision, time_stamp );

        read_unlock();
        return( tss );
    }
    catch (...)
    {
        read_unlock();
        throw;
    }
}


/**
 * Adds Empty TSStats with given name, precision, window_size,
 * and object IDs
 *
 * (locking/unlocking has been implemented inside the function)
 */
void StatsMgr::add_tsstats (
                   StatisticsName   name,
                            int32   window_size,
                          TimeSec   precision,
                     const IDVect  &ids
                          )
{
    write_lock();
    TSStats  &tss = _stats._statistics[ name ];

    tss.precision = precision;
    tss.window_size = window_size;
    tss.time_stamp = 0;

    for ( uint32 i = 0; i < ids.size(); ++i )
    {
        tss._tsstats[ ids[ i ]] = FixLenTimeSeries( window_size );
    }

    write_unlock();
}


bool StatsMgr::has_tsstats ( StatisticsName   name )
{
    //writeLock();
    if ( _stats._statistics.find( name ) != _stats._statistics.end() )
    {
    return true;
    }
    else
    {
    return false;
    }
//writeUnlock();
}


/**
 * Add an empty FixLenTimeSeries with given name and ID
 *
 * (locking/unlocking has been implemented inside the function)
 */
void StatsMgr::add_empty_statistics( StatisticsName name, idtype id )
{
    write_lock();

    try
    {
        TSStats  &tss = _stats.get_tsstats( name );
        tss._tsstats[ id ] = FixLenTimeSeries( tss.window_size );
    }
    catch (...)
    {
        cerr << "Statistics Name dose not exist: " << name << endl;

        write_unlock();
        exit( 1 );
    }

    write_unlock();
}


/**
 * Delete a FixLenTimeSeries with given name and ID
 *
 * (locking/unlocking has been implemented inside the function)
 */
void StatsMgr::delete_statistics( StatisticsName  name, idtype  id )
{
    try
    {
        write_lock();

        TSStats  &tss = _stats.get_tsstats( name );
        DEBUG << "deleting " << name << " " << id;
        tss._tsstats.erase( id );

        write_unlock();
    }
    catch (...)
    {
        write_unlock();
        cerr << "Statistics Name does not exist: " << name << endl;
    }
}

/**
 * Delete FixLenTimeSeries with given name and a vector of Ids
 *  olga
 * (locking/unlocking has been implemented inside the function)
 */
void StatsMgr::delete_statistics( StatisticsName  name, const IDVect &ids  )
{
    try
    {
        write_lock();
        TSStats  &tss = _stats.get_tsstats( name );

    for ( uint32 i = 0; i < ids.size(); ++i )
        {
            DEBUG << "deleting " << name << " " << ids[i];
            tss._tsstats.erase( ids[i] );
    }

        write_unlock();
    }
    catch (...)
    {
        write_unlock();
        cerr << "Statistics Name does not exist: " << name << endl;
    }
}



/**
 * Output the Statistics.
 */
/*ostream  &operator<< ( ostream  &os, StatsMgr  &stats_mgr )
{
    stats_mgr.readLock();
    os << stats_mgr.getStats();
    stats_mgr.readUnlock();

    return( os );
}*/

BOREALIS_NAMESPACE_END
