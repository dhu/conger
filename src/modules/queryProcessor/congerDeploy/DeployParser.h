/*
 * DeployParser.h
 *
 *  Created on: Mar 30, 2012
 *      Author: hellojinjie
 */

#ifndef DEPLOYPARSER_H_
#define DEPLOYPARSER_H_

#include "DeployDescript.h"
#include <string>

#include "common.h"


BOREALIS_NAMESPACE_BEGIN

class DeployParser
{
public:
	DeployParser();
	virtual ~DeployParser();

	DeployDescript parse(std::string conger_string);

private:

	DeployDescript deploy;

};

BOREALIS_NAMESPACE_END

#endif /* DEPLOYPARSER_H_ */
