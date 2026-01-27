// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#include "DawnlightGameMode.h"
#include "Subsystems/AnimalSpawnerSubsystem.h"
#include "Dawnlight.h"
#include "Subsystems/NightProgressSubsystem.h"
#include "Subsystems/SoulCollectionSubsystem.h"
#include "Subsystems/UpgradeSubsystem.h"
#include "Subsystems/WaveSpawnerSubsystem.h"
#include "Abilities/DawnlightAttributeSet.h"
#include "Characters/DawnlightCharacter.h"
#include "Characters/EnemyCharacter.h"
#include "Data/EnemyDataAsset.h"
#include "UI/Widgets/GameplayHUDWidget.h"
#include "UI/Widgets/GameResultWidget.h"
#include "UI/Widgets/UpgradeSelectionWidget.h"
#include "UI/Widgets/SetBonusDisplayWidget.h"
#include "UI/LevelTransitionSubsystem.h"
#include "Data/UpgradeDataAsset.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

ADawnlightGameMode::ADawnlightGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	// ========================================================================
	// デフォルト値 - Night Phase
	// ========================================================================
	NightPhaseDuration = 180.0f;  // 3分
	bSpawnAnimals = true;
	AnimalSpawnInterval = 10.0f;
	MaxAnimalCount = 5;

	// ========================================================================
	// デフォルト値 - Dawn Phase
	// ========================================================================
	TotalWaves = 3;
	EnemiesPerWave = { 5, 8, 12 };  // デモ用: 3Wave
	WaveInterval = 5.0f;
	DawnTransitionDuration = 3.0f;

	// ========================================================================
	// 自動開始設定
	// ========================================================================
	bAutoStart = true;
	AutoStartDelay = 2.0f;

	// ========================================================================
	// 状態初期化
	// ========================================================================
	CurrentPhase = EGamePhase::None;
	CurrentWave = 0;
	RemainingEnemies = 0;
	NightPhaseTimeRemaining = 0.0f;
	CurrentAnimalCount = 0;
}

void ADawnlightGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] BeginPlay"));

	// サブシステムを初期化
	InitializeSubsystems();

	// 自動開始が有効な場合
	if (bAutoStart)
	{
		GetWorld()->GetTimerManager().SetTimer(
			AutoStartTimerHandle,
			this,
			&ADawnlightGameMode::StartGame,
			AutoStartDelay,
			false
		);

		UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] ゲーム自動開始を%.1f秒後に予約"), AutoStartDelay);
	}
}

void ADawnlightGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Night Phase中のカウントダウン
	if (CurrentPhase == EGamePhase::Night)
	{
		NightPhaseTimeRemaining -= DeltaTime;

		if (NightPhaseTimeRemaining <= 0.0f)
		{
			NightPhaseTimeRemaining = 0.0f;
			EndNightPhase();
		}
	}
}

void ADawnlightGameMode::InitializeSubsystems()
{
	if (UWorld* World = GetWorld())
	{
		NightProgressSubsystem = World->GetSubsystem<UNightProgressSubsystem>();
		SoulCollectionSubsystem = World->GetSubsystem<USoulCollectionSubsystem>();
		UpgradeSubsystem = World->GetSubsystem<UUpgradeSubsystem>();

		if (NightProgressSubsystem.IsValid())
		{
			UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] NightProgressSubsystem を取得"));
		}

		if (SoulCollectionSubsystem.IsValid())
		{
			UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] SoulCollectionSubsystem を取得"));
		}

		if (UpgradeSubsystem.IsValid())
		{
			UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] UpgradeSubsystem を取得"));
		}

		WaveSpawnerSubsystem = World->GetSubsystem<UWaveSpawnerSubsystem>();
		if (WaveSpawnerSubsystem.IsValid())
		{
			UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] WaveSpawnerSubsystem を取得"));

			// ウェーブイベントをバインド
			WaveSpawnerSubsystem->OnWaveCompleted.AddDynamic(this, &ADawnlightGameMode::OnWaveSpawnerWaveCompleted);
			WaveSpawnerSubsystem->OnAllWavesCompleted.AddDynamic(this, &ADawnlightGameMode::OnWaveSpawnerAllWavesCompleted);
			WaveSpawnerSubsystem->OnEnemyKilled.AddDynamic(this, &ADawnlightGameMode::OnWaveSpawnerEnemyKilled);
		}

		AnimalSpawnerSubsystem = World->GetSubsystem<UAnimalSpawnerSubsystem>();
		if (AnimalSpawnerSubsystem.IsValid())
		{
			UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] AnimalSpawnerSubsystem を取得"));
		}
	}

	// アップグレードウィジェットを初期化
	InitializeUpgradeWidgets();
}

