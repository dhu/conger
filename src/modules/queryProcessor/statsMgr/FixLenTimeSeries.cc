/*
 * @(#)FixLenTimeSeries.cpp
 *
 * @author Ying Xing (yx@cs.brown.edu)
 * @version 2.0 04/07/2004
 *
 * Changed by Olga @25/04/05
 */

#include  <numeric>
#include  <cmath>
#include  <vector>
#include  <sstream>

#include  "FixLenTimeSeries.h"

BOREALIS_NAMESPACE_BEGIN

#ifndef HUGE
#define HUGE INT_MAX
#endif

// Default constructor
//
FixLenTimeSeries::FixLenTimeSeries():
    _number_of_samples( 0 ),
//    _samples( 0 ),
    _last_sample_index( 0 )
{}


// Constructs a new FixLenTimeSeries with given number of
// samples.  All samples are initialized to 0.
//
FixLenTimeSeries::FixLenTimeSeries( int32  number_of_samples )
                             throw( FixLenTimeSeries::error ):
    _number_of_samples( number_of_samples ),
    _last_sample_index( number_of_samples - 1 )
{
    if ( number_of_samples <= 0 )
    {
        throw  EMPTY_TIME_SERIES;
    }

    _samples= vector<double>(_number_of_samples);

    for ( int32  i = 0; i < _number_of_samples; ++i )
    {
          _samples.at(i)=0;
    }
}


// Copy constructor
//
FixLenTimeSeries::FixLenTimeSeries( const FixLenTimeSeries  &flts ):
    _number_of_samples( flts._number_of_samples ),
    _last_sample_index( flts._last_sample_index )
{
    _samples= vector<double>(_number_of_samples);


    for ( int32 i = 0; i < _number_of_samples; ++i )
    {
        //_samples[ i ] = flts._samples[ i ]; // olga@ 25/04 - temporary until we change samples to vector
         _samples.at(i)=flts._samples.at(i);
    }
}


// Assignment constructor
//
FixLenTimeSeries  &FixLenTimeSeries::operator=( const FixLenTimeSeries  &flts )
{
    if ( this == &flts )
    {
        return( *this );
    }

    _number_of_samples = flts._number_of_samples;
    _last_sample_index = flts._last_sample_index;

        _samples.clear();
        _samples = vector<double>(_number_of_samples);

    for ( int32 i = 0; i < _number_of_samples; ++i )
    {

         _samples.at(i)=flts._samples.at(i);
    }

    return( *this );
}


// Destructs a FixLenTimeSeries
//
FixLenTimeSeries::~FixLenTimeSeries()
{
    _samples.clear();
}


/*
 * Returns the ith sample.
 * The samples are indexed by 0 to the number of samples minus one.
 * The 0th sample is the latest sample.
 * By default, the latest sample is returned.
 * OUT_OF_RANGE error is thrown if index i is not correct.
 */
double  FixLenTimeSeries::get_sample( int32  i ) const
                              throw( FixLenTimeSeries::error )
{
    if ( i >= _number_of_samples || i < 0 )
    {
        throw  OUT_OF_RANGE;
    }

    i += _last_sample_index;

    if ( i >= _number_of_samples )
    {
        i -= _number_of_samples;
    }

    return (_samples.at(i));
}


/*
 * Returns the handle of the ith sample.
 * By default, the latest sample is returned.
 * OUT_OF_RANGE error is thrown if index i is not correct.
 */
double  &FixLenTimeSeries::get_sample( int32 i )
                               throw( FixLenTimeSeries::error )
{
    if ( i >= _number_of_samples || i < 0 )
    {
        throw OUT_OF_RANGE;
    }

    i += _last_sample_index;

    if ( i >= _number_of_samples )
    {
        i -= _number_of_samples;
    }

    return(_samples.at(i));
}


/*
 * Returns the average value form positon p_old to p_new.
 * If p_old = 0 and p_new = 0, then the average value of all samples
 * are returned.
 * Note that p_old and p_new are real numbers.
 * p_old is usually larger than p_new
 */
