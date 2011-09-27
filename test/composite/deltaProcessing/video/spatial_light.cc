#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <time.h>
#include "timer.h"

#include "SpatialMarshal.h"

using namespace Borealis;

#define  FRAME_WIDTH   320
#define  FRAME_HEIGHT  240
#define  FRAME_DEPTH     3

#define CHUNK_WIDTH 44
#define CHUNK_HEIGHT 48
#define CHUNK_DEPTH 3

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


int FilterEvents(const SDL_Event *event)
{
    if ( event->type == SDL_QUIT )
        exit(1);
    return 1;
}

const uint32  SLEEP_TIME  = 100;           // Delay between injections.
const uint32  BATCH_SIZE  =   1;           // Number of input tuples per batch.
const uint32  BATCH_COUNT =   3;           // Number batches to send.

#define FRAME_START 75
#define FRAME_FINISH 100

const Time  time0 = Time::now() - Time::msecs( 100 );
uint32 ccdFrameNumber = FRAME_START;
SDL_Surface *screen;    // Backbuffer
SDL_Surface *image;    // Image to put to the backbuffer
SDL_Surface *tempImage; // Temp image to write pixels to
SDL_Rect src_start, dest_start;    // src and dest regions for the blit
SDL_Rect src_result, dest_result;    // src and dest regions for the blit


SpatialMarshal   marshal;        // Client and I/O stream state.

uint32  result = 0;
uint32  total  = BATCH_COUNT * BATCH_SIZE;
uint32  remain = FRAME_FINISH - FRAME_START;

uint8   expect = 0;
uint8   *buffer;



int32 loadImageCCD(string fileName)
{
	SDL_RWops *rwop_display;
	uint32 size;

   ifstream fin(fileName.c_str(), ios::in | ios::binary | ios::ate);
   
  
  if(fin.is_open())
  {
  	size = fin.tellg();
  	
  	
  	buffer = new uint8[size];
  	
  	fin.seekg(0, ios::beg);
  	fin.read((char *) buffer, size);
  	fin.close();
  	
  	
    DEBUG << "Buffer BYTES: " << to_hex_string(((char *)buffer), 20) << " size=" << size;
    
  	
  	
  }
  else
  {
	DEBUG << "Read on " << fileName << " failed.\n";
        return -1;
   }
    // used to load FromFile
	rwop_display = SDL_RWFromMem(buffer, size);
    tempImage = IMG_LoadJPG_RW(rwop_display);
    if(tempImage == NULL)
      {
         	DEBUG << "CreateRGBSurface failed: " << SDL_GetError();
	        exit(1);
      }
    
    image = SDL_DisplayFormat(tempImage);
    SDL_BlitSurface(image, &src_start, screen, &dest_start);
    SDL_Flip(screen);

	
	
    SDL_FreeSurface(tempImage);
    SDL_FreeSurface(image);
    return size;
}

