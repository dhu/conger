#ifndef DELETE_QBOX_H_
#define DELETE_QBOX_H_

#include "QBox.h"
#include "Table.h"
#include "SQLDelete.h"

BOREALIS_NAMESPACE_BEGIN

class DeleteQBox : public QBox
{

 private:
    ptr<SQLDelete> _sql;
    Table* _table;
    ptr<DeleteQueryHandle> _query_handle;
    bool _pass_input;
    bool _pass_deletions;
    
    size_t _input_tuple_size;
    size_t _row_size;
    
 protected:
    void setup_impl() throw( AuroraException );
    void init_impl() throw( AuroraException );
    void run_impl(QBoxInvocation& inv) throw( AuroraException );
};

BOREALIS_NAMESPACE_END

#endif /*DELETEQBOX_H_*/
