// client processing of the incoming network stream

#include <vector>
#include <map>
#include <NMSTL/ptr>
#include "tuples.h"
#include "netio.h"

#include "cube.h"

#include <time.h>

extern int clientnum;
extern bool c2sinit, senditemstoserver, listenserv;
extern string toservermap;
extern string currentmap;
extern string clientpassword;

extern netio_state netstate;
extern std::vector< std::pair<int, NMSTL::ptr<borealis_tuple> > > client_pending_message;
bool mapchanged = false;
bool bothost = false;

std::map<int, bool> valid_items;

void neterr(char *s)
{
    conoutf("illegal network message (%s)", (int)s);
    //disconnect();
};

void changemapserv(char *name, int mode)        // forced map change from the server
{
    strncpy(currentmap, name, _MAXDEFSTR);
    gamemode = mode;
    load_world(name);
};

void changemap(char *name)                      // request map change, server may ignore
{
    strcpy_s(toservermap, name);
};

// update the position of other clients in the game in our world
// don't care if he's in the scenery or other players,
// just don't overlap with our client

void updatepos(dynent *d)
{
    const float r = player1->radius+d->radius;
    const float dx = player1->o.x-d->o.x;
    const float dy = player1->o.y-d->o.y;
    const float dz = player1->o.z-d->o.z;
    const float rz = player1->aboveeye+d->eyeheight;
    const float fx = (float)fabs(dx), fy = (float)fabs(dy), fz = (float)fabs(dz);
    if(fx<r && fy<r && fz<rz && d->state!=CS_DEAD)
    {
        if(fx<fy) d->o.y += dy<0 ? r-fy : -(r-fy);  // push aside
        else      d->o.x += dx<0 ? r-fx : -(r-fx);
    };
    int lagtime = lastmillis-d->lastupdate;
    if(lagtime)
    {
        d->plag = (d->plag*5+lagtime)/6;
        d->lastupdate = lastmillis;
    };
};


//////////////////////////////////////////////////
//
// Tuple handlers
//------------------------------------------------

void handle_init_tuple(const borealis_tuple* bt)
{
  const cube_init_tuple* t = static_cast<const cube_init_tuple*>(bt);

  dynent* d;
  int cn;

  if (clientnum < 0) {
    //HACK should use a filter box on query network but we need to put out the MAP from the server's table
    cn = t->clientnum;	

    //No need to check protocol versions
    toservermap[0] = 0;
    clientnum = cn;   

    // we are now fully connected
    //Note since MAPCHANGE is sent first server already has valid map
    char buffer[256] ;
    strncpy(buffer, t->toservermap, 256);
    changemapserv(buffer, t->nextmode);
    mapchanged = true;
    c2sinit = true;
    d = getclient(cn);	
    players[clientnum] = NULL;
    free(d);

    netstate.game_subs->game_subscribe();
  }
}

void handle_player_init_tuple(const borealis_tuple* bt)
{
  const cube_init_tuple* t = static_cast<const cube_init_tuple*>(bt);

  if(t->clientnum == -1) {
    // TODO: make sure this only happens at the bot owner
    if (bothost)
      BotManager.LetBotsUpdateStats();
    // New Player
    c2sinit = false;   
    sleep(1);
  } else {
    // Name change or new team.
    dynent* d = getclient(t->clientnum);
    strcpy_s(d->name, t->player_name );
    strcpy_s(d->team, t->team_name);
    d->lifesequence = t->lifesequence;
    //printf("client %d setting name %s  team %s \n",ct->clientnum, d->name, d->team);
  }
}


void handle_disconnect_tuple(const borealis_tuple* bt)
{
  const cube_disconnect_tuple* t = static_cast<const cube_disconnect_tuple*>(bt);
  int cn, isbot;
  dynent* d;

  cn = t->clientnum;
  isbot = t->isbot;
  if (isbot) {
      d = getbot(cn);
      if (!d) return;
	
      conoutf("bot %s disconnected", (int)(d->name[0] ? d->name : "[incompatible client]"));
      delete d->pBot;
      zapdynent(bots[cn]);
      bots.remove(cn);
  }
  else {
      d = getclient(cn);
      if(!d) return;
      conoutf("player %s disconnected", (int)(d->name[0] ? d->name : "[incompatible client]"));
      zapdynent(players[cn]);
  }
}


