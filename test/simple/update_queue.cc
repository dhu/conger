
#include "Update_QueueMarshal.h"

using namespace Borealis;

uint32 batch_size = 1;         // Number of input tuples per batch.
uint32 base_sleep_time = 1000; // Default.

FILE *fd;

// Send input from the data file.
//
void Update_QueueMarshal::sentInput1()
{
    static uint32 sleep_time = base_sleep_time;

    for (unsigned int i = 0; i < batch_size; ++i)
    {
        Input1 tuple;

        // read a tuple from the data file
        //
        int time, price;
        char quote[5];
        int r = fscanf(fd, "%d %s %d\n", &time, quote, &price);
        quote[4] = '\0';
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
        Update_QueueMarshal::setStringField(string(quote), 
                                            tuple._data.quote, 4);
        //strcpy(tuple._data.quote, quote);
        tuple._data.price = price;
        DEBUG << "Sending Input1(" 
              << tuple._data.time << ", "
              << quote << ", "
              << tuple._data.price << ")";
        batchInput1(&tuple);
    }

    sendInput1(sleep_time);
}

// Print the content of received tuples.
//
void Update_QueueMarshal::receivedOutput1(AggTuple *tuple)
{
    char quote[5];
    quote[0] = (tuple->quote)[0];
    quote[1] = (tuple->quote)[1];
    quote[2] = (tuple->quote)[2];
    quote[3] = (tuple->quote)[3];
    quote[4] = '\0';
    DEBUG << "Receiving Output1(" 
          << quote << ", "
          << tuple->time << ", "
          << tuple->count << ")";

}

int main(int argc, const char *argv[])
{
    Update_QueueMarshal marshal; // Client and I/O stream state.

    if ((argc != 2) && (argc != 3))
    {
        WARN << "Usage : " << argv[0] 
             << " <data_file_name> [<base_sleep_time>]";
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
            base_sleep_time = atoi(argv[2]);
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
