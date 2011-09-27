// editing.cpp: most map editing commands go here, entity editing commands are in world.cpp

#include <vector>
#include <NMSTL/ptr>

#include "tuples.h"
#include "netio.h"
#include "cube.h"

extern std::vector< std::pair< int, NMSTL::ptr<borealis_tuple> > > client_pending_message;
extern bool commandmode;

bool editmode = false; 

// the current selection, used by almost all editing commands
// invariant: all code assumes that these are kept inside MINBORD distance of the edge of the map

block sel =
{
    variable("selx",  0, 0, 4096, &sel.x,  NULL),
    variable("sely",  0, 0, 4096, &sel.y,  NULL),
    variable("selxs", 0, 0, 4096, &sel.xs, NULL),
    variable("selys", 0, 0, 4096, &sel.ys, NULL),
};

int selh = 0;
bool selset = false;
bool select_multiple = false;
std::vector< std::pair<block, int> > secondary_sel;

#define loopselxy(b) { makeundo(); loop(x,sel.xs) loop(y,sel.ys) { sqr *s = S(sel.x+x, sel.y+y); b; }; remip(sel); }

int cx, cy, ch;

int curedittex[] = { -1, -1, -1 };

bool dragging = false;
int lastx, lasty, lasth;

int lasttype = 0, lasttex = 0;
sqr rtex;

std::vector<bool> selected_items;
std::vector<bool> selected_players;
std::vector<bool> selected_bots;

VAR(editing,0,0,1);

void toggleedit()
{
    if(player1->state==CS_DEAD) return;                 // do not allow dead players to edit to avoid state confusion
    if(!editmode && !allowedittoggle()) return;         // not in most multiplayer modes
    if(!(editmode = !editmode))
    {
        settagareas();                                  // reset triggers to allow quick playtesting
        entinmap(player1);                              // find spawn closest to current floating pos
    }
    else
    {
        resettagareas();                                // clear trigger areas to allow them to be edited
        player1->health = 100;
        if(m_classicsp) monsterclear();                 // all monsters back at their spawns for editing
        projreset();
    };
    keyrepeat(editmode);
    selset = false;
    editing = editmode;

};

COMMANDN(edittoggle, toggleedit, ARG_NONE);

/*************************************************
 *
 * Selection helpers
 *
 *************************************************/

void toggleshiftselect() { select_multiple = !select_multiple; }

void selectitem()
{
    if (selected_items.size() < ents.length())
	selected_items.resize(ents.length());

    if (!select_multiple) fill(selected_items.begin(), selected_items.end(), false);

    vec selv = { worldpos.x, worldpos.y, worldpos.z };

    loopv(ents)
    {
          entity &e = ents[i];

          if(e.type < I_SHELLS || e.type > I_QUAD) continue;
	  if (!e.spawned) continue;
          if(OUTBORD(e.x, e.y)) continue;

          vec v = { e.x, e.y, S(e.x, e.y)->floor };

          vdist(dist, t, selv, v);
          if(dist<2.0)
	  {
	      selset = false;
	      selected_items[i] = true;
          }
    }
}

void selectplayer()
{
    if (cx == lastx && cy == lasty) return;

    if (selected_players.size() < players.length())
	selected_players.resize(players.length());

    if (!select_multiple) fill(selected_players.begin(), selected_players.end(), false);

    loopv(players)
    {
	if( players[i]
	    && (sel.x <= players[i]->o.x && players[i]->o.x <= (sel.x + sel.xs))
	    && (sel.y <= players[i]->o.y && players[i]->o.y <= (sel.y + sel.ys))
	    //&& selh <= players[i]->eyeheight )
	    )
	{   selected_players[i] = true;
	}
    }
}

