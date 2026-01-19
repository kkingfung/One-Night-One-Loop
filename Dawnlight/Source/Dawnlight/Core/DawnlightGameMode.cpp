// Copyright Epic Games, Inc. All Rights Reserved.

#include "DawnlightGameMode.h"
#include "Dawnlight.h"
#include "Subsystems/NightProgressSubsystem.h"
#include "Subsystems/SoulCollectionSubsystem.h"
#include "Abilities/DawnlightAttributeSet.h"
#include "Characters/DawnlightCharacter.h"
#include "UI/Widgets/GameplayHUDWidget.h"
#include "UI/Widgets/GameResultWidget.h"
#include "UI/LevelTransitionSubsystem.h"
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

		if (NightProgressSubsystem.IsValid())
		{
			UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] NightProgressSubsystem を取得"));
		}

		if (SoulCollectionSubsystem.IsValid())
		{
			UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperGameMode] SoulCollectionSubsystem を取得"));
		}
	}
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

	// BP側のイベント呼び出し（敵スポーンはBPで実装）
	BP_OnWaveStarted(CurrentWave, EnemyCount);
}

void ADawnlightGameMode::OnEnemyKilled()
{
	if (CurrentPhase != EGamePhase::Dawn || RemainingEnemies <= 0)
	{
		return;
	}

	RemainingEnemies--;

	UE_LOG(LogDawnlight, Verbose, TEXT("[SoulReaperGameMode] 敵撃破 - 残り: %d"), RemainingEnemies);

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

	// 次のWaveへ
	if (CurrentWave < TotalWaves)
	{
		GetWorld()->GetTimerManager().SetTimer(
			WaveIntervalTimerHandle,
			this,
			&ADawnlightGameMode::StartNextWave,
			WaveInterval,
			false
		);
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
		return;
	}

	if (!SoulCollectionSubsystem.IsValid())
	{
		return;
	}

	// TODO: スポーンポイントを取得してランダムな位置にスポーン
	// 現在はBPで実装することを想定
	// SoulCollectionSubsystem->SpawnRandomAnimal(SpawnLocation);

	UE_LOG(LogDawnlight, Verbose, TEXT("[SoulReaperGameMode] 動物スポーン処理（BPで実装予定）"));
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
