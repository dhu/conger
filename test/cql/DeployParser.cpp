/*
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

using namespace std;
using namespace xercesc;

DeployParser::DeployParser()
{
    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch)
    {
        char* message = XMLString::transcode(toCatch.getMessage());
        cout << "Failed to initialize xml: " << message <<  endl;
        XMLString::release(&message);
        exit(1);
    }
}

DeployParser::~DeployParser()
{
    XMLPlatformUtils::Terminate();
}

DeployDescript DeployParser::parse(string deployFilename)
{


     XercesDOMParser* parser = new XercesDOMParser();

     try
     {
         parser->parse(deployFilename.c_str());
     }
     catch (...)
     {
         cout << "I catch something" << endl;
     }

     DOMDocument *document = parser->getDocument();
     DOMElement *element = document->getDocumentElement();

     const XMLCh* tagNameXML = element->getTagName();
     char *tagName = XMLString::transcode(tagNameXML);
     cout << tagName << endl;
     XMLString::release(&tagName);

     XMLCh* attributeName = XMLString::transcode("xmlns");
     const XMLCh* attributeXML = element->getAttribute(attributeName);
     char *attribute = XMLString::transcode(attributeXML);
     cout << attribute << endl;
     XMLString::release(&attribute);
     XMLString::release(&attributeName);

     delete parser;
     return 0;

}

void DeployParser::parseDeployName(xercesc::DOMDocument& deployNameDOM)
{

}

void DeployParser::parseSchemas(xercesc::DOMDocument& schemasDOM)
{

}

void DeployParser::parseStreams(xercesc::DOMDocument& streamsDOM)
{

}

void DeployParser::parseQuerys(xercesc::DOMDocument& querysDOM)
{

}
