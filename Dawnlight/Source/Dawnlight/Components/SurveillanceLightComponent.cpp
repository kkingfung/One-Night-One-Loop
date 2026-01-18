// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurveillanceLightComponent.h"
#include "Dawnlight.h"
#include "SurveillanceSubsystem.h"
#include "Engine/World.h"

USurveillanceLightComponent::USurveillanceLightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// デフォルト設定
	LightRadius = 500.0f;
	LightIntensity = 1.0f;
	bContributesToDetection = true;
	bEnabledOnStart = true;
	bIsEnabled = false;
}

void USurveillanceLightComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bEnabledOnStart)
	{
		EnableLight();
	}

	UE_LOG(LogDawnlight, Log, TEXT("SurveillanceLightComponent: %s で開始しました (半径: %.1f, 強度: %.2f)"),
		*GetOwner()->GetName(), LightRadius, LightIntensity);
}

void USurveillanceLightComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DisableLight();

	Super::EndPlay(EndPlayReason);
}

void USurveillanceLightComponent::EnableLight()
{
	if (bIsEnabled)
	{
		return;
	}

	bIsEnabled = true;
	RegisterWithSubsystem();

	UE_LOG(LogDawnlight, Verbose, TEXT("SurveillanceLightComponent: %s の光源を有効化しました"), *GetOwner()->GetName());
}

void USurveillanceLightComponent::DisableLight()
{
	if (!bIsEnabled)
	{
		return;
	}

	bIsEnabled = false;
	UnregisterFromSubsystem();

	UE_LOG(LogDawnlight, Verbose, TEXT("SurveillanceLightComponent: %s の光源を無効化しました"), *GetOwner()->GetName());
}

void USurveillanceLightComponent::ToggleLight()
{
	if (bIsEnabled)
	{
		DisableLight();
	}
	else
	{
		EnableLight();
	}
}

void USurveillanceLightComponent::SetLightRadius(float NewRadius)
{
	LightRadius = FMath::Max(50.0f, NewRadius);

	// 有効な場合は再登録
	if (bIsEnabled)
	{
		UnregisterFromSubsystem();
		RegisterWithSubsystem();
	}
}

void USurveillanceLightComponent::SetLightIntensity(float NewIntensity)
{
	LightIntensity = FMath::Clamp(NewIntensity, 0.0f, 1.0f);

	// 有効な場合は再登録
	if (bIsEnabled)
	{
		UnregisterFromSubsystem();
		RegisterWithSubsystem();
	}
}

void USurveillanceLightComponent::RegisterWithSubsystem()
{
	USurveillanceSubsystem* Subsystem = GetSurveillanceSubsystem();
	if (Subsystem && GetOwner())
	{
		Subsystem->RegisterLightSource(GetOwner(), LightRadius, LightIntensity, bContributesToDetection);
	}
}

void USurveillanceLightComponent::UnregisterFromSubsystem()
{
	USurveillanceSubsystem* Subsystem = GetSurveillanceSubsystem();
	if (Subsystem && GetOwner())
	{
		Subsystem->UnregisterLightSource(GetOwner());
	}
}

USurveillanceSubsystem* USurveillanceLightComponent::GetSurveillanceSubsystem()
{
	if (CachedSubsystem.IsValid())
	{
		return CachedSubsystem.Get();
	}

	if (UWorld* World = GetWorld())
	{
		CachedSubsystem = World->GetSubsystem<USurveillanceSubsystem>();
		return CachedSubsystem.Get();
	}

	return nullptr;
}
