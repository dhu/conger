#include "QBox.h"
#include "NArgs.h"
#include "Aggregate.h"

#include "CombineQBox.h"
#include "DelayQBox.h"
#include "DelaySelQBox.h"
#include "JoinStatic.h"
#include "LRWaitForQBox.h"


#include "FidAlarmCounterFunc.h"
#include "LRSecToMin.h"
#include "RandFunction.h"

#include "CountDistinct.h"
#include "FidComplianceLookthroughAndAgg.h"
#include "FidLateTrackerFunc.h"
#include "LRAccidentAggFunc1.h"
#include "LRAccidentAggFunc2.h"
#include "LRAvg.h"
#include "LRCarCount.h"
#include "LRSegmentStatisticsFunc2.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
// Create a new instance of a user defined box.
//
QBox  *QBox::instantiate_udb(// Type name of the box.
                             const string  &type)
{
//..............................................................................


    if      (type == "combine")     return  new CombineQBox;
    else if (type == "delay")       return  new DelayQBox;
    else if (type == "delay_sel")   return  new DelaySelQBox;
    else if (type == "joinstatic")  return  new JoinStatic;
    else if (type == "LRWaitFor")   return  new LRWaitForQBox;

    return  NULL;
}



////////////////////////////////////////////////////////////////////////////////
//
// Create a new instance of a user defined math function.
//
Function *Function::instantiate_udb(// Name of the math function.
                                    const string  &name)
{
//..............................................................................


    if      (name == "FidAlarmCounter")  return  new FidAlarmCounterFunc;
    else if (name == "LRSecToMin")       return  new LRSecToMinFunction;
    else if (name == "rand")             return  new RandFunction;

    return  NULL;
}



////////////////////////////////////////////////////////////////////////////////
//
// Create a new instance of a user defined aggregate function.
//
Aggregate  *Aggregate::instantiate_udb(// Name of the aggregate function.
                                       const string  &name)
{
//..............................................................................


    if (name == "FidComplianceLookthroughAndAgg") return  new FidComplianceLookthroughAndAgg;
    else if (name == "FidLateTracker")            return  new FidLateTrackerFunc;
    else if (name == "count_distinct")            return  new CountDistinct;
    else if (name == "LRAccidentAggFunc1")        return  new LRAccidentAggFunc1;
    else if (name == "LRAccidentAggFunc2")        return  new LRAccidentAggFunc2;
    else if (name == "LRAvg")                     return  new LRAvg;
    else if (name == "LRCarCount")                return  new LRCarCount;
    else if (name == "LRSegmentStatisticsFunc2")  return  new LRSegmentStatisticsFunc2;

    return  NULL;
}


BOREALIS_NAMESPACE_END 
