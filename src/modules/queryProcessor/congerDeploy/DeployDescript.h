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

typedef struct SchemaFieldTypeStruct
{
    std::string fieldName;
    std::string typeName;
    int size;
    SchemaFieldTypeStruct()
    {
        size = 0;
    }
} SchemaFieldType;

typedef struct DeployDescriptStruct
{
    std::string deployName;

    /* std::map<schemaName, SchemaFieldType> schemas */
    std::map<std::string, std::list<SchemaFieldType> > schemas;

    /* std::map<streamName, schema> streams */
    std::map<std::string, std::string> inputStreams;
    std::map<std::string, std::string> outputStreams;

    /* std::pair<queryName, std::map<parameter, value> > query
     * 如果有多个输入流，则多个输入流用冒号隔开,例: <in, stream1:stream2>
     */
    std::list<std::pair<std::string, std::map<std::string, std::string> > > querys;

} DeployDescript;


#endif /* DEPLOYDESCRIPT_H_ */
