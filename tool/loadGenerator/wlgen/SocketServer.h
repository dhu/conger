#ifndef SocketServer_H
#define SocketServer_H

// C++ headers
#include <iostream>               // for cout
#include <cstring>                // for memset
#include <cstdio>                 // for perror
#include <cerrno>                 // for perror
#include <csignal>                // for signal, SIGPIPE, SIG_IGN


// Linux headers
#include <sys/types.h>            // for {socket, setsockopt, accept}
#include <sys/socket.h>           // for {socket, setsockopt, listen,accept}
#include <netinet/in.h>           // for htonl
#include <unistd.h>               // for close
#include <pthread.h>              // for pthread_mutex_*, pthread_cond_*
#include <common.h>

BOREALIS_NAMESPACE_BEGIN

/**This class implements a request from a socket
*/
class Request
{
public:
	Request(int sock) : socket_num(sock), next(0){}

	int  socket_num; /**< socket number */
	Request* next; /**< next request */
};

/**A thread safe request list
*/
class RequestList
{
public:
	/**Construct an empty list
	*/
	RequestList();

	/**add a request to this list
	*/
	void add_request( int);

	/**Returns a request from the list and remove it from the list
	*/
	int  get_request();

	/** Returns 1 if the list is empty
	*/
	int  is_empty();

private:
	Request*     first_request; /**< the first request */
	Request*     last_request;  /**< the last request */
	pthread_mutex_t  m_list;    /**< mutex for access list */
	pthread_cond_t   cond_have_request;     /**< condition */
};

inline RequestList::RequestList() : first_request(0),last_request(0)
{
	if (pthread_mutex_init(&m_list,0) < 0)
		perror("socket_tools::RequestList: pthread_mutex_init");
	if (pthread_cond_init(&cond_have_request,0) < 0)
		perror("socket_tools::RequestList: pthread_cond_init");
}

inline void RequestList::add_request(int sock)
{
	if (pthread_mutex_lock(&m_list) < 0)
		perror("socket_tools::RequestList: pthread_mutex_lock");
	Request* newrequest = new Request(sock);
	if(!last_request)
	{
		first_request = newrequest;
		last_request = newrequest;
	}
	else
	{
		last_request->next = newrequest;
	}
	if (pthread_cond_signal( &cond_have_request ) < 0)
		perror("socket_tools::RequestList: pthread_cond_signal");
	if (pthread_mutex_unlock(&m_list) < 0)
		perror("socket_tools::RequestList: pthread_mutex_unlock");
}

inline int RequestList::get_request()
{
	if (pthread_mutex_lock(&m_list) < 0)
		perror("socket_tools::RequestList: pthread_mutex_lock");

	while( !first_request )
	{
		if (pthread_cond_wait(&cond_have_request, &m_list) < 0)
			perror("socket_tools::RequestList: pthread_cond_wait");
	}

	int sock = first_request->socket_num;

	Request* next_request = first_request->next;
	delete first_request;
	first_request = next_request;
	if( !first_request )
		last_request = 0;
	if (pthread_mutex_unlock(&m_list) < 0)
		perror("socket_tools::RequestList: pthread_mutex_unlock");

	return sock;
}

/**This class implements a thread that handles an connection
*/
class ConnectionThread
{
public:

	/**Construct a connection thread
	* @param pt_num thread number
	* @param request_list the list of requests
	* @param serve the function that server the request
	* @param parameter the parameter to be passed to the serve function
	*/
	ConnectionThread(
		int            pt_num,
		RequestList*   request_list,
		void(*serve)(int, void*),
		void*      parameter
		);

	/**Start the thread
	*/
	bool start();

private:
	pthread_t        _thread; /**< the thread */
	int          _thread_id;  /**< id of the thread */
	RequestList*     _request_list;/** the list of the request */
	void (*_serve)(int, void*); 
	void*        _parameter;

	static void  connection_handler(ConnectionThread*);
};

inline ConnectionThread::ConnectionThread(
	int       pt_num,
	RequestList*  request_list,
	void(*serve)(int, void*),
	void*     parameter):
_thread_id(pt_num),
_request_list(request_list),
_serve(serve),
_parameter(parameter)
{
}

inline bool ConnectionThread::start()
{
	if (pthread_create(&_thread,
		0,
		(void*(*)(void*))connection_handler,
		(void*)this) < 0)
	{
		perror("socket_tools::ConnectionThread: pthread_create");
		return false;
	}
	return true;
}

