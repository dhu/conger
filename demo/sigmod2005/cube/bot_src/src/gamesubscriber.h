/* Code to subscribe to a borealis stream using RPC.
 * note that we will process and send tuples ourselves, the only
 * need for RPC is to notify borealis that it should start sending 
 * a stream to us.
 */

#ifndef GAME_SUBSCRIBER_H
#define GAME_SUBSCRIBER_H

#include <string>
#include <NMSTL/tcprpc>
#include <NMSTL/serial>

#include "DataHandler.h"

#include "rpc_QueryProcessor.h"

extern int getclientnum();

//replaces dots and : in inet address with nothing
void formataddr(const std::string& s, std::string& out);


class BufferedSocket : public NMSTL::TCPSocket
{
    std::string partial_send_msg;
    std::string partial_recv_msg;

    int recv_buf_size;

    NMSTL::Callback<void, void> close_cb;

public:
    BufferedSocket() : NMSTL::TCPSocket(), recv_buf_size(1024) {}
    BufferedSocket(NMSTL::InetAddress addr) : NMSTL::TCPSocket(addr), recv_buf_size(1024) {}
    BufferedSocket(NMSTL::Socket& s) : NMSTL::TCPSocket(s), recv_buf_size(1024) {}

    using NMSTL::TCPSocket::getpeername;
    using NMSTL::TCPSocket::getsockname;

    // returns the # bytes sent from the given buffer.
    int send_msg(const void* buf, size_t len);

    // returns the number of bytes copied into the given buffer.
    int recv_msg(void* buf, size_t len);

    // returns the # bytes recv over the network from this call.
    int recv_fixed(void* buf, size_t len);

    int send_remainder() { return partial_send_msg.length(); }
    int recv_remainder() { return partial_recv_msg.length(); }

    void set_partial_recv_buffer(const void* buf, size_t len)
    {
        std::string new_partial_recv_msg((const char*) buf, len);
        partial_recv_msg = new_partial_recv_msg + partial_recv_msg;
    }

    // Callback support.
    void set_close_callback(NMSTL::Callback<void, void> on_close) { close_cb = on_close; }
};

class GameSubscriber : private Borealis::Remote<Borealis::QueryProcessor> 
{
    NMSTL::InetAddress m_remote; // Address of remote borealis server
    NMSTL::InetAddress m_client; // Address of socket etc.. where we  are listening for tuples
    NMSTL::IOEventLoop m_loop;
    NMSTL::TCPRPCEndpoint m_endpoint;
    NMSTL::TCPSocket m_listener;

    NMSTL::InetAddress m_deploy;
    NMSTL::TCPRPCEndpoint m_deploy_endpoint;
    Borealis::Remote<Borealis::QueryProcessor> m_deploy_qp;

    NMSTL::ptr<Borealis::Subscription> m_fast_subscription;
    NMSTL::ptr< std::vector< NMSTL::ptr<BufferedSocket> > > m_recv_sockets;

    std::vector<std::string> m_queries;
    std::vector< NMSTL::ptr<BufferedSocket> > m_query_sockets;

    NMSTL::Mutex m_pending_query_mutex;
    std::vector< NMSTL::ptr<BufferedSocket> > m_pending_query_sockets;

    // TODO: should really change all streams/sockets in the game to use this map.
    std::map< std::string, NMSTL::ptr<BufferedSocket> > m_stream_sockets;

    typedef std::map<std::string, std::pair<void (*)(std::string, std::string), size_t> > query_handler_map;
    query_handler_map m_query_handlers;

    std::map<std::string, std::string> m_output_streams;
    std::map<std::string, std::vector<std::string> > m_query_boxes;

    class DatapathListener : public NMSTL::Thread
    {
        NMSTL::ptr<GameSubscriber> _game_subs;
        bool terminate;
    public:
        DatapathListener(NMSTL::ptr<GameSubscriber> game_subs) :
            _game_subs(game_subs), terminate(false)
        {}

        void stop() { terminate = true; }

    protected:
        void run() {
            delete_on_completion();
            while(!terminate) {
                _game_subs->update_subscriptions();
                NMSTL::Thread::sleep(NMSTL::Time::msecs(2000));
            }
        }

        friend class GameSubscriber;
    };

    NMSTL::ptr<DatapathListener> _dp_listener;

public:

    GameSubscriber( NMSTL::InetAddress dest_addr, NMSTL::InetAddress local_addr,
                    NMSTL::InetAddress deploy_addr, 
                    NMSTL::ptr< std::vector< NMSTL::ptr<BufferedSocket> > > recv_sockets ) :
        m_remote(dest_addr),
        m_client(local_addr),
        m_endpoint(m_loop, NMSTL::TCPSocket(dest_addr)),
        m_listener(NMSTL::TCPSocket(NMSTL::InetAddress(local_addr.get_port()), NMSTL::Socket::acceptor)),
        m_deploy(deploy_addr),
        m_deploy_endpoint(m_loop, NMSTL::TCPSocket(deploy_addr)),
        m_recv_sockets(recv_sockets)
    {
        *static_cast<Borealis::Remote<Borealis::QueryProcessor>*>(this) = m_endpoint.get<Borealis::QueryProcessor>("QueryProcessor");

        m_deploy_qp = m_deploy_endpoint.template get<Borealis::QueryProcessor>("QueryProcessor");
    }

    void init_subscribe();
    void game_subscribe();

    void update_subscriptions();

    std::string subscribed_addr() { return m_client.as_string(); }
    std::string deployment_addr() { return m_deploy.as_string(); }

    std::vector<std::string>& list_queries() { return m_queries; };
    bool find_query(std::string query_name);

    bool deploy_query( std::string query_name,
                       std::vector<std::pair<std::string, std::string> > box_xmls,
                       std::string output_stream, void (*handler)(std::string, std::string),
                       size_t tuple_size );

    void handle_query_output();

private:

    bool add_box(Borealis::Remote<Borealis::QueryProcessor>& qp,
                 std::string query_xml);

    bool subscribe_output(Borealis::Remote<Borealis::QueryProcessor>& qp,
                          std::string stream_name,
                          bool block_till_accept,
                          std::vector< NMSTL::ptr<BufferedSocket> >& subs_outputs);

    void add_query_handler( std::string query_name, std::string stream_name,
                            void (*handler)(std::string, std::string), size_t tuple_size );

    NMSTL::ptr<BufferedSocket> get_stream_socket(std::string stream_name);
    void add_stream_socket(std::string stream_name, NMSTL::ptr<BufferedSocket> sock);
    void remove_stream_socket(std::string stream_name);

    bool handle_socket_output(NMSTL::ptr<BufferedSocket> sock, bool update_stream_map);
};

#endif
