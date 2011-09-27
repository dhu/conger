#include "DataHandler.h"

#include <fstream>

BOREALIS_NAMESPACE_BEGIN

string DataHandler::NO_SPACE = string("No space");
string DataHandler::NOT_YET_CONNECTED = string("Not connected yet");
string DataHandler::NO_CONNECTION = string("No connection");
string DataHandler::WRITE_FAILED = string("Write failed");


// --------------------------------------------------
DataHandler::~DataHandler()
{

    get_socket().drain();
    get_socket().shutdown(true,true);
    want_read(false);
    want_write(false);
    reset_socket(get_socket(),false);
    close();

    WARN << "Deleting the data handler";
}


// --------------------------------------------------
// Invoke callback when StreamEvents arrive
int DataHandler::incoming_data(constbuf buf)
{

    if ( buf.length() > MAX_PER_SENDER_OUTPUT_QUEUE )
    {   FATAL << "There's a bug. The input buffer grew too much!!!";
    }

    int total_read = 0;
    ISerialData input(buf,ISerial::binary);

    while ( true )
    {

        ptr<StreamEvent> event(new StreamEvent());

        if ( ! (input >> *event) )
        {
             break;
        }

        total_read = input.pos();

        // Handle the event received if known how
        if ( _args->cb )
        {
            Status stat = _args->cb(event);
            if ( ! stat )
            {   WARN << "Callback failed with error: " << stat << "\n on event was: " << event;
            }
        }
    }

    return  total_read;
}

// --------------------------------------------------
/// Send stream events to remote host
Status DataHandler::send_data(ptr<StreamEvent> event)
{
    
    _is_new = false;

    if ( !_connected )
    {
        _connected = is_connected(); // Try to refresh just in case
        WARN << "Not connected yet";
        return  NOT_YET_CONNECTED;
    }

    if ( _terminated )
    {
        WARN << "Already terminated";
        return  NO_CONNECTION;
    }

    if ( (!get_socket() ) || (! is_connected()) )
    {
        WARN << "connection or socket is gone...";
        return  NO_CONNECTION;
    }

    if ( get_output_buffer_size() > _args->max_buffer )
    {
        WARN << "No space: " << get_output_buffer_size() << " > " << _args->max_buffer;

        return  NO_SPACE;
    }

    OSerialString oss(OSerial::binary|OSerial::nosignature);
    oss << (*event);

    if ( ! write(oss.str()) )
    {
        WARN << "Write failed";
        return WRITE_FAILED;
    }

    return  true;
}


BOREALIS_NAMESPACE_END
