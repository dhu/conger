// renderextras.cpp: misc gl render code and the HUD

#include "cube.h"

// Added by Rick
char DebugTexts[10][256] = {0,0,0,0,0,0,0,0,0,0};
// End add

// Added by yna
extern bool commandmode;
extern int tuplerate;

void line(int x1, int y1, float z1, int x2, int y2, float z2)
{
    glBegin(GL_POLYGON);
    glVertex3f((float)x1, z1, (float)y1);
    glVertex3f((float)x1, z1, y1+0.01f);
    glVertex3f((float)x2, z2, y2+0.01f);
    glVertex3f((float)x2, z2, (float)y2);
    glEnd();
    xtraverts += 4;
};

void linestyle(float width, int r, int g, int b)
{
    glLineWidth(width);
    glColor3ub(r,g,b);
};

void box(block &b, float z1, float z2, float z3, float z4)
{
    glBegin(GL_POLYGON);
    glVertex3f((float)b.x,      z1, (float)b.y);
    glVertex3f((float)b.x+b.xs, z2, (float)b.y);
    glVertex3f((float)b.x+b.xs, z3, (float)b.y+b.ys);
    glVertex3f((float)b.x,      z4, (float)b.y+b.ys);
    glEnd();
    xtraverts += 4;
};

void dot(int x, int y, float z)
{
    const float DOF = 0.1f;
    glBegin(GL_POLYGON);
    glVertex3f(x-DOF, (float)z, y-DOF);
    glVertex3f(x+DOF, (float)z, y-DOF);
    glVertex3f(x+DOF, (float)z, y+DOF);
    glVertex3f(x-DOF, (float)z, y+DOF);
    glEnd();
    xtraverts += 4;
};

void blendbox(int x1, int y1, int x2, int y2, bool border)
{
    glDepthMask(GL_FALSE);
    glDisable(GL_TEXTURE_2D);
    glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
    glBegin(GL_QUADS);
    if(border) glColor3d(0.5, 0.3, 0.4); 
    else glColor3d(1.0, 1.0, 1.0);
    glVertex2i(x1, y1);
    glVertex2i(x2, y1);
    glVertex2i(x2, y2);
    glVertex2i(x1, y2);
    glEnd();
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_POLYGON);
    glColor3d(0.2, 0.7, 0.4); 
    glVertex2i(x1, y1);
    glVertex2i(x2, y1);
    glVertex2i(x2, y2);
    glVertex2i(x1, y2);
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    xtraverts += 8;
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glDepthMask(GL_TRUE);
};

const int MAXSPHERES = 50;
struct sphere { vec o; float size, max; int type; sphere *next; };
sphere spheres[MAXSPHERES], *slist = NULL, *sempty = NULL;
bool sinit = false;

void newsphere(vec &o, float max, int type)
{
    if(!sinit)
    {
        loopi(MAXSPHERES)
        {
            spheres[i].next = sempty;
            sempty = &spheres[i];
        };
        sinit = true;
    };
    if(sempty)
    {
        sphere *p = sempty;
        sempty = p->next;
        p->o = o;
        p->max = max;
        p->size = 1;
        p->type = type;
        p->next = slist;
        slist = p;
    };
};

void renderspheres(int time)
{
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBindTexture(GL_TEXTURE_2D, 4);  

    for(sphere *p, **pp = &slist; p = *pp;)
    {
        glPushMatrix();
        float size = p->size/p->max;
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f-size);
        glTranslatef(p->o.x, p->o.z, p->o.y);
        glRotatef(lastmillis/5.0f, 1, 1, 1);
        glScalef(p->size, p->size, p->size);
        glCallList(1);
        glScalef(0.8f, 0.8f, 0.8f);
        glCallList(1);
        glPopMatrix();
        xtraverts += 12*6*2;

        if(p->size>p->max)
        {
            *pp = p->next;
            p->next = sempty;
            sempty = p;
        }
        else
        {
            p->size += time/100.0f;   
            pp = &p->next;
        };
    };

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
};

string closeent;
char *entnames[] =
{
    "none?", "light", "playerstart",
    "shells", "bullets", "rockets", "riflerounds",
    "health", "healthboost", "greenarmour", "yellowarmour", "quaddamage", 
    "teleport", "teledest", 
    "mapmodel", "monster", "trigger", "jumppad",
    "?", "?", "?", "?", "?", 
};

