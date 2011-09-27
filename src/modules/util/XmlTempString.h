#ifndef XMLTEMPSTRING_H
#define XMLTEMPSTRING_H

#include <exception>
#include <assert.h>
#include <xercesc/util/XMLString.hpp>

#include "common.h"

BOREALIS_NAMESPACE_BEGIN;

// So long as this class remains instantiated, the XMLChr * it provides is valid.
class XmlTempString
{
public:
  XmlTempString(const char * psz_string)
    throw (exception);

  virtual ~XmlTempString();

  // This object retains ownership of the string whose pointer is retunrned.
  const XMLCh * to_xml() const;

private:
  XmlTempString() {assert(false);}
  XmlTempString(const XmlTempString &) {assert(false);}
  XmlTempString & operator= (const XmlTempString &) {assert(false); return *this;}

  XMLCh * _p_xml_string;
};

BOREALIS_NAMESPACE_END;

#endif
