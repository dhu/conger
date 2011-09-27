#include "TimeSeries.h"

#include <cmath>
#include <numeric>
#include <algorithm>
#include <functional>

namespace StatisticsTools
{

#ifndef HUGE
#define HUGE INT_MAX
#endif


double TimeSeries::get_latest_average(unsigned int k) const 
{
    // returns zero for empty TimeSeries
    if (k == 0)
        return 0;

    // If k is larger thant he current # of samples
    // just return the average of current samples 
    if (k > _samples.size())
        return get_average();

    // compute average
    return accumulate(_samples.begin(), _samples.begin() + k, 0.0) / k;
}

double TimeSeries::get_variance() const
{
    // returns zero for empty TimeSeries
    unsigned int n = _samples.size();
    if (n == 0) return 0;

    // compute variance using Var(X) = E(X^2) - [E(X)]^2 
    
    // compute E(X^2) * n
    double sum_squres =
        inner_product(_samples.begin(), _samples.end(), _samples.begin(), 0.0);

    // compute E(X) * n
    double sum = accumulate(_samples.begin(), _samples.end(), 0.0);

    // compute variance
    return max(0.0, sum_squres/n - sum*sum/n/n);
}


double TimeSeries::get_standard_deviation() const
{
    // returns zero for empty TimeSeries
    unsigned int n = _samples.size();
    if (n == 0) return 0;

    // compute variance using Var(X) = E(X^2) - [E(X)]^2 
    
    // compute E(X^2) * n
    double sum_squres =
        inner_product(_samples.begin(), _samples.end(), _samples.begin(), 0.0);

    // compute E(X) * n
    double sum = accumulate(_samples.begin(), _samples.end(), 0.0);

    // comput standard deviation using std(X) = sqrt(var(X))
    double std = sqrt(max(0.0, sum_squres/n - sum*sum/n/n));

    // use 0 for very small std. The small value might be rounding error.
    // This is useful for computing coefficient of variation
    if (std < 0.000001) return 0;

    return std;
}

double TimeSeries::get_coefficient_of_variation() const
{
    // returns zero for empty TimeSeries
    if (_samples.size() == 0) return 0;

    // returns zero if std is zero
    if (get_standard_deviation() == 0) return 0;

    // returns a large number if mean is zero
    if (get_average() == 0) return HUGE;

    // compute coefficient of variation from std / |mean|
    return get_standard_deviation() / fabs(get_average());
}

double TimeSeries::get_maximum() const
{
    // find maximum element
    deque<double>::const_iterator 
        it = max_element(_samples.begin(), _samples.end());
    // returns it if found
    if (it != _samples.end())
        return *it;

    // returns zero for empty TimeSeries
    return 0;
}

double TimeSeries::get_minimum() const
{
    // find minimum element
    deque<double>::const_iterator 
        it = min_element(_samples.begin(), _samples.end());

    // returns it if found
    if (it != _samples.end())
        return *it;

    // returns zero for empty TimeSeries
    return 0;
}


TimeSeries& TimeSeries::operator+=(const TimeSeries& s)
            throw(error)
{
    // returns the current TimeSeries if either TimeSeries is empty
    if (s.is_empty())
        return *this;
    if (is_empty())
    {
        _samples = s.get_samples();
        return *this;
    }

    // check whether the number of samples in the two TimeSeries are the same
    if (_samples.size() != s.get_number_of_samples())
        throw(TimeSeries::DIMENTION_MISMATCH);

    // add samples 
    transform(
            _samples.begin(), 
            _samples.end(), 
            s.get_samples().begin(),
            _samples.begin(),
            plus<double>());

    return *this;
}

TimeSeries& TimeSeries::operator-=(const TimeSeries& s)
            throw(error)
{
    // returns the current TimeSeries if either TimeSeries is empty
    if (s.is_empty())
        return *this;

    // If the given TimeSeries is empty, assum all its entries are zero
    if (is_empty())
    {
        for (unsigned int i = 0; i < s.get_number_of_samples(); ++i)
        {
            _samples.push_back(-s.get_samples()[i]);
        }
        return *this;
    }

    // check whether the number of samples in the two TimeSeries are the same
    unsigned int n = _samples.size();
    if (s.get_number_of_samples() != n)
        throw(TimeSeries::DIMENTION_MISMATCH);

    // substract samples 
    transform(
            _samples.begin(), 
            _samples.end(), 
            s.get_samples().begin(), 
            _samples.begin(),
            minus<double>());
    
    return *this;
}


ostream& operator<<(ostream& os, const TimeSeries& ts)
{
    os << "mean(" << ts.get_average() << ") " ;
    os << "std("  << ts.get_standard_deviation() << ") ";
    os << "CV("  << ts.get_coefficient_of_variation() << ") ";
    os << "{";
    for (unsigned int i = 0; i < ts._samples.size(); ++i)
    {
        os << ts._samples[i] << " ";
    }
    os << "}";
    return os;
}

TimeSeries dot_product(
                const TimeSeries& s1,
                const TimeSeries& s2)
            throw(TimeSeries::error)
{
    // check whether the number of samples of the two TimeSeries matches
    unsigned int n = s1.get_number_of_samples();
    if (s2.get_number_of_samples() != n)
    {
        throw(TimeSeries::DIMENTION_MISMATCH);
    }

    // perform .* (as in matlab)
    deque<double> samples(n);
    transform(
            s1.get_samples().begin(),
            s1.get_samples().end(),
            s2.get_samples().begin(),
            samples.begin(),
            multiplies<double>());

    return TimeSeries(samples);
}

double covariance(
                const TimeSeries& s1,
                const TimeSeries& s2)
            throw(TimeSeries::error)
{
    // check whether the number of samples of the two TimeSeries matches
    unsigned int n = s1.get_number_of_samples();
    if (s2.get_number_of_samples() != n)
    {
        throw(TimeSeries::DIMENTION_MISMATCH);
    }

    // returns zero for empty TimeSeries
    if (n == 0)
    {
        return 0;
    }

    // cov(X1, X2) = E(X1X2) - E(X1)E(X2)
    return
        inner_product(
                s1.get_samples().begin(), 
                s1.get_samples().end(), 
                s2.get_samples().begin(), 
                0.0) / 
        n - 
        s1.get_average() * s2.get_average();
}

double correlation(
                const TimeSeries& s1,
                const TimeSeries& s2)
            throw(TimeSeries::error)
{
    // check whether the number of samples of the two TimeSeries matches
    unsigned int n = s1.get_number_of_samples();
    if (s2.get_number_of_samples() != n)
    {
        throw(TimeSeries::DIMENTION_MISMATCH);
    }


    // returns zero if the variance of both TimeSeries are small
    // (The TimeSeries can be considered as constant valued)
    double std1 = s1.get_standard_deviation();
    double std2 = s2.get_standard_deviation();
    if (s1.get_coefficient_of_variation() < 0.001 ||
            s2.get_coefficient_of_variation() < 0.001)
        return 0;

    // cor(X1, X2) = cov(X1, X2) / (std(X1)*std(X2))
    return
        covariance(s1, s2) / std1 / std2;
}

TimeSeries operator+(const TimeSeries& s1, const TimeSeries& s2)
    throw(TimeSeries::error)
{
    try
    {
      TimeSeries s = s1;
      s += s2;
      return s ;
    }
    catch (...)
    {
        throw;
    }
}

TimeSeries operator-(const TimeSeries& s1, const TimeSeries& s2)
    throw(TimeSeries::error)
{
    try
    {
        TimeSeries s = s1;
        s -= s2;
        return s;
    }
    catch (...)
    {
        throw;
    }
}


TimeSeries operator+(const TimeSeries& s1, double a)
{
    TimeSeries s(s1); 
    s += a;
    return s;
}

TimeSeries operator+(double a, const TimeSeries& s1)
{
    TimeSeries s(s1); 
    s += a;
    return s;
}

TimeSeries operator-(const TimeSeries& s1, double a)
{
    TimeSeries s(s1); 
    s -= a;
    return s;
}

TimeSeries operator-(double a, const TimeSeries& s1)
{
    TimeSeries s(s1.get_number_of_samples(), a); 
    s -= s1;
    return s;
}

TimeSeries operator*(double a, const TimeSeries& s1)
{
    TimeSeries s(s1);
    s *= a;
    return s ;
}

TimeSeries operator*(const TimeSeries& s1, double a)
{
    TimeSeries s(s1);
    s *= a;
    return s ;
}

TimeSeries operator/(const TimeSeries& s1, double a)
{
    TimeSeries s(s1);
    s /= a;
    return s ;
}


}
