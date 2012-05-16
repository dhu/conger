/**
 * DeployParser.cpp
 *
 *  Created on: Mar 30, 2012
 *      Author: hellojinjie
 */

#include "DeployParser.h"
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <NMSTL/debug>
#include <NMSTL/ptr>
#include <iostream>


BOREALIS_NAMESPACE_BEGIN
using namespace xercesc;

/**
 * XXX Xercesc 的内存管理真不懂。。这里的内存释放操作都没有做。
 */
DeployParser::DeployParser()
{
    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch)
    {
        char* message = XMLString::transcode(toCatch.getMessage());
        DEBUG << "Failed to initialize xml: " << message;
        XMLString::release(&message);
        exit(1);
    }
}

DeployParser::~DeployParser()
{
    XMLPlatformUtils::Terminate();
}

DeployDescript DeployParser::parse(string conger_string)
{
     XercesDOMParser* parser = new XercesDOMParser();
     try
     {
         MemBufInputSource input_source(
                 (XMLByte*) conger_string.c_str(),
                 strlen(conger_string.c_str()),
                 "conger_deploy_string"
         );
         parser->parse(input_source);
     }
     catch (...)
     {
         DEBUG << "I catch something";
         return this->deploy;
     }

     DOMDocument *document = parser->getDocument();

     /* 解析部署名称 */
     XMLCh* deployNameTagName = XMLString::transcode("deploy-name");
     DOMNodeList* deployNameNodeList = document->getElementsByTagName(deployNameTagName);

     if (deployNameNodeList->getLength() <= 0)
     {
         this->deploy.deployName = "Unspecified Deploy Name";
     }
     else
     {
         this->parseDeployName(deployNameNodeList->item(0));
     }

     DEBUG << this->deploy.deployName;

     XMLCh* schemasTagName = XMLString::transcode("schemas");
     DOMNodeList* schemasNodeList = document->getElementsByTagName(schemasTagName);
     /* 如果有好几个 schemas tag 的话，也要解析 */
     for (int i = 0; i < (int) schemasNodeList->getLength(); i++)
     {
         this->parseSchemas(schemasNodeList->item(i));
     }

     XMLCh* streamsTagName = XMLString::transcode("streams");
     DOMNodeList* streamsNodeList = document->getElementsByTagName(streamsTagName);
     for (int i = 0; i < (int) streamsNodeList->getLength(); i++)
     {
         this->parseStreams(streamsNodeList->item(i));
     }

     XMLCh* querysTagName = XMLString::transcode("querys");
     DOMNodeList* querysNodeList = document->getElementsByTagName(querysTagName);
     for (int i = 0; i < (int) querysNodeList->getLength(); i++)
     {
         this->parseQuerys(querysNodeList->item(i));
     }

     delete parser;
     return this->deploy;
}

void DeployParser::parseDeployName(xercesc::DOMNode* deployNameDOM)
{
    DOMNode* node = deployNameDOM->getFirstChild();
    const XMLCh* deployNameXML = node->getNodeValue();
    this->deploy.deployName = XMLString::transcode(deployNameXML);
}

/* 解析 <schemas></schemas> */
void DeployParser::parseSchemas(xercesc::DOMNode* schemasDOM)
{
    DOMNodeList* nodeList = schemasDOM->getChildNodes();
    for (int i = 0; i < (int) nodeList->getLength(); i++)
    {
        /* 解析 <schema></schema> */
        DOMNode* schemaNode = nodeList->item(i);
        if (schemaNode->getNodeType() != DOMNode::ELEMENT_NODE)
        {
            continue;
        }
        DOMNamedNodeMap* attributes = schemaNode->getAttributes();
        const XMLCh* schemaNameXML = ((DOMAttr*) (attributes->item(0)))
                ->getValue();
        string schemaName = XMLString::transcode(schemaNameXML);
        DOMNodeList* fieldNodeList = schemaNode->getChildNodes();
        list<SchemaFieldType> schemaList;
        for (int j = 0; j < (int) fieldNodeList->getLength(); j++)
        {
            if (fieldNodeList->item(j)->getNodeType() != DOMNode::ELEMENT_NODE)
            {
                continue;
            }
            string name;
            string type;
            int size = 0;
            DOMNamedNodeMap* fieldAttributes = fieldNodeList->item(j)->getAttributes();
            const XMLCh* nameXML = fieldAttributes
                    ->getNamedItem(XMLString::transcode("name"))->getNodeValue();
            const XMLCh* typeXML = fieldAttributes
                    ->getNamedItem(XMLString::transcode("type"))->getNodeValue();
            const XMLCh* sizeXML = fieldAttributes
                    ->getNamedItem(XMLString::transcode("size"))->getNodeValue();
            if (sizeXML != NULL)
            {
                size = atoi(XMLString::transcode(sizeXML));
            }
            SchemaFieldType schema_field;
            schema_field.fieldName = string(XMLString::transcode(nameXML));
            schema_field.typeName = string(XMLString::transcode(typeXML));
            schema_field.size = size;
            schemaList.push_back(schema_field);
        }
        DEBUG << "schema name: " << schemaName;
        for (list<SchemaFieldType>::iterator iterator =  schemaList.begin();
                iterator != schemaList.end(); iterator++)
        {
            DEBUG << iterator->fieldName << " " << iterator->typeName
                    << " " << iterator->size;
        }
        this->deploy.schemas[schemaName] = schemaList;
    }
}

