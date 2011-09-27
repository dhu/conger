
#ifndef LOAD_SHEDDER_H
#define LOAD_SHEDDER_H

#define LS_PERIOD         20000 // msec
#define MAX_GAP           100
#define DROP_RATE_LIMIT   0.05
#define FIXED_FIT_SPREAD  10   // along one dimension, by default
#define TOTAL_FIT_ERROR   1   // max error we can tolerate in total tput
//#define PERCENT_FIT_ERROR 5   // % max error we can tolerate in total tput 
#define MAX_RATE          50 // tuples/sec
#define CAPACITY          0.95 // system capacity

extern "C"
{
#include "glpk.h"
}
#include "common.h"
#include "RegionServer.h"
#include "rpc_QueryProcessor.h" // for RPC<Statistics>
#include "LSPlan.h"
#include "FeasibleInputTable.h"
#include <fstream>

BOREALIS_NAMESPACE_BEGIN

class LoadShedder
{

  public:

    LoadShedder(RegionalCatalog &catalog,
                map<string, Statistics > &statistics,
                IOEventLoop *loop,
                int load_shedding_quality_metric,
                bool active = false);

    LoadShedder(RegionalCatalog &catalog,
                map<string, Statistics > &statistics,
                IOEventLoop *loop,
                int load_shedding_quality_metric,
                int offline_duration,
                int percent_fit_error,
                bool active = false);

    ~LoadShedder();

    string format_double(double d, int p)
    {
        ostringstream o;
        int prec = numeric_limits<long double>::digits10;
        o.precision(prec);
        o << d;
        string s = o.str();
        size_t n = s.find('.');
        if ((n != string::npos) && (s.size() > n+p))
        {
            s[n+p] = '\0';
        }

        return string(s.c_str());
    }

    typedef struct DropLocation
    {
        int             id;
        Name            in_stream_name;
        Name            dest_box_name;
        uint16          dest_box_port;
        string          dest_node;
        string          box_type; // random_drop or window_drop
        BoxParameter    box_params;
        string          node;

        string as_string() const
        {
            return(string("DropLocation(") + id + ", " +
                   in_stream_name.as_string() + ", " +
                   dest_box_name.as_string() + ", " +
                   dest_box_port + ", " +
                   dest_node + ", " +
                   box_type + ", " +
                   box_params.as_string() + ", " +
                   node + ")");
        }

    } DropLocation;

    typedef map<int, DropLocation>            DropLocationMap;

    typedef map<int, double>                  DLLoadFactorMap;

    typedef map<string, DLLoadFactorMap>      NodeDLLoadFactorMap;

    // [node][dlprefix_index] -> <input_name, rate_factor>
    typedef map<string, 
                map<int, 
                    pair<string, double> > >  NodeDLInputRateFactorMap;

    typedef map<string, map<string, double> > TupleDeliveryMap;

    typedef map<int, vector<int> >            DLPrefixMap;



    /** NEW STUFF **/

    typedef map<string, pair<double, double> > InfeasibleArea;
    typedef struct Mapping
    {
        Mapping(InfeasibleArea i, FeasibleInputTable::RateMap r, double s)
        {
            if_area = i;
            f_point = r;
            score = s;
        }

        string format_double(double d, int p)
        {
               ostringstream o;
            int prec = numeric_limits<long double>::digits10;
            o.precision(prec);
            o << d;
            string s = o.str();
            size_t n = s.find('.');
            if ((n != string::npos) && (s.size() > n+p))
            {
                s[n+p] = '\0';
            }

            return string(s.c_str());
        }
        
        string as_string()
        {
            string s;
            s += "\nFeasible Point = { ";
            for (FeasibleInputTable::RateMapIter it = f_point.begin();
                 it != f_point.end(); it++)
            {
                s += format_double(it->second, 3);
                s += " ";
            }
            s += "}, ";
            s += "Score = { ";
            s += format_double(score, 3);
            s += " }, ";
            s += "\nArea = { ";
            for (InfeasibleArea::iterator it = if_area.begin();
                 it != if_area.end(); it++)
            {
                s += "[";
                s += format_double((it->second).first, 3);
                s += ", ";
                s += format_double((it->second).second, 3);
                s += "] ";
            }
            s += "}";

            return s;
        }

        InfeasibleArea              if_area; // this infeasible area should be,
        FeasibleInputTable::RateMap f_point; // brought to this feasible point, 
        double                      score;   // which has a score of this.
    } Mapping;
    vector<Mapping>                 _mappings; // all the mappings so far.

