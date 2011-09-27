#ifndef RandomNumGenerator_H
#define RandomNumGenerator_H

#include <cmath>        // for drand48()
#include <fstream>      // for fstream

#include "Types.h"
#include <common.h>

BOREALIS_NAMESPACE_BEGIN

/**This class define an object that can generate random numbers according to the distribution type and some parameters.
*/
class RandomNumGenerator
{
public:
	/**default constructor
	*/
	RandomNumGenerator(){}

	/** construct a RandomNumGenerator with given distribution type
	*/
	RandomNumGenerator(const distribution_t& par);

	/** destroy the RandomNumGenerator
	*/
	~RandomNumGenerator();

	/** Returns a random number
	* The getRandNum() function will return a random number each time it is called.
	* If the parameters are not valid, only zero will be returned by getRandNum().
	* For CONSTANT distribution, there is one parameter, which is the value of the constant.
	* For UNIFORM distribution, there are two parameters. The first one is the lower bound, the second one is the upper bound.
	* For NORMAL distribution, there are two parameters. The first one is the mean value, the second one is the standard deviation.
	* For EXPONENTIAL distribution, there is one parameter, which is the mean value.
	* For LINEAR distribution, there are four parameter. They are initial_value, step, lower_bound, upper_bound. Let count be the number of times that getRandNum() is called, the the random number returned by getRandNum() is min(max(lower_bound, initial_value + count * step), upper_bound).
	* For RANDOMWALK distribution, there are five parameter. They are initial_value, step, prob, lower_bound, upper_bound. Each time, the value generated is remembered. Each time getRandNum() is called, it return (last_value + step) with probability prob and (last_value - step) with probbility (1-prob). If the value is outside of the boundary, it will change the sign of the incremented value.
	* For RANDOMDIRECTIONWALK distribution, there are five parameter. They are initial_value, step, prob, lower_bound, upper_bound. Each time getRandNum() is called, with probability prob, the sign of step will be changed and (last_value + step) is returned.
	*/
	double      getRandNum();

	/** get a random number by given a interval
	* @param interval the interval
	* @return a random number given a interval
	*/
	double      getRandNum(double interval);

	/** get the type of the distribution
	* @param _type the distribution type
	* @return the type of the distribution
	*/
	distributiontype_t  type() const {return _type;}


	/**This function can check whether the parameters of a given distribution is valid
	* @param distr given distribution
	*/
	static bool     checkDistribution(const distribution_t& distr);

	/**This function returns whether the random numbergenerator is valid
	*/
	bool            valid() const { return _valid;}

private:
	distributiontype_t  _type; /**< distribution type */

	parlist_t       _parameters; /**< parameters */

	bool        _valid; /**< whether the parameters are valid */

	fstream*        _input_file; /**< The handle of a input file (the random numbers are in the input file */

	bool        _first_time;/**<Is true if the the RandomNumGenerator is called the first time */

	bool        _open; /**< Is true if the input file is open */

    /**returns the next random number 
     * @param interval the given interval
     */
	double      nextRand(double interval);

    /**Returns a number from normal distribution.  
     * @param m mean
     * @param s std
     */
	double      normal(double m, double s);
    
    /**Returns a number from exponential distribution. 
     * @param m mean 
     */ 
    double      exponential(double m); 
  
    /**Returns a number from linear distribution. 
     * @param interval the given interval
     */  
    double      linear(double interval);
    
    /**Returns a number from randomWalk distribution. 
     * @param interval the given interval
     */
	double      randomWalk(double interval);
   
    /**Returns a number from randomDirectionWalk distribution. 
     * @param interval the given interval
     */
    double      randomDirectionWalk(double interval);
    
    /**Returns a number from input file. */
    double      inputFile();
};

BOREALIS_NAMESPACE_END

#endif