void DeployParser::parseStreams(xercesc::DOMNode* streamsDOM)
{
    DOMNodeList* schemaNodeList = streamsDOM->getChildNodes();
    for (int i = 0; i < (int) schemaNodeList->getLength(); i++)
    {
        if (schemaNodeList->item(i)->getNodeType() != DOMNode::ELEMENT_NODE)
        {
            continue;
        }
        DOMNamedNodeMap* streamAttributes = schemaNodeList->item(i)->getAttributes();
        const XMLCh* nameXML = streamAttributes
                ->getNamedItem(XMLString::transcode("name"))->getNodeValue();
        const XMLCh* typeXML = streamAttributes
                ->getNamedItem(XMLString::transcode("type"))->getNodeValue();
        const XMLCh* schemaXML = streamAttributes
                ->getNamedItem(XMLString::transcode("schema"))->getNodeValue();
        string name = string(XMLString::transcode(nameXML));
        string type = string(XMLString::transcode(typeXML));
        string schema = string(XMLString::transcode(schemaXML));
        if (type == "input")
        {
            if (this->deploy.inputStreams.find(name) == this->deploy.inputStreams.end())
            {
                this->deploy.inputStreams[name] = schema;
            }
        }
        else if (type == "output")
        {
            if (this->deploy.outputStreams.find(name) == this->deploy.outputStreams.end())
            {
                this->deploy.outputStreams[name] = schema;
            }
        }

        DEBUG << "stream name: " << name << " schema " << schema << " type " << type;
    }
}

void DeployParser::parseQuerys(xercesc::DOMNode* querysDOM)
{
    DOMNodeList* queryNodeList = querysDOM->getChildNodes();
    for (int i = 0; i < (int) queryNodeList->getLength(); i++)
    {
        DOMNode* queryNode = queryNodeList->item(i);
        if (queryNode->getNodeType() != DOMNode::ELEMENT_NODE)
        {
            continue;
        }
        map<string, string> query;
        DOMNamedNodeMap* queryAttributes = queryNode->getAttributes();
        string queryName = string(XMLString::transcode(
                queryAttributes->getNamedItem(XMLString::transcode("name"))->getNodeValue()
        ));
        DOMNodeList* queryNodeChildList = queryNode->getChildNodes();
        for (unsigned int j = 0; j < queryNodeChildList->getLength(); j++)
        {
            DOMNode* node = queryNodeChildList->item(j);
            if (node->getNodeType() != DOMNode::ELEMENT_NODE)
            {
                continue;
            }
            const XMLCh* nodeNameXML = node->getNodeName();
            string nodeName = string(XMLString::transcode(nodeNameXML));
            DEBUG << nodeName;
            if (nodeName == "in")
            {
                if (query[string("in")] == "")
                {
                    query[string("in")] = this->getChildText(node);
                }
                else
                {
                    query[string("in")] = query[string("in")] + ":"
                            + this->getChildText(node);
                }
            }
            else if (string(XMLString::transcode(nodeNameXML)) == "out")
            {
                query[string("out")] = this->getChildText(node);
            }
            else if (string(XMLString::transcode(nodeNameXML)) == "cql")
            {
                query[string("cql")] = this->getChildText(node);
            }
        }
        this->deploy.querys[queryName] = query;
        DEBUG << "query name: " << queryName;
        for (map<string, string>::iterator iterator =  query.begin();
                iterator != query.end(); iterator++)
        {
            DEBUG << iterator->first << ": " << iterator->second;
        }
    }
}


string DeployParser::getChildText(DOMNode* node)
{
    string text;
    DOMNodeList* childNodes = node->getChildNodes();
    for (unsigned int i = 0; i < childNodes->getLength(); i++)
    {
        DOMNode* child = childNodes->item(i);
        if (child->getNodeType() == DOMNode::TEXT_NODE)
        {
            try
            {
                text = text + XMLString::transcode(((DOMText*) child)->getData());
            }
            catch (DOMException &e)
            {
                DEBUG << "DOMText:" << XMLString::transcode(e.getMessage());
            }
        }
        else if (child->getNodeType() == DOMNode::CDATA_SECTION_NODE)
        {
            try
            {
                string s = XMLString::transcode(((DOMCDATASection*) child)->getData());
                text = text + s;
            }
            catch (DOMException &e)
            {
                DEBUG << "DOMCDATASection:"  << XMLString::transcode(e.getMessage());
            }
        }
    }
    string delimiters = " \f\n\r\t\v";
    text.erase( text.find_last_not_of( delimiters ) + 1 );
    text.erase( 0, text.find_first_not_of( delimiters ) );
    return text;
}

BOREALIS_NAMESPACE_END
