// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayHUDViewModel.h"
#include "Dawnlight.h"
#include "Core/DawnlightGameMode.h"
#include "Subsystems/SoulCollectionSubsystem.h"
#include "Subsystems/AnimalSpawnerSubsystem.h"
#include "Subsystems/WaveSpawnerSubsystem.h"
#include "Characters/EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"

// プロパティ名定数
const FName UGameplayHUDViewModel::PROP_CurrentPhase = FName(TEXT("CurrentPhase"));
const FName UGameplayHUDViewModel::PROP_PhaseName = FName(TEXT("PhaseName"));
const FName UGameplayHUDViewModel::PROP_NightTimeRemaining = FName(TEXT("NightTimeRemaining"));
const FName UGameplayHUDViewModel::PROP_FormattedTimeRemaining = FName(TEXT("FormattedTimeRemaining"));
const FName UGameplayHUDViewModel::PROP_TotalSoulCount = FName(TEXT("TotalSoulCount"));
const FName UGameplayHUDViewModel::PROP_ReaperGaugePercent = FName(TEXT("ReaperGaugePercent"));
const FName UGameplayHUDViewModel::PROP_IsReaperModeReady = FName(TEXT("bIsReaperModeReady"));
const FName UGameplayHUDViewModel::PROP_IsReaperModeActive = FName(TEXT("bIsReaperModeActive"));
const FName UGameplayHUDViewModel::PROP_CurrentWaveNumber = FName(TEXT("CurrentWaveNumber"));
const FName UGameplayHUDViewModel::PROP_TotalWaveCount = FName(TEXT("TotalWaveCount"));
const FName UGameplayHUDViewModel::PROP_RemainingEnemies = FName(TEXT("RemainingEnemies"));
const FName UGameplayHUDViewModel::PROP_AliveAnimalCount = FName(TEXT("AliveAnimalCount"));
const FName UGameplayHUDViewModel::PROP_TotalAnimalCount = FName(TEXT("TotalAnimalCount"));
const FName UGameplayHUDViewModel::PROP_PlayerCurrentHP = FName(TEXT("PlayerCurrentHP"));
const FName UGameplayHUDViewModel::PROP_PlayerMaxHP = FName(TEXT("PlayerMaxHP"));
const FName UGameplayHUDViewModel::PROP_PlayerHPPercent = FName(TEXT("PlayerHPPercent"));
const FName UGameplayHUDViewModel::PROP_DamageBuffPercent = FName(TEXT("DamageBuffPercent"));
const FName UGameplayHUDViewModel::PROP_ShouldShowDawnWarning = FName(TEXT("bShouldShowDawnWarning"));

UGameplayHUDViewModel::UGameplayHUDViewModel()
	: CurrentPhase(EGamePhase::None)
	, NightTimeRemaining(0.0f)
	, TotalSoulCount(0)
	, ReaperGaugePercent(0.0f)
	, bIsReaperModeReady(false)
	, bIsReaperModeActive(false)
	, CurrentWaveNumber(0)
	, TotalWaveCount(0)
	, RemainingEnemies(0)
	, AliveAnimalCount(0)
	, TotalAnimalCount(0)
	, PlayerCurrentHP(0.0f)
	, PlayerMaxHP(100.0f)
	, PlayerHPPercent(1.0f)
	, DamageBuffPercent(0.0f)
	, bShouldShowDawnWarning(false)
{
}

void UGameplayHUDViewModel::Initialize(UWorld* InWorld)
{
	Super::Initialize(InWorld);

	if (!InWorld)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[GameplayHUDViewModel] World参照がnullです"));
		return;
	}

	// サブシステム参照を取得
	GameMode = Cast<ADawnlightGameMode>(UGameplayStatics::GetGameMode(InWorld));
	SoulSubsystem = InWorld->GetSubsystem<USoulCollectionSubsystem>();
	AnimalSubsystem = InWorld->GetSubsystem<UAnimalSpawnerSubsystem>();
	WaveSubsystem = InWorld->GetSubsystem<UWaveSpawnerSubsystem>();

	// イベントをバインド
	BindToSubsystems();

	// 初期データを同期
	SyncFromGameMode();
	SyncAnimalInfo();
	SyncWaveInfo();

	if (SoulSubsystem.IsValid())
	{
		TotalSoulCount = SoulSubsystem->GetTotalSoulCount();
	}

	// 初期状態を通知
	NotifyAllPropertiesChanged();

	UE_LOG(LogDawnlight, Log, TEXT("[GameplayHUDViewModel] 初期化完了"));
}

