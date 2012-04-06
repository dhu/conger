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
#include <xercesc/dom/DOM.hpp>

class DeployParser
{
public:
	DeployParser();
	virtual ~DeployParser();

	DeployDescript parse(std::string deployFilename);

private:

	DeployDescript deploy;

	void parseDeployName(xercesc::DOMNode* deployNameDOM);
	void parseSchemas(xercesc::DOMNode* schemasDOM);
	void parseStreams(xercesc::DOMNode* streamsDOM);
	void parseQuerys(xercesc::DOMNode* querysDOM);

	std::string getChildText(xercesc::DOMNode* node);
};

#endif /* DEPLOYPARSER_H_ */
