#ifndef XmlHandler_H
#define XmlHandler_H

//C++ headers:
#include <vector>       //for vector

//Xerces headers:
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMAttr.hpp>

#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif


#include <common.h>


BOREALIS_NAMESPACE_BEGIN
XERCES_CPP_NAMESPACE_USE

/**A XML Handler
*/
class XmlHandler
{
public:
	/**Default Constructor
	*/
	XmlHandler();

	/**Destructor
	*/
	~XmlHandler();


	/**read Xml file, change it to XMLDocument    
	*@param xmlFile the XML file to be read
	*/
	DOMDocument *readXmlFileToDoc(const char *xmlFile);	

	/**get all the stream names 
	*@param root the root element of the XML Document
	*/
	vector<char*> getInputStreamNames(const DOMNode *root);

	/**get input DOMNode by the given stream name
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*/
	DOMNode *getInputNode(const DOMNode *root, const char *stream_name);

	/**get genInfo DOMNode by the given stream name
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*/
	DOMNode *getGenInfoNode(const DOMNode *root, const char *stream_name);

	/**get genInfo attribute value by the given attribute name
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*@param attr_name the name of the attribute
	*/
	char *getGenInfoAttrValue(
		const       DOMNode *root, 
		const char  *stream_name, 
		const char  *attr_name);


	///////////////////////////////////////////////////
	//Comman Fuction
	///////////////////////////////////////////////////

	/**change distribution type from string to int. 
	* @param type_string the type in string formate
	* @param type_int the type in integer formate
	*/
	bool changeTypeToInt(char *type_string,int &type_int);

	/**get the number of parameters by the given distribution type.
	* @param type_int the distribution type
	* @param params_num the parameter number of the given distribution type.
	*/
	bool getParamsNum(int type_int, int &params_num);


	/**get paramDistribution DOMNode by the given distribution DOMNode and index 
	* @param distribution_node the distribution XML node
	* @param index the index of the parameter
	*/
	DOMNode *getParamDistNode(const DOMNode *distribution_node, int index);

	/**get paramDistribution type (string) by the given paramDistribution DOMNode
	* @param paramDist_node the paramDistribution XML node
	*/
	char *getParamDistTypeString(const DOMNode *paramDist_node);

	/**get distribution type (string) by the given distribution DOMNode
	*@param distribution_node the Distribution XML node
	*/
	char *getDistTypeString(const DOMNode *distribution_node);

	/**get parammeter by the given paramDistribution DOMNode and index
	*@param paramDist_node the paramDistribution XML node
	*@param index the index of the parameter
	*/

	double getParamValue(const DOMNode *paramDist_node, int index);


	////////////////////////////////////////////////////////////////
	//Following is for geting distribution information from input node
	////////////////////////////////////////////////////////////////

	/**get input distribution type (int) by the givene stream name
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*/
	int getInputDistTypeInt(const DOMNode *root, const char *stream_name);

	/**get distribution DOMNode by the given stream name
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*/
	DOMNode *getInputDistNode(const DOMNode *root, const char *stream_name);



	//each parameter of this distribution is a distribution. 
	//I call this kind of parameter: "paramDistribution",or "paramDist".


	/////////////////////////////////////////////////////////////////////
	//Following is for getting paramDistibution infomation from input node
	/////////////////////////////////////////////////////////////////////

	/**get paramDistribution type (int) by the given stream name and index
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*@param index the index of the paramDistribution
	*/

	int getInputParamDistTypeInt(
		const DOMNode       *root, 
		const char          *stream_name,
		int                 index);

	/**get parameter by the given stream name, paramDistribution, and index  
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*@param paramDist_index the index of the paramDistribution
	*@param param_index the index of the parameter
	*/

	double getInputParamDistParamValue(
		const DOMNode       *root, 
		const char          *stream_name,
		int                 paramDist_index,
		int                 param_index);




	///////////////////////////////////////////////////////////////////
	//Following is for getting attribute information
	///////////////////////////////////////////////////////////////////