    /***************/



    /************ FIT-based stuff *********************************/

    typedef struct DropLocationPlus
    {
        DropLocationPlus()
        {
            load_coef = 0;
            reduced_load_coef = 0;
            rate_coef = 1.0;
            quality_score = 0;
            reduced_quality_score = 0;
            input_flag = false;
        }

        int             id; // starts from 1, 0 denotes invalid entry
        Name            in_stream_name;
        Name            dest_box_name; // not used when input_flag = true
        uint16          dest_box_port; // not used when input_flag = true
        string          node;
        string          box_type; // random_drop or window_drop
        BoxParameter    box_params;
        double          load_coef;
        double          reduced_load_coef; // after subtree is fully applied
        double          rate_coef; // fraction of the input rate on this arc
        double          quality_score;
        double          reduced_quality_score; // after subtree is fully applied
        bool            input_flag; // true when this represents a split input
        string          input_stream; // name of the root input stream 

        string format_double(double d, int p)
        {
            ostringstream o;
            int prec = numeric_limits<long double>::digits10;
            o.precision(prec);
            o << d;
            string s = o.str();
            size_t n = s.find('.');
            if ((n != string::npos) && (s.size() > n+p))
            {
                s[n+p] = '\0';
            }

            return string(s.c_str());
        }
        
        string as_string() 
        {
            return(string("DropLocationPlus(") + id + ", " +
                   in_stream_name.as_string() + ", " +
                   dest_box_name.as_string() + ", " +
                   dest_box_port + ", " +
                   node + ", " +
                   box_type + ", " +
                   box_params.as_string() + "," +
                   format_double(load_coef, 3) + "," +
                   format_double(reduced_load_coef, 3) + "," +
                   format_double(rate_coef, 3) + "," +
                   format_double(quality_score, 3) + "," +
                   format_double(reduced_quality_score, 3) + "," +
                   input_flag + "," +
                   input_stream + ")");
        }

    } DropLocationPlus;
    typedef map<int, DropLocationPlus> DropLocationPlusMap;

