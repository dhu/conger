
#include "Update_Queue_Tumble2Marshal.h"

using namespace Borealis;

uint32 batch_size = 1;         // Number of input tuples per batch.
uint32 base_sleep_time = 1000; // Default.

FILE *fd;

// Send input from the data file.
//
void Update_Queue_Tumble2Marshal::sentInput1()
{
    static uint32 sleep_time = base_sleep_time;

    for (unsigned int i = 0; i < batch_size; ++i)
    {
        Input1 tuple;

        // read a tuple from the data file
        //
        int time, value;
        char sensor_id[5];
        int r = fscanf(fd, "%d %s %d\n", &time, sensor_id, &value);
        sensor_id[4] = '\0';
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

        tuple._data.time  = time; 
        Update_Queue_Tumble2Marshal::setStringField(string(sensor_id), 
                                            tuple._data.sensor_id, 4);
        tuple._data.value = value;
        DEBUG << "Sending Input1(" 
              << tuple._data.time << ", "
              << sensor_id << ", "
              << tuple._data.value << ")";
        batchInput1(&tuple);
    }

    sendInput1(sleep_time);
}

// Print the content of received tuples.
//
void Update_Queue_Tumble2Marshal::receivedOutput1(AggTuple *tuple)
{
    char sensor_id[5];
    sensor_id[0] = (tuple->sensor_id)[0];
    sensor_id[1] = (tuple->sensor_id)[1];
    sensor_id[2] = (tuple->sensor_id)[2];
    sensor_id[3] = (tuple->sensor_id)[3];
    sensor_id[4] = '\0';
    DEBUG << "Receiving Output1(" 
          << tuple->sensor_id << ", "
          << tuple->time << ", "
          << tuple->count << ")";
}

int main(int argc, const char *argv[])
{
    Update_Queue_Tumble2Marshal marshal; // Client and I/O stream state.

    if ((argc != 2) && (argc != 3))
    {
        WARN << "Usage : " << argv[0] 
             << " <data_file_name> [<rate (tuples/sec)>]";
        return 1;
    }
    else
    {
        if (!(fd = fopen(argv[1], "r")))
        {
            WARN << "Cannot open file: " << argv[1];
            return 1;
        }

        if (argc == 3)
        {
            double rate = atof(argv[2]);
            DEBUG << "rate = " << rate;
            base_sleep_time = int((1/rate)*1000);
            DEBUG << "base sleep time = " << base_sleep_time;
        }
    }

    marshal.open();

    // Send the first batch of tuples.
    // Queue up the next round with a delay.
    //
    marshal.sentInput1();

    // Run the client event loop.
    // Return only on an exception.
    //
    DEBUG << "run the client event loop...";
    marshal.runClient();
}
