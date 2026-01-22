// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "DawnlightPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UPauseMenuWidget;
class USettingsWidget;
class UConfirmationDialogWidget;

/**
 * Soul Reaper プレイヤーコントローラー
 *
 * Enhanced Inputを使用したプレイヤー入力管理
 * - 移動入力
 * - 攻撃/リーパーモード/インタラクトのトリガー
 * - 入力コンテキストの切り替え
 */
UCLASS()
class DAWNLIGHT_API ADawnlightPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADawnlightPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// ========================================================================
	// 入力アクション
	// ========================================================================

	/** 移動入力ハンドラ */
	void HandleMove(const FInputActionValue& Value);

	/** 通常攻撃入力ハンドラ */
	void HandleLightAttack(const FInputActionValue& Value);

	/** 強攻撃入力ハンドラ */
	void HandleHeavyAttack(const FInputActionValue& Value);

	/** 特殊攻撃入力ハンドラ */
	void HandleSpecialAttack(const FInputActionValue& Value);

	/** リーパーモード入力ハンドラ */
	void HandleReaperMode(const FInputActionValue& Value);

	/** インタラクト入力ハンドラ */
	void HandleInteract(const FInputActionValue& Value);

	/** ポーズ入力ハンドラ */
	void HandlePause(const FInputActionValue& Value);

	// ========================================================================
	// 入力コンテキスト管理
	// ========================================================================

	/** デフォルトの入力コンテキストを有効化 */
	UFUNCTION(BlueprintCallable, Category = "入力")
	void EnableDefaultInput();

	/** 入力を一時的に無効化（UI表示時など） */
	UFUNCTION(BlueprintCallable, Category = "入力")
	void DisableGameplayInput();

protected:
	// ========================================================================
	// 入力アセット参照
	// ========================================================================

	/** デフォルト入力マッピングコンテキスト */
	UPROPERTY(EditDefaultsOnly, Category = "入力|コンテキスト")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** リーパーモード中の入力マッピングコンテキスト */
	UPROPERTY(EditDefaultsOnly, Category = "入力|コンテキスト")
	TObjectPtr<UInputMappingContext> ReaperModeMappingContext;

	/** 移動アクション */
	UPROPERTY(EditDefaultsOnly, Category = "入力|アクション")
	TObjectPtr<UInputAction> MoveAction;

	/** 通常攻撃アクション（左クリック） */
	UPROPERTY(EditDefaultsOnly, Category = "入力|アクション")
	TObjectPtr<UInputAction> LightAttackAction;

	/** 強攻撃アクション（右クリック） */
	UPROPERTY(EditDefaultsOnly, Category = "入力|アクション")
	TObjectPtr<UInputAction> HeavyAttackAction;

	/** 特殊攻撃アクション（Q） */
	UPROPERTY(EditDefaultsOnly, Category = "入力|アクション")
	TObjectPtr<UInputAction> SpecialAttackAction;

	/** リーパーモードアクション */
	UPROPERTY(EditDefaultsOnly, Category = "入力|アクション")
	TObjectPtr<UInputAction> ReaperModeAction;

	/** インタラクトアクション */
	UPROPERTY(EditDefaultsOnly, Category = "入力|アクション")
	TObjectPtr<UInputAction> InteractAction;

	/** ポーズアクション */
	UPROPERTY(EditDefaultsOnly, Category = "入力|アクション")
	TObjectPtr<UInputAction> PauseAction;

	// ========================================================================
	// ポーズメニュー
	// ========================================================================

	/** ポーズメニューウィジェットクラス */
	UPROPERTY(EditDefaultsOnly, Category = "UI|ウィジェット")
	TSubclassOf<UPauseMenuWidget> PauseMenuWidgetClass;

	/** 設定ウィジェットクラス（ポーズ中） */
	UPROPERTY(EditDefaultsOnly, Category = "UI|ウィジェット")
	TSubclassOf<USettingsWidget> SettingsWidgetClass;

	/** 確認ダイアログウィジェットクラス */
	UPROPERTY(EditDefaultsOnly, Category = "UI|ウィジェット")
	TSubclassOf<UConfirmationDialogWidget> ConfirmationDialogWidgetClass;

public:
	// ========================================================================
	// ポーズ制御
	// ========================================================================

	/** ゲームをポーズ */
	UFUNCTION(BlueprintCallable, Category = "ポーズ")
	void PauseGame();

	/** ゲームを再開 */
	UFUNCTION(BlueprintCallable, Category = "ポーズ")
	void ResumeGame();

	/** ポーズ中かどうか */
	UFUNCTION(BlueprintPure, Category = "ポーズ")
	bool IsGamePaused() const { return bIsGamePaused; }

protected:
	// ========================================================================
	// ポーズメニューイベントハンドラ
	// ========================================================================

	UFUNCTION()
	void OnResumeRequested();

	UFUNCTION()
	void OnPauseSettingsRequested();

	UFUNCTION()
	void OnReturnToMainMenuRequested();

	UFUNCTION()
	void OnRestartRequested();

	UFUNCTION()
	void OnPauseSettingsApplied();

	UFUNCTION()
	void OnPauseSettingsCancelled();

	UFUNCTION()
	void OnMainMenuConfirmed();

	UFUNCTION()
	void OnMainMenuCancelled();

private:
	/** 入力コンテキストをサブシステムに追加 */
	void AddInputMappingContext(UInputMappingContext* Context, int32 Priority);

	/** 入力コンテキストをサブシステムから削除 */
	void RemoveInputMappingContext(UInputMappingContext* Context);

	/** ポーズUIを作成 */
	void CreatePauseWidgets();

	/** ポーズUIのイベントをバインド */
	void BindPauseEvents();

	/** ポーズUIのイベントをアンバインド */
	void UnbindPauseEvents();

	/** ポーズメニューを表示 */
	void ShowPauseMenu();

	/** ポーズ中の設定画面を表示 */
	void ShowPauseSettings();

	/** メインメニューに戻る確認ダイアログを表示 */
	void ShowReturnToMainMenuConfirmation();

	/** ポーズ中フラグ */
	bool bIsGamePaused;

	/** ポーズUI作成済みフラグ */
	bool bPauseWidgetsCreated;

	/** ポーズメニューウィジェットインスタンス */
	UPROPERTY()
	TObjectPtr<UPauseMenuWidget> PauseMenuWidget;

	/** 設定ウィジェットインスタンス（ポーズ中） */
	UPROPERTY()
	TObjectPtr<USettingsWidget> PauseSettingsWidget;

	/** 確認ダイアログインスタンス */
	UPROPERTY()
	TObjectPtr<UConfirmationDialogWidget> PauseConfirmationDialog;

	/** ダイアログコンテキスト */
	enum class EPauseDialogContext
	{
		None,
		ReturnToMainMenu
	};
	EPauseDialogContext CurrentPauseDialogContext;
};
