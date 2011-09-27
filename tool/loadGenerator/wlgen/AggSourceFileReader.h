#ifndef AggSourceFileReader_H
#define AggSourceFileReader_H

#include <fstream>              // for fstream
#include <vector>               // for vector
#include <common.h>

#include "AggStreamSource.h"    // for AggStreamSource
#include "Types.h"              // for distributiontype_t,
//     distribution_t
#include "XmlHandler.h"         //for DOMNode



BOREALIS_NAMESPACE_BEGIN

const int BUFFERSIZE = 1024 * 64;


/**A class for reading AggStream information
* This class is used for reading distribution information for a Aggregate Stream from a given file.
*/
class AggSourceFileReader
{
public:

	/**Read configurations from a file
	* @param filename name of the file
	* @param agg_sources the AggStreamSources (vector) to be read
	* @param show_process if show_process, the configuration of the agg_source will be printed
	*/
	static bool readFile(
		const char                    *filename,
		vector<AggStreamSource>       &agg_sources,
		bool                          show_process);

private:

	/**Read configuration for a specific stream from a file
	* @param file the given file
	* @param xmlHandler for handle the input XML file 
	* @param agg_source the AggStreamSouce to be configured
	* @param root the root element of the input XML file
	* @param stream_name the name of the stream which to be configured
	* @param show_process if show_process, the configuration of the agg_source will be printed
	*/
	static bool confAggStreamSource(
		fstream             &file,             
		XmlHandler          xmlhandler,
		AggStreamSource     &agg_source,
		const DOMNode       *root,
		const char          *stream_name,
		bool                show_process);


	/**Read distribution information for a specific input stream from a file
	* @param file the given file
	* @param xmlHandler for handle the input XML file 
	* @param agg_source the AggStreamSouce to be configured
	* @param root the root element of the input XML file
	* @param stream_name the name of the stream which to be configured
	* @param show_process if show_process, the configuration of the agg_source will be printed
	*/
	static bool readInputDistribution(
		fstream                     &file,
		XmlHandler                  xmlhandler,
		AggStreamSource             &agg_source,
		const DOMNode               *root, 
		const char                  *stream_name, 
		bool                        show_process);


	/**Read ParamDistribution information for a specific stream from a file
	* @param file the given file
	* @param xmlHandler for handle the input XML file 
	* @param root the root element of the input XML file
	* @param stream_name the name of the stream which to be configured
	* @param i the index of the paramDistribution
	* @param distr the distribution information
	* @param show_process if show_process, the distribution information will be printed
	*/
	static bool readInputParamDist(
		fstream             &file,
		XmlHandler          xmlhandler,
		const DOMNode       *root,
		const char          *stream_name,
		int                 i,
		distribution_t      &distr,
		bool                show_process);



	/**Read the attribute configuration for a specific input stream from a file
	* @param file the given file
	* @param xmlHandler for handle the input XML file 
	* @param agg_source the AggStreamSouce to be configured
	* @param root the root element of the input XML file
	* @param stream_name the name of the stream which to be configured
	* @param show_process if show_process, the configuration of the agg_source will be printed
	*/
	static bool readAttributes(
		fstream                     &file,
		XmlHandler                  xmlhandler,
		AggStreamSource             &agg_source,
		const DOMNode               *root,
		const char                  *stream_name,
		bool                        show_process);


	/**Read Attribute definition for a specific input stream from a file
	* @param file the given file
	* @param xmlHandler for handle the input XML file 
	* @param root the root element of the input XML file
	* @param stream_name the name of the stream which to be configured
	* @param i the index of the field of the stream
	* @param attr_def the AttributeDef to be configured
	* @param show_process if show_process, the Attribute information will be printed
	*/
	static bool readAttributeDef(
		fstream             &file,
		XmlHandler          xmlhandler,
		const DOMNode       *root,
		const char          *stream_name,
		int                 i,
		AttributeDef        &attr_def,
		bool                show_process);



	/**Read paramDistribution of a specific attribute for the stream from a file
	* @param file the given file
	* @param xmlHandler for handle the input XML file 
	* @param root the root element of the input XML file
	* @param stream_name the name of the stream which to be configured
	* @param attr_i the index of the field of the stream
	* @param param_j the index of the paramDistribution
	* @param distr the distribution information
	* @param show_process if show_process, the distribution information will be printed
	*/
	static bool readAttrParamDist(
		fstream             &file,
		XmlHandler          xmlhandler,
		const DOMNode       *root,
		const char          *stream_name,
		int                 attr_i,
		int                 param_j,
		distribution_t      &distr,
		bool                show_process);
};


BOREALIS_NAMESPACE_END

#endif
