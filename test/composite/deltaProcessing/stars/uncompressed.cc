#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <Magick++.h>
#include <time.h>

#include "UncompressedMarshal.h"

using namespace Borealis;

#define  FRAME_WIDTH   1024
#define  FRAME_HEIGHT  745
#define  FRAME_DEPTH     1 // temporarily 1, but hack for now
#define OUTPUT_FRAME_WIDTH 1021
#define OUTPUT_FRAME_HEIGHT 738
#define OUTPUT_FRAME_DEPTH 3
#define SCROLL_HEIGHT 2788
#define DISPLAY_WIDTH 342 
#define DISPLAY_HEIGHT 929 //divide by 3


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

const uint32  SLEEP_TIME  = 1000;           // Delay between injections.
const uint32  BATCH_SIZE  =   1;           // Number of input tuples per batch.
const uint32  BATCH_COUNT =   3;           // Number batches to send.

#define FRAME_START 13
#define FRAME_FINISH 23

const Time  time0 = Time::now() - Time::msecs( 100 );
uint32 i_frame_number = FRAME_START;
uint32 r_frame_number = FRAME_START;
uint32 g_frame_number = FRAME_START;
SDL_Surface *screen;    // Backbuffer
SDL_Surface *image;    // Image to put to the backbuffer
SDL_Surface *tempImage; // Temp image to write pixels to
SDL_Rect src_result, dest_result;    // src and dest regions for the blit


UncompressedMarshal   marshal;        // Client and I/O stream state.

uint32  result = 0;
uint32  total  = BATCH_COUNT * BATCH_SIZE;
uint32  i_remain = FRAME_FINISH - FRAME_START+1;
uint32  r_remain = FRAME_FINISH - FRAME_START+1;
uint32  g_remain = FRAME_FINISH - FRAME_START+1;

uint8   expect = 0;
uint8   i_buffer[FRAME_WIDTH*FRAME_HEIGHT];
uint8   r_buffer[FRAME_WIDTH*FRAME_HEIGHT];
uint8   g_buffer[FRAME_WIDTH*FRAME_HEIGHT];


void print_gray(uint8 *array, int x, int y)
{
	int offset = y*FRAME_WIDTH + x;
	DEBUG << (int) array[offset] << ", ";
}

void print_gray_checkpoint(uint8 *array)
{
	for(int x = 918; x < 922; ++x)
		print_gray(array, x, 795);
}

/*bool load_image(string filename, uint8 *out_buffer, bool search_flag)
{
	char dummy[50], current_value[10];
  ifstream fin(filename.c_str(), ios::in | ios::binary);
  // pull the header off the stream
  //DEBUG << "Header: ";
  for (int x = 0; x < 4; ++x)
  {
  	fin.getline(dummy, 50);
  	//DEBUG << "    " << dummy;
  }

  for(int x = 0; x < FRAME_WIDTH*FRAME_HEIGHT; ++x)
  	{	
  		fin.getline(current_value, 10);
  		
  		out_buffer[x] = (uint8) atoi(&current_value[0]);
  			
  	}

  //DEBUG << "First values: " << (int) out_buffer[0] << ", " << (int) out_buffer[1] << ", " << (int) out_buffer[2] << ", " <<  (int) out_buffer[3] << ", " <<  (int) out_buffer[4] ;   	
    if (fin.fail())
    {
        cout << "Read on " << filename << " failed.\n";
        return false;
    }
    else 
      fin.close();



    return true;
}*/

void fill_pgm(string filename, uint8 *channel)
{
	ifstream fin(filename.c_str(), ios::in | ios::binary | ios::ate);
	int size;
    char header[50];
  if(fin.is_open())
  {
        size = fin.tellg();
	    int header_size = size - FRAME_WIDTH*FRAME_HEIGHT;
		int rem_size = FRAME_WIDTH*FRAME_HEIGHT;

        fin.seekg(0, ios::beg);
        fin.read(header, header_size);
        fin.read((char *)channel, rem_size);
        fin.close();
  }
  if(fin.fail())
    DEBUG  << "Read failed on file: " << filename;
}

