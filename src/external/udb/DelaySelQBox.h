#ifndef DELAYSELQBOX_H
#define DELAYSELQBOX_H

#include "QBox.h"

BOREALIS_NAMESPACE_BEGIN

// DelayQBox with optional selectivity parameter
//
class DelaySelQBox : public QBox
{
  protected:
    void setup_impl() throw (AuroraException);
    void run_impl(QBoxInvocation& inv) throw (AuroraException);

  private:
    Time   _delay;
    bool   _churn;
    double _selectivity;
};

BOREALIS_NAMESPACE_END

#endif                     // DELAYSELQBOX_H