// ========================================================================
// ゲームフロー
// ========================================================================

void ADawnlightGameMode::StartGame()
{
	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] ゲーム開始"));

	// Night Phaseから開始
	StartNightPhase();
}

void ADawnlightGameMode::StartNightPhase()
{
	if (CurrentPhase == EGamePhase::Night)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[SoulReaperGameMode] Night Phaseは既に開始しています"));
		return;
	}

	SetPhase(EGamePhase::Night);

	// 時間をセット
	NightPhaseTimeRemaining = NightPhaseDuration;

	// 魂コレクションをクリア
	if (SoulCollectionSubsystem.IsValid())
	{
		SoulCollectionSubsystem->ClearSouls();
	}

	// HUDを表示
	ShowGameplayHUD();

	// 動物スポーンタイマーを開始
	if (bSpawnAnimals)
	{
		GetWorld()->GetTimerManager().SetTimer(
			AnimalSpawnTimerHandle,
			this,
			&ADawnlightGameMode::SpawnAnimal,
			AnimalSpawnInterval,
			true,  // ループ
			1.0f   // 初回遅延
		);
	}

	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] Night Phase開始（時間: %.0f秒）"), NightPhaseDuration);

	// BP側のイベント呼び出し
	BP_OnNightPhaseStarted();
}

void ADawnlightGameMode::EndNightPhase()
{
	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] Night Phase終了"));

	// 動物スポーンタイマーを停止
	GetWorld()->GetTimerManager().ClearTimer(AnimalSpawnTimerHandle);

	// Dawn Transition演出へ
	StartDawnTransition();
}

void ADawnlightGameMode::StartDawnTransition()
{
	// 既に遷移中または Dawn Phase の場合はスキップ
	if (CurrentPhase == EGamePhase::DawnTransition || CurrentPhase == EGamePhase::Dawn)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[SoulReaperGameMode] Dawn Transitionは既に開始しているか、Dawn Phaseです"));
		return;
	}

	SetPhase(EGamePhase::DawnTransition);

	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] Dawn Transition開始（%.1f秒）"), DawnTransitionDuration);

	// 収集した魂のバフを適用
	ApplyCollectedSoulBuffs();

	// 一定時間後にDawn Phaseへ
	GetWorld()->GetTimerManager().SetTimer(
		DawnTransitionTimerHandle,
		this,
		&ADawnlightGameMode::OnDawnTransitionComplete,
		DawnTransitionDuration,
		false
	);
}

void ADawnlightGameMode::OnDawnTransitionComplete()
{
	StartDawnPhase();
}

void ADawnlightGameMode::StartDawnPhase()
{
	if (CurrentPhase == EGamePhase::Dawn)
	{
		return;
	}

	SetPhase(EGamePhase::Dawn);

	// Wave初期化
	CurrentWave = 0;

	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] Dawn Phase開始（総Wave数: %d）"), TotalWaves);

	// WaveSpawnerSubsystemを初期化
	if (WaveSpawnerSubsystem.IsValid())
	{
		// デフォルト敵データを設定
		if (DefaultEnemyData)
		{
			WaveSpawnerSubsystem->SetDefaultEnemyData(DefaultEnemyData);
			UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] デフォルト敵データを設定: %s"), *DefaultEnemyData->DisplayName.ToString());
		}

		// ウェーブ設定を生成
		TArray<FWaveConfig> WaveConfigsArray;
		for (int32 i = 0; i < TotalWaves; i++)
		{
			FWaveConfig Config;
			Config.WaveNumber = i + 1;
			Config.TotalEnemies = EnemiesPerWave.IsValidIndex(i) ? EnemiesPerWave[i] : 5;
			Config.MaxConcurrentEnemies = FMath::Min(Config.TotalEnemies, 5);  // 同時に最大5体
			Config.SpawnInterval = 2.0f;
			Config.HealthMultiplier = 1.0f + (i * 0.2f);  // ウェーブごとにHP増加
			Config.DamageMultiplier = 1.0f + (i * 0.1f);  // ウェーブごとにダメージ増加

			WaveConfigsArray.Add(Config);
		}

		WaveSpawnerSubsystem->InitializeWaveSystem(WaveConfigsArray);

		UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] WaveSpawnerSubsystemを初期化（%d ウェーブ）"), TotalWaves);
	}

	// BP側のイベント呼び出し
	BP_OnDawnPhaseStarted();

	// 最初のWaveを開始
	StartNextWave();
}

