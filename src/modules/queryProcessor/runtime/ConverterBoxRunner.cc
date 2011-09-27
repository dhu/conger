
#include "ConverterBoxRunner.h"

#include "AuroraNode.h"
#include "Registry.h"

BOREALIS_NAMESPACE_BEGIN

ConverterBoxRunner::ConverterBoxRunner(AuroraNode  &node)
    throw (AuroraException)
    : _node(node),
      _thread(wrap(_loop, &IOEventLoop::run))
{
    INFO << "Available converter boxes: " << Registry<ConverterBox>::get();

/***********************************
    const DOMElement *server = conf->get_module_tag("server", ATTR_NOT_REQUIRED);
    if (!server) return;

    for (DOMNode *enode = server->getFirstChild(); enode; enode = enode->getNextSibling())
    {
        if (enode->getNodeType() != DOMNode::ELEMENT_NODE)
            continue;

        DOMElement *elt = static_cast<DOMElement *>(enode);
        if (to_string(elt->getTagName()) != "converter-box")
            continue;

        string clazz = xml_attribute(elt, "class", ATTR_NON_EMPTY);
        string stream_name = xml_attribute(elt, "stream", ATTR_NON_EMPTY);

        unsigned int port;
        xml_typed_attribute(elt, "port", port, ATTR_REQUIRED);

        const Registry<ConverterBox>::Info *info =
            Registry<ConverterBox>::get().forKey(clazz);

        if (!info)
        {
            WARN << "Unknown converter box class \"" << clazz << "\"";
            continue;
        }

        ptr<ConverterBox> box( info->instantiate() );
        // yna -- unsafe, thus deprecated. What are these ConverterBoxes???
        //ptr<Stream> stream = node.create_stream(stream_name, box->get_schema());
        //box->set_stream(stream);
        //INFO << "Creating converter box " << clazz << " on port " << port << " for stream " << stream_name;

        box_acceptor *acceptor = new box_acceptor(_loop, port, box);
        if (!*acceptor)
        {
            WARN << "Unable to create " << clazz << " on port " << port << ": "
                 << acceptor->stat();
            delete acceptor;
            continue;
        }

        // Delete acceptor automatically when loop dies
        acceptor->set_owned(false);
    }
*****************************************/
}


ConverterBoxRunner::~ConverterBoxRunner()
{
}

void ConverterBoxRunner::start()
{
    INFO << "Starting ConverterBoxRunner";
    _thread.start();
}

void ConverterBoxRunner::terminate()
{
    _loop.terminate();
    _thread.join();
}

int ConverterBoxRunner::BoxHandler::incoming_data(constbuf buf)
{
    try
    {
        return _box->handle_data(buf);
    } catch (AuroraException& e)
    {
        INFO << "In converter box " << typeid(*(_box.get())) << ": " << e;
        this->close();
        return 0;
    }
}

BOREALIS_NAMESPACE_END;
