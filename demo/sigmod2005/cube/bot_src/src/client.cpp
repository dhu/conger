// client.cpp, mostly network related client game code

#include <string>
#include <vector>

#include "tuples.h"
#include "netio.h"
#include "cube.h"

ENetHost *clienthost = NULL;
int connecting = 0;
int connattempts = 0;
int disconnecting = 0;
int clientnum = -1;         // our client id in the game
bool c2sinit = false;       // whether we need to tell the other clients our stats


// Added by yna.
extern vector<entity> ents;
extern char* tuple_type_str[];
std::vector<std::pair<int, NMSTL::ptr<borealis_tuple> > > client_pending_message;

extern bool send_position_flag; 
VAR(tuplerate, -25, 10, 25);

int tuple_put_items(std::vector<cube_inventory_item_tuple>& item_list_tuples)
{
  item_list_tuples.clear();

  printf("Number of entities: %d\n", ents.length());

  int total_items = 0;
  loopv(ents) if((ents[i].type>=I_SHELLS && ents[i].type<=I_QUAD) || ents[i].type==CARROT)
    {
      cube_inventory_item_tuple t;
      t.clientnum = clientnum;
      t.item_index = i;
      printf("Putting item %d of type %d\n", i, ents[i].type);
      item_list_tuples.push_back(t);
      total_items++;
    };

  return total_items;
}


int getclientnum() { return clientnum; };

bool multiplayer()
{
    // check not correct on listen server?
    if(clienthost) conoutf("operation not available in multiplayer");
    return clienthost!=NULL;
};

bool allowedittoggle()
{
    bool allow = !clienthost || gamemode==1;
    if(!allow) conoutf("editing in multiplayer requires coopedit mode (1)");
    return allow; 
};

VARF(rate, 0, 0, 25000, if(clienthost) enet_host_bandwidth_limit (clienthost, rate, rate));

void throttle();

VARF(throttle_interval, 0, 5, 30, throttle());
VARF(throttle_accel,    0, 2, 32, throttle());
VARF(throttle_decel,    0, 2, 32, throttle());

void throttle()
{
    if(!clienthost || connecting) return;
    assert(ENET_PEER_PACKET_THROTTLE_SCALE==32);
    enet_peer_throttle_configure(clienthost->peers, throttle_interval*1000, throttle_accel, throttle_decel);
};

void newname(char *name) { c2sinit = false; strn0cpy(player1->name, name, 16); };
void newteam(char *name) { c2sinit = false; strn0cpy(player1->team, name, 5); };

COMMANDN(team, newteam, ARG_1STR);
COMMANDN(name, newname, ARG_1STR);

void connects(char *servername)
{   
    disconnect(1);  // reset state
    addserver(servername);

    conoutf("attempting to connect to %s", (int)servername);
    ENetAddress address = { ENET_HOST_ANY, CUBE_SERVER_PORT };
    if(enet_address_set_host(&address, servername) < 0)
    {
        conoutf("could not resolve server %s", (int)servername);
        return;
    };

    clienthost = enet_host_create(NULL, 1, rate, rate);

    if(clienthost)
    {
        enet_host_connect(clienthost, &address, 1); 
        enet_host_flush(clienthost);
        connecting = lastmillis;
        connattempts = 0;
    }
    else
    {
        conoutf("could not connect to server");
        disconnect();
    };
};

void disconnect(int onlyclean, int async)
{
    if(clienthost) 
    {
        if(!connecting && !disconnecting) 
        {
            enet_peer_disconnect(clienthost->peers);
            enet_host_flush(clienthost);
            disconnecting = lastmillis;
        };
        if(clienthost->peers->state != ENET_PEER_STATE_DISCONNECTED)
        {
            if(async) return;
            enet_peer_reset(clienthost->peers);
        };
        enet_host_destroy(clienthost);
    };

    if(clienthost && !connecting) conoutf("disconnected");
    clienthost = NULL;
    connecting = 0;
    connattempts = 0;
    disconnecting = 0;
    clientnum = -1;
    c2sinit = false;
    player1->lifesequence = 0;
    loopv(players) zapdynent(players[i]);
    
    localdisconnect();

    if(!onlyclean) { stop(); localconnect(); };
};

void trydisconnect()
{
    if(!clienthost)
    {
        conoutf("not connected");
        return;
    };
    if(connecting) 
    {
        conoutf("aborting connection attempt");
        disconnect();
        return;
    };
    conoutf("attempting to disconnect...");
    disconnect(0, !disconnecting);
};

