#ifndef TQUERIES_H
#define TQUERIES_H

#include "WrapperConfig.h"
#include <string>
#include <vector>
#include "TQuery.h"

namespace tiny_dbwrapper
{
class TQueries
{

    friend class Wrapper;

public:
    TQueries();
    TQueries(vector<TQuery>& rv_tqueries);
    TQueries(const TQueries& r_tqueries);

    TQueries& operator=(const TQueries& r_tqueries);

    unsigned int    get_query_count();
    TQuery*         get_query(unsigned int i_index);
    vector<double>  get_current_score_vector();
    string          get_xml();

protected:
    TQuery*         add_query(TQuery& r_tquery);

private:
    vector<TQuery>  _v_tqueries;

};
}
#endif //TQUERIES_H