////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  UncompressedMarshal::receivedScroller(scrolling_frame  *tuple)
{
//..............................................................................
  NOTICE << "Out values: " <<  (int)*(tuple->scroll_frame) << ", " <<  (int) *(tuple->scroll_frame + 1) << ", " <<  (int)  *(tuple->scroll_frame + 2);
  NOTICE << "Sequence number: " << tuple->seq_no;
	
    tempImage = SDL_CreateRGBSurface(SDL_SWSURFACE, DISPLAY_WIDTH, DISPLAY_HEIGHT, 32, rmask, gmask, bmask, amask);
    if(tempImage == NULL)
      {
	cerr << "CreateRGBSurface failed: " << SDL_GetError() << endl;
	exit(1);
      }
	int num_pixels = DISPLAY_WIDTH*DISPLAY_HEIGHT;

	// TODO: change output_frame_height to SCROLL_HEIGHT for last box
	Magick::Image the_img(OUTPUT_FRAME_WIDTH, SCROLL_HEIGHT, "RGB", Magick::CharPixel, tuple->scroll_frame);
	the_img.scale(Magick::Geometry(DISPLAY_WIDTH, DISPLAY_HEIGHT));
	// Recycling the scroll frame is bad form, make this a dedicated buffer eventually
	the_img.write(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, "RGB", Magick::CharPixel, tuple->scroll_frame);
	for(int x = 0; x < num_pixels; ++x)
		{
		    //ARGB to ABGR
			((unsigned int*)tempImage->pixels)[x]  =  255 << 24;    //A
			((unsigned int*)tempImage->pixels)[x] |=  tuple->scroll_frame[3*x];    //R
			((unsigned int*)tempImage->pixels)[x] |=  tuple->scroll_frame[3*x+1] << 8;    //G
			((unsigned int*)tempImage->pixels)[x] |=  tuple->scroll_frame[3*x+2] << 16;    //B
			
      	}
     
    image = SDL_DisplayFormat(tempImage);
    SDL_BlitSurface(image, &src_result, screen, &dest_result);
    SDL_Flip(screen);
    SDL_FreeSurface(tempImage);
    SDL_FreeSurface(image);  

    DEBUG << "Displayed image";
    DEBUG << "Comparing " << tuple->seq_no << " to " << FRAME_FINISH - FRAME_START;
    if(tuple->seq_no >= (FRAME_FINISH - FRAME_START))
      {
        marshal.terminateClient();
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  UncompressedMarshal::sentI_Frame()
{

    Time         current_time;
    ostringstream os;
    string filename;
//..............................................................................


    
    for (uint32  i = 0; i < BATCH_SIZE; i++)
    {   
        os << "/pro/borealis/data/demo/array/stars/smaller/fpC-000094-i1-" << std::setw(4) << std::setfill('0') << i_frame_number << ".fit.pgm";
        
	filename = os.str();
	DEBUG << "Opening " << filename;
	fill_pgm(filename, i_buffer);
	if(i_frame_number == FRAME_START)
	{
		DEBUG << "I grayscale pix: ";
		print_gray_checkpoint(i_buffer);
	}
	
    // This has to be in the loop scope so the constructor is rerun.
    I_Frame tuple;

        tuple._data.frame = (uint8 *)&(i_buffer[0]);
        tuple._data.seq_no = i_frame_number - FRAME_START;
        DEBUG << "Sequence number: " <<  tuple._data.seq_no;
        

        // DEBUG  << "time=" << tuple._data.time << "  value=" << tuple._data.frame;
        batchI_Frame(&tuple);
		DEBUG << "Batched frame number: " << i_frame_number;
		++i_frame_number;
		
    }

    // Send a batch of tuples and delay if not done.
    //
    //DEBUG << "call sendPacket...";

    i_remain--;

    if (i_remain)
    {   
    	
    	sendI_Frame(SLEEP_TIME);
    }
    else
    {   
    	DEBUG << "Done sending i frames!";
    	sendI_Frame(0);
    }

    return;
}

////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  UncompressedMarshal::sentR_Frame()
{

    Time         current_time;
    ostringstream os;
    string filename;
//..............................................................................


    
    for (uint32  i = 0; i < BATCH_SIZE; i++)
    {   
        os << "/pro/borealis/data/demo/array/stars/smaller/fpC-000094-r1-" << std::setw(4) << std::setfill('0') << r_frame_number << ".fit.pgm";
	filename = os.str();
	DEBUG << "Opening " << filename;
	fill_pgm(filename, r_buffer);
	if(r_frame_number == FRAME_START)
	{
		DEBUG << "R grayscale pix: ";
		print_gray_checkpoint(r_buffer);
	}
    // This has to be in the loop scope so the constructor is rerun.
    R_Frame tuple;

        tuple._data.frame = (uint8 *)&(r_buffer[0]);
        tuple._data.seq_no = r_frame_number - FRAME_START; 
        

        // DEBUG  << "time=" << tuple._data.time << "  value=" << tuple._data.frame;
        batchR_Frame(&tuple);
		DEBUG << "Batched frame number: " << r_frame_number;
		++r_frame_number;
		
    }

    // Send a batch of tuples and delay if not done.
    //
    //DEBUG << "call sendPacket...";

    r_remain--;

    if (r_remain)
    {   
    	
    	sendR_Frame(SLEEP_TIME);
    }
    else
    {   sendR_Frame(0);
    	DEBUG << "Done sending r frames!";
    }

    return;
}

void  UncompressedMarshal::sentG_Frame()
{

    ostringstream os;
    string filename;
//..............................................................................


    
    for (uint32  i = 0; i < BATCH_SIZE; i++)
    {   
        os << "/pro/borealis/data/demo/array/stars/smaller/fpC-000094-g1-" << std::setw(4) << std::setfill('0') << g_frame_number << ".fit.pgm";
        
	filename = os.str();
	DEBUG << "Opening " << filename;
	fill_pgm(filename, g_buffer);
    // This has to be in the loop scope so the constructor is rerun.
    G_Frame tuple;

        tuple._data.frame = (uint8 *)&(g_buffer[0]);
        tuple._data.seq_no = g_frame_number - FRAME_START; 


        // DEBUG  << "time=" << tuple._data.time << "  value=" << tuple._data.frame;
        batchG_Frame(&tuple);
		DEBUG << "Batched frame number: " << g_frame_number;
		++g_frame_number;
		
    }

    // Send a batch of tuples and delay if not done.
    //
    //DEBUG << "call sendPacket...";

    g_remain--;

    if (g_remain)
    {   
    	
    	sendG_Frame(SLEEP_TIME);
    }
    else
    {   sendG_Frame(0);
    }

    return;
} 

void  UncompressedMarshal::sentO_Frame_1()
{
}

void  UncompressedMarshal::sentO_Frame_2()
{
} 
////////////////////////////////////////////////////////////////////////////////
//
int  main(int  argc, const char  *argv[])
{
//  Return:  1 - Could not deploy the network.
//           2 - An invalid tuple value was received.
//..............................................................................
	Time current_time;
  
          // Blit src rectangle
    src_result.x = 0;
    src_result.y = 0;
    src_result.w = DISPLAY_WIDTH;
    src_result.h = DISPLAY_HEIGHT;    
    
    // Blit destination rectangle
    dest_result.x = 0;    
    dest_result.y = 0;
    dest_result.w = DISPLAY_WIDTH;
    dest_result.h = DISPLAY_HEIGHT;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        cerr << "Unable to initialize SDL: " << SDL_GetError() << endl;
        exit(1);
    }
    
    atexit(SDL_Quit);
    SDL_SetEventFilter(FilterEvents);
    
    screen = SDL_SetVideoMode(DISPLAY_WIDTH, DISPLAY_HEIGHT, 32, SDL_DOUBLEBUF);
    if (screen == NULL)
    {
        cerr << "Unable to set video mode: " << SDL_GetError() << endl;
        exit(1);
    }
    
    SDL_WM_SetCaption("Uncompressed Demo","Stars");
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
        marshal.sentI_Frame();
        marshal.sentR_Frame();
        marshal.sentG_Frame();

        DEBUG  << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }
    return(result);
}
