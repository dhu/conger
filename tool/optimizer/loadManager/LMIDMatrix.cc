#include "LMIDMatrix.h"

BOREALIS_NAMESPACE_BEGIN

void LMIDMatrix::add_new_entry(const IdType& id)
{
    insert(value_type(id, vector<double>(_d, 0.0)));
}

void LMIDMatrix::get_row_sum(vector<double>& row_sum) const
{
    row_sum = vector<double>(_d, 0.0);
    for (const_iterator it = begin(); it != end(); ++it)
    {
        transform(
                it->second.begin(), 
                it->second.end(),
                row_sum.begin(),
                row_sum.begin(),
                plus<double>());
    }

}

void LMIDMatrix::normalize()
{
    vector<double> v(_d, 0.0);
    for (iterator it = begin(); it != end(); ++it)
    {
        transform(
                it->second.begin(), 
                it->second.end(),
                v.begin(),
                v.begin(),
                plus<double>());
    }
    for (iterator it = begin(); it != end(); ++it)
    {
        transform(
                it->second.begin(), 
                it->second.end(),
                v.begin(),
                it->second.begin(),
                divides<double>());
    }
}

void LMIDMatrix::add(const vector<double>& v)
{
    for (iterator it = begin(); it != end(); ++it)
    {
        transform(
                it->second.begin(), 
                it->second.end(),
                v.begin(),
                it->second.begin(),
                plus<double>());
    }
}

void LMIDMatrix::add(const IdType& id, const vector<double>& v)
{
    iterator it = find(id);
    if (it == end())
    {
        NOTICE << id << " is not found";
        return;
    }
    transform(
            it->second.begin(), 
            it->second.end(),
            v.begin(),
            it->second.begin(),
            plus<double>());
    
}

void LMIDMatrix::remove_unrelated_columns(const vector<double>& v)
{
    for (iterator it = begin(); it != end(); ++it)
    {
        for (unsigned int i = 0; i < v.size(); ++i) 
        {
            if (v[i] == 0)
            {
                it->second[i] = 0;
            }
        }
    }
}

ostream& operator<<(ostream& os, const LMIDMatrix& id_matrix)
{
    for (LMIDMatrix::const_iterator it = id_matrix.begin(); 
            it != id_matrix.end(); 
            ++it)
    {
        os << "(" << it->first << ") ";
        for (unsigned int i = 0; i < id_matrix._d; ++i) 
        {
            os << setw(5) << it->second[i] << " ";
        }
        os << endl;
    }
    return os;
}

BOREALIS_NAMESPACE_END
