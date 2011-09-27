#include "LoadOffer.h"

MEDUSA_NAMESPACE_BEGIN

long LoadOffer::s_counter = 0;

string
LoadOffer::repr() const
{

    string out(as_string());
    out << " Offered boxes: ";
    for ( vector<Stats>::const_iterator i = _boxes.begin();
          i != _boxes.end(); ++i)
        out << "-" << (*i);
    out << " Accepted boxes: ";
    for ( vector<Stats>::const_iterator j = _accepted_boxes.begin();
          j != _accepted_boxes.end(); ++j)
        out << "-" << (*j);
    out << " Offered price: " << _offer;
    out << " Counter-offered price: " << _counter_offer;
    return out;
}


MEDUSA_NAMESPACE_END
