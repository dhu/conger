/**
 * @(#)TSStats.cc
 *
 */

//changed by olga @25/04/2005  for rpc statistics

#include "TSStats.h"
#include <sstream>

BOREALIS_NAMESPACE_BEGIN

/**
 * Returns a TSStats that is resampled from the this TSStats
 *
 * If window_size/precision/time_stamp is zero, then the default value
 * is used.  Otherwise, the returned data are resamples according to the
 * given parameters.
 */
TSStats TSStats::copy(
                     int32  s_window_size,
                   TimeSec  s_precision,
                 TimeStamp  s_time_stamp
                     )
{
    while ( true )
    {   int32  v = vlock.read_version();

        TSStats tss_copy;

        if ( s_window_size == 0 && s_precision == 0 && s_time_stamp == 0 )
        {
            tss_copy = *this;
        }
        else
        {
            if (s_time_stamp == 0) s_time_stamp = time_stamp;
            if (s_precision == 0) s_precision = precision;
            if (s_window_size == 0) s_window_size = window_size;


            double start_point = (time_stamp - s_time_stamp) / precision;
            double sample_period = s_precision / precision;

            for ( IDTimeSeries::const_iterator sit = _tsstats.begin();
                  sit != _tsstats.end();
                  ++sit )
            {
              FixLenTimeSeries new_ts(s_window_size);
              FixLenTimeSeries::resample_fix_len_time_series(
                                          sit->second,
                                          sample_period,
                                          start_point,
                                          new_ts);
              tss_copy._tsstats[ sit->first ] = new_ts;
            }
        }

        if ( vlock.verify_version( v ))
        {
            return( tss_copy );
        }

        tss_copy._tsstats.clear();
    }
}


/**
 * Returns a TSStats that is resampled from the this TSStats
 * Only the statistis data of the objects in the given list are
 * returned.
 *
 * If window_size/precision/time_stamp is zero, then the default value
 * is used.  Otherwise, the returned data are resamples according to the
 * given parameters.
 */
TSStats TSStats::copy(
                     IDVect  ids,
                      int32  s_window_size,
                    TimeSec  s_precision,
                  TimeStamp  s_time_stamp )
{
    while ( true )
    {
        TSStats  tss_copy;
        int32    v;

        v = vlock.read_version();

        if (s_time_stamp == 0) s_time_stamp = time_stamp;
        if (s_precision == 0) s_precision = precision;
        if (s_window_size == 0) s_window_size = window_size;

        double start_point = (time_stamp - s_time_stamp) / precision;
        double sample_period = s_precision / precision;

        for ( uint32 i = 0; i < ids.size(); ++i )
        {
            IDTimeSeries::const_iterator sit = _tsstats.find( ids[ i ]);

            if ( sit != _tsstats.end() )
            {
                FixLenTimeSeries new_ts(s_window_size);
                FixLenTimeSeries::resample_fix_len_time_series(
                                          sit->second,
                                          sample_period,
                                          start_point,
                                          new_ts);

                tss_copy._tsstats[ ids[i] ] = new_ts;
            }
        }

        if ( vlock.verify_version( v ))
        {
            return( tss_copy );
        }

        tss_copy._tsstats.clear();
    }
}

/**
 * Return the names of a the entries in this statistics set
 * The list needs to be deleted after the caller is done with it
 * alexr@18/05/2005
 * */
IDVect* TSStats::stat_names()
{
    IDVect *names = new IDVect();

    for ( IDTimeSeries::const_iterator sit = _tsstats.begin();
          sit != _tsstats.end();
          ++sit )
        names->push_back( sit->first );

    return names;
}

/**
 * String representation: we need ot add the parameters. Now has only the map structure
 * olga@25/04/2005
 */
string TSStats::as_string() const
{

  ostringstream ts;
  ts << "WindowSize(" << window_size;
  ts << ") Precision(" << precision;
  ts << ") TimeStamp(" << time_stamp << ")\n";
  ts << endl;

  for ( IDTimeSeries::const_iterator sit = _tsstats.begin();
                  sit != _tsstats.end();
                  ++sit )
            {
            ts << "(" << sit->first << ")" ;
        ts << sit->second.as_string();
            }

  return ts.str();

}


/*ostream& operator<<( ostream& os, const TSStats& tss )
{
    os << "WindowSize(" << tss.window_size << ")";
    os << " Precision(" << tss.precision << ")";
    os << " TStamp(" << tss.time_stamp << ")" << endl;

    for ( TSStats::const_iterator sit = tss.begin();
        sit != tss.end();
        ++sit )
    {
        os << " " << sit->first << " " << sit->second << endl;
    }

    return( os );
}*/

BOREALIS_NAMESPACE_END
