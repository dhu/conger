#ifndef BOXPARAMETER_H
#define BOXPARAMETER_H

#include "Exceptions.h"
#include "XmlTempString.h"
#include "xercesDomUtil.h"

BOREALIS_NAMESPACE_BEGIN


/// A set of parameters named by strings.
///
class BoxParameter
{
  public:
    typedef   map<string, string >    ParameterMap;

    enum required
    {
        NOT_REQUIRED,
        REQUIRED,
        NON_EMPTY
    };

    BoxParameter() {}

    BoxParameter(const BoxParameter &bp);

    void set_parameter_map(const ParameterMap *pm);

    void set_parameter(string parameter, string value);

    string get_parameter(string parameter);

    // HACK
    // -- used for generating parameters for objects outside the main parsing.
    BoxParameter(const DOMElement* element);

    /// Returns true if there are no parameters.
    ///
    bool  empty() const
    {   return( _parameter_map.empty() );
    }

    /// Return a parameter.  If req is REQUIRED and the attribute
    /// is not specified, or if req is NON_EMPTY and the the
    /// attribute is not specified or empty (""), throws an exception.
    ///
    string  parameter( string    name,
                       required  required = NOT_REQUIRED
                     ) const
                throw( AuroraBadEntityException );


    /// Return a parameter value, or default if the parameter is not
    /// present.
    ///
    string  parameter( string  name,
                       string  default_value
                     ) const;


    /// Reads a parameter value into the "value" argument.  Throws an
    /// exception if (a) a value is specified but has an invalid
    /// format, or (b) req is REQUIRED but the attribute value is
    /// not specified.
    ///
    /// You can specify an optional value by setting value before
    /// calling this method.
    ///
    template <typename T>
    bool typed_parameter( string   name,
                              T  &value,
                       required   required = NOT_REQUIRED
                       ) const
                  throw( AuroraBadEntityException )
    {
        ParameterMap::const_iterator i = _parameter_map.find( name );

        if ( i == _parameter_map.end() )
        {   if ( required != NOT_REQUIRED )
            {   Throw( AuroraBadEntityException,
                       "Missing required attribute \"" + name + "\"" );
            }

            return( false );
        }

        // It's provided; coerce it
        //
        istringstream  iss( i->second );

        if ( iss >> value )
        {   return( true );
        }

        Throw( AuroraBadEntityException,
               "Invalid format for parameter named \"" + name + "\": \"" +
                          to_escaped_string(i->second) + "\"");
    }

    /// Returns a string representation of this parameter set.
    ///
    string as_string() const
    {   return( "Params" + to_string( _parameter_map ));
    }

    ParameterMap  *get_parameter_map()
    {   return( &_parameter_map );
    }

    const ParameterMap *get_parameter_map_const() const
    {
        return( &_parameter_map );
    }

    string get_parameter_xml();

    NMSTL_SIMPLY_SERIALIZABLE(BoxParameter, << _parameter_map);

  private:

    ParameterMap     _parameter_map;
};

BOREALIS_NAMESPACE_END

#endif                         // BOXPARAMETER_H
