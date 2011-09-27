#ifndef XERCESDOMUTIL_H
#define XERCESDOMUTIL_H

#include <exception>
#include <string>
#include <vector>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <sys/time.h>

#include "Exceptions.h"

BOREALIS_NAMESPACE_BEGIN;

#ifdef XERCES_CPP_NAMESPACE
using namespace xercesc;
#endif

//===============================================================================

// Creates an XML file with the specified name, whose contents is the specified
// subtree.
void write_dom_doc_to_file(const DOMDocument & doc, string filename)
  throw (exception);

// Creates a DOMDocument from the specified XML file using the provided
// implementation object.
DOMDocument * read_dom_doc_from_file(DOMImplementationLS & impl, string filename)
  throw (exception);

// Uses the DOMImplementationRegistry. Will throw an exception rather than
// returning NULL.
DOMImplementationLS * get_dom_implementation_ls()
  throw (exception);

// Uses the DOMImplementationRegistry
DOMImplementation * get_dom_implementation()
  throw (exception);

DOMElement * create_dom_element(DOMDocument & doc_owner, string element_name)
  throw (exception);

// Returns a pointer to the one and only child element with the the specified
// tag. Throws an exception if there wasn't exactly one such child.
DOMElement * get_dom_child_by_key_tag(const DOMElement & parent, string child_tag_name)
  throw (exception);

//===============================================================================

// Tries to parse the specified string as a bool ('T' or 'F'). Throws an
// exception if it can't.
bool parse_xml_string_as_bool(const XMLCh * p_xml)
  throw (exception);

// Parse the XML string as the specified kind of number. Throw an exception if it
// can't be done...

int parse_xml_string_as_int(const XMLCh * p_xml)
  throw (exception);

unsigned int parse_xml_string_as_uint(const XMLCh * p_xml)
  throw (exception);

unsigned long parse_xml_string_as_long(const XMLCh * p_xml)
  throw (exception);

unsigned long parse_xml_string_as_ulong(const XMLCh * p_xml)
  throw (exception);

// Note: This won't necessarily discover overflow, because I had to implement
// this on my own and didn't want to take the time to make a perfect bad-parse
// detector. -cjc
unsigned long long parse_xml_string_as_ulong_long(const XMLCh * p_xml)
  throw (exception);

double parse_xml_string_as_double(const XMLCh * p_xml)
  throw (exception);

//===============================================================================

// Creates an XML subtree, rooted at 'elem', representing the content of the
// vector.
void save_int_vector_to_xml(const vector<int> & v, DOMDocument & doc, DOMElement & elem)
  throw (exception);

// Given an element populated by saveIntVectorToXml, this will append 'v' with
// the saved vector's elements (in proper order).
void load_int_vector_from_xml(vector<int> & v, DOMElement & elem)
  throw (exception);

//===============================================================================

// Creates an XML subtree, rooted at 'elem', representing the content of the
// timeval.
void save_timeval_to_xml(const timeval & tv, DOMDocument & doc, DOMElement & elem)
  throw (exception);

// Given an element populated by saveTimevalToXml, this will populate 'tv' with
// the saved timeval's fields.
void load_timeval_from_xml(timeval & tv, DOMElement & elem)
  throw (exception);

//===============================================================================

// Numeric type promotion + normal overloading keeps us out of trouble here, so
// we can avoid user-specified templates. Thus, the proper version of this
// function will always be called for parameters like size_t and ssize_t.

void set_dom_attribute(DOMElement & elem, string attribute_name, bool new_value)
  throw (exception);

void set_dom_attribute(DOMElement & elem, string attribute_name, int new_value)
  throw (exception);

void set_dom_attribute(DOMElement & elem, string attribute_name, unsigned int new_value)
  throw (exception);

void set_dom_attribute(DOMElement & elem, string attribute_name, long new_value)
  throw (exception);

void set_dom_attribute(DOMElement & elem, string attribute_name, unsigned long new_value)
  throw (exception);

void set_dom_attribute(DOMElement & elem, string attribute_name, unsigned long long new_value)
  throw (exception);

// NOTE! This might produce a messed up XML file if 'newValue' has characters
// that are XML control characters. If/when we need to handle this, we can must
// an encoding scheme in this function and the corresponding 'getDomAttribute'
// function.
void set_dom_attribute(DOMElement & elem, string attribute_name, string new_value)
  throw (exception);

void set_dom_attribute(DOMElement & elem, string attribute_name, double new_value)
  throw (exception);



void get_dom_attribute(const DOMElement & elem, string attribute_name, bool & value)
  throw (exception);

