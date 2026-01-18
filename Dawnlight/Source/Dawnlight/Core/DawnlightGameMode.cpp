// Copyright Epic Games, Inc. All Rights Reserved.

#include "DawnlightGameMode.h"
#include "Dawnlight.h"
#include "Subsystems/NightProgressSubsystem.h"
#include "Subsystems/SurveillanceSubsystem.h"
#include "UI/Widgets/GameplayHUDWidget.h"
#include "UI/LevelTransitionSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

ADawnlightGameMode::ADawnlightGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	// デフォルト値
	bNightActive = false;
	bDawnSequenceActive = false;
	CurrentPhase = 0;
	NightDuration = 900.0f; // 15分
	DawnSequenceDuration = 5.0f; // 5秒の夜明け演出

	// 自動開始設定
	bAutoStartNight = true;
	AutoStartDelay = 2.0f; // 2秒後に開始
}

void ADawnlightGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogDawnlight, Log, TEXT("[DawnlightGameMode] BeginPlay"));

	// サブシステムを初期化
	InitializeSubsystems();

	// サブシステムイベントをバインド
	BindSubsystemEvents();

	// 自動開始が有効な場合
	if (bAutoStartNight)
	{
		GetWorld()->GetTimerManager().SetTimer(
			AutoStartTimerHandle,
			this,
			&ADawnlightGameMode::StartNight,
			AutoStartDelay,
			false
		);

		UE_LOG(LogDawnlight, Log, TEXT("[DawnlightGameMode] 夜の自動開始を%.1f秒後に予約"), AutoStartDelay);
	}
}

void ADawnlightGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 基本的なTickロジック（サブシステムが主要な処理を行う）
}

void ADawnlightGameMode::InitializeSubsystems()
{
	if (UWorld* World = GetWorld())
	{
		NightProgressSubsystem = World->GetSubsystem<UNightProgressSubsystem>();
		SurveillanceSubsystem = World->GetSubsystem<USurveillanceSubsystem>();

		if (NightProgressSubsystem.IsValid())
		{
			UE_LOG(LogDawnlight, Log, TEXT("[DawnlightGameMode] NightProgressSubsystem を取得しました"));
		}

		if (SurveillanceSubsystem.IsValid())
		{
			UE_LOG(LogDawnlight, Log, TEXT("[DawnlightGameMode] SurveillanceSubsystem を取得しました"));
		}
	}
}

void ADawnlightGameMode::BindSubsystemEvents()
{
	if (NightProgressSubsystem.IsValid())
	{
		NightProgressSubsystem->OnDawnTriggered.AddDynamic(this, &ADawnlightGameMode::HandleDawnTriggered);
		NightProgressSubsystem->OnPhaseChanged.AddDynamic(this, &ADawnlightGameMode::HandlePhaseChanged);

		UE_LOG(LogDawnlight, Log, TEXT("[DawnlightGameMode] サブシステムイベントをバインドしました"));
	}
}

void ADawnlightGameMode::UnbindSubsystemEvents()
{
	if (NightProgressSubsystem.IsValid())
	{
		NightProgressSubsystem->OnDawnTriggered.RemoveDynamic(this, &ADawnlightGameMode::HandleDawnTriggered);
		NightProgressSubsystem->OnPhaseChanged.RemoveDynamic(this, &ADawnlightGameMode::HandlePhaseChanged);
	}
}

void ADawnlightGameMode::StartNight()
{
	if (bNightActive)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[DawnlightGameMode] 夜は既に開始しています"));
		return;
	}

	bNightActive = true;
	CurrentPhase = 0;
	bDawnSequenceActive = false;

	// サブシステムに夜開始を通知
	if (NightProgressSubsystem.IsValid())
	{
		NightProgressSubsystem->StartNight(NightDuration);
	}

	// 監視システムをリセット
	if (SurveillanceSubsystem.IsValid())
	{
		SurveillanceSubsystem->ResetSurveillanceLevel();
	}

	// HUDを表示
	ShowGameplayHUD();

	UE_LOG(LogDawnlight, Log, TEXT("[DawnlightGameMode] 夜を開始しました（時間: %.0f秒）"), NightDuration);

	// BP側のイベント呼び出し
	OnNightStarted();
}

void ADawnlightGameMode::TriggerDawn()
{
	if (!bNightActive)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[DawnlightGameMode] 夜が開始されていません"));
		return;
	}

	// サブシステムを停止
	if (NightProgressSubsystem.IsValid())
	{
		NightProgressSubsystem->StopNight();
	}

	// 夜明け演出を開始
	StartDawnSequence();

	UE_LOG(LogDawnlight, Log, TEXT("[DawnlightGameMode] 夜明けをトリガーしました"));

	// BP側のイベント呼び出し
	OnDawnTriggered();
}

