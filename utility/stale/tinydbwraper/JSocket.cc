#include "JSocket.h"
#include <cstdlib>
#include <iostream>
#include <errno.h>
#include <fcntl.h>

TinyDBWrapper::JSocket::JSocket() : _h_socket(SOCKET_INVALID)
{

}

TinyDBWrapper::JSocket::~JSocket()
{
    if (is_valid())
    {
        ::close(_h_socket);
    }
}

bool TinyDBWrapper::JSocket::is_valid()
{
    return _h_socket != SOCKET_INVALID;
}

bool TinyDBWrapper::JSocket::create()
{
    _h_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (!is_valid())
    {
        return false;
    }

    return true;
}

void TinyDBWrapper::JSocket::close()
{
    if (!is_valid())
    {
        return;
    }

    ::close(_h_socket);
    _h_socket = SOCKET_INVALID;
}

void TinyDBWrapper::JSocket::set_non_blocking(bool f_non_blocking)
{
    if (!is_valid())
    {
        return;
    }

    int opts;

    opts = fcntl(_h_socket, F_GETFL);

    if (opts < 0)
    {
        return;
    }

    if (f_non_blocking)
    {
        opts = (opts | O_NONBLOCK);
    }
    else
    {
        opts = (opts & ~O_NONBLOCK);
    }

    fcntl(_h_socket, F_SETFL, opts);
}

