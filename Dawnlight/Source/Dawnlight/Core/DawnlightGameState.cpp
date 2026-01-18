// Copyright Epic Games, Inc. All Rights Reserved.

#include "DawnlightGameState.h"
#include "Dawnlight.h"
#include "Net/UnrealNetwork.h"

ADawnlightGameState::ADawnlightGameState()
{
	// 初期状態
	ProgressState = EGameProgressState::NotStarted;
	NightProgress = 0.0f;
	CurrentPhase = 0;
	TensionLevel = 0.0f;

	// 統計初期化
	TotalEvidenceValue = 0.0f;
	DetectionCount = 0;
	PhotosTaken = 0;
	TimesHidden = 0;
}

void ADawnlightGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADawnlightGameState, ProgressState);
	DOREPLIFETIME(ADawnlightGameState, NightProgress);
	DOREPLIFETIME(ADawnlightGameState, CurrentPhase);
	DOREPLIFETIME(ADawnlightGameState, TensionLevel);
	DOREPLIFETIME(ADawnlightGameState, TotalEvidenceValue);
	DOREPLIFETIME(ADawnlightGameState, DetectionCount);
	DOREPLIFETIME(ADawnlightGameState, PhotosTaken);
	DOREPLIFETIME(ADawnlightGameState, TimesHidden);
	DOREPLIFETIME(ADawnlightGameState, EventCompletionRecords);
}

void ADawnlightGameState::SetProgressState(EGameProgressState NewState)
{
	if (ProgressState == NewState)
	{
		return;
	}

	const EGameProgressState OldState = ProgressState;
	ProgressState = NewState;

	UE_LOG(LogDawnlight, Log, TEXT("GameState: 進行状態が変更されました %d -> %d"),
		static_cast<int32>(OldState), static_cast<int32>(NewState));

	OnGameStateChanged.Broadcast(NewState);
}

void ADawnlightGameState::SetNightProgress(float Progress)
{
	NightProgress = FMath::Clamp(Progress, 0.0f, 1.0f);
}

void ADawnlightGameState::SetCurrentPhase(int32 Phase)
{
	if (CurrentPhase == Phase)
	{
		return;
	}

	const int32 OldPhase = CurrentPhase;
	CurrentPhase = FMath::Clamp(Phase, 0, 2);

	UE_LOG(LogDawnlight, Log, TEXT("GameState: フェーズが変更されました %d -> %d"),
		OldPhase, CurrentPhase);

	OnPhaseChanged.Broadcast(OldPhase, CurrentPhase);
}

void ADawnlightGameState::RecordEventCompletion(FGameplayTag EventTag, bool bWasSuccessful, float EvidenceGained)
{
	FEventCompletionRecord Record;
	Record.EventTag = EventTag;
	Record.bWasSuccessful = bWasSuccessful;
	Record.CompletionProgress = NightProgress;
	Record.EvidenceGained = EvidenceGained;

	EventCompletionRecords.Add(Record);

	if (EvidenceGained > 0.0f)
	{
		AddEvidenceValue(EvidenceGained);
	}

	UE_LOG(LogDawnlight, Log, TEXT("GameState: イベント完了を記録 - %s (成功: %s, 証拠: %.1f)"),
		*EventTag.ToString(), bWasSuccessful ? TEXT("Yes") : TEXT("No"), EvidenceGained);
}

bool ADawnlightGameState::IsEventCompleted(FGameplayTag EventTag) const
{
	for (const FEventCompletionRecord& Record : EventCompletionRecords)
	{
		if (Record.EventTag == EventTag)
		{
			return true;
		}
	}
	return false;
}

void ADawnlightGameState::AddEvidenceValue(float Value)
{
	TotalEvidenceValue += Value;

	UE_LOG(LogDawnlight, Log, TEXT("GameState: 証拠価値追加 +%.1f (合計: %.1f)"),
		Value, TotalEvidenceValue);
}

void ADawnlightGameState::IncrementDetectionCount()
{
	DetectionCount++;

	UE_LOG(LogDawnlight, Log, TEXT("GameState: 検知回数 %d"), DetectionCount);
}

void ADawnlightGameState::IncrementPhotosTaken()
{
	PhotosTaken++;

	UE_LOG(LogDawnlight, Log, TEXT("GameState: 撮影回数 %d"), PhotosTaken);
}

void ADawnlightGameState::IncrementTimesHidden()
{
	TimesHidden++;

	UE_LOG(LogDawnlight, Log, TEXT("GameState: 隠れた回数 %d"), TimesHidden);
}

void ADawnlightGameState::SetTensionLevel(float NewLevel)
{
	const float OldTension = TensionLevel;
	TensionLevel = FMath::Clamp(NewLevel, 0.0f, 100.0f);

	if (!FMath::IsNearlyEqual(OldTension, TensionLevel))
	{
		OnTensionChanged.Broadcast(OldTension, TensionLevel);
	}
}

void ADawnlightGameState::ModifyTensionLevel(float Delta)
{
	SetTensionLevel(TensionLevel + Delta);
}

void ADawnlightGameState::OnRep_ProgressState()
{
	OnGameStateChanged.Broadcast(ProgressState);
}

void ADawnlightGameState::OnRep_NightProgress()
{
	// クライアント側で夜の進行度が更新された時の処理
}

void ADawnlightGameState::OnRep_TensionLevel()
{
	// 前の値を保持していないため、現在の値のみで通知
	OnTensionChanged.Broadcast(TensionLevel, TensionLevel);
}
