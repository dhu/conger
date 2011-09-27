#ifndef TimeStampGenerator_H
#define TimeStampGenerator_H

#include <ctime>            // for timeval, gettimeofday

#include "RandomNumGenerator.h"     // for RandomNumGenerator
#include "Timeval.h"            // for Timeval
#include <common.h>

BOREALIS_NAMESPACE_BEGIN

using namespace time_tools;


const double MIN_INTERVAL = 0.00001;      // minumum interval between ticks

/**This class is for generate time stamp
*/
class TimeStampGenerator
{
public:
	/**Default constructor
	*/
	TimeStampGenerator(){}

	/**Construct the TimeStampGenerator with a given distribution. For interarrival time generation
	* @param random_number_generator_par 
	*/
	TimeStampGenerator(const distribution_t& random_number_generator_par);

	/**Delete the TimeStampGenerator
	*/
	~TimeStampGenerator();

	/**Returns the timestamp
	*/
	Timeval& getTimestamp();

	/**Set the time stamp
	* @param t time stamp
	*/
	void     setTimeStamp(const timeval &t);

	/**generate new time stamp
	*/
	void     tick();

	/**Change the ratio in generating time stamp.
	* @param ratio If ratio > 1, then the rate is larger than normal rate
	*/
	void     changeRatio(double ratio);

	/**Change the interarrival time generator
	*/
	void     changeIntervalGenerator(
		const distribution_t& random_number_generator_par);

protected:

	Timeval      _timestamp;/**< Timestamp */

	RandomNumGenerator*  _interval_generator;/**<The interval generator */

	double       _ratio;/**< The ratio for the rate */

};

BOREALIS_NAMESPACE_END
#endif
