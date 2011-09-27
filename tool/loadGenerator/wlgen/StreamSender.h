#ifndef StreamSender_H
#define StreamSender_H

#include <common.h>
#include <MedusaClient.h>
#include <TupleHeader.h>


// Local headers
#include "StreamTuple.h"        // for StreamTuple, tuplelist_t
#include "SocketClient.h"       // for socket_tools::SocketClient
#include "XmlHandler.h"         // for XmlHandler


BOREALIS_NAMESPACE_BEGIN

struct SendItem
{
	tuplelist_t* tuple_list;

	SendItem(tuplelist_t* list) : tuple_list(list) {}

	~SendItem()
	{
		for (tuplelist_t::iterator it = tuple_list->begin();
			it != tuple_list->end();
			++it)
		{
			delete (*it);
		}
		delete tuple_list;
	}
};

typedef struct SendItem send_item_t;

/** This class is for connect to the Medusa and send tuples
*/
class StreamSender
{

public:

	/** Constructor
	*/
	StreamSender();

	/** send the tuples to Borealis
	* @param item including the tuples to be sent to Borealis
	*/
	static void sendTuples(send_item_t *item);

    
    /** send a tuple to Borealis
	* @param item including the tuples to be sent to Borealis
	*/
	static void sendATuple(StreamTuple *stream_tuple);

    static void changeDeployQuery();

private:


	static ptr<MedusaClient> _client; /**< Medusa client */

	static ptr<StreamEvent> _event; /**< Stream Event */

	/**Batch data from a given stream
	* @param stream_name the name of the stream
	*/      
	static void batch_data(
		ptr<StreamEvent>    _event, 
		const string        &stream_name, 
		StreamTuple         *stream_tuple);

	/**Start the given loop
	*/
	static void run_client(IOEventLoop* loop) { 
		_client->run(); }

    
    /**Determine the dotted IP address of a host machine.
     * @param host Name of the host machine; empty for local. 
     */    
    static char*  get_host_address( char *host );

    /**Change the IP address the from deploy XML file.
     */
    static char* change_address(char *original);

};



BOREALIS_NAMESPACE_END
#endif
