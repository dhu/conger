#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include "VideoMarshal.h"

using namespace std;
using namespace Borealis;

const uint32  SLEEP_TIME = 100;          // Delay between injections.
const uint32  BATCH_SIZE  = 1;          // Number of input tuples per batch.
//const uint32  BATCH_COUNT = 3;           // Number batches to send.
#define  FRAME_WIDTH    352
#define  FRAME_HEIGHT   288
#define  FRAME_DEPTH      3
#define  DISP_WIDTH  3*FRAME_WIDTH
#define  DISP_HEIGHT FRAME_HEIGHT
#define  NUM_FRAMES 375

const Time  time0 = Time::now() - Time::msecs( 100 );

      VideoMarshal   marshal;        // Client and I/O stream state.

      

unsigned char buffer[FRAME_WIDTH*FRAME_HEIGHT][FRAME_DEPTH];
char contrast[48]; 

int ccdFrameNumber = 100;
int irFrameNumber = 100;
int result;

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

SDL_Surface *screen;	// Backbuffer
SDL_Surface *image;	// Image to put to the backbuffer
SDL_Surface *tempImage; // Temp image to write pixels to
SDL_Rect src, dest, ir_src, ir_dest, ccd_src, ccd_dest;	// src and dest regions for the blit
SDL_RWops *rwop;
SDL_Surface *tempFrame, *loadFrame;

bool loadImageCCD(string fileName)
{
  NOTICE << "Result frame received! ";
  // work around segfaults by coding up the jpg
  rwop=SDL_RWFromFile(fileName.c_str(), "rb");
  tempFrame = IMG_LoadJPG_RW(rwop);
  if(!tempFrame) 
    {
      cout << "IMG LoadJPG RW: " << IMG_GetError() << endl; 
      return false;
    }


  loadFrame = SDL_DisplayFormat(tempFrame);
  SDL_BlitSurface(loadFrame, &ccd_src, screen, &ccd_dest);
    SDL_Flip(screen);

        
  for(int i=0; i < FRAME_WIDTH*FRAME_HEIGHT; i++)
    {
      buffer[i][0] = (unsigned char)( (((unsigned int*)loadFrame->pixels)[i]>>16) & 0xFF);
      buffer[i][1] = (unsigned char)( (((unsigned int*)loadFrame->pixels)[i]>>8 ) & 0xFF);
      buffer[i][2] = (unsigned char)( (((unsigned int*)loadFrame->pixels)[i]    ) & 0xFF);
    }

   SDL_FreeSurface(tempFrame);
   SDL_FreeSurface(loadFrame);
  return true;
}


bool loadImageIR(string fileName)
{
  // work around segfaults by coding up the jpg
  rwop=SDL_RWFromFile(fileName.c_str(), "rb");
  tempFrame = IMG_LoadJPG_RW(rwop);
  if(!tempFrame) 
    {
      cout << "IMG LoadJPG RW: " << IMG_GetError() << endl; 
      return false;
    }


  loadFrame = SDL_DisplayFormat(tempFrame);
  SDL_BlitSurface(loadFrame, &ir_src, screen, &ir_dest);
    SDL_Flip(screen);


   

        
  for(int i=0; i < FRAME_WIDTH*FRAME_HEIGHT; i++)
    {
      buffer[i][0] = (unsigned char)( (((unsigned int*)loadFrame->pixels)[i]>>16) & 0xFF);
      buffer[i][1] = (unsigned char)( (((unsigned int*)loadFrame->pixels)[i]>>8 ) & 0xFF);
      buffer[i][2] = (unsigned char)( (((unsigned int*)loadFrame->pixels)[i]    ) & 0xFF);
    }

  SDL_FreeSurface(tempFrame);
    SDL_FreeSurface(loadFrame);
  return true;
}



