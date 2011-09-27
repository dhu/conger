#include "BorealisLoadManager.h"
#include "LMQueryNetwork.h"
#include "LMStatisticsManager.h"
#include "LMPerformanceMonitor.h"
#include "LMController.h"
#include "LMStaticDistributor.h"
#include "MedusaClient.h"
#include "LMParams.h"
#include <pthread.h>
#include "Timeval.h"

//#define DEBUG_BOX_MIGRATION_TIME

BOREALIS_NAMESPACE_BEGIN
using namespace time_tools;

BorealisLoadManager::BorealisLoadManager(
        double update_interval,
        RegionalCatalog& catalog,
        map<string, Statistics>& statistics):
    _catalog(catalog),
    _statistics(statistics),
    _accept_input_command(false)
{
    LMParams::time_param.load_distribution_interval = update_interval;
    LMParams::runtime_param.warm_up_time =
        LMParams::distribution_param.initial_warm_up_period +
        LMParams::distribution_param.statistics_sample_period *
        LMParams::distribution_param.number_of_samples;
}

bool BorealisLoadManager::update_topology()
{
    DEBUG << "Updating topoloty...";
    if (!BorealisCatalogReader::read_catalog(_catalog))
        return false;
    DEBUG << "Topology updated";
    return true;
}

bool BorealisLoadManager::update_statistics()
{
    DEBUG << "Updating statistics ...";

    double precision = LMParams::distribution_param.statistics_sample_period;
    int window_size = static_cast<int>(
            LMParams::time_param.load_distribution_interval / precision);

    if (window_size > LMParams::distribution_param.number_of_samples)
        window_size = LMParams::distribution_param.number_of_samples;

    if (!BorealisStatisticsReader::read_statistics(
                _statistics,
                precision,
                window_size))
        return false;

    LMStatisticsManager::get_singleton()->update_statistics(window_size);

    DEBUG << "Statistics update finished";
    //cout << *(LMStatisticsManager::getSingleton()) << endl;

    return true;
}

void BorealisLoadManager::redistribute_load()
{
#ifdef DEBUG_BOX_MIGRATION_TIME

    int number_of_boxes = 0;
    for (LMNodeMap::const_iterator 
            it = LMNodeManager::get_singleton()->get_lmnodes().begin();
            it != LMNodeManager::get_singleton()->get_lmnodes().end();
            ++it)
    {
        number_of_boxes += it->second.get_boxes().size();
    }

    LMNodeMap::const_iterator it = 
        LMNodeManager::get_singleton()->get_lmnodes().begin();

    static int count = 0;
    if (count < 6 && 
        number_of_boxes == 3)
    {
        ++count;

        // wait until all operators are added
        sleep(10);
        NOTICE << "Start: DEBUG_BOX_MIGRATION_TIME";
        cout << *LMNodeManager::get_singleton();

        IdSet boxes_to_move;
        for (int i = 1; i <= LMParams::runtime_param.number_of_boxes_to_move; ++i)
        {
            string box_id = string("b");
            if (i < 10)
                box_id = box_id + "00";
            else
                box_id = box_id + "0";
            box_id = box_id + i;
            boxes_to_move.insert(box_id);
        }

        boxes_to_move.clear();
        switch (count)
        {
            case 1:
                return;
            case 2:
                boxes_to_move.insert("b001");
                break;
            case 3:
                boxes_to_move.insert("b003");
                break;
            case 4:
                boxes_to_move.insert("b003");
                break;
            case 5:
                ++it;
                boxes_to_move.insert("b003");
                break;
            case 6:
                ++it;
                boxes_to_move.insert("b001");
                break;
            default:
                return;
        }


        const IdType& sender_id = it->first;
        ++it;
        const IdType& receiver_id = it->first;


        LMParams::runtime_param.move_box_active = true;
        move_boxes_between_nodes( 
            sender_id,
            receiver_id,
            boxes_to_move );
        NOTICE << "End: DEBUG_BOX_MIGRATION_TIME";

        sleep(5);

        ofstream ofs;
        ofs.open(LMParams::runtime_param.status_filename.c_str(), ios::out);
        if (ofs)
        {
            ofs << "experimentFinished" << endl;
        }
        else
        {
            NOTICE << "Can not open status file : " << LMParams::runtime_param.status_filename.c_str();
        }
        ofs.close();

    }
#endif

    display_info();

    double interval = Timeval::now().double_value() - _last_box_migration_time;
    if (interval < LMParams::runtime_param.warm_up_time + LMParams::distribution_param.statistics_sample_period)
    {
        cout << endl;
        NOTICE << "No load distribution action before statistics are stable after load migration";
    }

    LMLoadManager::redistribute_load();

    if (_accept_input_command)
    {
        cout << endl;
        NOTICE << "Type h<enter> for more display options.";
    }
}

