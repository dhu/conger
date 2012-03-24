/*
 * PolarisAdmin.cc
 *
 *  Created on: Oct 7, 2011
 *      Author: jj
 */

#include "PolarisAdmin.h"
#include "DeployDiagram.h"
#include "HeadServer.h"
#include <string>
#include <fstream>
#include <antlr3.h>
#include "SQLLexer.h"
#include "SQLParser.h"
#include <assert.h>

using namespace Borealis;

string parseStatement(string sqlStatement);
string parseCreateSchema(pANTLR3_BASE_TREE treeNode);
string parseCreateStream(pANTLR3_BASE_TREE treeNode);
string parseCreateWindow(pANTLR3_BASE_TREE treeNode);
string parseSelect(pANTLR3_BASE_TREE treeNode);

typedef struct WindowStruct {
	string windowName;
	int size;
	int advance;
	string sizeType;
	WindowStruct()
	{
		size = 0;
		advance = 0;
	}
} Window;

static Window window;
static string inputStream;
static string outputStream;

/** 由一个 AST 树生成一个 xml 片段 */
string parseStatement(string sqlStatement)
{
	pANTLR3_INPUT_STREAM input;
	pSQLLexer lxr;
	pANTLR3_COMMON_TOKEN_STREAM tstream;
	pSQLParser psr;

	const char * inputString = sqlStatement.c_str();

	input = antlr3StringStreamNew((uint8_t *) inputString, ANTLR3_ENC_UTF8,
			strlen(inputString), (uint8_t *) "test_statement");
	lxr = SQLLexerNew(input);
	tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT,
			TOKENSOURCE(lxr));
	psr = SQLParserNew(tstream);

	SQLParser_statement_return statementAST = psr->statement(psr);

	/* get the AST root */
	pANTLR3_BASE_TREE root = statementAST.tree;
	pANTLR3_BASE_TREE treeNode;

	treeNode = (pANTLR3_BASE_TREE) root->getChild(root, 0);

	ANTLR3_UINT32 treeType = treeNode->getType(treeNode);
	string result;
	switch (treeType)
	{
	case TOK_CREATE_SCHEMA:
		result = parseCreateSchema(treeNode);
		break;
	case TOK_CREATE_STREAM:
		result = parseCreateStream(treeNode);
		break;
	case TOK_CREATE_WINDOW:
		result = parseCreateWindow(treeNode);
		break;
	case KW_SELECT: /* antlr 语法错误，此处应该为 TOK_SELECT */
		result = parseSelect(treeNode);
		break;
	default:
		WARN << "Unknown tree type... " << treeType;
		break;
	}
	input->close(input);
	lxr->free(lxr);
	tstream->free(tstream);
	psr->free(psr);
	return result;
}

string parseCreateSchema(pANTLR3_BASE_TREE treeNode)
{
	string xmlFragment = "<schema name=\"";
	pANTLR3_BASE_TREE childNode = (pANTLR3_BASE_TREE) treeNode->getChild(treeNode, 0);
	string schemaName = string((char *)childNode->getText(childNode)->chars);
	xmlFragment.append(schemaName);
	xmlFragment.append("\">\n");

	childNode = (pANTLR3_BASE_TREE) treeNode->getChild(treeNode, 1);
	assert(childNode->getType(childNode) == TOK_SCHEMA_LIST);

	for (unsigned int i = 0; i < childNode->getChildCount(childNode); i++)
	{
		pANTLR3_BASE_TREE oneNode = (pANTLR3_BASE_TREE) childNode->getChild(childNode, i);
		assert(oneNode->getType(oneNode) == TOK_NAME_TYPE);
		pANTLR3_BASE_TREE nameNode = (pANTLR3_BASE_TREE) oneNode->getChild(oneNode, 0);
		xmlFragment.append("\t<field  name=\"");
		string name = string((char *)nameNode->getText(nameNode)->chars);
		xmlFragment.append(name).append("\"  type=\"");
		pANTLR3_BASE_TREE typeNode = (pANTLR3_BASE_TREE) oneNode->getChild(oneNode, 1);
		string type = string((char *)typeNode->getText(typeNode)->chars);
		xmlFragment.append(type).append("\" />\n");
	}

	xmlFragment.append("</schema>\n");
	return xmlFragment;
}

/* <input stream="Packet" schema="PacketTuple" /> */
string parseCreateStream(pANTLR3_BASE_TREE treeNode)
{
	string xmlFragment = "<";
	assert(treeNode->getChildCount(treeNode) == 3);
	pANTLR3_BASE_TREE childNode = (pANTLR3_BASE_TREE) treeNode->getChild(treeNode, 0);
	int inputType = 0;
	if (childNode->getType(childNode) == KW_INPUT)
	{
		xmlFragment.append("input stream=\"");
		inputType = 1;
	}
	else
	{
		xmlFragment.append("output stream=\"");
		inputType = 2;
	}
	childNode = (pANTLR3_BASE_TREE) treeNode->getChild(treeNode, 1);
	string streamName = string((char *)childNode->getText(childNode)->chars);
	xmlFragment.append(streamName);
	xmlFragment.append("\" schema=\"");
	childNode = (pANTLR3_BASE_TREE) treeNode->getChild(treeNode, 2);
	string schemaName = string((char *)childNode->getText(childNode)->chars);
	xmlFragment.append(schemaName);
	xmlFragment.append("\" />\n");
	if (inputType == 1)
	{
		inputStream = streamName;
	}
	else
	{
		outputStream = streamName;
	}
	return xmlFragment;
}