int FilterEvents(const SDL_Event *event)
{
    if ( event->type == SDL_QUIT )
        exit(1);
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  VideoMarshal::receivedSelectedstream(selected_schema  *tuple)
{

//..............................................................................



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
        ((unsigned int*)tempImage->pixels)[i] |=  tuple->selected_frame[3*i  ];	      //R
        ((unsigned int*)tempImage->pixels)[i] |=  tuple->selected_frame[3*i+1] << 8;	//G
        ((unsigned int*)tempImage->pixels)[i] |=  tuple->selected_frame[3*i+2] << 16;	//B
    }


    image = SDL_DisplayFormat(tempImage);
    SDL_BlitSurface(image, &src, screen, &dest);
    SDL_Flip(screen);
    SDL_FreeSurface(tempImage);
    SDL_FreeSurface(image);

    if (irFrameNumber == NUM_FRAMES || ccdFrameNumber == NUM_FRAMES)
    {   NOTICE << "receivedResult:  Terminating ...";
        marshal.terminateClient();
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  VideoMarshal::sentCcdstream()
{

  //char fileChar[14];
  ostringstream os;
  string fileName;
//..............................................................................


    
    for (uint32  i = 0; i < BATCH_SIZE; i++)
    {   


        // This has to be in the loop scope so the constructor is rerun.
        Irstream  tuple;
	// incrementally get rid of this later
	
       	//sprintf(fileChar, "video1%03d.raw", irFrameNumber);

	
        os << "frames/video1" << std::setw(3) << std::setfill('0') << irFrameNumber << ".jpg";
        fileName = os.str();


	loadImageIR(fileName);

	
	tuple._data.ir_frame = (uint8 *) &buffer;

        batchIrstream(&tuple);
    }

    // Send a batch of tuples and delay if not done.
    //
    DEBUG << "call sendIrStream...";

    ++irFrameNumber;

    if (irFrameNumber <= NUM_FRAMES)
    {   sendIrstream(SLEEP_TIME);
    }
    else
    {   sendIrstream(0);
    }

    DEBUG << "Sent IR stream!";
    return;
}


////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  VideoMarshal::sentIrstream()
{
  ostringstream os;
  string fileName;
  //      char fileChar[14];
//..............................................................................

    
    for (uint32  i = 0; i < BATCH_SIZE; i++)
    {   



        // This has to be in the loop scope so the constructor is rerun.
        Ccdstream  tuple;

        strcpy(tuple._data.contrast, contrast);
	

       	os << "frames/video0" << std::setw(3) << std::setfill('0') << ccdFrameNumber << ".jpg";
        fileName = os.str();

	loadImageCCD(fileName);

	
	tuple._data.ccd_frame = (uint8 *)&buffer;

        
	batchCcdstream(&tuple);
    }

    DEBUG << "call sendCcdstream...";

    ++ccdFrameNumber;

    if (ccdFrameNumber <= NUM_FRAMES)
    {   sendCcdstream(SLEEP_TIME);
    }
    else
    {   sendCcdstream(0);
    }
    DEBUG << "Sent ccd stream!";
    return;
}

// do nothing placeholders
void VideoMarshal::sentJoinedstream()
{
  return;
}


void VideoMarshal::sentContraststream()
{
  return;
  }




////////////////////////////////////////////////////////////////////////////////
//
int  main(int  argc, const char  *argv[])
{
//  Return:  1 - Could not deploy the network.
//           2 - An invalid tuple value was received.
//..............................................................................



    // Blit src rectangle
    src.x = 0;
    src.y = 0;
    src.w = FRAME_WIDTH;
    src.h = FRAME_HEIGHT;

    // Blit destination rectangle
    dest.x = FRAME_WIDTH*2;	
    dest.y = 0;
    dest.w = FRAME_WIDTH;
    dest.h = FRAME_HEIGHT;


    ccd_src.x = 0;
    ccd_src.y = 0;
    ccd_src.w = FRAME_WIDTH;
    ccd_src.h = FRAME_HEIGHT;

    // Blit ccd_destination rectangle
    ccd_dest.x = 0;	
    ccd_dest.y = 0;
    ccd_dest.w = FRAME_WIDTH;
    ccd_dest.h = FRAME_HEIGHT;

    ir_src.x = 0;
    ir_src.y = 0;
    ir_src.w = FRAME_WIDTH;
    ir_src.h = FRAME_HEIGHT;

    // Blit ir_destination rectangle
    ir_dest.x = FRAME_WIDTH;	
    ir_dest.y = 0;
    ir_dest.w = FRAME_WIDTH;
    ir_dest.h = FRAME_HEIGHT;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
            cerr << "Unable to initialize SDL: " << SDL_GetError() << endl;
            exit(1);
    }

    atexit(SDL_Quit);
    SDL_SetEventFilter(FilterEvents);

    screen = SDL_SetVideoMode(DISP_WIDTH, FRAME_HEIGHT, 32, SDL_DOUBLEBUF);
    if (screen == NULL) 
    {
            cerr << "Unable to set video mode: " << SDL_GetError() << endl;
            exit(1);
    }
    SDL_WM_SetCaption("CCD                                                                                    IR                                                                                Result","output");
    SDL_WM_GrabInput(SDL_GRAB_OFF);


    for (uint8 x = 0; x < 48; ++x)
      contrast[x] = (char) x;
			     



    // Run the front-end, open a client, subscribe to outputs and inputs.
    result = marshal.open();



    if (result)
    {   WARN << "Could not deploy the network.";
        result = 1;
    }
    else
    {   DEBUG  << "time0 = " << time0;

        // Send the first batch of tuples.  Queue up the next round with a delay.
        marshal.sentIrstream();
	//marshal.sentCcdstream();
        DEBUG  << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    NOTICE << "process_video:  Done";

    return(result);
}
