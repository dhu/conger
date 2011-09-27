#ifndef WRAPPER_H
#define WRAPPER_H

#include "WrapperConfig.h"
#include <string>
#include <map>
#include "Catalog.h"
#include "Scores.h"
#include "TQuery.h"
#include "TQueries.h"
#include "JSocket.h"

namespace tiny_dbwrapper
{
class Wrapper
{

private:
    struct QueryComponents
    {
        vector<string>  vstr_select_expressions;
        vector<string>  vstr_where_predicates;
        string          str_group_by_expression;
        vector<string>  vstr_having_predicates;
        string          str_table;
    };

public:
    Wrapper();
private: // no duplicates allowed
    Wrapper(const Wrapper& r_wrapper);
    Wrapper& operator=(const Wrapper& r_wrapper);

public:
    bool            Initialize();
    void            deinitialize();
    wrapper_state    get_state();
    Catalog*        get_catalog();
    string          get_catalog_xml();
    TQuery*         prepare_query(string str_borealis_query_xml, int i_epoch_duration);
    TQuery* re_prep_query(TQuery* t);
    TQueries*       get_queries();
    string          get_queries_xml();

protected:
    bool            box_from_dom(DOMElement& r_domelement,
                               Box& r_box,
                               map<string, string>& rmstr_input_streams,
                               map<string, string>& rmstr_output_streams,
                               QueryComponents& rqc_query_components);
    bool            check_filter_box(Box& r_box, map<string, string>& rmstr_parameters, QueryComponents& rqc_query_components);
    bool            check_map_box(Box& r_box, map<string, string>& rmstr_parameters, QueryComponents& rqc_query_components);
    bool            check_static_join_box(Box& r_box, map<string, string>& rmstr_parameters, QueryComponents& rqc_query_components);
    bool            check_aggregate_box(Box& r_box, map<string, string>& rmstr_parameters, QueryComponents& rqc_query_components);
    bool            check_filter_predicate(string str_predicate);
    bool            check_having_predicate(string str_predicate);
    bool            check_aggregate_expression(string str_expression);
    bool            check_group_by_expression(string str_expression);
    string          generate_tiny_sqlquery(QueryComponents& rqc_query_components);
    int             check_query_syntax(string str_query);

private:
    Catalog         _catalog;
    TQueries        _tqueries;
    wrapper_state    _ws_state;
    JSocket         _cmd_socket;

};

}
#endif //WRAPPER_H
