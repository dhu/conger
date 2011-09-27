#include "TQuery.h"

TinyDBWrapper::TQuery::TQuery()
{
    _str_tquery         = "";
    _str_name           = "";
    _str_query_stream    = "";
    _v_boxes.erase(_v_boxes.begin(), _v_boxes.end());
    _qs_state           = qs_invalid;
    _i_tquery_id         = -1;
    _p_cmd_socket        = NULL;
    _l_score_periode     = 5000;
    _p_client           = NULL;
}

TinyDBWrapper::TQuery::TQuery(string str_tquery, string str_name, string str_query_stream, Schema& r_schema, vector<Box>& rv_boxes)
{
    _str_tquery         = str_tquery;
    _str_name           = str_name;
    _str_query_stream    = str_query_stream;
    _query_schema       = r_schema;
    _v_boxes.erase(_v_boxes.begin(), _v_boxes.end());
    _v_boxes.assign(rv_boxes.begin(), rv_boxes.end());
    _qs_state           = qs_invalid;
    _i_tquery_id         = -1;
    _p_cmd_socket        = NULL;
    _l_score_periode     = 5000;
    _p_client           = NULL;
}

TinyDBWrapper::TQuery::TQuery(const TQuery& r_tquery)
{
    _str_tquery         = r_tquery._str_tquery;
    _str_name           = r_tquery._str_name;
    _str_query_stream    = r_tquery._str_query_stream;
    _query_schema       = r_tquery._query_schema;
    _v_boxes.erase(_v_boxes.begin(), _v_boxes.end());
    _v_boxes.assign(r_tquery._v_boxes.begin(), r_tquery._v_boxes.end());
    _qs_state           = r_tquery._qs_state;
    _i_tquery_id         = r_tquery._i_tquery_id;
    _p_cmd_socket        = r_tquery._p_cmd_socket;
    _l_score_periode     = r_tquery._l_score_periode;
    _scores            = r_tquery._scores;
    _p_client           = r_tquery._p_client;
}

TinyDBWrapper::TQuery& TinyDBWrapper::TQuery::operator=(const TQuery& r_tquery)
{
    _str_tquery         = r_tquery._str_tquery;
    _str_name           = r_tquery._str_name;
    _str_query_stream    = r_tquery._str_query_stream;
    _query_schema       = r_tquery._query_schema;
    _v_boxes.erase(_v_boxes.begin(), _v_boxes.end());
    _v_boxes.assign(r_tquery._v_boxes.begin(), r_tquery._v_boxes.end());
    _qs_state           = r_tquery._qs_state;
    _i_tquery_id         = r_tquery._i_tquery_id;
    _p_cmd_socket        = r_tquery._p_cmd_socket;
    _l_score_periode     = r_tquery._l_score_periode;
    _scores            = r_tquery._scores;
    _p_client           = r_tquery._p_client;
    return *this;
}

string TinyDBWrapper::TQuery::get_name()
{
    return _str_name;
}

string TinyDBWrapper::TQuery::get_query_stream()
{
    return _str_query_stream;
}

TinyDBWrapper::Schema* TinyDBWrapper::TQuery::get_query_schema()
{
    return &_query_schema;
}

unsigned int TinyDBWrapper::TQuery::get_box_count()
{
    return _v_boxes.size();
}

TinyDBWrapper::Box* TinyDBWrapper::TQuery::get_box(unsigned int i_index)
{
    if (i_index < 0 || i_index > _v_boxes.size())
    {
        cerr << "access error box " << i_index << ", 0..." << _v_boxes.size() << endl;
        return NULL;
    }

    return &(_v_boxes.at(i_index));
}

TinyDBWrapper::Scores* TinyDBWrapper::TQuery::get_scores()
{
    return &_scores;
}

bool TinyDBWrapper::TQuery::re_prepare(int i_tiny_dbquery_id)
{
  set_tiny_dbquery_id(i_tiny_dbquery_id);
  _qs_state = qs_prepared;
  return true;
}

bool TinyDBWrapper::TQuery::prepare(string str_tiny_sqlquery, int i_tiny_dbquery_id)
{
    if (!_scores.init_scores())
    {
        return false;
    }
    set_tiny_dbquery(str_tiny_sqlquery);
    set_tiny_dbquery_id(i_tiny_dbquery_id);
    // got an Id from TinyDB -> query is prepared
    _qs_state = qs_prepared;
    return true;
}

