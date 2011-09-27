#ifndef LOCK_QBOX_H
#define LOCK_QBOX_H

#include "QBox.h"
#include <vector>

#include <ext/hash_map>

BOREALIS_NAMESPACE_BEGIN

class LockQBox : public QBox
{
  public:
    ~LockQBox();

  private:
    ptr<LockSet> _lock_set;
    ptr<Expression> _key_expr;
    typedef __gnu_cxx::hash_map<int, list<string> > PendingMap;
    PendingMap _pending_tuples;
    EvalContext _ctxt;
    size_t _tuple_size;

  protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);
};

BOREALIS_NAMESPACE_END

template<typename T, typename U>
inline std::string to_string(const __gnu_cxx::hash_map<T,U>& m)
{
    std::string out = "{";

    for (typename __gnu_cxx::hash_map<T, U>::const_iterator i = m.begin(); i != m.end(); ++i)
    {
        if (i != m.begin()) out.append("; ");
        out.append(NMSTL::to_string(i->first)).append("->").append(NMSTL::to_string(i->second));
    }

    out.append("}");
    return out;
}

#endif