double FixLenTimeSeries::get_average( double  p_old,
                                     double  p_new
                                   ) const
                              throw( error )
{
    double  avg = 0;

    if ( p_old == 0  &&  p_new == 0 )
    {
        for ( int32  i = 0; i < _number_of_samples; ++i )
        {
         avg+=_samples.at(i);
        }

        avg /= _number_of_samples;

        return( avg );
    }

    if ( p_old <= p_new )
    {
        throw OUT_OF_RANGE;
    }

    if ( p_new < 0 || p_old > _number_of_samples )
    {
        throw OUT_OF_RANGE;
    }

    int32 i = (int32)floor( p_new );

    if ( p_old <= i + 1 )
    {
        return( get_sample( i ));
    }

    avg += ( i + 1 - p_new ) * get_sample( i );

    ++i;

    while ( p_old > i + 1 )
    {
        avg += get_sample( i );
        ++i;
    }

    avg += ( p_old - i ) * get_sample( i );

    avg /= ( p_old - p_new );
    return( avg );
}

double FixLenTimeSeries::get_variance() const throw(error)
{
    double var = 0;

    double mean = get_average();

    for (int32 i = 0; i < _number_of_samples; i++)
    {
        var += (_samples.at(i) - mean)*(_samples.at(i) - mean);
    }

    var /= _number_of_samples;

    return var;
}

double FixLenTimeSeries::get_stddev() const throw(error)
{
    double var = get_variance();

    return (sqrt(var));
}

// Adds a sample to this FixLenTimeSeries.
//
void FixLenTimeSeries::add_sample( double  sample )
{
    --_last_sample_index;

    if ( _last_sample_index < 0 )
    {
        _last_sample_index += _number_of_samples;
    }

    _samples.at(_last_sample_index)=sample;
}


/**
* Create a copy of an given FixLenTimeSeries based on the given parameter.
* @param n the number of samples in the new time series
* @param k each sample in the new FixLenTimeSeries is the average value
*          of the k samples in the old FixLenTimeSeries
* @param ip where to start the resample (from the position of the
* latest sample)
*/
void FixLenTimeSeries::resample_fix_len_time_series(
    const FixLenTimeSeries  &old_ts,
    double   sample_period,
    double   start_point,
    FixLenTimeSeries& new_ts)
{
    unsigned int n = old_ts.get_number_of_samples();
    if (n == 0)
    {
        return;
    }

    // comput the samples from the last one to the earliest one
    // put the results in a vector
    vector<double> new_samples;
    double sp = start_point;
    double ep = start_point + sample_period;
    for (int i = 0; i < new_ts.get_number_of_samples(); ++i)
    {
        // compute the value of the number ith sample
        if (ep <= 0)
        {
             new_samples.push_back(old_ts.get_sample(0));
        }
        else if (sp >= n)
        {
             new_samples.push_back(old_ts.get_sample(n - 1));
        }
        else
        {
            sp = max(0.0, sp);
            ep = min((double)n, ep);
            double interval = ep - sp;
            double sum = 0;

            int min_index = (int)(floor(sp));
            int max_index = min((int)(n - 1), (int)(floor(ep)));
            for (int i = min_index; i <= max_index; ++i)
            {
                double support = min(i + 1.0, ep) - sp;
                sum += old_ts.get_sample(i) * support;
                sp = min(i + 1.0, ep);
            }
            new_samples.push_back(sum / interval);
        }
        sp = ep;
        ep += sample_period;

    }

    // add the new_samples to the new_ts
    for (int i = new_samples.size() - 1; i >= 0; --i)
    {
        new_ts.add_sample(new_samples[i]);
    }
}

/**
 * Represents the object as string
 * olga @25/04/05. Does not include the parameters but only the samples
 */

string FixLenTimeSeries::as_string() const
{

  ostringstream ts;
  ts << "(Last Sample Index= " << _last_sample_index << ")";

  ts << "{";

   int32  k = _last_sample_index;

    for ( int32 i = 0; i < _number_of_samples; ++i )
    {
      ts << _samples.at(k);

        if ( i < _number_of_samples - 1 )
        {
            ts << string(" ");
        }

        if ( ++k >= _number_of_samples )
        {
            k -= _number_of_samples;
        }
    }
    ts << "}\n";
  return ts.str();

}

BOREALIS_NAMESPACE_END

