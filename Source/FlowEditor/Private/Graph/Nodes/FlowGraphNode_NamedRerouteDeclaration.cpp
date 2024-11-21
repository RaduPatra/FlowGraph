// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Graph/Nodes/FlowGraphNode_NamedRerouteDeclaration.h"
#include "FlowAsset.h"
#include "Graph/FlowGraphSchema.h"
#include "Graph/Widgets/SFlowGraphNode.h"
#include "Nodes/Route/FlowNode_NamedRerouteDeclaration.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowGraphNode_NamedRerouteDeclaration)

UFlowGraphNode_NamedRerouteDeclaration::UFlowGraphNode_NamedRerouteDeclaration(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AssignedNodeClasses = {UFlowNode_NamedRerouteDeclaration::StaticClass()};
}

void UFlowGraphNode_NamedRerouteDeclaration::PostEditImport()
{
	Super::PostEditImport();
	Modify();
	if (UFlowNode_NamedRerouteDeclaration* Declaration = Cast<UFlowNode_NamedRerouteDeclaration>(GetFlowNodeBase()))
	{
		EnsureUniqueNodeTitle();
		Declaration->CustomNodeColor = FLinearColor::MakeRandomColor();
	}
	UFlowGraphSchema::OnNodeListChanged.Broadcast();
}

void UFlowGraphNode_NamedRerouteDeclaration::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Modify();
	const UFlowNode_NamedRerouteDeclaration* Declaration = Cast<UFlowNode_NamedRerouteDeclaration>(GetFlowNodeBase());
	if (Declaration && PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UFlowNode_NamedRerouteDeclaration, NodeTitle))
	{
		EnsureUniqueNodeTitle();
	}
	UFlowGraphSchema::OnNodeListChanged.Broadcast();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UFlowGraphNode_NamedRerouteDeclaration::PostEditUndo()
{
	Modify();
	UFlowGraphSchema::OnNodeListChanged.Broadcast();
	Super::PostEditUndo();
}

void UFlowGraphNode_NamedRerouteDeclaration::PostPlacedNewNode()
{
	Modify();
	EnsureUniqueNodeTitle();
	UFlowGraphSchema::OnNodeListChanged.Broadcast();
	Super::PostPlacedNewNode();
}

void UFlowGraphNode_NamedRerouteDeclaration::AllocateDefaultPins()
{
	check(Pins.Num() == 0);

	if (UFlowNode* FlowNode = Cast<UFlowNode>(GetFlowNodeBase()))
	{
		for (const FFlowPin& OutputPin : FlowNode->OutputPins)
		{
			CreateOutputPin(OutputPin);
		}
	}
}

void UFlowGraphNode_NamedRerouteDeclaration::EnsureUniqueNodeTitle() const
{
	UFlowNode_NamedRerouteDeclaration* Declaration = Cast<UFlowNode_NamedRerouteDeclaration>(GetFlowNodeBase());
	if (!Declaration)
	{
		return;
	}
	
	if (const UFlowAsset* FlowAsset = Declaration->GetFlowAsset())
	{
		int32 NameIndex = 1;
		const FString BaseTitle = Declaration->NodeTitle.ToString();
		FName UniqueTitle = Declaration->NodeTitle;

		while (true)
		{
			bool bIsUnique = true;
			for (const auto& Pair : FlowAsset->GetNodes())
			{
				if (const UFlowNode_NamedRerouteDeclaration* OtherDeclaration = Cast<UFlowNode_NamedRerouteDeclaration>(Pair.Value))
				{
					if (OtherDeclaration && OtherDeclaration != Declaration && OtherDeclaration->NodeTitle == UniqueTitle)
					{
						bIsUnique = false;
						break;
					}
				}
			}

			if (bIsUnique)
			{
				break;
			}

			UniqueTitle = FName(*FString::Printf(TEXT("%s_%d"), *BaseTitle, NameIndex));
			++NameIndex;
		}

		Declaration->NodeTitle = UniqueTitle;
	}
}