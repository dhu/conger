#include "stdio.h"

#include <map>
#include <vector>
#include <NMSTL/debug>
#include <NMSTL/net>

#include "netio.h"

using namespace std;
using namespace Borealis;

netio_state netstate;

typedef map<string, pair< size_t, void (*)(const borealis_tuple*)> > handler_map;
handler_map stream_handler_map;
void init_data_handlers(handler_map& stream_handlers);


void set_up_sockets_and_subscribe(string borealis_addr, string deploy_addr, string subscriber_addr)
{
  netstate.dest_control_addr = InetAddress(borealis_addr);
  if (netstate.dest_control_addr.get_port() == 0) {
      netstate.dest_control_addr = InetAddress(borealis_addr, 15000);
      netstate.dest_data_addr = InetAddress(borealis_addr, 15002);
  } else {
      netstate.dest_data_addr = 
	  InetAddress(netstate.dest_control_addr.get_host(),
		      netstate.dest_control_addr.get_port() + DATA_PORT_OFFSET);
  }

  netstate.local_addr = InetAddress(subscriber_addr);      
  if (netstate.local_addr.get_port() == 0) {
      netstate.local_addr = InetAddress(subscriber_addr, 25000);
  }


  // Subscribe to outputs (creates a recv socket per subscription).
  netstate.game_subs =
      ptr<GameSubscriber>
      (new GameSubscriber
       ( netstate.dest_control_addr, netstate.local_addr, InetAddress(deploy_addr), 
	 ptr<vector< ptr<BufferedSocket> > >::unowned(&(netstate.datapath_recv_sockets))));

  netstate.game_subs->init_subscribe();

  // Set up tuple handlers.
  init_data_handlers(stream_handler_map);

  netstate.datapath_send_socket = BufferedSocket(netstate.dest_data_addr);
  //netstate.datapath_send_socket.set_blocking(true);
  //NMSTL::Status r = netstate.datapath_send_socket.connect(netstate.dest_data_addr);
  //if (!r) printf("connect status: %s\n", to_string(r).c_str());
}





extern void handle_init_tuple             (const borealis_tuple*);
extern void handle_player_init_tuple      (const borealis_tuple*);
extern void handle_disconnect_tuple       (const borealis_tuple*);
extern void handle_position_tuple         (const borealis_tuple*);
extern void handle_inventory_tuple        (const borealis_tuple*);
extern void handle_inventory_item_tuple   (const borealis_tuple*);
extern void handle_item_pickup_tuple      (const borealis_tuple*);
extern void handle_item_spawn_tuple       (const borealis_tuple*);
extern void handle_item_acc_tuple         (const borealis_tuple*);
extern void handle_map_change_tuple       (const borealis_tuple*);
extern void handle_map_reload_tuple       (const borealis_tuple*);
extern void handle_mode_tuple             (const borealis_tuple*);
extern void handle_timeup_tuple           (const borealis_tuple*);
extern void handle_shot_tuple             (const borealis_tuple*);
extern void handle_damage_tuple           (const borealis_tuple*);
extern void handle_frags_tuple            (const borealis_tuple*);
extern void handle_death_tuple            (const borealis_tuple*);
extern void handle_sound_tuple            (const borealis_tuple*);
extern void handle_chat_tuple             (const borealis_tuple*);
extern void handle_servmsg_tuple          (const borealis_tuple*);
extern void handle_ping_tuple             (const borealis_tuple*);
extern void handle_edit_tuple             (const borealis_tuple*);
extern void handle_edit_ent_tuple         (const borealis_tuple*);
extern void handle_bot_init_tuple         (const borealis_tuple*);
extern void handle_add_bot_tuple          (const borealis_tuple*);


