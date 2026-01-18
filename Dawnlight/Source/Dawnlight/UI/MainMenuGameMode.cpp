// Copyright Epic Games, Inc. All Rights Reserved.

#include "MainMenuGameMode.h"
#include "Widgets/MainMenuWidget.h"
#include "Widgets/SettingsWidget.h"
#include "Widgets/ConfirmationDialogWidget.h"
#include "LevelTransitionSubsystem.h"
#include "Dawnlight.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/UserWidget.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	// デフォルトポーンを無効化（メニュー画面では不要）
	DefaultPawnClass = nullptr;

	CurrentDialogContext = EDialogContext::None;
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	// マウスカーソルを表示
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}

	// ウィジェットを作成して表示
	CreateWidgets();
	BindEvents();
	ShowMainMenu();

	UE_LOG(LogDawnlight, Log, TEXT("[MainMenuGameMode] メインメニュー開始"));
}

void AMainMenuGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindEvents();

	Super::EndPlay(EndPlayReason);
}

void AMainMenuGameMode::CreateWidgets()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		UE_LOG(LogDawnlight, Error, TEXT("[MainMenuGameMode] PlayerControllerが見つかりません"));
		return;
	}

	// メインメニューウィジェット
	if (MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<UMainMenuWidget>(PC, MainMenuWidgetClass);
		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport(0);
			MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 設定ウィジェット
	if (SettingsWidgetClass)
	{
		SettingsWidget = CreateWidget<USettingsWidget>(PC, SettingsWidgetClass);
		if (SettingsWidget)
		{
			SettingsWidget->AddToViewport(1);
			SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 確認ダイアログウィジェット
	if (ConfirmationDialogWidgetClass)
	{
		ConfirmationDialogWidget = CreateWidget<UConfirmationDialogWidget>(PC, ConfirmationDialogWidgetClass);
		if (ConfirmationDialogWidget)
		{
			ConfirmationDialogWidget->AddToViewport(10);
			ConfirmationDialogWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	UE_LOG(LogDawnlight, Log, TEXT("[MainMenuGameMode] ウィジェット作成完了"));
}

void AMainMenuGameMode::BindEvents()
{
	// メインメニューイベント
	if (MainMenuWidget)
	{
		MainMenuWidget->OnStartGameRequested.AddDynamic(this, &AMainMenuGameMode::OnStartGameRequested);
		MainMenuWidget->OnContinueGameRequested.AddDynamic(this, &AMainMenuGameMode::OnContinueGameRequested);
		MainMenuWidget->OnSettingsRequested.AddDynamic(this, &AMainMenuGameMode::OnSettingsRequested);
		MainMenuWidget->OnCreditsRequested.AddDynamic(this, &AMainMenuGameMode::OnCreditsRequested);
		MainMenuWidget->OnExitGameRequested.AddDynamic(this, &AMainMenuGameMode::OnExitGameRequested);
	}

	// 設定イベント
	if (SettingsWidget)
	{
		SettingsWidget->OnSettingsApplied.AddDynamic(this, &AMainMenuGameMode::OnSettingsApplied);
		SettingsWidget->OnSettingsCancelled.AddDynamic(this, &AMainMenuGameMode::OnSettingsCancelled);
	}

	// ダイアログイベント
	if (ConfirmationDialogWidget)
	{
		ConfirmationDialogWidget->OnDialogConfirmed.AddDynamic(this, &AMainMenuGameMode::OnExitConfirmed);
		ConfirmationDialogWidget->OnDialogCancelled.AddDynamic(this, &AMainMenuGameMode::OnExitCancelled);
	}
}

void AMainMenuGameMode::UnbindEvents()
{
	// メインメニューイベント
	if (MainMenuWidget)
	{
		MainMenuWidget->OnStartGameRequested.RemoveAll(this);
		MainMenuWidget->OnContinueGameRequested.RemoveAll(this);
		MainMenuWidget->OnSettingsRequested.RemoveAll(this);
		MainMenuWidget->OnCreditsRequested.RemoveAll(this);
		MainMenuWidget->OnExitGameRequested.RemoveAll(this);
	}

	// 設定イベント
	if (SettingsWidget)
	{
		SettingsWidget->OnSettingsApplied.RemoveAll(this);
		SettingsWidget->OnSettingsCancelled.RemoveAll(this);
	}

	// ダイアログイベント
	if (ConfirmationDialogWidget)
	{
		ConfirmationDialogWidget->OnDialogConfirmed.RemoveAll(this);
		ConfirmationDialogWidget->OnDialogCancelled.RemoveAll(this);
	}
}

void AMainMenuGameMode::ShowMainMenu()
{
	if (SettingsWidget)
	{
		SettingsWidget->HideImmediate();
	}

	if (MainMenuWidget)
	{
		MainMenuWidget->Show();
	}

	UE_LOG(LogDawnlight, Verbose, TEXT("[MainMenuGameMode] メインメニュー表示"));
}

void AMainMenuGameMode::ShowSettings()
{
	if (MainMenuWidget)
	{
		MainMenuWidget->HideImmediate();
	}

	if (SettingsWidget)
	{
		SettingsWidget->RefreshFromCurrentSettings();
		SettingsWidget->Show();
	}

	UE_LOG(LogDawnlight, Verbose, TEXT("[MainMenuGameMode] 設定画面表示"));
}

void AMainMenuGameMode::ShowExitConfirmation()
{
	CurrentDialogContext = EDialogContext::ExitGame;

	if (ConfirmationDialogWidget)
	{
		ConfirmationDialogWidget->ShowDialog(
			FText::FromString(TEXT("ゲーム終了")),
			FText::FromString(TEXT("ゲームを終了しますか？")),
			EConfirmationDialogType::YesNo
		);
	}

	UE_LOG(LogDawnlight, Verbose, TEXT("[MainMenuGameMode] 終了確認ダイアログ表示"));
}

void AMainMenuGameMode::OnStartGameRequested()
{
	UE_LOG(LogDawnlight, Log, TEXT("[MainMenuGameMode] ゲーム開始"));

	// レベル遷移サブシステムを使用してゲームレベルに遷移
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (ULevelTransitionSubsystem* TransitionSubsystem = GameInstance->GetSubsystem<ULevelTransitionSubsystem>())
		{
			TransitionSubsystem->TransitionToGameLevel(TEXT(""));
		}
	}
}

void AMainMenuGameMode::OnContinueGameRequested()
{
	UE_LOG(LogDawnlight, Log, TEXT("[MainMenuGameMode] 続きから（未実装）"));

	// TODO: セーブデータのロードとゲーム再開
}

void AMainMenuGameMode::OnSettingsRequested()
{
	ShowSettings();
}

void AMainMenuGameMode::OnCreditsRequested()
{
	UE_LOG(LogDawnlight, Log, TEXT("[MainMenuGameMode] クレジット（未実装）"));

	// TODO: クレジット画面の表示
}

void AMainMenuGameMode::OnExitGameRequested()
{
	ShowExitConfirmation();
}

void AMainMenuGameMode::OnSettingsApplied()
{
	UE_LOG(LogDawnlight, Log, TEXT("[MainMenuGameMode] 設定適用"));
	// 設定画面に留まる（ユーザーがBackを押すまで）
}

void AMainMenuGameMode::OnSettingsCancelled()
{
	ShowMainMenu();
}

void AMainMenuGameMode::OnExitConfirmed()
{
	if (CurrentDialogContext == EDialogContext::ExitGame)
	{
		UE_LOG(LogDawnlight, Log, TEXT("[MainMenuGameMode] ゲーム終了確認"));

		// ゲームを終了
		UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
	}

	CurrentDialogContext = EDialogContext::None;
}

void AMainMenuGameMode::OnExitCancelled()
{
	CurrentDialogContext = EDialogContext::None;
	UE_LOG(LogDawnlight, Verbose, TEXT("[MainMenuGameMode] 終了キャンセル"));
}