void handle_position_tuple(const borealis_tuple* bt)
{
  const cube_position_tuple* t = static_cast<const cube_position_tuple*>(bt);

  int cn = t->clientnum;
  bool is_bot = t->isbot;

  dynent* d = (is_bot? getbot(cn) : getclient(cn));

  if(!d) return;
  d->o.x   = t->position_x/DMF;
  d->o.y   = t->position_y/DMF;
  d->o.z   = t->position_z/DMF;
  d->yaw   = t->yaw/DAF;
  d->pitch = t->pitch/DAF;
  d->roll  = t->roll/DAF;
  d->vel.x = t->velocity_x/DVF;
  d->vel.y = t->velocity_y/DVF;
  d->vel.z = t->velocity_z/DVF;
  int f = t->rest_info;
  d->strafe = (f&3)==3 ? -1 : f&3;
  f >>= 2; 
  d->move = (f&3)==3 ? -1 : f&3;
  d->onfloor = (f>>2)&1;
  int state = f>>3;
  if(state==CS_DEAD && d->state!=CS_DEAD) d->lastaction = lastmillis;
  d->state = state;

  if (is_bot) {
      if (!d->bIsBot)
	  d->bIsBot = true;
  }

  if(!demoplayback) updatepos(d);
}


void handle_inventory_item_tuple(const borealis_tuple* bt)
{
  const cube_inventory_item_tuple* t = static_cast<const cube_inventory_item_tuple*>(bt);

  if(mapchanged) {
      senditemstoserver = false;
      mapchanged = false;
      resetspawns();
      valid_items.clear();
  }

  if (valid_items.size() < t->total_items) {
      std::map<int,bool>::iterator item_it = valid_items.find(t->item_index);
      if (item_it == valid_items.end()) {
	  setspawn(t->item_index, true);
	  valid_items[t->item_index] = true;
      }
  }
}


void handle_item_pickup_tuple(const borealis_tuple* bt)
{
  const cube_item_pickup_tuple* t = static_cast<const cube_item_pickup_tuple*>(bt);
  setspawn(t->item_index, false);
}


void handle_item_spawn_tuple(const borealis_tuple* bt)
{
  const cube_item_spawn_tuple* t = static_cast<const cube_item_spawn_tuple*>(bt);

  int i = t->item_index;
  setspawn(i, true);
  if(i>=ents.length()) return;
  vec v = { ents[i].x, ents[i].y, ents[i].z };

#ifdef USE_SOUND
  playsound(S_ITEMSPAWN, &v); 
#endif
}


void handle_item_acc_tuple(const borealis_tuple* bt)
{
  const cube_item_acc_tuple* t = static_cast<const cube_item_acc_tuple*>(bt);

  realpickup(t->item_index, player1);
}


void handle_map_change_tuple(const borealis_tuple* bt)
{
  const cube_map_change_tuple* t = static_cast<const cube_map_change_tuple*>(bt);

  char buffer[256] ;
  strncpy(buffer, t->toservermap, 256);
  printf("MAP Change recieved...%s\n", buffer);
  changemapserv(buffer, t->nextmode);
  mapchanged = true;
}


void handle_map_reload_tuple(const borealis_tuple* bt)
{
  const cube_map_reload_tuple* t = static_cast<const cube_map_reload_tuple*>(bt);

  sprintf_sd(nextmapalias)("nextmap_%s", getclientmap());
  char *map = getalias(nextmapalias);     // look up map in the cycle
  changemap(map ? map : getclientmap());
}


void handle_mode_tuple(const borealis_tuple* bt)
{
  const cube_mode_tuple* t = static_cast<const cube_mode_tuple*>(bt);
  nextmode = t->nextmode;
}


