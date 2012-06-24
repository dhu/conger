/**
 * ParseDriver.h
 *
 *  Created on: Jun 23, 2012
 *      Author: hellojinjie
 */

#ifndef PARSEDRIVER_H_
#define PARSEDRIVER_H_

#include "common.h"
#include "ParseContext.h"

BOREALIS_NAMESPACE_BEGIN
class ParseDriver
{
private:
    ParseContext context;

public:
    ParseDriver();
    virtual ~ParseDriver();
    void parse();

};

BOREALIS_NAMESPACE_END
#endif /* PARSEDRIVER_H_ */
