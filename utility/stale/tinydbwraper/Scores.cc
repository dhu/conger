#include "Scores.h"
#include "LifetimeFunction.h"
#include "LatencyFunction.h"
#include "ThroughputFunction.h"
#include "QualityFunction.h"

TinyDBWrapper::Scores::Scores()
{
    _v_scores.erase(_v_scores.begin(), _v_scores.end());
    _v_metrics.erase(_v_metrics.begin(), _v_metrics.end());
}

TinyDBWrapper::Scores::Scores(const Scores& r_scores)
{
    _v_scores.erase(_v_scores.begin(), _v_scores.end());
    _v_scores.assign(r_scores._v_scores.begin(), r_scores._v_scores.end());
    _v_metrics.erase(_v_metrics.begin(), _v_metrics.end());
    _v_metrics.assign(r_scores._v_metrics.begin(), r_scores._v_metrics.end());
    assign_metrics();
    assign_score_functions();
}

TinyDBWrapper::Scores& TinyDBWrapper::Scores::operator=(const Scores& r_scores)
{
    _v_scores.erase(_v_scores.begin(), _v_scores.end());
    _v_scores.assign(r_scores._v_scores.begin(), r_scores._v_scores.end());
    _v_metrics.erase(_v_metrics.begin(), _v_metrics.end());
    _v_metrics.assign(r_scores._v_metrics.begin(), r_scores._v_metrics.end());
    assign_metrics();
    assign_score_functions();
    return *this;
}

unsigned int TinyDBWrapper::Scores::get_score_count()
{
    return _v_scores.size();
}

TinyDBWrapper::Score* TinyDBWrapper::Scores::get_score(unsigned int i_index)
{
    if (i_index < 0 || i_index > _v_scores.size())
    {
        cerr << "access error score " << i_index << ", 0..." << _v_scores.size() << endl;
        return NULL;
    }

    return &(_v_scores.at(i_index));
}

unsigned int TinyDBWrapper::Scores::get_metric_count()
{
    return _v_metrics.size();
}

TinyDBWrapper::Metric* TinyDBWrapper::Scores::get_metric(unsigned int i_index)
{
    if (i_index < 0 || i_index > _v_metrics.size())
    {
        cerr << "access error metric " << i_index << ", 0..." << _v_metrics.size() << endl;
        return NULL;
    }

    return &(_v_metrics.at(i_index));
}

vector<double> TinyDBWrapper::Scores::get_score_weight_vector()
{
    vector<double> v_weights;

    for (unsigned int i=0; i<_v_scores.size(); i++)
    {
        v_weights.insert(v_weights.end(), _v_scores.at(i).get_weight());
    }

    return v_weights;
}

vector<double> TinyDBWrapper::Scores::get_estimate_score_vector()
{
    vector<double> v_estimate_scores;

    for (unsigned int i=0; i<_v_scores.size(); i++)
    {
      //vEstimateScores.insert(vEstimateScores.end(), m_vScores.at(i).GetEstimateValue());
      //HACK: FOR NOW TP always estimate
      v_estimate_scores.insert(v_estimate_scores.end(), get_metric(7)->get_current_value());
    }

    return v_estimate_scores;
}

vector<double> TinyDBWrapper::Scores::get_current_score_vector()
{
    vector<double> v_current_scores;

    for (unsigned int i=0; i<_v_scores.size(); i++)
    {
        v_current_scores.insert(v_current_scores.end(), _v_scores.at(i).get_current_value());
    }

    return v_current_scores;
}

