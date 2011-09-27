#include "AisMarshal.h"
#include <fstream>
#include "csvparser.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

using namespace Borealis;

// one ship rec = 12 bytes
typedef struct sr {
	float lat;
	float lon;
	int id;
} ship_rec;


#define  FRAME_WIDTH   640
#define  FRAME_HEIGHT  480
#define  FRAME_DEPTH     3

const uint32  SLEEP_TIME  = 100;           // Delay between injections.
const uint32  BATCH_SIZE  =   1;           // Number of input tuples per batch.
const uint32  BATCH_COUNT =   10;           // Number batches to send.
const uint32  MAX_VALS_PER_TUPLE    = 1000;
const Time  time0 = Time::now() - Time::msecs( 100 );

AisMarshal   marshal;        // Client and I/O stream state.

uint32  result = 0;
uint32  total  = BATCH_COUNT * BATCH_SIZE-2;
uint32  remain_w = BATCH_COUNT;
uint32  remain_e = BATCH_COUNT;

uint8 flat_vals_w[MAX_VALS_PER_TUPLE*sizeof(ship_rec)];
uint8 flat_vals_e[MAX_VALS_PER_TUPLE*sizeof(ship_rec)];

int32 e_time_start=1141189200, w_time_start=1141189200;
int32 stop_timestamp = 2141189200; 
      
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

SDL_Surface *screen;    // Backbuffer
SDL_Surface *image;    // Image to put to the backbuffer
SDL_Surface *tempImage; // Temp image to write pixels to
SDL_Rect start, finish;    // src and dest regions for the blit

uint8   buffer[FRAME_WIDTH*FRAME_HEIGHT][FRAME_DEPTH];



 	ship_rec the_ship;
 	bool first_run_w = true;
 	bool first_run_e = true;
 	vector<ship_rec> value_w, value_e;
 	string w_file_name("/u/jennie/AIS/20060301_west_clean.csv");
 	string e_file_name("/u/jennie/AIS/20060301_east_clean.csv");
 	ifstream w_data_file(w_file_name.c_str());
 	ifstream e_data_file(e_file_name.c_str());
 	unsigned int next_time_w = 0;
 	unsigned int next_time_e = 0;
 	
 bool parse_record(string rec, ship_rec &formatted_rec, unsigned int &time)
 {
    CSVParser parser;
	int field1, field6; 
        float field2, field3, field4, field5; 
	//cout << "Got string: " << rec << endl;
	parser << rec;
	parser >> field1 >> field2 >> field3 >> field4 >> field5 >> field6;
	formatted_rec.id = field1;
	formatted_rec.lat = field4;
	formatted_rec.lon = field5;
	time = field6;
	//cout << "Decoded record: " << formatted_rec.id << ", " << formatted_rec.course << ", " << formatted_rec.speed << ", " << formatted_rec.lat << ", " << formatted_rec.lon << " time: " << time << end;l
	return false;
 }	
 
 ship_rec get_next_ship(unsigned int &time, bool west)
 {
 	bool reject = true;
 	string record_line;
 	
	if(west)
	{
		if(!w_data_file.eof())
		{
			getline(w_data_file, record_line);
			reject = parse_record(record_line, the_ship, time);
		}
		else
			exit(0);
	}
	else
	{
		if(!e_data_file.eof())
		{
			getline(e_data_file, record_line);
			reject = parse_record(record_line, the_ship, time);
		}
		else
			exit(0);
	
				
 	 }	
 	 //cout << "Got record!";
 	 return the_ship;
 }

void insert_at_pos_west(ship_rec new_one, int pos)
{
  int old_len = value_w.size();
  ship_rec last_val = value_w[old_len-1];
  //cout << "last lat/lon: " << last_val.lat << ", " << last_val.lon << " inserting at " << pos << endl;;
  value_w.resize(old_len+1);
  value_w[old_len] = last_val;
  //value.push_back(last_val);
  
  // shift ones in front forward a slot

  for(int x = old_len-2; x >= pos; --x)
    value_w[x+1] = value_w[x];
  value_w[pos] = new_one;
}

void insert_at_pos_east(ship_rec new_one, int pos)
{
  int old_len = value_e.size();
  ship_rec last_val = value_e[old_len-1];
  //cout << "last lat/lon: " << last_val.lat << ", " << last_val.lon << " inserting at " << pos << endl;;
  value_e.resize(old_len+1);
  value_e[old_len] = last_val;
  //value.push_back(last_val);
  
  // shift ones in front forward a slot

  for(int x = old_len-2; x >= pos; --x)
    value_e[x+1] = value_e[x];
  value_e[pos] = new_one;
}
// is the first one less than the second in ordering?
bool is_less_than(ship_rec first, ship_rec second)
{
  /*if(first.lat < second.lat)
    return true;
  if(first.lat == second.lat && first.lon < second.lon)
    return true;
  return false;
  */
  if(first.id < second.id)
  	return true;
  return false;
  	
}

