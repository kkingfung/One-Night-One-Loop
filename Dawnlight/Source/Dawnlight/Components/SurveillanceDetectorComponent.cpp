// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurveillanceDetectorComponent.h"
#include "Dawnlight.h"
#include "SurveillanceSubsystem.h"
#include "Engine/World.h"

USurveillanceDetectorComponent::USurveillanceDetectorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.0f; // 毎フレーム更新（UpdateIntervalで制御）

	// デフォルト設定
	DetectionIncreaseRate = 0.3f;  // 約3.3秒で満タン
	DetectionDecayRate = 0.15f;    // 約6.6秒で空に
	DetectionThreshold = 0.8f;     // 80%で検知
	DetectionOffset = FVector::ZeroVector;
	UpdateInterval = 0.05f;        // 20Hz

	// 状態初期化
	bIsInLight = false;
	bWasInLight = false;
	bWasDetected = false;
	DetectionLevel = 0.0f;
	CurrentLightIntensity = 0.0f;
	TimeSinceLastUpdate = 0.0f;
}

void USurveillanceDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	// サブシステムをキャッシュ
	GetSurveillanceSubsystem();

	UE_LOG(LogDawnlight, Log, TEXT("SurveillanceDetectorComponent: %s で開始しました"), *GetOwner()->GetName());
}

void USurveillanceDetectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 更新間隔チェック
	TimeSinceLastUpdate += DeltaTime;
	if (TimeSinceLastUpdate < UpdateInterval)
	{
		return;
	}

	const float ActualDeltaTime = TimeSinceLastUpdate;
	TimeSinceLastUpdate = 0.0f;

	// 前の状態を保存
	bWasInLight = bIsInLight;
	bWasDetected = IsDetected();

	// サブシステムで光の状態をチェック
	USurveillanceSubsystem* Subsystem = GetSurveillanceSubsystem();
	if (Subsystem)
	{
		const FVector DetectionLocation = GetDetectionLocation();
		bIsInLight = Subsystem->IsLocationInLight(DetectionLocation);
		CurrentLightIntensity = Subsystem->GetLightIntensityAtLocation(DetectionLocation);
	}
	else
	{
		bIsInLight = false;
		CurrentLightIntensity = 0.0f;
	}

	// 検知レベルの更新
	if (bIsInLight)
	{
		// 光の中：検知レベル上昇（光の強度に比例）
		DetectionLevel += DetectionIncreaseRate * CurrentLightIntensity * ActualDeltaTime;
	}
	else
	{
		// 光の外：検知レベル減衰
		DetectionLevel -= DetectionDecayRate * ActualDeltaTime;
	}

	DetectionLevel = FMath::Clamp(DetectionLevel, 0.0f, 1.0f);

	// イベント発火
	if (bIsInLight && !bWasInLight)
	{
		UE_LOG(LogDawnlight, Verbose, TEXT("SurveillanceDetectorComponent: %s が光に入りました"), *GetOwner()->GetName());
		OnEnteredLight.Broadcast();
	}
	else if (!bIsInLight && bWasInLight)
	{
		UE_LOG(LogDawnlight, Verbose, TEXT("SurveillanceDetectorComponent: %s が光から出ました"), *GetOwner()->GetName());
		OnExitedLight.Broadcast();
	}

	const bool bCurrentlyDetected = IsDetected();
	if (bCurrentlyDetected && !bWasDetected)
	{
		UE_LOG(LogDawnlight, Log, TEXT("SurveillanceDetectorComponent: %s が検知されました"), *GetOwner()->GetName());
		OnDetected.Broadcast();
	}
	else if (!bCurrentlyDetected && bWasDetected)
	{
		UE_LOG(LogDawnlight, Log, TEXT("SurveillanceDetectorComponent: %s の検知が解除されました"), *GetOwner()->GetName());
		OnDetectionCleared.Broadcast();
	}
}

void USurveillanceDetectorComponent::ResetDetectionLevel()
{
	DetectionLevel = 0.0f;
	bWasDetected = false;
}

void USurveillanceDetectorComponent::SetDetectionLevel(float NewLevel)
{
	DetectionLevel = FMath::Clamp(NewLevel, 0.0f, 1.0f);
}

FVector USurveillanceDetectorComponent::GetDetectionLocation() const
{
	if (AActor* Owner = GetOwner())
	{
		return Owner->GetActorLocation() + DetectionOffset;
	}
	return FVector::ZeroVector;
}

USurveillanceSubsystem* USurveillanceDetectorComponent::GetSurveillanceSubsystem()
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
