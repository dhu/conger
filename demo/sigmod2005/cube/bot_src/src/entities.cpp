// entities.cpp: map entity related functions (pickup etc.)

#include <vector>
#include <NMSTL/ptr>

#include "tuples.h"
#include "cube.h"

extern int clientnum;
extern std::vector< std::pair< int, NMSTL::ptr<borealis_tuple> > > client_pending_message;
extern std::vector<bool> selected_items;

vector<entity> ents;

char *entmdlnames[] = 
{
	"shells", "bullets", "rockets", "rrounds", "health", "boost",
	"g_armour", "y_armour", "quad",	"teleporter",     
};

int triggertime = 0;

void renderent(entity &e, char *mdlname, float z, float yaw, int frame = 0, int numf = 1, int basetime = 0, float speed = 10.0f, bool selected = false)
{
    entity* e_p = &e;
    rendermodel(mdlname, frame, numf, 0, 1.1f, e.x, z+S(e.x, e.y)->floor, e.y, yaw, 0, false, 1.0f, speed, 0, basetime, selected);
};

void renderentities()
{
    if (selected_items.size() < ents.length())
	selected_items.resize(ents.length());

    
    if(lastmillis>triggertime+1000) triggertime = 0;
    loopv(ents)
    {
        entity &e = ents[i];
        if(e.type==MAPMODEL)
        {
            mapmodelinfo &mmi = getmminfo(e.attr2);
            if(!&mmi) continue;
	    rendermodel(mmi.name, 0, 1, e.attr4, (float)mmi.rad, e.x, (float)S(e.x, e.y)->floor+mmi.zoff+e.attr3, e.y, (float)((e.attr1+7)-(e.attr1+7)%15), 0, false, 1.0f, 10.0f, mmi.snap);
        }
        else
        {
            if(OUTBORD(e.x, e.y)) continue;
            if(e.type!=CARROT)
            {
		//printf("Render entity %d spawned %s\n", i, (e.spawned? "true" : "false"));
		if(!e.spawned && e.type!=TELEPORT) continue;
		if(e.type<I_SHELLS || e.type>TELEPORT) continue;
		//printf("Rendering entity %d %s\n", i, entmdlnames[e.type-I_SHELLS]);
		renderent(e, entmdlnames[e.type-I_SHELLS], (float)(1+sin(lastmillis/100.0+e.x+e.y)/20), lastmillis/10.0f, 0, 1, 0, 10.0f, selected_items[i]);
            }
	    else switch(e.attr2)
            {			
	    case 1:
	    case 3:
		continue;
					
	    case 2: 
	    case 0:
		if(!e.spawned) continue;
		renderent(e, "carrot", (float)(1+sin(lastmillis/100.0+e.x+e.y)/20), lastmillis/(e.attr2 ? 1.0f : 10.0f));
		break;
					
	    case 4: renderent(e, "switch2", 3,      (float)e.attr3*90, (!e.spawned && !triggertime) ? 1  : 0, (e.spawned || !triggertime) ? 1 : 2,  triggertime, 1050.0f);  break;
	    case 5: renderent(e, "switch1", -0.15f, (float)e.attr3*90, (!e.spawned && !triggertime) ? 30 : 0, (e.spawned || !triggertime) ? 1 : 30, triggertime, 35.0f); break;
            }; 
        };
    };
};

struct itemstat { int add, max, sound; } itemstats[] =
{
     10,    50, S_ITEMAMMO,
     20,   100, S_ITEMAMMO,
      5,    25, S_ITEMAMMO,
      5,    25, S_ITEMAMMO,
     25,   100, S_ITEMHEALTH,
     50,   200, S_ITEMHEALTH,
    100,   100, S_ITEMARMOUR,
    150,   150, S_ITEMARMOUR,
  20000, 30000, S_ITEMPUP,
};

void baseammo(int gun) { player1->ammo[gun] = itemstats[gun-1].add*2; };

