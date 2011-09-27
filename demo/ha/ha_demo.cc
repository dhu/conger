#include <iostream>
#include <sstream>
#include <fstream>

#include "Ha_DemoMarshal.h"

int group_by_values = 100;
float speed = 8;
int step_size = 5;  // real time (millisec)
bool increasing_rate = true;
//bool increasing_rate = false;

double t = 1;
//double max_time = 95;
//double max_rate = 900;

//double max_time = 300;
double max_time = 150;
double max_rate = 540;
//double max_rate = 1200;

using namespace Borealis;

Ha_DemoMarshal marshal;

string ip = "127.0.0.1";     // client host ip.
int port = DEFAULT_BOREALIS_PORT;           // client (and input) port.
FILE * pFile;
ofstream file_out;
#define TRACE_FILE_NAME "lbl-tcp-3.tcp"

char buffer[200];

long long start_time;
Time now1;
Time now2;

////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void Ha_DemoMarshal::receivedOutput(CountSchema *tuple) 
{
    Timestamp timestamp(*((timeval*)((char*)tuple - HEADER_SIZE)));
//    long long t1 = (long long)(static_cast<double>(tv.tv_sec) *1000 + static_cast<double>(tv.tv_usec) / 1000.0); 
    ostringstream string_out;
    // streamid, sourceid, start time of run, timestamp, delay
    file_out << tuple->agg_id << ", " << tuple->source << ", " << start_time << ", " << timestamp.to_msecs() << ", " << ((Timestamp::now()-timestamp).to_msecs()) << endl;
//    NOTICE << tuple->agg_id << ", " << tuple->source << ", " << start_time << ", " << t1 << ", " << (Time::now().to_msecs()-t1) << endl;
}

long target_timestamp1 = step_size;
long target_timestamp2 = step_size;
float timestamp1 = 0;
float timestamp2 = 0;
float timestamp_base = 0;

int source;
int destination;
int s_port;
int d_port;
int data_size;

Ha_DemoMarshal::Input1 tuple1;
Ha_DemoMarshal::Input2 tuple2;

Time next_send_time1 = Time::now() + Time::msecs(step_size);
Time next_send_time2 = Time::now() + Time::msecs(step_size);

long sum_send_intervals1 = 0;
int count_send_intervals1 = 0;
long sum_send_intervals2 = 0;
int count_send_intervals2 = 0;
int enqueue_count1 = 0;
int enqueue_count2 = 0;
int sequence_number = 0;
double interval = 0;

void read_type_1_tuple()
{
    float tt;
    while (fscanf(pFile, "%f %d %d %d %d %d", &tt, &source, &destination, &s_port,
    &d_port, &data_size) < 6) 
    {
        fseek(pFile, 0, SEEK_SET);
        timestamp_base = timestamp1;
    }
    if (increasing_rate)
    {
        interval = max_time/max_rate/t;
        if (t > max_time)
            interval = 1/max_rate;
        timestamp1 = t;
        t += interval;
    }
    else
    {
        timestamp1 = timestamp_base + tt/speed;
    }
    tuple1._data.source = (int)(source);
    tuple1._data.timestamp = (int)(timestamp1);
}

void read_type_2_tuple()
{
    tuple2._data.source = sequence_number % group_by_values;
    timestamp2 = 1.0*sequence_number/group_by_values;
    tuple2._data.timestamp = (int)(timestamp2);
    sequence_number++;
}

