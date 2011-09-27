/*
 * @(#)LMIDMatrix.h
 */

#ifndef LMIDMatrix_H
#define LMIDMatrix_H

#include "LMCommon.h"

BOREALIS_NAMESPACE_BEGIN


/**
 * A LMIDMatrix Object is a matrix that stores load coefficients
 */
class LMIDMatrix : public map<IdType, vector<double> >
{

public:

    /**
     * Constructs a new LMIDMatrix with d dimension
     */
    LMIDMatrix(unsigned int d) : _d(d) {}

    /**
     * Returns the dimension of the matrix
     */
    unsigned int get_dimension() const { return _d; }

    /**
     * Returns the dimension of the matrix
     */
    void get_row_sum(vector<double>& row_sum) const; 

    /**
     * Add a new entry (row) to the matrix
     * @param id id of the new entry
     */
    void add_new_entry(const IdType& id); 

    /**
     * Normalize the column of the matrix to be 1
     */
    void normalize();

    /**
     * Add a vector to each row
     * @param v the vector to be add
     */
    void add(const vector<double>& v);

    /**
     * Add a vector to the row with given id
     * @param id ID of the row to add to
     * @param v the vector to be add
     */
    void add(const IdType& id, const vector<double>& v);

    /**
     * Set the columns that are not related to the given vector (the columns of
     * the given vector that are zero) to zero
     * @param v the given reverence vector
     */
    void remove_unrelated_columns(const vector<double>& v);


    /**
     * Output to ostream
     */
    friend ostream& operator<<(ostream& os, const LMIDMatrix& id_matrix);

  // Attributes
private:

    /**
     * The dimension of the matrix (number of columns)
     */
    unsigned int _d;
};


BOREALIS_NAMESPACE_END
#endif


