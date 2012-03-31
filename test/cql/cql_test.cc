#include "cql_test.h"
#include <iostream>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

using namespace std;
using namespace xercesc;

int main()
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
    
    XercesDOMParser* parser = new XercesDOMParser();

    char* cqlXmlFilename = "cql_test.xml";
    try 
    {
        parser->parse(cqlXmlFilename);
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