bool TinyDBWrapper::TQuery::run(MedusaClient* p_client)
{
    if (_qs_state != qs_prepared)
    {
      cerr << "Error: query not in state PREPARED" << (int)_qs_state << endl;
        return false;
    }
    _qs_state = qs_running;
    _p_client = p_client;
    int i_port = DATATHREAD_FIRST_PORT;
    cout << "Run was called ..." << endl;
    while (!_data_thread.create(i_port, this))
    {
        i_port++;
        if (i_port > DATATHREAD_FIRST_PORT + DATATHREAD_NO_OF_PORTS)
        {
            cerr << "Error: no free data port" << endl;
            return false;
        }
    }
    // update TinySQL statement with new epoch duration
    _p_cmd_socket->write_byte(CMD_RUNQUERY);
    _p_cmd_socket->write_int(_i_tquery_id);
    _p_cmd_socket->write_long(_l_score_periode);
    _p_cmd_socket->write_int(i_port);
    _p_cmd_socket->flush();
    if (_p_cmd_socket->read_int() == -1)
    {
        cerr << "Error: query could not be started" << endl;
        return false;
    }
    return true;
}

bool TinyDBWrapper::TQuery::set_epoch_duration(int i_duration)
{
    _p_cmd_socket->write_byte(CMD_SETEPOCHDUR);
    _p_cmd_socket->write_int(_i_tquery_id);
    _p_cmd_socket->write_int(i_duration);
    _p_cmd_socket->flush();
    if (_p_cmd_socket->read_int() == -1)
    {
        cerr << "Error: new epoch duration could not be set" << endl;
    }
    return true;
}

void TinyDBWrapper::TQuery::stop()
{
    if (_qs_state != qs_running)
    {
      cerr << "Error: query not in state RUNNING " << (int)_qs_state << endl;
      return;
    }
    _p_cmd_socket->write_byte(CMD_STOPQUERY);
    _p_cmd_socket->write_int(_i_tquery_id);
    _p_cmd_socket->flush();
    cout << "Sending ...\n";
    if (_p_cmd_socket->read_int() == -1)
    {
        cerr << "Error: query could not be stopped" << endl;
    }
    // cancel anyway
    _data_thread.cancel();
    _qs_state = qs_stopped;
}

QueryState TinyDBWrapper::TQuery::get_state()
{
    return _qs_state;
}

long TinyDBWrapper::TQuery::set_score_period(long l_score_periode_inms)
{
    long l_temp = _l_score_periode;
    _l_score_periode = l_score_periode_inms;
    return l_temp;
}

long TinyDBWrapper::TQuery::get_score_period()
{
    return _l_score_periode;
}

vector<double> TinyDBWrapper::TQuery::get_estimate_score_vector()
{
    return _scores.get_estimate_score_vector();
}

vector<double> TinyDBWrapper::TQuery::get_current_score_vector()
{
    return _scores.get_current_score_vector();
}

/*
<query name="test_query">
    <box name="my_filter" type="filter">
        <input port="1" stream="sensors-stream"/>
        <output port="1" stream="output1"/>
        <param name="predicate.0" value="epoch > 4"/>
        <param name="pass-on-false-port" value="0"/>
    </box>
</query>
*/
string TinyDBWrapper::TQuery::get_xml()
{
    string str_xml = "<query name=\"" + get_name() + "\">\n\t";
    for (unsigned int i=0; i<_v_boxes.size(); i++)
    {
        string str_temp = _v_boxes.at(i).get_xml();
        unsigned int i_pos = 0;
        while ((i_pos = str_temp.find("\n", i_pos)) != string::npos)
        {
            i_pos++;
            if (i_pos < str_temp.length())
            {
                str_temp.insert(i_pos, "\t");
            }
        }
        str_xml.append(str_temp);
    }
    str_xml.append("</query>\n");
    return str_xml;
}

void TinyDBWrapper::TQuery::set_name(string str_name)
{
    _str_name = str_name;
}

void TinyDBWrapper::TQuery::set_query_stream(string str_query_stream)
{
    _str_query_stream = str_query_stream;
}

void TinyDBWrapper::TQuery::set_query_schema(Schema& r_schema)
{
    _query_schema = r_schema;
}

void TinyDBWrapper::TQuery::add_box(Box& r_box)
{
    _v_boxes.insert(_v_boxes.end(), r_box);
}

void TinyDBWrapper::TQuery::set_cmd_socket(JSocket* p_socket)
{
    _p_cmd_socket = p_socket;
}

TinyDBWrapper::JSocket* TinyDBWrapper::TQuery::get_cmd_socket()
{
    return _p_cmd_socket;
}

void TinyDBWrapper::TQuery::set_tiny_dbquery(string str_tquery)
{
    _str_tquery = str_tquery;
}

string TinyDBWrapper::TQuery::get_tiny_dbquery()
{
    return _str_tquery;
}

void TinyDBWrapper::TQuery::set_tiny_dbquery_id(int i_tquery_id)
{
    _i_tquery_id = i_tquery_id;
}

int TinyDBWrapper::TQuery::get_tiny_dbquery_id()
{
    return _i_tquery_id;
}
