#include "args.h"
#include "Table_TestMarshal.h"

/*
  Description:
  this application creates a table, and a insert, update and read box on the table.
  Our table has tuples of 5 attributes.
  Our client inserts a new tuple every 2 seconds, updates a tuple every second, and reads tuples 5 times a second.

  Note: this test will create a tables/ subdirectory from where you run it.
*/

using namespace Borealis;
using namespace NMSTL;

const int INSERT_BATCH_SIZE = 10;
const int UPDATE_BATCH_SIZE = 10;
const int READ_BATCH_SIZE = 1;

const int INSERT_SLEEP_TIME = 2000;
const int UPDATE_SLEEP_TIME = 1000;
const int READ_SLEEP_TIME = 200;

int counter = 0;

void Table_TestMarshal::receivedSelect( TableTuple *tuple )
{
    NOTICE << "Tuple ("
           << tuple->f1 << ", "
           << tuple->f2 << ", "
           << tuple->f3 << ", "
           << tuple->f4 << ", "
           << tuple->f5 << ")";
}

void Table_TestMarshal::sentInsert()
{
    // Enqueue tuples to the table insert stream.
    for(int i = 0; i < INSERT_BATCH_SIZE; i++)
    {
        Insert insert_tuple;
        insert_tuple._data.f1 = ++counter;
        insert_tuple._data.f2 = rand();
        insert_tuple._data.f3 = rand();
        insert_tuple._data.f4 = rand();
        insert_tuple._data.f5 = rand();

        batchInsert( &insert_tuple );
    }

    sendInsert( INSERT_SLEEP_TIME );
}

void Table_TestMarshal::sentUpdate()
{
    if (counter > 5)
    {
        // Enqueue tuples to the table insert stream.
        for(int i = 0; i < UPDATE_BATCH_SIZE; i++)
        {
            Update update_tuple;
            update_tuple._data.f1 = rand() % counter;
            update_tuple._data.f2 = rand();
            update_tuple._data.f3 = rand();
            update_tuple._data.f4 = rand();
            update_tuple._data.f5 = rand();

            batchUpdate( &update_tuple );
        }
    }

    sendUpdate( UPDATE_SLEEP_TIME );
}

void Table_TestMarshal::sentRead1()
{
    if (counter > 5)
    {
        // Enqueue tuples to the table insert stream.
        for(int i = 0; i < READ_BATCH_SIZE; i++)
        {
            Read1 read_tuple;
            read_tuple._data.k1 = rand() % counter;
            read_tuple._data.k2 = 0;
            read_tuple._data.k3 = 0;
            
            batchRead1( &read_tuple );
        }
    }

    sendRead1( READ_SLEEP_TIME );
}

int main(int argc, const char* argv[])
{
    Table_TestMarshal _app_handle;

    string ip = "127.0.0.1";
    int port = 15000;
    stdArgs( argc, argv, ip, port );

    int32 status = _app_handle.open();
    if (status)
    {
        WARN << "Failed to deploy query network";
    }
    else
    {
        _app_handle.sentInsert();
        _app_handle.sentUpdate();
        _app_handle.sentRead1();
        _app_handle.runClient();
    }

    return status;
}
