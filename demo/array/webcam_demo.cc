#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "ArrayMarshal.h"

using namespace std;
using namespace Borealis;

#define  FRAME_WIDTH    320
#define  FRAME_HEIGHT   240
#define  FRAME_DEPTH      3
#define  FRAME_SIZE  230400

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int rmask = 0xff000000;
    int gmask = 0x00ff0000;
    int bmask = 0x0000ff00;
    int amask = 0x000000ff;
#else
    int rmask = 0x000000ff;
    int gmask = 0x0000ff00;
    int bmask = 0x00ff0000;
    int amask = 0xff000000;
#endif

unsigned char *buffer;
int IRframeNumber = 1;
int CCDframeNumber = 1;

bool loadImage(string fileName, unsigned char* buf)
{
    ifstream fin(fileName.c_str(), ios::in | ios::binary);
    fin.read((char *)buf, FRAME_WIDTH * FRAME_HEIGHT * FRAME_DEPTH);
    if (fin.fail())
    {
        cout << "Read on " << fileName << " failed.";
        return false;
    }
    return true;
}



SDL_Surface *screen;	// Backbuffer
SDL_Surface *image;	// Image to put to the backbuffer
SDL_Surface *tempImage; // Temp image to write pixels to
SDL_Rect src, dest;	// src and dest regions for the blit

int FilterEvents(const SDL_Event *event)
{
    if ( event->type == SDL_QUIT )
        exit(1);
    return 1;
}

const uint32  SLEEP_TIME  = 1000;           // Delay between injections.
const uint32  BATCH_SIZE  =   1;           // Number of input tuples per batch.
const uint32  BATCH_COUNT =   500;           // Number batches to send.

const Time  time0 = Time::now() - Time::msecs( 100 );

    ArrayMarshal   marshal;        // Client and I/O stream state.

    uint32  result = 0;
    uint32  total  = BATCH_COUNT * BATCH_SIZE;
    uint32  remain = BATCH_COUNT;

    uint8   expect = 0;
    uint8   value[FRAME_WIDTH][FRAME_HEIGHT][FRAME_DEPTH];


////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  ArrayMarshal::receivedResult(PacketTuple  *tuple)
{
//..............................................................................

    DEBUG << "Received tuple!";
    tempImage = SDL_CreateRGBSurface(SDL_SWSURFACE, FRAME_WIDTH, FRAME_HEIGHT, 32, rmask, gmask, bmask, amask);
    if(tempImage == NULL)
    {
        DEBUG << "CreateRGBSurface failed: " << SDL_GetError();
        exit(1);
    }

    for(int i=0; i < FRAME_WIDTH * FRAME_HEIGHT; i++)
    {
        //ARGB to ABGR
        ((unsigned int*)tempImage->pixels)[i]  =  255 << 24;	//A
        ((unsigned int*)tempImage->pixels)[i] |=  tuple->value[3*i  ];	      //R
        ((unsigned int*)tempImage->pixels)[i] |=  tuple->value[3*i+1] << 8;	//G
        ((unsigned int*)tempImage->pixels)[i] |=  tuple->value[3*i+2] << 16;	//B
    }


    image = SDL_DisplayFormat(tempImage);
    SDL_BlitSurface(image, &src, screen, &dest);
    SDL_Flip(screen);
    SDL_FreeSurface(tempImage);
    SDL_FreeSurface(image);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  ArrayMarshal::sentPacket()
{


//..............................................................................

    Packet  tuple;

    ostringstream os;
    os << "../../../frames/video0" << setw(3) << setfill('0') << CCDframeNumber++ << ".raw";
    string fileName = os.str();

    loadImage(fileName, buffer);

    tuple._data.value = buffer;

    batchPacket(&tuple);
    DEBUG << "Sent CCD tuple!";

    if (CCDframeNumber < 125)
    {   sendPacket(SLEEP_TIME);
    }
    else
    {   sendPacket(0);
        NOTICE << "Finished";
    }
    return;
}


/*void  ArrayMarshal::sentIr()
{



//..............................................................................


    Ir  tuple;

    ostringstream os;
    os << "../../../frames/video1" << setw(3) << setfill('0') << IRframeNumber++ << ".raw";
    string fileName = os.str();

    loadImage(fileName, buffer);

    tuple._data.ir_frame = buffer;

    batchIr(&tuple);
    DEBUG << "Sent IR tuple!";

    if (IRframeNumber < 125)
    {   sendIr(SLEEP_TIME);
    }
    else
    {   sendIr(0);
        NOTICE << "Finished";
    }
    return;
    }*/



////////////////////////////////////////////////////////////////////////////////
//
int  main(int  argc, const char  *argv[])
{
//  Return:  1 - Could not deploy the network.
//           2 - An invalid tuple value was received.
//..............................................................................

    buffer = new unsigned char[FRAME_WIDTH * FRAME_HEIGHT * FRAME_DEPTH];


    // Blit src rectangle
    src.x = 0;
    src.y = 0;
    src.w = FRAME_WIDTH;
    src.h = FRAME_HEIGHT;

    // Blit destination rectangle
    dest.x = 0;	
    dest.y = 0;
    dest.w = FRAME_WIDTH;
    dest.h = FRAME_HEIGHT;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
            cerr << "Unable to initialize SDL: " << SDL_GetError() << endl;
            exit(1);
    }

    atexit(SDL_Quit);
    SDL_SetEventFilter(FilterEvents);

    screen = SDL_SetVideoMode(FRAME_WIDTH, FRAME_HEIGHT, 32, SDL_DOUBLEBUF);
    if (screen == NULL) 
    {
            cerr << "Unable to set video mode: " << SDL_GetError() << endl;
            exit(1);
    }

    SDL_WM_SetCaption("Borealis Output","output");
    SDL_WM_GrabInput(SDL_GRAB_OFF);






    // Run the front-end, open a client, subscribe to outputs and inputs.
    result = marshal.open();

    if (result)
    {   WARN << "Could not deploy the network.";
        result = 1;
    }
    else
    {   DEBUG  << "time0 = " << time0;

        // Use a constant array each frame.
        for (uint32  i = 0; i < FRAME_WIDTH; i++)
        {   for (uint32  j = 0; j < FRAME_HEIGHT; j++)
            {   for (uint32  k = 0; k < FRAME_DEPTH; k++)
                {   value[i][j][k] = k;
        }   }   }

        // Send the first batch of tuples.  Queue up the next round with a delay.
	marshal.sentPacket();
        //marshal.sentIr();

        DEBUG  << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    NOTICE << "Exiting successfully!";

    return(result);
}