// Added by Rick: baseammo for bots
void botbaseammo(int gun, dynent *d) { d->ammo[gun] = itemstats[gun-1].add*2; };

// these two functions are called when the server acknowledges that you really
// picked up the item (in multiplayer someone may grab it before you).

void radditem(int i, int &v)
{
    itemstat &is = itemstats[ents[i].type-I_SHELLS];
    ents[i].spawned = false;
    v += is.add;
    if(v>is.max) v = is.max;
    playsoundc(is.sound);
};

void realpickup(int n, dynent *d)
{
    switch(ents[n].type)
    {
        case I_SHELLS:  radditem(n, d->ammo[1]); break;
        case I_BULLETS: radditem(n, d->ammo[2]); break;
        case I_ROCKETS: radditem(n, d->ammo[3]); break;
        case I_ROUNDS:  radditem(n, d->ammo[4]); break;
        case I_HEALTH:  radditem(n, d->health);  break;
        case I_BOOST:   radditem(n, d->health);  break;

        case I_GREENARMOUR:
            radditem(n, d->armour);
            d->armourtype = A_GREEN;
            break;

        case I_YELLOWARMOUR:
            radditem(n, d->armour);
            d->armourtype = A_YELLOW;
            break;

        case I_QUAD:
            radditem(n, d->quadmillis);
            conoutf("you got the quad!");
            break;
    };
};

// these functions are called when the client touches the item

void additem(int i, int &v, int spawnsec)
{
    if(v<itemstats[ents[i].type-I_SHELLS].max)                              // don't pick up if not needed
    {
        //addmsg(1, 4, SV_ITEMPICKUP, i, m_classicsp ? 100000 : spawnsec, true); // first ask the server for an ack
	NMSTL::ptr<cube_item_pickup_tuple> ct = NMSTL::ptr<cube_item_pickup_tuple>(new cube_item_pickup_tuple());
	ct->clientnum = clientnum;
	ct->item_index = i;
	ct->spawn_secs = (m_classicsp ? 100000.0 : (double) spawnsec);
	ct->client_pickup = 1;
	client_pending_message.push_back(std::make_pair((int)SV_ITEMPICKUP, ct.dynamic_cast_to<borealis_tuple>()));

        ents[i].spawned = false;                                            // even if someone else gets it first
    };
};

void teleport(int n, dynent *d)     // also used by monsters
{
    int e = -1, tag = ents[n].attr1, beenhere = -1;
    for(;;)
    {
        e = findentity(TELEDEST, e+1);
        if(e==beenhere || e<0) { conoutf("no teleport destination for tag %d", tag); return; };
        if(beenhere<0) beenhere = e;
        if(ents[e].attr2==tag)
        {
            d->o.x = ents[e].x;
            d->o.y = ents[e].y;
            d->o.z = ents[e].z;
            d->yaw = ents[e].attr1;
            d->pitch = 0;
            d->vel.x = d->vel.y = d->vel.z = 0;
            entinmap(d);
            // Edited by Rick
            //playsoundc(S_TELEPORT);
            if (d->bIsBot)
                 botplaysound(S_TELEPORT, d);
            else
                 playsoundc(S_TELEPORT);
            break;
        };
    };
};

