#ifndef BROutputReceiver_H
#define BROutputReceiver_H

#include <common.h>
#include <MedusaClient.h>
#include <TupleHeader.h>
#include <fstream>

BOREALIS_NAMESPACE_BEGIN

/**
 * The LMBox class is a class that stores the topoligy information of the Boxes
 */
class BROutputReceiver
{
public:

    /**
     * Constructs a new LMBox.
     * @param id the identifer of the LMBox.
     */
    BROutputReceiver(
        const string& cfg_filename,
        const InetAddress& client_address,
        double statistics_window_size,
        const string& output_file_prefix);

    /**
     * Join the thread of the BROutputReceiver
     */
    void join(); 


private:
    /**
     * Generates inputs
     */
    static void run(IOEventLoop* loop) { loop->run(); }

    /**
     * Subscirbe a an output stream on a node
     * @param stream_name the name of the stream
     * @param node_address the address of the node the stream is from
     */
    void subscribe_output(
        const string& stream_name,
        const string& node_address,
        const InetAddress& client_address);

    /**
     * Handler to dispatch tuples received
     */
    static Status output_handler( ptr<StreamEvent> event );

    /**
     * Record statistics every second
     */
    void record_statistics();

  // Attribute(s)
private:

    pthread_t _thread;

    ptr<MedusaClient> _client;

    static int _number_of_fields;

    double _statistics_window_size;

    double _last_time_stamp;

    /*
     * The file that output average latency statistics
     */
    static ofstream _latency_fs;

    /*
     * The file that output average latency statistics
     */
    ofstream _avg_latency_fs;



    /// statistics
    static unsigned int _number_of_output_tuples;
    static unsigned int _number_of_output_tuples_per_window;
    static double _total_latency_per_window;
    static double _ssq_latency_per_window;
    static double _max_latency_per_window;
    static double _min_latency_per_window;


};


BOREALIS_NAMESPACE_END
#endif