void handle_timeup_tuple(const borealis_tuple* bt)
{
  const cube_timeup_tuple* t = static_cast<const cube_timeup_tuple*>(bt);
  timeupdate(t->time_remain);
}


void handle_recvmap_tuple(const borealis_tuple* bt)
{
  //const cube_recvmap_tuple* t = static_cast<const cube_recvmap_tuple*>(bt);
 
  //sgetstr();
  //conoutf("received map \"%s\" from server, reloading..", (int)&text);
  //int mapsize = getint(p);
  //writemap(text, mapsize, p);
  //p += mapsize;
  //changemapserv(text, gamemode);
}


void handle_shot_tuple(const borealis_tuple* bt)
{
  const cube_shot_tuple* t = static_cast<const cube_shot_tuple*>(bt);

  dynent* d = getclient(t->clientnum);
			
  int gun = t->gun_number;
  vec s, e;

  s.x = t->from_x/DMF;
  s.y = t->from_y/DMF;
  s.z = t->from_z/DMF;
  e.x = t->to_x/DMF;
  e.y = t->to_y/DMF;
  e.z = t->to_z/DMF;
  if(gun==GUN_SG) createrays(s, e);
  shootv(gun, s, e, d);
}


void handle_damage_tuple(const borealis_tuple* bt)
{
  const cube_damage_tuple* t = static_cast<const cube_damage_tuple*>(bt);

  int cn = t->clientnum;
  int target = t->target;
  int damage = t->damage;
  int ls = t->lifesequence;
  bool client_is_bot = t->client_isbot;
  bool target_is_bot = t->target_isbot;

  //bool dmgbybot = t->isbot;

  dynent* d = (client_is_bot? getbot(cn) : getclient(cn));
  dynent* a = (target_is_bot? getbot(target) : getclient(target));

  if (target==clientnum && !target_is_bot)
  {
    if(ls==player1->lifesequence) selfdamage(damage, cn, d);
  }
  else if(!target_is_bot)
  {
#ifdef USE_SOUND
      playsound(S_PAIN1+rnd(5), &d->o); 
#endif
  }
  else
  {
      if (d && a)
      {
	  // Do we know the bot info? if so we are the host...
	  if (a->pBot) a->pBot->BotPain(damage, d);
      }
#ifdef USE_SOUND
      playsound(S_PAIN1+rnd(5), &a->o);
#endif
  }
}


void handle_frags_tuple(const borealis_tuple* bt)
{
  const cube_frags_tuple* t = static_cast<const cube_frags_tuple*>(bt);

  int cn = t->clientnum;
  bool is_bot = t->isbot;
  dynent *d = (is_bot ? getbot(cn) : players[cn]);
  if (d)
      d->frags = t->frags;
}


void handle_death_tuple(const borealis_tuple* bt)
{
  const cube_death_tuple* t = static_cast<const cube_death_tuple*>(bt);

  dynent* d;
  int cn = t->clientnum;
  int actor = t->actor;
  int client_is_bot = t->client_isbot;
  int killed_by_bot = t->actor_isbot;

  d = (client_is_bot ? getbot(cn) : getclient(cn));

  if (actor==cn)
  {
      conoutf("%s suicided", (int)d->name);
  }
  else if ((actor==clientnum) && !killed_by_bot)
  {
      int frags;
      if(isteam(player1->team, d->team))
      {
	  frags = -1;
	  conoutf("you fragged a teammate (%s)", (int)d->name);
      }
      else
      {
	  frags = 1;
	  conoutf("you fragged %s", (int)d->name);
      };
      //addmsg(1, 2, SV_FRAGS, player1->frags += frags);
      player1->frags += frags;

      NMSTL::ptr<cube_frags_tuple> ct = NMSTL::ptr<cube_frags_tuple>(new cube_frags_tuple());
      ct->clientnum = getclientnum();
      ct->frags = player1->frags;
      ct->isbot = 0;
      client_pending_message.push_back(std::make_pair((int)SV_FRAGS, ct.dynamic_cast_to<borealis_tuple>()));
  }
  else
  {
      dynent* a = (killed_by_bot? getbot(actor) : (actor == -1? getclient(cn) : getclient(actor)));
      if (a && d)
      {
	  if(isteam(a->team, d->name)) {
	      conoutf("%s fragged his teammate (%s)", (int)a->name, (int)d->name);
	  }
	  else {
	      conoutf("%s fragged %s", (int)a->name, (int)d->name);
	  };
      };
  }

#ifdef USE_SOUND
  playsound(S_DIE1+rnd(2), &d->o);
#endif

  d->lifesequence++;
  d->deaths++;
}


