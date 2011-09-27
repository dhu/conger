#ifndef HA_METHOD_H
#define HA_METHOD_H

#include  "common.h"
#include  "Exceptions.h"

BOREALIS_NAMESPACE_BEGIN

typedef enum
{
    NO_HA = 0,
    AMNESIA,
    UPSTREAM_BACKUP,
    PASSIVE_STANDBY,
    ACTIVE_STANDBY
} HAMethod;

HAMethod string_to_method(string method) throw( AuroraBadEntityException );

// To ease translation from integers
const HAMethod hamethod_t[] =
{
    NO_HA,
    AMNESIA,
    UPSTREAM_BACKUP,
    PASSIVE_STANDBY,
    ACTIVE_STANDBY
};


// To ease printing of HAMethods
const string  hamethod_s[] =
{   "no_ha",
    "amnesia",
    "upstream_backup",
    "passive_standby",
    "active_standby"
};

typedef enum
{
    WHOLE = 0,
    ROUND_ROBIN,
    MIN_MAX
} CheckpointMethod;

CheckpointMethod string_to_checkpoint_method(string method) throw( AuroraBadEntityException );

// To ease translation from integers
const CheckpointMethod checkpoint_method_t[] =
{
    WHOLE,
    ROUND_ROBIN,
    MIN_MAX
};


// To ease printing of CheckpointMethod
const string  checkpoint_method_s[] =
{   "whole",
    "round_robin",
    "min_max"
};


BOREALIS_NAMESPACE_END
#endif                        // HA_METHOD_H
