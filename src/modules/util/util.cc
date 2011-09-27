#include <util.h>

#include <dirent.h>

#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
// Returns true iff the specified directory exists.
// The path may contain a trailing slash without consequence.
// If the path exists but the specified file is not a directory,
// this still returns false.
//
bool Util::dir_exists(string  dir_path)
                throw(AuroraException)
{
    string a_path;
    struct stat file_stat;
//..............................................................................


    if ( dir_path.length() == 0 )
    {   throw SmIllegalParameterValueException(
                  __FILE__, __LINE__, "dirPath had a length of 0" );
    }

    // Ensure the directory name doesn't end in a '/'...

    if (( dir_path.length() > 1 )  &&
        ( dir_path.at( dir_path.length() - 1 ) == '/' ))
    {
        a_path = dir_path.substr( 0, dir_path.length() - 1 );
    }
    else
    {   a_path = dir_path;
    }

    int rc = lstat( a_path.c_str(), &file_stat );

    if ( rc == 0 )
    {   if ( S_ISDIR( file_stat.st_mode ))
        {   return( true );
        }
        else
        {   return( false );
        }
    }

    switch( errno )
    {
      case EBADF:
        Throw( AuroraException,
               "stat returned EBADF, but only 'fstat' should return that code" );

      case ENOENT:
        return( false );

      case ENOTDIR:
        return( false );

      case ELOOP:
        Throw( AuroraException,
               "stat returned ELOOP (Too many symbolic links traversed)." );

      case EFAULT:
        Throw( AuroraException, "stat returned EFAULT" );

      case EACCES:
        Throw( AuroraException, "stat returned ECCES (Perimssion denied)." );

      case ENOMEM:
        Throw( AuroraException, "stat returned ENOMEM" );

      case ENAMETOOLONG:
        Throw( AuroraException, "stat returned ENAMETOOLONG" );
    }

    // Control should never reach here, so this is just to shut-up the compiler's warnings...
    assert( false );
    return( false );
}



////////////////////////////////////////////////////////////////////////////////
//
// Returns true iff any object exists with the specified pathname.
//
bool Util::file_exists(string  file_path)
                 throw(AuroraException)
{
    string a_path;
    struct stat file_stat;
//..............................................................................


    if ( file_path.length() == 0 )
    {   Throw( AuroraException, "filePath had a length of 0" );
    }

    // Ensure the pathname doesn't end in a '/'...
    if (( file_path.length() > 1 )  &&
        ( file_path.at( file_path.length() - 1 ) == '/' ))
    {
        a_path = file_path.substr(0, file_path.length()-1);
    }
    else
    {   a_path = file_path;
    }

    int rc = lstat( a_path.c_str(), &file_stat );

    if ( rc == 0 )
    {   return( true );
    }

    switch( errno )
    {
      case EBADF:
        Throw( AuroraException,
               "stat returned EBADF, but only 'fstat' should return that code" );

      case ENOENT:
        return( false );

      case ENOTDIR:
        return( false );

      case ELOOP:
        Throw( AuroraException,
               "stat returned ELOOP (Too many symbolic links traversed)." );

      case EFAULT:
        Throw( AuroraException, "stat returned EFAULT" );

      case EACCES:
        Throw( AuroraException, "stat returned ECCES (Perimssion denied)." );

      case ENOMEM:
        Throw( AuroraException, "stat returned ENOMEM" );

      case ENAMETOOLONG:
        Throw( AuroraException, "stat returned ENAMETOOLONG" );
    }

    // Control should never reach here, so this is just to shut-up the compiler's warnings...
    assert(false);
    return false;
}



////////////////////////////////////////////////////////////////////////////////
//
// Returns the full path name of a file.
//
string  Util::full_path(// A local file or directory.
                        // If empty the working directory is returned
                        // with a trailing slash.
                        //
                        string  local)
{
    char  cwd[PATH_MAX + 1];
//..............................................................................


    if (getcwd(cwd, PATH_MAX) == NULL)
    {   DEBUG << "Could not get the working directory.";
        return("");
    }

    return(to_string(cwd) + "/" + local);
}



////////////////////////////////////////////////////////////////////////////////
//
// Determine the dotted IP address of a host machine.
//
string  Util::get_host_address(// Name of the host machine; empty for local.
                               string   host)
{
    struct hostent  *host_ent;
    char           **host_list;
    char             hostname[128];
    struct in_addr  *host_number;
    char            *host_ip;
//
// The default port is a static assignment for now.
// It could be assigned by scanning available channels???
//..............................................................................


    if (( host == "127.0.0.1" )  || ( host.empty() )  ||
        ( host == "localhost" ))
    {
        if ( gethostname( hostname, 80 ))           // IF no local host name,
        {   if ( host.empty() )                     //    IF none given,
            {   strcpy( hostname, "127.0.0.1" );    //       Default IP.
            }
            else                                    //    ELSE
            {   strcpy( hostname, host.c_str() );   //       Given name.
            }                                       //    END IF
        }                                           // END IF
    }
    else
    {   strcpy( hostname, host.c_str() );
    }

    //DEBUG << "host=" << to_string( hostname );
    host_ent = gethostbyname( hostname );

    if ( host_ent )
    {   host_list = host_ent->h_addr_list;

        while (( host_number = (struct in_addr *)*host_list++ ))
        {   host_ip = inet_ntoa( *host_number );

            //DEBUG  << "hit " << to_string( host_ip );
            break;
        }
    }

    // Assert host_ip???
    return( to_string( host_ip ));
}



////////////////////////////////////////////////////////////////////////////////
//
// Compose a complete cononical endpoint:  <dotted ip address>:<port>
//
string  Util::form_endpoint(// An enpoint as:  [<host>][:[<port>]]
                            string   endpoint,

                            // A default port number.
                            uint16   port)
{
    string             form;
    string::size_type  index;
//
// The default port is a static assignment for now.
// It could be assigned by scanning available channels???
//..............................................................................


    form  = endpoint;
    index = endpoint.find(":", 0);            // Check for an optional port.

    if (index != string::npos)
    {   form = form.erase(index);             // Trim off the port.
    }

    form = get_host_address(form);            // Get the dotted host address.

    if ((index == string::npos)  ||           // IF no colon or no port,
        (index == endpoint.size() - 1))
    {
        form << ":" << port;                  // Add the default port number.
    }
    else
    {   form += endpoint.substr(index, 999);  // Use the given port number.
    }

    DEBUG << "Complete address is " << form;

    return  form;
}



////////////////////////////////////////////////////////////////////////////////
//
// Adgust the port number in a network address.
//
InetAddress  Util::fudge_port(// A network address to adjust.
                              InetAddress    endpoint,

                              // Port offset.
                              int16          fudge)
{
    string     host;
    uint16     port;
//
//..............................................................................


    host = endpoint.get_host();
    port = (uint16)(endpoint.get_port()) + fudge;

    return  InetAddress(host, port);
}


BOREALIS_NAMESPACE_END