void BorealisLoadManager::display_info()
{
    if (LMParams::display_param.show_box_location)
        cout << *(LMNodeManager::get_singleton()) ;

    if (LMParams::display_param.show_query_network)
      cout << *(LMQueryNetwork::get_singleton()) << endl;

    if (LMParams::display_param.show_lmstatistics_manager &&
            LMParams::display_param.show_average_statistics) 
    {
        LMStatisticsManager::get_singleton()->output_average_statistics(cout);
    }
    else if (LMParams::display_param.show_lmstatistics_manager &&
            !LMParams::display_param.show_average_statistics) 
    {
        cout << *(LMStatisticsManager::get_singleton()) << endl;
    }

    if (LMParams::display_param.show_node_load)
        LMStatisticsManager::get_singleton()->output_statistics(
                    cout, 
                    LMStatisticsManager::NODE_LOAD,
                    LMParams::display_param.show_average_statistics);

    if (LMParams::display_param.show_box_load)
        LMStatisticsManager::get_singleton()->output_statistics(
                    cout, 
                    LMStatisticsManager::BOX_LOAD,
                    LMParams::display_param.show_average_statistics);

    if (LMParams::display_param.show_stream_rate)
        LMStatisticsManager::get_singleton()->output_statistics(
                    cout, 
                    LMStatisticsManager::STREAM_RATE,
                    LMParams::display_param.show_average_statistics);

    if (LMParams::display_param.show_selectivity)
        LMStatisticsManager::get_singleton()->output_statistics(
                    cout, 
                    LMStatisticsManager::SELECTIVITY,
                    LMParams::display_param.show_average_statistics);

    if (LMParams::display_param.show_box_cost)
        LMStatisticsManager::get_singleton()->output_statistics(
                    cout, 
                    LMStatisticsManager::BOX_COST,
                    LMParams::display_param.show_average_statistics);
}

void BorealisLoadManager::move_boxes_between_nodes(
                const IdType& sender_id,
                const IdType& receiver_id,
                const IdSet& box_ids)
{

    if (!LMParams::runtime_param.move_box_active)
        return;

    if (LMParams::display_param.show_basic_info)
    {
        NOTICE << "Moving boxes: ";
        cout << endl << "*******************************************" << endl;
        cout << "(" << sender_id << ")->(" << receiver_id << ")"
                      << box_ids << endl;
        cout << endl << "*******************************************" << endl;
    }

    vector<Name> boxes_to_move;
    for (IdSet::const_iterator it = box_ids.begin();
            it != box_ids.end();
            ++it)
    {
        boxes_to_move.push_back(Name(*it));
    }

    // just for debugging
    /*
    boxes_to_move.clear();
    static bool move = true;
    if (move)
    {
        NOTICE << "move b1 b4 ";
        boxes_to_move.push_back(Name("b1"));
        boxes_to_move.push_back(Name("b4"));
        move = false;
    }
    */

    InetAddress dest = InetAddress(receiver_id);

    MedusaID box_src(sender_id);
    MedusaID box_dest(receiver_id);

    ptr<MedusaClient> client = (ptr<MedusaClient>) new MedusaClient(sender_id);
    Remote<QueryProcessor> qp = client->qp_for(box_src);

    DEBUG << "-------------before move box";
    RPC<void> r = qp.move_boxes(boxes_to_move, box_dest);
    DEBUG << "-------------after move box";

    if ( !r.stat() )
            //ERROR << r.stat();
            FATAL << r.stat();


    int sleep_time = 1;
    NOTICE << "Sleep " << sleep_time << " seconds after box migration between a node pair.";
    sleep(sleep_time);

    _last_box_migration_time = Timeval::now().double_value();
}

