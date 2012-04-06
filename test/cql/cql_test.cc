#include "cql_test.h"
#include <iostream>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "DeployParser.h"
#include "DeployDescript.h"

using namespace std;
using namespace xercesc;

int main()
{
    DeployParser parser;
    DeployDescript deploy = parser.parse("cql_test.xml");

    return 0;
}
