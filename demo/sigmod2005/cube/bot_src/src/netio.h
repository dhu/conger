#ifndef NET_IO_H
#define NET_IO_H

// yna
// Following macros annoyingly defined in tools.h
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include <string>
#include <NMSTL/net>

#include "tuples.h"
#include "gamesubscriber.h"

/* Setup the a cube client's sockets and subscribe to the apropriate streams
 * We assume that CLS + Borealis are started and that the query +schema are loaded.
 */

void set_up_sockets_and_subscribe(std::string borealis_addr, std::string deploy_addr, std::string subscriber_addr);

/* We assume that the client's socket is already setup etc..
 * Immediately sends the tuple over the network.
 * 
 * Also sets source_ip to the tuple
 *
 */
void send_cube_tuple(int tuple_type, borealis_tuple* t);

/*  
 * Makes repeated calls to recv to get any awaiting 
 * cube tuples then calls the tuple handling function on the recieved tuples
 * 
 */
void handle_incoming_tuples();

typedef struct {

  BufferedSocket datapath_send_socket;
  std::vector< NMSTL::ptr<BufferedSocket> > datapath_recv_sockets; 
  NMSTL::InetAddress dest_control_addr;
  NMSTL::InetAddress dest_data_addr;
  NMSTL::InetAddress local_addr;

  NMSTL::ptr<GameSubscriber> game_subs;

} netio_state;

#endif