void ADawnlightGameMode::SetPhase(EGamePhase NewPhase)
{
	if (CurrentPhase == NewPhase)
	{
		return;
	}

	EGamePhase OldPhase = CurrentPhase;
	CurrentPhase = NewPhase;

	// デリゲート発火
	OnPhaseChanged.Broadcast(OldPhase, NewPhase);

	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] フェーズ変更: %d → %d"),
		static_cast<int32>(OldPhase), static_cast<int32>(NewPhase));
}

// ========================================================================
// Wave管理
// ========================================================================

void ADawnlightGameMode::StartNextWave()
{
	if (CurrentPhase != EGamePhase::Dawn)
	{
		return;
	}

	CurrentWave++;

	if (CurrentWave > TotalWaves)
	{
		// 全Wave完了
		OnGameCleared();
		return;
	}

	// Wave敵数を取得
	int32 EnemyCount = 5;  // デフォルト
	if (EnemiesPerWave.IsValidIndex(CurrentWave - 1))
	{
		EnemyCount = EnemiesPerWave[CurrentWave - 1];
	}

	RemainingEnemies = EnemyCount;

	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] Wave %d/%d 開始（敵数: %d）"),
		CurrentWave, TotalWaves, EnemyCount);

	// デリゲート発火
	OnWaveStarted.Broadcast(CurrentWave);

	// WaveSpawnerSubsystemでウェーブを開始
	if (WaveSpawnerSubsystem.IsValid())
	{
		if (CurrentWave == 1)
		{
			WaveSpawnerSubsystem->StartFirstWave();
		}
		else
		{
			WaveSpawnerSubsystem->StartNextWave();
		}
	}

	// BP側のイベント呼び出し（追加のカスタマイズ用）
	BP_OnWaveStarted(CurrentWave, EnemyCount);
}

void ADawnlightGameMode::OnEnemyKilled()
{
	// @deprecated: WaveSpawnerSubsystem経由での敵撃破管理を推奨
	// この関数は後方互換性のために残されていますが、
	// WaveSpawnerと二重カウントになる可能性があります。

	if (CurrentPhase != EGamePhase::Dawn || RemainingEnemies <= 0)
	{
		return;
	}

	// WaveSpawnerSubsystemが有効な場合は、そちらに処理を委譲
	if (WaveSpawnerSubsystem.IsValid())
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[SoulReaperGameMode] OnEnemyKilled()は非推奨です。WaveSpawnerSubsystem経由での敵撃破管理を使用してください。"));
		return;
	}

	// フォールバック: WaveSpawnerSubsystemがない場合のみローカルで処理
	RemainingEnemies--;
	UE_LOG(LogDawnlight, Verbose, TEXT("[SoulReaperGameMode] 敵撃破（フォールバック） - 残り: %d"), RemainingEnemies);

	CheckWaveCompletion();
}

void ADawnlightGameMode::CheckWaveCompletion()
{
	if (RemainingEnemies > 0)
	{
		return;
	}

	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] Wave %d 完了"), CurrentWave);

	// デリゲート発火
	OnWaveCompleted.Broadcast(CurrentWave);

	// BP側のイベント呼び出し
	BP_OnWaveCompleted(CurrentWave);

	// 次のWaveへ（最終Wave以外はアップグレード選択を挟む）
	if (CurrentWave < TotalWaves)
	{
		// アップグレード選択画面を表示
		ShowUpgradeSelection(CurrentWave);
	}
	else
	{
		// 全Wave完了
		OnGameCleared();
	}
}

// ========================================================================
// ゲーム終了
// ========================================================================

