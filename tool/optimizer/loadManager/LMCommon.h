#ifndef LMCommon_h
#define LMCommon_h

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <stdio.h>
#include <algorithm>

//#define SIMULATION

#ifndef SIMULATION
  #include <common.h>
#else
  #include "Common.h"
  #define BOREALIS_NAMESPACE_BEGIN namespace std {}; namespace DASIM { using namespace ::std;
  #define BOREALIS_NAMESPACE_END };
#endif


BOREALIS_NAMESPACE_BEGIN

#ifdef SIMULATION
  typedef int IdType;
#else
  typedef string IdType;
#endif

typedef vector<IdType> IdVect;
typedef set<IdType> IdSet; 
typedef map<IdType, IdType> IdMap; 
typedef vector<set<IdType> > IdSetVect; 
typedef pair<IdType, IdType> IdPair;
typedef vector<IdPair> IdPairVect;
typedef pair<IdType, double> IdValuePair;
typedef vector<IdValuePair> IdValuePairVect;
typedef multimap<double, IdType> DescendingIds;
typedef multimap<double, IdSet, greater<double> > DescendingIdSet;
typedef multimap<double, IdSet> AscendingIdSet;
typedef map<IdPair, double> IdPairValueMap;

inline bool less_than(const IdValuePair& p1, const IdValuePair& p2) 
{
    return p1.second < p2.second;
}
inline bool greater_than(const IdValuePair& p1, const IdValuePair& p2) 
{
    return p1.second > p2.second;
}


typedef int PortId;  // The type for ports in Borealis and simulator

inline IdType get_selectivity_id(
        const IdType& box_id, 
        const PortId output_port_id)
{
#ifdef SIMULATION
  return box_id * 10000 + output_port_id;
#else
  ostringstream tmp;
  tmp << box_id << " : " << output_port_id;
  return tmp.str();
#endif
}

inline IdType get_box_id(const IdType& selectivity_id)
{
#ifdef SIMULATION
  return (IdType)(selectivity_id / 10000);
#else
  int index = selectivity_id.find(" :");
  return selectivity_id.substr(0, index);
#endif
}

inline IdType get_arc_id(
    const IdType& stream_id,
    const IdType& input_box_id,
    const IdType& output_box_id)
{
#ifdef SIMULATION
    return stream_id;
#else
    ostringstream tmp;
    tmp << stream_id << "(" << input_box_id << "->" << output_box_id << ")";
    return tmp.str();
#endif
}

inline IdType get_stream_id(const IdType& arc_id)
{
#ifdef SIMULATION
    return arc_id;
#else
    int index = arc_id.find("(");
    return arc_id.substr(0, index);
#endif
}

inline ostream& operator << (ostream& os, const IdMap& idmap)
{
    bool first = true;
    os << "(";
    for (IdMap::const_iterator it = idmap.begin(); it != idmap.end(); ++it)
    {
        if (first)
          first = false;
        else
          os << " ";
        os << "<" << it->first << ", " << it->second << ">";
    }
    os << ")";
    return os;
}

inline ostream& operator << (ostream& os, const IdSet& idset)
{
    bool first = true;
    os << "(";
    for (IdSet::const_iterator it = idset.begin(); it != idset.end(); ++it)
    {
        if (first)
          first = false;
        else
          os << " ";
        os << *it;
    }
    os << ")";
    return os;
}

inline ostream& operator << (ostream& os, const IdVect& ids)
{
    bool first = true;
    os << "(";
    for (IdVect::const_iterator it = ids.begin(); it != ids.end(); ++it)
    {
        if (first)
          first = false;
        else
          os << " ";
        os << *it;
    }
    os << ")";
    return os;
}


inline ostream& operator << (ostream& os, const IdPair& idpair)
{
    os << "(" << idpair.first << ", " << idpair.second << ")"; return os;
}

inline ostream& operator << (ostream& os, const IdPairVect& id_pairs)
{
    os << "{";
    for (unsigned int i = 0; i < id_pairs.size(); ++i) os << id_pairs[i];
    os << "}";
    return os;
}

inline ostream& operator << (ostream& os, const IdValuePairVect& id_value_pairs)
{
    os << "{";
    for (unsigned int i = 0; i < id_value_pairs.size(); ++i) 
    {
        os << "(" << id_value_pairs[i].first << ", "
           << id_value_pairs[i].second << ") ";
    }
    os << "}";
    return os;
}

inline ostream& operator << (ostream& os, const DescendingIdSet& id_set)
{
    os << "{";
    for (DescendingIdSet::const_iterator it = id_set.begin();
            it != id_set.end();
            ++it)
    {
        os << "<" << it->first << ", "
           << it->second << "> ";
    }
    os << "}";
    return os;
}

inline ostream& operator << (ostream& os, const AscendingIdSet& id_set)
{
    os << "{";
    for (AscendingIdSet::const_iterator it = id_set.begin();
            it != id_set.end();
            ++it)
    {
        os << "<" << it->first << ", "
           << it->second << "> ";
    }
    os << "}";
    return os;
}

inline ostream& operator << (ostream& os, const IdPairValueMap& id_pair_value_map)
{
    os << "{";
    for (IdPairValueMap::const_iterator it = id_pair_value_map.begin();
            it != id_pair_value_map.end();
            ++it)
    {
        os << "<" << it->first << ", "
           << it->second << "> ";
    }
    os << "}";
    return os;
}

BOREALIS_NAMESPACE_END

#endif
