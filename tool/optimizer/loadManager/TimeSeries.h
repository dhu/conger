#ifndef TimeSeries_H
#define TimeSeries_H

#include <deque>
#include <iostream>
#include <numeric>
#include <functional>

namespace StatisticsTools
{

using namespace std;

/**
 * The TimeSeries class implements TimeSeries.
 */
class TimeSeries
{
public:
    typedef enum
    {
        DIMENTION_MISMATCH
    }error;


public:
    /**
     * Constructs a empty TimeSeries.
     */
    TimeSeries() {}

    /**
     * Constructs a TimeSeries with given number of sample, and
     * all samples are initialized to zero.
     * @param number_of_samples the number of samples
     */
    explicit TimeSeries(unsigned int number_of_samples): 
        _samples(number_of_samples) {}

    /**
     * Constructs a TimeSeries with given number of sample, and
     * all samples are initialized to a given value
     * @param number_of_samples the number of samples
     * @param initial_value the number of samples
     */
    TimeSeries(unsigned int number_of_samples, double initial_value): 
        _samples(number_of_samples, initial_value) {}

    /**
     * Constructs a TimeSeries from a set of samples
     * @param samples the given samples
     */
    TimeSeries(const deque<double>& samples): _samples(samples) {}

    /**
     * Returns true if the number of samples is zero
     * @return true if the number of samples is zero
     */
    bool is_empty() const { return _samples.size() == 0; }

    /**
     * Returns the reference of the samples (The oldest sample is in the back)
     * @return the reference of the samples (The oldest sample is in the back)
     */
    const deque<double>& get_samples() const { return _samples; }

    /**
     * Returns the reference of the samples (The oldest sample is in the back)
     * @return the reference of the samples (The oldest sample is in the back)
     */
    deque<double>& get_samples() { return _samples; }

    /**
     * Returns a sample with a given index (0 is the index of the last sample)
     * @param index the index of the sample to be returned
     * @returns a sample with a given index (0 is the index of the last sample)
     */
    double get_sample(unsigned int index = 0) const { return _samples[index]; }

    /**
     * Return the latest sample. 
     * @return the latest sample
     */
    double get_last_sample() const { return _samples.front(); }

    /**
     * Returns the number of samples.
     * @return the number of samples.
     */
    unsigned int get_number_of_samples() const {return _samples.size(); }

    /**
     * Returns the average of samples.
     * @return the average of samples.
     */
    inline double get_average() const;

    /**
     * Returns the avearge of the latest k samples
     * @param k the number of latest samples to compute average on
     * @return the avearge of the latest k samples
     */
    double get_latest_average(unsigned int k) const; 

    /**
     * Returns the variance of samples.
     * @return the variance of samples.
     */
    double get_variance() const;

    /**
     * Returns the standard deviation of samples.
     * @return the standard deviation of samples.
     */
    double get_standard_deviation() const;

    /**
     * Returns the coefficient of variation of samples.
     * @return the coefficient of variation of samples.
     */
    double get_coefficient_of_variation() const;

    /**
     * Returns the maximum of samples.
     * @return the maximum of samples.
     */
    double get_maximum() const;

    /**
     * Returns the minimum of samples.
     * @return the minimum of samples.
     */
    double get_minimum() const;

    /**
     * Adds a sample to this TimeSeries.
     * @param sample the sample value to add to this TimeSeries.
     */
    void add_sample(double sample) { _samples.push_front(sample); }

    /**
     * Adds a sample to this TimeSeries and pop the oldest sample out
     * @param sample the sample value to be added to this TimeSeries.
     */
    void slide_sample(double sample)
    {
        _samples.push_front(sample); _samples.pop_back();
    }

    /**
     * Adds a sample to this TimeSeries at the other end
     * @param sample the sample value to be added to this TimeSeries.
     */
    void reverse_add_sample(double sample)
    {
        _samples.push_back(sample);
    }

    /**
     * Adds a sample to this TimeSeries at the other end and pop an sample out
     * @param sample the sample value to be added to this TimeSeries.
     */
    void reverse_slide_sample(double sample)
    {
        _samples.push_back(sample); _samples.pop_front();
    }

    /**
     * Clear all samples
     */
    void clear() {_samples.clear();}

    /**
     * Adds an TimeSeries to this TimeSeries
     * @param s the TimeSeries to be added
     * @return the reference of this TimeSeries
     * @thows DIMENTION_MISMATCH when the number of samples in the two
     *      TimeSeries mismatch 
     */
    TimeSeries& operator+=(const TimeSeries& s)
        throw(error);

    /**
     * Substracts an TimeSeries from this
     * @param s the TimeSeries to be substracted
     * @return the reference of this TimeSeries
     * @thows DIMENTION_MISMATCH when the number of samples in the two
     *      TimeSeries mismatch 
     */
    TimeSeries& operator-=(const TimeSeries& s)
        throw(error);

    /**
     * Add a constant to all samples 
     * @param a the constant to be added
     * @return the reference of this TimeSeries
     */
    TimeSeries& operator+=(double a) 
    {
        transform(_samples.begin(), _samples.end(), _samples.begin(), 
                bind2nd(plus<double>(), a)); 
        return *this;
    }

