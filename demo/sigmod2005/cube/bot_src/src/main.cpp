// main.cpp: initialisation & main loop

#include <string>

#include "netio.h"
#include "cube.h"

bool send_position_flag = false;

void cleanup(char *msg)         // single program exit point;
{
    stop();
    disconnect(true);
    cleangl();
    cleansound();
    cleanupserver();
    SDL_ShowCursor(1);
    if(msg)
    {
        #ifdef WIN32
        MessageBox(NULL, msg, "cube fatal error", MB_OK|MB_SYSTEMMODAL);
        #else
        printf(msg);
        #endif
    };
    SDL_Quit();
    exit(1);
};

void quit()                     // normal exit
{
    writeservercfg();
    cleanup(NULL);
};

void fatal(char *s, char *o)    // failure exit
{
    sprintf_sd(msg)("%s%s (%s)\n", s, o, SDL_GetError());
    cleanup(msg);
};

void *alloc(int s)              // for some big chunks... most other allocs use the memory pool
{
    void *b = calloc(1,s);
    if(!b) fatal("out of memory!");
    return b;
};

int minfps = 32;
int maxfps = 40;

int scr_w = 640;
int scr_h = 480;

bool grabbed = false;

void screenshot()
{
    SDL_Surface *image;
    SDL_Surface *temp;
    int idx;
    if(image = SDL_CreateRGBSurface(SDL_SWSURFACE, scr_w, scr_h, 24, 0x0000FF, 0x00FF00, 0xFF0000, 0))
    {
        if(temp  = SDL_CreateRGBSurface(SDL_SWSURFACE, scr_w, scr_h, 24, 0x0000FF, 0x00FF00, 0xFF0000, 0))
        {
            glReadPixels(0, 0, scr_w, scr_h, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
            for (idx = 0; idx<scr_h; idx++)
            {
                char *dest = (char *)temp->pixels+3*scr_w*idx;
                memcpy(dest, (char *)image->pixels+3*scr_w*(scr_h-1-idx), 3*scr_w);
                endianswap(dest, 3, scr_w);
            };
            sprintf_sd(buf)("screenshot_%d.bmp", lastmillis);
            SDL_SaveBMP(temp, buf);
            SDL_FreeSurface(temp);
        };
        SDL_FreeSurface(image);
    };
};

COMMAND(screenshot, ARG_NONE);
COMMAND(quit, ARG_NONE);

void fpsrange(int low, int high)
{
    if(low>high || low<1) return;
    minfps = low;
    maxfps = high;
};

COMMAND(fpsrange, ARG_2INT);

void keyrepeat(bool on)
{
    SDL_EnableKeyRepeat(on ? SDL_DEFAULT_REPEAT_DELAY : 0,
                             SDL_DEFAULT_REPEAT_INTERVAL);
};

VARF(gamespeed, 10, 70, 1000, if(multiplayer()) gamespeed = 70);

int islittleendian = 1;
int framesinmap = 0;

int main(int argc, char **argv)
{    
    bool dedicated = false, listen = false;
    int fs = SDL_FULLSCREEN, par = 0, uprate = 0;
    char *sdesc = "", *ip = "", *master = NULL, *passwd = "";
    islittleendian = *((char *)&islittleendian);

    std::string borealis_addr;
    std::string deploy_addr;
    std::string subscriber_addr;

    #define log(s) puts("init: " s)
    log("sdl");
    
    for(int i = 1; i<argc; i++)
    {
        char *a = &argv[i][2];
        if(argv[i][0]=='-') switch(argv[i][1])
        {
            //case 'd': listen = dedicated = true; break;
            case 'l': listen = true; break;
            case 't': fs     = 0; break;
            case 'w': scr_w  = atoi(a); break;
            case 'h': scr_h  = atoi(a); break;
            case 'u': uprate = atoi(a); break;
            case 'n': sdesc  = a; break;
            case 'i': ip     = a; break;
            case 'm': master = a; break;
            case 'p': passwd = a; break;
            case 'b':
                borealis_addr = std::string(a);
                printf("game addr %s\n", borealis_addr.c_str());
                break;
            case 'd':
                deploy_addr = std::string(a);
                printf("deploy addr %s\n", deploy_addr.c_str());
                break;
            case 's':
                subscriber_addr = std::string(a);
                printf("subscriber addr %s\n", subscriber_addr.c_str());
                break;
            case 'o':
                enable_motion_logging = true;
                motion_log_filename = std::string(a);
            default:  conoutf("unknown commandline option");
        }
        else conoutf("unknown commandline argument");
    };
    
    #ifdef _DEBUG
    par = SDL_INIT_NOPARACHUTE;
    fs = 0;
    #endif

    //if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO|par)<0) fatal("Unable to initialize SDL");
    if(SDL_Init(SDL_INIT_TIMER|par)<0) fatal("Unable to initialize SDL");

    log("net");
    if(enet_initialize()<0) fatal("Unable to initialise network module");

    initclient();
    initserver(dedicated, listen, uprate, sdesc, ip, master, passwd);  // never returns if dedicated
      
    log("world");
    empty_world(7, true);

#ifndef DISABLE_RENDER
    log("video: sdl");
    if(SDL_InitSubSystem(SDL_INIT_VIDEO)<0) fatal("Unable to initialize SDL Video");

    log("video: mode");
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    if(SDL_SetVideoMode(scr_w, scr_h, 0, SDL_OPENGL|fs)==NULL) fatal("Unable to create OpenGL screen");

    log("video: misc");
    SDL_WM_SetCaption("cube engine", NULL);
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    keyrepeat(false);
    SDL_ShowCursor(0);

    log("gl");
    gl_init(scr_w, scr_h);

    log("basetex");
    int xs, ys;
    if(!installtex(2,  path(newstring("data/newchars.png")), xs, ys) ||
       !installtex(3,  path(newstring("data/martin/base.png")), xs, ys) ||
       !installtex(6,  path(newstring("data/martin/ball1.png")), xs, ys) ||
       !installtex(7,  path(newstring("data/martin/smoke.png")), xs, ys) ||
       !installtex(8,  path(newstring("data/martin/ball2.png")), xs, ys) ||
       !installtex(9,  path(newstring("data/martin/ball3.png")), xs, ys) ||
       !installtex(4,  path(newstring("data/explosion.jpg")), xs, ys) ||
       !installtex(5,  path(newstring("data/items.png")), xs, ys) ||
       !installtex(1,  path(newstring("data/crosshair.png")), xs, ys)) fatal("could not find core textures (hint: run cube from the parent of the bin directory)");
#endif

    log("sound");
    initsound();

#ifndef DISABLE_RENDER
    log("cfg");
    // Modified by Rick
    //newmenu("frags\tpj\tping\tteam\tname");
    newmenu("frags\tdeaths\tpj\tping\tteam\tname");
    newmenu("ping\tplr\tserver");
    exec("data/keymap.cfg");
    //exec("data/models.cfg");
    // Modified by Rick
    //exec("data/menus.cfg");
    exec("data/menus_bot.cfg");
    exec("data/prefabs.cfg");
    exec("data/sounds.cfg");
    exec("servers.cfg");
    exec("autoexec.cfg");
    //exec("data/default_map_settings.cfg");
#endif

    //log("localconnect");
    //localconnect();
    changemap("metl3");		// if this map is changed, also change depthcorrect()

    if (borealis_addr.empty())
	borealis_addr = std::string("127.0.0.1");

    if (deploy_addr.empty())
        deploy_addr = std::string("127.0.0.1");

    if (subscriber_addr.empty())
        subscriber_addr = std::string("127.0.0.1");

    set_up_sockets_and_subscribe(borealis_addr, deploy_addr, subscriber_addr);
    
    log("mainloop");
    int ignore = 5;
    for(;;)
    {
        int millis = SDL_GetTicks()*gamespeed/100;
        if(millis-lastmillis>200) lastmillis = millis-200;
        else if(millis-lastmillis<1) lastmillis = millis-1;
        //cleardlights();
        updateworld(millis);
        if(!demoplayback) serverslice(/*enet_time_get_sec()*/time(NULL), 0);
        static float fps = 30.0f;
        fps = (1000.0f/curtime+fps*50)/51;
#ifndef DISABLE_RENDER
        computeraytable(player1->o.x, player1->o.y);
        readdepth(scr_w, scr_h);
        SDL_GL_SwapBuffers();
        if(framesinmap++<5)
	    // cheap hack to get rid of initial sparklies, even when triple buffering etc.
        {
			player1->yaw += 5;
			gl_drawframe(scr_w, scr_h, 1.0f, fps);
			player1->yaw -= 5;
        };
        gl_drawframe(scr_w, scr_h, fps<minfps ? fps/minfps : (fps>maxfps ? fps/maxfps : 1.0f), fps);
#endif
        //SDL_Delay(100);
        SDL_Event event;
        int lasttype = 0, lastbut = 0;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    quit();
                    break;

                case SDL_KEYDOWN:
		    if(event.key.keysym.sym == SDLK_F1) {
			send_position_flag = !send_position_flag;
		    }
		    if(event.key.keysym.sym == SDLK_F2) {
                        if (!grabbed)
		            SDL_WM_GrabInput(SDL_GRAB_ON);
			else
		            SDL_WM_GrabInput(SDL_GRAB_OFF);
			grabbed = !grabbed;
		    }
		    if (event.key.keysym.sym == SDLK_LSHIFT) {
			toggleshiftselect();
			break;
		    }

                case SDL_KEYUP: 
		    if (event.key.keysym.sym == SDLK_LSHIFT) {
			toggleshiftselect();
		    }
                    keypress(event.key.keysym.sym, event.key.state==SDL_PRESSED, event.key.keysym.unicode);
                    break;

                case SDL_MOUSEMOTION:
                    if(ignore) { ignore--; break; };
                    mousemove(event.motion.xrel, event.motion.yrel);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    if(lasttype==event.type && lastbut==event.button.button) break; // why?? get event twice without it
                    keypress(-event.button.button, event.button.state!=0, 0);
                    lasttype = event.type;
                    lastbut = event.button.button;
                    break;
            };
        };
    };
    quit();
    return 1;
};
