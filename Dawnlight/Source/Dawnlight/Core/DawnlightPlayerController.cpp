// Copyright Epic Games, Inc. All Rights Reserved.

#include "DawnlightPlayerController.h"
#include "Dawnlight.h"
#include "Characters/DawnlightCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widgets/PauseMenuWidget.h"
#include "UI/Widgets/SettingsWidget.h"
#include "UI/Widgets/ConfirmationDialogWidget.h"
#include "UI/LevelTransitionSubsystem.h"

ADawnlightPlayerController::ADawnlightPlayerController()
{
	bIsGamePaused = false;
	bPauseWidgetsCreated = false;
	CurrentPauseDialogContext = EPauseDialogContext::None;
}

void ADawnlightPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// デフォルト入力コンテキストを追加
	if (DefaultMappingContext)
	{
		AddInputMappingContext(DefaultMappingContext, 0);
		UE_LOG(LogDawnlight, Log, TEXT("DawnlightPlayerController: デフォルト入力コンテキストを追加しました"));
	}
}

void ADawnlightPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Enhanced Input Componentにキャスト
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogDawnlight, Error, TEXT("DawnlightPlayerController: Enhanced Input Componentが見つかりません"));
		return;
	}

	// 入力アクションをバインド
	if (MoveAction)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADawnlightPlayerController::HandleMove);
	}

	if (LightAttackAction)
	{
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &ADawnlightPlayerController::HandleLightAttack);
	}

	if (HeavyAttackAction)
	{
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ADawnlightPlayerController::HandleHeavyAttack);
	}

	if (SpecialAttackAction)
	{
		EnhancedInputComponent->BindAction(SpecialAttackAction, ETriggerEvent::Started, this, &ADawnlightPlayerController::HandleSpecialAttack);
	}

	if (ReaperModeAction)
	{
		EnhancedInputComponent->BindAction(ReaperModeAction, ETriggerEvent::Started, this, &ADawnlightPlayerController::HandleReaperMode);
	}

	if (InteractAction)
	{
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ADawnlightPlayerController::HandleInteract);
	}

	if (PauseAction)
	{
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &ADawnlightPlayerController::HandlePause);
	}

	UE_LOG(LogDawnlight, Log, TEXT("DawnlightPlayerController: 入力アクションをバインドしました"));
}

void ADawnlightPlayerController::HandleMove(const FInputActionValue& Value)
{
	// 2Dベクトルとして移動入力を取得
	const FVector2D MovementVector = Value.Get<FVector2D>();

	// キャラクターに移動を委譲
	if (ADawnlightCharacter* DawnlightChar = Cast<ADawnlightCharacter>(GetPawn()))
	{
		DawnlightChar->HandleMoveInput(MovementVector);
	}
}

void ADawnlightPlayerController::HandleLightAttack(const FInputActionValue& Value)
{
	UE_LOG(LogDawnlight, Verbose, TEXT("DawnlightPlayerController: 通常攻撃入力を受信"));

	// キャラクターに通常攻撃を委譲
	if (ADawnlightCharacter* DawnlightChar = Cast<ADawnlightCharacter>(GetPawn()))
	{
		if (!DawnlightChar->IsAttacking())
		{
			DawnlightChar->PerformLightAttack();
		}
	}
}

void ADawnlightPlayerController::HandleHeavyAttack(const FInputActionValue& Value)
{
	UE_LOG(LogDawnlight, Verbose, TEXT("DawnlightPlayerController: 強攻撃入力を受信"));

	// キャラクターに強攻撃を委譲
	if (ADawnlightCharacter* DawnlightChar = Cast<ADawnlightCharacter>(GetPawn()))
	{
		if (!DawnlightChar->IsAttacking())
		{
			DawnlightChar->PerformHeavyAttack();
		}
	}
}

void ADawnlightPlayerController::HandleSpecialAttack(const FInputActionValue& Value)
{
	UE_LOG(LogDawnlight, Verbose, TEXT("DawnlightPlayerController: 特殊攻撃入力を受信"));

	// キャラクターに特殊攻撃を委譲
	if (ADawnlightCharacter* DawnlightChar = Cast<ADawnlightCharacter>(GetPawn()))
	{
		if (!DawnlightChar->IsAttacking())
		{
			DawnlightChar->PerformSpecialAttack();
		}
	}
}

void ADawnlightPlayerController::HandleReaperMode(const FInputActionValue& Value)
{
	UE_LOG(LogDawnlight, Log, TEXT("DawnlightPlayerController: リーパーモード入力を受信"));

	// キャラクターにリーパーモード発動を委譲
	if (ADawnlightCharacter* DawnlightChar = Cast<ADawnlightCharacter>(GetPawn()))
	{
		// リーパーモードが発動可能な場合のみ発動
		if (DawnlightChar->CanActivateReaperMode())
		{
			DawnlightChar->ActivateReaperMode();
		}
	}
}

