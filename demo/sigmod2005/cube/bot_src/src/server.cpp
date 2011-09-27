// server.cpp: little more than enhanced multicaster
// runs dedicated or as client coroutine

#include "cube.h" 

enum { ST_EMPTY, ST_LOCAL, ST_TCPIP };

struct client                   // server side version of "dynent" type
{
    int type;
    ENetPeer *peer;
    string hostname;
    string mapvote;
    string name;
    int modevote;
};

vector<client> clients;

string smapname;

vector<server_entity> sents;

bool notgotitems = true;        // true when map has changed and waiting for clients to send item
int mode = 0;

void restoreserverstate(vector<entity> &ents)   // hack: called from savegame code, only works in SP
{
    loopv(sents)
    {
        sents[i].spawned = ents[i].spawned;
        sents[i].spawnsecs = 0;
    }; 
};

int interm = 0, minremain = 0, mapend = 0;
bool mapreload = false;

char *serverpassword = "";

bool listenserv;
ENetHost * serverhost = NULL;
int bsend = 0, brec = 0, laststatus = 0, lastsec = 0;

#define MAXOBUF 100000

void process(ENetPacket *packet, int sender);
void multicast(ENetPacket *packet, int sender);
void disconnect_client(int n, char *reason);

void send(int n, ENetPacket *packet)
{
	if(!packet) return;
    switch(clients[n].type)
    {
        case ST_TCPIP:
        {
            enet_peer_send(clients[n].peer, 0, packet);
            bsend += packet->dataLength;
            break;
        };

        case ST_LOCAL:
            localservertoclient(packet->data, packet->dataLength);
            break;

    };
};

void send2(bool rel, int cn, int a, int b)
{
    ENetPacket *packet = enet_packet_create(NULL, 32, rel ? ENET_PACKET_FLAG_RELIABLE : 0);
    uchar *start = packet->data;
    uchar *p = start+2;
    putint(p, a);
    putint(p, b);
    *(ushort *)start = ENET_HOST_TO_NET_16(p-start);
    enet_packet_resize(packet, p-start);
    if(cn<0) process(packet, -1);
    else send(cn, packet);
    if(packet->referenceCount==0) enet_packet_destroy(packet);
};

void sendservmsg(char *msg)
{
    ENetPacket *packet = enet_packet_create(NULL, _MAXDEFSTR+10, ENET_PACKET_FLAG_RELIABLE);
    uchar *start = packet->data;
    uchar *p = start+2;
    putint(p, SV_SERVMSG);
    sendstring(msg, p);
    *(ushort *)start = ENET_HOST_TO_NET_16(p-start);
    enet_packet_resize(packet, p-start);
    multicast(packet, -1);
    if(packet->referenceCount==0) enet_packet_destroy(packet);
};

void disconnect_client(int n, char *reason)
{
    printf("disconnecting client (%s) [%s]\n", clients[n].hostname, reason);
    enet_peer_disconnect(clients[n].peer);
    clients[n].type = ST_EMPTY;
    send2(true, -1, SV_CDIS, n);
};

void resetitems() { sents.setsize(0); notgotitems = true; };

void pickup(int i, int sec, int sender)         // server side item pickup, acknowledge first client that gets it
{
    if(i>=sents.length()) return;
    if(sents[i].spawned)
    {
        sents[i].spawned = false;
        sents[i].spawnsecs = sec;
        send2(true, sender, SV_ITEMACC, i);
    };
};

void resetvotes()
{
    loopv(clients) clients[i].mapvote[0] = 0;
};

bool vote(char *map, int reqmode, int sender)
{
    strcpy_s(clients[sender].mapvote, map);
    clients[sender].modevote = reqmode;
    int yes = 0, no = 0; 
    loopv(clients) if(clients[i].type!=ST_EMPTY)
    {
        if(clients[i].mapvote[0]) { if(strcmp(clients[i].mapvote, map)==0 && clients[i].modevote==reqmode) yes++; else no++; }
        else no++;
    };
    if(yes==1 && no==0) return true;  // single player
    sprintf_sd(msg)("%s suggests %s on map %s (set map to vote)", clients[sender].name, modestr(reqmode), map);
    sendservmsg(msg);
    if(yes/(float)(yes+no) <= 0.5f) return false;
    sendservmsg("vote passed");
    resetvotes();
    return true;    
};

// server side processing of updates: does very little and most state is tracked client only
// could be extended to move more gameplay to server (at expense of lag)

