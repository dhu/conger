#ifndef NMSTL_UTIL_H
#define NMSTL_UTIL_H



#include <NMSTL/internal.h>
#include <NMSTL/ptr>
#include <NMSTL/text>
#include <NMSTL/thread>
#include <NMSTL/cthread>

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/dom/DOMException.hpp>

#ifdef XERCES_CPP_NAMESPACE
using namespace xercesc;
#endif

NMSTL_NAMESPACE_BEGIN;

string to_string(const XMLException& e);
string to_string(const DOMException& e);

string to_string(const XMLCh* ch);

NMSTL_NAMESPACE_END;

//#define memcpy __builtin_memcpy

#endif