void selectbot()
{
    if (cx == lastx && cy == lasty) return;

    if (selected_bots.size() < bots.length())
	selected_bots.resize(bots.length());

    if (!select_multiple) fill(selected_bots.begin(), selected_bots.end(), false);

    loopv(bots)
    {
	if( bots[i]
	    && (sel.x <= bots[i]->o.x && bots[i]->o.x <= (sel.x + sel.xs))
	    && (sel.y <= bots[i]->o.y && bots[i]->o.y <= (sel.y + sel.ys))
	    //&& selh <= bots[i]->eyeheight )
	    )
	{   selected_bots[i] = true;
	}
    }
}

bool selected_player(int clientnum)
{
    if (clientnum >= selected_players.size()) return false;
    return selected_players[clientnum];
}

bool selected_bot(int botnum)
{
    if (botnum >= selected_bots.size()) return false;
    return selected_bots[botnum];
}

int find_nth_selection(std::vector<bool>& selections, int n)
{
    int count = 1;
    std::vector<bool>::iterator select_it = find(selections.begin(), selections.end(), true);

    while (select_it != selections.end() && count < n) {
        count++;
        select_it = find(++select_it, selections.end(), true);
    }

    if ( select_it == selections.end() )
        return -1;

    return select_it - selections.begin();
}

int find_selected_item(int nth) { return find_nth_selection(selected_items, nth); }
int find_selected_player(int nth) { return find_nth_selection(selected_players, nth); }
int find_selected_bot(int nth) { return find_nth_selection(selected_bots, nth); }


void clear_selected_items()
{
   fill(selected_items.begin(), selected_items.end(), false);
}

void clear_selected_players()
{
   fill(selected_players.begin(), selected_players.end(), false);
}

void clear_selected_bots()
{
   fill(selected_bots.begin(), selected_bots.end(), false);
}

void clear_selected_region()
{
    block b = { 0, 0, 0, 0 };
    sel = b;
    selh = 0;
    selset = false;
}




void select_items(std::vector<int>& item_indices)
{
    std::vector<int>::iterator item_it = item_indices.begin();
    std::vector<int>::iterator item_end = item_indices.end();

    int num_items = selected_items.size();

    for(; item_it != item_end; ++item_it)
	if (*item_it < num_items)
	    selected_items[*item_it] = true;
}

void select_players(std::vector<int>& player_indices)
{
    std::vector<int>::iterator player_it = player_indices.begin();
    std::vector<int>::iterator player_end = player_indices.end();

    if (selected_players.size() < players.length())
	selected_players.resize(players.length());

    int num_players = selected_players.size();

    for(; player_it != player_end; ++player_it)
	if (*player_it < num_players)
	    selected_players[*player_it] = true;
}

void select_bots(std::vector<int>& bot_indices)
{
    std::vector<int>::iterator bot_it = bot_indices.begin();
    std::vector<int>::iterator bot_end = bot_indices.end();

    if (selected_bots.size() < bots.length())
	selected_bots.resize(bots.length());

    int num_bots = selected_bots.size();
    fill(selected_bots.begin(), selected_bots.end(), false);

    for(; bot_it != bot_end; ++bot_it)
	if (*bot_it < num_bots)
	    selected_bots[*bot_it] = true;
}

void select_region(block& region, int& regionh)
{
    secondary_sel.clear();
    sel = region;
    selh = regionh;
    selset = true;
}

/*************************************************/


void correctsel()                                       // ensures above invariant
{
    selset = !OUTBORD(sel.x, sel.y);
    int bsize = ssize-MINBORD;
    if(sel.xs+sel.x>bsize) sel.xs = bsize-sel.x;
    if(sel.ys+sel.y>bsize) sel.ys = bsize-sel.y;
    if(sel.xs<=0 || sel.ys<=0) selset = false;
};

bool noteditmode()
{
    correctsel();
    if(!editmode) conoutf("this function is only allowed in edit mode");
    return !editmode;
};

bool noselection()
{
    if(!selset) conoutf("no selection");
    return !selset;
};

#define EDITSEL   if(noteditmode() || noselection()) return;
#define EDITSELMP if(noteditmode() || noselection() || multiplayer()) return;
#define EDITMP    if(noteditmode() || multiplayer()) return;