string ctext;
void toserver(char *text) { conoutf("%s:\f %s", (int)player1->name, (int)text); strn0cpy(ctext, text, 80); };
void echo(char *text) { conoutf("%s", (int)text); };

COMMAND(echo, ARG_VARI);
COMMANDN(say, toserver, ARG_VARI);
COMMANDN(connect, connects, ARG_1STR);
COMMANDN(disconnect, trydisconnect, ARG_NONE);

// collect c2s messages conveniently

vector<ivector> messages;

void addmsg(int rel, int num, int type, ...)
{
    if(demoplayback) return;
    
    if(num!=msgsizelookup(type)){ sprintf_sd(s)("inconsistant msg size for %d (%d != %d)", type, num, msgsizelookup(type)); fatal(s); };
    ivector &msg = messages.add();
    msg.add(num);
    msg.add(rel);
    msg.add(type);
    va_list marker;
    va_start(marker, type); 
    loopi(num-1) msg.add(va_arg(marker, int));
    va_end(marker);  
};

void server_err()
{
    conoutf("server network error, disconnecting...");
    disconnect();
};

int lastupdate = 0, lastping = 0;
string toservermap;
string currentmap;
bool senditemstoserver = false;     // after a map change, since server doesn't have map data

string clientpassword;
void password(char *p) { strcpy_s(clientpassword, p); };
COMMAND(password, ARG_1STR);

bool netmapstart() { senditemstoserver = true; return clienthost!=NULL; };

void initclientnet()
{
    ctext[0] = 0;
    toservermap[0] = 0;
    clientpassword[0] = 0;
    newname("unnamed");
    newteam("red");
};

void sendpackettoserv(void *packet)
{
    if(clienthost) { enet_host_broadcast(clienthost, 0, (ENetPacket *)packet); enet_host_flush(clienthost); }
    else localclienttoserver((ENetPacket *)packet);
}

std::map<int, int> stream_seq_nos;
extern std::map<int, bool> bot_sent_tuple;

extern void send_cube_tuple(int, borealis_tuple*);

