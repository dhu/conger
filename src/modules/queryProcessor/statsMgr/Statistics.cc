/**
 * @(#)Stats.cpp
 *
 * Changed by olga @ 25/04/05: 1)included the statistics in a serializable field.
 *                             2) Added borealis namespace
 *                             3)added as_string(), removed << operator
 *
 */

#include "Statistics.h"
#include <cmath>

BOREALIS_NAMESPACE_BEGIN


/**
 * Return all statistics with given name
 *
 * STATISTICS_NAME_NOT_FOUND is thrown if the given name is not valid
 */
/****************************************
const TSStats  &Statistics::getTSStats( StatisticsName  name ) const
                                 throw( Statistics::Error )

{
    Statistics::const_iterator  it = find( name );

    if ( it != end() )
    {
    return( it->second );
    }
    else
    {
    throw  STATISTICS_NAME_NOT_FOUND;
    }
}
*****************************************/


/**
 * Return all statistics with given name
 *
 * STATISTICS_NAME_NOT_FOUND is thrown if the given name is not valid
 */
TSStats  &Statistics::get_tsstats( StatisticsName name )
                         //throw( Statistics::Error )
{
    NameStatistics::iterator it = _statistics.find( name );

    if ( it != _statistics.end() )
    {
        return( it->second );
    }
    else
    {
        throw STATISTICS_NAME_NOT_FOUND;
    }
}

/**
 * Return true if statistics exists. throwing exceptions is useless
 * if not in main thread...
 */
bool Statistics::has_tsstats( StatisticsName  name )
{
    NameStatistics::iterator it = _statistics.find( name );

    if ( it != _statistics.end() )
        return true;

    return false;
}

/**
 * Return statistics with given name and ID
 *
 * STATISTICS_NAME_NOT_FOUND is thrown if the given name is not valid
 * TSStats::ID_NOT_FOUND is thrown if the given id is not valid
 */
const FixLenTimeSeries  &Statistics::get_statistics(
                                        StatisticsName  name,
                                                idtype  id
                                                  ) const
         throw( Statistics::error, TSStats::error )
{
    DEBUG << "Statistics::get_statistics begin";
    NameStatistics::const_iterator  it = _statistics.find( name );

    if ( it != _statistics.end() )
    {
        TSStats::IDTimeSeries::const_iterator sit = it->second._tsstats.find(id);

        if ( sit != it->second._tsstats.end() )
        {
            DEBUG << "Statistics::get_statistics returning";
            return( sit->second );
        }
        else
        {
            DEBUG << "Statistics::get_statistics crashing";
            throw  TSStats::ID_NOT_FOUND;
        }
    }
    else
    {
        DEBUG << "Statistics::get_statistics crashing";
        throw  STATISTICS_NAME_NOT_FOUND;
    }
}


/**
 * Return statistics with given name and ID
 *
 * STATISTICS_NAME_NOT_FOUND is thrown if the given name is not valid
 * TSStats::ID_NOT_FOUND is thrown if the given id is not valid
 */
FixLenTimeSeries  &Statistics::get_statistics(
                                  StatisticsName  name,
                                          idtype  id
                                            )
   throw( Statistics::error, TSStats::error )
{
    //DEBUG << "Statistics::get_statistics begin";
    //DEBUG << "name = " << name;
    //DEBUG << "  id = " << id;
    NameStatistics::iterator it = _statistics.find( name );
    //DEBUG << "beep";
    if ( it != _statistics.end() )
    {
        TSStats::IDTimeSeries::iterator sit = it->second._tsstats.find( id );
        //DEBUG << "beep";
        // XXX: The problem is that id is not found.
        if ( sit != it->second._tsstats.end() )
        {
            //DEBUG << "beep";
            return( sit->second );
        }
        else
        {
            DEBUG << "THROWING TSStats::ID_NOT_FOUND!";
            DEBUG << "name = " << name;
            DEBUG << "  id = " << id;
            for( NameStatistics::iterator i = _statistics.begin(); i != _statistics.end(); ++i )
            {
                DEBUG << "Found TSStats for: " << i->first;
                for( TSStats::IDTimeSeries::iterator j = i->second._tsstats.begin(); j != i->second._tsstats.end(); ++j )
                {
                    DEBUG << "    Found FixLenTimeSeries for: " << j->first;
                }
            }
            //DEBUG << "Crashing for a backtrace ...";
            //assert(0);
            throw TSStats::ID_NOT_FOUND;
        }
    }
    else
    {
        DEBUG << "beep";
        throw STATISTICS_NAME_NOT_FOUND;
    }
}

