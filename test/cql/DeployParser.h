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

	void parseDeployName(xercesc::DOMDocument& deployNameDOM);
	void parseSchemas(xercesc::DOMDocument& schemasDOM);
	void parseStreams(xercesc::DOMDocument& streamsDOM);
	void parseQuerys(xercesc::DOMDocument& querysDOM);
};

#endif /* DEPLOYPARSER_H_ */
