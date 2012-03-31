/*
 * DeployDescript.h
 *
 *  Created on: Mar 30, 2012
 *      Author: hellojinjie
 */

#ifndef DEPLOYDESCRIPT_H_
#define DEPLOYDESCRIPT_H_

#include <string>
#include <map>
#include <list>

typedef struct DeployDescriptStruct
{
    std::string deployName;

    /* std::map<schemaName, std::map<fieldName, fieldType> > schemas */
    std::map<std::string, std::map<std::string, std::string> > schemas;

    /* std::map<streamName, schema> streams */
    std::map<std::string, std::string> streams;

    /* std::map<queryName, std::map<parament, value> > querys */
    std::map<std::string, std::map<std::string, std::string> > querys;

} DeployDescript;

#endif /* DEPLOYDESCRIPT_H_ */