void process(ENetPacket * packet, int sender)   // sender may be -1
{
    if(ENET_NET_TO_HOST_16(*(ushort *)packet->data)!=packet->dataLength)
    {
        disconnect_client(sender, "packet length");
        return;
    };
        
    uchar *end = packet->data+packet->dataLength;
    uchar *p = packet->data+2;
    char text[MAXTRANS];
    int cn = -1, type;

    while(p<end) switch(type = getint(p))
    {
        case SV_TEXT:
            sgetstr();
            break;

        case SV_INITC2S:
            sgetstr();
            strcpy_s(clients[cn].name, text);
            sgetstr();
            getint(p);
            break;

	// Added by Rick
        case SV_ADDBOT:
	    getint(p);
            sgetstr();
            //strcpy_s(clients[cn].name, text);
            sgetstr();
            getint(p);
            break;
	// End add by Rick
		        
	case SV_MAPCHANGE:
        {
            sgetstr();
            int reqmode = getint(p);
            if(reqmode<0) reqmode = 0;
            if(smapname[0] && !mapreload && !vote(text, reqmode, sender)) return;
            mapreload = false;
            mode = reqmode;
            minremain = mode&1 ? 15 : 10;
            mapend = lastsec+minremain*60;
            interm = 0;
            strcpy_s(smapname, text);
            resetitems();
            sender = -1;
            break;
        };
        
        case SV_ITEMLIST:
        {
            int n;
            while((n = getint(p))!=-1) if(notgotitems)
            {
                server_entity se = { false, 0 };
                while(sents.length()<=n) sents.add(se);
                sents[n].spawned = true;
            };
            notgotitems = false;
            break;
        };

        case SV_ITEMPICKUP:
        {
            int n = getint(p);
            int spawnsec = getint(p);
            
            if (getint(p))
                 pickup(n, spawnsec, sender);
            break;
        };

        case SV_PING:
            send2(false, cn, SV_PONG, getint(p));
            break;

        case SV_POS:
        {
            cn = getint(p);
            if(cn<0 || cn>=clients.length() || clients[cn].type==ST_EMPTY)
            {
                disconnect_client(sender, "client num");
                return;
            };
            int size = msgsizelookup(type);
            assert(size!=-1);
            loopi(size-2) getint(p);
            break;
        };

        case SV_SENDMAP:
        {
            sgetstr();
            int mapsize = getint(p);
            sendmaps(sender, text, mapsize, p);
            return;
        }
	/*
	// Added by Rick
	case SV_BOTUPDATE:
	{
	    /*
            cn = getint(p);
            if(cn<0 || cn>=clients.length() || clients[cn].type==ST_EMPTY)
            {
                disconnect_client(sender, "bot num");
                return;
            };
            int size = msgsizelookup(type);
            assert(size!=-1);
            loopi(size-2) getint(p);
            break;
        }*/
	// End add by Rick

        case SV_RECVMAP:
			send(sender, recvmap(sender));
            return;
            
        case SV_EXT:   // allows for new features that require no server updates 
        {
            for(int n = getint(p); n; n--) getint(p);
            break;
        };
		
        default:
        {
            int size = msgsizelookup(type);
            if(size==-1) { disconnect_client(sender, "tag type"); return; };
            loopi(size-1) getint(p);
        };
    };

    if(p>end) { disconnect_client(sender, "end of packet"); return; };
    multicast(packet, sender);
};

void send_welcome(int n)
{
    ENetPacket * packet = enet_packet_create (NULL, MAXTRANS, ENET_PACKET_FLAG_RELIABLE);
    uchar *start = packet->data;
    uchar *p = start+2;
    putint(p, SV_INITS2C);
    putint(p, n);
    putint(p, PROTOCOL_VERSION);
    putint(p, smapname[0]);
    sendstring(serverpassword, p);
    if(smapname[0])
    {
        putint(p, SV_MAPCHANGE);
        sendstring(smapname, p);
        putint(p, mode);
        putint(p, SV_ITEMLIST);
        loopv(sents) if(sents[i].spawned) putint(p, i);
        putint(p, -1);
    };
    *(ushort *)start = ENET_HOST_TO_NET_16(p-start);
    enet_packet_resize(packet, p-start);
    send(n, packet);
};

void multicast(ENetPacket *packet, int sender)
{
    loopv(clients)
    {
        if(i==sender) continue;
        send(i, packet);
    };
};

void localclienttoserver(ENetPacket *packet)
{
    process(packet, 0);
    if(!packet->referenceCount) enet_packet_destroy (packet);
};

client &addclient()
{
    loopv(clients) if(clients[i].type==ST_EMPTY) return clients[i];
    return clients.add();
};

void checkintermission()
{
    if(!minremain)
    {
        interm = lastsec+10;
        mapend = lastsec+1000;
    };
    send2(true, -1, SV_TIMEUP, minremain--);
};

void startintermission() { minremain = 0; checkintermission(); };

