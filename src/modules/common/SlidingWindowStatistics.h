#ifndef SLIDING_WINDOW_STAT_H
#define SLIDING_WINDOW_STAT_H

#include <sstream>
#include <cmath>
#include "common.h"

BOREALIS_NAMESPACE_BEGIN

class SlidingWindowStatistics {

  // Attribute(s)
  protected:

    /**
     * The number of bins.
     */
    unsigned int _numberOfBins;

    /**
     * The number of samples.
     */
    unsigned int _sampleSize;

    /**
     * The sum of samples.
     */
    double _sum;

    /**
     * The sum of squares of samples.
     */
    double _sumSqures;

    /**
     * The maximum.
     */
    double _maximum;

    /**
     * The minimum.
     */
    double _minimum;

    /**
     * A vector of sample sizes.
     */
    vector<int> _sampleSizes;

    /**
     * A vector of sums.
     */
    vector<double> _sums;

    /**
     * A vector of sums of squres.
     */
    vector<double> _sumsSqures;

    /**
     * A vector of maximums.
     */
    vector<double> _maximums;

    /**
     * A vector of minimums.
     */
    vector<double> _minimums;


   // Method(s)
  public:

    SlidingWindowStatistics();

    /**
     * Constructs a new SlidingWindowStatistics.
     * @param numberOfBins the number of bins.
     */
    SlidingWindowStatistics(int numberOfBins);

    SlidingWindowStatistics(SlidingWindowStatistics& other);

    void set(SlidingWindowStatistics& other);

    /**
     * Destroys this SlidingWindowStatistics.
     */
    virtual ~SlidingWindowStatistics();

    /**
     * Returns the number of bins.
     * @return the number of bins.
     */
    inline int getNumberOfBins() {return _sampleSizes.size();};

    /**
     * Returns the number of samples.
     * @return the number of samples.
     */
    inline int getNumberOfSamples() {return _sampleSize;};

    /**
     * Returns the number of samples in the specified bin.
     * @param binIndex the index of the bin to get the number of samples.
     * @return the number of samples in the specified bin.
     */
    inline int getNumberOfSamples(int binIndex) {return _sampleSizes[binIndex];};

    /**
     * Returns the number of samples in the current bin.
     * @return the number of samples in the current bin.
     */
    inline double getCurrentNumberOfSamples() {return getNumberOfSamples(_sampleSizes.size()-1);};

    /**
     * Returns the number of samples in the previous bin.
     * @return the number of samples in the previous bin.
     */
    inline double getPreviousNumberOfSamples() {return getNumberOfSamples(_sampleSizes.size()-2);};

    /**
     * Returns the sum of samples.
     * @return the sum of samples.
     */
    inline double getSum() {return _sum;};

    /**
     * Returns the sum of samples in the specified bin.
     * @param binIndex the index of the bin to get the sum of samples.
     * @return the sum of samples in the specified bin.
     */
    inline double getSum(int binIndex) {return _sums[binIndex];};

    /**
     * Returns the sum of samples in the current bin.
     * @return the sum of samples in the current bin.
     */
    inline double getCurrentSum() {return getSum(_sampleSizes.size()-1);};

    /**
     * Returns the sum of samples in the previous bin.
     * @return the sum of samples in the previous bin.
     */
    inline double getPreviousSum() {return getSum(_sampleSizes.size()-2);};

    /**
     * Returns the average of samples.
     * @return the average of samples.
     */
    inline double getAverage() {if (_sampleSize == 0) return 0; else return _sum/_sampleSize;};

    /**
     * Returns the average of samples in the specified bin.
     * @param binIndex the index of the bin to get the average of samples.
     * @return the average of samples in the specified bin.
     */
    inline double getAverage(int binIndex) {if (_sampleSizes[binIndex] == 0) return 0; else return _sums[binIndex]/_sampleSizes[binIndex];};

    /**
     * Returns the average of samples in the current bin.
     * @return the average of samples in the current bin.
     */
    inline double getCurrentAverage() {return getAverage(_sampleSizes.size()-1);};

