#include  "GenerateMarshal.h"


BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
// Generate marshaling code from xml files for multiple clients.
//
Status  GenerateMarshal::generate_marshal(/// Relative paths of XML sources.
                                          string  xml_file)
{
    Status     status = true;

    map<string, string>::iterator  client;
//..............................................................................


    DEBUG << "Generating " << _generate_client.size() << " clients.";
    _xml_file = xml_file;

    for (client  = _generate_client.begin();
         client != _generate_client.end(); client++)
    {
         // Prefix for the current client name.
         //
         _prefix  = client->first;

         // Endpoint for the client's listener port.
         //
         _monitor = client->second;
         DEBUG  << "_monitor=" << _monitor;

         status = generate_client();
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
// Generate marshaling code for a single client.
//
Status  GenerateMarshal::generate_client()
{
    string    endpoint;

    StreamLinkMap::iterator     subscriber;
    SubscriptionMap            *subscription;
    SubscriptionMap::iterator   output;
    //map<string, GeneratePublish>::iterator  publish;
//
//..............................................................................


    /***********************
    for (publish  = _generate_publish.begin();
         publish != _generate_publish.begin(); publish++)
    {
        if (publish->second._prefix.empty())
        {   DEBUG << "NYI:  map(sink box node->pub)";

             //do over sinks
             //      map(sink box node->pub)
             //end do
        }
        else
        {  // map(node->pub)
        }
    }
    ****************************/

    // Scan subscriptions and save those for the client being generated.
    //
    _subscribe.clear();

    for (subscriber  = _overlay._subscriber_map.begin();
         subscriber != _overlay._subscriber_map.end(); ++ subscriber)
    {
        subscription = &((*subscriber).second);

        for (output  = subscription->begin();
             output != subscription->end(); ++output)
        {
            endpoint = to_string(output->second.get_subscription_endpoint());
            DEBUG << "endpoint=" << endpoint << " _monitor=" << _monitor;

            if (endpoint == _monitor)
            {   _subscribe.insert(&output->second);
            }
        }
    }

    DEBUG << "There are " << _subscribe.size() << " subscriptions.";
    Status  status = generate_header(_prefix + "Marshal.h");

    if (status)
    {   status = generate_body(_prefix + "Marshal.cc");
    }

    return  status;
}

BOREALIS_NAMESPACE_END
