// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

class UMainMenuWidget;
class USettingsWidget;
class UConfirmationDialogWidget;

/**
 * メインメニュー用ゲームモード
 *
 * タイトル画面の管理を担当
 * - メインメニューUIの表示
 * - 画面遷移の制御
 * - ゲーム開始処理
 */
UCLASS()
class DAWNLIGHT_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMainMenuGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ========================================================================
	// ウィジェットクラス参照
	// ========================================================================

	/** メインメニューウィジェットクラス */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|ウィジェット")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

	/** 設定ウィジェットクラス */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|ウィジェット")
	TSubclassOf<USettingsWidget> SettingsWidgetClass;

	/** 確認ダイアログウィジェットクラス */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|ウィジェット")
	TSubclassOf<UConfirmationDialogWidget> ConfirmationDialogWidgetClass;

	// ========================================================================
	// ウィジェットインスタンス
	// ========================================================================

	UPROPERTY(BlueprintReadOnly, Category = "UI|インスタンス")
	TObjectPtr<UMainMenuWidget> MainMenuWidget;

	UPROPERTY(BlueprintReadOnly, Category = "UI|インスタンス")
	TObjectPtr<USettingsWidget> SettingsWidget;

	UPROPERTY(BlueprintReadOnly, Category = "UI|インスタンス")
	TObjectPtr<UConfirmationDialogWidget> ConfirmationDialogWidget;

	// ========================================================================
	// メニューイベントハンドラ
	// ========================================================================

	/** ゲーム開始リクエスト */
	UFUNCTION()
	void OnStartGameRequested();

	/** 続きからリクエスト */
	UFUNCTION()
	void OnContinueGameRequested();

	/** 設定リクエスト */
	UFUNCTION()
	void OnSettingsRequested();

	/** クレジットリクエスト */
	UFUNCTION()
	void OnCreditsRequested();

	/** ゲーム終了リクエスト */
	UFUNCTION()
	void OnExitGameRequested();

	// ========================================================================
	// 設定イベントハンドラ
	// ========================================================================

	/** 設定適用 */
	UFUNCTION()
	void OnSettingsApplied();

	/** 設定キャンセル */
	UFUNCTION()
	void OnSettingsCancelled();

	// ========================================================================
	// ダイアログイベントハンドラ
	// ========================================================================

	/** 終了確認 */
	UFUNCTION()
	void OnExitConfirmed();

	/** 終了キャンセル */
	UFUNCTION()
	void OnExitCancelled();

	// ========================================================================
	// 画面制御
	// ========================================================================

	/** メインメニューを表示 */
	UFUNCTION(BlueprintCallable, Category = "UI|制御")
	void ShowMainMenu();

	/** 設定画面を表示 */
	UFUNCTION(BlueprintCallable, Category = "UI|制御")
	void ShowSettings();

	/** 終了確認ダイアログを表示 */
	UFUNCTION(BlueprintCallable, Category = "UI|制御")
	void ShowExitConfirmation();

private:
	/** ウィジェットを作成 */
	void CreateWidgets();

	/** イベントをバインド */
	void BindEvents();

	/** イベントをアンバインド */
	void UnbindEvents();

	/** 現在アクティブなダイアログコンテキスト */
	enum class EDialogContext
	{
		None,
		ExitGame
	};
	EDialogContext CurrentDialogContext;
};