void renderents()       // show sparkly thingies for map entities in edit mode
{
    closeent[0] = 0;
    if(!editmode || commandmode) return;
    loopv(ents)
    {
        entity &e = ents[i];
        if(e.type==NOTUSED) continue;
        vec v = { e.x, e.y, e.z };
        particle_splash(2, 2, 40, v);
    };
    int e = closestent();
    if(e>=0)
    {
        entity &c = ents[e];
        sprintf_s(closeent)("closest entity = %s (%d, %d, %d, %d), selection = (%d, %d)", entnames[c.type], c.attr1, c.attr2, c.attr3, c.attr4, getvar("selxs"), getvar("selys"));
    };
};

void loadsky(char *basename)
{
    static string lastsky = "";
    if(strcmp(lastsky, basename)==0) return;
    char *side[] = { "ft", "bk", "lf", "rt", "dn", "up" };
    int texnum = 14;
    loopi(6)
    {
        sprintf_sd(name)("packages/%s_%s.jpg", basename, side[i]);
        int xs, ys;
        if(!installtex(texnum+i, path(name), xs, ys, true)) conoutf("could not load sky textures");
    };
    strcpy_s(lastsky, basename);
};

COMMAND(loadsky, ARG_1STR);

float cursordepth = 0.9f;
GLint viewport[4];
GLdouble mm[16], pm[16];
vec worldpos;

void readmatrices()
{
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mm);
    glGetDoublev(GL_PROJECTION_MATRIX, pm);
};

// stupid function to cater for stupid ATI drivers that return incorrect depth values

float depthcorrect(float d)
{
        /* 
	static int when = 0;
	static float factor = 1;
	//if(when++==5) factor = 0.992205f/d;		// this magic value is tied to the fixed start spawn point in metl3
	return d*factor;*/
        
        // FIX FOR ATI CARDS BY 'arghvark'
        return (d<=1/256.0f) ? d*256 : d;  
}

// find out the 3d target of the crosshair in the world easily and very acurately.
// sadly many very old cards and drivers appear to fuck up on gluUnProject() and give false
// coordinates, making shooting and such impossible.
// also hits map entities which is unwanted.
// could be replaced by a more acurate version of monster.cpp los() if needed

void readdepth(int w, int h)
{
    glReadPixels(w/2, h/2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &cursordepth);
    double worldx = 0, worldy = 0, worldz = 0;
    gluUnProject(w/2, h/2, depthcorrect(cursordepth), mm, pm, viewport, &worldx, &worldz, &worldy);
    worldpos.x = (float)worldx;
    worldpos.y = (float)worldy;
    worldpos.z = (float)worldz;   
    vec r = { (float)mm[0], (float)mm[4], (float)mm[8] };
    vec u = { (float)mm[1], (float)mm[5], (float)mm[9] };
    setorient(r, u);
};

void drawicon(float tx, float ty, int x, int y)
{
    glBindTexture(GL_TEXTURE_2D, 5);
    glBegin(GL_QUADS);
    tx /= 192;
    ty /= 192;
    float o = 1/3.0f;
    int s = 120;
    glTexCoord2f(tx,   ty);   glVertex2i(x,   y);
    glTexCoord2f(tx+o, ty);   glVertex2i(x+s, y);
    glTexCoord2f(tx+o, ty+o); glVertex2i(x+s, y+s);
    glTexCoord2f(tx,   ty+o); glVertex2i(x,   y+s);
    glEnd();
    xtraverts += 4;
};

void invertperspective()
{
    // This only generates a valid inverse matrix for matrices generated by gluPerspective()
    GLdouble inv[16];
    memset(inv, 0, sizeof(inv));

    inv[0*4+0] = 1.0/pm[0*4+0];
    inv[1*4+1] = 1.0/pm[1*4+1];
    inv[2*4+3] = 1.0/pm[3*4+2];
    inv[3*4+2] = -1.0;
    inv[3*4+3] = pm[2*4+2]/pm[3*4+2];

    glLoadMatrixd(inv);
};

VAR(crosshairsize, 0, 15, 50);

int dblend = 0;
void damageblend(int n) { dblend += n; };

VAR(hidestats, 0, 0, 1);
VAR(crosshairfx, 0, 1, 1);

//yna
VAR(hideplayerstats, 0, 0, 1);
VAR(hideclosest, 0, 0, 1);

