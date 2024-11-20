// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FlowInjectionTag.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnCustomInputChange);
UCLASS(BlueprintType, Blueprintable)
class FLOW_API UFlowInjectionTag : public UPrimaryDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Flow Graph")
	TArray<FName> RequiredCustomInputs;

	UPROPERTY(EditDefaultsOnly, Category = "Flow Graph")
	TArray<FName> RequiredCustomOutputs;

public:
	FOnCustomInputChange OnCustomInputChange;

	TArray<FName> GetRequiredCustomInputs() const { return RequiredCustomInputs; }
	TArray<FName> GetRequiredCustomOutputs() const { return RequiredCustomOutputs; }


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