    /**
     * Substract a constant to all samples 
     * @param a the constant to be added
     * @return the reference of this TimeSeries
     */
    TimeSeries& operator-=(double a) 
    {
        transform(_samples.begin(), _samples.end(), _samples.begin(), 
                bind2nd(minus<double>(), a)); 
        return *this;
    }

    /**
     * multiply the samples by a constant
     * @param a the constant to be multiplied
     * @return the reference of this TimeSeries
     */
    TimeSeries& operator*=(double a) 
    {
        transform(_samples.begin(), _samples.end(), _samples.begin(), 
                bind2nd(multiplies<double>(), a)); 
        return *this;
    }

    /**
     * divide the samples by a constant
     * @param a the constant to be divided
     * @return the reference of this TimeSeries
     */
    TimeSeries& operator/=(double a) 
    {
        transform(_samples.begin(), _samples.end(), _samples.begin(), 
                bind2nd(divides<double>(), a)); 
        return *this;
    }

    /**
     * Output a TimeSeries and its statistics
     * @param os output stream
     * @param ts the TimeSeries to be output
     * @retun the reference of the output stream
     */
    friend ostream& operator<<(ostream& os, const TimeSeries& ts);

    /**
     * Returns a TimeSeries that is the dot product of the two given TimeSeries
     * @param s1 the first given TimeSeries
     * @param s2 the second given TimeSeries
     * @return the dot product of s1 and s2
     * @thows DIMENTION_MISMATCH when the number of samples in the two
     *      TimeSeries mismatch with each other
     */
    friend TimeSeries dot_product(
            const TimeSeries& s1,
            const TimeSeries& s2)
        throw(error);

    /**
     * Returns the covaricance of two TimeSeries
     * @param s1 the first given TimeSeries
     * @param s2 the second given TimeSeries
     * @return the covaricance of s1 and s2
     * @thows DIMENTION_MISMATCH when the number of samples in the two
     *      TimeSeries mismatch with each other
     */
    friend double covariance(
          const TimeSeries& s1,
          const TimeSeries& s2)
        throw(error);

    /**
     * Returns the correlation of two TimeSeries
     * @param s1 the first given TimeSeries
     * @param s2 the second given TimeSeries
     * @return the correlation of s1 and s2
     * @thows DIMENTION_MISMATCH when the number of samples in the two
     *      TimeSeries mismatch with each other
     */
    friend double correlation(
          const TimeSeries& s1,
          const TimeSeries& s2)
        throw(error);

protected:
    /**
     * A deque of samples.
     */
    deque<double> _samples;
};

/**
 * Returns the summation of two TimeSeries
 * @param s1 the first given TimeSeries
 * @param s2 the second given TimeSeries
 * @return the summation of s1 and s2
 * @thows DIMENTION_MISMATCH when the number of samples in the two
 *      TimeSeries mismatch with each other
 */
TimeSeries operator+(const TimeSeries& s1, const TimeSeries& s2)
    throw(TimeSeries::error);

/**
 * Returns the difference of two TimeSeries
 * @param s1 the first given TimeSeries
 * @param s2 the second given TimeSeries
 * @return the difference of s1 and s2
 * @thows DIMENTION_MISMATCH when the number of samples in the two
 *      TimeSeries mismatch with each other
 */
TimeSeries operator-(const TimeSeries& s1, const TimeSeries& s2)
    throw(TimeSeries::error);

/**
 * Returns the sum of a TimeSeries and a constant
 * @param s1 the given TimeSeries
 * @param a the given constant
 * @return the sum of a and s1
 */
TimeSeries operator+(const TimeSeries& s1, double a);

/**
 * Returns the sum of a TimeSeries and a constant
 * @param a the given constant
 * @param s1 the given TimeSeries
 * @return the sum of a and s1
 */
TimeSeries operator+(double a, const TimeSeries& s1);

/**
 * Returns the difference of a TimeSeries and a constant
 * @param s1 the given TimeSeries
 * @param a the given constant
 * @return the difference of a and s1
 */
TimeSeries operator-(const TimeSeries& s1, double a);

/**
 * Returns the difference of a TimeSeries and a constant
 * @param a the given constant
 * @param s1 the given TimeSeries
 * @return the difference of a and s1
 */
TimeSeries operator-(double a, const TimeSeries& s1);

/**
 * Returns the product of a TimeSeries and a constant
 * @param s1 the given TimeSeries
 * @param a the given constant
 * @return the product of a and s1
 */
TimeSeries operator*(double a, const TimeSeries& s1);

/**
 * Returns the product of a TimeSeries and a constant
 * @param a the given constant
 * @param s1 the given TimeSeries
 * @return the product of a and s1
 */
TimeSeries operator*(const TimeSeries& s1, double a);

/**
 * Returns the quotient of a TimeSeries and a constant
 * @param s1 the given TimeSeries
 * @param a the given constant
 * @return the quotient of s1 divided by a 
 */
TimeSeries operator/(const TimeSeries& s1, double a);


inline double TimeSeries::get_average() const
{
    if (_samples.size() == 0) return 0;
    else
        return accumulate(_samples.begin(), _samples.end(), 0.0) /_samples.size();
}

}

#endif

