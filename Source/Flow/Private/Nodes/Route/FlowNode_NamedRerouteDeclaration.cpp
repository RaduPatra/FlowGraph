// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/Route/FlowNode_NamedRerouteDeclaration.h"

#include "FlowAsset.h"
#include "Nodes/Route/FlowNode_NamedRerouteUsage.h"

UFlowNode_NamedRerouteDeclaration::UFlowNode_NamedRerouteDeclaration(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Route");
	NodeStyle = EFlowNodeStyle::Custom;
	CustomNodeColor = FLinearColor::MakeRandomColor();
	NodeTitle = TEXT("Named Reroute");
#endif
	AllowedSignalModes = {EFlowSignalMode::Enabled, EFlowSignalMode::Disabled, EFlowSignalMode::PassThrough};
}

void UFlowNode_NamedRerouteDeclaration::ExecuteInput(const FName& PinName)
{
	TriggerFirstOutput(true);
}

#if WITH_EDITOR
FText UFlowNode_NamedRerouteDeclaration::GetNodeTitle() const
{
	return FText::FromString(NodeTitle.ToString());
}

bool UFlowNode_NamedRerouteDeclaration::GetDynamicTitleColor(FLinearColor& OutColor) const
{
	if (NodeStyle == EFlowNodeStyle::Custom)
	{
		OutColor = CustomNodeColor;
		return true;
	}

	return false;
}

void UFlowNode_NamedRerouteDeclaration::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, NodeTitle) ||
		PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, CustomNodeColor))
	{
		GetGraphNode()->PostEditChangeProperty(PropertyChangedEvent);
		for (const auto Usage : GetFlowAsset()->FindNamedRerouteUsages(GetGuid()))
		{
			if (Usage)
			{
				Usage->TryUpdateNode();
			}
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif // WITH_EDITOR