void UGameplayHUDViewModel::Deinitialize()
{
	UnbindFromSubsystems();
	Super::Deinitialize();
}

void UGameplayHUDViewModel::BindToSubsystems()
{
	// GameModeのイベントをバインド
	if (GameMode.IsValid())
	{
		GameMode->OnPhaseChanged.AddDynamic(this, &UGameplayHUDViewModel::HandlePhaseChanged);
		GameMode->OnWaveStarted.AddDynamic(this, &UGameplayHUDViewModel::HandleWaveStarted);
		GameMode->OnWaveCompleted.AddDynamic(this, &UGameplayHUDViewModel::HandleWaveCompleted);
	}

	// SoulCollectionSubsystemのイベントをバインド
	if (SoulSubsystem.IsValid())
	{
		SoulSubsystem->OnSoulCollected.AddDynamic(this, &UGameplayHUDViewModel::HandleSoulCollected);
	}

	// WaveSpawnerSubsystemのイベントをバインド
	if (WaveSubsystem.IsValid())
	{
		WaveSubsystem->OnEnemyKilled.AddDynamic(this, &UGameplayHUDViewModel::HandleEnemyKilled);
	}

	UE_LOG(LogDawnlight, Verbose, TEXT("[GameplayHUDViewModel] イベントバインド完了"));
}

void UGameplayHUDViewModel::UnbindFromSubsystems()
{
	// GameModeのイベントをアンバインド
	if (GameMode.IsValid())
	{
		GameMode->OnPhaseChanged.RemoveDynamic(this, &UGameplayHUDViewModel::HandlePhaseChanged);
		GameMode->OnWaveStarted.RemoveDynamic(this, &UGameplayHUDViewModel::HandleWaveStarted);
		GameMode->OnWaveCompleted.RemoveDynamic(this, &UGameplayHUDViewModel::HandleWaveCompleted);
	}

	// SoulCollectionSubsystemのイベントをアンバインド
	if (SoulSubsystem.IsValid())
	{
		SoulSubsystem->OnSoulCollected.RemoveDynamic(this, &UGameplayHUDViewModel::HandleSoulCollected);
	}

	// WaveSpawnerSubsystemのイベントをアンバインド
	if (WaveSubsystem.IsValid())
	{
		WaveSubsystem->OnEnemyKilled.RemoveDynamic(this, &UGameplayHUDViewModel::HandleEnemyKilled);
	}

	UE_LOG(LogDawnlight, Verbose, TEXT("[GameplayHUDViewModel] イベントアンバインド完了"));
}

void UGameplayHUDViewModel::HandlePhaseChanged(EGamePhase OldPhase, EGamePhase NewPhase)
{
	CurrentPhase = NewPhase;
	PhaseName = GetPhaseDisplayName(NewPhase);

	NotifyPropertyChanged(PROP_CurrentPhase);
	NotifyPropertyChanged(PROP_PhaseName);

	// フェーズ移行時にデータを同期
	SyncFromGameMode();

	// Night → Dawn移行時にWave情報を更新
	if (NewPhase == EGamePhase::Dawn)
	{
		SyncWaveInfo();
	}

	UE_LOG(LogDawnlight, Log, TEXT("[GameplayHUDViewModel] フェーズ変更: %s → %s"),
		*UEnum::GetValueAsString(OldPhase), *UEnum::GetValueAsString(NewPhase));
}

