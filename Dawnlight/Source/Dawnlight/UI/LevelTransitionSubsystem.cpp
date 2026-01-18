// Copyright Epic Games, Inc. All Rights Reserved.

#include "LevelTransitionSubsystem.h"
#include "Dawnlight.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

void ULevelTransitionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bIsTransitioning = false;
	LoadingStartTime = 0.0;

	UE_LOG(LogDawnlight, Log, TEXT("[LevelTransitionSubsystem] 初期化完了"));
}

void ULevelTransitionSubsystem::Deinitialize()
{
	// ローディング画面をクリーンアップ
	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->RemoveFromParent();
		LoadingScreenWidget = nullptr;
	}

	UE_LOG(LogDawnlight, Log, TEXT("[LevelTransitionSubsystem] 終了処理完了"));

	Super::Deinitialize();
}

void ULevelTransitionSubsystem::TransitionToMainMenu()
{
	TransitionToLevel(MainMenuLevelName, true);
}

void ULevelTransitionSubsystem::TransitionToGameLevel(const FString& LevelName)
{
	FName LevelFName = LevelName.IsEmpty() ? DefaultGameLevelName : FName(*LevelName);
	TransitionToLevel(LevelFName, true);
}

void ULevelTransitionSubsystem::TransitionToLevel(const FName& LevelName, bool bShowLoadingScreen)
{
	if (bIsTransitioning)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[LevelTransitionSubsystem] 既に遷移中です"));
		return;
	}

	if (LevelName.IsNone())
	{
		UE_LOG(LogDawnlight, Error, TEXT("[LevelTransitionSubsystem] 無効なレベル名"));
		return;
	}

	bIsTransitioning = true;
	PendingLevelName = LevelName;

	UE_LOG(LogDawnlight, Log, TEXT("[LevelTransitionSubsystem] レベル遷移開始: %s"), *LevelName.ToString());

	// 遷移開始を通知
	OnLevelTransitionStarted.Broadcast(LevelName.ToString());

	if (bShowLoadingScreen)
	{
		ShowLoadingScreen();
		LoadingStartTime = FPlatformTime::Seconds();

		// 少し待ってから実際の遷移を開始（ローディング画面の表示を確保）
		if (UWorld* World = GetGameInstance()->GetWorld())
		{
			World->GetTimerManager().SetTimer(
				LoadingTimerHandle,
				FTimerDelegate::CreateLambda([this, LevelName]() { ExecuteLevelTransition(LevelName); }),
				0.1f,
				false
			);
		}
		else
		{
			ExecuteLevelTransition(LevelName);
		}
	}
	else
	{
		ExecuteLevelTransition(LevelName);
	}
}

void ULevelTransitionSubsystem::RestartCurrentLevel()
{
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World)
	{
		UE_LOG(LogDawnlight, Error, TEXT("[LevelTransitionSubsystem] ワールドが見つかりません"));
		return;
	}

	FName CurrentLevelName = FName(*World->GetMapName());

	// 「UEDPIE_」などのプレフィックスを除去
	FString CleanedName = World->GetMapName();
	CleanedName.RemoveFromStart(World->StreamingLevelsPrefix);

	TransitionToLevel(FName(*CleanedName), true);

	UE_LOG(LogDawnlight, Log, TEXT("[LevelTransitionSubsystem] 現在のレベルをリスタート: %s"), *CleanedName);
}

void ULevelTransitionSubsystem::ShowLoadingScreen()
{
	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	if (!LoadingScreenWidgetClass)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[LevelTransitionSubsystem] ローディング画面ウィジェットクラスが設定されていません"));
		return;
	}

	UWorld* World = GetGameInstance()->GetWorld();
	if (!World)
	{
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	LoadingScreenWidget = CreateWidget<UUserWidget>(PC, LoadingScreenWidgetClass);
	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->AddToViewport(9999); // 最前面に表示
		UE_LOG(LogDawnlight, Verbose, TEXT("[LevelTransitionSubsystem] ローディング画面を表示"));
	}
}

void ULevelTransitionSubsystem::HideLoadingScreen()
{
	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->RemoveFromParent();
		LoadingScreenWidget = nullptr;
		UE_LOG(LogDawnlight, Verbose, TEXT("[LevelTransitionSubsystem] ローディング画面を非表示"));
	}
}

void ULevelTransitionSubsystem::SetLoadingProgress(float Progress)
{
	float ClampedProgress = FMath::Clamp(Progress, 0.0f, 1.0f);
	OnLoadingProgressChanged.Broadcast(ClampedProgress);
}

void ULevelTransitionSubsystem::ExecuteLevelTransition(const FName& LevelName)
{
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World)
	{
		UE_LOG(LogDawnlight, Error, TEXT("[LevelTransitionSubsystem] ワールドが見つかりません"));
		bIsTransitioning = false;
		return;
	}

	// プログレスを50%に設定
	SetLoadingProgress(0.5f);

	// 通常のレベル遷移を使用
	// 非同期ロードが必要な場合は、FStreamableManager or LoadPackageAsyncを使用
	UGameplayStatics::OpenLevel(World, LevelName);

	// プログレスを100%に設定
	SetLoadingProgress(1.0f);

	// 最小表示時間を確保
	double ElapsedTime = FPlatformTime::Seconds() - LoadingStartTime;
	double RemainingTime = MinLoadingDisplayTime - ElapsedTime;

	if (RemainingTime > 0 && LoadingScreenWidget)
	{
		World->GetTimerManager().SetTimer(
			LoadingTimerHandle,
			this,
			&ULevelTransitionSubsystem::FinishLoadingAfterMinTime,
			RemainingTime,
			false
		);
	}
	else
	{
		FinishLoadingAfterMinTime();
	}
}

void ULevelTransitionSubsystem::OnAsyncLoadComplete()
{
	// 非同期ロード完了時の処理
	// 現在は同期ロードを使用しているため、この関数は呼ばれない
	SetLoadingProgress(1.0f);
	FinishLoadingAfterMinTime();
}

void ULevelTransitionSubsystem::FinishLoadingAfterMinTime()
{
	HideLoadingScreen();
	bIsTransitioning = false;

	// 遷移完了を通知
	OnLevelTransitionCompleted.Broadcast(PendingLevelName.ToString());

	UE_LOG(LogDawnlight, Log, TEXT("[LevelTransitionSubsystem] レベル遷移完了: %s"), *PendingLevelName.ToString());
}
