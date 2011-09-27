#ifndef COMBINEQBOX_H
#define COMBINEQBOX_H

#include "QBox.h"
#include <ext/hash_map>

BOREALIS_NAMESPACE_BEGIN

using namespace __gnu_cxx;

class CombineQBox : public QBox
{
  protected:
    class StringHash
    {
      public:
        size_t operator() (const string& s) const
        {
            size_t v = 0;

            if ( s.size() == sizeof( size_t ))
            {   return( *(size_t*)s.data() );
            }

            for (string::const_iterator i = s.begin(); i != s.end(); ++i)
            {   v = 5*v + *i;
            }

            return v;
        }
    };

    void setup_impl() throw (AuroraException);

    static string bits_to_string(unsigned int value, int bits);

    void run_impl(QBoxInvocation& inv) throw (AuroraException);

  private:
    // for each input, the offset of the key field within that tuple
    vector<int> _key_field_offsets;

    unsigned int _key_field_size;
    DataType _key_field_type;

    // key: value of key field (i.e., of length _key_field_size)
    // value: (a) int value containing bitmask of which tuples we've seen
    //        (b) value of tuple from each input, concatenated in order
    hash_map<string, pair<int, string>, StringHash> _data;

    // the bitmask that means "we're done": (1 << numInputs) - 1
    int _final_bitmask;

    // for each input, the offset of the copy of the tuple within the _data value
    vector<int> _data_offsets;

    vector<ptr<Expression> > _expressions;
    ptr<EvalContext> _ectxt;
};

BOREALIS_NAMESPACE_END

#endif                     // COMBINEQBOX_H
