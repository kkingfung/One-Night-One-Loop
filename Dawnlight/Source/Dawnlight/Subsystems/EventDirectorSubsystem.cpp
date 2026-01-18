// Copyright Epic Games, Inc. All Rights Reserved.

#include "EventDirectorSubsystem.h"
#include "Dawnlight.h"
#include "DawnlightTags.h"
#include "EventDataAsset.h"

void UEventDirectorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentPhase = EPhaseType::Introduction;
	CurrentTension = 0.0f;
	FixedEvents.Empty();
	CompletedFixedEvents.Empty();
	RandomEventPool.Empty();
	RandomEventUsageCount.Empty();
	EventSelectionLog.Empty();

	UE_LOG(LogDawnlight, Log, TEXT("EventDirectorSubsystem: 初期化しました"));
}

void UEventDirectorSubsystem::Deinitialize()
{
	UE_LOG(LogDawnlight, Log, TEXT("EventDirectorSubsystem: 終了しました"));

	Super::Deinitialize();
}

FGameplayTag UEventDirectorSubsystem::GetCurrentPhaseTag() const
{
	switch (CurrentPhase)
	{
	case EPhaseType::Introduction:
		return DawnlightTags::Phase_Night_Introduction;
	case EPhaseType::Relaxation:
		return DawnlightTags::Phase_Night_Relaxation;
	case EPhaseType::Climax:
		return DawnlightTags::Phase_Night_Climax;
	default:
		return DawnlightTags::Phase_Night_Introduction;
	}
}

void UEventDirectorSubsystem::SetPhase(EPhaseType NewPhase)
{
	if (CurrentPhase == NewPhase)
	{
		return;
	}

	const EPhaseType OldPhase = CurrentPhase;
	CurrentPhase = NewPhase;

	AddToLog(FString::Printf(TEXT("フェーズ変更: %d → %d"), static_cast<int32>(OldPhase), static_cast<int32>(NewPhase)));

	UE_LOG(LogDawnlight, Log, TEXT("EventDirectorSubsystem: フェーズが変更されました %d → %d"),
		static_cast<int32>(OldPhase), static_cast<int32>(NewPhase));

	OnPhaseChanged.Broadcast(OldPhase, NewPhase);
}

void UEventDirectorSubsystem::AdvancePhase()
{
	switch (CurrentPhase)
	{
	case EPhaseType::Introduction:
		SetPhase(EPhaseType::Relaxation);
		break;
	case EPhaseType::Relaxation:
		SetPhase(EPhaseType::Climax);
		break;
	case EPhaseType::Climax:
		// クライマックス以降は進まない
		break;
	}
}

void UEventDirectorSubsystem::RegisterFixedEvent(UEventDataAsset* EventAsset, int32 Order)
{
	if (!EventAsset)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("EventDirectorSubsystem: 無効な固定イベントが渡されました"));
		return;
	}

	// 順序を確保するためリサイズ
	if (FixedEvents.Num() <= Order)
	{
		FixedEvents.SetNum(Order + 1);
	}

	FixedEvents[Order] = EventAsset;

	AddToLog(FString::Printf(TEXT("固定イベント登録: %s (順序: %d)"), *EventAsset->GetName(), Order));

	UE_LOG(LogDawnlight, Log, TEXT("EventDirectorSubsystem: 固定イベント %s を登録しました (順序: %d)"),
		*EventAsset->GetName(), Order);
}

void UEventDirectorSubsystem::MarkFixedEventCompleted(FGameplayTag EventTag)
{
	CompletedFixedEvents.Add(EventTag);

	AddToLog(FString::Printf(TEXT("固定イベント完了: %s"), *EventTag.ToString()));

	UE_LOG(LogDawnlight, Log, TEXT("EventDirectorSubsystem: 固定イベント %s を完了しました"), *EventTag.ToString());
}

bool UEventDirectorSubsystem::IsFixedEventCompleted(FGameplayTag EventTag) const
{
	return CompletedFixedEvents.Contains(EventTag);
}

UEventDataAsset* UEventDirectorSubsystem::GetNextFixedEvent() const
{
	for (const TObjectPtr<UEventDataAsset>& EventAsset : FixedEvents)
	{
		if (EventAsset && !IsFixedEventCompleted(EventAsset->EventTag))
		{
			return EventAsset;
		}
	}

	return nullptr;
}

