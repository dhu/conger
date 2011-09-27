#ifndef SCORE_H
#define SCORE_H

#include "WrapperConfig.h"
#include <string>
#include <vector>
#include "Metric.h"
#include "Constraint.h"
#include "ScoreFunction.h"

namespace tiny_dbwrapper
{
class Score
{

    friend class Scores;

public:
    Score();
    Score(string str_name, double d_weight, double d_estimate_value, vector<Metric*>& rvp_metrics, Constraint& r_constraint, ScoreFunction* p_score_function);
    Score(const Score& r_score);

    Score& operator=(const Score& r_score);

    string          get_name();
    double          get_weight();
    double          get_estimate_value();
    double          get_current_value();
    unsigned int    get_metric_count();
    Metric*         get_metric(unsigned int i_index);
    Constraint*     get_constraint();
    double          get_soft_constraint();
    double          get_hard_constraint();
    string          get_xml();

protected:
    void            set_name(string str_name);
    void            set_weight(double d_weight);
    void            add_metric(Metric* p_metric);
    void            set_constraint(Constraint& r_constraint);
    void            set_score_function(ScoreFunction* p_score_function);
    void            clear_metrics();

private:
    string          _str_name;
    double          _d_weight;
    vector<Metric*> _vp_metrics;
    Constraint      _constraint;
    ScoreFunction*  _p_score_function;

};

}
#endif //SCORE_H