void selectpos(int x, int y, int xs, int ys)
{
    block s = { x, y, xs, ys };
    sel = s;
    selh = 0;
    correctsel();
};

void makesel()
{
    block s = { min(lastx,cx), min(lasty,cy), abs(lastx-cx)+1, abs(lasty-cy)+1 };
    sel = s;
    selh = max(lasth,ch);
    correctsel();
    if(selset) rtex = *S(sel.x, sel.y);
};

VAR(flrceil,0,0,2);

float sheight(sqr *s, sqr *t, float z)                  // finds out z height when cursor points at wall
{
    return !flrceil //z-s->floor<s->ceil-z
        ? (s->type==FHF ? s->floor-t->vdelta/4.0f : (float)s->floor)
        : (s->type==CHF ? s->ceil+t->vdelta/4.0f : (float)s->ceil);
};

void cursorupdate()                                     // called every frame from hud
{
    flrceil = ((int)(player1->pitch>=0))*2;

    volatile float x = worldpos.x;                      // volatile needed to prevent msvc7 optimizer bug?
    volatile float y = worldpos.y;
    volatile float z = worldpos.z;
    
    cx = (int)x;
    cy = (int)y;

    if(OUTBORD(cx, cy)) return;
    sqr *s = S(cx,cy);
    
    if(fabs(sheight(s,s,z)-z)>1)                        // selected wall
    {
        x += x>player1->o.x ? 0.5f : -0.5f;             // find right wall cube
        y += y>player1->o.y ? 0.5f : -0.5f;

        cx = (int)x;
        cy = (int)y;

        if(OUTBORD(cx, cy)) return;
    };
        
    if(dragging) makesel();

    const int GRIDSIZE = 5;
    const float GRIDW = 0.5f;
    const float GRID8 = 2.0f;
    const float GRIDS = 2.0f;
    const int GRIDM = 0x7;
    
    // render editing grid

    /*
    for(int ix = cx-GRIDSIZE; ix<=cx+GRIDSIZE; ix++) for(int iy = cy-GRIDSIZE; iy<=cy+GRIDSIZE; iy++)
    {
        if(OUTBORD(ix, iy)) continue;
        sqr *s = S(ix,iy);
        if(SOLID(s)) continue;
        float h1 = sheight(s, s, z);
        float h2 = sheight(s, SWS(s,1,0,ssize), z);
        float h3 = sheight(s, SWS(s,1,1,ssize), z);
        float h4 = sheight(s, SWS(s,0,1,ssize), z);
        if(s->tag) linestyle(GRIDW, 0xFF, 0x40, 0x40);
        else if(s->type==FHF || s->type==CHF) linestyle(GRIDW, 0x80, 0xFF, 0x80);
        else linestyle(GRIDW, 0x80, 0x80, 0x80);
        block b = { ix, iy, 1, 1 };
        box(b, h1, h2, h3, h4);
        linestyle(GRID8, 0x40, 0x40, 0xFF);
        if(!(ix&GRIDM))   line(ix,   iy,   h1, ix,   iy+1, h4);
        if(!(ix+1&GRIDM)) line(ix+1, iy,   h2, ix+1, iy+1, h3);
        if(!(iy&GRIDM))   line(ix,   iy,   h1, ix+1, iy,   h2);
        if(!(iy+1&GRIDM)) line(ix,   iy+1, h4, ix+1, iy+1, h3);
    };
    */

    if(!SOLID(s))
    {
        float ih = sheight(s, s, z);
        linestyle(GRIDS, 0xFF, 0xFF, 0xFF);
        block b = { cx, cy, 1, 1 };
        box(b, ih, sheight(s, SWS(s,1,0,ssize), z), sheight(s, SWS(s,1,1,ssize), z), sheight(s, SWS(s,0,1,ssize), z));
        linestyle(GRIDS, 0xFF, 0x00, 0x00);
        dot(cx, cy, ih);
        ch = (int)ih;
    };

    if(selset)
    {
        linestyle(GRIDS, 0xFF, 0x40, 0x40);
        box(sel, (float)selh, (float)selh, (float)selh, (float)selh);

	std::vector< std::pair<block, int> >::iterator sel_it = secondary_sel.begin();
	std::vector< std::pair<block, int> >::iterator sel_end = secondary_sel.end();

	for (; sel_it != sel_end; ++sel_it)
	    box(sel_it->first, (float)sel_it->second, (float)sel_it->second, (float)sel_it->second, (float)sel_it->second);
    };
};

