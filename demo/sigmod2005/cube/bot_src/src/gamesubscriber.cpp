#include "gamesubscriber.h"
#include "tuples.h"

using namespace std;
using namespace Borealis;

void formataddr(const std::string& s, std::string& out)
{
    out = "";
    for(int i  = 0; i < s.length(); ++i) {
	if (s[i] != '.' && s[i] != ':') {
	    out.push_back(s[i]);
	}
    }
}


int BufferedSocket::send_msg(const void* buf, size_t len)
{
    const int prev_msg_length = partial_send_msg.length();
    std::string new_msg = std::string((const char*) buf, len);
    partial_send_msg += new_msg;

    int bytes_sent = write(partial_send_msg.data(), partial_send_msg.length());
    if (bytes_sent <= partial_send_msg.length()) {
        partial_send_msg = std::string(partial_send_msg.data() + bytes_sent, partial_send_msg.length() - bytes_sent);
    } else {
        partial_send_msg = "";
    }

    return (bytes_sent < prev_msg_length? 0 : bytes_sent - prev_msg_length);
}

int BufferedSocket::recv_msg(void* buf, size_t len)
{

    char new_data[recv_buf_size];
    int bytes_recv = read(new_data, recv_buf_size);

    if (bytes_recv > 0)
        partial_recv_msg += std::string(new_data, bytes_recv);
    else if (errno != EAGAIN && partial_recv_msg.length() <= 0) {
        if (close_cb) close_cb();
        return -1;
    }
        

    int bytes_copied = 0;
    if (partial_recv_msg.length() >= len) {
        bytes_copied = len;
        memcpy(buf, partial_recv_msg.data(), len);
        if (partial_recv_msg.length() == len)
            partial_recv_msg = "";
        else
            partial_recv_msg = std::string(partial_recv_msg.data() + len, partial_recv_msg.length() - len);
    } else {
        bytes_copied = partial_recv_msg.length();
        memcpy(buf, partial_recv_msg.data(), partial_recv_msg.length());
        partial_recv_msg = "";
    }

    return bytes_copied;
}

int BufferedSocket::recv_fixed(void* buf, size_t len)
{
    char new_msg[recv_buf_size];
    int bytes_recv = read(new_msg, recv_buf_size);

    if (bytes_recv > 0)
        partial_recv_msg += std::string(new_msg, bytes_recv);
    else if (errno != EAGAIN && partial_recv_msg.length() <= 0) {
        if (close_cb) close_cb();
        return -1;
    }

    if (partial_recv_msg.length() >= len) {
        memcpy(buf, partial_recv_msg.data(), len);
        if (partial_recv_msg.length() == len)
            partial_recv_msg = "";
        else
            partial_recv_msg = std::string(partial_recv_msg.data() + len, partial_recv_msg.length() - len);
    }

    return bytes_recv;
}

bool GameSubscriber::add_box(Remote<QueryProcessor>& qp, string query_xml)
{
    RPC<void> r;

    if (query_xml.length() > 0) {
        DEBUG << "Adding " << query_xml;
        r = qp.add_xml_string(query_xml);
        DEBUG << "Query add_xml_string: " << r;
    }

    if ( !r.stat() )
        return false;
        
    return true;
}

bool GameSubscriber::subscribe_output(Remote<QueryProcessor> &qp,
                                      string stream_name,
                                      bool block_till_accept,
                                      vector< ptr<BufferedSocket> >& subs_sockets)
{
    RPC<void> r;

    DEBUG << "Subscribing with endpoint " << m_client;

    string subscribe_xml = string() + 
        "<subscribe stream=\"" + stream_name
        + "\" endpoint=\"" + to_string(m_client) + "\"/>";

    r = qp.add_xml_string(subscribe_xml);
    DEBUG << "Subscribe add_xml_string: " << r.stat();

    if ( !r.stat() )
        return false;

    if ( block_till_accept ) {
        InetAddress accept_addr(m_client.get_port());
        m_listener.set_blocking(true);
        Socket r_socket = m_listener.accept(accept_addr);
        m_listener.set_blocking(false);

        ptr<BufferedSocket> recv_socket = ptr<BufferedSocket>(new BufferedSocket(r_socket));
        recv_socket->set_blocking(false);
        subs_sockets.push_back(recv_socket);

        // At least add all client subscribed streams to the stream/socket map.
        add_stream_socket(stream_name, recv_socket);
    }
    
    return true;
}


