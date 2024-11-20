// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/Utils//FlowInjectionTag.h"

#if WITH_EDITOR
void UFlowInjectionTag::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property && (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UFlowInjectionTag, RequiredCustomInputs) || PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UFlowInjectionTag, RequiredCustomOutputs)))
	{
		OnCustomInputChange.Broadcast();
	}
}
#endif