void ADawnlightGameMode::OnPlayerDeath()
{
	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] プレイヤー死亡 - ゲームオーバー"));

	SetPhase(EGamePhase::LoopEnd);

	// タイマーをクリア
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	// HUDを非表示
	HideGameplayHUD();

	// 敗北画面を表示
	ShowResultScreen(false);

	// デリゲート発火
	OnGameOver.Broadcast();

	// BP側のイベント呼び出し
	BP_OnGameOver();
}

void ADawnlightGameMode::OnGameCleared()
{
	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] ゲームクリア！"));

	SetPhase(EGamePhase::LoopEnd);

	// タイマーをクリア
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	// HUDを非表示
	HideGameplayHUD();

	// 勝利画面を表示
	ShowResultScreen(true);

	// デリゲート発火
	OnGameClear.Broadcast();

	// BP側のイベント呼び出し
	BP_OnGameClear();
}

void ADawnlightGameMode::RestartGame()
{
	// タイマーをクリア
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	// ウィジェットをクリーンアップ
	HideGameplayHUD();
	if (ResultWidget)
	{
		ResultWidget->RemoveFromParent();
		ResultWidget = nullptr;
	}

	// 状態をリセット
	CurrentPhase = EGamePhase::None;
	CurrentWave = 0;
	RemainingEnemies = 0;
	NightPhaseTimeRemaining = 0.0f;
	CurrentAnimalCount = 0;

	// レベルをリスタート
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (ULevelTransitionSubsystem* LevelTransition = GameInstance->GetSubsystem<ULevelTransitionSubsystem>())
			{
				LevelTransition->RestartCurrentLevel();
				return;
			}
		}

		// フォールバック：直接レベルをリロード
		UGameplayStatics::OpenLevel(World, FName(*World->GetMapName()));
	}

	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] ゲームをリスタート"));
}

void ADawnlightGameMode::ReturnToMainMenu()
{
	// タイマーをクリア
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	// メインメニューに遷移
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (ULevelTransitionSubsystem* LevelTransition = GameInstance->GetSubsystem<ULevelTransitionSubsystem>())
			{
				LevelTransition->TransitionToMainMenu();
				return;
			}
		}
	}

	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] メインメニューに戻る"));
}

// ========================================================================
// UI
// ========================================================================

void ADawnlightGameMode::ShowGameplayHUD()
{
	if (GameplayHUDWidget)
	{
		GameplayHUDWidget->Show();
		return;
	}

	if (!GameplayHUDWidgetClass)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[SoulReaperGameMode] GameplayHUDWidgetClass が設定されていません"));
		return;
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	GameplayHUDWidget = CreateWidget<UGameplayHUDWidget>(PC, GameplayHUDWidgetClass);
	if (GameplayHUDWidget)
	{
		GameplayHUDWidget->AddToViewport(0);
		GameplayHUDWidget->Show();

		UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] ゲームプレイHUDを表示"));
	}
}

void ADawnlightGameMode::HideGameplayHUD()
{
	if (GameplayHUDWidget)
	{
		GameplayHUDWidget->Hide();
	}
}

void ADawnlightGameMode::ShowResultScreen(bool bVictory)
{
	if (ResultWidget)
	{
		// 既に存在する場合は再利用
		int32 TotalSouls = SoulCollectionSubsystem.IsValid() ? SoulCollectionSubsystem->GetTotalSoulCount() : 0;
		int32 WavesCleared = bVictory ? TotalWaves : FMath::Max(0, CurrentWave - 1);
		ResultWidget->ShowResult(
			bVictory ? EGameResult::Victory : EGameResult::Defeat,
			TotalSouls,
			WavesCleared,
			TotalWaves
		);
		return;
	}

	if (!ResultWidgetClass)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[SoulReaperGameMode] ResultWidgetClass が設定されていません"));
		return;
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	ResultWidget = CreateWidget<UGameResultWidget>(PC, ResultWidgetClass);
	if (ResultWidget)
	{
		ResultWidget->AddToViewport(10);  // HUDより上に表示

		// イベントをバインド
		ResultWidget->OnRestartRequested.AddDynamic(this, &ADawnlightGameMode::RestartGame);
		ResultWidget->OnMainMenuRequested.AddDynamic(this, &ADawnlightGameMode::ReturnToMainMenu);

		// 結果を表示
		int32 TotalSouls = SoulCollectionSubsystem.IsValid() ? SoulCollectionSubsystem->GetTotalSoulCount() : 0;
		int32 WavesCleared = bVictory ? TotalWaves : FMath::Max(0, CurrentWave - 1);
		ResultWidget->ShowResult(
			bVictory ? EGameResult::Victory : EGameResult::Defeat,
			TotalSouls,
			WavesCleared,
			TotalWaves
		);

		// 入力モードをUIに変更
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(ResultWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);

		UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] 結果画面を表示: %s"),
			bVictory ? TEXT("勝利") : TEXT("敗北"));
	}
}

