#ifndef MEDUSA_LOADOFFER_H
#define MEDUSA_LOADOFFER_H

#include "common.h"
#include "MedusaID.h"
#include "Stats.h"

MEDUSA_NAMESPACE_BEGIN

#define REJECT -1.0

class LoadOffer
{
 public:
    LoadOffer(MedusaID current_owner_id = MedusaID(""),
              vector<Stats> boxes = vector<Stats>(),
              double price = REJECT)
        : _current_owner_id(current_owner_id),
        _boxes(boxes),
        _accepted_boxes(),
        _offer(price),
        _counter_offer(REJECT)
        {

        _transaction_nb = next_transaction_nb();

    }

    static long next_transaction_nb() { return s_counter++; }
    long _transaction_nb;

    MedusaID _current_owner_id;
    vector<Stats> _boxes;
    vector<Stats> _accepted_boxes;
    double _offer;
    double _counter_offer;

    NMSTL_SIMPLY_SERIALIZABLE(LoadOffer, << _current_owner_id << _boxes
                              << _accepted_boxes << _offer << _counter_offer);

    bool operator<(const LoadOffer& l) const
    {
        return (this->_offer < l._offer);
    }
    static bool less_than(const ptr<LoadOffer> l1, const ptr<LoadOffer> l2)
    {
        return *l1 < *l2;
    }

    string as_string() const
    {
        string out("Offer: ");
        out << _transaction_nb << " from " << _current_owner_id;
        return out;
    }
    string repr() const;


 protected:
    static long s_counter;

};


NMSTL_TO_STRING(LoadOffer);

MEDUSA_NAMESPACE_END

#endif
