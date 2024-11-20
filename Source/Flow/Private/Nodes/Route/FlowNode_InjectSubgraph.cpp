// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/Route/FlowNode_InjectSubgraph.h"

#include "FlowAsset.h"
#include "FlowSubsystem.h"
#include "Nodes/Utils/FlowInjectionTag.h"

TSoftObjectPtr<UFlowAsset> UFlowNode_InjectSubgraph::GetAssetToInject()
{
	const AActor* Actor = TryGetRootFlowActorOwner();
	TSoftObjectPtr<UFlowAsset> AssetToInject = nullptr;
	if (Actor)
	{
		if (UFlowComponent* FlowComponent = Actor->FindComponentByClass<UFlowComponent>())
		{
			const UFlowAsset* RootFlowInstance = GetAssetRootFlow(GetFlowAsset());
			if (const FInjectedAssets* AssetsData = FlowComponent->InjectionAssetsMap.Find(RootFlowInstance))
			{
				FInjectedAssets InjectionAssets = *AssetsData;
				if (UFlowAsset** InjectedInstance = InjectionAssets.InjectionAssets.Find(InjectionTag))
				{
					AssetToInject = *InjectedInstance;
				}
			}
		}
	}

	return AssetToInject;
}

void UFlowNode_InjectSubgraph::ExecuteInput(const FName& PinName)
{
	if (PinName == TEXT("Start"))
	{
		if (GetFlowSubsystem())
		{
			const TSoftObjectPtr<UFlowAsset> AssetToInject = GetAssetToInject();
			Asset = AssetToInject;
			if (Asset.IsValid())
			{
				UFlowAsset* Inst = GetFlowSubsystem()->CreateSubFlow(this);
			}
			else
			{
				TriggerFirstOutput(true);
			}
		}
	}
}

bool UFlowNode_InjectSubgraph::CanBeAssetInstanced() const
{
	return (bCanInstanceIdenticalAsset || Asset.ToString() != GetFlowAsset()->GetTemplateAsset()->GetPathName());
}




void UFlowNode_InjectSubgraph::InitializeInstance()
{
	Super::InitializeInstance();
}

UFlowAsset* UFlowNode_InjectSubgraph::GetAssetRootFlow(UFlowAsset* asset)
{
	if (UFlowAsset* Parent = asset->GetParentInstance())
	{
		GetAssetRootFlow(Parent);
	}
	return asset;
}

void UFlowNode_InjectSubgraph::PreloadContent()
{
	const TSoftObjectPtr<UFlowAsset> AssetToInject = GetAssetToInject();
	Asset = AssetToInject;
	if (Asset.IsValid())
	{
		GetFlowSubsystem()->CreateSubFlow(this, FString(), true);
	}
	else
	{
		// LogError(TEXT("Failed to find injection Flow Asset"));
	}
}

void UFlowNode_InjectSubgraph::FlushContent()
{
	const AActor* Actor = TryGetRootFlowActorOwner();
	if (UFlowComponent* FlowComponent = Actor ? Actor->FindComponentByClass<UFlowComponent>() : nullptr)
	{
		GetFlowSubsystem()->RemoveSubFlow(this, EFlowFinishPolicy::Abort);
	}
}

#if WITH_EDITOR
FString UFlowNode_InjectSubgraph::GetStatusString() const
{
	FString Status = GetNodeDescription();
	if (Asset.IsValid())
	{
		Status += FString::Printf(TEXT(": (%s)"), *Asset.GetAssetName());
	}
	else
	{
		Status += TEXT(":<No Asset>");
	}
	return Status;
}
EDataValidationResult UFlowNode_InjectSubgraph::ValidateNode()
{
	if (!InjectionTag)
	{
		ValidationLog.Error<UFlowNode>(TEXT("InjectionTagAsset is empty! Node will be skipped."), this);
		return EDataValidationResult::Invalid;
	}
	return EDataValidationResult::Valid;
}

TArray<FFlowPin> UFlowNode_InjectSubgraph::GetContextInputs() const
{
	TArray<FFlowPin> EventNames;
	if (InjectionTag)
	{
		for (const FName& PinName : InjectionTag->GetRequiredCustomInputs())
		{
			if (!PinName.IsNone())
			{
				EventNames.Emplace(PinName);
			}
		}
	}
	return EventNames;
}

TArray<FFlowPin> UFlowNode_InjectSubgraph::GetContextOutputs() const
{
	TArray<FFlowPin> EventNames;
	if (InjectionTag)
	{
		for (const FName& PinName : InjectionTag->GetRequiredCustomOutputs())
		{
			if (!PinName.IsNone())
			{
				EventNames.Emplace(PinName);
			}
		}
	}
	return EventNames;
}

void UFlowNode_InjectSubgraph::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property && PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UFlowNode_InjectSubgraph, InjectionTag))
	{
		bool bRes = OnReconstructionRequested.ExecuteIfBound();
		SubscribeToInjectionAssetChanges();
	}
}

UObject* UFlowNode_InjectSubgraph::GetAssetToEdit()
{
	return InjectionTag;
}

void UFlowNode_InjectSubgraph::PostLoad()
{
	Super::PostLoad();
	SubscribeToInjectionAssetChanges();
}

void UFlowNode_InjectSubgraph::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	if (PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(UFlowNode_InjectSubgraph, InjectionTag))
	{
		if (InjectionTag && InputChangeHandle.IsValid())
		{
			InjectionTag->OnCustomInputChange.Remove(InputChangeHandle);
			InputChangeHandle.Reset();
		}
	}
}

FString UFlowNode_InjectSubgraph::GetNodeDescription() const
{
	if (!InjectionTag)
	{
		return "Missing Injection Tag Asset!";
	}
	return InjectionTag->GetName();
}

void UFlowNode_InjectSubgraph::SubscribeToInjectionAssetChanges()
{
	if (InjectionTag)
	{
		TWeakObjectPtr<UFlowNode_InjectSubgraph> SelfWeakPtr(this);

		const bool bIsBound = InjectionTag->OnCustomInputChange.IsBoundToObject(this);
		if (bIsBound || InputChangeHandle.IsValid())
		{
			return;
		}
		InputChangeHandle = InjectionTag->OnCustomInputChange.AddLambda([SelfWeakPtr]()
		{
			if (SelfWeakPtr.IsValid())
			{
				bool bRes = SelfWeakPtr->OnReconstructionRequested.ExecuteIfBound();
			}
		});
	}
}

#endif
