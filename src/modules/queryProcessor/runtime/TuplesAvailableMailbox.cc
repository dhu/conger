#include "TuplesAvailableMailbox.h"
#include "nmstl_util.h"
#include <iostream>
#include <assert.h>

BOREALIS_NAMESPACE_BEGIN;

//===============================================================================

TuplesAvailableMailbox::TuplesAvailableMailbox()
{
    _closed = false;

    int rc = pthread_cond_init(& _cond, NULL);
    assert(rc == 0);

    rc = pthread_mutex_init(& _mtx, NULL);
    assert(rc == 0);
}

//===============================================================================

TuplesAvailableMailbox::~TuplesAvailableMailbox()
{
}

//===============================================================================

void TuplesAvailableMailbox::on_non_empty(TupleQueue * p_tq)
{
    int rc = pthread_mutex_lock(& _mtx);
    // Suggest something like this (this was a quick hack)
    /**
    if (rc != 0)
    {
        if (rc == EINVAL) cout << " ***** EINVAL" << endl;
        if (rc == EDEADLK) cout << " ***** EDEADLK" << endl;
        perror("failed with");
    }
    */
    assert(rc == 0);

    DEBUG << "Adding report for " << (void*)p_tq;
    bool is_news = _unreceived_reports.insert(p_tq).second;

    if (is_news)
    {
        DEBUG << "It's news - broadcasting";
        rc = pthread_cond_broadcast(& _cond);
        assert(rc == 0);
    }

    rc = pthread_mutex_unlock(& _mtx);
    assert(rc == 0);
}

//===============================================================================

void TuplesAvailableMailbox::close()
{
    int rc = pthread_mutex_lock(& _mtx);
    assert(rc == 0);

    DEBUG << "Closing";
    _closed = true;

    rc = pthread_cond_broadcast(& _cond);
    assert(rc == 0);

    rc = pthread_mutex_unlock(& _mtx);
    assert(rc == 0);
}

//===============================================================================

bool TuplesAvailableMailbox::get_event(tuple_queue_ptr & p_tq)
{
    int rc = pthread_mutex_lock(& _mtx);
    assert(rc == 0);

    while (!_closed && _unreceived_reports.empty())
    {
        rc = pthread_cond_wait(& _cond, & _mtx);
        assert(rc == 0);
    }

    bool ret;

    if (_unreceived_reports.empty())
    {
        ret = false;
    } else
    {
        p_tq = *(_unreceived_reports.begin());
        _unreceived_reports.erase(_unreceived_reports.begin());
        DEBUG << "Handling report " << (void*)p_tq << "; "
              << _unreceived_reports.size() << " left";
        ret = true;
    }

    rc = pthread_mutex_unlock(& _mtx);
    assert(rc == 0);

    return ret;
}

//===============================================================================

BOREALIS_NAMESPACE_END;
