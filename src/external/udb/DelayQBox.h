#ifndef DELAYQBOX_H
#define DELAYQBOX_H

#include "QBox.h"

BOREALIS_NAMESPACE_BEGIN

class DelayQBox : public QBox
{
  protected:
    void setup_impl() throw (AuroraException);

    void run_impl(QBoxInvocation& inv) throw (AuroraException);

  private:
    Time _delay;
    bool _churn;
};

BOREALIS_NAMESPACE_END

#endif                     // DELAYQBOX_H
