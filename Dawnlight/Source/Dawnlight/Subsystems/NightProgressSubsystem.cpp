// Copyright Epic Games, Inc. All Rights Reserved.

#include "NightProgressSubsystem.h"
#include "Dawnlight.h"
#include "Engine/World.h"

void UNightProgressSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bNightActive = false;
	bNightPaused = false;
	bDawnWarningIssued = false;
	RemainingTime = 0.0f;
	TotalDuration = 0.0f;
	CurrentPhase = 0;

	// デフォルトのフェーズ閾値
	PhaseThresholds.Add(0.33f); // 33%で緩和フェーズへ
	PhaseThresholds.Add(0.66f); // 66%でクライマックスへ

	// 夜明け警告：残り10%
	DawnWarningThreshold = 0.1f;

	UE_LOG(LogDawnlight, Log, TEXT("NightProgressSubsystem: 初期化しました"));
}

void UNightProgressSubsystem::Deinitialize()
{
	bNightActive = false;

	UE_LOG(LogDawnlight, Log, TEXT("NightProgressSubsystem: 終了しました"));

	Super::Deinitialize();
}

void UNightProgressSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bNightActive || bNightPaused)
	{
		return;
	}

	// 時間を減らす
	RemainingTime -= DeltaTime;

	// フェーズ進行をチェック
	CheckPhaseProgression();

	// 夜明け警告
	if (!bDawnWarningIssued && IsDawnApproaching())
	{
		bDawnWarningIssued = true;
		UE_LOG(LogDawnlight, Log, TEXT("NightProgressSubsystem: 夜明けが近づいています"));
		OnDawnApproaching.Broadcast();
	}

	// 夜明け（時間切れ）
	if (RemainingTime <= 0.0f)
	{
		RemainingTime = 0.0f;
		bNightActive = false;

		UE_LOG(LogDawnlight, Log, TEXT("NightProgressSubsystem: 夜明けをトリガーしました"));
		OnDawnTriggered.Broadcast();
	}
}

TStatId UNightProgressSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UNightProgressSubsystem, STATGROUP_Tickables);
}

bool UNightProgressSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (const UWorld* World = Cast<UWorld>(Outer))
	{
		return World->IsGameWorld();
	}
	return false;
}

void UNightProgressSubsystem::StartNight(float Duration)
{
	if (Duration <= 0.0f)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("NightProgressSubsystem: 無効な時間が指定されました: %.1f"), Duration);
		return;
	}

	TotalDuration = Duration;
	RemainingTime = Duration;
	bNightActive = true;
	bNightPaused = false;
	bDawnWarningIssued = false;
	CurrentPhase = 0;

	UE_LOG(LogDawnlight, Log, TEXT("NightProgressSubsystem: 夜を開始しました (時間: %.1f秒)"), Duration);

	OnNightStarted.Broadcast();
}

void UNightProgressSubsystem::StopNight()
{
	bNightActive = false;
	bNightPaused = false;

	UE_LOG(LogDawnlight, Log, TEXT("NightProgressSubsystem: 夜を停止しました"));
}

void UNightProgressSubsystem::PauseNight()
{
	if (bNightActive && !bNightPaused)
	{
		bNightPaused = true;
		UE_LOG(LogDawnlight, Log, TEXT("NightProgressSubsystem: 夜を一時停止しました"));
	}
}

void UNightProgressSubsystem::ResumeNight()
{
	if (bNightActive && bNightPaused)
	{
		bNightPaused = false;
		UE_LOG(LogDawnlight, Log, TEXT("NightProgressSubsystem: 夜を再開しました"));
	}
}

float UNightProgressSubsystem::GetNightProgress() const
{
	if (TotalDuration <= 0.0f)
	{
		return 0.0f;
	}

	return 1.0f - (RemainingTime / TotalDuration);
}

FString UNightProgressSubsystem::GetFormattedRemainingTime() const
{
	const int32 Minutes = FMath::FloorToInt(RemainingTime / 60.0f);
	const int32 Seconds = FMath::FloorToInt(FMath::Fmod(RemainingTime, 60.0f));

	return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}

bool UNightProgressSubsystem::IsDawnApproaching() const
{
	if (TotalDuration <= 0.0f)
	{
		return false;
	}

	const float RemainingRatio = RemainingTime / TotalDuration;
	return RemainingRatio <= DawnWarningThreshold;
}

void UNightProgressSubsystem::CheckPhaseProgression()
{
	const float Progress = GetNightProgress();

	// フェーズ閾値をチェック
	for (int32 i = 0; i < PhaseThresholds.Num(); ++i)
	{
		// 現在のフェーズよりも先の閾値を超えた場合
		if (i >= CurrentPhase && Progress >= PhaseThresholds[i])
		{
			const int32 OldPhase = CurrentPhase;
			CurrentPhase = i + 1;

			UE_LOG(LogDawnlight, Log, TEXT("NightProgressSubsystem: フェーズが変更されました %d → %d (進行度: %.1f%%)"),
				OldPhase, CurrentPhase, Progress * 100.0f);

			OnPhaseChanged.Broadcast(OldPhase, CurrentPhase);
		}
	}
}
