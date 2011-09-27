#include "Score.h"

TinyDBWrapper::Score::Score()
{
    _str_name           = "";
    _d_weight           = 0.0;
    _vp_metrics.erase(_vp_metrics.begin(), _vp_metrics.end());
    _p_score_function    = NULL;
}

TinyDBWrapper::Score::Score(string str_name, double d_weight, double d_estimate_value, vector<Metric*>& rvp_metric, Constraint& r_constraint, ScoreFunction* p_score_function)
{
    _str_name           = str_name;
    _d_weight           = d_weight;
    _vp_metrics.erase(_vp_metrics.begin(), _vp_metrics.end());
    _vp_metrics.assign(rvp_metric.begin(), rvp_metric.end());
    _constraint        = r_constraint;
    _p_score_function    = p_score_function;
}

TinyDBWrapper::Score::Score(const Score& r_score)
{
    _str_name           = r_score._str_name;
    _d_weight           = r_score._d_weight;
    _vp_metrics.erase(_vp_metrics.begin(), _vp_metrics.end());
    _vp_metrics.assign(r_score._vp_metrics.begin(), r_score._vp_metrics.end());
    _constraint        = r_score._constraint;
    _p_score_function    = r_score._p_score_function;
}

TinyDBWrapper::Score& TinyDBWrapper::Score::operator=(const Score& r_score)
{
    _str_name           = r_score._str_name;
    _d_weight           = r_score._d_weight;
    _vp_metrics.erase(_vp_metrics.begin(), _vp_metrics.end());
    _vp_metrics.assign(r_score._vp_metrics.begin(), r_score._vp_metrics.end());
    _constraint        = r_score._constraint;
    _p_score_function    = r_score._p_score_function;
    return *this;
}

string TinyDBWrapper::Score::get_name()
{
    return _str_name;
}

double TinyDBWrapper::Score::get_weight()
{
    return _d_weight;
}

double TinyDBWrapper::Score::get_estimate_value()
{
    if (_p_score_function == NULL || _vp_metrics.size() == 0)
    {
        return 0.0;
    }
    vector<double> v_values;
    for (unsigned int i=0; i<_vp_metrics.size(); i++)
    {
        v_values.insert(v_values.end(), _vp_metrics.at(i)->get_estimate_value());
    }
    return _p_score_function->calculate_score(v_values);
}

double TinyDBWrapper::Score::get_current_value()
{
    if (_p_score_function == NULL || _vp_metrics.size() == 0)
    {
        return 0.0;
    }
    vector<double> v_values;
    for (unsigned int i=0; i<_vp_metrics.size(); i++)
    {
        v_values.insert(v_values.end(), _vp_metrics.at(i)->get_current_value());
    }
    return _p_score_function->calculate_score(v_values);
}

unsigned int TinyDBWrapper::Score::get_metric_count()
{
    return _vp_metrics.size();
}

TinyDBWrapper::Metric* TinyDBWrapper::Score::get_metric(unsigned int i_index)
{
    if (i_index < 0 || i_index > _vp_metrics.size())
    {
        cerr << "access error metric " << i_index << ", 0..." << _vp_metrics.size() << endl;
        return NULL;
    }

    return _vp_metrics.at(i_index);
}

TinyDBWrapper::Constraint* TinyDBWrapper::Score::get_constraint()
{
    return &_constraint;
}

double TinyDBWrapper::Score::get_soft_constraint()
{
    return _constraint.get_soft_value();
}

double TinyDBWrapper::Score::get_hard_constraint()
{
    return _constraint.get_hard_value();
}

/*
<score name="s1" weight="0.75">
    <metric name="m1" estimatevalue="0.75"/>
    <metric name="m2" estimatevalue="3"/>
    c(m1+m2)*0.1"/>
    <constraint softvalue="0.3" hardvalue="0.1"/>
</score>
*/
string TinyDBWrapper::Score::get_xml()
{
    char pc_buffer[256];
    string str_xml = "<score name=\"" + get_name() + "\" weight=\"";
    sprintf(pc_buffer, "%f", get_weight());
    str_xml.append(pc_buffer);
    str_xml.append("\">\n");
    for (unsigned int i=0; i<_vp_metrics.size(); i++)
    {
        str_xml.append("\t");
        str_xml.append(_vp_metrics.at(i)->get_xml());
    }
    str_xml.append("\t<function value=\"");
    if (_p_score_function != NULL)
    {
        str_xml.append(_p_score_function->get_xml());
    }
    str_xml.append("\"/>\n");
    str_xml.append("\t" + _constraint.get_xml());
    str_xml.append("</score>\n");
    return str_xml;
}

void TinyDBWrapper::Score::set_name(string str_name)
{
    _str_name = str_name;
}

void TinyDBWrapper::Score::set_weight(double d_weight)
{
    _d_weight = d_weight;
}

void TinyDBWrapper::Score::add_metric(Metric* p_metric)
{
    _vp_metrics.insert(_vp_metrics.end(), p_metric);
}

void TinyDBWrapper::Score::set_constraint(Constraint& r_constraint)
{
    _constraint = r_constraint;
}

void TinyDBWrapper::Score::set_score_function(ScoreFunction* p_score_function)
{
    _p_score_function = p_score_function;
}

void TinyDBWrapper::Score::clear_metrics()
{
    _vp_metrics.erase(_vp_metrics.begin(), _vp_metrics.end());
}