vector<block *> undos;                                  // unlimited undo
VAR(undomegs, 0, 1, 10);                                // bounded by n megs

void pruneundos(int maxremain)                          // bound memory
{
    int t = 0;
    loopvrev(undos)
    {
        t += undos[i]->xs*undos[i]->ys*sizeof(sqr);
        if(t>maxremain) free(undos.remove(i));
    };
};

void makeundo()
{
    undos.add(blockcopy(sel));
    pruneundos(undomegs<<20);
};

void editundo()
{
    EDITMP;
    if(undos.empty()) { conoutf("nothing more to undo"); return; };
    block *p = undos.pop();
    blockpaste(*p);
    free(p);
};

block *copybuf = NULL;

void copy()
{
    EDITSELMP;
    if(copybuf) free(copybuf);
    copybuf = blockcopy(sel);
};

void paste()
{
    EDITMP;
    if(!copybuf) { conoutf("nothing to paste"); return; };
    sel.xs = copybuf->xs;
    sel.ys = copybuf->ys;
    correctsel();
    if(!selset || sel.xs!=copybuf->xs || sel.ys!=copybuf->ys) { conoutf("incorrect selection"); return; };
    makeundo();
    copybuf->x = sel.x;
    copybuf->y = sel.y;
    blockpaste(*copybuf);
};

void tofronttex()                                       // maintain most recently used of the texture lists when applying texture
{
    loopi(3)
    {
        int c = curedittex[i];
        if(c>=0)
        {
            uchar *p = hdr.texlists[i];
            int t = p[c];
            for(int a = c-1; a>=0; a--) p[a+1] = p[a];
            p[0] = t;
            curedittex[i] = -1;
        };
    };
};

void editdrag(bool isdown)
{
    if(dragging = isdown)
    {
        lastx = cx;
        lasty = cy;
        lasth = ch;
        selset = false;
        tofronttex();
    };

    if (dragging && select_multiple)
	secondary_sel.push_back(std::make_pair(sel, selh));
    else if (!select_multiple)
	secondary_sel.clear();	


    makesel();

    if (commandmode) {
	selectitem();
	selectplayer();
	selectbot();
    }

};

// the core editing function. all the *xy functions perform the core operations
// and are also called directly from the network, the function below it is strictly
// triggered locally. They all have very similar structure.

void editheightxy(bool isfloor, int amount, block &sel)
{
    loopselxy(if(isfloor)
    {
        s->floor += amount;
        if(s->floor>=s->ceil) s->floor = s->ceil-1;
    }
    else
    {
        s->ceil += amount;
        if(s->ceil<=s->floor) s->ceil = s->floor+1;
    });
};

void editheight(int flr, int amount)
{
    EDITSEL;
    bool isfloor = flr==0;
    editheightxy(isfloor, amount, sel);
    NMSTL::ptr<cube_edit_tuple> t = NMSTL::ptr<cube_edit_tuple>(new cube_edit_tuple());
    t->edit_type = SV_EDITH;
    t->selection_x = sel.x;
    t->selection_y = sel.y;
    t->selection_xs = sel.xs;
    t->selection_ys = sel.ys;
    t->selection_type = (isfloor? 1 : 0);
    t->amount = amount;

    client_pending_message.push_back(std::make_pair((int)SV_EDITH, t.dynamic_cast_to<borealis_tuple>()));
};

COMMAND(editheight, ARG_2INT);