string parseCreateWindow(pANTLR3_BASE_TREE treeNode)
{
	pANTLR3_BASE_TREE childNode = (pANTLR3_BASE_TREE) treeNode->getChild(treeNode, 0);
	window.windowName = string((char *)childNode->getText(childNode)->chars);
	childNode = (pANTLR3_BASE_TREE) treeNode->getChild(treeNode, 1);
	char * sizeChar = (char *)childNode->getText(childNode)->chars;
	window.size = atoi(sizeChar);
	childNode = (pANTLR3_BASE_TREE) treeNode->getChild(treeNode, 2);
	char * advanceChar = (char *)childNode->getText(childNode)->chars;
	window.advance = atoi(advanceChar);
	childNode = (pANTLR3_BASE_TREE) treeNode->getChild(treeNode, 3);
	window.sizeType = string((char *)childNode->getText(childNode)->chars);
	return string();
}

/*
<box name="mybox"   type="aggregate" >
	<in   stream="Packet"    />
	<out  stream="Aggregate" />

	<parameter   name="aggregate-function.0"  value="max(price)"  />
	<parameter   name="aggregate-function-output-name.0" value="maxprice"  />

	<parameter   name="aggregate-function.1"  value="max(time)"  />
	<parameter   name="aggregate-function-output-name.1" value="currenttime"  />

	<parameter   name="window-size-by"        value="VALUES"   />
	<parameter   name="window-size"           value="180"        />
	<parameter   name="advance"               value="10"        />
	<parameter   name="order-by"              value="FIELD"    />
	<parameter   name="order-on-field"        value="time"     />
</box>
*/
string parseSelect(pANTLR3_BASE_TREE treeNode)
{
	if (window.size == 0)
	{
		WARN << "Create window before query...";
	}
	string result = "<box name=\"mybox\"   type=\"aggregate\" >\n"
			"\t<in   stream=\"";
	result.append(inputStream).append("\"    />\n\t<out  stream=\"");
	result.append(outputStream).append("\" />\n");
	pANTLR3_BASE_TREE childNode = (pANTLR3_BASE_TREE) treeNode->getChild(treeNode, 0);
	int childCount = childNode->getChildCount(childNode);
	for (int i = 0; i < childCount; i++)
	{
		pANTLR3_BASE_TREE node = (pANTLR3_BASE_TREE) childNode->getChild(childNode, i);
		if (node->getType(node) == TOK_SELEXPR)
		{
			pANTLR3_BASE_TREE subnode = (pANTLR3_BASE_TREE) node->getChild(node, 0);
			string functionName = string((char *)subnode->getText(subnode)->chars);
			subnode = (pANTLR3_BASE_TREE) node->getChild(node, 1);
			string itemName = string((char *)subnode->getText(subnode)->chars);
			subnode = (pANTLR3_BASE_TREE) node->getChild(node, 2);
			string asName = string((char *)subnode->getText(subnode)->chars);
			char *t = "\t<parameter   name=\"aggregate-function.%d\"  value=\"%s(%s)\"  />\n"
					"\t<parameter   name=\"aggregate-function-output-name.%d\" value=\"%s\"  />\n";
			char tmp[300];
			sprintf(tmp, t, i, functionName.c_str(), itemName.c_str(), i, asName.c_str());
			result.append(string(tmp));
		}
	}
	char *t = "\t<parameter   name=\"window-size-by\"        value=\"VALUES\"   />\n"
			"\t<parameter   name=\"window-size\"           value=\"%d\"        />\n"
			"\t<parameter   name=\"advance\"               value=\"%d\"        />\n"
			"\t<parameter   name=\"order-by\"              value=\"FIELD\"    />\n"
			"\t<parameter   name=\"order-on-field\"        value=\"%s\"     />\n";
	char tmp[900];
	sprintf(tmp, t, window.size, window.advance, window.sizeType.c_str());
	result.append(string(tmp)).append("</box>\n");

	return result;
}

PolarisAdmin::PolarisAdmin()
{

}

void PolarisAdmin::transferSQLToXML(string sqlFilename, string xmlFilename)
{
	string xmlContent = "<?xml version=\"1.0\"?>\n"
			"<!DOCTYPE borealis SYSTEM \"/home/jj/Workspace/borealis_summer_2008/borealis/src/src/borealis.dtd\">\n"
			"<!-- Borealis query diagram for:  stock.cc -->\n"
			"<borealis>\n";

	fstream instream(sqlFilename.c_str(), fstream::in);

	char buffer[1024];
	while (instream.good())
	{
		instream.getline(buffer, 1024);
		if (strlen(buffer) == 0)
		{
			continue;
		}
		string line(buffer);
		xmlContent.append(parseStatement(line));
	}
	xmlContent.append("</borealis>\n");

	fstream outstream(xmlFilename.c_str(), fstream::out);
	outstream << xmlContent;
	outstream.close();
	instream.close();
}

void PolarisAdmin::start(string sqlFilename)
{
	int dotPosition = sqlFilename.find_first_of(".");
	string queryName = sqlFilename.substr(0, dotPosition);
	string xmlFilename = queryName.append(".xml");

	transferSQLToXML(sqlFilename, xmlFilename);

	HeadServer server;
	DeployDiagram &diagram = server.get_global_catalog();
	Status status;
	status = diagram.parse_file(xmlFilename);
	DEBUG << status;
	status = diagram.generate_deployment();
	DEBUG << status;
	DEBUG << diagram.get_head();
	status = diagram.deploy();
	DEBUG << status;
}

PolarisAdmin::~PolarisAdmin()
{

}

int main(int argc, const char *argv[])
{
	if (argc != 2)
	{
		WARN << "Usage: padmin filename.sql";
		exit(1);
	}

	PolarisAdmin padmin;
	padmin.start(string(argv[1]));
}
