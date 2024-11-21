// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"

#include "FlowNode_NamedRerouteDeclaration.generated.h"

/**
 * Captures the value of an input, may be used at multiple other points in the graph without requiring connecting wires, allows tiding up of complex graphs
 */
UCLASS(NotBlueprintable)
class FLOW_API UFlowNode_NamedRerouteDeclaration : public UFlowNode
{
	GENERATED_UCLASS_BODY()
	friend class UFlowNode_NamedRerouteUsage;
	
protected:
	virtual void ExecuteInput(const FName& PinName) override;

#if WITH_EDITOR
	virtual bool GetDynamicTitleColor(FLinearColor& OutColor) const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual FText GetNodeTitle() const override;
#endif
	
public:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Named Reroute")
	FName NodeTitle;
	/** The color of the graph node. The same color will apply to all linked usages of this Declaration node */
	UPROPERTY(EditAnywhere, Category = "Named Reroute", DisplayName = "Node Color")
	FLinearColor CustomNodeColor;
#endif
};