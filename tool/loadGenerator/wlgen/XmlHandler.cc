//Local headers
#include "XmlHandler.h"

#if defined(XERCES_NEW_IOSTREAMS)
#include <fstream>
#else
#include <fstream.h>
#endif


BOREALIS_NAMESPACE_BEGIN
//namespace xml_handler
//{

XmlHandler::XmlHandler(){}

XmlHandler::~XmlHandler(){}

GenXmlErrorHandler::GenXmlErrorHandler() :

fSawErrors(false)
{
}

GenXmlErrorHandler::~GenXmlErrorHandler()
{
}

bool GenXmlErrorHandler::handleError(const DOMError& domError)
{
    fSawErrors = true;
    if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
        WARN << "\nWarning at file " ;
    else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
        ERROR << "\nError at file ";
    else
        ERROR << "\nFatal Error at file ";

    ERROR <<domError.getLocation()->getURI()
        << ", line " << domError.getLocation()->getLineNumber()
        << ", char " << domError.getLocation()->getColumnNumber()
        << "\n  Message: " << domError.getMessage();

    return true;
}


void GenXmlErrorHandler::resetErrors()
{
    fSawErrors = false;
}



//-----------------------------------------------------------------------------
//read xml file
//-----------------------------------------------------------------------------

//read Xml file, change it to Document
DOMDocument* XmlHandler::readXmlFileToDoc(const char* xmlFile)
{
    //variable localeStr is used for XMLPlatformUtils
    char    localeStr[64];
    memset(localeStr, 0, sizeof localeStr);

    //Initialize the XML4C system
    try
    {
        if(strlen(localeStr))
        {
            XMLPlatformUtils::Initialize(localeStr);
        }
        else
        {
            XMLPlatformUtils::Initialize();
        }
    }
    catch(const XMLException& toCatch)
    {
        ERROR<<"Error during initialization! :\n"<< toCatch.getMessage();

        return NULL;

    }
    //Instantiate the DOM parser.
    static const XMLCh gLS[] = {chLatin_L, chLatin_S, chNull};
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);
    parser = ((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS,0);

    // enable datatype normalization - default is off
    parser->setFeature(XMLUni::fgDOMDatatypeNormalization, true);

    // And create our error handler and install it
    GenXmlErrorHandler errorHandler;
    parser->setErrorHandler(&errorHandler);

    //reset error count first
    errorHandler.resetErrors();

    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = 0;

    try
    {
        // reset document pool
        parser->resetDocumentPool();
        doc = parser->parseURI(xmlFile);
    }

    catch (const XMLException& toCatch)
    {
        ERROR << "\nError during parsing: '" << xmlFile << "'\n"
            << "Exception message is:  \n"
            << toCatch.getMessage() << "\n";

        return NULL;
    }
    catch (const DOMException& toCatch)
    {
        const unsigned int maxChars = 2047;
        XMLCh errText[maxChars + 1];

        ERROR << "\nDOM Error during parsing: '" << xmlFile << "'\n";

        if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
            ERROR << "Message is: " << errText;
        return NULL;

    }

    catch (...)
    {
        ERROR << "\nUnexpected exception during parsing: '" << xmlFile << "'\n";
        return NULL;

    }

    if (errorHandler.getSawErrors())
    {
        WARN << "\nErrors occurred, no output available\n";
        return NULL;
    }

    return doc;

    //  Delete the parser itself.  Must be done prior to calling Terminate, below.
    // parser->release();

    // And call the termination method
    // XMLPlatformUtils::Terminate();


}


//----------------------------------------------------------------------------
//Following is for parsing the query network xml file.
//----------------------------------------------------------------------------


