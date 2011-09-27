#ifndef WGServer_H
#define WGServer_H

// C++ headers
#include <map>		// for pair
#include <fstream>
#include <common.h>

// Local headers
#include "SocketServer.h"	// for SocketServer
#include "WorkloadGenerator.h"	    // for WorkloadGenerator

BOREALIS_NAMESPACE_BEGIN
/**This class is for accepting request from GUI.
 *
 */
class WGServer
{
	static const int DEFAULT_PORT = 17000;
	static const int DEFAULT_TYPE = SOCK_STREAM;
	static const int DEFAULT_NUM_HANDLERS = 1;

	static const int MAX_BUFFER_SIZE = 1024 * 1024;

public:
	/**Creates a WGServer with given workload_generator with default port and default nubmer of handler threads
     * @param workload_generator the given workload_generator 
     * 
     */
	WGServer(WorkloadGenerator* workload_generator);

	/** Creates a WGServer with given workload_generator with given port and nubmer of handler threads
     * @param workload_generator the given workload_generator
     * @param port the given port
     * @param num_handler_threads the given number of handler threads
     */
	WGServer(
		WorkloadGenerator* workload_generator,
		int port,
		int num_handler_threads);

	/** Destroy the server
     */
	~WGServer();

	/** Initialze the server
     */
	bool initialize();

private:
	int			_port;  /**< the port number */
	int			_num_handler_threads;   /**< the given number of handler threads */

	SocketServer        *_server;  /**< the socket server to connect */
	WorkloadGenerator*		_workload_generator; /**< the workload generator */  

	/**This function is writen by arsinger to receive command packet from GUI
     *
     * @param sock socket number
     * @param me WGServer
     */
    static void	    serverHandler(int sock, WGServer* me);
	
    /**Read Socket
     *
     */
    static bool	    readSocket(
		int	socket,
		char	*buf,
		int	max_len,
		int	&file_length);

    /**read in the information for aggegate stream sources
     *@param file the given file
	 *@param xmlHandler for handle the input XML file 
	 *@param root the root element of the input XML file
	 *@param streams all the aggregate stream sources to be configured
     */
	static bool	    readAggStreamInfo(
		fstream             &file,
		XmlHandler          xmlhandler,
		const DOMNode       *root,
		vector<AggStreamSource> &streams);

    /**Configure a aggregate Stream Source with given parameter
     *@param file the given file
	 *@param xmlHandler for handle the input XML file 
	 *@param agg_source the AggStreamSouce to be configured
	 *@param root the root element of the input XML file
	 *@param stream_name the name of the stream which to be configured
	 *@param show_process if show_process, the configuration of the agg_source will be printed
     */
	static bool confAggStreamSource(
		fstream             &file,             
		XmlHandler          xmlhandler,
		AggStreamSource     &agg_source,
		const DOMNode       *root,
		const char          *stream_name,
		bool                show_process);

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
		fstream		&file,
		XmlHandler		xmlhandler,
		const DOMNode	*root,
		const char		*stream_name,
		int 		i,
		distribution_t	&distr,
		bool                show_process);

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
		fstream		&file,
		XmlHandler		xmlhandler,
		const DOMNode	*root,
		const char		*stream_name,
		int 		i,
		AttributeDef	&attr_def,
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
		fstream		&file,
		XmlHandler		xmlhandler,
		const DOMNode	*root,
		const char		*stream_name,
		int 		attr_i,
		int 		param_j,
		distribution_t	&distr,
		bool                show_process);


    /**
     * read command string from socket command file
     */
    static bool readLine(fstream& file, char* buf, int len);

    
};

BOREALIS_NAMESPACE_END

#endif