void init_data_handlers(handler_map& stream_handlers)
{
    string f_clientname;
    formataddr(netstate.game_subs->subscribed_addr(), f_clientname);

  // TODO: name incoming streams.

#define add_stream_handler(stream_name, tuple_struct, function_name)	\
    stream_handlers[stream_name] = make_pair(sizeof(tuple_struct), function_name)

  add_stream_handler(string("self_init-") + f_clientname,           cube_init_tuple,              handle_init_tuple);
  add_stream_handler("valid_player",                             cube_init_tuple,              handle_player_init_tuple);
  add_stream_handler("client_disconnect",                        cube_disconnect_tuple,        handle_disconnect_tuple);
  add_stream_handler(string("useful_pos-") + f_clientname,          cube_position_tuple,          handle_position_tuple);

  //add_stream_handler("", cube_inventory_tuple,         handle_inventory_tuple);
  add_stream_handler("player_items",                             cube_inventory_item_tuple,    handle_inventory_item_tuple);
  add_stream_handler("successful_pickup",                        cube_item_pickup_tuple,       handle_item_pickup_tuple);
  add_stream_handler("item_spawns",                              cube_item_spawn_tuple,        handle_item_spawn_tuple);
  add_stream_handler(string("item_ack-") + f_clientname,            cube_item_acc_tuple,          handle_item_acc_tuple);

  // TODO: ignore mode changes/reloads/timeups for now.
  add_stream_handler("new_map",                                  cube_map_change_tuple,        handle_map_change_tuple);
  add_stream_handler("reload_map",                               cube_map_reload_tuple,        handle_map_reload_tuple);
  add_stream_handler("mode_change",                              cube_mode_tuple,              handle_mode_tuple);
  add_stream_handler("map_timeup",                               cube_timeup_tuple,            handle_timeup_tuple);

  add_stream_handler("player_shots",                             cube_shot_tuple,              handle_shot_tuple);
  add_stream_handler("player_damage",                            cube_damage_tuple,            handle_damage_tuple);
  add_stream_handler("player_frags",                             cube_frags_tuple,             handle_frags_tuple);
  add_stream_handler("player_deaths",                            cube_death_tuple,             handle_death_tuple);

  add_stream_handler("sound",                                    cube_sound_tuple,             handle_sound_tuple);
  add_stream_handler("chat",                                     cube_chat_tuple,              handle_chat_tuple);
  add_stream_handler("server_msgs",                              cube_servmsg_tuple,           handle_servmsg_tuple);

  add_stream_handler(string("pong-") + f_clientname,                cube_ping_tuple,              handle_ping_tuple);
  add_stream_handler(string("cp-") + f_clientname,                  cube_ping_tuple,              handle_ping_tuple);

  add_stream_handler("world_edit",                               cube_edit_tuple,              handle_edit_tuple);
  add_stream_handler("entity_edit",                              cube_edit_ent_tuple,          handle_edit_ent_tuple);

  add_stream_handler(string("my_bot_init-") + f_clientname,         cube_bot_init_tuple,          handle_bot_init_tuple);
  add_stream_handler(string("useful_bot-") + f_clientname,          cube_add_bot_tuple,           handle_add_bot_tuple);

#undef add_stream_handler

}


void send_raw(string stream_name, const char* data, size_t size)
{
  StreamEvent evt(stream_name);
//  evt.m_inject = true;
  evt._inject = true;
  evt.insert_bin(string(data, size));
  OSerialString oss(OSerial::binary|OSerial::nosignature);
  oss << evt;

  netstate.datapath_send_socket.send_msg(oss.str().data(), oss.str().length());
}

// NOTE:
// Clients can't send all types of messages, some are initiated by the server alone.
// Clients should be able to handle all types of messages though.

#define SEND_TUPLE(stream_name, tuple_struct) \
  void send_##tuple_struct(const borealis_tuple* bt) \
  { \
    const tuple_struct *t = static_cast<const tuple_struct*>(bt); \
    send_raw(string(stream_name), (const char*) t, sizeof(tuple_struct)); \
  }

// No send method for item accs (only "server" does this).
SEND_TUPLE("player_inits",     cube_init_tuple);
SEND_TUPLE("player_positions", cube_position_tuple);
SEND_TUPLE("player_items",     cube_inventory_item_tuple);
SEND_TUPLE("item_pickups",     cube_item_pickup_tuple);
SEND_TUPLE("map_change",       cube_map_change_tuple);
SEND_TUPLE("mode_change",      cube_mode_tuple);
SEND_TUPLE("player_shots",     cube_shot_tuple);
SEND_TUPLE("player_damage",    cube_damage_tuple);
SEND_TUPLE("player_frags",     cube_frags_tuple);
SEND_TUPLE("player_deaths",    cube_death_tuple);
SEND_TUPLE("sound",            cube_sound_tuple);
SEND_TUPLE("chat",             cube_chat_tuple);
SEND_TUPLE("player_ping",      cube_ping_tuple);
SEND_TUPLE("world_edit",       cube_edit_tuple);
SEND_TUPLE("entity_edit",      cube_edit_ent_tuple);
SEND_TUPLE("new_bots",         cube_add_bot_tuple);

#undef SEND_TUPLE
  