void UEventDirectorSubsystem::SetRandomEventPool(const TArray<UEventDataAsset*>& EventPool)
{
	RandomEventPool.Empty();

	for (UEventDataAsset* EventAsset : EventPool)
	{
		if (EventAsset)
		{
			RandomEventPool.Add(EventAsset);
		}
	}

	UE_LOG(LogDawnlight, Log, TEXT("EventDirectorSubsystem: ランダムイベントプールを設定しました (%d件)"),
		RandomEventPool.Num());
}

FEventSelectionResult UEventDirectorSubsystem::SelectRandomEvent()
{
	FEventSelectionResult Result;

	if (RandomEventPool.Num() == 0)
	{
		AddToLog(TEXT("ランダムイベント選択: プールが空です"));
		return Result;
	}

	// スコアに基づいて選択
	UEventDataAsset* BestEvent = nullptr;
	float BestScore = -1.0f;

	for (const TObjectPtr<UEventDataAsset>& EventAsset : RandomEventPool)
	{
		if (!EventAsset)
		{
			continue;
		}

		const float Score = CalculateEventScore(EventAsset);

		if (Score > BestScore)
		{
			BestScore = Score;
			BestEvent = EventAsset;
		}
	}

	if (BestEvent)
	{
		Result.SelectedEvent = BestEvent;
		Result.TensionAtSelection = CurrentTension;
		Result.SelectionReason = FString::Printf(TEXT("スコア: %.2f, 緊張度: %.2f"), BestScore, CurrentTension);

		AddToLog(FString::Printf(TEXT("ランダムイベント選択: %s (%s)"),
			*BestEvent->GetName(), *Result.SelectionReason));

		UE_LOG(LogDawnlight, Log, TEXT("EventDirectorSubsystem: イベント %s を選択しました (スコア: %.2f)"),
			*BestEvent->GetName(), BestScore);

		OnEventSelected.Broadcast(Result);
	}

	return Result;
}

void UEventDirectorSubsystem::RecordRandomEventUsage(FGameplayTag EventTag)
{
	int32& Count = RandomEventUsageCount.FindOrAdd(EventTag);
	Count++;

	AddToLog(FString::Printf(TEXT("イベント使用記録: %s (回数: %d)"), *EventTag.ToString(), Count));
}

void UEventDirectorSubsystem::SetTension(float NewTension)
{
	CurrentTension = FMath::Clamp(NewTension, 0.0f, 1.0f);
}

void UEventDirectorSubsystem::AddTension(float Amount)
{
	SetTension(CurrentTension + Amount);
}

void UEventDirectorSubsystem::ClearSelectionLog()
{
	EventSelectionLog.Empty();
}

void UEventDirectorSubsystem::AddToLog(const FString& Message)
{
	const FString TimestampedMessage = FString::Printf(TEXT("[%s] %s"),
		*FDateTime::Now().ToString(TEXT("%H:%M:%S")), *Message);

	EventSelectionLog.Add(TimestampedMessage);

	// ログが多すぎる場合は古いものを削除
	constexpr int32 MaxLogEntries = 100;
	if (EventSelectionLog.Num() > MaxLogEntries)
	{
		EventSelectionLog.RemoveAt(0, EventSelectionLog.Num() - MaxLogEntries);
	}
}

float UEventDirectorSubsystem::CalculateEventScore(const UEventDataAsset* EventAsset) const
{
	if (!EventAsset)
	{
		return 0.0f;
	}

	float Score = 1.0f;

	// 緊張度との差分（近いほど高スコア）
	const float TensionDiff = FMath::Abs(EventAsset->TensionLevel - CurrentTension);
	Score -= TensionDiff * 0.5f;

	// フェーズの適合性
	if (EventAsset->RequiredPhase == GetCurrentPhaseTag())
	{
		Score += 0.3f;
	}

	// 使用回数によるペナルティ
	const int32* UsageCount = RandomEventUsageCount.Find(EventAsset->EventTag);
	if (UsageCount && *UsageCount > 0)
	{
		Score -= (*UsageCount) * 0.2f;
	}

	// ランダム要素（0-0.1）
	Score += FMath::FRand() * 0.1f;

	return FMath::Max(Score, 0.0f);
}