void ADawnlightPlayerController::HandleInteract(const FInputActionValue& Value)
{
	UE_LOG(LogDawnlight, Verbose, TEXT("DawnlightPlayerController: インタラクト入力を受信"));

	// インタラクト処理（実装後）
}

void ADawnlightPlayerController::HandlePause(const FInputActionValue& Value)
{
	UE_LOG(LogDawnlight, Log, TEXT("DawnlightPlayerController: ポーズ入力を受信"));

	if (bIsGamePaused)
	{
		ResumeGame();
	}
	else
	{
		PauseGame();
	}
}

void ADawnlightPlayerController::EnableDefaultInput()
{
	if (DefaultMappingContext)
	{
		AddInputMappingContext(DefaultMappingContext, 0);
	}
}

void ADawnlightPlayerController::DisableGameplayInput()
{
	if (DefaultMappingContext)
	{
		RemoveInputMappingContext(DefaultMappingContext);
	}
}

void ADawnlightPlayerController::AddInputMappingContext(UInputMappingContext* Context, int32 Priority)
{
	if (!Context)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(Context, Priority);
	}
}

void ADawnlightPlayerController::RemoveInputMappingContext(UInputMappingContext* Context)
{
	if (!Context)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(Context);
	}
}

// ========================================================================
// ポーズ機能
// ========================================================================

void ADawnlightPlayerController::PauseGame()
{
	if (bIsGamePaused)
	{
		return;
	}

	bIsGamePaused = true;

	// ウィジェットを作成（初回のみ）
	if (!bPauseWidgetsCreated)
	{
		CreatePauseWidgets();
		BindPauseEvents();
		bPauseWidgetsCreated = true;
	}

	// ゲームをポーズ
	UGameplayStatics::SetGamePaused(this, true);

	// マウスカーソルを表示してUI入力モードに
	bShowMouseCursor = true;
	SetInputMode(FInputModeGameAndUI());

	// ポーズメニューを表示
	ShowPauseMenu();

	UE_LOG(LogDawnlight, Log, TEXT("[DawnlightPlayerController] ゲームをポーズ"));
}

void ADawnlightPlayerController::ResumeGame()
{
	if (!bIsGamePaused)
	{
		return;
	}

	bIsGamePaused = false;

	// ポーズUIを非表示
	if (PauseMenuWidget)
	{
		PauseMenuWidget->HideImmediate();
	}
	if (PauseSettingsWidget)
	{
		PauseSettingsWidget->HideImmediate();
	}
	if (PauseConfirmationDialog)
	{
		PauseConfirmationDialog->HideImmediate();
	}

	// ゲームを再開
	UGameplayStatics::SetGamePaused(this, false);

	// マウスカーソルを非表示にしてゲーム入力モードに
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	UE_LOG(LogDawnlight, Log, TEXT("[DawnlightPlayerController] ゲームを再開"));
}