void get_dom_attribute(const DOMElement & elem, string attribute_name, int & value)
  throw (exception);

void get_dom_attribute(const DOMElement & elem, string attribute_name, unsigned int & value)
  throw (exception);

void get_dom_attribute(const DOMElement & elem, string attribute_name, long & value)
  throw (exception);

void get_dom_attribute(const DOMElement & elem, string attribute_name, unsigned long & value)
  throw (exception);

void get_dom_attribute(const DOMElement & elem, string attribute_name, unsigned long long & value)
  throw (exception);

void get_dom_attribute(const DOMElement & elem, string attribute_name, string & new_value)
  throw (exception);

void get_dom_attribute(const DOMElement & elem, string attribute_name, double & new_value)
  throw (exception);


// Returns true if the attribute with the specified name is defined in the
// element, otherwise returns false.
bool is_dom_attribute_present(const DOMElement & elem, string attribute_name)
  throw (exception);




//*******************************************************************************
// Alternative approach to Xerces stuff that Jon Salz from MIT wrote.  Should
// peacefully coexist with the stuff above, and have only partial overlap of
// functionality...
//*******************************************************************************

// Utility functions for XML parsing.  (There's some overlap with xercesDomUtil.H.)

enum xml_attr_req
{
    ATTR_NOT_REQUIRED = 0,
    ATTR_REQUIRED = 1,
    ATTR_NON_EMPTY = 2
};

// Parse a whole XML file.  (Differs from readDomDocFromFile in that
// you don't need to create your own DOMImplementationLS.)
ptr<DOMDocument> parse_xml_file(string filename) throw (AuroraBadXmlException);

// Parse a string as an XML document.
ptr<DOMDocument> parseXmlString(string data) throw (AuroraBadXmlException);

// Parse data as an XML document.
ptr<DOMDocument> parseXmlString(const void *data, unsigned int length) throw (AuroraBadXmlException);

// Return an attribute of an elemnt.  If req is ATTR_REQUIRED and the
// attribute is not specified, or if req is ATTR_NON_EMPTY and the the
// attribute is not specified or empty (""), throws an exception.
string xml_attribute(const DOMElement *element, string name,
                    xml_attr_req req = ATTR_NOT_REQUIRED) throw(AuroraBadEntityException);

// Return an attribute of an element, or def if the attribute is not
// present.
string xml_attribute(const DOMElement *element, string name, string def) throw (AuroraBadEntityException);

// Throw an exception unless the given tag has a particular name.
// (Basically an assert.)
void xml_expect_tag(const DOMElement *element, string name) throw (AuroraBadEntityException);

// Reads an attribute value into the "value" argument.  Throws an
// exception if (a) a value is specified but has an invalid format, or
// (b) req is ATTR_REQUIRED but the attribute value is not specified.
//
// You can specify an optional value by setting value before calling
// this method.
template <class T>
inline bool xml_typed_attribute(const DOMElement *element, string name, T& value,
                              xml_attr_req req = ATTR_NOT_REQUIRED) throw (AuroraBadEntityException)
{
    XMLCh xname[64];
    XMLString::transcode(name.c_str(), xname, sizeof xname - 1);

    const DOMAttr *attr = element->getAttributeNode(xname);
    if (!attr)
    {
        if (req != ATTR_NOT_REQUIRED)
        {
            Throw(AuroraBadEntityException,
                  "<" + to_string(element->getTagName()) + "> lacks "
                  "required attribute \"" + name + "\"");
        }
        return false;
    }

    // It's provided; coerce it
    string val = to_string(attr->getValue());

    istringstream i(val);
    if (i >> value)
        return true;

    Throw(AuroraBadEntityException, "Invalid format for <" +
          to_string(element->getTagName()) + " " + name + "=\"" + val + "\">");
}

// Escapes the &<>"' characters in a string.
string xml_escape(string in);

// Returns the only tag within parent (or zero if none).  Throws an exception
// if there more than one tag, or if there are no tags and required is true.
DOMElement* xml_only_tag(const DOMElement* parent, bool required = true)
    throw (AuroraException);

// Returns the only tag with a particular name within parent (or zero
// if none).  Throws an exception if there more than one tag, or if
// there are no tags and required is true.
DOMElement* xml_only_tag(const DOMElement* parent, string tag, bool required = true)
    throw (AuroraException);

// Returns immediate subnodes with a given tag.  Clears children first.
void xml_child_elements(vector<DOMElement*>& children, const DOMElement* parent, string tag = string());

// Serialize a DOM node as text.
string xml_serialize(const DOMNode *node);

BOREALIS_NAMESPACE_END;

#endif
