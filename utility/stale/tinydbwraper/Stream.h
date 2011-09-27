#ifndef STREAM_H
#define STREAM_H

#include "WrapperConfig.h"
#include <string>

namespace tiny_dbwrapper
{
class Stream
{

public:
    Stream();
    Stream(int i_port, string str_name);
    Stream(const Stream& r_stream);

    Stream& operator=(const Stream& r_stream);

    int     get_port();
    string  get_name();

protected:
    void    set_port(int i_port);
    void    set_name(string str_name);

private:
    int     _i_port;
    string  _str_name;

};

}
#endif //STREAM_H