// ========================================================================
// 動物スポーン
// ========================================================================

void ADawnlightGameMode::SpawnAnimal()
{
	if (CurrentPhase != EGamePhase::Night)
	{
		return;
	}

	if (CurrentAnimalCount >= MaxAnimalCount)
	{
		UE_LOG(LogDawnlight, Verbose, TEXT("[SoulReaperGameMode] 最大動物数に達しています（%d/%d）"), CurrentAnimalCount, MaxAnimalCount);
		return;
	}

	// AnimalSpawnerSubsystemを使用してスポーン
	if (AnimalSpawnerSubsystem.IsValid())
	{
		if (AnimalSpawnerSubsystem->SpawnRandomAnimal())
		{
			CurrentAnimalCount++;
			UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] 動物をスポーン（現在: %d/%d）"), CurrentAnimalCount, MaxAnimalCount);
		}
	}
	else
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[SoulReaperGameMode] AnimalSpawnerSubsystemが無効です"));
	}
}

// ========================================================================
// バフ適用
// ========================================================================

void ADawnlightGameMode::ApplyCollectedSoulBuffs()
{
	if (!SoulCollectionSubsystem.IsValid())
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[SoulReaperGameMode] SoulCollectionSubsystemが無効です"));
		return;
	}

	// プレイヤーキャラクターの属性セットを取得
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn)
	{
		return;
	}

	ADawnlightCharacter* PlayerCharacter = Cast<ADawnlightCharacter>(PlayerPawn);
	if (!PlayerCharacter)
	{
		return;
	}

	// AttributeSetを取得してバフを適用
	UDawnlightAttributeSet* AttributeSet = PlayerCharacter->GetDawnlightAttributeSet();
	if (!AttributeSet)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[SoulReaperGameMode] AttributeSetが見つかりません"));
		return;
	}

	// 収集した魂のバフを適用
	SoulCollectionSubsystem->ApplyCollectedBuffs(AttributeSet);

	int32 TotalSouls = SoulCollectionSubsystem->GetTotalSoulCount();
	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] 収集した魂のバフを適用完了（総魂数: %d）"), TotalSouls);
}

// ========================================================================
// アップグレード選択
// ========================================================================

void ADawnlightGameMode::InitializeUpgradeWidgets()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	// アップグレード選択ウィジェットを作成
	if (UpgradeSelectionWidgetClass && !UpgradeSelectionWidget)
	{
		UpgradeSelectionWidget = CreateWidget<UUpgradeSelectionWidget>(PC, UpgradeSelectionWidgetClass);
		if (UpgradeSelectionWidget)
		{
			UpgradeSelectionWidget->AddToViewport(5);  // HUDより上、結果画面より下
			UpgradeSelectionWidget->SetVisibility(ESlateVisibility::Collapsed);

			// 選択完了イベントをバインド
			UpgradeSelectionWidget->OnSelectionComplete.AddDynamic(this, &ADawnlightGameMode::OnUpgradeSelectionComplete);

			UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] UpgradeSelectionWidget を作成"));
		}
	}

	// セットボーナス表示ウィジェットを作成（HUDの一部として）
	if (SetBonusDisplayWidgetClass && !SetBonusDisplayWidget)
	{
		SetBonusDisplayWidget = CreateWidget<USetBonusDisplayWidget>(PC, SetBonusDisplayWidgetClass);
		if (SetBonusDisplayWidget)
		{
			SetBonusDisplayWidget->AddToViewport(1);  // HUDより下
			SetBonusDisplayWidget->SetVisibility(ESlateVisibility::Collapsed);

			UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] SetBonusDisplayWidget を作成"));
		}
	}
}