/*
<scores>
    <score name="s1" weight="0.75">
        <metric name="m1" estimatevalue="0.75"/>
        <metric name="m2" estimatevalue="3"/>
        <function value="(m1+m2)*0.1"/>
        <softconstraint value="0.3"/>
        <hardconstraint value="0.3"/>
    </score>
</scores>
*/
string TinyDBWrapper::Scores::get_xml()
{
    string str_xml = "<scores>\n\t";
    for (unsigned int i=0; i<_v_scores.size(); i++)
    {
        string str_temp = _v_scores.at(i).get_xml();
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
    str_xml.append("</scores>\n");

    return str_xml;
}

/*
 initializes the score functions and metrics, hard coded
 metrics in the order they are used:
     transmissions left, total transmissions per second, transmissions per second and query, average latency per tuple, total sent tuples, total rerceived tuples, join selectivity
 score functions:
     lifetime, latency, throughput, quality
*/
bool TinyDBWrapper::Scores::init_scores()
{
    ////////////////////////////////////////////////////////////
    // metrics
    ////////////////////////////////////////////////////////////
    // epoch
    Metric _transmissions_left;
    _transmissions_left.set_name("transmissions_left");
    _transmissions_left.set_estimate_value(ESTIMATE_TRANS_LEFT);
    _transmissions_left.set_current_value(ESTIMATE_TRANS_LEFT);
    // transmitted tuples per epoch
    Metric _transmissions;
    _transmissions.set_name("transmissions");
    _transmissions.set_estimate_value(ESTIMATE_TRANSMISSIONS);
    _transmissions.set_current_value(0);
    // transmitted tuples per epoch
    Metric _transmissions_query;
    _transmissions_query.set_name("transmissions_query");
    _transmissions_query.set_estimate_value(ESTIMATE_TRANS_QUERY);
    _transmissions_query.set_current_value(0);
    // average latency per tuple
    Metric _latency;
    _latency.set_name("latency");
    _latency.set_estimate_value(ESTIMATE_LATENCY);
    _latency.set_current_value(0);
    // received tuples per second
    Metric _throughput;
    _throughput.set_name("throughput");
    _throughput.set_estimate_value(ESTIMATE_THROUGHPUT);
    _throughput.set_current_value(0);
    // total sent tuples
    Metric _sent;
    _sent.set_name("sent");
    _sent.set_estimate_value(ESTIMATE_SENT);
    _sent.set_current_value(0);
    // total rerceived tuples
    Metric _received;
    _received.set_name("received");
    _received.set_estimate_value(ESTIMATE_RECEIVED);
    _received.set_current_value(0);
    // join selectivity
    Metric _join_sel;
    _join_sel.set_name("join_selectivity");
    _join_sel.set_estimate_value(ESTIMATE_JOINSEL);
    _join_sel.set_current_value(0);
    // the order is important
    add_metric(_transmissions_left);
    add_metric(_transmissions);
    add_metric(_transmissions_query);
    add_metric(_latency);
    add_metric(_throughput);
    add_metric(_sent);
    add_metric(_received);
    add_metric(_join_sel);
    ////////////////////////////////////////////////////////////
    // scores
    ////////////////////////////////////////////////////////////
    // lifetime
    Score s_lifetime;
    s_lifetime.set_name("Lifetime");
    s_lifetime.set_weight(WEIGHT_LIFETIME);
    // latency
    Score s_latency;
    s_latency.set_name("Latency");
    s_latency.set_weight(WEIGHT_LATENCY);
    // throughput
    Score s_throughput;
    s_throughput.set_name("Throughput");
    s_throughput.set_weight(WEIGHT_THROUGHPUT);
    // quality
    Score s_quality;
    s_quality.set_name("Quality");
    s_quality.set_weight(WEIGHT_QUALITY);
    // the order is important
    add_score(s_lifetime);
    add_score(s_latency);
    add_score(s_throughput);
    add_score(s_quality);

    if (!assign_metrics())
    {
        return false;
    }
    return assign_score_functions();
}

/*
    assigns the metrics to the score functions, hard coded
*/
bool TinyDBWrapper::Scores::assign_metrics()
{
    // expecting exact order and number of scores and metrics
    if (get_score_count() != 4)
    {
        cerr << "Error: Scores::AssignMetrics: Number of scores mismatch" << endl;
        return false;
    }
    if (get_metric_count() != 8)
    {
        cerr << "Error: Number of metrics mismatch" << endl;
        return false;
    }
    // lifetime, uses: transmissions_left, transmissions
    if (get_score(0)->get_name().compare("Lifetime") != 0)
    {
        cerr << "Error: Unexpected Score 1" << endl;
        return false;
    }
    if (get_metric(0)->get_name().compare("transmissions_left") != 0
        || get_metric(1)->get_name().compare("transmissions") != 0)
    {
        cerr << "Error: Unexpected Metric 1 and/or 2" << endl;
        return false;
    }
    get_score(0)->clear_metrics();
    get_score(0)->add_metric(get_metric(0));
    get_score(0)->add_metric(get_metric(1));
    // latency uses latency
    if (get_score(1)->get_name().compare("Latency") != 0)
    {
        cerr << "Error: Unexpected Score 2" << endl;
        return false;
    }
    if (get_metric(3)->get_name().compare("latency") != 0)
    {
        cerr << "Error: Unexpected Metric 4" << endl;
        return false;
    }
    get_score(1)->clear_metrics();
    get_score(1)->add_metric(get_metric(3));
    // throughput uses throughput
    if (get_score(2)->get_name().compare("Throughput") != 0)
    {
        cerr << "Error: Unexpected Score 3" << endl;
        return false;
    }
    if (get_metric(4)->get_name().compare("throughput") != 0)
    {
        cerr << "Error: Unexpected Metric 5" << endl;
        return false;
    }
    get_score(2)->clear_metrics();
    get_score(2)->add_metric(get_metric(4));
    // quality uses sent and received
    if (get_score(3)->get_name().compare("Quality") != 0)
    {
        cerr << "Error: Unexpected Score 4" << endl;
        return false;
    }
    if (get_metric(5)->get_name().compare("sent") != 0
        || get_metric(6)->get_name().compare("received") != 0 )
    {
        cerr << "Error: Unexpected Metric 6 and/or 7" << endl;
        return false;
    }
    get_score(3)->clear_metrics();
    get_score(3)->add_metric(get_metric(5));
    get_score(3)->add_metric(get_metric(6));
    return true;
}

bool TinyDBWrapper::Scores::assign_score_functions()
{
    // expecting exact order and number of scores
    if (get_score_count() != 4)
    {
        cerr << "Error: Scores::AssignScoreFunctions:: Number of scores mismatch" << endl;
        return false;
    }
    // lifetime
    if (get_score(0)->get_name().compare("Lifetime") != 0)
    {
        cerr << "Error: Unexpected Score 1" << endl;
        return false;
    }
    get_score(0)->set_score_function(&_lifetime_function);
    // latency
    if (get_score(1)->get_name().compare("Latency") != 0)
    {
        cerr << "Error: Unexpected Score 2" << endl;
        return false;
    }
    get_score(1)->set_score_function(&_latency_function);
    // throughput
    if (get_score(2)->get_name().compare("Throughput") != 0)
    {
        cerr << "Error: Unexpected Score 3" << endl;
        return false;
    }
    get_score(2)->set_score_function(&_throughput_function);
    // quality
    if (get_score(3)->get_name().compare("Quality") != 0)
    {
        cerr << "Error: Unexpected Score 4" << endl;
        return false;
    }
    get_score(3)->set_score_function(&_quality_function);
    return true;
}

void TinyDBWrapper::Scores::add_score(Score& r_score)
{
    _v_scores.insert(_v_scores.end(), r_score);
}

void TinyDBWrapper::Scores::add_metric(Metric& r_metric)
{
    _v_metrics.insert(_v_metrics.end(), r_metric);
}