//get all the input stream Names
vector<char*> XmlHandler::getInputStreamNames(const DOMNode *root)
{
    vector<char*> result;
    if (root) 
    {
        DOMNode *child;
        for(child = root->getFirstChild(); child!=0; child =child->getNextSibling())
        {
            if(child->getNodeType()==DOMNode::ELEMENT_NODE)
            {
                char *name = XMLString::transcode(child->getNodeName());
                if(!strcmp(name, "input"))
                {
                    if(child->hasAttributes())
                    {
                        DOMNamedNodeMap *stream_attr_nodes = child->getAttributes();
                        int size = stream_attr_nodes->getLength();
                        for(int i = 0; i<size; ++i)
                        {
                            DOMAttr *stream_attr_node = (DOMAttr*)stream_attr_nodes->item(i);
                            char *attr_name = XMLString::transcode(stream_attr_node->getName());
                            if(!strcmp(attr_name,"stream"))
                            {
                                char *attr_value = XMLString::transcode(stream_attr_node->getValue());
                                result.push_back(attr_value);
                            }
                        }

                    }
                }
            }
        }
    }
    return result;
}

//get input DOMNode by the givene stream name
DOMNode* XmlHandler::getInputNode(const DOMNode *root, const char *stream_name)
{
    DOMNode *result = NULL;
    DOMNode *child;

    if(root)
    {
        for(child = root->getFirstChild(); child!=0; child = child->getNextSibling())
        {
            if(child->getNodeType()==DOMNode::ELEMENT_NODE)
            {
                char *child_name = XMLString::transcode(child->getNodeName());
                if(!strcmp(child_name, "input"))
                {
                    if(child->hasAttributes())
                    {
                        DOMNamedNodeMap *input_attr_nodes = child->getAttributes();
                        int size = input_attr_nodes->getLength();
                        for(int i = 0; i<size; ++i)
                        {
                            DOMAttr *input_attr_node = (DOMAttr*)input_attr_nodes->item(i);
                            char *attr_name = XMLString::transcode(input_attr_node->getName());
                            if(!strcmp(attr_name,"stream"))
                            {
                                char *attr_value = XMLString::transcode(input_attr_node->getValue());
                                if(!strcmp(attr_value,stream_name))
                                {
                                    result = child;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}


//get gen_info DOMNode by the given stream name
DOMNode* XmlHandler::getGenInfoNode(const DOMNode *root, const char *stream_name)
{
    DOMNode *result = NULL;
    DOMNode *input_node = XmlHandler::getInputNode(root, stream_name);
    if(input_node)
    {
        DOMNode *child;
        for(child= input_node->getFirstChild(); child!=0; child = child->getNextSibling())
        {
            if(child->getNodeType()==DOMNode::ELEMENT_NODE)
            {
                char *child_name = XMLString::transcode(child->getNodeName());

                if(!strcmp(child_name,"gen_info"))
                {
                    result = child;
                }
            }
        }
    }
    return result;
}


//get genInfo attribute value by the givene attribute name
char* XmlHandler::getGenInfoAttrValue(const DOMNode *root, const char *stream_name, const char *attr_name)
{
    char *result = "";
    DOMNode *genInfo_node = XmlHandler::getGenInfoNode(root,stream_name);
    if(genInfo_node)
    {
        if(genInfo_node->hasAttributes()) 
        {
            DOMNamedNodeMap *genInfo_attr_nodes = genInfo_node->getAttributes();
            int size = genInfo_attr_nodes->getLength();
            for(int i = 0; i<size; ++i)
            {
                DOMAttr *genInfo_attr_node = (DOMAttr*)genInfo_attr_nodes->item(i);
                char *current_attr_name = XMLString::transcode(genInfo_attr_node->getName());

                if(!strcmp(current_attr_name,attr_name))
                {
                    char *attr_value = XMLString::transcode(genInfo_attr_node->getValue());
                    result = attr_value;
                }
            }
        }
    }
    return result;
}

//get input distribution type (int) by the given stream name
int XmlHandler::getInputDistTypeInt(const DOMNode *root, const char *stream_name)
{
    int result = -1;

    DOMNode *distribution_node = XmlHandler::getInputDistNode(root,stream_name);
    char *type_string = XmlHandler::getDistTypeString(distribution_node);

    int temp_type_int = -1;
    int &type_int = temp_type_int ;

    if(XmlHandler::changeTypeToInt(type_string, type_int))    
    {
        result = type_int;   
    }
    else
    {
        result = -1;
    }

    return result;
}


//get distribution DOMNode by the given stream name
DOMNode* XmlHandler::getInputDistNode(const DOMNode *root, const char *stream_name)
{
    DOMNode *result = NULL;
    DOMNode *input_node = XmlHandler::getInputNode(root, stream_name);
    if(input_node)
    {
        DOMNode *child;
        for(child= input_node->getFirstChild(); child!=0; child = child->getNextSibling())
        {
            if(child->getNodeType()==DOMNode::ELEMENT_NODE)
            {
                char *child_name = XMLString::transcode(child->getNodeName());
                if(!strcmp(child_name,"distribution"))
                {
                    result = child;
                }
            }
        }
    }
    return result;
}

//get distribution type (string) by the givene distribution DOMNode
char* XmlHandler::getDistTypeString(const DOMNode *distribution_node)
{
    char *result = "";
    if(distribution_node)
    {
        if(distribution_node->hasAttributes()) 
        {
            DOMNamedNodeMap *distribution_attr_nodes = distribution_node->getAttributes();
            int size = distribution_attr_nodes->getLength();
            for(int i = 0; i<size; ++i)
            {
                DOMAttr *distribution_attr_node = (DOMAttr*)distribution_attr_nodes->item(i);
                char *current_attr_name = XMLString::transcode(distribution_attr_node->getName());
                if(!strcmp(current_attr_name,"type"))
                {
                    char *attr_value = XMLString::transcode(distribution_attr_node->getValue());
                    result = attr_value;
                }
            }
        }
    }
    return result;
}


//get paramDistribution type (int) by the given stream name and index
int XmlHandler::getInputParamDistTypeInt(
    const DOMNode   *root, 
    const char      *stream_name, 
    int             index)
{
    int result = -1;

    DOMNode *distribution_node = XmlHandler::getInputDistNode(root, stream_name);
    DOMNode *input_paramDist_node = XmlHandler::getParamDistNode(distribution_node,index);

    char *type_string = XmlHandler::getParamDistTypeString(input_paramDist_node);

    int temp_type_int = -1;
    int &type_int = temp_type_int;

    if(XmlHandler::changeTypeToInt(type_string, type_int))    
    {
        result = type_int;   
    }
    else
    {
        result = -1;
    }

    return result;
}


//get paramDistribution DOMNode by the given distribution DOMNode and index
DOMNode* XmlHandler::getParamDistNode(const DOMNode *distribution_node, int index)
{
    DOMNode *result = NULL;
    if(distribution_node)
    {
        int current_index = 0;
        DOMNode *child = distribution_node->getFirstChild(); 

        while((current_index < index && child!=0)||child->getNodeType()!=DOMNode::ELEMENT_NODE)
        {
            if(child->getNodeType()==DOMNode::ELEMENT_NODE)
            {
                current_index = current_index + 1;
            }
            child = child->getNextSibling();
        }
        result = child;
    }

    return result;
}

//get paramDistribution type (string) by the given paramDistribuiton DOMNode
char* XmlHandler::getParamDistTypeString(const DOMNode *paramDist_node)
{
    char *result = "";

    if(paramDist_node)
    {
        if(paramDist_node->hasAttributes())
        {
            DOMNamedNodeMap *paramDist_attr_nodes = paramDist_node->getAttributes();
            int size = paramDist_attr_nodes->getLength();
            for(int i = 0; i<size; i++)
            {
                DOMAttr *paramDist_attr_node = (DOMAttr*)paramDist_attr_nodes->item(i);
                char *attr_name = XMLString::transcode(paramDist_attr_node->getName());
                if(!strcmp(attr_name,"type"))
                {
                    result =XMLString::transcode(paramDist_attr_node->getValue());
                }
            }
        }
    }

    return result;
}


//get parameter by the given stream name, paramDistribution, and index  
double XmlHandler::getInputParamDistParamValue(
    const DOMNode       *root, 
    const char          *stream_name,
    int                 paramDist_index,
    int                 param_index)
{
    DOMNode *dist_node = XmlHandler::getInputDistNode(root, stream_name);
    DOMNode *paramDist_node = XmlHandler::getParamDistNode(dist_node, paramDist_index);
    double value = XmlHandler::getParamValue(paramDist_node, param_index);
    return value;
}



//get parameter by the given paramDistribution DOMNode and index
double XmlHandler::getParamValue(const DOMNode *paramDist_node, int index)
{
    double result = 0.0;

    if(paramDist_node)
    {
        int current_index = 0;
        DOMNode *child = paramDist_node->getFirstChild();

        //get child whose index is specified
        while((current_index < index && child!=0)||child->getNodeType()!=DOMNode::ELEMENT_NODE)
        {
            if(child->getNodeType()==DOMNode::ELEMENT_NODE)
            {
                current_index = current_index + 1;
            }
            child = child->getNextSibling();
        }

        if(child->hasAttributes())
        {
            DOMNamedNodeMap *param_attr_nodes = child->getAttributes();
            int size = param_attr_nodes->getLength();
            for(int i = 0; i<size; i++)
            {
                DOMAttr *param_attr_node = (DOMAttr*)param_attr_nodes->item(i);
                char *attr_name = XMLString::transcode(param_attr_node->getName());
                if(!strcmp(attr_name,"value"))
                {
                    char* value_string =XMLString::transcode(param_attr_node->getValue());
                    result = atol(value_string);
                }
            }

        }

    }

    return result;
}

//get number of attribute by the given stream name
int XmlHandler::getAttrNum(const DOMNode *root, const char *stream_name)
{
    int result = 0;

    char *schema_name = XmlHandler::getSchemaName(root, stream_name);
    DOMNode *schema_node = XmlHandler::getSchemaNode(root, schema_name);

    DOMNode *child;
    if(schema_node)
    {
        for(child = schema_node->getFirstChild(); child!=0; child = child->getNextSibling())
        {
            if(child->getNodeType()==DOMNode::ELEMENT_NODE)
            {
                char *child_name = XMLString::transcode(child->getNodeName());
                if(!strcmp(child_name, "field"))
                {
                    result = result + 1;
                }
            }
        }
    }

    return result;
}



//get schema name by the given input stream name
char* XmlHandler::getSchemaName(const DOMNode *root, const char *stream_name)
{
    char *result = "";

    DOMNode *input_node = XmlHandler::getInputNode(root, stream_name);
    if(input_node)
    {
        if(input_node->hasAttributes())
        {
            DOMNamedNodeMap *input_attr_nodes = input_node->getAttributes();
            int size = input_attr_nodes->getLength();
            for(int i = 0; i<size; ++i)
            {
                DOMAttr *input_attr_node = (DOMAttr*)input_attr_nodes->item(i);
                char *attr_name = XMLString::transcode(input_attr_node->getName());
                if(!strcmp(attr_name,"schema"))
                {
                    char *attr_value = XMLString::transcode(input_attr_node->getValue());
                    return attr_value;
                }
            }
        }
    }
    return result;
}

//get schema DOMNode by the given schema name
DOMNode* XmlHandler::getSchemaNode(const DOMNode *root, const char *schema_name)
{
    DOMNode *result = NULL;
    DOMNode *child;
    if(root)
    {
        for(child = root->getFirstChild(); child!=0; child = child->getNextSibling())
        {
            if(child->getNodeType()==DOMNode::ELEMENT_NODE)
            {
                char *child_name = XMLString::transcode(child->getNodeName());
                if(!strcmp(child_name, "schema"))
                {
                    if(child->hasAttributes())
                    {
                        DOMNamedNodeMap *schema_attr_nodes = child->getAttributes();
                        int size = schema_attr_nodes->getLength();
                        for(int i = 0; i<size; ++i)
                        {
                            DOMAttr *schema_attr_node = (DOMAttr*)schema_attr_nodes->item(i);
                            char *attr_name = XMLString::transcode(schema_attr_node->getName());
                            if(!strcmp(attr_name,"name"))
                            {
                                char *attr_value = XMLString::transcode(schema_attr_node->getValue());
                                if(!strcmp(attr_value,schema_name))
                                {
                                    result = child;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}



//get Attribute data type by the given stream name and index
int XmlHandler::getAttrDataType(const DOMNode *root, const char* stream_name, int i)
{
    int result = 0;
    char *data_type = "";

    DOMNode *attr_node = XmlHandler::getAttrNode(root, stream_name, i);

    if(attr_node)
    {
        if(attr_node->hasAttributes())
        {
            DOMNamedNodeMap *attr_attr_nodes = attr_node->getAttributes();
            int size = attr_attr_nodes->getLength();
            for(int i = 0; i<size; ++i)
            {
                DOMAttr *attr_attr_node = (DOMAttr*)attr_attr_nodes->item(i);
                char *attr_name = XMLString::transcode(attr_attr_node->getName());
                if(!strcmp(attr_name,"type"))
                {
                    char *attr_value = XMLString::transcode(attr_attr_node->getValue());
                    data_type = attr_value;
                }
            }
        }
    }

    //change datatype from string to int
    if(!(strcmp(data_type, "int")))
    {
        result = 0;
    }
    else
    {
        if(!strcmp(data_type, "float"))
        {
            result = 1;
        }
        else
        {
            if(!strcmp(data_type, "double"))
            {
                result = 2;
            }
            else
            {
                if(!strcmp(data_type, "string"))
                {
                    result = 3;
                }
                else
                {
                    result = 3;

                    NOTICE<< "The data type '" 
                        << data_type 
                        << "' is not correct. Changed it to STRING.";

                }
            }
        }
    }

    return result;
}

//get Attribute Node by the given stream name and index
DOMNode* XmlHandler::getAttrNode(const DOMNode *root, const char *stream_name, int i)
{
    DOMNode *result = NULL;

    char *schema_name = XmlHandler::getSchemaName(root, stream_name);
    DOMNode *schema_node = XmlHandler::getSchemaNode(root, schema_name);

    if(schema_node)
    {
        int current_index = 0;
        DOMNode *child = schema_node->getFirstChild(); 

        while((current_index < i && child!=0)||child->getNodeType()!=DOMNode::ELEMENT_NODE)
        {
            if(child->getNodeType()==DOMNode::ELEMENT_NODE)
            {
                current_index = current_index + 1;
            }
            child = child->getNextSibling();
        }
        result = child;
    }

    return result;
}

//get Attribute Data Length by the given strem name and index
int XmlHandler::getAttrDataLength(const DOMNode *root, const char *stream_name, int i)
{
    int result = 0;
    char *data_length = "";

    DOMNode *attr_node = XmlHandler::getAttrNode(root, stream_name, i);

    if(attr_node)
    {
        if(attr_node->hasAttributes())
        {
            DOMNamedNodeMap *attr_attr_nodes = attr_node->getAttributes();
            int size = attr_attr_nodes->getLength();
            for(int i = 0; i<size; ++i)
            {
                DOMAttr *attr_attr_node = (DOMAttr*)attr_attr_nodes->item(i);
                char *attr_name = XMLString::transcode(attr_attr_node->getName());
                if(!strcmp(attr_name,"size"))
                {
                    char *attr_value = XMLString::transcode(attr_attr_node->getValue());
                    data_length = attr_value;
                }
            }
        }
    }

    result = atol(data_length);
    return result;
}

//get Attribute distribution type (int) by the given stream name and index
int XmlHandler::getAttrDistTypeInt(const DOMNode *root, const char *stream_name, int i)
{
    int result = -1;

    DOMNode *attr_dist_node = XmlHandler::getAttrDistNode(root, stream_name, i);
    char *type_string = XmlHandler::getDistTypeString(attr_dist_node);

    int temp_type_int = -1;
    int &type_int = temp_type_int ;

    if(XmlHandler::changeTypeToInt(type_string, type_int))    
    {
        result = type_int;   
    }
    else
    {
        result = -1;
    }

    return result;
}

//get Attribute distribution node by the given stream name and index 
DOMNode* XmlHandler::getAttrDistNode(const DOMNode *root, const char *stream_name, int i)
{
    DOMNode *result = NULL; 
    DOMNode *attr_node = XmlHandler::getAttrNode(root, stream_name, i);

    DOMNode *child;
    if(attr_node)
    {
        for(child = attr_node->getFirstChild(); child!=0; child = child->getNextSibling())
        {
            if(child->getNodeType()==DOMNode::ELEMENT_NODE)
            {
                char *child_name = XMLString::transcode(child->getNodeName());
                if(!strcmp(child_name, "distribution"))
                {
                    return child; 
                }
            }
        }
    }
    return result;
}

//get paramDistribution type (int) by the given stream name and index
int XmlHandler::getAttrParamDistTypeInt(
                                        const DOMNode       *root, 
                                        const char          *stream_name, 
                                        int                 attr_i, 
                                        int                 param_j)
{
    int result = -1;

    DOMNode *distribution_node = XmlHandler::getAttrDistNode(root, stream_name, attr_i);
    DOMNode *attr_paramDist_node = XmlHandler::getParamDistNode(distribution_node, param_j);

    char *type_string = XmlHandler::getParamDistTypeString(attr_paramDist_node);

    int temp_type_int = -1;
    int &type_int = temp_type_int;

    if(XmlHandler::changeTypeToInt(type_string, type_int))    
    {
        result = type_int;   
    }
    else
    {
        result = -1;
    }

    return result;


}

//get parameter by the given stream name, paramDistribution, and indexes  
double XmlHandler::getAttrParamDistParamValue(
    const DOMNode       *root, 
    const char          *stream_name,
    int                 attr_index,
    int                 paramDist_index,
    int                 param_index)
{
    DOMNode *dist_node = XmlHandler::getAttrDistNode(root, stream_name, attr_index);
    DOMNode *paramDist_node = XmlHandler::getParamDistNode(dist_node, paramDist_index);
    double value = XmlHandler::getParamValue(paramDist_node, param_index);
    return value;

}


//change distribution type from string to int. 
bool XmlHandler::changeTypeToInt(char *type_string, int &type_int)
{
    if(!strcmp(type_string,"CONSTANT"))
    {
        type_int = 0;
    }
    else
    {
        if(!strcmp(type_string,"UNIFORM"))
        {
            type_int = 1;
        }
        else
        {
            if(!strcmp(type_string,"NORMAL"))
            {
                type_int = 2;
            }
            else
            {
                if(!strcmp(type_string,"EXPONENTIAL"))
                {
                    type_int = 3;
                }
                else
                {
                    if(!strcmp(type_string,"LINEAR"))
                    {
                        type_int = 4;
                    }
                    else
                    {
                        if(!strcmp(type_string,"RANDOMWALK"))
                        {
                            type_int = 5;
                        }
                        else
                        {
                            if(!strcmp(type_string,"RANDOMDIRECTIONWALK"))
                            {
                                type_int = 6;
                            }
                            else
                            {
                                if(!strcmp(type_string,"TIMESTAMP"))
                                {
                                    type_int = 7;
                                }
                                else
                                {
                                    if(!strcmp(type_string,"INPUTFILE"))
                                    {
                                        type_int = 8;
                                    }
                                    else
                                    {
                                        if(!strcmp(type_string,"FILENAME"))
                                        {
                                            type_int = 9;
                                        }
                                        else
                                        {
                                            if(!strcmp(type_string,"OTHER"))
                                            {
                                                type_int = 10;
                                            }
                                            else                              
                                            {
                                                return false;
                                            }
                                        }
                                    }
                                }
                            }

                        }
                    }
                }
            }
        }
    }
    return true;

}


//get the number of parameters by the givene distribution type. 
bool XmlHandler::getParamsNum(int type_int, int &type_params_num)
{
    switch(type_int)
    {
    case 0:
        type_params_num = 1;
        break;
    case 1:
        type_params_num = 2;
        break; 
    case 2:
        type_params_num = 2;
        break;
    case 3:
        type_params_num = 1;
        break;
    case 4:
        type_params_num = 4;
        break;
    case 5:
        type_params_num = 5;
        break;
    case 6:
        type_params_num = 5;
        break;
    case 7:
        type_params_num = 0;
        break;
    case 8:
        type_params_num = 0;
        break;
    case 9:
        type_params_num = 0;
        break;
    case 10:    
        type_params_num = 0;
        break;
    default:
        return false;

    }
    return true;

}


//--------------------------------------------------------------------------
//Following is for parsing deployment file.
//--------------------------------------------------------------------------

//get stream numbers by the root node of the deployment file
int XmlHandler::getDeployStreamNum (const DOMNode *root)
{
    int result = 0;
    if (root) 
    {
        DOMNode *child;
        for(child = root->getFirstChild(); child!=0; child =child->getNextSibling())
        {
            if(child->getNodeType()==DOMNode::ELEMENT_NODE)
            {
                char *name = XMLString::transcode(child->getNodeName());
                if(!strcmp(name, "node"))
                {
                    result = result + 1;   
                }
            }
        }
    }
    return result;

}

//get deploy DOMNode by the the given index
DOMNode* XmlHandler::getDeployNode(const DOMNode *root, const int index)
{
    DOMNode *result = NULL;
    if(root)
    {
        int current_index = 0;
        DOMNode *child = root->getFirstChild();


        while((current_index <= index && child!=0)||child->getNodeType()!=DOMNode::ELEMENT_NODE)
        {
            if(child->getNodeType()==DOMNode::ELEMENT_NODE)
            {
                char *child_name = XMLString::transcode(child->getNodeName());
                if(!strcmp(child_name, "node"))
                {
                    if(current_index==index)
                    {
                        result = child;
                        return result;
                    }
                    else
                    {
                        current_index = current_index + 1;
                    }
                }

            }
            child = child->getNextSibling();
        }

    }

    return result;
}


//get deploy node attribute value by the given attribute name and index
char* XmlHandler::getDeployAttrValue(const DOMNode *root, const char *attr_name, int index)
{
    char *result = "";
    DOMNode *deploy_node = XmlHandler::getDeployNode(root,index);
    if(deploy_node)
    {
        if(deploy_node->hasAttributes()) 
        {
            DOMNamedNodeMap *deploy_attr_nodes = deploy_node->getAttributes();
            int size = deploy_attr_nodes->getLength();
            for(int i = 0; i<size; ++i)
            {
                DOMAttr *deploy_attr_node = (DOMAttr*)deploy_attr_nodes->item(i);
                char *current_attr_name = XMLString::transcode(deploy_attr_node->getName());

                if(!strcmp(current_attr_name,attr_name))
                {
                    char *attr_value = XMLString::transcode(deploy_attr_node->getValue());
                    result = attr_value;
                }
            }
        }
    }

    return result;
}

//}


BOREALIS_NAMESPACE_END
