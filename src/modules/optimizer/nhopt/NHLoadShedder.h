
/*
 * TO DO:
 * . error/spread
 * . intelligent search
 * . initial priority and gap propagation (simply taking priority as 1, and
 *   gap as MAX_GAP for now)
 * . we are currently assuming query trees. general graphs to be added.
 * . drop type for more advanced query plans
 * . node splits
 */

#ifndef NH_LOAD_SHEDDER_H
#define NH_LOAD_SHEDDER_H

#include "common.h"
#include "BasicComponent.h"
#include "FeasibleInputTable.h"
#include "Diagram.h"
#include "MedusaClient.h"

#define MAX_GAP 100 // currently has no significance, just make it large.

#define NEIGHBOR_WAIT        5000   // msec (for parent to shed load)
#define MEASURE_WAIT         170000 // msec (before throughput is measured)
#define MEASURE_PERIOD       80000  // msec (how long to measure)
#define LS_PERIOD            10000  // msec (until we try to shed load again)
#define LS_STABLE            10000  // msec (to stabilize after load shedding)
#define INITIAL_WARM_UP_WAIT 45000  // msec (for catalog and stats)

#define STDDEV_LIMIT         2.0
#define DROP_RATE_LIMIT      0.05

BOREALIS_NAMESPACE_BEGIN

class NHLoadShedder : public BasicComponent
{
  public:

    typedef enum {NONE, LOCAL, NH, LOCAL_TOPDOWN} NHLoadShedderMode;

    NHLoadShedder(string id, NHLoadShedderMode mode);

    ~NHLoadShedder();

    typedef pair<string, int> StreamSinkPair;

    typedef struct DropLocation
    {
        DropLocation()
        {
            load_coef = 0;
            rate_coef = 1.0;
            quality_score = 0;
            input_flag = false;
        }

        int             id; // starts from 1, 0 denotes invalid entry
        Name            in_stream_name;
        Name            dest_box_name; // not used when input_flag = true
        uint16          dest_box_port; // not used when input_flag = true
        string          box_type; // random_drop or window_drop
        BoxParameter    box_params;
        double          load_coef;
        double          reduced_load_coef; // after subtree is fully applied
        double          rate_coef; // fraction of the input rate on this arc
        double          quality_score;
        double          reduced_quality_score; // after subtree is fully applied
        bool            input_flag; // true when this represents a split input
        string          input_stream; // name of the root input stream 

        string as_string() const
        {
            return(string("DropLocation(") + id + ", " +
                   in_stream_name.as_string() + ", " +
                   dest_box_name.as_string() + ", " +
                   dest_box_port + ", " +
                   box_type + ", " +
                   box_params.as_string() + "," +
                   load_coef + "," +
                   reduced_load_coef + "," +
                   rate_coef + "," +
                   quality_score + "," +
                   reduced_quality_score + "," +
                   input_flag + "," +
                   input_stream + ")");
        }

    } DropLocation;
    typedef map<int, DropLocation> DropLocationMap;

    typedef struct InputInfo
    {
        InputInfo()
        {
            source_server = NULL;
        }

        double      spread;
        int         num_points; // number of feasible points along this dim.
        double      max_rate;   // max feasible rate without any local plans
        double      rate_bound; // max feasible rate with all local plans
        vector<int> dl_id;      // first one is for input, others are for sinks
        vector<int> dl_subtree;
        vector<int> dl_subtree_sorted;
        double      reduced_load_coef; // after local plans are fully applied
        double      reduced_quality;   // after local plans are fully applied
        double      total_load_coef;
        double      total_quality;
        InetAddress *source_server; // if any

        string as_string() const
        {
            string s;
            s = string("InputInfo(");
            s = s + spread + ", " + num_points + ", " + max_rate + ", " 
                  + rate_bound + ", { ";
            for (unsigned int i = 0; i < dl_id.size(); i++)
            {
                s = s + dl_id[i] + " ";
            }
            s = s + "}, { ";
            for (unsigned int i = 0; i < dl_subtree.size(); i++)
            {
                s = s + dl_subtree[i] + " ";
            }
            s = s + "}, { ";
            for (unsigned int i = 0; i < dl_subtree_sorted.size(); i++)
            {
                s = s + dl_subtree_sorted[i] + " ";
            }
            s = s + "}, ";
            s = s + reduced_load_coef + ", " +  reduced_quality + ", " 
                  + total_load_coef + ", " + total_quality;
            if (source_server)
            {
                s = s + ", " + to_string(*source_server) + ")";
            }
            else
            {
                s = s + ")";
            }

            return s;
        }

    } InputInfo;
    typedef map<string, InputInfo> InputInfoMap;

    bool is_feasible(FeasibleInputTable::RateMap rates, double *excess_load) 
         throw(AuroraException);
    bool is_feasible_with_plan(FeasibleInputTable::RateMap rates, 
                               double *excess_load) 
         throw(AuroraException);

