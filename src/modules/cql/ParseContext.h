/**
 * ParseContext.h
 *
 *  Created on: Jun 21, 2012
 *      Author: hellojinjie
 */

#ifndef PARSECONTEXT_H_
#define PARSECONTEXT_H_

#include "common.h"

BOREALIS_NAMESPACE_BEGIN

class ParseContext
{
private:
    string originCQL;
    typedef int SelectClause;
    typedef int FromClause;
    typedef int WhereClause;
    typedef int GroupByClause;
    typedef int HavingClause;

    typedef int ProjTerm;
    typedef int ArithExpr;

public:
    ParseContext();
    virtual ~ParseContext();
};


BOREALIS_NAMESPACE_END

#endif /* PARSECONTEXT_H_ */
