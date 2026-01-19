// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DawnlightWidgetBase.h"
#include "GameResultWidget.generated.h"

class UTextBlock;
class UButton;
class UImage;
class UVerticalBox;

/**
 * ゲーム結果の種類
 */
UENUM(BlueprintType)
enum class EGameResult : uint8
{
	None		UMETA(DisplayName = "なし"),
	Victory		UMETA(DisplayName = "勝利"),
	Defeat		UMETA(DisplayName = "敗北")
};

/**
 * ゲーム結果ウィジェット
 *
 * 勝利/敗北時に表示される結果画面
 * - 結果タイトル（VICTORY / DEFEAT）
 * - 収集した魂の統計
 * - クリアしたWave数
 * - リスタート/メインメニューボタン
 */
UCLASS()
class DAWNLIGHT_API UGameResultWidget : public UDawnlightWidgetBase
{
	GENERATED_BODY()

public:
	// ========================================================================
	// 初期化
	// ========================================================================

	/**
	 * 結果画面を設定して表示
	 *
	 * @param Result 勝利/敗北
	 * @param TotalSouls 収集した総魂数
	 * @param WavesCleared クリアしたWave数
	 * @param TotalWaves 総Wave数
	 */
	UFUNCTION(BlueprintCallable, Category = "結果画面")
	void ShowResult(EGameResult Result, int32 TotalSouls, int32 WavesCleared, int32 TotalWaves);

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** リスタートボタンが押された */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRestartRequested);
	UPROPERTY(BlueprintAssignable, Category = "結果画面|イベント")
	FOnRestartRequested OnRestartRequested;

	/** メインメニューボタンが押された */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMainMenuRequested);
	UPROPERTY(BlueprintAssignable, Category = "結果画面|イベント")
	FOnMainMenuRequested OnMainMenuRequested;

protected:
	virtual void NativeConstruct() override;

	// ========================================================================
	// UI要素（Blueprintでバインド）
	// ========================================================================

	/** 結果タイトル（VICTORY / DEFEAT） */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultTitleText;

	/** サブタイトル（説明テキスト） */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ResultSubtitleText;

	/** 背景画像/オーバーレイ */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> BackgroundImage;

	/** 統計情報コンテナ */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> StatsContainer;

	/** 収集魂数テキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SoulCountText;

	/** Wave進捗テキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WaveProgressText;

	/** リスタートボタン */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartButton;

	/** リスタートボタンテキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RestartButtonText;

	/** メインメニューボタン */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainMenuButton;

	/** メインメニューボタンテキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MainMenuButtonText;

	// ========================================================================
	// スタイル設定
	// ========================================================================

	/** 勝利時のタイトル色 */
	UPROPERTY(EditDefaultsOnly, Category = "結果画面|スタイル")
	FLinearColor VictoryColor;

	/** 敗北時のタイトル色 */
	UPROPERTY(EditDefaultsOnly, Category = "結果画面|スタイル")
	FLinearColor DefeatColor;

	/** 勝利時のタイトルテキスト */
	UPROPERTY(EditDefaultsOnly, Category = "結果画面|テキスト")
	FText VictoryTitleText;

	/** 勝利時のサブタイトルテキスト */
	UPROPERTY(EditDefaultsOnly, Category = "結果画面|テキスト")
	FText VictorySubtitleText;

	/** 敗北時のタイトルテキスト */
	UPROPERTY(EditDefaultsOnly, Category = "結果画面|テキスト")
	FText DefeatTitleText;

	/** 敗北時のサブタイトルテキスト */
	UPROPERTY(EditDefaultsOnly, Category = "結果画面|テキスト")
	FText DefeatSubtitleText;

private:
	// ========================================================================
	// ボタンイベント
	// ========================================================================

	/** リスタートボタンクリック */
	UFUNCTION()
	void OnRestartButtonClicked();

	/** メインメニューボタンクリック */
	UFUNCTION()
	void OnMainMenuButtonClicked();

	// ========================================================================
	// 内部処理
	// ========================================================================

	/** UIを勝利/敗北に応じて設定 */
	void SetupResultUI(EGameResult Result);

	/** 統計情報を設定 */
	void SetupStats(int32 TotalSouls, int32 WavesCleared, int32 TotalWaves);

	/** 現在の結果 */
	EGameResult CurrentResult;
};