    // RPC: #include "FeasibleInputTable.h"

    void read_local_catalog()
    {
        RPC< uint32 >  result;

        if (_local_catalog == NULL)
        {
            result = _qp.get_local_catalog();

            if (result.valid())
            {
                _local_catalog = (Diagram  *)(*result);
                DEBUG << "Local catalog has been read...";
            }
            else
            {
                WARN << "Local catalog can not be read...";
            }
        }
    }

    void read_local_statistics()
    {
        _statistics = *_qp.get_statistics();
        DEBUG << "Local statistics has been read...";
    }

    RPC<bool>      is_done();
    AsyncRPC<void> send_FIT(InetAddress addr, FeasibleInputTable fit);
    AsyncRPC<void> measure_remote(); 

    NMSTL_RPC_OBJECT(NHLoadShedder);

  private:

    Diagram            *_local_catalog;
    Statistics         _statistics;

    FeasibleInputTable _fit;       // merged FIT based on local + children
    FeasibleInputTable _local_fit; // FIT based only on local info
    map<InetAddress, FeasibleInputTable> _children_fit; // raw FITs of children

    map<StreamSinkPair, double> _priorities; // priority for each local
                                             // <output stream, sink index> pair
    map<StreamSinkPair, int>    _gaps;       // gap for each local output.. pair

    InputInfoMap        _input_info;     // input stream information
    DropLocationMap     _drop_locations; // all drop locations (used + unused)
    vector<int>         _dl_sort_order;  // actual drop location id's, sorted

    vector<InetAddress> _source_servers;
    vector<InetAddress> _sink_servers;

    bool                _done;           // load has been shed or not.
    NHLoadShedderMode   _mode;           // NONE, LOCAL, NH, or LOCAL_TOPDOWN

    bool                _measuring;      // QP has started measuring tput or not

    ptr<ofstream>       _stats_file;

    int                  _current_fit_index;
    map<string, double > _current_drop_boxes; // box_name -> drop_rate

    void in_thread_init();
    void finish_up();

    void measure_local();

    void setup_none();
    void setup_local();
    void setup_local_td(); // ?
    void setup_nh();

    bool is_leaf_node();
    bool is_root_node();

    void insert_source_server(InetAddress addr);
    void find_source_servers() 
         throw(AuroraBadEntityException);
    void insert_sink_server(InetAddress addr);
    void find_sink_servers();

    int  get_drop_location_id(Name stream, Name box_name, uint16 sink_port);
    void find_dlproperties(Name box_name, string drop_type, 
                           BoxParameter::ParameterMap box_params);
    void find_drop_locations()
         throw(AuroraBadEntityException);

    void compute_load_down(Name box_name, double *load, double *quality)
         throw(AuroraException);
    void compute_load_coefs()
         throw(AuroraException);

    void compute_input_info()
         throw(AuroraException);

    void compute_spreads();

    void collect_dl_from_subtree(Name stream_name, Name box_name, 
                                 double sel); 
    int  pick_dl(Name stream_name)
         throw(AuroraException);
    vector<int>::iterator get_dl_iter(Name stream_name, int dl_id);
    void eliminate_dl_subtree(Name stream_name, Name next_box_name);
    void update_dl_supertree(Name stream_name, int min_dl, CatalogBox *box);
    void sort_dl_subtree(Name stream_name);
    void collect_dl_for_inputs()
         throw(AuroraException);

    double compute_FIT_score(FeasibleInputTable::RateMap rates)
           throw(AuroraException);
    vector<LSPlan::drop_box>::iterator locate_drop(Name box_name,
                                                vector<LSPlan::drop_box> drops);

    void determine_dl_sort_order();
    void create_local_FIT();

    void initial_prep();
    void generate_local_FIT();

    int  find_best_FIT(FeasibleInputTable::RateMap rates);
    void apply_FIT(FeasibleInputTable::RateMap rates, int fit_index);
    bool is_overloaded();
    void shed_load()
         throw(AuroraException);
    void shed_load_continuous()
         throw(AuroraException);

    void propagate_FIT_to_parent(); // assuming single parent

    double compute_merged_rate_down(Name box_name,
                               FeasibleInputTable::RateMap rates,
                               ptr<LSPlan> plan);
    double compute_merged_rate(string stream_name,
                               FeasibleInputTable::RateMap rates,
                               ptr<LSPlan> plan);
    FeasibleInputTable::FIT_Entry merge_entry(FeasibleInputTable::FIT_Entry e1,
                                              FeasibleInputTable::FIT_Entry e2);
    void merge_child(FeasibleInputTable fit);
    void merge_FIT_from_children();

};

BOREALIS_NAMESPACE_END

#endif

