#include <xercesDomUtil.h>
#include <XmlTempString.h>
#include <parseutil.h>

#include <Exceptions.h>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <sstream>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

BOREALIS_NAMESPACE_BEGIN;

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// *** NOTE: ALL OF THESE HELPER FUNCTIONS ASSUME THAT XERCES IS PRESENTLY
// *** INITIALIZED VIA XMLPlatformUtils::Initialize()
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


//===============================================================================

void write_dom_doc_to_file(const DOMDocument & doc, string filename)
    throw (exception)
{
    try
        {
            DOMImplementationLS * p_ls_impl = get_dom_implementation_ls();
            DOMWriter * p_writer = p_ls_impl->createDOMWriter();

            // Try to use pretty-printing...
            if (p_writer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
                {
                    p_writer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
                }

            XMLFormatTarget * p_target = new LocalFileFormatTarget(filename.c_str());
            p_writer->writeNode(p_target, doc);

            p_writer->release();
            delete p_target;
        }
    catch (const DOMException & e)
        {
            ostringstream os;
            os << "DOMException thrown: " << XMLString::transcode(e.msg);
            Throw(AuroraException, os.str());
        }
}

//===============================================================================

DOMDocument * read_dom_doc_from_file(DOMImplementationLS & impl, string filename)
    throw (exception)

{
    DOMBuilder * p_builder =
        impl.createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, NULL);

    if (p_builder == NULL)
        {
            Throw(AuroraException, "Couldn't create a DOM Builder");
        }

    XmlTempString xml_filename(filename.c_str());
    return p_builder->parseURI(xml_filename.to_xml());
}

//===============================================================================


DOMImplementationLS * get_dom_implementation_ls()
    throw (exception)
{
    try
        {
            XmlTempString xstr_ls("LS");

            DOMImplementation * p_impl = DOMImplementationRegistry::getDOMImplementation(xstr_ls.to_xml());
            if (p_impl == NULL)
                {
                    Throw(AuroraException,
                                      "DOMImplementationRegistry::getDOMImplementation(\"LS\") returned NULL");
                }

            DOMImplementationLS * p_ls = dynamic_cast<DOMImplementationLS *>(p_impl);
            if (p_impl == NULL)
                {
                    Throw(AuroraException, "Class cast problem");
                }

            return p_ls;
        }
    catch (const DOMException & e)
        {
            ostringstream os;
            os << "DOMException thrown: " << XMLString::transcode(e.msg);
            Throw(AuroraException, os.str());
        }
}

//===============================================================================

