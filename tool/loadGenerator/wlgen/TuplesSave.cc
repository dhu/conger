#include "TuplesSave.h"

BOREALIS_NAMESPACE_BEGIN
//using namespace time_tools;

TuplesSave::TuplesSave()
{
}


void TuplesSave::saveTuples(SendItem *send_item_t)
{
    if(!saveToFile("/tmp/GenSaveTuples", send_item_t))
    {
        ERROR << "Saving tuples failed.";
    }
    else
    {
        //NOTICE << "Saving tuples success.";
    }
        
   
}

bool TuplesSave::saveToFile(const char* filename, SendItem *send_item_t)
{
    fstream file;
    file.open(filename, ios::out | ios::app | ios::binary);
    if(!file)
    {
        ERROR << "Can not open file: " << filename;
        return false;
    }

    
    if(file.is_open())
    {
        tuplelist_t *my_tuple_list=send_item_t->tuple_list;
    
        for(tuplelist_t::iterator it = my_tuple_list->begin();
                it!=my_tuple_list->end();
                ++it)
        {
            StreamTuple *stream_tuple = (StreamTuple*)(*it);
  
            file.write((char*)stream_tuple, sizeof(StreamTuple));
          
        }
    }
    file.close();
    return true;
}

bool TuplesSave::reSendTuples(StreamBuffer *stream_buffer, char *filename)
{
    fstream file;
    file.open(filename, ios::in | ios::out | ios::binary);
    if(!file)
    {
        ERROR << "Can not open file: " << filename;
        return false;
    }
    
    timeval now;
    gettimeofday(&now,0);
    Timeval now_timestamp = Timeval(now);


    if(file.is_open())
    {
   
        NOTICE << "Begin to send the exist tuples from the file" << filename;
        file.seekg(ios::beg);
        
        //get now timestamp
        timeval now;
        gettimeofday(&now,0);
        Timeval now_timestamp = Timeval(now);
        Timeval now1_timestamp = Timeval(now_timestamp.double_value()+1) ;
        Timeval first_timestamp;
        
        //get timestamp of the first stream tuple
        StreamTuple *first_stream_tuple = new StreamTuple();
        
        file.read((char*)first_stream_tuple, sizeof(StreamTuple));
        
        if(file.bad())
        {
            ERROR << "Reading first exsit tuple failed.";
            return false;

        }
        else
        {
            first_timestamp = first_stream_tuple->getTimestamp();
        
            first_stream_tuple->changeTimestamp(now1_timestamp);
        
            stream_buffer->addExistTuple(first_stream_tuple);
        
        }
       
        while(!file.eof())
        {
        
            StreamTuple *stream_tuple = new StreamTuple();
            file.read((char*)stream_tuple, sizeof(StreamTuple));
            
            int len = stream_tuple->getDataSize();
            
            Timeval old_timestamp = stream_tuple->getTimestamp();
            
            Timeval current_timestamp = now1_timestamp;    
            current_timestamp += old_timestamp - first_timestamp;
            stream_tuple->changeTimestamp(current_timestamp);
        
            if(!(len==0 || len>50))
            {
                stream_buffer->addExistTuple(stream_tuple);
            }
        }

    }
    file.close();
    return true;
}


BOREALIS_NAMESPACE_END
