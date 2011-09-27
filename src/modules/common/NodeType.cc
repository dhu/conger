#include "NodeType.h"

BOREALIS_NAMESPACE_BEGIN

// --------------------------------------------------
/// For process-pair style of HA
const NodeType NodeType::PRIMARY = NodeType("Primary");

/// For replication style HA where we manually setup a bunch of replicas and tell them who they are
const NodeType NodeType::REPLICA = NodeType("Replica");

/// To monitor all upstream nodes that can provide us with the content that we want
const NodeType NodeType::PROVIDER = NodeType("Provider");

/// Default
const NodeType NodeType::NONE = NodeType("None");

BOREALIS_NAMESPACE_END

