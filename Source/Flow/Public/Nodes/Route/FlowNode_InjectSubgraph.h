// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlowNode_SubGraph.h"
#include "Nodes/Utils/FlowInjectionTag.h"
#include "FlowNode_InjectSubgraph.generated.h"

class UFlowInjectionTag;
class UFlowInjectComponentsManager;
/**
 * 
 */
UCLASS()
class FLOW_API UFlowNode_InjectSubgraph : public UFlowNode_SubGraph
{
	GENERATED_BODY()

public:
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InjectionTag;
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UFlowInjectionTag* InjectionTag;
	FDelegateHandle InputChangeHandle;

	TSoftObjectPtr<UFlowAsset> GetAssetToInject();
	virtual void ExecuteInput(const FName& PinName) override;
	virtual bool CanBeAssetInstanced() const override;
	virtual void InitializeInstance() override;
	UFlowAsset* GetAssetRootFlow(UFlowAsset* asset);
	virtual void PreloadContent() override;
	virtual void FlushContent() override;

#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
	virtual FString GetStatusString() const override;
	virtual TArray<FFlowPin> GetContextInputs() const override;
	virtual TArray<FFlowPin> GetContextOutputs() const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual UObject* GetAssetToEdit() override;
	virtual void PostLoad() override;
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual EDataValidationResult ValidateNode() override;
	void SubscribeToInjectionAssetChanges();
#endif
};