void handle_sound_tuple(const borealis_tuple* bt)
{
  const cube_sound_tuple* t = static_cast<const cube_sound_tuple*>(bt);

#ifdef USE_SOUND
  dynent *d = (t->isbot? getbot(t->clientnum) : getclient(t->clientnum));
  if (d)
      playsound(t->playsound_n, &d->o);
#endif
}


void handle_chat_tuple(const borealis_tuple* bt)
{
  const cube_chat_tuple* t = static_cast<const cube_chat_tuple*>(bt);

  int cn = t->clientnum;
  if (cn >= 0) {
    dynent* d  = getclient(cn);
    if(!d) return;
    conoutf("%s:\f %s", (int)d->name, (int)t->chat_text);  
  }
}

void handle_servmsg_tuple(const borealis_tuple* bt)
{
  const cube_servmsg_tuple* t = static_cast<const cube_servmsg_tuple*>(bt);
  
  conoutf("%s", (int)t->text);
}

void handle_ping_tuple(const borealis_tuple* bt)
{
  const cube_ping_tuple* t = static_cast<const cube_ping_tuple*>(bt);

  if (t->ping_type == SV_CLIENTPING) {

    int cn = t->clientnum;
    players[cn]->ping = t->lastmillis;

  } else if (t->ping_type == SV_PONG) {

    player1->ping = (player1->ping*5+lastmillis-t->lastmillis)/6;
    NMSTL::ptr<cube_ping_tuple> ct = NMSTL::ptr<cube_ping_tuple>(new cube_ping_tuple);
    ct->clientnum = clientnum;
    ct->ping_type = SV_CLIENTPING;
    ct->lastmillis = player1->ping;
    client_pending_message.push_back(std::make_pair((int)SV_CLIENTPING, ct.dynamic_cast_to<borealis_tuple>()));
    
  } else {
      printf("Received invalid ping tuple of type %d\n", t->ping_type);
  }
}


void handle_edit_tuple(const borealis_tuple* bt)
{
  const cube_edit_tuple* t = static_cast<const cube_edit_tuple*>(bt);

  int x  = t->selection_x;
  int y  = t->selection_y;
  int xs = t->selection_xs;
  int ys = t->selection_ys;
  int v  = t->selection_type; 

  block b = { x, y, xs, ys };

  int etype = t->edit_type;

  switch(etype)
    {
    case SV_EDITH: editheightxy(v!=0, t->amount, b); break;
    case SV_EDITT: edittexxy(v, t->lasttex, b); break;
    case SV_EDITS: edittypexy(v, b); break;
    case SV_EDITD: setvdeltaxy(v, b); break;
    case SV_EDITE: editequalisexy(v!=0, b); break; 
    };
}


void handle_edit_ent_tuple(const borealis_tuple* bt)
{
  // coop edit of ent
  const cube_edit_ent_tuple* t = static_cast<const cube_edit_ent_tuple*>(bt);

  int i = t->entity_length;
  while(ents.length()<=i) ents.add().type = NOTUSED;
  ents[i].type = t->entity_type;
  ents[i].x = t->entity_x;
  ents[i].y = t->entity_y;
  ents[i].z = t->entity_z;
  ents[i].attr1 = t->entity_attribute1;
  ents[i].attr2 = t->entity_attribute2;
  ents[i].attr3 = t->entity_attribute3;
  ents[i].attr4 = t->message;
  ents[i].spawned = false;
  //if(ents[i].type==LIGHT || to==LIGHT) calclight();
}


