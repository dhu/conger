#include "GenerateMarshal.h"

using namespace Borealis;


////////////////////////////////  marshal.cc  //////////////////////////////////
//
//  Generate marshaling code for a Borealis network diagram.
//
//      marshal  [-l]  <xml file> ...
//
//  Writes <Program>Marshal.cc and <Program>Marshal.h in the working directory.
//  The file list has the relative path of source XML.
//  The Program name is extracted from the name of the first file,
//  but may be overridden using the Xml client prefix element.
//


//      -l  Override all host addresses with the local host (127.0.0.1).
//          This is used to test a distributed deployment on a single computer.
//
int main(int argc, const char *argv[])
{
    GenerateMarshal  diagram;

    string    xml_file;         // Relative path of the XML source.
    string    file_list = "";

    int       result    = 0;
    int       first     = 1;
    Boolean   deployed  = False;
    Status    status    = true;
//
//..............................................................................


if (argc < 2)
{   NOTICE <<  "USAGE:  marshal  [-l]  <xml file> ...";
    status = false;
    result = -1;
}
else if (to_string(argv[1]) == "-l")
{   if (argc < 3)
    {   NOTICE <<  "USAGE:  marshal  [-l]  <xml file> ...";
        result = -1;
        status = false;
    }

    NOTICE << "Deploying all components locally.";
    diagram.deploy_local();
    first++;
}

if (status)
{   for (int  i = first; i < argc; i++)
    {   xml_file = to_string(argv[i]);

        if (xml_file == "-l")
        {   ERROR << "The -l option must be first on the command line.";
            result = -1;
            status = false;
            break;
        }

        NOTICE << "marshal xml:  " << xml_file;

        // The first XML file name is used to create a default client name.
        //
        if (i == first)
        {   diagram.setDefaultPrefix(xml_file);
            file_list = xml_file;
        }
        else
        {   file_list = file_list + " " + xml_file;
        }

        status = diagram.parse_file(xml_file);

        if (status)
        {   // Walk the XML parse tree again to add extra diagram items.
            //
            status = diagram.parse_marshal();
        }

        if (!status)
        {   NOTICE << "Parse error:  " << status;
            result = -1;
            break;
        }

        if (!diagram.is_borealis_diagram())
        {   deployed = True;
        }
    }


    if ((status)  &&  (!deployed))
    {   status = diagram.generate_deployment();
    }

    if (status)
    {   status = diagram.infer_schema();

        if (!status)
        {   NOTICE << status;
            result = -2;
        }
    }

    if (status)
    {   status = diagram.generate_marshal(file_list);

        if ( !status )
        {   NOTICE << "Code generation error:  " << status;
            result = -3;
        }
    }
}

return  result;
}
