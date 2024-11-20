// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "UObject/Interface.h"
#include "Templates/SubclassOf.h"

#include "FlowPredicateInterface.generated.h"

class UFlowNodeAddOn;

USTRUCT(BlueprintType)
struct FLOW_API FFlowContextData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	AActor* FlowOwner;

	UPROPERTY(BlueprintReadOnly)
	const UObject* Context;

	FFlowContextData(AActor* owner);
	FFlowContextData(AActor* owner, const UObject* context);
	FFlowContextData();
};


// Predicate interface for AddOns
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable, DisplayName = "Flow Predicate Interface")
class UFlowPredicateInterface : public UInterface
{
	GENERATED_BODY()
};

class FLOW_API IFlowPredicateInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	bool EvaluatePredicate() const;
	virtual bool EvaluatePredicate_Implementation() const { return true; }

	static bool ImplementsInterfaceSafe(const UFlowNodeAddOn* AddOnTemplate);

	UFUNCTION(BlueprintNativeEvent)
	bool PreEvaluatePredicate(FFlowContextData& Context) const;
	virtual bool PreEvaluatePredicate_Implementation(FFlowContextData& Context) const { return true; }
};