void send_type_1_tuple() 
{
  if (target_timestamp1 != step_size) {  // if not first
    enqueue_count1++;
    marshal.batchInput1(&tuple1);
//    NOTICE << "input: (" << tuple._data.timestamp1 << ", " << tuple._data.size << ")";
  }
  for(;;) {
    read_type_1_tuple();
    if (timestamp1*1000 > target_timestamp1) {
        while(timestamp1*1000 > target_timestamp1)
        {
            int dec = target_timestamp1/1000;
            target_timestamp1 += step_size;
            if (dec != target_timestamp1/1000)
            {
                float avg = 0;
                if (count_send_intervals1 > 0) 
                    avg = (sum_send_intervals1/count_send_intervals1);
                if (increasing_rate)
                    NOTICE << "timestamp = " << target_timestamp1/1000 
                        << ", sleep time =  " << avg
                        << ", data rate =  " << get_formatted_number(1.0/interval, 2);
                else
                    NOTICE << "timestamp = " << target_timestamp1/1000 
                        << ", sleep time =  " << avg;
                count_send_intervals1 = 0;
                sum_send_intervals1 = 0;
            }
            next_send_time1 = next_send_time1 + Time::msecs(step_size);
        }
        long sleep_time = (next_send_time1 - Time::now()).to_msecs();
//        NOTICE << sleep_time;
        if (sleep_time <= 0)
        {
            sleep_time = 1;
        }
        now1 = Time::now();
        if (enqueue_count1 == 0)
        {
            marshal.batchInput1(&tuple1); // duplication: need to be fixed later one. but crucial.
        }
        marshal.sendInput1(sleep_time);
        enqueue_count1 = 0;
        break;
    }
    enqueue_count1++;
    marshal.batchInput1(&tuple1);
//    NOTICE << "input: (" << tuple._data.timestamp1 << ", " << tuple._data.size << ")";
  } 
}

void send_type_2_tuple() 
{
  if (target_timestamp2 != step_size) {  // if not first
    enqueue_count2++;
    marshal.batchInput2(&tuple2);
//    NOTICE << "input: (" << tuple._data.timestamp2 << ", " << tuple._data.size << ")";
  }
  for(;;) {
    read_type_2_tuple();
    if (timestamp2*1000 > target_timestamp2) {
        while(timestamp2*1000 > target_timestamp2)
        { 
            int dec = target_timestamp2/1000;
            target_timestamp2 += step_size;
            if (dec != target_timestamp2/1000)
            {
                float avg = 0;
                if (count_send_intervals2 > 0) 
                    avg = (sum_send_intervals2/count_send_intervals2);
                NOTICE << "timestamp = " << target_timestamp2/1000 
                       << ", sleep time =  " << avg;
                count_send_intervals2 = 0;
                sum_send_intervals2 = 0;
            }
            next_send_time2 = next_send_time2 + Time::msecs(step_size);
        }
        long sleep_time = (next_send_time2 - Time::now()).to_msecs();
//        NOTICE << sleep_time;
        if (sleep_time <= 0)
        {
            sleep_time = 1;
        }
        now2 = Time::now();
        if (enqueue_count2 == 0)
        {
            marshal.batchInput2(&tuple2); // duplication: need to be fixed later on. but crucial.
        }
        marshal.sendInput2(sleep_time);
        enqueue_count2 = 0;
        break;
    }
    enqueue_count2++;
    marshal.batchInput2(&tuple2);
//    NOTICE << "input: (" << tuple._data.timestamp2 << ", " << tuple._data.size << ")";
  }
}

////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void Ha_DemoMarshal::sentInput1()
{
  long sleep_time = (Time::now() - now1).to_msecs();
  sum_send_intervals1+= sleep_time;
  count_send_intervals1++;
  send_type_1_tuple();
}

void Ha_DemoMarshal::sentInput2()
{
  long sleep_time = (Time::now() - now2).to_msecs();
  sum_send_intervals2 += sleep_time;
  count_send_intervals2++;
  send_type_2_tuple();
}

////////////////////////////////////////////////////////////////////////////////
//
int main(int argc, const char *argv[])
{

    int32  status;

    // Run the front-end, open a client, subscribe to outputs and inputs.
    status = marshal.open();

    if (status) {
        WARN << "Could not deploy the network.";
    }
    else {

        pFile = fopen (TRACE_FILE_NAME,"r");

        char* iteration = getenv("BOREALIS_CASE_ID");
        string file_name = "output.";
        if (iteration != 0)
        {
            (file_name += iteration) += ".";
        }
        file_name += "log";
        file_out.open(file_name.c_str());

        next_send_time1 = Time::now() + Time::msecs(step_size);
        next_send_time2 = Time::now() + Time::msecs(step_size);

        now1 = Time::now();
        now2 = Time::now();
        start_time = Time::now().to_msecs();
        NOTICE << "start sending tuples!";

        // Send the first batch of tuples. Queue up the next round with a delay.
        send_type_1_tuple();
        send_type_2_tuple();

        DEBUG << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    return(status);
}


