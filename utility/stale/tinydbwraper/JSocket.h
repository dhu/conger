#ifndef JSOCKET_H
#define JSOCKET_H

#include "WrapperConfig.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SOCKET_INVALID  -1
#define MAXCONNECTIONS  10

using namespace std;

namespace tiny_dbwrapper
{
class JSocket
{

public:
    JSocket();
    virtual ~JSocket();
private: // no duplicates allowed
    JSocket(const JSocket& r_jsocket);
    JSocket& operator=(const JSocket& r_jsocket);

public:
    // Control part
    bool    is_valid();
    bool    create();
    void    close();
    void    set_non_blocking(bool f_non_blocking=true);

    // Server part
    bool    listen(int i_port);
    bool    accept(JSocket& rs_new_socket);

    // Client part
    bool    connect(char* c_ipaddr, int i_port);

    // Data part
    INT_64  read_long();
    INT_32  read_int();
    INT_16  read_short();
    BYTE    read_byte();
    double  read_double();
    int     read_byte_array(BYTE* pb_data, int i_count);

    void    write_long(const INT_64 i_data);
    void    write_int(const INT_32 i_data);
    void    write_short(const INT_16 i_data);
    void    write_byte(const BYTE b_data);
    void    write_double(const double d_data);
    void    write_byte_array(const BYTE* pb_data, const int i_count);

    void    flush();

private:
    int         _h_socket;

};
}
#endif //JSOCKET_H