    /**
     * Returns the average of samples in the previous bin.
     * @return the average of samples in the previous bin.
     */
    inline double getPreviousAverage() {return getAverage(_sampleSizes.size()-2);};

    /**
     * Returns the standard deviation of samples.
     * @return the standard deviation of samples.
     */
    inline double getStandardDeviation() {if (_sampleSize == 0) return 0; else return sqrt(_sumSqures/_sampleSize - _sum*_sum/_sampleSize/_sampleSize);};

    /**
     * Returns the standard deviation of samples in the specified bin.
     * @param binIndex the index of the bin to get the standard deviation of samples.
     * @return the standard deviation of samples in the specified bin.
     */
    inline double getStandardDeviation(int binIndex) {if (_sampleSizes[binIndex] == 0) return 0; else return sqrt(_sumsSqures[binIndex]/_sampleSizes[binIndex] - _sums[binIndex]*_sums[binIndex]/_sampleSizes[binIndex]/_sampleSizes[binIndex]);};

    /**
     * Returns the standard deviation of samples in the current bin.
     * @return the standard deviation of samples in the current bin.
     */
    inline double getCurrentStandardDeviation() {return getStandardDeviation(_sampleSizes.size()-1);};

    /**
     * Returns the standard deviation of samples in the previous bin.
     * @return the standard deviation of samples in the previous bin.
     */
    inline double getPreviousStandardDeviation() {return getStandardDeviation(_sampleSizes.size()-2);};

    /**
     * Returns the maximum of samples.
     * @return the maximum of samples.
     */
    inline double getMaximum() {return _maximum;};

    /**
     * Returns the maximum of samples in the specified bin.
     * @param binIndex the index of the bin to get the maximum of samples.
     * @return the maximum of samples in the specified bin.
     */
    inline double getMaximum(int binIndex) {return _maximums[binIndex];};

    /**
     * Returns the maximum of samples in the current bin.
     * @return the maximum of samples in the current bin.
     */
    inline double getCurrentMaximum() {return getMaximum(_sampleSizes.size()-1);};

    /**
     * Returns the maximum of samples in the previous bin.
     * @return the maximum of samples in the previous bin.
     */
    inline double getPreviousMaximum() {return getMaximum(_sampleSizes.size()-2);};

    /**
     * Returns the minimum of samples.
     * @return the minimum of samples.
     */
    inline double getMinimum() {return _minimum;};

    /**
     * Returns the minimum of samples in the specified bin.
     * @param binIndex the index of the bin to get the minimum of samples.
     * @return the minimum of samples in the specified bin.
     */
    inline double getMinimum(int binIndex) {return _minimums[binIndex];};

    /**
     * Returns the minimum of samples in the current bin.
     * @return the minimum of samples in the current bin.
     */
    inline double getCurrentMinimum() {return getMinimum(_sampleSizes.size()-1);};

    /**
     * Returns the minimum of samples in the previous bin.
     * @return the minimum of samples in the previous bin.
     */
    inline double getPreviousMinimum() {return getMinimum(_sampleSizes.size()-2);};

    /**
     * Slides the window of this SlidingWindowStatistics.
     */
    void slide();

    /**
     * Adds a sample to this SlidingWindowStatistics.
     * @param sample the sample value to add to this SlidingWindowStatistics.
     */
    void addSample(double sample);

    /**
     * Adds samples to this SlidingWindowStatistics.
     * @param numberOfSamples the number of samples.
     * @param average the sum of samples.
     * @param standardDeviation the standard deviation of samples.
     * @param maximum the maximum of samples.
     * @param minimum the maximum of samples.
     */
    void addSamples(int numberOfSamples, double average, double standardDeviation, double maximum, double minimum);

    string as_string() const;

    NMSTL_SIMPLY_SERIALIZABLE(SlidingWindowStatistics, << _numberOfBins << _sampleSize 
            << _sum << _sumSqures << _maximum << _minimum 
            << _sampleSizes 
            << _sums << _sumsSqures << _maximums << _minimums);

};

BOREALIS_NAMESPACE_END

#endif