/*void print_current_list()
{
  
  for(uint32 x = 0; x < value.size(); ++x)
	cout << "          Record: " << value[x].id << ", " << value[x].course << ", " << value[x].speed << ", " << value[x].lat << ", " << value[x].lon << endl;;
}*/

void insert_into_sorted_list_west(ship_rec ship)
{
  //cout << "Before insert of point: " << ship.lat << ", " << ship.lon << endl;
  //print_current_list();
  if(value_w.size() == 0)
    {
      value_w.push_back(ship);
      return;
    }

  
  uint32 x = 0; 
  while(is_less_than(value_w[x], ship) && x < value_w.size())
    {
      ++x;
    }

   insert_at_pos_west(ship, x);
   
   //cout << "After insert of point: " << ship.lat << ", " << ship.lon << endl;
   //print_current_list();

}

void insert_into_sorted_list_east(ship_rec ship)
{
  //cout << "Before insert of point: " << ship.lat << ", " << ship.lon << endl;
  //print_current_list();
  if(value_e.size() == 0)
    {
      value_e.push_back(ship);
      return;
    }

  
  uint32 x = 0; 
  while(is_less_than(value_e[x], ship) && x < value_e.size())
    {
      ++x;
    }

   insert_at_pos_east(ship, x);
   
   //cout << "After insert of point: " << ship.lat << ", " << ship.lon << endl;
   //print_current_list();

}
 // get a set of ships for one timestamp
 // may refine later to deal in AIS epochs
 void get_ships(uint32 &time, bool west)
 {
 	string record_line;
 	unsigned int rec_time;
 	
 	if(west)
 	{
 		value_w.clear();
 		rec_time = time = next_time_w;
 		// dealing in 10 second increments
	 	while(time+10 >= rec_time || first_run_w == true)
 			{
 				if(!first_run_w && the_ship.lon < -100.0)	
				  insert_into_sorted_list_west(the_ship);
			   			
 				the_ship = get_next_ship(rec_time, west);
 				if(first_run_w)
 				{
 					time = rec_time;
	 				first_run_w = false;
 				}
 			}	
 	
 		next_time_w = rec_time;
 	}
 	else
 	{
 		value_e.clear();
 		rec_time = time = next_time_e;
 		// dealing in 10 second increments
	 	while(time+10 >= rec_time || first_run_e == true)
 			{
 				if(!first_run_e && the_ship.lon >= -100.0)	
				  insert_into_sorted_list_east(the_ship);
			   			
 				the_ship = get_next_ship(rec_time, west);
 				if(first_run_e)
 				{
 					time = rec_time;
	 				first_run_e = false;
 				}
 			}	
 	
 		next_time_e = rec_time;
 	}
 }



 // return # of values
 int flatten_vals(bool west)
 {
 	int ship_size = sizeof(ship_rec); 
 	ostringstream positions, the_ids;
 	ship_rec current_record;
 	
 	if(west)
 	{
 		uint32 num_vals = value_w.size();
 		uint8 *ship_rec;
 		if(num_vals > MAX_VALS_PER_TUPLE)
 			num_vals = MAX_VALS_PER_TUPLE;
 	
 		for(uint32 x = 0; x < num_vals; ++x)
 		{
 			current_record = value_w[x];
 			positions << "(" << current_record.lat << ", " << current_record.lon << ") ";
			the_ids << current_record.id << ", ";
 			ship_rec = (uint8 *) &(value_w[x]);
 			memcpy(flat_vals_w + x*ship_size, ship_rec, ship_size);
 		}
 		DEBUG << "West sending positions: " << positions.str();
 		DEBUG << "West Sending ids: " << the_ids.str();
 		//DEBUG << "First elements: " << to_hex_string(flat_vals_w, 40);
 	
 		return num_vals;
 	}
 	else
 	{
 		uint32 num_vals = value_e.size();
 		uint8 *ship_rec;
 		if(num_vals > MAX_VALS_PER_TUPLE)
 			num_vals = MAX_VALS_PER_TUPLE;
 	
 		for(uint32 x = 0; x < num_vals; ++x)
 		{
 			current_record = value_e[x];
 			positions << "(" << current_record.lat << ", " << current_record.lon << ") ";
			the_ids << current_record.id << ", ";
 			ship_rec = (uint8 *) &(value_e[x]);
 			memcpy(flat_vals_e + x*ship_size, ship_rec, ship_size);
 		}
 		DEBUG << "East sending positions: " << positions.str();
 		DEBUG << "East Sending ids: " << the_ids.str();
 		//DEBUG << "First elements: " << to_hex_string(flat_vals_e, 40);
 	
 		return num_vals;
 	}
 	 
 }
////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//