    typedef struct InputInfo
    {
        InputInfo()
        {
            spread = 100; // max rate for all inputs
            num_points = 0;
            max_rate = 0;
            rate_bound = 0;
            dl_id.clear();
            dl_subtree.clear();
            dl_subtree_sorted.clear();
            reduced_load_coef = 0;
            reduced_quality = 0;
            total_load_coef = 0;
            total_quality = 0;
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
        double      total_quality;     // can also be used as path selectivity
        InetAddress *source_server;    // if any

        string format_double(double d, int p)
        {
               ostringstream o;
            int prec = numeric_limits<long double>::digits10;
            o.precision(prec);
            o << d;
            string s = o.str();
            size_t n = s.find('.');
            if ((n != string::npos) && (s.size() > n+p))
            {
                s[n+p] = '\0';
            }

            return string(s.c_str());
        }
        
        string as_string() 
        {
            string s;
            s = string("InputInfo(");
            s = s + format_double(spread, 3) + ", " + num_points + ", " 
                  + format_double(max_rate, 3) + ", " 
                  + format_double(rate_bound, 3) + ", { ";
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
            s = s + format_double(reduced_load_coef, 3) + ", " 
                  + format_double(reduced_quality, 3) + ", " 
                  + format_double(total_load_coef, 3) + ", " 
                  + format_double(total_quality, 3);
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

    /**************************************************************/

    void activate();
    void de_activate();
    bool get_status();

    TupleDeliveryMap get_current_tuple_delivery();

    void add_ignore_node(string node_name);

    void run(bool active, bool *ls_done, int ls_level);

    int  select_plan();
    bool add_default_plan();
    void refresh_rates();

    double compute_score(map<string, double> rates);
    void change_plan_id(int old_id, int new_id);
    bool add_new_plan(int id, map<string, double> rates);
    int  plan_exists(map<string, double> rates, 
                     map<int, map<string, double> > plans);
    bool generate_new_plan();
    void upload_plans();

    void prepare_stable_metadata();

    void subtract(InfeasibleArea &area1, InfeasibleArea area2);
    void generate_feasible_boundary();
    pair<double, int> generate(FeasibleInputTable::RateMap rates,
                               map<string, double>::iterator iter,
                               double pivot_value, int map_index);
    void precompute_plans();

    bool compute_node_loads_original();
    bool is_overloaded(map<string, double> rates);
    void print_plans();
    void print_plan(int plan_id);
    void shed_load(int plan_id);

    /************ FIT-based stuff *********************************/

    void prepare_node_metadata();
    void precompute_FIT_plans();
    //bool compute_node_loads_FIT();
    void shed_load_FIT();

    /**************************************************************/

    string as_string() const;

  private:

    IOEventLoop                     *_loop;
    bool                            _no_drops;
    bool                            _active;

    RegionalCatalog&                _catalog;
    map<string, Statistics>&        _statistics;

    DropLocationMap                 _drop_locations;

    set<string>                     _ignore_nodes;
    set<string>                     _load_shedder_nodes;
    set<string>                     _input_streams;

    NodeDLLoadFactorMap             _load_factor_matrix;

    NodeDLLoadFactorMap             _load_factor_matrix2;

    NodeDLInputRateFactorMap        _rate_factor_matrix;

    DLPrefixMap                     _dl_prefixes;

    map<int, double>                _original_solution_variables;

    map<int, set<string> >          _output_cardinality;

    map<int, map<string, double> >  _output_cardinality2;

    map<int, map<string, double> >  _output_total_selectivity;

    map<int, map<string, string> >  _output_input_mapping;

    map<int, double>                _stream_rates; // for dl arcs
    map<string, double>             _input_rates;  // for global diagram inputs

    //map<string, map<string, double> > _node_input_rates; // for node inputs
    map<string, map<string, double> > _node_input_load_coefs; // for node inputs
    map<string, map<string, map<string, double> > > 
                                      _node_input_rate_coefs; // for node inputs

    LPX                             *_lp;            // current linear program

    map<string, LSPlan>             _additional_plans; // to adjust inputs
    map<string, LSPlanMessage>      _node_plans;       // current plans per node

    map<int, double>                _plan_scores;
    map<int, map<string, double> >  _precomputed_plan_ids;

    map<int, TupleDeliveryMap>      _tuple_delivery; // % delivery map per plan

    int                             _current_plan_id;

    map<string, double>             _node_loads;

    int                             _ls_level; // 0: capacity = 1.0
                                               // (0, 1.0]: capacity = _ls_level

    int                             _quality_metric;
    int                             _offline_duration;
    int                             _percent_fit_error;

    map<string, bool>               _overloaded_nodes;

    /************ FIT-based stuff *********************************/

    map<string, InputInfoMap>           _input_info_pernode;
    map<string, DropLocationPlusMap>    _drop_locations_pernode;
    map<string, set<string> >           _source_servers_pernode;
    map<string, set<string> >           _sink_servers_pernode;
    map<string, FeasibleInputTable>     _node_fit;
    map<string, vector<int> >           _dl_sort_order;
    map<string, int>                    _current_fit_index;
    map<string, map<string, double> >   _current_drop_boxes;
    map<string, double>                 _max_rates;
    fstream                             _fit_data;

    /**************************************************************/

    int    solve_lp();
    bool   construct_lp();
    bool   construct_lp2(int id, map<string, double> rates);
    void   get_children(int prefix_id, set<int> *children);

    void   get_stream_rates();
    double get_stream_rate(string stream_name, string node);

    void   output_cardinality(Name box_name, set<string> *output_set);
    void   compute_output_cardinalities();

    void   go_to_outputs(Name box_name, string stream_name, int dl_prefix_id, 
                       double selectivity);
    void   go_to_inputs(Name box_name, int dl_prefix_id);
    void   compute_output_info();

    void   compute_load(Name box_name, string node, double *load);
    void   get_there(Name box_name, string node, double *load);
    double compute_load_factor(int loc_id, string node);
    void   generate_load_factor_matrix();

    void   find_drop_locations();
    int    get_drop_location_id(Name stream, Name box_name, uint16 sink_port);

    void   compute_load_2(Name box_name, string node, double *load);
    void   get_there_2(Name box_name, string node, double *load);
    double compute_load_factor_2(int prefix_id, string node);
    void   generate_load_factor_matrix_2();

    bool   is_in_prefix(int loc_id, int prefix_id);
    double compute_rate_factor_node(string node, int prefix_id, string box_name,
                                    double selectivity);
    double compute_rate_factor(string node, int prefix_id, string box_name,
                               double selectivity);
    void   generate_rate_factor_matrix();

    void   find_prefixes(Name box_name, vector<int> prefix, double fraction);
    void   find_dlprefixes();

    void   find_dlproperties(Name box_name, string drop_type,
                             BoxParameter::ParameterMap box_params);

    //bool isInputStream(string stream_name);
    bool   is_start_location(int loc_id);
    bool   exists_upstream_location(Name stream_name);

    bool   compute_node_loads();

    void   assign_rate_coefs(Name box_name, string input_name, double sel);
    void   compute_load_node_input(Name box_name, string node, double *load);
    void   generate_node_input_info();
    void   record_plan_point(int id, map<string, double> rates, 
                             double score);
    void   record_fit_point(int id, FeasibleInputTable::RateMap rates, 
                            int state, double score);
    bool   is_cross_stream(Name stream_name);

    bool   generate_lsplans();
    void   finish(bool *ls_done);

    /************ FIT-based stuff *********************************/

    void get_stream_rates_pernode();
    int  get_drop_location_id_pernode(Name stream, Name box_name, 
                                      uint16 sink_port, string &node);
    void find_dlproperties_pernode(Name box_name, string drop_type,
                                   BoxParameter::ParameterMap box_params);
    void find_drop_locations_pernode();
    bool is_leaf_node(string node);
    bool is_root_node(string node);
    void find_source_sink_servers();
    void compute_load_down(Name box_name, double *load, double *quality);
    void compute_load_coefs_pernode();
    void compute_input_info_pernode();
    void compute_input_spread_pernode(string node);
    void collect_dl_from_subtree_pernode(string node, Name stream_name, 
                                         Name box_name, double sel);
    int  pick_dl_pernode(string node, Name stream_name);
    vector<int>::iterator get_dl_iter_pernode(string node, Name stream_name, 
                                              int dl_id);
    void eliminate_dl_subtree_pernode(string node, Name stream_name, 
                                      Name next_box_name);
    void update_dl_supertree_pernode(string node, Name stream_name, 
                                     int min_dl, CatalogBox *box);
    void sort_dl_subtree_pernode(string node, Name stream_name);
    void collect_dl_for_inputs_pernode(string node);
    void determine_dl_sort_order_pernode(string node);

    double compute_FIT_score(string node, FeasibleInputTable::RateMap rates);
    bool is_feasible_pernode2(string node, FeasibleInputTable::RateMap rates,
                              ptr<LSPlan> plan);
    void compute_load_with_drops(string node, ptr<LSPlan> plan,
                                 Name box_name, double *load);
    bool is_feasible_pernode(string node, FeasibleInputTable::RateMap rates, 
                             double *excess_load);
    bool is_feasible_with_plan_pernode(string node, 
                                       FeasibleInputTable::RateMap rates, 
                                       double *excess_load);
    double compute_merged_rate_down_pernode(string node, Name stream_name,
                                            FeasibleInputTable::RateMap rates,
                                            ptr<LSPlan> plan);
    double compute_merged_rate_pernode(string node, string stream_name, 
                                       FeasibleInputTable::RateMap rates, 
                                       ptr<LSPlan> plan);
    FeasibleInputTable::FIT_Entry merge_entry(FeasibleInputTable::FIT_Entry e1,
                                              FeasibleInputTable::FIT_Entry e2);
    void merge_child_pernode(string node, FeasibleInputTable fit);
    ptr<LSPlan> generate_FIT_plan(string node, 
                                  FeasibleInputTable::RateMap rates,
                                  double score);
    void generate_FIT_points(string node, 
                             InputInfoMap::iterator iter,
                             double capacity_budget, 
                             FeasibleInputTable::RateMap &rates);
    void generate_node_FIT(string node, double spread);
    void generate_FIT_recursive(string node, double spread);
    void generate_FIT_plans(string node);
    void generate_FIT();
    /*
    void apply_FIT_pernode(string node, 
                           FeasibleInputTable::RateMap current_rates, 
                           int fit_index);
    int  find_best_FIT_pernode(string node, FeasibleInputTable::RateMap rates);
    */

    /**************************************************************/

};
NMSTL_TO_STRING(LoadShedder);

BOREALIS_NAMESPACE_END

#endif