void GameSubscriber::init_subscribe()
{
    string f_clientname;
    InetAddress m_local_addr(InetAddress::local_hostname());
    string m_local_ip = m_local_addr.get_host();
    formataddr(m_client.as_string(), f_clientname);

    string m_deploy_ip = m_remote.as_string();

    // Filter player_init for self init.
    Schema schema(Name("cube_init"));
    string output_name = string("self_init-") + f_clientname;
    string filter_expr = string("source_ip == '" + m_local_ip + "'");

    string query_xml = string() +
        "  <box name=\"" + "filter_init_" + f_clientname + "\" type=\"filter\" node=\"" + m_deploy_ip + "\">\n" +
        "  <in  stream=\"valid_player\" />\n" +
        "  <out stream=\"" + output_name  + "\" />\n" +
        "  <parameter name=\"expression.0\" value=\"" + filter_expr + "\"/>\n" +
        "  </box>\n";

    printf("Performing initial subscribe...\n");

    add_box(*this, query_xml);
    subscribe_output(*this, output_name, true, *m_recv_sockets);
}



void GameSubscriber::game_subscribe()
{
    string f_clientname;
    InetAddress m_local_addr(InetAddress::local_hostname());
    string m_local_ip = m_local_addr.get_host();
    formataddr(m_client.as_string(), f_clientname);

    string m_deploy_ip = m_remote.as_string();

    string local_clientnum;
    local_clientnum << getclientnum();

    printf("Performing game subscribe...\n");

    // Filter player_positions to everyone else.
    Schema pos_schema(Name("cube_position"));
    string pos_output_name = string("useful_pos-") + f_clientname;
    string pos_filter_expr = string("not(clientnum = " + local_clientnum + ")" );

    string pos_query_xml = string() +
        "<box name=\"" + "filter_pos_" + f_clientname + "\" type=\"filter\" node=\"" + m_deploy_ip + "\">\n" +
        "  <in  stream=\"player_positions\" />\n" +
        "  <out stream=\"" + pos_output_name  + "\" />\n" +
        "  <parameter name=\"expression.0\" value=\"" + pos_filter_expr + "\"/>\n" +
        "</box>\n";

    add_box(*this, pos_query_xml);
    subscribe_output(*this, pos_output_name, true, *m_recv_sockets);

    // Filter bot inits to local client.
    Schema bot_init_schema(Name("cube_valid_bot"));
    string bot_init_output_name = string("my_bot_init-") + f_clientname;
    string bot_init_filter_expr = string("host_clientnum = " + local_clientnum);
  
    string bot_init_query_xml = string() +
        "<box name=\"" + "filtered_bot_inits_" + f_clientname + "\" type=\"filter\" node=\"" + m_deploy_ip + "\">\n" +
        "  <in  stream=\"valid_bots\" />\n" +
        "  <out stream=\"" + bot_init_output_name  + "\" />\n" +
        "  <parameter name=\"expression.0\" value=\"" + bot_init_filter_expr + "\"/>\n" +
        "</box>\n";
  
    add_box(*this, bot_init_query_xml);
    subscribe_output(*this, bot_init_output_name, true, *m_recv_sockets);

    // Filter bot adds to everyone else.
    Schema new_bot_schema(Name("cube_add_bot"));
    string new_bot_output_name = string("useful_bot-") + f_clientname;
    string new_bot_filter_expr = string("not(host_clientnum = " + local_clientnum + ")" );
  
    string new_bot_query_xml = string() +
        "<box name=\"" + "filtered_bot_adds_" + f_clientname + "\" type=\"filter\" node=\"" + m_deploy_ip + "\">\n" +
        "  <in  stream=\"new_bots\" />\n" +
        "  <out stream=\"" + new_bot_output_name  + "\" />\n" +
        "  <parameter name=\"expression.0\" value=\"" + new_bot_filter_expr + "\"/>\n" +
        "</box>\n";

    add_box(*this, new_bot_query_xml);
    subscribe_output(*this, new_bot_output_name, true, *m_recv_sockets);

    // Filter ack_pickups to self clientnum
    Schema ack_schema(Name("cube_item_acc"));
    string ack_output_name = string("item_ack-") + f_clientname;
    string ack_filter_expr = string("clientnum = " + local_clientnum);

    string ack_query_xml = string() +
        "<box name=\"" + "filter_ack_" + f_clientname + "\" type=\"filter\" node=\"" + m_deploy_ip + "\">\n" +
        "  <in  stream=\"ack_pickup\" />\n" +
        "  <out stream=\"" + ack_output_name  + "\" />\n" +
        "  <parameter name=\"expression.0\" value=\"" + ack_filter_expr + "\"/>\n" +
        "</box>\n";

    add_box(*this, ack_query_xml);
    subscribe_output(*this, ack_output_name, true, *m_recv_sockets);

    // Filter pongs to self clientnum
    Schema pong_schema("cube_ping");
    string pong_output_name = string("pong-") + f_clientname;
    string pong_filter_expr = string("clientnum = " + local_clientnum);

    string pong_query_xml = string() + 
        "<box name=\"" + "filter_pong_" + f_clientname + "\" type=\"filter\" node=\"" + m_deploy_ip + "\">\n" +
        "  <in  stream=\"player_pong\"/>\n" +
        "  <out stream=\"" + pong_output_name  + "\"/>\n" +
        "  <parameter name=\"expression.0\" value=\"" + pong_filter_expr + "\"/>\n" +
        "</box>\n";

    add_box(*this, pong_query_xml);
    subscribe_output(*this, pong_output_name, true, *m_recv_sockets);

    // Filter clientpings to self clientnum
    Schema cp_schema(Name("cube_ping"));
    string cp_output_name = string("cp-") + f_clientname;
    string cp_filter_expr = string("clientnum = " + local_clientnum);

    string cp_query_xml = string() +
        "<box name=\"" + "filter_cp_" + f_clientname + "\" type=\"filter\" node=\"" + m_deploy_ip + "\">\n" +
        "  <in  stream=\"player_clientping\" />\n" +
        "  <out stream=\"" + cp_output_name  + "\" />\n" +
        "  <parameter name=\"expression.0\" value=\"" + cp_filter_expr + "\"/>\n" +
        "  </box>\n";

    add_box(*this, cp_query_xml);
    subscribe_output(*this, cp_output_name, true, *m_recv_sockets);


    // Subscribe to remaining streams.
//#define subscribe_stream(stream_name)                           \
//    StreamDef stream_name##_stream_def(#stream_name);           \
//    subscribe_output(*this, stream_name##_stream_def, Name(), string(), true, *m_recv_sockets);

#define subscribe_stream(stream_name)                           \
    subscribe_output(*this, #stream_name, true, *m_recv_sockets);
    
    subscribe_stream(valid_player);
    subscribe_stream(client_disconnect);

    subscribe_stream(player_items);
    subscribe_stream(successful_pickup);
    subscribe_stream(item_spawns);

    // TODO: ignore mode changes/reloads/timeups for now.
    subscribe_stream(new_map);
    subscribe_stream(reload_map);
    subscribe_stream(mode_change);
    subscribe_stream(map_timeup);

    subscribe_stream(player_shots);
    subscribe_stream(player_damage);
    subscribe_stream(player_frags);
    subscribe_stream(player_deaths);

    subscribe_stream(sound);
    subscribe_stream(chat);
    subscribe_stream(server_msgs);

    subscribe_stream(world_edit);
    subscribe_stream(entity_edit);

#undef subscribe_stream


    _dp_listener = ptr<DatapathListener>(new DatapathListener(ptr<GameSubscriber>::unowned(this)));
    _dp_listener->start();
}


bool GameSubscriber::find_query(std::string query_name)
{
    std::vector<std::string>::iterator found =
        find(m_queries.begin(), m_queries.end(), query_name);
    return (found != m_queries.end());
}

void GameSubscriber::add_query_handler( std::string query_name, std::string stream_name,
					void (*handler)(std::string, std::string), size_t tuple_size )
{
    DEBUG << "Adding query handler for " << query_name << " from stream " << stream_name;
    m_query_handlers[stream_name] = make_pair(handler, tuple_size);
    m_output_streams[stream_name] = query_name;
    m_queries.push_back(query_name);
    DEBUG << "# Handlers: " << m_query_handlers.size();
}

ptr<BufferedSocket> GameSubscriber::get_stream_socket(std::string stream_name)
{
    if (m_stream_sockets.find(stream_name) == m_stream_sockets.end())
        return ptr<BufferedSocket>();
    return m_stream_sockets[stream_name];
}

void GameSubscriber::add_stream_socket(std::string stream_name, ptr<BufferedSocket> sock)
{
    m_stream_sockets[stream_name] = sock;
}

void GameSubscriber::remove_stream_socket(std::string stream_name)
{
    m_stream_sockets[stream_name] = ptr<BufferedSocket>();
}

bool GameSubscriber::deploy_query(
                std::string query_name,
                std::vector<std::pair<std::string, std::string> > box_xmls,
                std::string output_stream,
                void (*handler)(std::string, std::string), size_t tuple_size )
{
    if (find_query(query_name)) {
        WARN << "Query " << query_name << " already exists...";
        return false;
    }

    if ( !m_deploy_qp ) {
        WARN << "No Borealis node selected for deploying queries.";
        return false;
    }

    std::vector<std::pair<std::string, std::string> >::iterator xml_it = box_xmls.begin();
    std::vector<std::pair<std::string, std::string> >::iterator xml_end = box_xmls.end();

    bool added = false;
    for (; xml_it != xml_end; ++xml_it) {
        // Check if box already exists in the query.
        std::map<std::string, std::vector<std::string> >::iterator box_found = m_query_boxes.find(xml_it->first);
        if ( box_found != m_query_boxes.end() ) {
            using namespace NMSTL;
            DEBUG << "Box " << xml_it->first << " is already part of " << query_name;
            return false;
        }

        // Set up the box.
        added = add_box(m_deploy_qp, xml_it->second);
        if ( !added ) {
            using namespace NMSTL;
            DEBUG << "Failed to add " << query_name;
            return added;
        }
    }

    // Subscribe to query.
    added = subscribe_output(m_deploy_qp, output_stream, false, m_query_sockets);

    // Set up a handler.
    if ( added )
        add_query_handler(query_name, output_stream, handler, tuple_size);
        
    return added;
}
                                          

bool GameSubscriber::handle_socket_output( ptr<BufferedSocket> sock,
                                           bool update_stream_map)
{
    if ( sock ) {

        int recv_len = 0;
        int recv_buffer_sz = 1024;
        char buffer[recv_buffer_sz];
        string acc;

        while ( (recv_len = (sock->recv_msg(buffer, recv_buffer_sz))) > 0 ) {
            acc += string(buffer, recv_len);
        }

        if (acc.length() == 0)
            return false;

        ISerialData iss(constbuf(acc.data(), acc.length()), ISerial::binary);
        ptr<StreamEvent> evt(new StreamEvent());
        string stream_name;

        unsigned int last_event_pos = 0;
        unsigned int buf_length = acc.length();

        while( (last_event_pos < buf_length) && iss.stat() && iss >> *evt ) {

            last_event_pos = iss.pos();

            // Dispatch event to apt handler.
//            stream_name = evt->m_stream.as_string();
            stream_name = evt->_stream.as_string();
            
            query_handler_map::iterator handler_it = m_query_handlers.find(stream_name);
            query_handler_map::iterator handler_end = m_query_handlers.end();

            if (handler_it != handler_end) {
                void (*handler)(string, string) = handler_it->second.first;
                size_t tuple_size = handler_it->second.second;
//                for(int c = 0; c < evt->m_inserted_count; ++c) {
//                    string data((c*tuple_size) + evt->m_bin_tuples.data(), tuple_size);
//                    handler(m_output_streams[stream_name], data);
//                }

                DEBUG << "Received " << evt->_inserted_count << " tuples.";

                for(int c = 0; c < evt->_inserted_count; ++c) {
                    string data((c*tuple_size) + evt->_bin_tuples.data(), tuple_size);
                    handler(m_output_streams[stream_name], data);
                }
            } else {
                NOTICE << "No handler for stream " << stream_name;
                NOTICE << "Valid stream handlers:";
                handler_it = m_query_handlers.begin();
                for (; handler_it != handler_end; ++handler_it)
                    NOTICE << handler_it->first;
            }

        }
	  
        if (!iss.stat() || iss.remainder().length() > 0) {
            DEBUG << "Copying " << acc.length() - last_event_pos << " bytes of unfinished data";
            sock->set_partial_recv_buffer(acc.data() + last_event_pos, acc.length() - last_event_pos);
        }

        if (update_stream_map && !stream_name.empty()) {
            ptr<BufferedSocket> prev_sock = get_stream_socket(stream_name);
            if (prev_sock != sock) {
                vector< ptr<BufferedSocket> >::iterator found =
                    find(m_query_sockets.begin(), m_query_sockets.end(), prev_sock);
                if (found != m_query_sockets.end())
                    m_query_sockets.erase(found);
            }

            m_query_sockets.push_back(sock);
            sock->set_close_callback(wrap(this, &GameSubscriber::remove_stream_socket, stream_name));
            
            // previous socket gets garbage collected when it's replaced in the map.
            add_stream_socket(stream_name, sock);

            DEBUG << "Added a stream socket for " << stream_name << " from " << sock->getpeername();

            return true;
        }
    }

    else 
        NOTICE << "Invalid socket found!";

    return false;
}


void GameSubscriber::handle_query_output()
{
    // TODO: almost identical to netio.cpp::handle_incoming_tuples.
    // Abstract!

    vector< ptr<BufferedSocket> >::iterator recv_it = m_query_sockets.begin();
    vector< ptr<BufferedSocket> >::iterator recv_end = m_query_sockets.end();

    bool updated;

    for (; recv_it != recv_end; recv_it++) {
        updated = handle_socket_output( *recv_it, false );
    }

    locking(m_pending_query_mutex)
    {

        // Update any pending queries deleting previous connections.
        if (m_pending_query_sockets.size() > 0) {
            
            vector< ptr<BufferedSocket> > tmp_socks;
            recv_it = m_pending_query_sockets.begin();
            for ( ; recv_it != m_pending_query_sockets.end(); ++recv_it) {
                updated = handle_socket_output( *recv_it, true );
                if (!updated)
                    tmp_socks.push_back(*recv_it);
            }
            m_pending_query_sockets.clear();
            
            copy(tmp_socks.begin(), tmp_socks.end(), 
                 back_inserter(m_pending_query_sockets));
        }
    }

}


void GameSubscriber::update_subscriptions()
{
    InetAddress accept_addr;
    Socket pending_connection;

    //DEBUG << "Attempting to accept a connection...";

    m_listener.set_blocking(false);
    accept_addr = InetAddress(m_client.get_port());
    pending_connection = m_listener.accept(accept_addr);
    while ( pending_connection.stat()  )
    {
        DEBUG << "Accepted a pending subscription";
        ptr<BufferedSocket> recv_socket = ptr<BufferedSocket>(new BufferedSocket(pending_connection));
        recv_socket->set_blocking(false);

        locking(m_pending_query_mutex)
        {
            m_pending_query_sockets.push_back(recv_socket);
        }

        pending_connection = m_listener.accept(accept_addr);
    }

}
