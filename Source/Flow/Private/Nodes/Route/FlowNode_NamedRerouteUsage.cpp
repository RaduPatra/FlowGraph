// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/Route/FlowNode_NamedRerouteUsage.h"

#include "FlowAsset.h"

UFlowNode_NamedRerouteUsage::UFlowNode_NamedRerouteUsage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Named Reroute");
	NodeStyle = EFlowNodeStyle::Custom;
#endif
	OutputPins = {};
	AllowedSignalModes = {EFlowSignalMode::Enabled, EFlowSignalMode::Disabled, EFlowSignalMode::PassThrough};
}

void UFlowNode_NamedRerouteUsage::ExecuteInput(const FName& PinName)
{
	if (LinkedDeclaration)
	{
		Finish();
		GetFlowAsset()->TriggerInput(LinkedDeclaration->GetGuid(), PinName);
	}
}

void UFlowNode_NamedRerouteUsage::RegisterLinkedDeclaration(UFlowNode_NamedRerouteDeclaration* Declaration)
{
	Modify();
	if (Declaration)
	{
		LinkedDeclaration = Declaration;
	}
	
#if WITH_EDITOR
	TryUpdateNode();
#endif
}

void UFlowNode_NamedRerouteUsage::UnregisterLinkedDeclaration()
{
	Modify();
	if (LinkedDeclaration)
	{
		LinkedDeclaration = nullptr;
	}

#if WITH_EDITOR
	TryUpdateNode();
#endif
}

#if WITH_EDITOR
FText UFlowNode_NamedRerouteUsage::GetNodeTitle() const
{
	return FText::FromString(NodeTitle.ToString());
}

FString UFlowNode_NamedRerouteUsage::GetNodeDescription() const
{
	if (!LinkedDeclaration)
	{
		return TEXT("Declaration is NOT valid!");
	}

	return FString();
}

FString UFlowNode_NamedRerouteUsage::GetStatusString() const
{
	if (ActivationState == EFlowNodeState::Active && !LinkedDeclaration)
	{
		return TEXT("The Named Reroute Declaration that is linked to this usage is NOT valid!");
	}

	return FString();
}

EDataValidationResult UFlowNode_NamedRerouteUsage::ValidateNode()
{
	if (!LinkedDeclaration)
	{
		ValidationLog.Error<UFlowNode>(TEXT("Named Reroute Declaration is NOT valid!"), this);
		return EDataValidationResult::Invalid;
	}

	return EDataValidationResult::Valid;
}

bool UFlowNode_NamedRerouteUsage::GetDynamicTitleColor(FLinearColor& OutColor) const
{
	if (LinkedDeclaration)
	{
		if (NodeStyle == EFlowNodeStyle::Custom)
		{
			OutColor = LinkedDeclaration->CustomNodeColor;
			return true;
		}
	}

	return false;
}

void UFlowNode_NamedRerouteUsage::TryUpdateNode()
{
	Modify();
	if (LinkedDeclaration)
	{
		NodeTitle = FName(*LinkedDeclaration->GetNodeTitle().ToString());
	}
	else
	{
		NodeTitle = FName(TEXT("Invalid Named Reroute"));
	}
}
#endif