void edittexxy(int type, int t, block &sel)            
{
    loopselxy(switch(type)
    {
        case 0: s->ftex = t; break;
        case 1: s->wtex = t; break;
        case 2: s->ctex = t; break;
        case 3: s->utex = t; break;
    });
};

void edittex(int type, int dir)
{
    EDITSEL;
    if(type<0 || type>3) return;
    if(type!=lasttype) { tofronttex(); lasttype = type; };
    int atype = type==3 ? 1 : type;
    int i = curedittex[atype];
    i = i<0 ? 0 : i+dir;
    curedittex[atype] = i = min(max(i, 0), 255);
    int t = lasttex = hdr.texlists[atype][i];
    edittexxy(type, t, sel);

    NMSTL::ptr<cube_edit_tuple> ct = NMSTL::ptr<cube_edit_tuple>(new cube_edit_tuple());
    ct->edit_type = SV_EDITT;
    ct->selection_x = sel.x;
    ct->selection_y = sel.y;
    ct->selection_xs = sel.xs;
    ct->selection_ys = sel.ys;
    ct->selection_type = type;
    ct->lasttex = t;

    client_pending_message.push_back(std::make_pair((int)SV_EDITT, ct.dynamic_cast_to<borealis_tuple>()));
};

void replace()
{
    EDITSELMP;
    loop(x,ssize) loop(y,ssize)
    {
        sqr *s = S(x, y);
        switch(lasttype)
        {
            case 0: if(s->ftex == rtex.ftex) s->ftex = lasttex; break;
            case 1: if(s->wtex == rtex.wtex) s->wtex = lasttex; break;
            case 2: if(s->ctex == rtex.ctex) s->ctex = lasttex; break;
            case 3: if(s->utex == rtex.utex) s->utex = lasttex; break;
        };
    };
    block b = { 0, 0, ssize, ssize }; 
    remip(b);
};

void edittypexy(int type, block &sel)
{
    loopselxy(s->type = type);
};

void edittype(int type)
{
    EDITSEL;
    if(type==CORNER && (sel.xs!=sel.ys || sel.xs==3 || sel.xs>4 && sel.xs!=8
                   || sel.x&~-sel.xs || sel.y&~-sel.ys))
                   { conoutf("corner selection must be power of 2 aligned"); return; };
    edittypexy(type, sel);

    NMSTL::ptr<cube_edit_tuple> t = NMSTL::ptr<cube_edit_tuple>(new cube_edit_tuple());
    t->edit_type = SV_EDITS;
    t->selection_x = sel.x;
    t->selection_y = sel.y;
    t->selection_xs = sel.xs;
    t->selection_ys = sel.ys;
    t->selection_type = type;

    client_pending_message.push_back(std::make_pair((int)SV_EDITS, t.dynamic_cast_to<borealis_tuple>()));
};

void heightfield(int t) { edittype(t==0 ? FHF : CHF); };
void solid(int t)       { edittype(t==0 ? SPACE : SOLID); };
void corner()           { edittype(CORNER); };

COMMAND(heightfield, ARG_1INT);
COMMAND(solid, ARG_1INT);
COMMAND(corner, ARG_NONE);

void editequalisexy(bool isfloor, block &sel)
{
    int low = 127, hi = -128;
    loopselxy(
    {
        if(s->floor<low) low = s->floor;
        if(s->ceil>hi) hi = s->ceil;
    });
    loopselxy(
    {
        if(isfloor) s->floor = low; else s->ceil = hi;
        if(s->floor>=s->ceil) s->floor = s->ceil-1;
    });
};

void equalize(int flr)
{
    bool isfloor = flr==0;
    EDITSEL;
    editequalisexy(isfloor, sel);

    NMSTL::ptr<cube_edit_tuple> t = NMSTL::ptr<cube_edit_tuple>(new cube_edit_tuple());
    t->edit_type = SV_EDITE;
    t->selection_x = sel.x;
    t->selection_y = sel.y;
    t->selection_xs = sel.xs;
    t->selection_ys = sel.ys;
    t->selection_type = (isfloor? 1 : 0);

    client_pending_message.push_back(std::make_pair((int)SV_EDITE, t.dynamic_cast_to<borealis_tuple>()));
};

