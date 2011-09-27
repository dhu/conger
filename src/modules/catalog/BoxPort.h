#ifndef  BOXPORT_H
#define  BOXPORT_H

#include  "common.h"

BOREALIS_NAMESPACE_BEGIN

class CatalogBox;

/// Stream access to an input or output port on a box.
///
class BoxPort
{
  public:

    BoxPort() : _box(0), _port(0) {};

    // BoxPort(const BoxPort &bp);

    void         shallow_copy(const BoxPort &bp);

    CatalogBox   *get_port_box() const;

    uint16       get_port() const;

    void         set_box_port(CatalogBox *box, uint16 port);

    void         set_box(CatalogBox *box);

    string       as_string() const;

    NMSTL_SIMPLY_SERIALIZABLE(BoxPort, << _port);

  private:

    CatalogBox  *_box; // Box containing the port.

    // Note: BUT Aurora expects to use these as direct vector indices.
    // Changed to be zero-based -- yna.
    uint16      _port; // Zero-based index in box inputs or outputs.
};

BOREALIS_NAMESPACE_END

#endif                       // BOXPORT_H
