#ifndef EXISTS_QBOX_H
#define EXISTS_QBOX_H

#include "QBox.h"
#include "Table.h"
#include "SQLSelect.h"
#include "Tuple.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * This box is a quick hack that Linear Road needed and Eddie asked
 * for. No warrantee, and it will likely be ripped out when we stop
 * caring about Linear Road or transition Linear Road to more accepted
 * operators. Anyone using this who is not Linear Road is probably a
 * bad person. It should probably get moved to a plugin directory of
 * the LR demo or something. --tibbetts
 */

class ExistsQBox : public QBox
{
private:
    ptr<SQLSelect> _sql;
    Table *_table;
    ptr<SelectQueryHandle> _query_handle;

    void emit_tuple(const void *input_tuple, int exists);

protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);
};


BOREALIS_NAMESPACE_END

#endif                                 // EXISTS_QBOX_H