DOMImplementation * get_dom_implementation()
  throw (exception)
{
  try
    {
      XmlTempString xstr_ls("LS");
      DOMImplementation * p_impl = DOMImplementationRegistry::getDOMImplementation(xstr_ls.to_xml());
      return p_impl;
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

DOMElement * create_dom_element(DOMDocument & doc_owner, string element_name)
  throw (exception)
{
  try
    {
      XmlTempString xstr_elem_name(element_name.c_str());
      DOMElement * return_val = doc_owner.createElement(xstr_elem_name.to_xml());
      return return_val;
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

DOMElement * get_dom_child_by_key_tag(const DOMElement & parent, string child_tag_name)
  throw (exception)
{
  try
    {
      XmlTempString xstr_child_tag(child_tag_name.c_str());

      DOMNodeList * p_child_list = parent.getElementsByTagName(xstr_child_tag.to_xml());

      if (p_child_list->getLength() != 1)
    {
      ostringstream os;
      os << "Couldn't find exactly one child element with the tag \""
         << child_tag_name << "\"" << endl;
      Throw(AuroraException, os.str());
    }

      return dynamic_cast<DOMElement *>(p_child_list->item(0));
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

bool parse_xml_string_as_bool(const XMLCh * p_xml)
  throw (exception)
{
  if (p_xml == NULL)
    {
      Throw(AuroraException, "pXml == NULL");
    }

  const char * p_value = XMLString::transcode(p_xml);

  bool return_val;
  try
      {
          return_val = string_to_bool(p_value);
      }
  catch (...)
      {
          delete p_value;
          throw;
      }

  delete [] p_value;
  return return_val;
}

//===============================================================================

int parse_xml_string_as_int(const XMLCh * p_xml)
  throw (exception)
{
  if (p_xml == NULL)
    {
      Throw(AuroraException, "pXml == NULL");
    }

  const char * p_value = XMLString::transcode(p_xml);

  int return_val;
  try
      {
          return_val = string_to_int(p_value);
      }
  catch (...)
      {
          delete p_value;
          throw;
      }

  delete [] p_value;
  return return_val;

  /*

  const char  * pValue = XMLString::transcode(pXml);

  char * endPtr;
  long longVal = strtol(pValue, & endPtr, 10);

  if (*endPtr != '\0')
    {
      ostringstream os;
      os << "Tried to parse an XML string as an int, but we couldn't: \""
     << pValue << "\"";
      Throw(AuroraException, os.str());
    }

  if ((longVal < numeric_limits<int>::min()) ||
      (longVal > numeric_limits<int>::max()))
    {
      ostringstream  os;
      os << "Tried to parse an XML string as an int, but it was out of range: \""
     << pValue << "\"";
      Throw(AuroraException, os.str());
    }

  delete [] pValue;

  return int(longVal);
  */
}

//===============================================================================

unsigned int parse_xml_string_as_uint(const XMLCh * p_xml)
  throw (exception)
{
  if (p_xml == NULL)
    {
      Throw(AuroraException, "pXml == NULL");
    }

  const char  * p_value = XMLString::transcode(p_xml);

  unsigned int return_val;
  try
      {
          return_val = string_to_uint(p_value);
      }
  catch (...)
      {
          delete p_value;
          throw;
      }

  delete [] p_value;
  return return_val;
}

//===============================================================================

unsigned long parse_xml_string_as_long(const XMLCh * p_xml)
  throw (exception)
{
  if (p_xml == NULL)
    {
      Throw(AuroraException, "pXml == NULL");
    }

  const char * p_value = XMLString::transcode(p_xml);

  long long_val;
  try
      {
          long_val = string_to_long(p_value);
      }
  catch (...)
      {
          delete p_value;
          throw;
      }

  delete [] p_value;
  return long_val;
}

//===============================================================================

unsigned long parse_xml_string_as_ulong(const XMLCh * p_xml)
  throw (exception)
{
  if (p_xml == NULL)
    {
      Throw(AuroraException, "pXml == NULL");
    }

  const char * p_value = XMLString::transcode(p_xml);

  unsigned long ulong_val;
  try
      {
          ulong_val = string_to_ulong(p_value);
      }
  catch (...)
      {
          delete p_value;
          throw;
      }

  delete [] p_value;
  return ulong_val;
}

//===============================================================================

unsigned long long parse_xml_string_as_ulong_long(const XMLCh * p_xml)
  throw (exception)
{
  if (p_xml == NULL)
    {
      Throw(AuroraException, "pXml == NULL");
    }

  const char * p_value = XMLString::transcode(p_xml);

  unsigned long long ull_val;
  try
      {
          ull_val = string_to_ulong_long(p_value);
      }
  catch (...)
      {
          delete p_value;
          throw;
      }


    /*
  size_t numDigits = strlen(pValue);
  if ((numDigits == 0) ||
      (numDigits > size_t(numeric_limits<unsigned long long>::digits10)))
    {
      ostringstream os;
      os << "Tried to parse an XML string as an unsigned long long, but we couldn't: \""
     << pValue << "\" (it had an inappropriate number of digits";
      Throw(AuroraException, os.str());
    }

  unsigned long long accumulator = 0;

  for (size_t i = 0; i < numDigits; ++i)
    {
      char c = pValue[i];

      if (! isdigit(c))
    {
      ostringstream os;
      os << "Tried to parse an XML string as an unsigned long long, but we couldn't: \""
         << pValue << "\"";
      Throw(AuroraException, os.str());
    }

      accumulator = (accumulator * 10) + (c - '0');
    }
    */

  delete [] p_value;
  return ull_val;
}

//===============================================================================

double parse_xml_string_as_double(const XMLCh * p_xml)
  throw (exception)
{
  if (p_xml == NULL)
    {
      Throw(AuroraException, "pXml == NULL");
    }

  const char * p_value = XMLString::transcode(p_xml);

  double return_val;
  try
      {
          return_val = string_to_double(p_value);
      }
  catch (...)
      {
          delete p_value;
          throw;
      }

  delete [] p_value;
  return return_val;
}

//===============================================================================

void save_int_vector_to_xml(const vector<int> & v, DOMDocument & doc, DOMElement & elem)
  throw (exception)
{
  for (size_t i = 0; i < v.size(); ++i)
    {
      DOMElement * p_int_elem = create_dom_element(doc, "int");
      set_dom_attribute(* p_int_elem, "value", v[i]);
      elem.appendChild(p_int_elem);
    }
}

//===============================================================================

void load_int_vector_from_xml(vector<int> & v, DOMElement & elem)
  throw (exception)
{
  DOMNodeList * p_child_list = elem.getChildNodes();

  XmlTempString xstr_int("int");

  for (XMLSize_t i = 0; i < (p_child_list->getLength()); ++i)
    {
      DOMElement & int_elem = *(dynamic_cast<DOMElement *>(p_child_list->item(i)));

      if (XMLString::compareString(int_elem.getTagName(),
                   xstr_int.to_xml()) != 0)
    {
      Throw(AuroraException, "A child element wasn't tagged as 'int'");
    }

      int value;
      get_dom_attribute(int_elem, "value", value);
      v.push_back(value);
    }
}

//===============================================================================

void save_timeval_to_xml(const timeval & tv, DOMDocument & doc, DOMElement & elem)
  throw (exception)
{
  set_dom_attribute(elem, "tv_sec",  tv.tv_sec);
  set_dom_attribute(elem, "tv_usec", tv.tv_usec);
}

//===============================================================================

void load_timeval_from_xml(timeval & tv, DOMElement & elem)
  throw (exception)
{
  get_dom_attribute(elem, "tv_sec",  tv.tv_sec);
  get_dom_attribute(elem, "tv_usec", tv.tv_usec);
}

//===============================================================================

void set_dom_attribute(DOMElement & elem, string attribute_name, bool new_value)
  throw (exception)
{
  try
    {
      char sz_bool[2];
      if (new_value)
    {
      sz_bool[0] = 'T';
    }
      else
    {
      sz_bool[0] = 'F';
    }

      sz_bool[1] = '\0';

      XmlTempString xstr_attr_name(attribute_name.c_str());
      XmlTempString xstr_attr_value(sz_bool);

      elem.setAttribute(xstr_attr_name.to_xml(), xstr_attr_value.to_xml());
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

void set_dom_attribute(DOMElement & elem, string attribute_name, int new_value)
  throw (exception)
{
  try
    {
      char sz_value[30];
      if (snprintf(sz_value, sizeof(sz_value), "%d", new_value) == -1)
    {
      Throw(AuroraException, "Undersized text buffer");
    }

      XmlTempString xstr_attr_name(attribute_name.c_str());
      XmlTempString xstr_attr_value(sz_value);

      elem.setAttribute(xstr_attr_name.to_xml(), xstr_attr_value.to_xml());
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

void set_dom_attribute(DOMElement & elem, string attribute_name, unsigned int new_value)
  throw (exception)
{
  try
    {
      char sz_value[30];
      if (snprintf(sz_value, sizeof(sz_value), "%u", new_value) == -1)
    {
      Throw(AuroraException, "Undersized text buffer");
    }

      XmlTempString xstr_attr_name(attribute_name.c_str());
      XmlTempString xstr_attr_value(sz_value);

      elem.setAttribute(xstr_attr_name.to_xml(), xstr_attr_value.to_xml());
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

void set_dom_attribute(DOMElement & elem, string attribute_name, long new_value)
  throw (exception)
{
  try
    {
      char sz_value[30];
      if (snprintf(sz_value, sizeof(sz_value), "%ld", new_value) == -1)
    {
      Throw(AuroraException, "Undersized text buffer");
    }

      XmlTempString xstr_attr_name(attribute_name.c_str());
      XmlTempString xstr_attr_value(sz_value);

      elem.setAttribute(xstr_attr_name.to_xml(), xstr_attr_value.to_xml());
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

void set_dom_attribute(DOMElement & elem, string attribute_name, unsigned long new_value)
  throw (exception)
{
  try
    {
      char sz_value[30];
      if (snprintf(sz_value, sizeof(sz_value), "%lu", new_value) == -1)
    {
      Throw(AuroraException, "Undersized text buffer");
    }

      XmlTempString xstr_attr_name(attribute_name.c_str());
      XmlTempString xstr_attr_value(sz_value);

      elem.setAttribute(xstr_attr_name.to_xml(), xstr_attr_value.to_xml());
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

void set_dom_attribute(DOMElement & elem, string attribute_name,
             unsigned long long new_value)
  throw (exception)
{
  try
    {
      char sz_value[numeric_limits<unsigned long long>::digits10 + 1];
      if (snprintf(sz_value, sizeof(sz_value), "%llu", new_value) == -1)
    {
      Throw(AuroraException, "Undersized text buffer");
    }

      XmlTempString xstr_attr_name(attribute_name.c_str());
      XmlTempString xstr_attr_value(sz_value);

      elem.setAttribute(xstr_attr_name.to_xml(), xstr_attr_value.to_xml());
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

void set_dom_attribute(DOMElement & elem, string attribute_name, string new_value)
  throw (exception)
{
  try
    {
      XmlTempString xstr_attr_name(attribute_name.c_str());
      XmlTempString xstr_attr_value(new_value.c_str());

      elem.setAttribute(xstr_attr_name.to_xml(), xstr_attr_value.to_xml());
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

void set_dom_attribute(DOMElement & elem, string attribute_name, double new_value)
  throw (exception)
{
  ostringstream os;
  os << new_value;

  try
    {
      XmlTempString xstr_attr_name(attribute_name.c_str());
      XmlTempString xstr_attr_value(os.str().c_str());

      elem.setAttribute(xstr_attr_name.to_xml(), xstr_attr_value.to_xml());
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

void get_dom_attribute(const DOMElement & elem, string attribute_name, bool & value)
  throw (exception)
{
  try
    {
      XmlTempString xstr_attr_name(attribute_name.c_str());
      value = parse_xml_string_as_bool(elem.getAttribute(xstr_attr_name.to_xml()));
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

void get_dom_attribute(const DOMElement & elem, string attribute_name, int & value)
  throw (exception)
{
  try
    {
      XmlTempString xstr_attr_name(attribute_name.c_str());
      value = parse_xml_string_as_int(elem.getAttribute(xstr_attr_name.to_xml()));
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

void get_dom_attribute(const DOMElement & elem, string attribute_name, unsigned int & value)
  throw (exception)
{
  try
    {
      XmlTempString xstr_attr_name(attribute_name.c_str());
      value = parse_xml_string_as_uint(elem.getAttribute(xstr_attr_name.to_xml()));
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

void get_dom_attribute(const DOMElement & elem, string attribute_name, long & value)
  throw (exception)
{
  try
    {
      XmlTempString xstr_attr_name(attribute_name.c_str());
      value = parse_xml_string_as_long(elem.getAttribute(xstr_attr_name.to_xml()));
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

void get_dom_attribute(const DOMElement & elem, string attribute_name, unsigned long & value)
  throw (exception)
{
  try
    {
      XmlTempString xstr_attr_name(attribute_name.c_str());
      value = parse_xml_string_as_ulong(elem.getAttribute(xstr_attr_name.to_xml()));
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

void get_dom_attribute(const DOMElement & elem, string attribute_name,
             unsigned long long & value)
  throw (exception)
{
  try
    {
      XmlTempString xstr_attr_name(attribute_name.c_str());
      value = parse_xml_string_as_ulong_long(elem.getAttribute(xstr_attr_name.to_xml()));
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

void get_dom_attribute(const DOMElement & elem, string attribute_name, string & value)
  throw (exception)
{
  try
    {
      XmlTempString xstr_attr_name(attribute_name.c_str());

      const XMLCh * p_xml = elem.getAttribute(xstr_attr_name.to_xml());
      if (p_xml == NULL)
    {
      Throw(AuroraException, "pXml == NULL");
    }

      const char * p_value = XMLString::transcode(p_xml);
      value = p_value;
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

void get_dom_attribute(const DOMElement & elem, string attribute_name, double & value)
  throw (exception)
{
  try
    {
      XmlTempString xstr_attr_name(attribute_name.c_str());
      value = parse_xml_string_as_double(elem.getAttribute(xstr_attr_name.to_xml()));
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

bool is_dom_attribute_present(const DOMElement & elem, string attribute_name)
  throw (exception)
{
  try
    {
      XmlTempString xstr_attr_name(attribute_name.c_str());
      return (elem.getAttributeNode(xstr_attr_name.to_xml()) != NULL);
    }
  catch (const DOMException & e)
    {
      ostringstream os;
      os << "DOMException thrown: " << XMLString::transcode(e.msg);
      Throw(AuroraException, os.str());
    }
}

//===============================================================================

struct AuroraDOMErrorHandler : public DOMErrorHandler
{
    string err;

    bool handleError(const DOMError &dom_error)
    {
        err = "DOM error (";
        if (dom_error.getLocation()->getURI())
            err << dom_error.getLocation()->getURI() << ", ";
        err << "line " << dom_error.getLocation()->getLineNumber()
            << ", column " << dom_error.getLocation()->getColumnNumber() << "): "
            << dom_error.getMessage();

        return false;
    }
};

//===============================================================================

ptr<DOMDocument> parse_xml_file(string filename) throw (AuroraBadXmlException)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf))
        Throw(AuroraBadXmlException, "Unable to stat XML file \"" + filename + "\"");

    XMLPlatformUtils::Initialize();

    static const XMLCh g_ls[] = { chLatin_L, chLatin_S, chNull };
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(g_ls);
    ptr<DOMBuilder> parser(((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0));
    assert(parser);

    AuroraDOMErrorHandler errh;
    parser->setErrorHandler(&errh);
    parser->setFeature(XMLUni::fgXercesUserAdoptsDOMDocument, true);

    try
    {
        XmlTempString xfilename(filename.c_str());
        ptr<DOMDocument> doc( parser->parseURI(xfilename.to_xml()) );

        if (!errh.err.empty())
            Throw(AuroraBadXmlException, errh.err);

        if (!doc)
            Throw(AuroraBadXmlException, "Unable to parse document (unknown error)");

        if (!doc->getDocumentElement())
            Throw(AuroraBadXmlException, "Unable to parse document; no root element (unknown error)");

        return doc;
    } catch (XMLException& e)
    {
        Throw(AuroraBadXmlException, "XML exception: " + to_string(e));
    } catch (DOMException& e)
    {
        Throw(AuroraBadXmlException, "DOM exception: " + to_string(e));
    }
}

//===============================================================================

// XXX: consolidate with parseXmlFile code somehow
ptr<DOMDocument> parseXmlString(string data) throw (AuroraBadXmlException)
{
    return parseXmlString(data.data(), data.length());
}

ptr<DOMDocument> parseXmlString(const void *data, unsigned int length) throw (AuroraBadXmlException)
{
    XMLPlatformUtils::Initialize();

    static const XMLCh g_ls[] = { chLatin_L, chLatin_S, chNull };
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(g_ls);
    ptr<DOMBuilder> parser(((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0));
    assert(parser);

    AuroraDOMErrorHandler errh;
    parser->setErrorHandler(&errh);
    parser->setFeature(XMLUni::fgXercesUserAdoptsDOMDocument, true);

    try
    {
        MemBufInputSource in((const XMLByte*)data, length,
                             "parseXmlString");
        Wrapper4InputSource domin(&in, false);
        ptr<DOMDocument> doc( parser->parse(domin) );

        if (!errh.err.empty())
            Throw(AuroraBadXmlException, errh.err);

        if (!doc)
            Throw(AuroraBadXmlException, "Unable to parse document (unknown error)");

        if (!doc->getDocumentElement())
            Throw(AuroraBadXmlException, "Unable to parse document; no root element (unknown error)");

        return doc;
    } catch (XMLException& e)
    {
        WARN << to_string(e);
        Throw(AuroraBadXmlException, "XML exception: " + to_string(e));
    } catch (DOMException& e)
    {
        WARN << to_string(e);
        Throw(AuroraBadXmlException, "DOM exception: " + to_string(e));
    }
}

//===============================================================================

string xml_attribute(const DOMElement* element, string name, xml_attr_req req)
    throw (AuroraBadEntityException)
{
    XMLCh xname[64];
    XMLString::transcode(name.c_str(), xname, sizeof xname - 1);

    const DOMAttr *attr = element->getAttributeNode(xname);
    const XMLCh *value = attr ? attr->getValue() : 0;

    if ( (req == ATTR_REQUIRED && !value) ||
         (req == ATTR_NON_EMPTY && (!value || !*value)) )
        Throw(AuroraBadEntityException,
              "<" + to_string(element->getTagName()) + "> lacks "
              "required attribute \"" + name + "\"");

    if (!value) return string();
    return to_string(value);
}

//===============================================================================

string xml_attribute(const DOMElement* element, string name, string def)
    throw (AuroraBadEntityException)
{
    XMLCh xname[64];
    XMLString::transcode(name.c_str(), xname, sizeof xname - 1);

    const DOMAttr *attr = element->getAttributeNode(xname);
    return attr ? to_string(attr->getValue()) : def;
}

//===============================================================================

void xml_expect_tag(const DOMElement *element, string required_name)
    throw (AuroraBadEntityException)
{
    if (!element)
        Throw(AuroraBadEntityException, "Expected <" + required_name + ">");

    string actual_name = to_string(element->getTagName());
    if (actual_name != required_name)
        Throw(AuroraBadEntityException, "Expected <" + required_name + "> but got <" + actual_name + ">");
}

//===============================================================================

string xml_escape(string in)
{
    string::size_type  start = 0;
    string::size_type  match;

    string out;

    while ((match = in.find_first_of("&<>\"'", start)) != string::npos)
    {
        out.append(in, start, match - start);
        switch (in[match])
        {
          case '&':
            out.append("&amp;"); break;
          case '<':
            out.append("&lt;"); break;
          case '>':
            out.append("&gt;"); break;
          case '\"':
            out.append("&quot;"); break;
          case '\'':
            out.append("&apos;"); break;
          default:
            out += in[match];
        }
        start = match + 1;
    }

    out.append(in, start, string::npos);
    return out;
}

//===============================================================================

DOMElement* xml_only_tag(const DOMElement* parent, bool required)
    throw (AuroraException)
{
    return xml_only_tag(parent, string(), required);
}

DOMElement* xml_only_tag(const DOMElement* parent, string tag, bool required)
    throw (AuroraException)
{
    DOMElement *ret = 0;

    for (DOMNode *vnode = parent->getFirstChild(); vnode; vnode = vnode->getNextSibling())
    {
        if (vnode->getNodeType() != DOMNode::ELEMENT_NODE)
            continue;

        DOMElement *elt = static_cast<DOMElement*>(vnode);
        if (!tag.empty() && to_string(elt->getTagName()) != tag)
            continue;

        if (ret)
            Throw(AuroraBadEntityException,
                  "Expect only one " + (tag.empty() ? "tag" : "<" + tag + "> tag")
                  + " in " + to_string(parent->getTagName()));

        ret = static_cast<DOMElement*>(vnode);
    }

    if (!ret && required)
        Throw(AuroraBadEntityException,
              "Missing " + (tag.empty() ? "tag" : "<" + tag + "> tag") +
              " in <" + to_string(parent->getTagName()) + ">");
    return ret;
}

void xml_child_elements(vector<DOMElement*>& children, const DOMElement* parent, string tag)
{
    children.clear();

    for (DOMNode *node = parent->getFirstChild(); node; node = node->getNextSibling())
    {
        if (node->getNodeType() != DOMNode::ELEMENT_NODE)
            continue;

        DOMElement *elt = static_cast<DOMElement *>(node);
        if (!tag.empty() && to_string(elt->getTagName()) != tag)
            continue;

        children.push_back(elt);
    }
}

// Serialize a DOM node as UTF-8 text.
string xml_serialize(const DOMNode *node)
{
    XMLPlatformUtils::Initialize();

    static const XMLCh g_ls[] = { chLatin_L, chLatin_S, chNull };
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(g_ls);
    DOMWriter *writer = ((DOMImplementationLS*)impl)->createDOMWriter();

    MemBufFormatTarget target;
    writer->writeNode(&target, *node);
    delete writer;

    return string((const char *)target.getRawBuffer(), target.getLen());
}

BOREALIS_NAMESPACE_END;