void ADawnlightGameMode::HandleDawnTriggered()
{
	// サブシステムからの夜明け通知
	UE_LOG(LogDawnlight, Log, TEXT("[DawnlightGameMode] サブシステムから夜明け通知を受信"));

	// 夜明け演出を開始
	StartDawnSequence();

	// BP側のイベント呼び出し
	OnDawnTriggered();
}

void ADawnlightGameMode::HandlePhaseChanged(int32 OldPhase, int32 NewPhase)
{
	CurrentPhase = NewPhase;

	UE_LOG(LogDawnlight, Log, TEXT("[DawnlightGameMode] フェーズ変更: %d → %d"), OldPhase, NewPhase);

	// BP側のイベント呼び出し
	OnPhaseChanged(OldPhase, NewPhase);
}

void ADawnlightGameMode::StartDawnSequence()
{
	if (bDawnSequenceActive)
	{
		return;
	}

	bNightActive = false;
	bDawnSequenceActive = true;

	// HUDを非表示
	HideGameplayHUD();

	// Night Complete画面を表示
	ShowNightCompleteScreen();

	// 一定時間後に演出完了
	GetWorld()->GetTimerManager().SetTimer(
		DawnSequenceTimerHandle,
		this,
		&ADawnlightGameMode::OnDawnSequenceComplete,
		DawnSequenceDuration,
		false
	);

	UE_LOG(LogDawnlight, Log, TEXT("[DawnlightGameMode] 夜明け演出を開始（%.1f秒）"), DawnSequenceDuration);
}

void ADawnlightGameMode::OnDawnSequenceComplete()
{
	bDawnSequenceActive = false;

	UE_LOG(LogDawnlight, Log, TEXT("[DawnlightGameMode] 夜明け演出が完了しました"));

	// BP側のイベント呼び出し
	OnNightCompleteSequenceFinished();
}

void ADawnlightGameMode::RestartNight()
{
	// タイマーをクリア
	GetWorld()->GetTimerManager().ClearTimer(AutoStartTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(DawnSequenceTimerHandle);

	// ウィジェットをクリーンアップ
	HideGameplayHUD();
	if (NightCompleteWidget)
	{
		NightCompleteWidget->RemoveFromParent();
		NightCompleteWidget = nullptr;
	}

	// 状態をリセット
	bNightActive = false;
	bDawnSequenceActive = false;
	CurrentPhase = 0;

	// レベルをリスタート
	if (UWorld* World = GetWorld())
	{
		if (ULevelTransitionSubsystem* LevelTransition = World->GetGameInstance()->GetSubsystem<ULevelTransitionSubsystem>())
		{
			LevelTransition->RestartCurrentLevel();
		}
		else
		{
			// フォールバック：直接レベルをリロード
			UGameplayStatics::OpenLevel(World, FName(*World->GetMapName()));
		}
	}

	UE_LOG(LogDawnlight, Log, TEXT("[DawnlightGameMode] 夜をリスタートしました"));
}

void ADawnlightGameMode::ReturnToMainMenu()
{
	// タイマーをクリア
	GetWorld()->GetTimerManager().ClearTimer(AutoStartTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(DawnSequenceTimerHandle);

	// サブシステムイベントをアンバインド
	UnbindSubsystemEvents();

	// メインメニューに遷移
	if (UWorld* World = GetWorld())
	{
		if (ULevelTransitionSubsystem* LevelTransition = World->GetGameInstance()->GetSubsystem<ULevelTransitionSubsystem>())
		{
			LevelTransition->TransitionToMainMenu();
		}
	}

	UE_LOG(LogDawnlight, Log, TEXT("[DawnlightGameMode] メインメニューに戻ります"));
}

void ADawnlightGameMode::ShowGameplayHUD()
{
	if (GameplayHUDWidget)
	{
		GameplayHUDWidget->Show();
		return;
	}

	if (!GameplayHUDWidgetClass)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[DawnlightGameMode] GameplayHUDWidgetClass が設定されていません"));
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

		UE_LOG(LogDawnlight, Log, TEXT("[DawnlightGameMode] ゲームプレイHUDを表示しました"));
	}
}

void ADawnlightGameMode::HideGameplayHUD()
{
	if (GameplayHUDWidget)
	{
		GameplayHUDWidget->Hide();
		UE_LOG(LogDawnlight, Verbose, TEXT("[DawnlightGameMode] ゲームプレイHUDを非表示にしました"));
	}
}

void ADawnlightGameMode::ShowNightCompleteScreen()
{
	if (NightCompleteWidget)
	{
		NightCompleteWidget->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	if (!NightCompleteWidgetClass)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[DawnlightGameMode] NightCompleteWidgetClass が設定されていません"));
		return;
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	NightCompleteWidget = CreateWidget<UUserWidget>(PC, NightCompleteWidgetClass);
	if (NightCompleteWidget)
	{
		NightCompleteWidget->AddToViewport(100); // HUDより前面
		UE_LOG(LogDawnlight, Log, TEXT("[DawnlightGameMode] Night Complete画面を表示しました"));
	}
}
