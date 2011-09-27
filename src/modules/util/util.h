#ifndef UTIL_H
#define UTIL_H

#include <NMSTL/tcprpc>

#include "Exceptions.h"
#include "Name.h"


BOREALIS_NAMESPACE_BEGIN


class  Util
{
  public:

    /// Returns true iff the specified directory exists.
    /// If the path exists but the specified file is not a directory,
    /// this still returns false.
    ///
    static bool  dir_exists(/// The path may contain a trailing slash.
                            string  dir_path )
                      throw(AuroraException);


    /// Returns true iff any object exists with the specified pathname.
    ///
    static bool  file_exists(string  file_path)
                       throw(AuroraException);


    /// Returns the full path name of a file or an empty string if an error.
    ///
    static string  full_path(/// A local file or directory.
                             /// If empty the working directory is returned
                             /// with no trailing slash.
                             ///
                             string  local = "");


    /// Determine the dotted IP address of a host machine.
    ///
    static string  get_host_address(/// Name of the host machine; empty for local.
                                    string   host = "");


    /// Compose a complete cononical endpoint.
    ///
    static string  form_endpoint(/// An enpoint as:  [<ip address>][:<port>]
                                 string   endpoint,

                                 /// A default port number.
                                 uint16   port = 0);


    /// Adgust the port number in a network address.
    ///
    static InetAddress  fudge_port(// A network address to adjust.
                                   InetAddress   endpoint,

                                   // Port offset.
                                   int16         fudge);


    /// Construct the name used in the catalog for an object.
    /// Extend an object name to make it unique to each Borealis user.
    /// This functionality is not implemented, but the code base should
    /// use this so it is ready.
    ///
    static string  full_name(Name  name)
    {   return(to_string(name));
    }
};


/// Handle a string conversion for a uint8 differently from other types.
template <typename type>
class cast_into_string
{
  public:

    static  string  into_string(type  x)
    {   return(to_string(x));
    }
};


/// Handle a string conversion for a uint8 differently from other types.
template <>
class cast_into_string<int8>
{
  public:

    static  string  into_string(int8  x)
    {   return(to_string((int32)x));
    }
};


BOREALIS_NAMESPACE_END
#endif                                // UTIL_H
