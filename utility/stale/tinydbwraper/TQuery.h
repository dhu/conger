#ifndef TQUERY_H
#define TQUERY_H

#include "WrapperConfig.h"
#include <string>
#include <vector>
#include "Schema.h"
#include "Box.h"
#include "MedusaClient.h"
#include "DataThread.h"
#include "JSocket.h"
#include "Scores.h"

using namespace Borealis;

namespace tiny_dbwrapper
{
class TQuery
{

    friend class Wrapper;
    friend class DataThread;

public:
    TQuery();
private:
    TQuery(string str_tquery, string str_name, string str_query_stream, Schema& r_schema, vector<Box>& rv_boxes);
public:
    TQuery(const TQuery& r_tquery);

    TQuery& operator=(const TQuery& r_tquery);

    string          get_name();
    string          get_query_stream();
    Schema*         get_query_schema();
    unsigned int    get_box_count();
    Box*            get_box(unsigned int i_index);
    Scores*         get_scores();
    bool            re_prepare(int i_tiny_dbquery_id);
    bool            prepare(string str_tiny_sqlquery, int i_tiny_dbquery_id);
    bool            run(MedusaClient* p_client);
    bool            set_epoch_duration(int i_duration);
    void            stop();
    QueryState      get_state();
    long            set_score_period(long l_score_periode_inms);
    long            get_score_period();
    vector<double>  get_estimate_score_vector();
    vector<double>  get_current_score_vector();
    string          get_xml();
    string          get_tiny_dbquery();

protected:
    void            set_name(string str_name);
    void            set_query_stream(string str_query_stream);
    void            set_query_schema(Schema& r_schema);
    void            add_box(Box& r_box);
    void            set_cmd_socket(JSocket* p_socket);
    JSocket*        get_cmd_socket();
    void            set_tiny_dbquery(string str_tquery);
    void            set_tiny_dbquery_id(int i_tquery_id);
    int             get_tiny_dbquery_id();

private:
    string          _str_tquery;
    string          _str_name;
    string          _str_query_stream;
    Schema          _query_schema;
    vector<Box>     _v_boxes;
    QueryState      _qs_state;
    int             _i_tquery_id;
    DataThread      _data_thread;
    JSocket*        _p_cmd_socket;
    long            _l_score_periode;
    Scores          _scores;
    MedusaClient*   _p_client;

};

}
#endif //TQUERY_H
