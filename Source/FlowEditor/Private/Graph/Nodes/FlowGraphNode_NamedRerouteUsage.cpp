// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Graph/Nodes/FlowGraphNode_NamedRerouteUsage.h"
#include "FlowAsset.h"
#include "Graph/Widgets/SFlowGraphNode.h"

#include "Nodes/Route/FlowNode_NamedRerouteUsage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowGraphNode_NamedRerouteUsage)

UFlowGraphNode_NamedRerouteUsage::UFlowGraphNode_NamedRerouteUsage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AssignedNodeClasses = {UFlowNode_NamedRerouteUsage::StaticClass()};
}