void pickup(int n, dynent *d)
{
    int np = 1;
    loopv(players) if(players[i]) np++;
    // Added by Rick: Count bots aswell
    loopv(bots) if(bots[i]) np++;
    np = np<3 ? 4 : (np>4 ? 2 : 3);         // spawn times are dependent on number of players
    int ammo = np*2;
    switch(ents[n].type)
    {
        case I_SHELLS:  additem(n, d->ammo[1], ammo); break;
        case I_BULLETS: additem(n, d->ammo[2], ammo); break;
        case I_ROCKETS: additem(n, d->ammo[3], ammo); break;
        case I_ROUNDS:  additem(n, d->ammo[4], ammo); break;
        case I_HEALTH:  additem(n, d->health,  np*5); break;
        case I_BOOST:   additem(n, d->health,  60);   break;

        case I_GREENARMOUR:
            // (100h/100g only absorbs 166 damage)
            if(d->armourtype==A_YELLOW && d->armour>66) break;
            additem(n, d->armour, 20);
            break;

        case I_YELLOWARMOUR:
            additem(n, d->armour, 20);
            break;

        case I_QUAD:
            additem(n, d->quadmillis, 60);
            break;
            
        case CARROT:
            ents[n].spawned = false;
            triggertime = lastmillis;
            trigger(ents[n].attr1, ents[n].attr2, false);  // needs to go over server for multiplayer
            break;

        case TELEPORT:
        {
            static int lastteleport = 0;
            if(lastmillis-lastteleport<500) break;
            lastteleport = lastmillis;
            teleport(n, d);
            break;
        };
        
        case JUMPPAD:
        {
            static int lastjumppad = 0;
            if(lastmillis-lastjumppad<300) break;
            lastjumppad = lastmillis;
            vec v = { (int)(char)ents[n].attr3/10.0f, (int)(char)ents[n].attr2/10.0f, ents[n].attr1/10.0f };
			vadd(player1->vel, v);
            playsoundc(S_JUMPPAD);
            break;
        };
    };
};

void checkitems()
{
    if(editmode) return;
    loopv(ents)
    {
        entity &e = ents[i];
        if(e.type==NOTUSED) continue;
        if(!ents[i].spawned && e.type!=TELEPORT && e.type!=JUMPPAD) continue;
        if(OUTBORD(e.x, e.y)) continue;
        vec v = { e.x, e.y, S(e.x, e.y)->floor+player1->eyeheight };
        vdist(dist, t, player1->o, v);
        if(dist<(e.type==TELEPORT ? 4 : 2.5)) pickup(i, player1);
    };
};

void checkquad(int time)
{
    if(player1->quadmillis && (player1->quadmillis -= time)<0)
    {
        player1->quadmillis = 0;
        playsoundc(S_PUPOUT);
        conoutf("quad damage is over");
    };
};

void putitems(uchar *&p)            // puts items in network stream and also spawns them locally
{
    loopv(ents) if((ents[i].type>=I_SHELLS && ents[i].type<=I_QUAD) || ents[i].type==CARROT)
    {
        putint(p, i);
        ents[i].spawned = true;
    };
};

void resetspawns() { loopv(ents) ents[i].spawned = false; };
void setspawn(int i, bool on)
{
    if(i<ents.length()) { 
	ents[i].spawned = on;
	//printf("Spawning item %d\n", i);
    }
};

// Added by Rick
bool intersect(entity *e, vec &from, vec &to, vec *end)   // if lineseg hits entity bounding box(entity version)
{
    mapmodelinfo &mmi = getmminfo(e->attr2);
    if(!&mmi || !mmi.h) return false;
    
    float lo = (float)(S(e->x, e->y)->floor+mmi.zoff+e->attr3);
    float hi = lo+mmi.h;
    vec v = to, w = { e->x, e->y, lo + (fabs(hi-lo)/2.0f) }, *p; 
    vsub(v, from);
    vsub(w, from);
    float c1 = dotprod(w, v);

    if(c1<=0) p = &from;
    else
    {
        float c2 = dotprod(v, v);
        if(c2<=c1) p = &to;
        else
        {
            float f = c1/c2;
            vmul(v, f);
            vadd(v, from);
            p = &v;
        };
    };
                    
    if (p->x <= e->x+mmi.rad
        && p->x >= e->x-mmi.rad
        && p->y <= e->y+mmi.rad
        && p->y >= e->y-mmi.rad
        && p->z <= hi
        && p->z >= lo)
     {
          if (end) *end = *p;
          return true;
     }
     return false;
};
// End add by Rick