void UGameplayHUDViewModel::HandleSoulCollected(const FSoulCollectedEventData& EventData)
{
	TotalSoulCount = EventData.TotalSoulCount;
	NotifyPropertyChanged(PROP_TotalSoulCount);

	UE_LOG(LogDawnlight, Verbose, TEXT("[GameplayHUDViewModel] 魂収集: 合計 %d"), TotalSoulCount);
}

void UGameplayHUDViewModel::HandleWaveStarted(int32 WaveNumber)
{
	CurrentWaveNumber = WaveNumber;
	NotifyPropertyChanged(PROP_CurrentWaveNumber);

	// Wave情報を同期
	SyncWaveInfo();

	// Wave開始イベントを発火（アナウンスメント表示用）
	OnWaveStartedEvent.Broadcast(WaveNumber);

	UE_LOG(LogDawnlight, Log, TEXT("[GameplayHUDViewModel] Wave %d 開始"), WaveNumber);
}

void UGameplayHUDViewModel::HandleWaveCompleted(int32 WaveNumber)
{
	// Wave完了時の処理
	SyncWaveInfo();

	UE_LOG(LogDawnlight, Log, TEXT("[GameplayHUDViewModel] Wave %d 完了"), WaveNumber);
}

void UGameplayHUDViewModel::HandleEnemyKilled(AEnemyCharacter* Enemy)
{
	// 残り敵数を更新
	SyncWaveInfo();
}

void UGameplayHUDViewModel::SyncFromGameMode()
{
	if (!GameMode.IsValid())
	{
		return;
	}

	// フェーズ情報
	CurrentPhase = GameMode->GetCurrentPhase();
	PhaseName = GetPhaseDisplayName(CurrentPhase);

	// Night Phase時間
	const float OldTime = NightTimeRemaining;
	NightTimeRemaining = GameMode->GetNightPhaseTimeRemaining();
	FormattedTimeRemaining = FormatTime(NightTimeRemaining);

	// 夜明け警告判定
	const bool OldWarning = bShouldShowDawnWarning;
	bShouldShowDawnWarning = (CurrentPhase == EGamePhase::Night) &&
							  (NightTimeRemaining <= DAWN_WARNING_THRESHOLD) &&
							  (NightTimeRemaining > 0.0f);

	// 変更通知（値が変わった場合のみ）
	if (OldTime != NightTimeRemaining)
	{
		NotifyPropertyChanged(PROP_NightTimeRemaining);
		NotifyPropertyChanged(PROP_FormattedTimeRemaining);
	}

	if (OldWarning != bShouldShowDawnWarning)
	{
		NotifyPropertyChanged(PROP_ShouldShowDawnWarning);
	}
}

void UGameplayHUDViewModel::SyncAnimalInfo()
{
	if (!AnimalSubsystem.IsValid())
	{
		return;
	}

	const int32 OldAlive = AliveAnimalCount;
	const int32 OldTotal = TotalAnimalCount;

	AliveAnimalCount = AnimalSubsystem->GetAliveAnimalCount();
	TotalAnimalCount = AnimalSubsystem->GetTotalSpawnedCount();

	if (OldAlive != AliveAnimalCount)
	{
		NotifyPropertyChanged(PROP_AliveAnimalCount);
	}

	if (OldTotal != TotalAnimalCount)
	{
		NotifyPropertyChanged(PROP_TotalAnimalCount);
	}
}

void UGameplayHUDViewModel::SyncWaveInfo()
{
	if (!WaveSubsystem.IsValid())
	{
		return;
	}

	const int32 OldWave = CurrentWaveNumber;
	const int32 OldTotal = TotalWaveCount;
	const int32 OldEnemies = RemainingEnemies;

	CurrentWaveNumber = WaveSubsystem->GetCurrentWaveNumber();
	TotalWaveCount = WaveSubsystem->GetTotalWaveCount();
	RemainingEnemies = WaveSubsystem->GetRemainingEnemiesInWave();

	if (OldWave != CurrentWaveNumber)
	{
		NotifyPropertyChanged(PROP_CurrentWaveNumber);
	}

	if (OldTotal != TotalWaveCount)
	{
		NotifyPropertyChanged(PROP_TotalWaveCount);
	}

	if (OldEnemies != RemainingEnemies)
	{
		NotifyPropertyChanged(PROP_RemainingEnemies);
	}
}