void c2binfo(dynent* d)
{
  std::vector< std::pair < int, NMSTL::ptr<borealis_tuple> > > tuples_to_send;
  std::vector< cube_inventory_item_tuple > item_list_tuples;

  bool serveriteminitdone = false;
  int total_items = 0;

  int update_elapse = (tuplerate < 0? 1000 * (-tuplerate) : (tuplerate == 0? 40 : 1000/tuplerate));

  if(lastmillis - lastupdate < update_elapse) { return; }   // don't update faster than 25fps


  if(toservermap[0]) {

    printf("sending map change\n");

    // suggest server to change map
    // do this exclusively as map change may invalidate rest of update
    NMSTL::ptr<cube_map_change_tuple> t = NMSTL::ptr<cube_map_change_tuple>(new cube_map_change_tuple());
    t->clientnum = clientnum;
    memcpy(t->toservermap, toservermap, 128); // The string is typedefed char[260] 
    t->nextmode = nextmode;
    tuples_to_send.push_back(std::make_pair((int)SV_MAPCHANGE, t.dynamic_cast_to<borealis_tuple>()));

    strncpy(currentmap, toservermap ,_MAXDEFSTR);
    toservermap[0] = 0;

  } else {

      //printf("other logic\n");

    if (send_position_flag) {
      NMSTL::ptr<cube_position_tuple> t = NMSTL::ptr<cube_position_tuple>(new cube_position_tuple());
      t->clientnum  = clientnum;
      memset(t->team_name, 0, 32);
      memcpy(t->team_name, d->team, strlen(d->team));
      t->position_x = d->o.x*DMF;
      t->position_y = d->o.y*DMF;
      t->position_z = d->o.z*DMF;
      t->yaw        = d->yaw*DAF;
      t->pitch      = d->pitch*DAF;
      t->roll       = d->roll*DAF;
      t->velocity_x = d->vel.x*DVF;
      t->velocity_y = d->vel.y*DVF;
      t->velocity_z = d->vel.z*DVF;
      t->rest_info  = (d->strafe&3) | ((d->move&3)<<2) | (((int)d->onfloor)<<4) | ((editmode ? CS_EDITING : d->state)<<5);  
      t->isbot      = d->bIsBot;
      t->seq_no     = stream_seq_nos[SV_POS];
      tuples_to_send.push_back(std::make_pair((int)SV_POS, t.dynamic_cast_to<borealis_tuple>()));
    }	  

    if(senditemstoserver) {
      
      if(!m_noitems)
	  total_items = tuple_put_items(item_list_tuples);

      // Locally spawn items.
      //putitems();
      senditemstoserver = false;
      serveriteminitdone = true;
    };


    if(ctext[0]) {
      // player chat, not flood protected for now
      NMSTL::ptr<cube_chat_tuple> t = NMSTL::ptr<cube_chat_tuple>(new cube_chat_tuple());
      t->clientnum = clientnum;
      memcpy(t->chat_text,ctext, 1024);
      tuples_to_send.push_back(std::make_pair((int)SV_TEXT, t.dynamic_cast_to<borealis_tuple>()));

      ctext[0] = 0;
    };
    

    if(!c2sinit) {
      // tell other clients who I am

	printf("Sending out SV_INITC2S\n");

	NMSTL::InetAddress local_addr(NMSTL::InetAddress::local_hostname());
	NMSTL::ptr<cube_init_tuple> t = NMSTL::ptr<cube_init_tuple>(new cube_init_tuple());
	t->clientnum = clientnum;
	memset(t->source_ip, 0, 16);
	memcpy(t->source_ip, local_addr.get_host().c_str(), local_addr.get_host().length());
	memcpy(t->player_name, player1->name, 32);
	memcpy(t->team_name, player1->team, 32);
	t->lifesequence = player1->lifesequence;
	strncpy(t->toservermap, currentmap, 128); // The string is typedefed char[260] 
	t->nextmode = nextmode;
	tuples_to_send.push_back(std::make_pair((int)SV_INITC2S, t.dynamic_cast_to<borealis_tuple>()));

	if (clientnum>=0) c2sinit = true;
    };


    if(send_position_flag && ( lastmillis-lastping > 1000)) { // was 250
      NMSTL::ptr<cube_ping_tuple> t = NMSTL::ptr<cube_ping_tuple>(new cube_ping_tuple());
      t->clientnum = clientnum;
      t->ping_type = SV_PING;
      t->lastmillis = lastmillis;
      tuples_to_send.push_back(std::make_pair((int)SV_PING, t.dynamic_cast_to<borealis_tuple>()));

      lastping = lastmillis;
    };  

    
  }

  //physically send stuff

  lastupdate = lastmillis;


  std::map<int, bool> tuple_type_found;

  //if(serveriteminitdone) loadgamerest();  // hack
  std::vector< std::pair< int, NMSTL::ptr<borealis_tuple> > >::iterator tuple_it  = tuples_to_send.begin();
  std::vector< std::pair< int, NMSTL::ptr<borealis_tuple> > >::const_iterator tuple_end = tuples_to_send.end();
  for(; tuple_it != tuple_end; tuple_it++) {
      send_cube_tuple(tuple_it->first, tuple_it->second.get());
      tuple_type_found[tuple_it->first] = true;
  }

  if (total_items > 0)
      printf("Total items on map: %d\n", total_items);

  std::vector< cube_inventory_item_tuple >::iterator items_it  = item_list_tuples.begin();
  std::vector< cube_inventory_item_tuple >::const_iterator items_end = item_list_tuples.end();
  for(; items_end != items_it; items_it++) {
      NMSTL::ptr<cube_inventory_item_tuple> t = NMSTL::ptr<cube_inventory_item_tuple>(new cube_inventory_item_tuple(*items_it));
      t->total_items = total_items;
      send_cube_tuple(SV_ITEMLIST, t.dynamic_cast_to<borealis_tuple>().get());
      tuple_type_found[SV_ITEMLIST] = true;
  }


  std::vector< std::pair< int, NMSTL::ptr<borealis_tuple> > >::iterator pending_it = client_pending_message.begin();
  std::vector< std::pair< int, NMSTL::ptr<borealis_tuple> > >::const_iterator pending_end = client_pending_message.end();

  for(; pending_it != pending_end; pending_it++) {
    send_cube_tuple(pending_it->first, pending_it->second.get());
    tuple_type_found[pending_it->first] = true;
  }

  client_pending_message.clear();

  // Update per stream seq nos.
  std::map<int, bool>::iterator bot_type_it = bot_sent_tuple.begin();
  std::map<int, bool>::iterator bot_type_end = bot_sent_tuple.end();

  for (; bot_type_it != bot_type_end; ++bot_type_it)
      tuple_type_found[bot_type_it->first] = bot_type_it->second;

  bot_sent_tuple.clear();

  std::map<int,bool>::iterator type_it = tuple_type_found.begin();
  std::map<int,bool>::iterator type_end = tuple_type_found.end();
  for (; type_it != type_end; ++type_it)
      ++stream_seq_nos[type_it->first];

}


void getb2c() // get updates from borealis stream
{
    handle_incoming_tuples();
}
