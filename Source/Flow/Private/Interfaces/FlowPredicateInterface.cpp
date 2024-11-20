// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Interfaces/FlowPredicateInterface.h"
#include "AddOns/FlowNodeAddOn.h"

FFlowContextData::FFlowContextData(AActor* owner)
{
	FlowOwner = owner;
	Context = nullptr;
}

FFlowContextData::FFlowContextData(AActor* owner, const UObject* context)
{
	FlowOwner = owner;
	Context = context;
}

FFlowContextData::FFlowContextData()
	: FlowOwner(nullptr)
	, Context(nullptr)
{
}

bool IFlowPredicateInterface::ImplementsInterfaceSafe(const UFlowNodeAddOn* AddOnTemplate)
{
	if (!IsValid(AddOnTemplate))
	{
		return false;
	}

	UClass* AddOnClass = AddOnTemplate->GetClass();
	if (AddOnClass->ImplementsInterface(UFlowPredicateInterface::StaticClass()))
	{
		return true;
	}

	return false;
}
