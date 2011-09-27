#ifndef FIXLENTIMESERIES_H
#define FIXLENTIMESERIES_H

#include  "common.h"

// Changed by olga @ 25/04/2005 for rpc on statistics
//
//using namespace Borealis;

BOREALIS_NAMESPACE_BEGIN

/**
 * The FixLenTimeSeries class implements a Fixed Length TimeSeries.
 *
 * @author Ying Xing (yx@cs.brown.edu)
 * @version 2.0 05/10/2004
 */
class FixLenTimeSeries
{
public:
    /**
     * The exception type used for this class
     */
    typedef enum
    {
        OUT_OF_RANGE,
        EMPTY_TIME_SERIES
    } error;


public:
    /**
     * Default constructor.
     */
    FixLenTimeSeries();


    /**
     * Constructs a new FixLenTimeSeries with given number of
     * samples.  All samples are initialized to 0.
     * @param numberOfSamples the lengh of the time series.
     */
    FixLenTimeSeries( int32  number_of_samples ) throw( error );


    /**
     * Copy constructor
     * @param flts the FixLenTimeSeries to copy from
     */
    FixLenTimeSeries( const FixLenTimeSeries  &flts );


    /**
     * Assignment constructor
     * @param flts the FixLenTimeSeries to copy from
     */
    FixLenTimeSeries  &operator=( const FixLenTimeSeries  &flts );


    /**
     * Destructs a FixLenTimeSeries
     */
    ~FixLenTimeSeries();


    /**
     * Returns the complete vector of samples. (added by tatbul).
     */
    vector<double>  get_samples() const { return( _samples ); }


    /**
     * Returns the number of samples.
     */
    int32  get_number_of_samples() const { return( _number_of_samples ); }


    /**
     * Returns the ith sample.
     * The samples are indexed by 0 to the number of samples minus one.
     * The 0th sample is the latest sample.
     * By default, the latest sample is returned.
     * @param i the index of the sample
     * @throws Error  OUT_OF_RANGE error is thrown if index i is not correct.
     */
    double  get_sample( int32  i = 0 ) const throw( error );


    /**
     * Returns the handle of the ith sample.
     * By default, the latest sample is returned.
     * @param i the index of the sample
     * @throws Error  OUT_OF_RANGE error is thrown if index i is not correct.
     */
    double  &get_sample( int32  i = 0 ) throw( error );


    /**
     * Returns the average value form positon p_old to p_new.
     * If p_old = 0 and p_new = 0, then the average value of all samples
     * are returned.
     * Note that p_old and p_new are real numbers.
     * p_old is usually larger than p_new
     * @param p_old the index of the first sample in the range
     * @param p_new the index of the last sample in the range
     */
    double  get_average( double p_old = 0, double p_new = 0 ) const
                 throw( error );

    double get_variance() const throw(error); // -- tatbul

    double get_stddev() const throw(error); // -- tatbul


    /**
     * Adds a sample to this FixLenTimeSeries.
     * @param sample the sample to be added
     */
    void  add_sample( double  sample );


    /**
    * Create a copy of an given FixLenTimeSeries based on the given parameter.
    * @param old_ts  Time series to copy.
    * @param sample_period  Each sample in the new FixLenTimeSeries is
    *         the average value of the k samples in the old FixLenTimeSeries.
    * @param start_point Where to start the resample (from the position of the
    *           latest sample).
    */
    static void resample_fix_len_time_series(
           const FixLenTimeSeries  &old_ts,
           double  sample_period,
           double  start_point,
           FixLenTimeSeries& new_ts);


    /**
     * output the FixLenTimeSeries
     * @param os the output stream
     * @param flts the FixLenTimeSeries to be outputed
     */
//    friend  ostream  &operator<<( ostream  &os,const FixLenTimeSeries  &flts );

     /**
      * Represent the object as a string
      */
    //added by olga 25/04/05
    string as_string() const;

private:
    /**
     * The number of samples
     */
    int32   _number_of_samples;

    /**
     * The circular buffer that stores the samples
     */
    vector<double>  _samples;

    /**
     * The index of the first sample
     */
    int32   _last_sample_index;


    //added by olga @ 25/4/2005. Samples are missing from here. Need to put then in vector
    NMSTL_SIMPLY_SERIALIZABLE( FixLenTimeSeries, << _number_of_samples << _samples << _last_sample_index);

};

BOREALIS_NAMESPACE_END

#endif                           // FIXLENTIMESERIES_H
