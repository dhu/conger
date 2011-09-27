#include "SlidingWindowStatistics.h"

MEDUSA_NAMESPACE_BEGIN

SlidingWindowStatistics::SlidingWindowStatistics(): 
_numberOfBins(10),
_sampleSize(0),
_sum(0),
_sumSqures(0),
_maximum(0),
_minimum(0) {
  _sampleSizes.push_back(0);
  _sums.push_back(0);
  _sumsSqures.push_back(0);
  _maximums.push_back(0);
  _minimums.push_back(0);
}

SlidingWindowStatistics::SlidingWindowStatistics(int numberOfBins): 
_numberOfBins(numberOfBins),
_sampleSize(0),
_sum(0),
_sumSqures(0),
_maximum(0),
_minimum(0) {
  _sampleSizes.push_back(0);
  _sums.push_back(0);
  _sumsSqures.push_back(0);
  _maximums.push_back(0);
  _minimums.push_back(0);
}

SlidingWindowStatistics::~SlidingWindowStatistics() {
}

void SlidingWindowStatistics::slide() {
  _sampleSizes.push_back(0);
  _sums.push_back(0);
  _sumsSqures.push_back(0);
  _maximums.push_back(0);
  _minimums.push_back(0);

  if (_sampleSizes.size() > _numberOfBins) {
    _sampleSize -= _sampleSizes[0];
    _sum -= _sums[0];
    _sumSqures -= _sumsSqures[0];
    _sampleSizes.erase(_sampleSizes.begin());
    _sums.erase(_sums.begin());
    _sumsSqures.erase(_sumsSqures.begin());
    _maximums.erase(_maximums.begin());
    _minimums.erase(_minimums.begin());
  }

  _maximum = 0;
  _minimum = 0;
  bool first = true;
  for (register unsigned int i = 0; i < _sampleSizes.size(); i++) {
    if (_sampleSizes[i] > 0) {
      if (first) {
        _maximum = _maximums[i];
        _minimum = _minimums[i];
        first = false;
      }
      else {
        if (_maximum < _maximums[i])
          _maximum = _maximums[i];
        if (_minimum > _minimums[i])
          _minimum = _minimums[i];
      }
    }
  }
}

SlidingWindowStatistics::SlidingWindowStatistics(SlidingWindowStatistics& other)
{
    set(other);
}

void SlidingWindowStatistics::set(SlidingWindowStatistics& other)
{
    _numberOfBins = other._numberOfBins;
    _sampleSize = other._sampleSize;
    _sum = other._sum;
    _sumSqures = other._sumSqures;
    _maximum = other._maximum;
    _minimum = other._minimum;
    _sampleSizes = other._sampleSizes;
    _sums = other._sums;
    _sumsSqures = other._sumsSqures;
    _maximums = other._maximums;
    _minimums = other._minimums;
}

void SlidingWindowStatistics::addSample(double sample) {
  double squredSample = sample*sample;
  _sampleSize ++;
  _sum += sample;
  _sumSqures += squredSample;
  if (_sampleSize == 1) {
    _maximum = sample;
    _minimum = sample;
  }
  else {
    if (_maximum < sample)
      _maximum = sample;
    if (_minimum > sample)
      _minimum = sample;
  }

  int currentIndex = _sampleSizes.size()-1;
  _sampleSizes[currentIndex] ++;
  _sums[currentIndex] += sample;
  _sumsSqures[currentIndex] += squredSample;
  if (_sampleSizes[currentIndex] == 1) {
    _maximums[currentIndex] = sample;
    _minimums[currentIndex] = sample;
  }
  else {
    if (_maximums[currentIndex] < sample)
      _maximums[currentIndex] = sample;
    if (_minimums[currentIndex] > sample)
      _minimums[currentIndex] = sample;
  }
}

void SlidingWindowStatistics::addSamples(int numberOfSamples, double average, double standardDeviation, double maximum, double minimum) {
  if (numberOfSamples > 0) {
    double sumSqures = numberOfSamples*(standardDeviation*standardDeviation + average*average);
    if (_sampleSize == 0) {
      _maximum = maximum;
      _minimum = minimum;
    }
    else {
      if (_maximum < maximum)
        _maximum = maximum;
      if (_minimum > minimum)
        _minimum = minimum;
    }
    _sampleSize += numberOfSamples;
    _sum += numberOfSamples*average;
    _sumSqures += sumSqures;

    int currentIndex = _sampleSizes.size()-1;
    if (_sampleSizes[currentIndex] == 0) {
      _maximums[currentIndex] = maximum;
      _minimums[currentIndex] = minimum;
    }
    else {
      if (_maximums[currentIndex] < maximum)
        _maximums[currentIndex] = maximum;
      if (_minimums[currentIndex] > minimum)
        _minimums[currentIndex] = minimum;
    }
    _sampleSizes[currentIndex] += numberOfSamples;
    _sums[currentIndex] += numberOfSamples*average;
    _sumsSqures[currentIndex] += sumSqures;
  }
}

string SlidingWindowStatistics::as_string() const
{
    return "";
}

MEDUSA_NAMESPACE_END
