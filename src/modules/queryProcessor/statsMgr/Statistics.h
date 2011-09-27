#ifndef  STATISTICS_H
#define  STATISTICS_H

#include <string>
#include <vector>
#include <iostream>
#include "TSStats.h"
#include <NMSTL/serial>
#include <NMSTL/util>
#include <NMSTL/text>

BOREALIS_NAMESPACE_BEGIN

// changed by olga @ 25/04/2005
//
typedef string  StatisticsName;


/**
 * This class implemtnes a container that contains all the statistics
 * for a Borealis node
 */
class Statistics
{
public:
    typedef enum
    {
        STATISTICS_NAME_NOT_FOUND,
    } error;

     typedef map<StatisticsName, TSStats> NameStatistics;

     NameStatistics _statistics; //added by olga

     bool empty() const {return _statistics.empty();}

    /**
     * Return all statistics with given name
     * @param name name of the statistics
     * @throws STATISTICS_NAME_NOT_FOUND if the given name is not valid
     */
    //const TSStats  &getTSStats( StatisticsName  name ) const
    //                       throw( Error );

    /**
     * Return all statistics with given name
     * @param name name of the statistics
     * @throws STATISTICS_NAME_NOT_FOUND if the given name is not valid
     */
    TSStats  &get_tsstats( StatisticsName  name );   // throw(Error);

    /**
     * checks if a given statistics name is defined.
     * @param name name of the statistics
     * returns true if statistics name exists.
     */
    bool has_tsstats( StatisticsName  name );


    /**
     * Return statistics with given name and ID
     * @param name name of the statistics
     * @param id id of the object
     * @throws STATISTICS_NAME_NOT_FOUND if the given name is not valid
     * @throws TSStats::ID_NOT_FOUND if the given id is not valid
     */
    const FixLenTimeSeries  &get_statistics( StatisticsName   name,
                                                    idtype   id
                                          ) const
                                     throw( error, TSStats::error );

    /**
     * Return statistics with given name and ID
     * @param name name of the statistics
     * @param id id of the object
     * @throws STATISTICS_NAME_NOT_FOUND if the given name is not valid
     * @throws TSStats::ID_NOT_FOUND if the given id is not valid
     */
    FixLenTimeSeries  &get_statistics( StatisticsName  name, idtype  id )
                               throw( error, TSStats::error );


    /**
     * Return the last sample for the specific statistics and object
     * olga
     */
    double &get_last_sample(StatisticsName  name, idtype  id )
                               throw( error, TSStats::error );

    /**
     * Returns a copy of the Stats.
     */
    Statistics  copy();

    /**
     * Copy TSStats.
     * If window_size/precision/time_stamp is zero, then the default value
     * is used.  Otherwise, the returned data are resamples according to the
     * given parameters.
     * @param name name of the statistics
     * @param window_size the window_size of the returned statistics
     * @param precision the precision of the returned statistics
     * @param time_stamp the time_stamp of the returned statistics
     * @throws STATISTICS_NAME_NOT_FOUND if the given name is not valid
     */
    TSStats  copy_tsstats( StatisticsName  name,
                                   int32  window_size = 0,
                                 TimeSec  precision = 0,
                               TimeStamp  time_stamp = 0
                        )
                   throw( error );

    /**
     * Copy TSStats with given object names.
     * Only the statistis data of the objects in the given list are
     * returned.
     *
     * If window_size/precision/time_stamp is zero, then the default value
     * is used.  Otherwise, the returned data are resamples according to the
     * given parameters.
     * @param name name of the statistics
     * @param ids the ids of the objects
     * @param window_size the window_size of the returned statistics
     * @param precision the precision of the returned statistics
     * @param time_stamp the time_stamp of the returned statistics
     * @throws STATISTICS_NAME_NOT_FOUND if the given name is not valid
     */
    TSStats  copy_tsstats( StatisticsName  name,
                                  IDVect  ids,
                                   int32  window_size = 0,
                                 TimeSec  precision = 0,
                               TimeStamp  time_stamp = 0
                        )
                   throw( error );


    /**
     * Output the names of all statistics
     * @param os the output stream
     */
    void output_statistics_names( ostream  &os );


    /**
     * Output the Stats
     * @param os the output stream
     * @param stats the statistics to be outputted
     */
   // friend ostream  &operator<< ( ostream  &os, const Statistics  &stats );


    /**
     * added by olga @ 25/4/2005
     * String representation
     * */
    string as_string() const;
    NMSTL_SIMPLY_SERIALIZABLE( Statistics, << _statistics);

};
//added by olga @ 25/4/2005
NMSTL_TO_STRING(Statistics);

BOREALIS_NAMESPACE_END
#endif                         // STATISTICS_H
