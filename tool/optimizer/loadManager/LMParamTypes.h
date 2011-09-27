#ifndef LMParamTypes_H
#define LMParamTypes_H

#include "LMCommon.h"

BOREALIS_NAMESPACE_BEGIN

typedef enum
{
    VOM_RAND = 0,
    VOM_NORM1,
    VOM_NORM2,
    VOM_SUM
} VectorOrderingMethod;


typedef enum
{
    LDP_WARMUP = 0,
    LDP_INITIAL,
    LDP_DYNAMIC
} LoadDistributionPeriod;

typedef enum
{
    SF_LOAD = 0,
    SF_RANDOM,
    SF_REDUCED_BANDWIDTH,
    SF_REDUCED_LOAD_VARIANCE,
    SF_CORRELATION_BASED,
    SF_INCREASED_CORRELATION
} ScoreFunctionType;

typedef enum
{
    PLDM_NONE = 0,
    PLDM_ONE_WAY,
    PLDM_REDISTRIBUTE,
    PLDM_SELECTIVE_EXCHANGE
} PairwiseLoadDistributionMethod;

/* obsolete
typedef enum
{
    IBDM_RANDOM = 0,
    IBDM_SINGLE,
    IBDM_CONNECTED,
    IBDM_CUT,
    IBDM_JHW
} InitialBoxDistributionMethod;

typedef enum
{
    LDBM_NONE = 0,
    LDBM_LOAD_SHARING,
    LDBM_LOAD_BALANCING
} LoadDistributionBaseMethod;

typedef enum
{
    GLDM_NONE = 0,
    GLDM_GREEDY
} GlobalLoadDistributionMethod;

typedef enum
{
    BSM_RANDOM = 0,
    BSM_BALANCING,
    BSM_CORRELATION_BASED
} BoxSelectionMethod;

*/


/* For bandwidth algorithm. Not added yet.
typedef enum
{
    OCM_LARGEST_BANDDWITHD_FIRST = 0,
    OCM_SMALLEST_LOAD_FIRST,
    OCM_LARGEST_RATIO_FIRST,
    OCM_CORRELATION_BASED,
    OCM_RANDOM
} OperatorClusteringMethod;

typedef enum
{
    OCSF_REDUCED_BANDWITH,
    OCSF_REVERSE_CLUSER_STD,
    OCSF_REVERSE_LOAD,
    OCSF_REDUCED_BANDWITH_CLUSTER_STD_RATIO,
    OCSF_REDUCED_BANDWITH_CLUSTER_LOAD_RATIO
} OperatorClusteringScoreFunction;

typedef enum
{
    LBM_MIN_BANDWIDTH,
    LBM_CORRELATION_BASED
} LoadBalancingMethod;
*/


BOREALIS_NAMESPACE_END

#endif
