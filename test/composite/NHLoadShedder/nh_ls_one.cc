
#include "Nh_Ls_OneMarshal.h"

using namespace Borealis;

uint32 batch_size = 5;        // Number of input tuples per batch.
uint32 base_sleep_time = 100; // Initial warm-up data sending period.

uint32 base_sleep_time1, base_sleep_time2;

FILE *fd1, *fd2;

int run_time = 0; 
Time start_time;

int overload_time = 155000;

// Send input from the data file.
//
void Nh_Ls_OneMarshal::sentInput1()
{
    uint32 sleep_time;

    Time current_time = Time::now();
    double time_passed =  static_cast<double>(
                            (current_time - start_time).to_usecs());
    double time_passed_msec = time_passed/1000.0;

    if (time_passed_msec < overload_time)
    {
        sleep_time = base_sleep_time*batch_size;
    }
    else
    {
        sleep_time = base_sleep_time1*batch_size;
    }

    for (unsigned int i = 0; i < batch_size; ++i)
    {
        Input1 tuple;

        // read a tuple from the data file
        //
        int time, sensor_id, temperature;
        int r = fscanf(fd1, "%d %d %d\n", &time, &sensor_id, &temperature);
        if (r == EOF)
        {
            WARN << "end of file";
            return;
        }
        else if (r != 3)
        {
            WARN << "problem reading file";
            return;
        }

        tuple._data.time = time; 
        tuple._data.sensor_id = sensor_id; 
        tuple._data.temperature = temperature;
        DEBUG << "Sending Input1(" 
              << tuple._data.time << ", "
              << tuple._data.sensor_id << ", "
              << tuple._data.temperature << ")";
        batchInput1(&tuple);
    }

    if (time_passed_msec >= run_time)
    {
        NOTICE << "Timeout";
        return;
    }

    sendInput1(sleep_time);
}

void Nh_Ls_OneMarshal::sentInput2()
{
    uint32 sleep_time;

    Time current_time = Time::now();
    double time_passed =  static_cast<double>(
                            (current_time - start_time).to_usecs());
    double time_passed_msec = time_passed/1000.0;

    if (time_passed_msec < overload_time)
    {
        sleep_time = base_sleep_time*batch_size;
    }
    else
    {
        sleep_time = base_sleep_time2*batch_size;
    }

    for (unsigned int i = 0; i < batch_size; ++i)
    {
        Input2 tuple;

        // read a tuple from the data file
        //
        int time, sensor_id, temperature;
        int r = fscanf(fd2, "%d %d %d\n", &time, &sensor_id, &temperature);
        if (r == EOF)
        {
            WARN << "end of file";
            return;
        }
        else if (r != 3)
        {
            WARN << "problem reading file";
            return;
        }

        tuple._data.time = time; 
        tuple._data.sensor_id = sensor_id; 
        tuple._data.temperature = temperature;
        DEBUG << "Sending Input2(" 
              << tuple._data.time << ", "
              << tuple._data.sensor_id << ", "
              << tuple._data.temperature << ")";
        batchInput2(&tuple);
    }

    if (time_passed_msec >= run_time)
    {
        NOTICE << "Timeout";
        return;
    }

    sendInput2(sleep_time);
}

// Print the content of received tuples.
//
void Nh_Ls_OneMarshal::receivedOutput1(OutputSchema *tuple)
{
    DEBUG << "Receiving Output1(" 
          << tuple->time << ", "
          << tuple->sensor_id << ", "
          << tuple->temperature << ")";
}

// Print the content of received tuples.
//
void Nh_Ls_OneMarshal::receivedOutput2(OutputSchema *tuple)
{
    DEBUG << "Receiving Output2(" 
          << tuple->time << ", "
          << tuple->sensor_id << ", "
          << tuple->temperature << ")";
}

int main(int argc, const char *argv[])
{
    Nh_Ls_OneMarshal marshal;

    if (argc != 5)
    {
        WARN << "Usage : " << argv[0] 
             << " <data_file_name> <rate1 (tuples/sec)> <rate2 (tuples/sec)>"
             << " <run_time(msec)>";
        return 1;
    }
    else
    {
        if (!(fd1 = fopen(argv[1], "r")))
        {
            WARN << "Cannot open file: " << argv[1];
            return 1;
        }

        if (!(fd2 = fopen(argv[1], "r")))
        {
            WARN << "Cannot open file: " << argv[1];
            return 1;
        }

        double rate1 = atof(argv[2]);
        NOTICE << "rate1 = " << rate1;
        base_sleep_time1 = int((1/rate1)*1000);
        NOTICE << "base sleep time1 = " << base_sleep_time1;
        double rate2 = atof(argv[3]);
        NOTICE << "rate2 = " << rate2;
        base_sleep_time2 = int((1/rate2)*1000);
        NOTICE << "base sleep time2 = " << base_sleep_time2;
        run_time = atoi(argv[4]);
    }

    start_time = Time::now(); 

    marshal.open();

    // Send the first batch of tuples.
    // Queue up the next round with a delay.
    //
    marshal.sentInput1();
    marshal.sentInput2();

    // Run the client event loop.
    // Return only on an exception.
    //
    NOTICE << "run the client event loop...";
    marshal.runClient();
}
