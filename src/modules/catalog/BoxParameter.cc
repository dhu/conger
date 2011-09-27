
#include "BoxParameter.h"
#include "XmlTempString.h"

BOREALIS_NAMESPACE_BEGIN;

// Copy constructor.
//
BoxParameter::BoxParameter(const BoxParameter &bp)
{
    const ParameterMap *pm = bp.get_parameter_map_const();

    ParameterMap::const_iterator i;

    for (i = pm->begin(); i != pm->end(); i++)
    {
        _parameter_map[i->first] = i->second;
    }
}


// Set the map to the argument.
//
void BoxParameter::set_parameter_map(const ParameterMap *pm)
{
    if (pm)
    {
        ParameterMap::const_iterator i;

        for (i = pm->begin(); i != pm->end(); i++)
        {
            _parameter_map[i->first] = i->second;
        }
    }
}


// Set a parameter value.
//
void BoxParameter::set_parameter(string parameter, string value)
{
    _parameter_map[parameter] = value;
}


// Get a value for a box parameter.
//
string BoxParameter::get_parameter(string parameter)
{
    return _parameter_map[parameter];
}

////////////////////////////////////////////////////////////////////////////////
//
BoxParameter::BoxParameter(const DOMElement* element)
{
    XmlTempString xparam("parameter");
    DOMNodeList *param_nodes = element->getElementsByTagName(xparam.to_xml());

    for( uint32 i = 0; i < param_nodes->getLength(); ++i)
    {
        try
        {
            const DOMElement* param_element = static_cast<const DOMElement*>( param_nodes->item(i) );
            string param_name = xml_attribute(param_element, "name", ATTR_REQUIRED);
            string param_value = xml_attribute(param_element, "value", ATTR_REQUIRED);
            NOTICE << "param : " << param_name << " " << param_value;
            _parameter_map[param_name] = param_value;
        } catch (AuroraException ae)
        {
        continue;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
string  BoxParameter::parameter(
                                  string    name,
                                  required  required
                               ) const
                          throw( AuroraBadEntityException )
{
    map<string, string>::const_iterator i;
//..............................................................................


    i = _parameter_map.find( name );

    if (( required == REQUIRED )  &&  ( i == _parameter_map.end() ) ||
        (( required == NON_EMPTY )  &&
         (( i == _parameter_map.end() )  || ( i->second.empty() ))))
    {
        Throw( AuroraBadEntityException,
               "Missing required attribute \"" + name + "\"" );
    }

    if ( i == _parameter_map.end() )
    {   return string();
    }
    else
    {   return i->second;
    }
}


////////////////////////////////////////////////////////////////////////////////
//
string  BoxParameter::parameter(
                                   string  name,
                                   string  default_value
                               ) const
{
    map<string, string>::const_iterator  i;
//..............................................................................


    i = _parameter_map.find( name );

    if ( i == _parameter_map.end() )
    {   return( default_value );
    }

    return( i->second );
}



////////////////////////////////////////////////////////////////////////////////
//
// Returns parameter map in XML.
//
string BoxParameter::get_parameter_xml()
{
    string pm_xml; 

    ParameterMap::const_iterator iter;
//..............................................................................


    for (iter = _parameter_map.begin(); iter != _parameter_map.end(); iter++)
    {
        pm_xml += " <parameter name=\"" + iter->first + 
                  "\" value=\"" + iter->second +"\" />";
    }

    return pm_xml;
}


BOREALIS_NAMESPACE_END
