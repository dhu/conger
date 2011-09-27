#include "TQueries.h"

TinyDBWrapper::TQueries::TQueries()
{
    _v_tqueries.erase(_v_tqueries.begin(), _v_tqueries.end());
}

TinyDBWrapper::TQueries::TQueries(vector<TQuery>& rv_tqueries)
{
    _v_tqueries.erase(_v_tqueries.begin(), _v_tqueries.end());
    _v_tqueries.assign(rv_tqueries.begin(), rv_tqueries.end());
}

TinyDBWrapper::TQueries::TQueries(const TQueries& r_tqueries)
{
    _v_tqueries.erase(_v_tqueries.begin(), _v_tqueries.end());
    _v_tqueries.assign(r_tqueries._v_tqueries.begin(), r_tqueries._v_tqueries.end());
}

TinyDBWrapper::TQueries& TinyDBWrapper::TQueries::operator=(const TQueries& r_tqueries)
{
    _v_tqueries.erase(_v_tqueries.begin(), _v_tqueries.end());
    _v_tqueries.assign(r_tqueries._v_tqueries.begin(), r_tqueries._v_tqueries.end());
    return *this;
}

unsigned int TinyDBWrapper::TQueries::get_query_count()
{
    return _v_tqueries.size();
}

TinyDBWrapper::TQuery* TinyDBWrapper::TQueries::get_query(unsigned int i_index)
{
    if (i_index < 0 || i_index > _v_tqueries.size())
    {
        cerr << "access error query " << i_index << ", 0..." << _v_tqueries.size() << endl;
        return NULL;
    }

    return &(_v_tqueries.at(i_index));
}

vector<double> TinyDBWrapper::TQueries::get_current_score_vector()
{
    vector<double> v_score, v_temp;
    int i_running_count=0;
    for (unsigned int i=0; i<get_query_count(); i++)
    {
        if (get_query(i)->get_state() == qs_running)
        {
            if (i_running_count == 0)
            {
                v_score = get_query(i)->get_current_score_vector();
            }
            else
            {
                v_temp = get_query(i)->get_current_score_vector();
                for (unsigned int j=0; j<v_score.size(); j++)
                {
                    v_score.at(j) = v_score.at(j) + v_temp.at(j);
                }
                v_temp.erase(v_temp.begin(), v_temp.end());
            }
            i_running_count++;
        }
    }

    if (i_running_count != 0)
    {
        for (unsigned int j=0; j<v_score.size(); j++)
        {
            v_score.at(j) = v_score.at(j) / i_running_count;
        }
    }
    return v_score;
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
string TinyDBWrapper::TQueries::get_xml()
{
    string str_xml;
    for (unsigned int i; i<_v_tqueries.size(); i++)
    {
        str_xml.append(_v_tqueries.at(i).get_xml());
    }
    return str_xml;
}

TinyDBWrapper::TQuery* TinyDBWrapper::TQueries::add_query(TQuery& r_tquery)
{
    _v_tqueries.insert(_v_tqueries.end(), r_tquery);
    return &(_v_tqueries.back());
}