void send_cube_tuple(int tuple_type, borealis_tuple* tuple)
{

    switch(tuple_type) {

#define CAST_AND_SEND(tuple_type, tuple_struct)	\
  case tuple_type: \
    send_##tuple_struct(tuple); \
  break;

      CAST_AND_SEND(SV_INITC2S, cube_init_tuple);
      CAST_AND_SEND(SV_POS, cube_position_tuple);
      CAST_AND_SEND(SV_ITEMLIST, cube_inventory_item_tuple);
      CAST_AND_SEND(SV_ITEMPICKUP, cube_item_pickup_tuple);
      CAST_AND_SEND(SV_MAPCHANGE, cube_map_change_tuple);
      CAST_AND_SEND(SV_GAMEMODE, cube_mode_tuple);
      CAST_AND_SEND(SV_SHOT, cube_shot_tuple);
      CAST_AND_SEND(SV_DAMAGE, cube_damage_tuple);
      CAST_AND_SEND(SV_FRAGS, cube_frags_tuple);
      CAST_AND_SEND(SV_DIED, cube_death_tuple);
      CAST_AND_SEND(SV_SOUND, cube_sound_tuple);
      CAST_AND_SEND(SV_TEXT, cube_chat_tuple);
      CAST_AND_SEND(SV_PING, cube_ping_tuple);
      CAST_AND_SEND(SV_CLIENTPING, cube_ping_tuple);
      CAST_AND_SEND(SV_EDITH, cube_edit_tuple);
      CAST_AND_SEND(SV_EDITT, cube_edit_tuple);
      CAST_AND_SEND(SV_EDITS, cube_edit_tuple);
      CAST_AND_SEND(SV_EDITE, cube_edit_tuple);
      CAST_AND_SEND(SV_EDITD, cube_edit_tuple);
      CAST_AND_SEND(SV_EDITENT, cube_edit_ent_tuple);
      CAST_AND_SEND(SV_ADDBOT, cube_add_bot_tuple);

#undef CAST_AND_SEND

    default:
	printf("Attempted to send unknown tuple of type: %d\n", tuple_type);
	break;
    }
}


// Output dispatcher.

void handle_incoming_tuples()
{
  int recv_buffer_sz = 1024;

  vector< ptr<BufferedSocket> >::iterator recv_it = netstate.datapath_recv_sockets.begin();
  vector< ptr<BufferedSocket> >::iterator recv_end = netstate.datapath_recv_sockets.end();

  for (; recv_it != recv_end; recv_it++) {

      if ( *recv_it ) {

	  int recv_len = 0;
	  char buffer[recv_buffer_sz];
	  string acc;

	  while ( (recv_len = ((*recv_it)->recv_msg(buffer, recv_buffer_sz))) > 0 ) {
	      acc += string(buffer, recv_len);
	  }

	  if (acc.length() == 0)
	      continue;

	  ISerialData iss(constbuf(acc.data(), acc.length()), ISerial::binary);
	  ptr<StreamEvent> evt(new StreamEvent());

	  int num_proc = 0;
          unsigned int last_event_pos = 0;
          unsigned int buf_length = acc.length();

	  while( (last_event_pos < buf_length) && iss.stat() && (iss >> *evt)) {

              last_event_pos = iss.pos();
	      
	      // Dispatch event to apt handler.
//	      string stream_name = evt->m_stream.as_string();
          string stream_name = evt->_stream.as_string();
	      handler_map::iterator handler_it = stream_handler_map.find(stream_name);
	      handler_map::iterator handler_end = stream_handler_map.end();

//	      if (handler_it != handler_end) {
//                  assert(evt->m_tuple_size == handler_it->second.first);

          if (handler_it != handler_end) {
              assert(evt->_tuple_size == handler_it->second.first);

              size_t tuple_size = handler_it->second.first;
              void (*handler)(const borealis_tuple*) = handler_it->second.second;
//              for(int c = 0; c < evt->m_inserted_count; ++c) {
//                  handler((borealis_tuple*) ((c*tuple_size) + evt->m_bin_tuples.data()));
//                  ++num_proc;
//              }

              DEBUG << "Received " << evt->_inserted_count << " tuples.";

              for(int c = 0; c < evt->_inserted_count; ++c) {
                  handler((borealis_tuple*) ((c*tuple_size) + evt->_bin_tuples.data()));
                  ++num_proc;
              }

	      } else {
              NOTICE << "No handler for stream " << stream_name;
	      }
	      
	  }
	  
	  // TODO : Check pgs comment: "If there is an error we only read 47 bytes of Stream even header"
      constbuf remain_cb = iss.remainder();
      unsigned int remain_length = remain_cb.length();
	  if (!iss.stat() || remain_length > 0) {
              // yna -- iss.remainder starts at the failed object,
              // not at the beginning of the StreamEvent
              DEBUG << "Copying " << acc.length() - last_event_pos << " bytes of unfinished data";
	      (*recv_it)->set_partial_recv_buffer(acc.data() + last_event_pos, acc.length() - last_event_pos);
	  }
      }

      else 
	  NOTICE << "Invalid socket found!";
  }

  netstate.game_subs->handle_query_output();
}