void ADawnlightPlayerController::CreatePauseWidgets()
{
	// ポーズメニューウィジェット
	if (PauseMenuWidgetClass)
	{
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(this, PauseMenuWidgetClass);
		if (PauseMenuWidget)
		{
			PauseMenuWidget->AddToViewport(100);
			PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 設定ウィジェット
	if (SettingsWidgetClass)
	{
		PauseSettingsWidget = CreateWidget<USettingsWidget>(this, SettingsWidgetClass);
		if (PauseSettingsWidget)
		{
			PauseSettingsWidget->AddToViewport(101);
			PauseSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 確認ダイアログウィジェット
	if (ConfirmationDialogWidgetClass)
	{
		PauseConfirmationDialog = CreateWidget<UConfirmationDialogWidget>(this, ConfirmationDialogWidgetClass);
		if (PauseConfirmationDialog)
		{
			PauseConfirmationDialog->AddToViewport(110);
			PauseConfirmationDialog->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	UE_LOG(LogDawnlight, Log, TEXT("[DawnlightPlayerController] ポーズUIを作成"));
}

void ADawnlightPlayerController::BindPauseEvents()
{
	// ポーズメニューイベント
	if (PauseMenuWidget)
	{
		PauseMenuWidget->OnResumeRequested.AddDynamic(this, &ADawnlightPlayerController::OnResumeRequested);
		PauseMenuWidget->OnPauseSettingsRequested.AddDynamic(this, &ADawnlightPlayerController::OnPauseSettingsRequested);
		PauseMenuWidget->OnReturnToMainMenuRequested.AddDynamic(this, &ADawnlightPlayerController::OnReturnToMainMenuRequested);
		PauseMenuWidget->OnRestartRequested.AddDynamic(this, &ADawnlightPlayerController::OnRestartRequested);
	}

	// 設定イベント
	if (PauseSettingsWidget)
	{
		PauseSettingsWidget->OnSettingsApplied.AddDynamic(this, &ADawnlightPlayerController::OnPauseSettingsApplied);
		PauseSettingsWidget->OnSettingsCancelled.AddDynamic(this, &ADawnlightPlayerController::OnPauseSettingsCancelled);
	}

	// ダイアログイベント
	if (PauseConfirmationDialog)
	{
		PauseConfirmationDialog->OnDialogConfirmed.AddDynamic(this, &ADawnlightPlayerController::OnMainMenuConfirmed);
		PauseConfirmationDialog->OnDialogCancelled.AddDynamic(this, &ADawnlightPlayerController::OnMainMenuCancelled);
	}
}

void ADawnlightPlayerController::UnbindPauseEvents()
{
	if (PauseMenuWidget)
	{
		PauseMenuWidget->OnResumeRequested.RemoveAll(this);
		PauseMenuWidget->OnPauseSettingsRequested.RemoveAll(this);
		PauseMenuWidget->OnReturnToMainMenuRequested.RemoveAll(this);
		PauseMenuWidget->OnRestartRequested.RemoveAll(this);
	}

	if (PauseSettingsWidget)
	{
		PauseSettingsWidget->OnSettingsApplied.RemoveAll(this);
		PauseSettingsWidget->OnSettingsCancelled.RemoveAll(this);
	}

	if (PauseConfirmationDialog)
	{
		PauseConfirmationDialog->OnDialogConfirmed.RemoveAll(this);
		PauseConfirmationDialog->OnDialogCancelled.RemoveAll(this);
	}
}

void ADawnlightPlayerController::ShowPauseMenu()
{
	if (PauseSettingsWidget)
	{
		PauseSettingsWidget->HideImmediate();
	}

	if (PauseMenuWidget)
	{
		PauseMenuWidget->Show();
	}
}

void ADawnlightPlayerController::ShowPauseSettings()
{
	if (PauseMenuWidget)
	{
		PauseMenuWidget->HideImmediate();
	}

	if (PauseSettingsWidget)
	{
		PauseSettingsWidget->RefreshFromCurrentSettings();
		PauseSettingsWidget->Show();
	}
}

void ADawnlightPlayerController::ShowReturnToMainMenuConfirmation()
{
	CurrentPauseDialogContext = EPauseDialogContext::ReturnToMainMenu;

	if (PauseConfirmationDialog)
	{
		PauseConfirmationDialog->ShowDialog(
			FText::FromString(TEXT("メインメニューに戻る")),
			FText::FromString(TEXT("現在の進行状況は失われます。メインメニューに戻りますか？")),
			EConfirmationDialogType::YesNo
		);
	}
}

// ========================================================================
// ポーズメニューイベントハンドラ
// ========================================================================

void ADawnlightPlayerController::OnResumeRequested()
{
	ResumeGame();
}

void ADawnlightPlayerController::OnPauseSettingsRequested()
{
	ShowPauseSettings();
}

void ADawnlightPlayerController::OnReturnToMainMenuRequested()
{
	ShowReturnToMainMenuConfirmation();
}

void ADawnlightPlayerController::OnRestartRequested()
{
	UE_LOG(LogDawnlight, Log, TEXT("[DawnlightPlayerController] リスタート"));

	// ゲームのポーズを解除
	UGameplayStatics::SetGamePaused(this, false);
	bIsGamePaused = false;

	// 現在のレベルをリスタート
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (ULevelTransitionSubsystem* TransitionSubsystem = GameInstance->GetSubsystem<ULevelTransitionSubsystem>())
		{
			TransitionSubsystem->RestartCurrentLevel();
		}
	}
}

void ADawnlightPlayerController::OnPauseSettingsApplied()
{
	UE_LOG(LogDawnlight, Log, TEXT("[DawnlightPlayerController] 設定を適用"));
}

void ADawnlightPlayerController::OnPauseSettingsCancelled()
{
	ShowPauseMenu();
}

void ADawnlightPlayerController::OnMainMenuConfirmed()
{
	if (CurrentPauseDialogContext == EPauseDialogContext::ReturnToMainMenu)
	{
		UE_LOG(LogDawnlight, Log, TEXT("[DawnlightPlayerController] メインメニューに戻る"));

		// ゲームのポーズを解除
		UGameplayStatics::SetGamePaused(this, false);
		bIsGamePaused = false;

		// メインメニューに遷移
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			if (ULevelTransitionSubsystem* TransitionSubsystem = GameInstance->GetSubsystem<ULevelTransitionSubsystem>())
			{
				TransitionSubsystem->TransitionToMainMenu();
			}
		}
	}

	CurrentPauseDialogContext = EPauseDialogContext::None;
}

void ADawnlightPlayerController::OnMainMenuCancelled()
{
	CurrentPauseDialogContext = EPauseDialogContext::None;
}
