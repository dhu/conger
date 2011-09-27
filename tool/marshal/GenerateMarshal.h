#ifndef GENERATEMARSHAL_H
#define GENERATEMARSHAL_H

#include  "MarshalDiagram.h"


#define uppercase(s)   transform( s.begin(), s.end(),                 \
                                  s.begin(), (int(*)(int))toupper );


#define BOX_TOP     "////////////////////////////////////////////////////////////////////////////////\n//\n"
#define BOX_BOTTOM  "//..............................................................................\n\n\n"



BOREALIS_NAMESPACE_BEGIN

class GenerateMarshal : public MarshalDiagram
{
  public:

    /// Generate marshaling code from an xml file.
    ///
    Status  generate_marshal(/// Relative path of the XML source.
                             string  xml_file );


  private:

    /// Generate marshaling code for a single client.
    ///
    Status  generate_client();


    /// Create a header file for the marshaling code.
    ///
    Status  generate_header(/// Relative path of the header file.
                            string  file );


    /// Create methods for the marshaling code.
    ///
    Status  generate_body(/// Relative path of the target file.
                          string  file);


    ///  Write the top common portion of the marshaling header file.
    ///
    void  header_top(/// An open header file output stream.
                     ofstream  &header);


    ///  Write marshaling header file declarations for all input streams.
    ///
    void  header_publish(/// An open header file output stream.
                         ofstream  &header);


    ///  Write marshaling header file declarations for all output streams.
    ///
    void  header_subscribe(/// An open header file output stream.
                           ofstream  &header);


    ///  Write marshaling header file field declarations for a schema.
    ///
    void  schema_structure(/// An open header file output stream.
                           ofstream  &header,

                           /// Schema containing the fields.
                           CatalogSchema  *schema);


    ///  Write the bottom common portion of the marshaling header file.
    ///
    void  header_bottom(/// An open method file output stream.
                        ofstream  &header);


    ///  Write the top common portion of the marshaling method file.
    ///
    Status  body_top(/// An open method file output stream.
                     ofstream  &body);


    ///  Write the method body to create a client and connect the first node.
    ///
    void  open_client(/// An open method file output stream.
                      ofstream   &body);


    ///  Write the method body to listen for outputs.
    ///
    void  listen_stream(/// An open method file output stream.
                        ofstream  &body );


    ///  Write the method body to connect inputs..
    ///
    void  connect_stream(/// An open method file output stream.
                         ofstream  &body,

                         /// If True, generate an else statement.
                         Boolean    else_clause = False);


    ///  Write marshaling methods to connect each input on a client.
    ///
    void  body_publish_client(/// An open method file output stream.
                              ofstream  &header );


    ///  Write marshaling methods to connect a single published input.
    ///
    void  body_publish(/// An open method file output stream.
                       ofstream  &header,

                       /// Stream name (proper case).
                       string     name,

                       /// Publish declaration.
                       vector<string>   *node);


    ///  Write marshaling methods to subscribe to each output.
    ///
    void  body_subscribe(/// An open method file output stream.
                         ofstream  &header );


    ///  Write the bottom common portion of the marshaling method file.
    ///
    void  body_bottom(/// An open method file output stream.
                      ofstream  &body );


    /// Check if ip_argument specifies only port or both ip and port
    /// If there's only a port, assigne default ip
    /// Parse argument and assign results to the ip and port parameters
    ///
    void stream_address(/// Stream endpoint:  [<ip address>:]<port>
                        string   endpoint,

                        /// Node to which the stream is deployed.
                        string  &ip,

                        /// Port used to access the stream.
                        string  &port);


    /// Tuples that have already been generated.
    ///
    set<string>   _tuple;


    /// Subscriptions from the current client being generated.
    ///
    set<CatalogSubscription *>   _subscribe;


    /// Relative paths of the XML sources separated by a single space.
    ///
    string          _xml_file;


    /// Endpoint for the current client's listener port.
    ///
    string          _monitor;
};


BOREALIS_NAMESPACE_END
#endif                       // GENERATEMARSHAL_H