void UGameplayHUDViewModel::UpdatePlayerHealth(float CurrentHP, float MaxHP)
{
	const float OldCurrent = PlayerCurrentHP;
	const float OldMax = PlayerMaxHP;
	const float OldPercent = PlayerHPPercent;

	PlayerCurrentHP = CurrentHP;
	PlayerMaxHP = MaxHP;
	PlayerHPPercent = (MaxHP > 0.0f) ? FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f) : 0.0f;

	if (!FMath::IsNearlyEqual(OldCurrent, PlayerCurrentHP))
	{
		NotifyPropertyChanged(PROP_PlayerCurrentHP);

		// ダメージを受けた場合イベント発火
		if (PlayerCurrentHP < OldCurrent)
		{
			OnPlayerDamaged.Broadcast(OldCurrent - PlayerCurrentHP, PlayerCurrentHP);
		}
	}

	if (!FMath::IsNearlyEqual(OldMax, PlayerMaxHP))
	{
		NotifyPropertyChanged(PROP_PlayerMaxHP);
	}

	if (!FMath::IsNearlyEqual(OldPercent, PlayerHPPercent))
	{
		NotifyPropertyChanged(PROP_PlayerHPPercent);
	}
}

void UGameplayHUDViewModel::UpdateReaperGauge(float NormalizedValue)
{
	const float OldValue = ReaperGaugePercent;
	const bool OldReady = bIsReaperModeReady;

	ReaperGaugePercent = FMath::Clamp(NormalizedValue, 0.0f, 1.0f);
	bIsReaperModeReady = (ReaperGaugePercent >= 1.0f);

	if (!FMath::IsNearlyEqual(OldValue, ReaperGaugePercent))
	{
		NotifyPropertyChanged(PROP_ReaperGaugePercent);
	}

	if (OldReady != bIsReaperModeReady)
	{
		NotifyPropertyChanged(PROP_IsReaperModeReady);
	}
}

void UGameplayHUDViewModel::UpdateDamageBuff(float BuffPercent)
{
	if (!FMath::IsNearlyEqual(DamageBuffPercent, BuffPercent))
	{
		DamageBuffPercent = BuffPercent;
		NotifyPropertyChanged(PROP_DamageBuffPercent);
	}
}

void UGameplayHUDViewModel::SetReaperModeActive(bool bActive)
{
	if (bIsReaperModeActive != bActive)
	{
		bIsReaperModeActive = bActive;
		NotifyPropertyChanged(PROP_IsReaperModeActive);
		OnReaperModeChanged.Broadcast(bActive);

		UE_LOG(LogDawnlight, Log, TEXT("[GameplayHUDViewModel] リーパーモード: %s"),
			bActive ? TEXT("発動") : TEXT("終了"));
	}
}

FText UGameplayHUDViewModel::FormatTime(float Seconds) const
{
	const int32 Minutes = FMath::FloorToInt(Seconds / 60.0f);
	const int32 Secs = FMath::FloorToInt(FMath::Fmod(Seconds, 60.0f));

	return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Secs));
}

FText UGameplayHUDViewModel::GetPhaseDisplayName(EGamePhase Phase) const
{
	switch (Phase)
	{
	case EGamePhase::Night:
		return FText::FromString(TEXT("NIGHT PHASE"));
	case EGamePhase::DawnTransition:
		return FText::FromString(TEXT("DAWN APPROACHES..."));
	case EGamePhase::Dawn:
		return FText::FromString(TEXT("DAWN PHASE"));
	case EGamePhase::LoopEnd:
		return FText::FromString(TEXT("LOOP COMPLETE"));
	default:
		return FText::FromString(TEXT("---"));
	}
}