void resetserverifempty()
{
    loopv(clients) if(clients[i].type!=ST_EMPTY) return;
    clients.setsize(0);
    smapname[0] = 0;
    resetvotes();
    resetitems();
    mode = 0;
    mapreload = false;
    minremain = 10;
    mapend = lastsec+minremain*60;
    interm = 0;
};

int nonlocalclients = 0;

void serverslice(int seconds, unsigned int timeout)   // main server update, called from cube main loop in sp, or dedicated server loop
{
    loopv(sents)        // spawn entities when timer reached
    {
        if(sents[i].spawnsecs && (sents[i].spawnsecs -= seconds-lastsec)<=0)
        {
            sents[i].spawnsecs = 0;
            sents[i].spawned = true;
            send2(true, -1, SV_ITEMSPAWN, i);
        };
    };
    
    lastsec = seconds;
    
    if((mode>1 || (mode==0 && nonlocalclients)) && seconds>mapend-minremain*60) checkintermission();
    if(interm && seconds>interm)
    {
        interm = 0;
        loopv(clients) if(clients[i].type!=ST_EMPTY)
        {
            send2(true, i, SV_MAPRELOAD, 0);    // ask a client to trigger map reload
            mapreload = true;
            break;
        };
    };

    resetserverifempty();
    
    if(!listenserv) return;     // below is network only

	int numplayers = 0;
	loopv(clients) if(clients[i].type!=ST_EMPTY) ++numplayers;
	serverms(mode, numplayers, minremain, smapname, seconds);

    if(seconds-laststatus>60)   // display bandwidth stats, useful for server ops
    {
        nonlocalclients = 0;
        loopv(clients) if(clients[i].type==ST_TCPIP) nonlocalclients++;
        laststatus = seconds;     
        if(nonlocalclients || bsend || brec) printf("status: %d remote clients, %.1f send, %.1f rec (K/sec)\n", nonlocalclients, bsend/60.0f/1024, brec/60.0f/1024);
        bsend = brec = 0;
    };

    ENetEvent event;
    if(enet_host_service(serverhost, &event, timeout) > 0)
    switch(event.type)
    {
        case ENET_EVENT_TYPE_CONNECT:
        {
            client &c = addclient();
            c.type = ST_TCPIP;
            c.peer = event.peer;
            c.peer->data = (void *)(&c-&clients[0]);
            char hn[1024];
            strcpy_s(c.hostname, (enet_address_get_host(&c.peer->address, hn, sizeof(hn))==0) ? hn : "localhost");
            printf("client connected (%s)\n", c.hostname);
            send_welcome(&c-&clients[0]); 
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE:
            brec += event.packet->dataLength;
            process(event.packet, (int)event.peer->data); 
            if(event.packet->referenceCount==0) enet_packet_destroy(event.packet);
            break;

        case ENET_EVENT_TYPE_DISCONNECT: 
            if((int)event.peer->data<0) break;
            printf("disconnected client (%s)\n", clients[(int)event.peer->data].hostname);
            clients[(int)event.peer->data].type = ST_EMPTY;
            send2(true, -1, SV_CDIS, (int)event.peer->data);
            event.peer->data = (void *)-1;
            break;
    };
};

void cleanupserver()
{
    if(serverhost) enet_host_destroy(serverhost);
};

void localdisconnect()
{
    loopv(clients) if(clients[i].type==ST_LOCAL) clients[i].type = ST_EMPTY;
};

void localconnect()
{
    client &c = addclient();
    c.type = ST_LOCAL;
    strcpy_s(c.hostname, "local");
    send_welcome(&c-&clients[0]); 
};

void initserver(bool dedicated, bool l, int uprate, char *sdesc, char *ip, char *master, char *passwd)
{
    serverpassword = passwd;
	servermsinit(master ? master : "wouter.fov120.com/cube/masterserver/", sdesc, l);
    
    if(listenserv = l)
    {
        ENetAddress address = { ENET_HOST_ANY, CUBE_SERVER_PORT };
        if(*ip && enet_address_set_host(&address, ip)<0) printf("WARNING: server ip not resolved");
        serverhost = enet_host_create(&address, MAXCLIENTS, 0, uprate);
        if(!serverhost) fatal("could not create server host\n");
        loopi(MAXCLIENTS) serverhost->peers[i].data = (void *)-1;
    };

    resetserverifempty();

    if(dedicated)       // do not return, this becomes main loop
    {
        #ifdef WIN32
        SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
        #endif
        printf("dedicated server started, waiting for clients...\nCtrl-C to exit\n\n");
        atexit(cleanupserver);
        atexit(enet_deinitialize);
        for(;;) serverslice(/*enet_time_get_sec()*/time(NULL), 5);
    };
};
