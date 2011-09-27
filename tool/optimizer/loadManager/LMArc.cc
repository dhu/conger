#include "LMArc.h"
#include "LMBox.h"

BOREALIS_NAMESPACE_BEGIN

ostream& operator<<(ostream& os, const LMArc& arc)
{
    os << "("
          << arc._id << " | ";

    if (arc._source_box == 0)
        os << "_,_";
    else
        os << arc._source_box->get_id() << "," << arc._source_port;

    os << " -> ";

    if (arc._destination_box == 0)
        os << "_,_;";
    else
        os << arc._destination_box->get_id() << "," << arc._destination_port;

    os << ")";
    return os;
}

ostream& operator<<(ostream& os, const LMArcVect& arcs)
{
    for (LMArcVect::const_iterator ait = arcs.begin();
              ait != arcs.end();
              ++ait)
    {
        os << *ait << endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const LMArcMap& arcs)
{
    for (LMArcMap::const_iterator ait = arcs.begin();
              ait != arcs.end();
              ++ait)
    {
        os << *(ait->second) << endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const LMArcSet& arcs)
{
    for (LMArcSet::const_iterator ait = arcs.begin();
              ait != arcs.end();
              ++ait)
    {
        os << *ait << endl;
    }
    return os;
}

BOREALIS_NAMESPACE_END