void BorealisLoadManager::start_input_reading()
{
    pthread_t id;
    if (pthread_create(&id,
                                      0,
                                      (void*(*)(void*))accept_input_command,
                                      (void*)this) < 0)
    {
        WARN << "Can not start input reader thread.";
    }
}

void BorealisLoadManager::accept_input_command(BorealisLoadManager* lm)
{
    lm->_accept_input_command = true;
    while(1)
    {
        string command;
        cin >> command;
        if (command == "s")
            stop_all_display();
        else if (command == "b")
            LMParams::display_param.show_basic_info =
              !LMParams::display_param.show_basic_info;
        else if (command == "q")
            LMParams::display_param.show_query_network =
              !LMParams::display_param.show_query_network;
        else if (command == "l")
            LMParams::display_param.show_box_location =
              !LMParams::display_param.show_box_location;
        else if (command == "rs")
            LMParams::display_param.show_gathered_statistics =
              !LMParams::display_param.show_gathered_statistics;
        else if (command == "st")
            LMParams::display_param.show_lmstatistics_manager =
              !LMParams::display_param.show_lmstatistics_manager;
        else if (command == "nl")
            LMParams::display_param.show_node_load =
              !LMParams::display_param.show_node_load;
        else if (command == "bl")
            LMParams::display_param.show_box_load =
              !LMParams::display_param.show_node_load;
        else if (command == "sr")
            LMParams::display_param.show_stream_rate =
              !LMParams::display_param.show_stream_rate;
        else if (command == "sl")
            LMParams::display_param.show_selectivity =
              !LMParams::display_param.show_selectivity;
        else if (command == "bc")
            LMParams::display_param.show_box_cost =
              !LMParams::display_param.show_box_cost;
        else if (command == "m")
            LMParams::runtime_param.move_box_active =
                !LMParams::runtime_param.move_box_active;
        else if (command == "ts")
            LMParams::display_param.show_average_statistics = false;
        else if (command == "avg")
            LMParams::display_param.show_average_statistics = true;
        else
        {
            cout << "Type the following command to toggle between display." << endl;
            cout << "b : basic load distribution informaiton" << endl;
            cout << "q : query graph" << endl;
            cout << "l : box locations" << endl;
            cout << "rs : received raw statatistics " << endl;
            cout << "st : all statatistics maintained by LMStatisticsManager " << endl;
            cout << "nl : node load" << endl;
            cout << "bl : box load" << endl;
            cout << "sr : stream rate" << endl;
            cout << "sl : selectivity" << endl;
            cout << "bc : box cost" << endl;
            cout << "ts : show time series statistics" << endl;
            cout << "avg : show average statistics" << endl;
            cout << "s : stop all display" << endl;
        }
        lm->display_info();
    }
}

void BorealisLoadManager::stop_all_display()
{
    LMParams::display_param.show_basic_info = false;
    LMParams::display_param.show_query_network = false;
    LMParams::display_param.show_box_location = false;
    LMParams::display_param.show_gathered_statistics = false;
    LMParams::display_param.show_lmstatistics_manager = false;
    LMParams::display_param.show_node_load = false;
    LMParams::display_param.show_box_load = false;
    LMParams::display_param.show_stream_rate = false;
    LMParams::display_param.show_selectivity = false;
    LMParams::display_param.show_box_cost = false;
}


BOREALIS_NAMESPACE_END
