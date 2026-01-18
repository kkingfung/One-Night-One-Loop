// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurveillanceSubsystem.h"
#include "Dawnlight.h"
#include "DawnlightTags.h"
#include "Engine/World.h"

void USurveillanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentSurveillanceLevel = ESurveillanceLevel::Low;
	RegisteredLightSources.Empty();

	UE_LOG(LogDawnlight, Log, TEXT("SurveillanceSubsystem: 初期化しました"));
}

void USurveillanceSubsystem::Deinitialize()
{
	RegisteredLightSources.Empty();

	UE_LOG(LogDawnlight, Log, TEXT("SurveillanceSubsystem: 終了しました"));

	Super::Deinitialize();
}

bool USurveillanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// ゲームワールドでのみ作成
	if (const UWorld* World = Cast<UWorld>(Outer))
	{
		return World->IsGameWorld();
	}
	return false;
}

void USurveillanceSubsystem::RegisterLightSource(AActor* LightActor, float Radius, float Intensity, bool bContributesToDetection)
{
	if (!LightActor)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("SurveillanceSubsystem: 無効な光源アクターが渡されました"));
		return;
	}

	// 既に登録されているか確認
	for (const FLightSourceInfo& Info : RegisteredLightSources)
	{
		if (Info.LightActor == LightActor)
		{
			UE_LOG(LogDawnlight, Warning, TEXT("SurveillanceSubsystem: 光源 %s は既に登録されています"), *LightActor->GetName());
			return;
		}
	}

	FLightSourceInfo NewInfo;
	NewInfo.LightActor = LightActor;
	NewInfo.Radius = Radius;
	NewInfo.Intensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
	NewInfo.bContributesToDetection = bContributesToDetection;

	RegisteredLightSources.Add(NewInfo);

	UE_LOG(LogDawnlight, Log, TEXT("SurveillanceSubsystem: 光源 %s を登録しました (半径: %.1f)"), *LightActor->GetName(), Radius);
}

void USurveillanceSubsystem::UnregisterLightSource(AActor* LightActor)
{
	if (!LightActor)
	{
		return;
	}

	const int32 RemovedCount = RegisteredLightSources.RemoveAll([LightActor](const FLightSourceInfo& Info)
	{
		return Info.LightActor == LightActor;
	});

	if (RemovedCount > 0)
	{
		UE_LOG(LogDawnlight, Log, TEXT("SurveillanceSubsystem: 光源 %s を解除しました"), *LightActor->GetName());
	}
}

bool USurveillanceSubsystem::IsLocationInLight(const FVector& Location) const
{
	for (const FLightSourceInfo& Info : RegisteredLightSources)
	{
		if (!Info.LightActor.IsValid() || !Info.bContributesToDetection)
		{
			continue;
		}

		const FVector LightLocation = Info.LightActor->GetActorLocation();
		const float Distance = FVector::Distance(Location, LightLocation);

		if (Distance <= Info.Radius)
		{
			// 遮蔽物チェック
			if (!IsLocationOccluded(Location, LightLocation))
			{
				return true;
			}
		}
	}

	return false;
}

float USurveillanceSubsystem::GetLightIntensityAtLocation(const FVector& Location) const
{
	float MaxIntensity = 0.0f;

	for (const FLightSourceInfo& Info : RegisteredLightSources)
	{
		if (!Info.LightActor.IsValid() || !Info.bContributesToDetection)
		{
			continue;
		}

		const FVector LightLocation = Info.LightActor->GetActorLocation();
		const float Distance = FVector::Distance(Location, LightLocation);

		if (Distance <= Info.Radius)
		{
			// 遮蔽物チェック
			if (!IsLocationOccluded(Location, LightLocation))
			{
				// 距離に基づく減衰
				const float Falloff = 1.0f - (Distance / Info.Radius);
				const float CurrentIntensity = Info.Intensity * Falloff;
				MaxIntensity = FMath::Max(MaxIntensity, CurrentIntensity);
			}
		}
	}

	return MaxIntensity;
}

bool USurveillanceSubsystem::IsLocationOccluded(const FVector& Location, const FVector& LightLocation) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;

	// 光源から対象位置へのライントレース
	const bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		LightLocation,
		Location,
		ECC_Visibility,
		QueryParams
	);

	return bHit;
}

FGameplayTag USurveillanceSubsystem::GetSurveillanceLevelTag() const
{
	switch (CurrentSurveillanceLevel)
	{
	case ESurveillanceLevel::Low:
		return DawnlightTags::Surveillance_Level_Low;
	case ESurveillanceLevel::Medium:
		return DawnlightTags::Surveillance_Level_Medium;
	case ESurveillanceLevel::High:
		return DawnlightTags::Surveillance_Level_High;
	case ESurveillanceLevel::Critical:
		return DawnlightTags::Surveillance_Level_Critical;
	default:
		return DawnlightTags::Surveillance_Level_Low;
	}
}

void USurveillanceSubsystem::SetSurveillanceLevel(ESurveillanceLevel NewLevel)
{
	if (CurrentSurveillanceLevel == NewLevel)
	{
		return;
	}

	const ESurveillanceLevel OldLevel = CurrentSurveillanceLevel;
	CurrentSurveillanceLevel = NewLevel;

	UE_LOG(LogDawnlight, Log, TEXT("SurveillanceSubsystem: 監視レベルが変更されました %d → %d"),
		static_cast<int32>(OldLevel), static_cast<int32>(NewLevel));

	OnSurveillanceLevelChanged.Broadcast(OldLevel, NewLevel);
}

void USurveillanceSubsystem::IncreaseSurveillanceLevel()
{
	const int32 CurrentLevel = static_cast<int32>(CurrentSurveillanceLevel);
	const int32 MaxLevel = static_cast<int32>(ESurveillanceLevel::Critical);

	if (CurrentLevel < MaxLevel)
	{
		SetSurveillanceLevel(static_cast<ESurveillanceLevel>(CurrentLevel + 1));
	}
}

void USurveillanceSubsystem::ResetSurveillanceLevel()
{
	SetSurveillanceLevel(ESurveillanceLevel::Low);
}

void USurveillanceSubsystem::CleanupInvalidLightSources()
{
	RegisteredLightSources.RemoveAll([](const FLightSourceInfo& Info)
	{
		return !Info.LightActor.IsValid();
	});
}
