#include <XmlTempString.h>

BOREALIS_NAMESPACE_BEGIN;

#ifdef XERCES_CPP_NAMESPACE
using namespace xercesc;
#endif

XmlTempString::XmlTempString(const char * psz_string)
  throw (exception)
{
  _p_xml_string = XMLString::transcode(psz_string);
}

XmlTempString::~XmlTempString()
{
  delete[] _p_xml_string;
}


const XMLCh * XmlTempString::to_xml() const
{
  return _p_xml_string;
}

BOREALIS_NAMESPACE_END;