////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  SpatialMarshal::receivedCcddetected(ccd_schema  *tuple, int32 max_frame)
{
	SDL_RWops *rwop;
	uint32 size;
//..............................................................................
  //NOTICE << "Out values: " <<  (int)*(tuple->frame) << ", " <<  (int) *(tuple->frame + 1) << ", " <<  (int)  *(tuple->frame + 2);
	DEBUG << "Received chunk!";
	
      tempImage = SDL_CreateRGBSurface(SDL_SWSURFACE, FRAME_WIDTH, FRAME_HEIGHT, 32, rmask, gmask, bmask, amask);
    if(tempImage == NULL)
      {
	cerr << "CreateRGBSurface failed: " << SDL_GetError() << endl;
	exit(1);
      }
	
	size =  max_frame;
	DEBUG << "Size: " << size << " bytes: " << to_hex_string(tuple->frame, 20);
	uint8 *the_jpeg = tuple->frame;
	rwop = SDL_RWFromMem(the_jpeg, size);
	
	tempImage = IMG_LoadJPG_RW(rwop);
    image = SDL_DisplayFormat(tempImage);
    SDL_BlitSurface(image, &src_result, screen, &dest_result);
    SDL_Flip(screen);
    SDL_FreeSurface(tempImage);
    SDL_FreeSurface(image);  
	//sleep(1000);
    DEBUG << "Updated image!";
    if(tuple->final_bit == 1)
      {
        marshal.terminateClient();
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  SpatialMarshal::sentCcdstream()
{
    int32        timestamp;
    Time         current_time;
    ostringstream os;
    string fileName;
//..............................................................................


    current_time = Time::now();

    timestamp = (int32)( current_time - time0 ).to_secs();
    if ( timestamp < 0 ) timestamp = 0;
    //DEBUG  << "timestamp = " << timestamp << "  current_time = " << current_time;
    
    for (uint32  i = 0; i < BATCH_SIZE; i++)
    {   
      
        os << "/pro/borealis/data/demo/array/video1" << std::setw(3) << std::setfill('0') << ccdFrameNumber << "_resize.jpg";
	fileName = os.str();
	DEBUG << "Opening " << fileName;
	int32 size = loadImageCCD(fileName);
        // This has to be in the loop scope so the constructor is rerun.
        Ccdstream tuple;

	    
        tuple._data.frame = buffer;
	if(ccdFrameNumber == FRAME_FINISH - 1)
        	tuple._data.final_bit = 1;
        else 
         	tuple._data.final_bit = 0;

        
        // DEBUG  << "time=" << tuple._data.time << "  value=" << tuple._data.frame;
        batchCcdstream(&tuple, size);
	    ++ccdFrameNumber;
	    delete[] buffer;
    }

    // Send a batch of tuples and delay if not done.
    //
    //DEBUG << "call sendPacket...";

    remain--;

    if (remain)
    {   
    	sendCcdstream(SLEEP_TIME);
    }
    else
    {   sendCcdstream(0);
    }

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
int  main(int  argc, const char  *argv[])
{
//  Return:  1 - Could not deploy the network.
//           2 - An invalid tuple value was received.
//..............................................................................
	int total_time;
	Time current_time;
	timer theTotalTimeFinerGrain;
	theTotalTimeFinerGrain.start();
  
      // Blit src rectangle
    src_start.x = 0;
    src_start.y = 0;
    src_start.w = FRAME_WIDTH;
    src_start.h = FRAME_HEIGHT;    
    
    // Blit destination rectangle
    dest_start.x = 0;    
    dest_start.y = 0;
    dest_start.w = FRAME_WIDTH;
    dest_start.h = FRAME_HEIGHT;

      // Blit src rectangle
    src_result.x = 0;
    src_result.y = 0;
    src_result.w = FRAME_WIDTH;
    src_result.h = FRAME_HEIGHT;    
    
    // Blit destination rectangle
    dest_result.x = FRAME_WIDTH;    
    dest_result.y = 0;
    dest_result.w = FRAME_WIDTH;
    dest_result.h = FRAME_HEIGHT;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        cerr << "Unable to initialize SDL: " << SDL_GetError() << endl;
        exit(1);
    }
    
    atexit(SDL_Quit);
    SDL_SetEventFilter(FilterEvents);
    
    screen = SDL_SetVideoMode(FRAME_WIDTH*2, FRAME_HEIGHT, 32, SDL_DOUBLEBUF);
    if (screen == NULL)
    {
        cerr << "Unable to set video mode: " << SDL_GetError() << endl;
        exit(1);
    }
    
    SDL_WM_SetCaption("Chunk Demo","chunk");
    SDL_WM_GrabInput(SDL_GRAB_OFF);    


    // Run the front-end, open a client, subscribe to outputs and inputs.
    result = marshal.open();

    if (result)
    {   WARN << "Could not deploy the network.";
        result = 1;
    }
    else
    {   DEBUG  << "time0 = " << time0;



        // Send the first batch of tuples.  Queue up the next round with a delay.
        marshal.sentCcdstream();

        DEBUG  << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }
	total_time = Time::now().to_secs() - time0.to_secs();
    NOTICE << "validate_chunk: total time: " << total_time;
    theTotalTimeFinerGrain.stop();
    theTotalTimeFinerGrain.check("Clocked ");
    return(result);
}
