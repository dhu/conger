#ifndef TSSTATS_H
#define TSSTATS_H

#include  <NMSTL/serial>
#include <NMSTL/text>
#include <NMSTL/util>

#include  "FixLenTimeSeries.h"
#include  "VersionRWLock.h"

BOREALIS_NAMESPACE_BEGIN

// changed by olga 25/04/05 for the rpc on statistics
//
/**
 * Currently, we use double to represent timestamp
 */
typedef double  TimeStamp;


/**
 * Currently, we use double to represent time and the 1 represent 1 second;
 */
typedef double  TimeSec;

/**
 * The ID type of the objects (boxes, strings, etc.)
 */
typedef string  idtype;

/**
 * The type for a vector of IDs
 */
typedef vector<idtype>  IDVect;


/**
 * This TSStats struct specifies the format of the statistics used in Borealis
 * One TSStats object contain all information of a certain kind of
 * statistics, e.g., stream rate.
 */
struct TSStats
{
public:
    typedef enum
    {
        ID_NOT_FOUND
    } error;

    typedef map<idtype, FixLenTimeSeries> IDTimeSeries; // added by olga

    /**
     * The time stamp of the last statistics sample
     */
    TimeStamp  time_stamp;

    /**
     * Each sample of the statistics is the average
     * value over certain period. This period is
     * called the precision of the samples in Borealis.
     */
    TimeSec  precision;

    /**
     * Each statistics is a timeSeries. The number of Samples in each
     * timeSeries is called the window size of the statistics
     */
    int32  window_size;

    /**
     * This lock is used to protect this data structur
     * If mutli-threads are used, then the writer thread should use
     * writeLock before updating this data structur and writeUnLock()
     * after updating this data structure
     * The reader should readVersion() first and use the returned version
     * to check whether the data has been updated.
     */

    VersionRWLock  vlock;

    /**
     * The data structure that stores all the FixLenTimeSeries
     */
    IDTimeSeries _tsstats;

    /**
     * Returns the precesion
     */
    TimeSec get_precision() const {return precision;}

    /**
     * Write lock the TSStats
     */
    void write_lock() { vlock.write_lock(); }

    /**
     * Write lock the TSStats
     */
    void write_un_lock() { vlock.write_un_lock(); }

    /**
     * Returns a TSStats that is resampled from the this TSStats
     *
     * If window_size/precision/time_stamp is zero, then the default value
     * is used.  Otherwise, the returned data are resamples according to the
     * given parameters.
     *
     * @param window_size the window size of the returned statistics
     * @param precision the precision of the returned statistics
     * @param time_stamp the time stamp of the latest sample
     */
    TSStats copy(
                int32  window_size = 0,
              TimeSec  precision = 0,
            TimeStamp  time_stamp = 0
                );

    /**
     * Returns a TSStats that is resampled from the this TSStats
     * Only the statistis data of the objects in the given list are
     * returned.
     *
     * If window_size/precision/time_stamp is zero, then the default value
     * is used.  Otherwise, the returned data are resamples according to the
     * given parameters.
     * @param ids the ids of the given objects
     * @param window_size the window size of the returned statistics
     * @param precision the precision of the returned statistics
     * @param time_stamp the time stamp of the latest sample
     */
    TSStats copy(
        IDVect ids,
        int32  window_size = 0,
        TimeSec precision = 0,
        TimeStamp time_stamp = 0
                );
    /**
     * Return the names of a the entries in this statistics set
     * alexr@18/05/2005
     * */
    IDVect* stat_names();

    /**
     * String representation: we need ot add the parameters. Now has only the map structure
     * olga@25/04/2005
     * */
    string as_string() const ;

private:

     //added by olga @ 25/4/2005
    NMSTL_SIMPLY_SERIALIZABLE (TSStats, << time_stamp << precision << window_size << _tsstats);

};

/**
 * Output the TSStats.
 * @param os the output stream
 * @param tss the TSStats to be outputed
 */
//ostream  &operator<<( ostream  &os, const  TSStats  &tss );

BOREALIS_NAMESPACE_END

#endif                       // TSSTATS_H
