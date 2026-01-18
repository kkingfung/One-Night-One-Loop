// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DawnlightWidgetBase.h"
#include "PauseMenuWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;

// デリゲート宣言
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResumeRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPauseSettingsRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReturnToMainMenuRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRestartRequested);

/**
 * ポーズメニューウィジェット
 *
 * ゲーム中のポーズ画面UIを管理
 * - 再開
 * - 設定
 * - リスタート
 * - メインメニューに戻る
 */
UCLASS()
class DAWNLIGHT_API UPauseMenuWidget : public UDawnlightWidgetBase
{
	GENERATED_BODY()

public:
	UPauseMenuWidget(const FObjectInitializer& ObjectInitializer);

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** 再開がリクエストされた */
	UPROPERTY(BlueprintAssignable, Category = "ポーズ|イベント")
	FOnResumeRequested OnResumeRequested;

	/** 設定がリクエストされた */
	UPROPERTY(BlueprintAssignable, Category = "ポーズ|イベント")
	FOnPauseSettingsRequested OnPauseSettingsRequested;

	/** メインメニューへ戻るがリクエストされた */
	UPROPERTY(BlueprintAssignable, Category = "ポーズ|イベント")
	FOnReturnToMainMenuRequested OnReturnToMainMenuRequested;

	/** リスタートがリクエストされた */
	UPROPERTY(BlueprintAssignable, Category = "ポーズ|イベント")
	FOnRestartRequested OnRestartRequested;

protected:
	// ========================================================================
	// UUserWidget オーバーライド
	// ========================================================================

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// ========================================================================
	// UI要素（Blueprintでバインド）
	// ========================================================================

	/** 再開ボタン */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ポーズ|要素")
	TObjectPtr<UButton> ResumeButton;

	/** 設定ボタン */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ポーズ|要素")
	TObjectPtr<UButton> SettingsButton;

	/** リスタートボタン */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ポーズ|要素")
	TObjectPtr<UButton> RestartButton;

	/** メインメニューに戻るボタン */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ポーズ|要素")
	TObjectPtr<UButton> MainMenuButton;

	/** ポーズタイトルテキスト */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ポーズ|要素")
	TObjectPtr<UTextBlock> PauseTitleText;

	/** 背景オーバーレイ（暗転用） */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ポーズ|要素")
	TObjectPtr<UImage> BackgroundOverlay;

	// ========================================================================
	// ボタンハンドラ
	// ========================================================================

	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void OnSettingsClicked();

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnMainMenuClicked();

	void OnMenuButtonHovered();

private:
	/** ボタンにイベントをバインド */
	void BindButtonEvents();

	/** ボタンのイベントをアンバインド */
	void UnbindButtonEvents();
};
