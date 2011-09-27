#include "args.h"
#include "Movebox_TestMarshal.h"

uint32 BATCH_SIZE = 10;
uint32 SLEEP_TIME = 3000;

double seqno = 0.0;

int iterations = 0;
int stable_condition = 7;

// Variables for testing move
bool first = true;
bool shift = false;

InetAddress node1;
InetAddress node2;
InetAddress node3;

InetAddress location;

/*
 * Tests 3 types of moves:
 *
 * -- slides:
 *
 *       Site  1    Site  2    Site  3
 *   -||-a--b--c-||-d--e--f-||-g--h--i-||-
 *
 * c: Site 1 => Site 2 => Site 1 => Site 2 ....
 *
 * -- edge shifts:
 *
 *       Site  1    Site  2    Site  3
 *   -||-a--b--c-||-d--e--f-||-g--h--i-||-
 *
 * c: Site 1 => Site 3 => Site 1 => Site 3 ....
 *
 * -- in-node shifts:
 *
 *       Site  1    Site  2    Site  3
 *   -||-a--b--c-||-d--e--f-||-g--h--i-||-
 *
 * b: Site 1 => Site 3 => Site 1 => Site 3 ....
 *
*/

void test_slide(MedusaClient* _client)
{
    if (first)
    {
        location = node1;
        first = false;
    }

    Name box_c("box_c");
    vector<Name> boxes_to_move;
    boxes_to_move.push_back(box_c);

    InetAddress dest = (location == node1? node2 : node1);

    MedusaID box_src(to_string(location));
    MedusaID box_dest(to_string(dest));

    Remote<QueryProcessor> qp = _client->qp_for(box_src);
    RPC<void> r = qp.move_boxes(boxes_to_move, box_dest);

    if ( !r.stat() )
        ERROR << r.stat();

    location = dest;
}

void test_shift_edge(MedusaClient* _client)
{
    if (first)
    {
        location = node1;
        first = false;
    }

    Name box_c("box_c");
    vector<Name> boxes_to_move;
    boxes_to_move.push_back(box_c);

    InetAddress dest = (location == node1? node3 : node1);

    MedusaID box_src(to_string(location));
    MedusaID box_dest(to_string(dest));

    Remote<QueryProcessor> qp = _client->qp_for(box_src);
    RPC<void> r = qp.move_boxes(boxes_to_move, box_dest);

    if ( !r.stat() )
        ERROR << r.stat();

    location = dest;
}

void test_shift_innode(MedusaClient* _client)
{
    if (first)
    {
        location = node1;
        first = false;
    }

    Name box_b("box_b");
    vector<Name> boxes_to_move;
    boxes_to_move.push_back(box_b);

    InetAddress dest = (location == node1? node3 : node1);

    MedusaID box_src(to_string(location));
    MedusaID box_dest(to_string(dest));

    Remote<QueryProcessor> qp = _client->qp_for(box_src);
    RPC<void> r = qp.move_boxes(boxes_to_move, box_dest);

    if ( !r.stat() )
        ERROR << r.stat();

    location = dest;
}

void Movebox_TestMarshal::receivedProbability(dmt_schema* tuple)
{
    NOTICE << "Win p = " << tuple->rand;
}

void Movebox_TestMarshal::sentRng()
{
    if (iterations > stable_condition)
    {
    DEBUG << "Attempting to move boxes....";
        test_shift_innode(_client);
        iterations = 0;
    }

    for (uint32 i = 0; i < BATCH_SIZE; i++)
    {
    Rng tuple;
        //tuple._data.rand = drand48();
    tuple._data.rand = seqno;
        seqno += 0.0001;

    batchRng( &tuple );
    }

    iterations++;

    sendRng(SLEEP_TIME);
}

int main(int argc, const char* argv[])
{
    Movebox_TestMarshal _app_handle;

    string ip = "127.0.0.1";
    int port = 15000;
    stdArgs( argc, argv, ip, port );

    node1 = InetAddress("127.0.0.1", 15000);
    node2 = InetAddress("127.0.0.1", 17000);
    node3 = InetAddress("127.0.0.1", 19000);

    int32 status = _app_handle.open();
    if (status)
    {
    WARN << "Failed to deploy query network";
    }
    else
    {
    _app_handle.sentRng();
    _app_handle.runClient();
    }

    return status;
}