bool TinyDBWrapper::JSocket::listen(int i_port)
{
    if (!is_valid())
    {
        return false;
    }

    sockaddr_in s_addr;

    memset((void*)&s_addr, 0, sizeof(s_addr));

    s_addr.sin_family        = AF_INET;
    s_addr.sin_addr.s_addr   = INADDR_ANY;
    s_addr.sin_port          = htons(i_port);

    if (::bind(_h_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1)
    {
        return false;
    }

    if (::listen(_h_socket, MAXCONNECTIONS) == -1)
    {
        return false;
    }

    return true;
}

bool TinyDBWrapper::JSocket::accept(JSocket& rs_new_socket)
{
    sockaddr_in s_addr;
    int i_len = sizeof(s_addr);

    memset((void*)&s_addr, 0, sizeof(s_addr));

    rs_new_socket._h_socket = ::accept(_h_socket, (sockaddr*)&s_addr, (socklen_t*) &i_len);

    if (rs_new_socket._h_socket <= 0)
    {
        return false;
    }

    return true;
}

bool TinyDBWrapper::JSocket::connect(char* c_ipaddr, int i_port)
{
    if (!is_valid())
    {
        return false;
    }

    sockaddr_in s_addr;

    memset((void*)&s_addr, 0, sizeof(s_addr));

    s_addr.sin_family        = AF_INET;
    s_addr.sin_addr.s_addr   = inet_addr(c_ipaddr);
    s_addr.sin_port          = htons(i_port);

    if (::connect(_h_socket, (sockaddr*)&s_addr, sizeof(s_addr)) != 0)
    {
        return false;
    }

    return true;
}

INT_64 TinyDBWrapper::JSocket::read_long()
{
    INT_64 i_ret = 0;
    BYTE* pb_ret = (BYTE*)&i_ret;
    BYTE b_buffer[8];

    memset(b_buffer, 0, 8);

    int test;
    if ((test = read_byte_array(&b_buffer[0], 8)) != 8)
    {
        cerr << "Socket::ReadLong Error: " << errno << ", Read bytes: " << test << endl;
        return 0;
    }
    else
    {
        pb_ret[0] = b_buffer[7];
        pb_ret[1] = b_buffer[6];
        pb_ret[2] = b_buffer[5];
        pb_ret[3] = b_buffer[4];
        pb_ret[4] = b_buffer[3];
        pb_ret[5] = b_buffer[2];
        pb_ret[6] = b_buffer[1];
        pb_ret[7] = b_buffer[0];
    }

    return i_ret;
}

INT_32 TinyDBWrapper::JSocket::read_int()
{
    INT_32 i_ret = 0;
    BYTE* pb_ret = (BYTE*)&i_ret;
    BYTE b_buffer[4];

    memset(b_buffer, 0, 4);

    int test;
    if ((test = read_byte_array(&b_buffer[0], 4)) != 4)
    {
        cerr << "Socket::ReadInt Error: " << errno << ", Read bytes: " << test << endl;
        return 0;
    }
    else
    {
        pb_ret[0] = b_buffer[3];
        pb_ret[1] = b_buffer[2];
        pb_ret[2] = b_buffer[1];
        pb_ret[3] = b_buffer[0];
    }

    return i_ret;
}


INT_16 TinyDBWrapper::JSocket::read_short()
{
    INT_16 i_ret = 0;
    BYTE* pb_ret = (BYTE*)&i_ret;
    BYTE b_buffer[2];

    memset(b_buffer, 0, 2);

    int test;
    if ((test = read_byte_array(&b_buffer[0], 2)) != 2)
    {
        cerr << "Socket::ReadShort Error: " << errno << ", Read bytes: " << test << endl;
        return 0;
    }
    else
    {
        pb_ret[0] = b_buffer[1];
        pb_ret[1] = b_buffer[0];
    }

    return i_ret;
}

double TinyDBWrapper::JSocket::read_double()
{
    double d_ret = 0.0;
    INT_64* pl_temp = (INT_64*)&d_ret;

    *pl_temp = read_long();

    return d_ret;
}

BYTE TinyDBWrapper::JSocket::read_byte()
{
    char c_ret;

    c_ret = 0;

    if (::recv(_h_socket, &c_ret, 1, 0) != 1)
    {
        cerr << "Socket::ReadByte Error: " << errno << endl;
    }

    return c_ret;
}

int TinyDBWrapper::JSocket::read_byte_array(BYTE* pb_data, int i_count)
{
    int i_ret = 0;
    int i_to_read = i_count;
    memset(pb_data, 0, i_count);

    // if you try to read all at once, you sometimes get not all bytes ???
    while (i_to_read>0)
    {
        if ((i_ret = ::recv(_h_socket, pb_data+(i_count-i_to_read), i_to_read, 0)) == -1)
        {
            cerr << "Socket::ReadByteArray Error: " << errno << endl;
            return i_count - i_to_read;
        }
        i_to_read -= i_ret;
    }

    return i_count - i_to_read;
}

void TinyDBWrapper::JSocket::write_long(const INT_64 i_data)
{
    char c_buffer[8];

    c_buffer[0] = (char)(i_data >> 56);
    c_buffer[1] = (char)(i_data >> 48);
    c_buffer[2] = (char)(i_data >> 40);
    c_buffer[3] = (char)(i_data >> 32);
    c_buffer[4] = (char)(i_data >> 24);
    c_buffer[5] = (char)(i_data >> 16);
    c_buffer[6] = (char)(i_data >> 8);
    c_buffer[7] = (char)(i_data >> 0);

    if (::send(_h_socket, c_buffer, 8, 0) == -1)
    {
        cerr << "Socket::WriteLong Error: " << errno << endl;
    }
}

void TinyDBWrapper::JSocket::write_int(const INT_32 i_data)
{
    char c_buffer[4];

    c_buffer[0] = (char)(i_data >> 24);
    c_buffer[1] = (char)(i_data >> 16);
    c_buffer[2] = (char)(i_data >> 8);
    c_buffer[3] = (char)(i_data >> 0);

    if (::send(_h_socket, c_buffer, 4, 0) == -1)
    {
        cerr << "Socket::WriteInt Error: " << errno << endl;
    }
}

void TinyDBWrapper::JSocket::write_short(const INT_16 i_data)
{
    char c_buffer[2];

    c_buffer[0] = (char)(i_data >> 8);
    c_buffer[1] = (char)(i_data >> 0);

    if (::send(_h_socket, c_buffer, 2, 0) == -1)
    {
        cerr << "Socket::WriteShort Error: " << errno << endl;
    }
}

void TinyDBWrapper::JSocket::write_double(const double d_data)
{
    INT_64* pl_temp = (INT_64*)&d_data;

    write_long(*pl_temp);
}

void TinyDBWrapper::JSocket::write_byte(const BYTE b_data)
{
    if (::send(_h_socket, &b_data, 1, 0) == -1)
    {
        cerr << "Socket::WriteByte Error: " << errno << endl;
    }
}

void TinyDBWrapper::JSocket::write_byte_array(const BYTE* pb_data, const int i_count)
{
    if (::send(_h_socket, pb_data, i_count, 0) == -1)
    {
        cerr << "Socket::WriteByteArray Error: " << errno << endl;
    }
}

void TinyDBWrapper::JSocket::flush()
{
    ::sync();
}
