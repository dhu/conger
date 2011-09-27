#ifndef CONVERTER_BOX_RUNNER_H
#define CONVERTER_BOX_RUNNER_H

#include <NMSTL/netioevent>

#include "ConverterBox.h"

BOREALIS_NAMESPACE_BEGIN;

class AuroraNode;

class ConverterBoxRunner
{
  public:
    ConverterBoxRunner(AuroraNode& node) throw (AuroraException);
    ~ConverterBoxRunner();

    void start();

    // Terminate and join
    void terminate();

  private:
    class BoxHandler : public NetHandler<>
    {
      public:
        BoxHandler(IOEventLoop &loop, IOHandle h, ptr<ConverterBox> box) :
            NetHandler<>(loop, h),
            _box(box)
        {
        }

        virtual int incoming_data(constbuf buf);

      private:
        ptr<ConverterBox> _box;
        Stream *_stream;
    };

    AuroraNode& _node;
    IOEventLoop _loop;
    CThread _thread;

    typedef TCPAcceptor<BoxHandler, ptr<ConverterBox> > box_acceptor;
};

BOREALIS_NAMESPACE_END;

#endif
