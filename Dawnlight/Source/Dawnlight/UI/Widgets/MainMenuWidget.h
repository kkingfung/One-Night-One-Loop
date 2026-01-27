// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DawnlightWidgetBase.h"
#include "MainMenuWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UWidgetAnimation;

// デリゲート宣言
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartGameRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContinueGameRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingsRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreditsRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitGameRequested);

/**
 * メインメニューウィジェット
 *
 * タイトル画面のUIを管理
 * - ゲーム開始
 * - 続きから
 * - 設定
 * - クレジット
 * - ゲーム終了
 */
UCLASS()
class DAWNLIGHT_API UMainMenuWidget : public UDawnlightWidgetBase
{
	GENERATED_BODY()

public:
	UMainMenuWidget(const FObjectInitializer& ObjectInitializer);

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** ゲーム開始がリクエストされた */
	UPROPERTY(BlueprintAssignable, Category = "メニュー|イベント")
	FOnStartGameRequested OnStartGameRequested;

	/** 続きからがリクエストされた */
	UPROPERTY(BlueprintAssignable, Category = "メニュー|イベント")
	FOnContinueGameRequested OnContinueGameRequested;

	/** 設定がリクエストされた */
	UPROPERTY(BlueprintAssignable, Category = "メニュー|イベント")
	FOnSettingsRequested OnSettingsRequested;

	/** クレジットがリクエストされた */
	UPROPERTY(BlueprintAssignable, Category = "メニュー|イベント")
	FOnCreditsRequested OnCreditsRequested;

	/** ゲーム終了がリクエストされた */
	UPROPERTY(BlueprintAssignable, Category = "メニュー|イベント")
	FOnExitGameRequested OnExitGameRequested;

	// ========================================================================
	// 公開関数
	// ========================================================================

	/** 続きからボタンの有効/無効を設定 */
	UFUNCTION(BlueprintCallable, Category = "メニュー")
	void SetContinueButtonEnabled(bool bEnabled);

	/** バージョン情報を設定 */
	UFUNCTION(BlueprintCallable, Category = "メニュー")
	void SetVersionText(const FString& Version);

	// ========================================================================
	// 表示制御オーバーライド
	// ========================================================================

	/** ウィジェットを表示（スタガーアニメーション付き） */
	virtual void Show() override;

protected:
	// ========================================================================
	// UUserWidget オーバーライド
	// ========================================================================

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ========================================================================
	// UI要素（Blueprintでバインド）
	// ========================================================================

	/** 新規ゲームボタン */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "メニュー|要素")
	TObjectPtr<UButton> NewGameButton;

	/** 続きからボタン */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "メニュー|要素")
	TObjectPtr<UButton> ContinueButton;

	/** 設定ボタン */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "メニュー|要素")
	TObjectPtr<UButton> SettingsButton;

	/** クレジットボタン */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "メニュー|要素")
	TObjectPtr<UButton> CreditsButton;

	/** 終了ボタン */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "メニュー|要素")
	TObjectPtr<UButton> ExitButton;

	/** タイトルテキスト */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "メニュー|要素")
	TObjectPtr<UTextBlock> TitleText;

	/** サブタイトルテキスト */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "メニュー|要素")
	TObjectPtr<UTextBlock> SubtitleText;

	/** バージョンテキスト */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "メニュー|要素")
	TObjectPtr<UTextBlock> VersionText;

	/** 背景画像 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "メニュー|要素")
	TObjectPtr<UImage> BackgroundImage;

	// ========================================================================
	// ボタンハンドラ
	// ========================================================================

	UFUNCTION()
	void OnNewGameClicked();

	UFUNCTION()
	void OnContinueClicked();

	UFUNCTION()
	void OnSettingsClicked();

	UFUNCTION()
	void OnCreditsClicked();

	UFUNCTION()
	void OnExitClicked();

	// ========================================================================
	// ホバーハンドラ
	// ========================================================================

	void OnMenuButtonHovered();

private:
	/** ボタンにイベントをバインド */
	void BindButtonEvents();

	/** ボタンのイベントをアンバインド */
	void UnbindButtonEvents();

	/** ボタンにジューシーアニメーションを適用 */
	void SetupJuicyAnimations();

	/** メニュー項目のリストを取得 */
	TArray<UWidget*> GetMenuItems() const;
};