COMMAND(equalize, ARG_1INT);

void setvdeltaxy(int delta, block &sel)
{
    loopselxy(s->vdelta = max(s->vdelta+delta, 0));    
};

void setvdelta(int delta)
{
    EDITSEL;
    setvdeltaxy(delta, sel);

    NMSTL::ptr<cube_edit_tuple> t = NMSTL::ptr<cube_edit_tuple>(new cube_edit_tuple());
    t->edit_type = SV_EDITD;
    t->selection_x = sel.x;
    t->selection_y = sel.y;
    t->selection_xs = sel.xs;
    t->selection_ys = sel.ys;
    t->selection_type = delta;

    client_pending_message.push_back(std::make_pair((int)SV_EDITD, t.dynamic_cast_to<borealis_tuple>()));

};

const int MAXARCHVERT = 50;
int archverts[MAXARCHVERT][MAXARCHVERT];
bool archvinit = false;

void archvertex(int span, int vert, int delta)
{
    if(!archvinit)
    {
        archvinit = true;
        loop(s,MAXARCHVERT) loop(v,MAXARCHVERT) archverts[s][v] = 0;
    };
    if(span>=MAXARCHVERT || vert>=MAXARCHVERT || span<0 || vert<0) return;
    archverts[span][vert] = delta;
};

void arch(int sidedelta, int _a)
{
    EDITSELMP;
    sel.xs++;
    sel.ys++;
    if(sel.xs>MAXARCHVERT) sel.xs = MAXARCHVERT;
    if(sel.ys>MAXARCHVERT) sel.ys = MAXARCHVERT;
    loopselxy(s->vdelta =
        sel.xs>sel.ys
            ? (archverts[sel.xs-1][x] + (y==0 || y==sel.ys-1 ? sidedelta : 0))
            : (archverts[sel.ys-1][y] + (x==0 || x==sel.xs-1 ? sidedelta : 0)));
};

void slope(int xd, int yd)
{
    EDITSELMP;
    int off = 0;
    if(xd<0) off -= xd*sel.xs;
    if(yd<0) off -= yd*sel.ys;
    sel.xs++;
    sel.ys++;
    loopselxy(s->vdelta = xd*x+yd*y+off);
};

void perlin(int scale, int seed, int psize)
{
    EDITSELMP;
    sel.xs++;
    sel.ys++;
    makeundo();
    sel.xs--;
    sel.ys--;
    perlinarea(sel, scale, seed, psize);
    sel.xs++;
    sel.ys++;
    remip(sel);
    sel.xs--;
    sel.ys--;
};

VARF(fullbright, 0, 0, 1,
    if(fullbright)
    {
        if(noteditmode()) return;
        loopi(mipsize) world[i].r = world[i].g = world[i].b = 176;
    };
);

void edittag(int tag)
{
    EDITSELMP;
    loopselxy(s->tag = tag);
};

void newent(char *what, char *a1, char *a2, char *a3, char *a4)
{
    EDITSEL;
    newentity(sel.x, sel.y, (int)player1->o.z, what, ATOI(a1), ATOI(a2), ATOI(a3), ATOI(a4));
};

COMMANDN(select, selectpos, ARG_4INT);
COMMAND(edittag, ARG_1INT);
COMMAND(replace, ARG_NONE);
COMMAND(archvertex, ARG_3INT);
COMMAND(arch, ARG_2INT);
COMMAND(slope, ARG_2INT);
COMMANDN(vdelta, setvdelta, ARG_1INT);
COMMANDN(undo, editundo, ARG_NONE);
COMMAND(copy, ARG_NONE);
COMMAND(paste, ARG_NONE);
COMMAND(edittex, ARG_2INT);
COMMAND(newent, ARG_5STR);
COMMAND(perlin, ARG_3INT);


