#ifndef AggStreamSource_H
#define AggStreamSource_H

#include <vector>       // for vector

#include "Types.h"      // for {datatype_t, distributiontype_t,
//      distribution_t, agg_stream_id_t}
#include <common.h>

BOREALIS_NAMESPACE_BEGIN

/**
* Attribute definition
* The difinition of a attribute
*/

struct AttributeDef
{
	datatype_t               attribute_data_type; /**< data type */
	distributiontype_t       source_type;         /**< source type */
	vector<distribution_t>   source_parameters;   /**< This is a list of distributions which decide the parameters of the source distribution. Since the the prameters may be different for each of the sources, thus they are specified by a distribution instead of real valued parameters. */
};

/** Aggregate Stream Source
* The definition of the aggregate stream.
*/
class AggStreamSource
{
public:
	/**Default constructor
	*/
	AggStreamSource();

	agg_stream_id_t      id; /**< the aggregate stream id */

	int   num_streams; /**< number of individual streams */

	int   num_tuples;  /**<  number of tuples that will be generated for each individual stream. -1 for infinit stream. */

	distributiontype_t       rate_type; /**< the distribution of the aggregate stream. */

	vector<distribution_t>   rate_parameters; /**< parameters for the distribution. */

	vector<AttributeDef>     attributes; /**< The attribute definitions */


	/**
	* print the source information for debug
	*/
	void             print();

private:

	/**
	* print the source information for debug
	*/
	void printDistribution(distribution_t& distr);
	/**
	* print the source information for debug
	*/
	void printDistributionVector(vector<distribution_t>& distr_v);
	/**
	* print the source information for debug
	*/
	void printAttibueDef(AttributeDef& attr_def);
	/**
	* print the source information for debug
	*/
	void printDistributionType(distributiontype_t type, bool is_source);
	/**
	* print the source information for debug
	*/
	void printDateType(attr_type_t type);
};

BOREALIS_NAMESPACE_END
#endif