//////////////////////////////////////////////////
//
// Additional handlers for bots
//------------------------------------------------

void handle_bot_init_tuple(const borealis_tuple* bt)
{
    const cube_bot_init_tuple* t = static_cast<const cube_bot_init_tuple*>(bt);

    dynent* d = get_pending_bot(t->pending_clientnum);
    while(t->clientnum>=bots.length()) bots.add(NULL);
    bots[t->clientnum] = d;
    pending_bots[t->pending_clientnum] = NULL;
}

void handle_add_bot_tuple(const borealis_tuple* bt)
{
    const cube_add_bot_tuple* t = static_cast<const cube_add_bot_tuple*>(bt);
    dynent* d = getbot(t->clientnum);

    if (!d) return;
		
    if(d->name[0]) {
	// already connected
	if(strcmp(d->name, t->player_name))
	    conoutf("%s is now known as %s", (int)d->name, (int)&t->player_name);
    }
    else {
	// new client
	conoutf("connected: %s", (int)&t->player_name);
    }

    strcpy_s(d->name, t->player_name);
    strcpy_s(d->team, t->team_name);
    d->lifesequence = t->lifesequence;
    d->bIsBot = true;
    d->pBot = NULL;
}


//////////////////////////////////////////////////


void localservertoclient(uchar *buf, int len)   // processes any updates from the server
{
    if(ENET_NET_TO_HOST_16(*(ushort *)buf)!=len) neterr("packet length");
    incomingdemodata(buf, len);
           
    uchar *end = buf+len;
    uchar *p = buf+2;
    char text[MAXTRANS];
    int cn = -1, type;
    dynent *d = NULL;
    bool mapchanged = false;

    while(p<end) switch(type = getint(p))
    {
        case SV_INITS2C:                    // welcome messsage from the server
        {
            cn = getint(p);
            int prot = getint(p);
            if(prot!=PROTOCOL_VERSION)
            {
                conoutf("you are using a different game protocol (you: %d, server: %d)", PROTOCOL_VERSION, prot);
                disconnect();
                return;
            };
            toservermap[0] = 0;
            clientnum = cn;                 // we are now fully connected
            if(!getint(p)) strcpy_s(toservermap, getclientmap());   // we are the first client on this server, set map
            sgetstr();
            if(text[0] && strcmp(text, clientpassword))
            {
                conoutf("you need to set the correct password to join this server!");
                disconnect();
                return;
            };
            break;
        };

        case SV_POS:                        // position of another client
        {
            cn = getint(p);
            d = getclient(cn);
            if(!d) return;
            d->o.x   = getint(p)/DMF;
            d->o.y   = getint(p)/DMF;
            d->o.z   = getint(p)/DMF;
            d->yaw   = getint(p)/DAF;
            d->pitch = getint(p)/DAF;
            d->roll  = getint(p)/DAF;
            d->vel.x = getint(p)/DVF;
            d->vel.y = getint(p)/DVF;
            d->vel.z = getint(p)/DVF;
            int f = getint(p);
            d->strafe = (f&3)==3 ? -1 : f&3;
            f >>= 2; 
            d->move = (f&3)==3 ? -1 : f&3;
            d->onfloor = (f>>2)&1;
            int state = f>>3;
            if(state==CS_DEAD && d->state!=CS_DEAD) d->lastaction = lastmillis;
            d->state = state;
            if(!demoplayback) updatepos(d);
            break;
        };

        case SV_SOUND:
            playsound(getint(p), &d->o);
            break;

        // Added by Rick
        case SV_BOTSOUND:
        {
             int Sound = getint(p);
             dynent *pBot = getbot(getint(p));
            
             if (pBot)
                  playsound(Sound, &pBot->o);
                 
             break;
        }
        // End add by Rick
                
        case SV_TEXT:
            sgetstr();
            conoutf("%s:\f %s", (int)d->name, (int)&text); 
            break;

        case SV_MAPCHANGE:     
            sgetstr();
            changemapserv(text, getint(p));
            mapchanged = true;
            break;
        
        case SV_ITEMLIST:
        {
            int n;
            if(mapchanged) { senditemstoserver = false; resetspawns(); };
            while((n = getint(p))!=-1) if(mapchanged) setspawn(n, true);
            break;
        };

        case SV_MAPRELOAD:          // server requests next map
        {
            getint(p);
            sprintf_sd(nextmapalias)("nextmap_%s", getclientmap());
            char *map = getalias(nextmapalias);     // look up map in the cycle
            changemap(map ? map : getclientmap());
            break;
        };

        case SV_INITC2S:            // another client either connected or changed name/team
        {
            sgetstr();
            if(d->name[0])          // already connected
            {
                if(strcmp(d->name, text))
                    conoutf("%s is now known as %s", (int)d->name, (int)&text);
            }
            else                    // new client
            {
                // Added by Rick: If we are the host("the bot owner"), tell the bots to update their stats
                if (listenserv)
                    BotManager.LetBotsUpdateStats();
                // End add by Rick

                c2sinit = false;    // send new players my info again 
                conoutf("connected: %s", (int)&text);
            }; 
            strcpy_s(d->name, text);
            sgetstr();
            strcpy_s(d->team, text);
            d->lifesequence = getint(p);
            break;
        };

	// Added by Rick
        case SV_ADDBOT:            // another client either connected or changed name/team
        {
	    d = getbot(getint(p));
	    if (!d)
	    	break;
		
            sgetstr();
            if(d->name[0])          // already connected
            {
                if(strcmp(d->name, text))
                    conoutf("%s is now known as %s", (int)d->name, (int)&text);
            }
            else                    // new client
            {
                //c2sinit = false;    // send new players my info again 
                conoutf("connected: %s", (int)&text);
            }; 
            strcpy_s(d->name, text);
            sgetstr();
            strcpy_s(d->team, text);
            d->lifesequence = getint(p);
            d->bIsBot = true;
            d->pBot = NULL;
            break;
        };
	// End add by Rick
	
        case SV_CDIS:
            cn = getint(p);
	    d = getclient(cn);
	
	    if (!d)
	    	break;
	
            conoutf("player %s disconnected", (int)(d->name[0] ? d->name : "[incompatible client]")); 
            zapdynent(players[cn]);
            break;

        // Added by Rick
        case SV_BOTDIS:
            cn = getint(p);
    	    d = getbot(cn);
	
	    if (!d)
	    	break;
	
    	    conoutf("bot %s disconnected", (int)(d->name[0] ? d->name : "[incompatible client]"));
	    delete d->pBot;
            zapdynent(bots[cn]);
	    bots.remove(cn);
            break;
	// End add by Rick
	
	case SV_SHOT:
        {
            int gun = getint(p);
            vec s, e;
            s.x = getint(p)/DMF;
            s.y = getint(p)/DMF;
            s.z = getint(p)/DMF;
            e.x = getint(p)/DMF;
            e.y = getint(p)/DMF;
            e.z = getint(p)/DMF;
            if(gun==GUN_SG) createrays(s, e);
            shootv(gun, s, e, d);
            break;
        };

        case SV_DAMAGE:             
        {
            int target = getint(p);
            int damage = getint(p);
            int ls = getint(p);
            bool dmgbybot = getint(p);
            // Modified by Rick: Check if a bot damaged this player
            if(target==clientnum)
            {
                 dynent *a = (dmgbybot) ? getbot(target) : d;
                 if(ls==player1->lifesequence)
                      selfdamage(damage, cn, a);
            }
            else playsound(S_PAIN1+rnd(5), &getclient(target)->o);
            break;
        };

	// Added by Rick
        case SV_CLIENT2BOTDMG:
	case SV_BOT2BOTDMG:
        {
            int target = getint(p);
            int damage = getint(p);
            int damager = getint(p);
	    d = getbot(target);
	    
	    dynent *a;
	    if (damager == -1)
	    {
	    	// HACK! if the local client who sended the message is the damager, its -1
		a = getclient(cn);
	    }   
	    else if (type == SV_CLIENT2BOTDMG)
	    	a = getclient(damager);
	    else
	    	a = getbot(damager);
		
	    if (d && a)
	    {
	    	// Do we know the bot info? if so we are the host...
		if (d->pBot) d->pBot->BotPain(damage, a);
	    }
            playsound(S_PAIN1+rnd(5), &getbot(target)->o);
            break;
        }
	// End add by Rick

	case SV_DIED:
        {
            int actor = getint(p);
	    int KilledByBot = getint(p);
	    
            if(actor==cn)
            {
                conoutf("%s suicided", (int)d->name);
            }
	    else if(actor==clientnum)
            {
                int frags;
                if(isteam(player1->team, d->team))
                {
                    frags = -1;
                    conoutf("you fragged a teammate (%s)", (int)d->name);
                }
                else
                {
                    frags = 1;
                    conoutf("you fragged %s", (int)d->name);
                };
                addmsg(1, 2, SV_FRAGS, player1->frags += frags);
            }
            else
            {
	    	// Modified by Rick
                //dynent *a = getclient(actor);
		
		dynent *a;
		
		// Killed by a bot?
		if (KilledByBot)
		    a = getbot(actor);
		else
		    a = getclient(actor);
		   
                if(a)
                {
                    if(isteam(a->team, d->name))
                    {
                        conoutf("%s fragged his teammate (%s)", (int)a->name, (int)d->name);
                    }
                    else
                    {
                        conoutf("%s fragged %s", (int)a->name, (int)d->name);
                    };
                };
            };
            playsound(S_DIE1+rnd(2), &d->o);
            d->lifesequence++;
            d->deaths++; // Added by Rick
            break;
        };
	
	// Added by Rick
	case SV_BOTDIED:
        {
            int actor = getint(p);
	    int killer = getint(p);
            bool KilledByABot = getint(p);
	    
            if(actor==killer)
            {
                conoutf("%s suicided", (int)d->name);
            }
	    else if((killer==clientnum) && !KilledByABot)
            {
                int frags;
		d = getbot(actor);
                if(isteam(player1->team, d->team))
                {
                    frags = -1;
                    conoutf("you fragged a teammate (%s)", (int)d->name);
                }
                else
                {
                    frags = 1;
                    conoutf("you fragged %s", (int)d->name);
                };
                addmsg(1, 2, SV_FRAGS, player1->frags += frags);
            }            	    
	    else
	    {
	    	dynent *a = getbot(actor);
                if (KilledByABot)
                     d = getbot(killer);
                else if (killer == -1)
                    // if killer = -1, 'a player1' sended the message(hack)
                     d = getclient(cn);
                else
		     d = getclient(killer);
                     
                if(a && d)
                {
                    if(isteam(a->team, d->name))
                    {
                        conoutf("%s fragged his teammate (%s)", (int)d->name, (int)a->name);
                    }
                    else
                    {
                        conoutf("%s fragged %s", (int)d->name, (int)a->name);
                    }
                }
	    }
            
	    d = getbot(actor);
	    playsound(S_DIE1+rnd(2), &d->o);
            d->lifesequence++;
            d->deaths++;
            break;
        };	
	// End add by Rick

        case SV_FRAGS:
            players[cn]->frags = getint(p);
            break;

	// Added by Rick
        case SV_BOTFRAGS:
	    d = getbot(getint(p));
	    if (d)
            	d->frags = getint(p);
            break;
	// End add by Rick
        
	case SV_ITEMPICKUP:
            setspawn(getint(p), false);
            getint(p);
            getint(p); // Added by Rick
            break;

        case SV_ITEMSPAWN:
        {
            int i = getint(p);
            setspawn(i, true);
            if(i>=ents.length()) break;
            vec v = { ents[i].x, ents[i].y, ents[i].z };
            playsound(S_ITEMSPAWN, &v); 
            break;
        };

        case SV_ITEMACC:            // server acknowledges that I picked up this item
            realpickup(getint(p), player1);
            break;

        case SV_EDITH:              // coop editing messages, should be extended to include all possible editing ops
        case SV_EDITT:
        case SV_EDITS:
        case SV_EDITD:
        case SV_EDITE:
        {
            int x  = getint(p);
            int y  = getint(p);
            int xs = getint(p);
            int ys = getint(p);
            int v  = getint(p);
            block b = { x, y, xs, ys };
            switch(type)
            {
                case SV_EDITH: editheightxy(v!=0, getint(p), b); break;
                case SV_EDITT: edittexxy(v, getint(p), b); break;
                case SV_EDITS: edittypexy(v, b); break;
                case SV_EDITD: setvdeltaxy(v, b); break;
                case SV_EDITE: editequalisexy(v!=0, b); break;
            };
            break;
        };

        case SV_EDITENT:            // coop edit of ent
        {
            int i = getint(p);
            while(ents.length()<=i) ents.add().type = NOTUSED;
            int to = ents[i].type;
            ents[i].type = getint(p);
            ents[i].x = getint(p);
            ents[i].y = getint(p);
            ents[i].z = getint(p);
            ents[i].attr1 = getint(p);
            ents[i].attr2 = getint(p);
            ents[i].attr3 = getint(p);
            ents[i].attr4 = getint(p);
            ents[i].spawned = false;
            if(ents[i].type==LIGHT || to==LIGHT) calclight();
            break;
        };

        case SV_PING:
            getint(p);
            break;

        case SV_PONG: 
            addmsg(0, 2, SV_CLIENTPING, player1->ping = (player1->ping*5+lastmillis-getint(p))/6);
            break;

        case SV_CLIENTPING:
            players[cn]->ping = getint(p);
            break;

        case SV_GAMEMODE:
            nextmode = getint(p);
            break;

        case SV_TIMEUP:
            timeupdate(getint(p));
            break;

        case SV_RECVMAP:
        {
            sgetstr();
            conoutf("received map \"%s\" from server, reloading..", (int)&text);
            int mapsize = getint(p);
            writemap(text, mapsize, p);
            p += mapsize;
            changemapserv(text, gamemode);
            break;
        };
        
        case SV_SERVMSG:
            sgetstr();
            conoutf("%s", (int)text);
            break;

	// Added by Rick
	case SV_BOTUPDATE:
	{
            cn = getint(p);
            d = getbot(cn);
            if(!d) return;
            d->o.x   = getint(p)/DMF;
            d->o.y   = getint(p)/DMF;
            d->o.z   = getint(p)/DMF;
            d->yaw   = getint(p)/DAF;
            d->pitch = getint(p)/DAF;
            d->roll  = getint(p)/DAF;
            d->vel.x = getint(p)/DVF;
            d->vel.y = getint(p)/DVF;
            d->vel.z = getint(p)/DVF;
            int f = getint(p);
            d->strafe = (f&3)==3 ? -1 : f&3;
            f >>= 2; 
            d->move = (f&3)==3 ? -1 : f&3;
            d->onfloor = (f>>2)&1;
            int state = f>>3;
            if(state==CS_DEAD && d->state!=CS_DEAD) d->lastaction = lastmillis;
            d->state = state;
	    
	    if (!d->bIsBot)
	    	d->bIsBot = true;
		
            if(!demoplayback) updatepos(d);
            break;
	}
	// End add by Rick
        case SV_EXT:        // so we can messages without breaking previous clients/servers, if necessary
        {
            for(int n = getint(p); n; n--) getint(p);
            break;
        };

        default:
            neterr("type");
            return;
    };
};