inline void ConnectionThread::connection_handler(ConnectionThread* me)
{
	while(1)
	{
		int sock = me->_request_list->get_request();
		me->_serve(sock, me->_parameter);
	}
}


/** Socket Server
*/
class SocketServer
{

public:
	/**Construct a socket server
	* @param port port of the server
	* @param type type of the server connections
	* @param num_handler_threads number of handler threads in the server
	* @param server_handler the hanlder function
	* @param parameter the parameter that will be passed to the handler
	*/
	SocketServer(
		int port,
		int type,
		int num_handler_threads,
		void(*server_handler)(int, void*),
		void* parameter
		);

	/**Close the server
	*/
	~SocketServer();

	/**Start the server
	*/
	bool start();

private:
	int      _port; /**< port of the server */


	int          _type;/**< type of the connection */


	int      _num_handler_threads;/**<number of handler threads */

	void     (*_server_handler)(int, void*); /**< handler function */

	void*    _parameter; /**< parameter to be passed to the handler function */

	int      _sock;/**< socket for the connection */

	pthread_t        _monitor_thread; /**< the monitor thread */

	RequestList      _request_list;/**< the list of requests */

	ConnectionThread**   _connection_threads;/**< a connection thread */

	static void      monitor(SocketServer* me);/**< request monitoring */

	bool         bindServer();/**< bind the server */
};

inline SocketServer::SocketServer(
								  int port,
								  int type,
								  int num_handler_threads,
								  void (*server_handler)(int, void*),
								  void* parameter):
_port(port),
_type(type),
_num_handler_threads(num_handler_threads),
_server_handler(server_handler),
_parameter(parameter),
_sock(0),
_connection_threads(0)
{
}


inline SocketServer::~SocketServer()
{
	if (_connection_threads != 0)
	{
		for (int i = 0; i < _num_handler_threads; ++i)
			delete  _connection_threads[i];
		delete _connection_threads;
	}
	if (_sock != 0)
		close(_sock);
}

inline bool SocketServer::start()
{
	// Writing to an unconnected socket will cause a process to receive
	// a SIGPIPE signal.  We don't want to die if this happens, so we
	// ignore SIGPIPE.
	signal (SIGPIPE, SIG_IGN);

	//Create thread pool
	_connection_threads = new ConnectionThread*[_num_handler_threads];
	for (int i=0; i < _num_handler_threads; ++i)
	{
		_connection_threads[i] =
			new ConnectionThread(
			i+1,
			&_request_list,
			_server_handler,
			_parameter);
		if (_connection_threads[i]->start() == false)
		{
			WARN << "socket_tools::SocketServer: "
				<< "creating connection threads failed.";
			for (int j = 0; j <= i; ++j)
				delete _connection_threads[i];
			delete _connection_threads;
			_connection_threads = 0;
			return false;
		}
	}

	// Bind Server
	if (!bindServer())
		return false;

	//Create monitor thread
	if (pthread_create(&_monitor_thread,
		0,
		(void*(*)(void*))monitor,
		(void*)this) < 0)
	{
		perror("socket_tools::SocketServer: pthread_create");
		WARN << "socket_tools::SocketServer: "
			<< "creating monitor thread failed.";
		return false;
	}

	return true;
}

inline bool SocketServer::bindServer()
{
	// Create socket.
	_sock = socket(AF_INET, _type, 0);
	if (_sock == -1)
	{
		perror("socket_tools::SocketServer: opening server socket");
		_sock = 0;
		return false;
	}

	// Make socket reusable
	int on=1;
	if (setsockopt(_sock, SOL_SOCKET,SO_REUSEADDR , &on, sizeof(on)) == -1)
	{
		perror("socket_tools::SocketServer: set socket option");
		return false;
	}

	// Bind socket
	struct sockaddr_in server;
	memset(&server, 0,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(_port);
	if (bind(_sock, (struct sockaddr *) &server, sizeof server)
		== -1)
	{
		perror("socket_tools::SocketServer: binding server socket");
		return false;
	}

	return true;
}

inline void SocketServer::monitor(SocketServer* me)
{
	// Start accepting connections.
	listen(me->_sock, me->_num_handler_threads);
	while(true)
	{
		int msgsock = accept(me->_sock,(struct sockaddr *) 0,(socklen_t *) 0);
		if (msgsock == -1)
			perror("socket_tools::SocketServer: server accept") ;
		else
		{
			me->_request_list.add_request(msgsock);
		}
	}
}

BOREALIS_NAMESPACE_END

#endif
