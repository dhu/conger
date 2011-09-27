#include "StreamSender.h"

// C++ headers
#include <fstream>

// Local headers
#include "Types.h"          // for agg_stream_id_t
#include "util.h"

#define MAX_BUFFER      100000
BOREALIS_NAMESPACE_BEGIN

ptr<MedusaClient> StreamSender::_client;

StreamSender::StreamSender()
{
	char *deployment_file_name = "../../../utility/test/simple/gentest_deploy.xml";

	fstream of;
	of.open(deployment_file_name, ios::in);
	if (!of)
	{
		ERROR << "can't open input configuration file: " << deployment_file_name ;
		exit(1);
	}


	XmlHandler xmlhandler;

	//change XML file to XML Document, get root Node
	DOMDocument *doc;
	doc = xmlhandler.readXmlFileToDoc(deployment_file_name);
	DOMNode *root;
	if(doc)
	{
		root = (DOMNode*) doc->getDocumentElement();
	}
	else
	{
		ERROR << "Changing XML file '" 
			<< deployment_file_name
			<< "' to XML document failed.";
	}


	char *node_address_original; 
    char *node_address;
   
    node_address_original = xmlhandler.getDeployAttrValue(root, "endpoint", 0);

    node_address = change_address(node_address_original);
  
    DEBUG << "the connected node address is: "<< node_address;
    
	InetAddress addr(Util::form_endpoint(node_address,DEFAULT_BOREALIS_PORT));

	_client = (ptr<MedusaClient>)new MedusaClient(addr);

	if (!_client->set_data_path(
		MAX_BUFFER, addr.get_host(), addr.get_port()))
	{
		FATAL << "Failed setting data path for node "  << node_address;
	}
	else
	{
		NOTICE << "Success.";
	}


	pthread_t thread;
	if (pthread_create(&thread,
		0,
		(void*(*)(void*))run_client,
		(void*)&(_client->get_loop())) < 0)
	{
		WARN << "Can not input client thread.";
	}

}

void StreamSender::sendTuples(send_item_t *item)
{


    //Mesusa    
	ptr<StreamEvent> _event;
    
    //for batch size
    tuplelist_t *my_tuple_list = item->tuple_list;
	for (tuplelist_t::iterator it = my_tuple_list->begin();
		it != my_tuple_list->end();
		++it)
	{
		StreamTuple *current_stream_tuple = (StreamTuple*)(*it);
	    string stream_name = current_stream_tuple->getAggStreamID().stringID;      
        
        _event = ptr<StreamEvent>( new StreamEvent( stream_name ));
	    _event->_inject = true;

        batch_data(_event, stream_name, current_stream_tuple);	

   
    	// Transmit data to the node.
	    Status status = _client->fast_post_event(_event);

	    if (!status)
	    {
		    if ( status.as_string() == DataHandler::NO_SPACE )
		    {
			    WARN << "sending data for stream: "
				    << stream_name << "Node buffer full";
		    }
		    else
		    {
			    ERROR << "Connection closed... stopping event stream";
			    return;
		    }
	    }
	    else
	    {
		    DEBUG << "data sent to " << stream_name;
	    }
    }

	delete item;
}

void StreamSender::sendATuple(StreamTuple *stream_tuple)
{


    //Mesusa    
	ptr<StreamEvent> _event;
    
    //for batch size
	string stream_name = stream_tuple->getAggStreamID().stringID;      
        
    _event = ptr<StreamEvent>( new StreamEvent( stream_name ));
	_event->_inject = true;

    batch_data(_event, stream_name, stream_tuple);	

   
    // Transmit data to the node.
	Status status = _client->fast_post_event(_event);

	if (!status)
	{
        if ( status.as_string() == DataHandler::NO_SPACE )
	    {
            WARN << "sending data for stream: "
	            << stream_name << "Node buffer full";
	    }
		else
		{
		    ERROR << "Connection closed... stopping event stream";
			return;
		}
	}
	else
	{
	    DEBUG << "exist tuple sent to " << stream_name;
	}

}


void StreamSender::batch_data(
							  ptr<StreamEvent>         _event,
							  const string            &stream_name,
							  StreamTuple             *stream_tuple)
{

#ifdef DEBUG_INPUT
	cout << stream_name << " (" << endl;
#endif

	TupleHeader header;
       
	string buf = string((const char*)&header, sizeof(TupleHeader));

	string data = string(
			stream_tuple->getDataHandle(), 
			stream_tuple->getDataSize());
	buf += data;


#ifdef DEBUG_INPUT
#endif
	_event->insert_bin(buf); 

#ifdef DEBUG_INPUT
	cout << ")" << endl;
#endif

}

/**Determine the dotted IP address of a host machine.
 * @param host Name of the host machine; empty for local. 
 */    
char* StreamSender::get_host_address(  char *host )
{
    struct hostent  *host_ent;
    char           **host_list;
    char             hostname[128];
    struct in_addr  *host_number;
    char            *host_ip;
//
// The default port is a static assignment for now.
// It could be assigned by scanning available channels???
//..............................................................................


   // if (( strcmp(host, "127.0.0.1")) || ( strcmp(host, "localhost")))
   // {
        if ( gethostname( hostname, 80 ))           // IF no local host name,
        {   //if ( host.empty() )                     //    IF none given,
            //{   strcpy( hostname, "127.0.0.1" );    //       Default IP.
            //}
            //else                                    //    ELSE
            //{   
                strcpy( hostname, host );   //       Given name.
            //}                                       //    END IF
        }                                           // END IF
    //}
    //else
    //{   strcpy( hostname, host);
    //}

    //DEBUG << "host=" << to_string( hostname );
    host_ent = gethostbyname( hostname );

    if ( host_ent )
    {   host_list = host_ent->h_addr_list;

        while (( host_number = (struct in_addr *)*host_list++ ))
        {   host_ip = inet_ntoa( *host_number );

            //DEBUG  << "hit " << to_string( host_ip );
            break;
        }
    }

    // Assert host_ip???
    return( host_ip );
}


char* StreamSender::change_address(char *original)
{
    char *result;
    char *node_address_result;
    size_t index = strcspn(original,":");
   
    char node_address_ip_string[index];
    
    size_t i = 0;
    for(i = 0; i < index; i++)
    {
        node_address_ip_string[i] = original[i];
    }
    node_address_ip_string[i] = '\0';
    
    
    if(!strcmp(node_address_ip_string,"127.0.0.1") 
                || !strcmp(node_address_ip_string, "localhost")
                || !strcmp(node_address_ip_string, ""))
    {
        node_address_result = get_host_address(node_address_ip_string);
        
        NOTICE << "local ip address is: "<< node_address_result;
        
        int len = strlen(node_address_result);
        
        int k = 0;
        while(original[index + k]!='\0')
        {
            node_address_result[len+k]= original[index+k];
            k = k+1;
        }

        node_address_result[len+k]='\0';
        
        result = node_address_result;
        
    }
    else
    {
        result = original;
    }
    
    return result;

}



BOREALIS_NAMESPACE_END