	/**get schema name by the given input stream name  
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*/
	char *getSchemaName(const DOMNode *root, const char *stream_name);

	/**get schema DOMNode by the given schema name
	*@param root the root element of the XML Document
	*@param stream_name the name of the schema
	*/
	DOMNode *getSchemaNode(const DOMNode *root, const char *schema_name);

	/**get number of attribute by the given stream name
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*/
	int getAttrNum(const DOMNode *root, const char *stream_name);

	/**get Attribute data type by the given stream name and index
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*@param i the index of the attribute
	*/ 
	int getAttrDataType(const DOMNode *root, const char *stream_name, int i);

	/**get Attribute Node by the given stream name and index
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*@param i the index of the attribute
	*/
	DOMNode *getAttrNode(const DOMNode *root, const char *stream_name, int i);

	/**get Attribute Data Length by the given strem name and index
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*@param i the index of the attribute
	*/
	int getAttrDataLength(const DOMNode *root, const char *stream_name, int i);


	/**get Attribute distribution node by the given stream name and index 
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*@param i the index of the attribute
	*/
	DOMNode *getAttrDistNode(const DOMNode *root, const char *stream_name, int i);

	/**get Attribute distribution type (int) by the given stream name and index
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*@param i the index of the attribute
	*/
	int getAttrDistTypeInt(const DOMNode *root, const char *stream_name, int i);


	///////////////////////////////////////////////////////////////////////
	//Following is for getting paramDistribution information from attr node
	///////////////////////////////////////////////////////////////////////

	/**get paramDistribution type (int) by the given stream name and indexes
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*@param attr_i the index of the attribute
	*@param param_j the index of the parameter
	*/
	int getAttrParamDistTypeInt(
		const DOMNode       *root, 
		const char          *stream_name, 
		int                 attr_i, 
		int                 param_j);

	/**get parameter by the given stream name, paramDistribution, and indexes  
	*@param root the root element of the XML Document
	*@param stream_name the name of the stream
	*@param attr_index the index of the attribute
	*@param paramDist_index the index of the paramDistribution
	*@param param_index the index of the parameter
	*/
	double getAttrParamDistParamValue(
		const DOMNode       *root, 
		const char          *stream_name,
		int                 attr_index,
		int                 paramDist_index,
		int                 param_index);



	////////////////////////////////////////////////////////////////////////
	//Following is for parsing the deployment file.
	////////////////////////////////////////////////////////////////////////

	/**get stream numbers by the root node of the deployment file
	*@param root the root element of the XML Document
	*/
	int getDeployStreamNum (const DOMNode *root);

	/**get publish DOMNode by the the given index
	*@param root the root element of the XML Document
	*@param index the index of the publish XML node
	*/
	DOMNode* getDeployNode(const DOMNode *root, const int index);

	/**get attribute value by the given attribute name and index
	*@param root the root element of the XML Document
	*@param attr_name the name of the attribute
	*@param index the index of the attribute
	*/
	char* getDeployAttrValue(
		const DOMNode       *root, 
		const char          *attr_name,
		int                 index);

private:	
	DOMBuilder	 *parser;

};

class GenXmlErrorHandler: public DOMErrorHandler
{
public:
	GenXmlErrorHandler();
	~GenXmlErrorHandler();

	bool getSawErrors() const;
	bool handleError(const DOMError& domError);
	void resetErrors();

private:

	GenXmlErrorHandler(const GenXmlErrorHandler&);
	void operator=(const GenXmlErrorHandler&);
	bool fSawErrors;

};



class StrX
{
public:

	StrX(const XMLCh* const toTranscode)
	{
		//Call the private transcoding method
		fLocalForm = XMLString::transcode(toTranscode);
	}

	~StrX()
	{
		XMLString::release(&fLocalForm);
	}

	const char* localForm() const
	{
		return fLocalForm;
	}
private:
	char* fLocalForm;

};

inline bool GenXmlErrorHandler::getSawErrors() const
{
	return fSawErrors;
}

BOREALIS_NAMESPACE_END
#endif