void  AisMarshal::receivedUs_Map(rendered_map  *tuple)
{
//..............................................................................

    NOTICE << "receivedMoving " <<   " time= " << tuple->time << " value=" << (int)*(tuple->map) << " remain=" << total;
    
    tempImage = SDL_CreateRGBSurface(SDL_SWSURFACE, FRAME_WIDTH, FRAME_HEIGHT, 32, rmask, gmask, bmask, amask);
    if(tempImage == NULL)
      {
	cerr << "CreateRGBSurface failed: " << SDL_GetError() << endl;
	exit(1);
      }

	for(int x = 0; x < FRAME_WIDTH*FRAME_HEIGHT; ++x)
		{
	//ARGB to ABGR
		((unsigned int*)tempImage->pixels)[x]  =  255 << 24;    //A
		((unsigned int*)tempImage->pixels)[x] |=  tuple->map[3*x];    //R
		((unsigned int*)tempImage->pixels)[x] |=  tuple->map[3*x+1] << 8;    //G
		((unsigned int*)tempImage->pixels)[x] |=  tuple->map[3*x+2] << 16;    //B
      }
    image = SDL_DisplayFormat(tempImage);
    SDL_BlitSurface(image, &start, screen, &finish);
    SDL_Flip(screen);
    SDL_FreeSurface(tempImage);
    SDL_FreeSurface(image);
    //sleep(2);  
    DEBUG << "Updated image!";
    
    
    total--;

    if (total == 0 || tuple->time >= stop_timestamp)
    {   NOTICE << "receivedResult:  Terminating ...";
    	w_data_file.close();
    	e_data_file.close();
        marshal.terminateClient();
    }

    //return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//

void  AisMarshal::sentIncoming_West()
{
	uint32 timestamp;
	int num_vals;
//..............................................................................
    
    for (uint32  i = 0; i < BATCH_SIZE; i++)
    {   
        
		get_ships(timestamp, true);
        // This has to be in the loop scope so the constructor is rerun.
        Incoming_West  tuple;

        tuple._data.time  = timestamp-1;//inclusive
        num_vals = flatten_vals(true);     	    
        tuple._data.ships = flat_vals_w;
	        
        
        DEBUG  << "west sent time=" << tuple._data.time << "  number of ships: " << num_vals;
        batchIncoming_West(&tuple, num_vals);
    }

    // Send a batch of tuples and delay if not done.
    //
    //DEBUG << "call sendIncoming...";

    remain_w--;
    //DEBUG << "remain west: " << remain;
    if (remain_w > 0)
    {   sendIncoming_West(SLEEP_TIME);
    }
    else
    {  
    	stop_timestamp = timestamp;
    	 sendIncoming_West(0);
    }

    return;
}


void  AisMarshal::sentIncoming_East()
{
	uint32 timestamp;
	int num_vals;
//..............................................................................
    
    for (uint32  i = 0; i < BATCH_SIZE; i++)
    {   
        
		get_ships(timestamp, false);
        // This has to be in the loop scope so the constructor is rerun.
        Incoming_East  tuple;

        tuple._data.time  = timestamp-1;
                
        num_vals = flatten_vals(false); 
        DEBUG  << "east sent time=" << tuple._data.time << "  number of ships: " << num_vals;
        tuple._data.ships = flat_vals_e;
        
        // DEBUG  << "time=" << tuple._data.time << "  value=" << tuple._data.value;
        batchIncoming_East(&tuple, num_vals);
    }

    // Send a batch of tuples and delay if not done.
    //
    //DEBUG << "call sendIncoming...";

    remain_e--;
	//DEBUG << "remain east: " << remain;
    if (remain_e > 0)
    {   sendIncoming_East(SLEEP_TIME);
    }
    else
    {   
    	stop_timestamp = timestamp;
    	sendIncoming_East(0);
    }

    return;
}



void  AisMarshal::sentMapped_West()
{
}

void  AisMarshal::sentMapped_East()
{
}
void AisMarshal::sentTime_Snapshot()
{
}

////////////////////////////////////////////////////////////////////////////////
//
int  main(int  argc, const char  *argv[])
{
//  Return:  1 - Could not deploy the network.
//           2 - An invalid tuple value was received.
//..............................................................................


    // Run the front-end, open a client, subscribe to outputs and inputs.
    
    start.x = 0;
    start.y = 0;
    start.w = FRAME_WIDTH;
    start.h = FRAME_HEIGHT;    
    
    // Blit destination rectangle
    finish.x = 0;    
    finish.y = 0;
    finish.w = FRAME_WIDTH;
    finish.h = FRAME_HEIGHT;
    
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
    
    SDL_WM_SetCaption("AIS Map","Ships");
    SDL_WM_GrabInput(SDL_GRAB_OFF);    
    
    
    result = marshal.open();
    
	if(!w_data_file.is_open() || !e_data_file.is_open())
		FATAL << "File failed to open!";
	
		
    if (result)
    {   WARN << "Could not deploy the network.";
        result = 1;
    }
    else
    {           // Send the first batch of tuples.  Queue up the next round with a delay.
        marshal.sentIncoming_West();
        // TODO: re-add this
        marshal.sentIncoming_East();

        DEBUG  << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    NOTICE << "validate_ais:  Done";

    return  result;
}