/**
 * Return last sample with given name and ID
 * olga
 */
double  &Statistics::get_last_sample(
                                  StatisticsName  name,
                                          idtype  id
                                            )
   throw( Statistics::error, TSStats::error )
{
    NameStatistics::iterator it = _statistics.find( name );

    if ( it != _statistics.end() )
    {
        TSStats::IDTimeSeries::iterator sit = it->second._tsstats.find( id );

        if ( sit != it->second._tsstats.end() )
        {
            return( sit->second.get_sample(0) ); // get the last sample
        }
        else
        {
            throw TSStats::ID_NOT_FOUND;
        }
    }
    else
    {
        throw STATISTICS_NAME_NOT_FOUND;
    }
}


/**
 * Returns a copy of the Stats.
 */
Statistics  Statistics::copy()
{
    Statistics stats_copy;

    for ( NameStatistics::iterator sit = _statistics.begin();
          sit != _statistics.end();
          ++sit )
    {
        stats_copy._statistics.insert( _statistics.end(),
            NameStatistics::value_type( sit->first, sit->second.copy() ));
    }

    return( stats_copy );
}


/**
 * Copy TSStats.
 *
 * If window_size/precision/time_stamp is zero, then the default value
 * is used.  Otherwise, the returned data are resamples according to the
 * given parameters.
 *
 * STATISTICS_NAME_NOT_FOUND is thrown if the given name is not valid
 */
TSStats  Statistics::copy_tsstats(
                         StatisticsName  name,
                                  int32  window_size,
                                TimeSec  precision,
                              TimeStamp  time_stamp
                                )
       throw( Statistics::error )
{
    try
    {
        TSStats& tss = get_tsstats(name);
        return( tss.copy( window_size, precision, time_stamp ));
    }
    catch (...)
    {
        throw;
    }
}


/**
 * Copy TSStats with given object names.
 * Only the statistis data of the objects in the given list are
 * returned.
 *
 * If window_size/precision/time_stamp is zero, then the default value
 * is used.  Otherwise, the returned data are resamples according to the
 * given parameters.
 *
 * Stats::STATISTICS_NAME_NOT_FOUND is thrown if the given name is not valid
 */
TSStats Statistics::copy_tsstats(
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
        TSStats  &tss = get_tsstats( name );
        return( tss.copy( ids, window_size, precision, time_stamp ));
    }
    catch (...)
    {
        throw;
    }
}


/**
 * Output the names of all statistics
 */
void Statistics::output_statistics_names( ostream  &os )
{
    for ( NameStatistics::iterator it = _statistics.begin();
          it != _statistics.end();
          ++it )
    {
        os << it->first << endl;
    }
}
/**
 * added by olga @ 25/4/2005
 * String representation
 */
string Statistics::as_string() const
{

  string stats="\n";

  stats += "====================== Statistics ==================\n";
  for ( NameStatistics::const_iterator it = _statistics.begin();
          it != _statistics.end();
          ++it )
  {
    stats += "--------------- " + it->first +" ----------------\n";
    stats += it->second.as_string();
    stats += "\n";
   }

  stats += "====================================================\n";
  return (stats);

}
/**
 * Output the Stats
 */
/*ostream  &operator<< ( ostream  &os, const Statistics  &stats )
{
    os << "Have " << stats.size() << " statistics to print " << endl;
    os << "==========================Stats==============================="
       << endl;

    for ( Statistics::const_iterator nit = stats.begin();
          nit != stats.end();
          ++nit )
    {
        os << "-----------" << nit->first << "-----------" << endl;
        os << nit->second;
    }

    os << "=============================================================="
       << endl;

    return( os );
}*/

BOREALIS_NAMESPACE_END
