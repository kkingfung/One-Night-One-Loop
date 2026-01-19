// Copyright Epic Games, Inc. All Rights Reserved.

#include "NightProgressSubsystem.h"
#include "Dawnlight.h"

// ========================================================================
// UWorldSubsystem インターフェース
// ========================================================================

void UNightProgressSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 初期値を設定
	RemainingTime = 0.0f;
	TotalNightDuration = 180.0f;  // デフォルト3分
	TensionLevel = 0.0f;
	bDawnWarningIssued = false;

	UE_LOG(LogDawnlight, Log, TEXT("[NightProgressSubsystem] 初期化完了"));
}

void UNightProgressSubsystem::Deinitialize()
{
	UE_LOG(LogDawnlight, Log, TEXT("[NightProgressSubsystem] 終了処理"));
	Super::Deinitialize();
}

bool UNightProgressSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// ゲームワールドでのみ作成
	if (const UWorld* World = Cast<UWorld>(Outer))
	{
		return World->IsGameWorld();
	}
	return false;
}

// ========================================================================
// 時間情報
// ========================================================================

void UNightProgressSubsystem::SetRemainingTime(float Time)
{
	// 初回設定時に総時間を記録
	if (TotalNightDuration <= 0.0f || Time > TotalNightDuration)
	{
		TotalNightDuration = Time;
	}

	RemainingTime = FMath::Max(0.0f, Time);

	// 夜明け警告のチェック（残り30秒以下）
	if (!bDawnWarningIssued && IsDawnApproaching())
	{
		bDawnWarningIssued = true;
		OnDawnApproaching.Broadcast();
		UE_LOG(LogDawnlight, Log, TEXT("[NightProgressSubsystem] 夜明けが近づいています！残り: %.1f秒"), RemainingTime);
	}
}

float UNightProgressSubsystem::GetNightProgress() const
{
	if (TotalNightDuration <= 0.0f)
	{
		return 0.0f;
	}

	// 経過した時間の割合（0 = 開始、1 = 終了）
	return 1.0f - (RemainingTime / TotalNightDuration);
}

bool UNightProgressSubsystem::IsDawnApproaching() const
{
	// 残り30秒以下で夜明けが近い
	return RemainingTime <= 30.0f && RemainingTime > 0.0f;
}

// ========================================================================
// 緊張度
// ========================================================================

void UNightProgressSubsystem::SetTensionLevel(float Level)
{
	const float OldLevel = TensionLevel;
	TensionLevel = FMath::Clamp(Level, 0.0f, 1.0f);

	// 閾値チェック（0.5と0.8で通知）
	const TArray<float> Thresholds = { 0.5f, 0.8f };
	for (float Threshold : Thresholds)
	{
		if (OldLevel < Threshold && TensionLevel >= Threshold)
		{
			OnTensionThresholdReached.Broadcast(TensionLevel);
			UE_LOG(LogDawnlight, Log, TEXT("[NightProgressSubsystem] 緊張度が閾値を超えました: %.2f"), TensionLevel);
		}
	}
}

void UNightProgressSubsystem::AddTension(float Amount)
{
	SetTensionLevel(TensionLevel + Amount);
}
