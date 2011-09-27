#include "Stream.h"

TinyDBWrapper::Stream::Stream()
{
    _i_port     = -1;
    _str_name   = "";
}

TinyDBWrapper::Stream::Stream(int i_port, string str_name)
{
    _i_port     = i_port;
    _str_name   = str_name;
}

TinyDBWrapper::Stream::Stream(const Stream& r_stream)
{
    _i_port     = r_stream._i_port;
    _str_name   = r_stream._str_name;
}

TinyDBWrapper::Stream& TinyDBWrapper::Stream::operator=(const Stream& r_stream)
{
    _i_port     = r_stream._i_port;
    _str_name   = r_stream._str_name;
    return *this;
}

int TinyDBWrapper::Stream::get_port()
{
    return _i_port;
}

string TinyDBWrapper::Stream::get_name()
{
    return _str_name;
}

void TinyDBWrapper::Stream::set_port(int i_port)
{
    _i_port = i_port;
}

void TinyDBWrapper::Stream::set_name(string str_name)
{
    _str_name = str_name;
}