void gl_drawhud(int w, int h, int curfps, int nquads, int curvert, bool underwater)
{
    readmatrices();
    if(editmode)
    {
        if(cursordepth==1.0f) worldpos = player1->o;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        cursorupdate();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    };

    // Added by Rick: Draw waypoints
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //WaypointClass.Think();   
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // End add
    
    glDisable(GL_DEPTH_TEST);
    invertperspective();
    glPushMatrix();  
    glOrtho(0, VIRTW, VIRTH, 0, -1, 1);
    glEnable(GL_BLEND);

    if(dblend || underwater)
    {
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBegin(GL_QUADS);
        if(dblend) glColor3d(0.0f, 0.9f, 0.9f);
        else glColor3d(0.9f, 0.5f, 0.0f);
        glVertex2i(0, 0);
        glVertex2i(VIRTW, 0);
        glVertex2i(VIRTW, VIRTH);
        glVertex2i(0, VIRTH);
        glEnd();
        glDepthMask(GL_TRUE);
        dblend -= curtime/3;
        if(dblend<0) dblend = 0;
    };

    glEnable(GL_TEXTURE_2D);
   
    char *command = getcurcommand();
    char *player = playerincrosshair();
    // Added by Rick: Display botnames aswell
    char *bot = botincrosshair();
    if(command) draw_textf("> %s_", 20, 1570, 2, (int)command);
    else if(closeent[0] && !hideclosest) draw_text(closeent, 20, 1570, 2);
    else if(player) draw_text(player, 20, 1570, 2);
    // Added by Rick
    else if(bot) draw_text(bot, 20, 1570, 2);

    renderscores();
    if(!rendermenu())
    {
        glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, 1);
        glBegin(GL_QUADS);
        glColor3ub(255,255,255);
        if(crosshairfx)
        {
            if(player1->gunwait) glColor3ub(128,128,128);
            else if(player1->health<=25) glColor3ub(255,0,0);
            else if(player1->health<=50) glColor3ub(255,128,0);
        };
        float chsize = (float)crosshairsize;
        glTexCoord2d(0.0, 0.0); glVertex2f(VIRTW/2 - chsize, VIRTH/2 - chsize);
        glTexCoord2d(1.0, 0.0); glVertex2f(VIRTW/2 + chsize, VIRTH/2 - chsize);
        glTexCoord2d(1.0, 1.0); glVertex2f(VIRTW/2 + chsize, VIRTH/2 + chsize);
        glTexCoord2d(0.0, 1.0); glVertex2f(VIRTW/2 - chsize, VIRTH/2 + chsize);
        glEnd();
    };

    glPopMatrix();

    glPushMatrix();    
    glOrtho(0, VIRTW*4/3, VIRTH*4/3, 0, -1, 1);
    renderconsole();

    if(!hidestats)
    {
        glPopMatrix();
        glPushMatrix();
        glOrtho(0, VIRTW*3/2, VIRTH*3/2, 0, -1, 1);
        draw_textf("fps %d", 3200, 2250, 2, curfps);
        draw_textf("lod %d", 3200, 2320, 2, lod_factor());
        draw_textf("wqd %d", 3200, 2390, 2, nquads); 
        draw_textf("wvt %d", 3200, 2460, 2, curvert);
        draw_textf("evt %d", 3200, 2530, 2, xtraverts);
	draw_textf("rate %d", 3200, 2600, 2, tuplerate);
    };

    if (commandmode)
	renderqueries();
    
    // Added by Rick
    int count=0;
    while((count<10) && DebugTexts[count][0])
    {
        int top = 500 + (70*count);
        int left = 3200 - (strlen(DebugTexts[count]) * 20);
        draw_text(DebugTexts[count], left, top, 2);
        count++;
    }
     
    for(count=0;count<10;count++) DebugTexts[count][0] = 0;
    //End add by Rick
    
    glPopMatrix();

    if(player1->state==CS_ALIVE && !hideplayerstats)
    {
        glPushMatrix();
        glOrtho(0, VIRTW/2, VIRTH/2, 0, -1, 1);
        draw_textf("%d",  90, 827, 2, player1->health);
        if(player1->armour) draw_textf("%d", 390, 827, 2, player1->armour);
        draw_textf("%d", 690, 827, 2, player1->ammo[player1->gunselect]);
        glPopMatrix();
        glPushMatrix();
        glOrtho(0, VIRTW, VIRTH, 0, -1, 1);
        glDisable(GL_BLEND);
        drawicon(128, 128, 20, 1650);
        if(player1->armour) drawicon((float)(player1->armourtype*64), 0, 620, 1650); 
        int g = player1->gunselect;
        int r = 64;
        if(g>2) { g -= 3; r = 128; };
        drawicon((float)(g*64), (float)r, 1220, 1650);   
        glPopMatrix();
    };

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
};

// Added by Rick

#ifndef RELEASE_BUILD
void AddDebugText(char *text, ...)
{
     int i=0;
     va_list v;
     char sz[1024];
     
     while(i<10)
     {
          if (!DebugTexts[i][0])
          {
               va_start(v, text);
               vsprintf(sz, text, v);
               va_end(v);
          
               strcpy(DebugTexts[i], sz);
               break;
          }
          i++;
     }
}
#endif

// End add by Rick
