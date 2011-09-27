#ifndef TuplesSave_H
#define TuplesSave_H

#include <common.h>
#include <fstream>


//Local hearders
//#include "StreamTuple.h"
//#include "StreamSender.h"
#include "StreamBuffer.h"
//#include "TupleHeader.h"

BOREALIS_NAMESPACE_BEGIN
/**This class is for save and resend tuples to Borealis
 *
 */
class TuplesSave
{


public:
    
    /**Constructor
     *
     */
    TuplesSave();
    ~TuplesSave();

    void saveTuples(SendItem *send_item_t);
    bool saveToFile(const char *filename, SendItem *send_item_t);
    
    bool reSendTuples(StreamBuffer *stream_buffer, char *filename);

    
    
private:

    
};


BOREALIS_NAMESPACE_END
#endif
