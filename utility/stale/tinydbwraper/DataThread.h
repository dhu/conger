#ifndef DATATHREAD_H
#define DATATHREAD_H

#include "WrapperConfig.h"
#include <pthread.h>
#include "JSocket.h"


using namespace std;

namespace tiny_dbwrapper
{
class TQuery;
class DataThread
{

public:
    DataThread();
    ~DataThread();

    bool            create(int i_port, TQuery* p_tquery);
    bool            cancel();


protected:
    static void*    run(void* p_param);

private:
    pthread_t       _thread;
    JSocket         _listen_socket;
    JSocket         _data_socket;
    TQuery*         _p_tquery;

};
}
#endif //DATATHREAD_H