void ADawnlightGameMode::ShowUpgradeSelection(int32 WaveNumber)
{
	if (!UpgradeSelectionWidget || !UpgradeSubsystem.IsValid())
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[SoulReaperGameMode] UpgradeSelectionWidget または UpgradeSubsystem が無効"));

		// ウィジェットがなければ次のWaveを直接開始
		GetWorld()->GetTimerManager().SetTimer(
			WaveIntervalTimerHandle,
			this,
			&ADawnlightGameMode::StartNextWave,
			WaveInterval,
			false
		);
		return;
	}

	// アップグレード選択肢を生成
	TArray<UUpgradeDataAsset*> Choices = UpgradeSubsystem->GenerateUpgradeChoices(WaveNumber, 3);

	if (Choices.Num() == 0)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[SoulReaperGameMode] 利用可能なアップグレードがありません"));

		// 選択肢がなければ次のWaveを直接開始
		GetWorld()->GetTimerManager().SetTimer(
			WaveIntervalTimerHandle,
			this,
			&ADawnlightGameMode::StartNextWave,
			WaveInterval,
			false
		);
		return;
	}

	// ウィジェットに選択肢を渡して表示
	UpgradeSelectionWidget->ShowWithChoices(Choices, WaveNumber);

	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] アップグレード選択画面を表示（Wave: %d）"), WaveNumber);
}

void ADawnlightGameMode::HideUpgradeSelection()
{
	if (UpgradeSelectionWidget)
	{
		UpgradeSelectionWidget->ClearAndHide();
	}
}

void ADawnlightGameMode::OnUpgradeSelectionComplete(UUpgradeDataAsset* SelectedUpgrade)
{
	// ウィジェットを非表示
	HideUpgradeSelection();

	if (SelectedUpgrade)
	{
		UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] アップグレード選択完了: %s"),
			*SelectedUpgrade->DisplayName.ToString());
	}
	else
	{
		UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] アップグレードをスキップ"));
	}

	// 次のWaveを開始
	GetWorld()->GetTimerManager().SetTimer(
		WaveIntervalTimerHandle,
		this,
		&ADawnlightGameMode::StartNextWave,
		WaveInterval,
		false
	);
}

// ========================================================================
// WaveSpawnerSubsystem コールバック
// ========================================================================

void ADawnlightGameMode::OnWaveSpawnerWaveCompleted(int32 WaveNumber, bool bSuccess)
{
	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] WaveSpawnerからウェーブ完了通知: Wave %d, 成功: %s"),
		WaveNumber, bSuccess ? TEXT("はい") : TEXT("いいえ"));

	if (!bSuccess)
	{
		// ウェーブ失敗時はゲームオーバー
		OnPlayerDeath();
		return;
	}

	// GameModeのウェーブ完了デリゲートを発火
	OnWaveCompleted.Broadcast(WaveNumber);

	// BP側のイベント呼び出し
	BP_OnWaveCompleted(WaveNumber);

	// 次のWaveへ（最終Wave以外はアップグレード選択を挟む）
	if (WaveNumber < TotalWaves)
	{
		// アップグレード選択画面を表示
		ShowUpgradeSelection(WaveNumber);
	}
	// 最終ウェーブ完了は OnWaveSpawnerAllWavesCompleted で処理
}

void ADawnlightGameMode::OnWaveSpawnerAllWavesCompleted()
{
	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] WaveSpawnerから全ウェーブ完了通知"));

	// ゲームクリア処理
	OnGameCleared();
}

void ADawnlightGameMode::OnWaveSpawnerEnemyKilled(AEnemyCharacter* Enemy)
{
	if (CurrentPhase != EGamePhase::Dawn)
	{
		return;
	}

	// WaveSpawnerSubsystemから残り敵数を取得して同期
	if (WaveSpawnerSubsystem.IsValid())
	{
		RemainingEnemies = WaveSpawnerSubsystem->GetRemainingEnemiesInWave();
	}
	else
	{
		// フォールバック: ローカルカウントを減らす
		if (RemainingEnemies > 0)
		{
			RemainingEnemies--;
		}
	}

	UE_LOG(LogDawnlight, Verbose, TEXT("[SoulReaperGameMode] 敵撃破（WaveSpawner経由） - 残り: %d"), RemainingEnemies);

	// 注: ウェーブ完了判定はWaveSpawnerSubsystem内で行われ、
	// OnWaveSpawnerWaveCompleted コールバックで通知される
}
