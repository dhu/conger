#ifndef BRInputGenerator_H
#define BRInputGenerator_H

#include <common.h>
#include <MedusaClient.h>
#include <TupleHeader.h>
#include "Function.h"

BOREALIS_NAMESPACE_BEGIN


/**
 * The LMBox class is a class that stores the topoligy information of the Boxes
 */
class BRInputGenerator
{

public:

    /**
     * Constructs a new LMBox.
     * @param id the identifer of the LMBox.
     */
    BRInputGenerator(string cfg_filename);

    /**
     * Generates inputs
     */
    void run();

private:

    /*
     * Send data from a given stream
     * @param stream_name the name of the stream
     */
    void send_data(string stream_name);

    /*
     * Batch data from a given stream
     * @param stream_name the name of the stream
     */
    void batch_data(
            ptr<StreamEvent> event, 
            const string& stream_name);

    /*
     * Start the given loop
     */
    static void run_client(IOEventLoop* loop) { loop->run(); }

private:

    IOEventLoop _loop;

    /*
     * A map from input stream name to send stream event
     */
    map<string, ptr<MedusaClient> > _stream_clients;

    /*
     * A map from input stream name to interarrival time functions
     */
    map<string, ptr<Function> > _functions;

    /*
     * A map from input stream name to input data function
     */
    map<string, vector<ptr<Function> > > _input_data_functions;

    /*
     * A map from input stream name to input data function
     */
    map<string, double> _input_rate_multipliers;

    /*
     * A map from input stream name to input data function
     */
    map<string, int> _batch_size;

    /*
     * A map from input stream name to last sending data time
     */
    map<string, double> _next_sending_time;

    /**
     * The starting time
     */
    double _start_time;
};


BOREALIS_NAMESPACE_END
#endif


