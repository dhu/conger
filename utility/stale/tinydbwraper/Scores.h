#ifndef SCORES_H
#define SCORES_H

#include "WrapperConfig.h"
#include <string>
#include <vector>
#include "Score.h"
#include "ScoreFunction.h"
#include "Metric.h"
#include "LifetimeFunction.h"
#include "LatencyFunction.h"
#include "ThroughputFunction.h"
#include "QualityFunction.h"

namespace tiny_dbwrapper
{
class Scores
{

    friend class TQuery;

public:
    Scores();
    Scores(const Scores& r_scores);

    Scores& operator=(const Scores& r_scores);

    unsigned int    get_score_count();
    Score*          get_score(unsigned int i_index);
    unsigned int    get_metric_count();
    Metric*         get_metric(unsigned int i_index);
    vector<double>  get_score_weight_vector();
    vector<double>  get_estimate_score_vector();
    vector<double>  get_current_score_vector();
    string          get_xml();

protected:
    bool            init_scores();
    bool            assign_metrics();
    bool            assign_score_functions();
    void            add_score(Score& r_score);
    void            add_metric(Metric& r_metric);
    void            add_score_function(ScoreFunction& r_score_function);

private:
    vector<Score>           _v_scores;
    vector<Metric>          _v_metrics;
    LifetimeFunction        _lifetime_function;
    LatencyFunction         _latency_function;
    ThroughputFunction      _throughput_function;
    QualityFunction         _quality_function;

};

}
#endif //SCORES_H
