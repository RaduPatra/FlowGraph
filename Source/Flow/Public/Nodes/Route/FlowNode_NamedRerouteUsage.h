// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "CoreMinimal.h"

#include "FlowNode_NamedRerouteDeclaration.h"
#include "FlowNode_NamedRerouteUsage.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable)
class FLOW_API UFlowNode_NamedRerouteUsage : public UFlowNode
{
	GENERATED_UCLASS_BODY()
	friend class UFlowNode_NamedRerouteDeclaration;
public:
	virtual void ExecuteInput(const FName& PinName) override;
	virtual void RegisterLinkedDeclaration(UFlowNode_NamedRerouteDeclaration* Declaration);
	virtual void UnregisterLinkedDeclaration();
	virtual UFlowNode_NamedRerouteDeclaration* GetLinkedDeclaration() const{return LinkedDeclaration;}

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	FName NodeTitle;
#endif
	
protected:
#if WITH_EDITOR
	virtual FText GetNodeTitle() const override;
	virtual FString GetNodeDescription() const override;
	virtual FString GetStatusString() const override;
	virtual EDataValidationResult ValidateNode() override;
	virtual bool GetDynamicTitleColor(FLinearColor& OutColor) const override;
#endif
private:
#if WITH_EDITOR
	void TryUpdateNode();
#endif
	// The declaration this usage is linked to
	UPROPERTY(TextExportTransient)
	TObjectPtr<UFlowNode_NamedRerouteDeclaration> LinkedDeclaration;
};