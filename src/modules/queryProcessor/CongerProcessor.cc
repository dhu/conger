/**
 * CongerProcessor.cc
 *
 *  Created on: Apr 17, 2012
 *      Author: hellojinjie
 */
#include "QueryProcessor.h"

BOREALIS_NAMESPACE_BEGIN

AsyncRPC<void> QueryProcessor::add_conger_string(string conger_config)
{
    INFO << conger_config;
    AsyncRPC<void> completion;
    completion.post(true);
    return completion;
}

BOREALIS_NAMESPACE_END
