#ifndef AttributeSource_H
#define AttributeSource_H


#include "GenDataType.h"           // for GenDataType
#include "Types.h"                 // for parlist_t, distributiontype_t
#include "RandomNumGenerator.h"    // for RandomNumGenerator
#include "Timeval.h"               // for Timeval
#include <common.h>

BOREALIS_NAMESPACE_BEGIN
using namespace time_tools;


/** This class constains the source for an attribute
*/
class AttributeSource
{
public:
	/** Default constructor
	*/
	AttributeSource(){}

	/** Construct an AttributeSource with a distributiontion for the value as well as the data type for the value 
	* @param source_distr this is the distribution information of the attribute
	* @param type data type
	*/
	AttributeSource(const distribution_t& source_distr, datatype_t type);

	/**Initialize it with the initial timestamp
	* @param first_time the initial timestamp
	*/
	void initialize(Timeval first_time);

	/**Get the distribution type of the attribute source
	* @return the distribution type of the source
	*/
	distributiontype_t   getSourceType() const ;

	/** Get the size of the data
	* @return the size of the data
	*/
	int          getDataSize() const;

	/**Get the type of the data
	* @return the type of the data
	*/
	datatype_t   getDataType() const;

	/**Get the next data given a new timestamp
	* @param newtime a new timestam
	* @param data 
	* @return the next data
	*/
	int  getNextData(const Timeval& newtime, char* data);

private:
	int          _data_size; /**< size of the data */

	datatype_t   _data_type; /**< type of the data */

	RandomNumGenerator   _randnumber_generator; /**< the RandomNumGenerator */

	Timeval  _last_time; /**< Last timestamp, for history information */
};

BOREALIS_NAMESPACE_END
#endif
