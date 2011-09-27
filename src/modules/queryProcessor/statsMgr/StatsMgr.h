#ifndef STATSMGR_H
#define STATSMGR_H

#include "Statistics.h"
#include "RWlock.h"

// changed by olga @ 25/04/05. Added namespace

BOREALIS_NAMESPACE_BEGIN

/**
 * The StatsMgr class implements a statistics manager.
 * StatsMgr contains a container that hold all the statistics and
 * provide RW lock to protest the data structure.
 */
class StatsMgr
{
public:
    /**
     * Copy the whole Stats structure.
     *
     * (locking/unlocking has been implemented inside the function)
     */
    Statistics copy_stats() ;

    /**
     * Copy TSStats according to the given parameters.
     *
     * If window_size/precision/time_stamp is zero, then the default value
     * is used.  Otherwise, the returned data are resamples according to the
     * given parameters.
     *
     * (locking/unlocking has been implemented inside the function)
     *
     * @param name the name of the statistics
     * @param window_size the window size of the returned statistics
     * @param precision the precision of the returned statistics
     * @param time_stamp the time stamp of the latest sample
     * @throws Statistics::STATISTICS_NAME_NOT_FOUND if the name is invalid.
     */
    TSStats copy_tsstats(
                StatisticsName  name,
                         int32  window_size = 0,
                       TimeSec  precision = 0,
                     TimeStamp  time_stamp = 0
                       )
                  throw( Statistics::error );

    /**
     * Copy TSStats with given object names.
     *
     * If window_size/precision/time_stamp is zero, then the default value
     * is used.  Otherwise, the returned data are resamples according to the
     * given parameters.
     *
     * (locking/unlocking has been implemented inside the function)
     *
     * @param name the name of the statistics
     * @param ids the ids of the given objects
     * @param window_size the window size of the returned statistics
     * @param precision the precision of the returned statistics
     * @param time_stamp the time stamp of the latest sample
     * @throws Statistics::STATISTICS_NAME_NOT_FOUND if the name is invalid.
     * @throws TSStats::ID_NOT_FOUND if the given id is not valid
     */
    TSStats copy_tsstats(
                StatisticsName  name,
                        IDVect  ids,
                         int32  window_size = 0,
                       TimeSec  precision = 0,
                     TimeStamp  time_stamp = 0
                       )
                  throw( Statistics::error );


    /**
     * Adds Empty TSStats with given name, precision, window_size,
     * and object IDs
     *
     * (locking/unlocking has been implemented inside the function)
     *
     * @param name the name of the statistics
     * @param window_size the window size of the statistics
     * @param precision the precision of the statistics
     * @param ids the ids of the given objects
     */
    void add_tsstats (
             StatisticsName   name,
                      int32   window_size,
                    TimeSec   precision,
               const IDVect  &ids
                    );

    bool has_tsstats ( StatisticsName   name );

    /**
     * Add an empty FixLenTimeSeries with given name and ID
     *
     * (locking/unlocking has been implemented inside the function)
     *
     * @param name the name of the statistics
     * @param id the id of the given object
     */
    void add_empty_statistics (
             StatisticsName  name,
                     idtype  id
                            );

    /**
     * Delete a FixLenTimeSeries with given name and ID
     *
     * (locking/unlocking has been implemented inside the function)
     *
     * @param name the name of the statistics
     * @param id the id of the given object
     */
    void delete_statistics (
             StatisticsName  name,
                     idtype  id
                          );
/**
     * Delete  FixLenTimeSeries with given name and a vector of Ids
     *
     * (locking/unlocking has been implemented inside the function)
     *
     * @param name the name of the statistics
     * @param id the vector of ids
     */
    void delete_statistics (
             StatisticsName  name,
                     const IDVect &ids);


    /**
     * Get a read lock of the statistics
     */
    void read_lock()    { _lock.readlock();    }

    /**
     * Release the read lock
     */
    void read_unlock()  { _lock.readunlock();  }

    /**
     * Get a write lock of the statistics
     */
    void write_lock()   { _lock.writelock();   }

    /**
     * Release the write lock
     */
    void write_unlock() { _lock.writeunlock(); }

    /**
     * Write lock a TSStats
     * Before any TSStats gets updated, it must be write locked first
     * A TSStats data structure must be read or write at the same time.
     * Do NOT readLock or writeLock before calling this
     */
    void write_lock_statistics(StatisticsName name);

    /**
     * Write unlock a TSStats
     */
    void write_un_lock_statistics(StatisticsName name);


    /**
     * Returns the Stats
     *
     * Must read lock before this functon and read unlock after this function
     */
    const Statistics  &get_stats() const {   return( _stats ); }


    /**
     * Returns the Stats
     *
     * Must read lock before this functon and read unlock after this function
     */
    Statistics  &get_stats()
    {   //DEBUG << "StatsMgr::get_stats running";
        return( _stats );
    }

    /**
     * Returns all statistics with a given name
     * @param name the name of the statistics to return
     * Added by Olga@ 04/22/2005
     */
    const TSStats &get_stats(StatisticsName name)
    {
        return _stats.get_tsstats(name);
    }


    /**
     * Return statistics with given name and ID
     * @param name name of the statistics
     * @param id id of the object
     * Added by Olga @ 04/22/2005
     */

    const FixLenTimeSeries & get_stats( StatisticsName name, idtype id)
    {
        return _stats.get_statistics(name,id);
    }

     /**
     * Return last statistics sample with given name and ID
     * @param name name of the statistics
     * @param id id of the object
     * Added by Olga @ 05/31/2005
     */

    const double & get_last_sample(StatisticsName name, idtype id)
    {
        return _stats.get_last_sample(name,id);
    }





private:
    /*
     * The data structure contains all statistics
     */
    Statistics   _stats;

    /*
     * The RWLock that used to protect _stats
     */
    RWLock  _lock;
};

/**
 * Output the StatsMgr.
 * @param os the output stream
 * @param stats_mgr the StatsMgr to be outputed
 */
//ostream  &operator<< ( ostream  &os, StatsMgr  &stats_mgr );

BOREALIS_NAMESPACE_END

#endif                          // STATSMGR_H
