// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DawnlightWidgetBase.h"
#include "UpgradeSelectionWidget.generated.h"

class UUpgradeDataAsset;
class UUpgradeCardWidget;
class UUpgradeSubsystem;
class UHorizontalBox;
class UTextBlock;
class UButton;
class UImage;
class UOverlay;

/**
 * アップグレード選択完了時のデリゲート
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpgradeSelectionComplete, UUpgradeDataAsset*, SelectedUpgrade);

/**
 * アップグレード選択ウィジェット
 *
 * Wave終了後に表示されるアップグレード選択画面
 * 複数のUpgradeCardWidgetを配置して選択肢を表示
 *
 * 機能:
 * - 3枚のアップグレードカード表示
 * - リロールボタン
 * - Wave情報表示
 * - 選択後のアニメーション
 */
UCLASS()
class DAWNLIGHT_API UUpgradeSelectionWidget : public UDawnlightWidgetBase
{
	GENERATED_BODY()

public:
	// ========================================================================
	// 初期化・表示
	// ========================================================================

	/**
	 * アップグレード選択肢を設定して表示
	 * @param Choices 選択肢のアップグレードデータ配列
	 * @param WaveNumber 現在のウェーブ番号
	 */
	UFUNCTION(BlueprintCallable, Category = "アップグレード選択")
	void ShowWithChoices(const TArray<UUpgradeDataAsset*>& Choices, int32 WaveNumber);

	/** 表示をクリアして非表示 */
	UFUNCTION(BlueprintCallable, Category = "アップグレード選択")
	void ClearAndHide();

	// ========================================================================
	// アクション
	// ========================================================================

	/** リロール（選択肢を再生成） */
	UFUNCTION(BlueprintCallable, Category = "アップグレード選択")
	void RequestReroll();

	/** スキップ（アップグレードを選ばない） */
	UFUNCTION(BlueprintCallable, Category = "アップグレード選択")
	void RequestSkip();

	// ========================================================================
	// 状態
	// ========================================================================

	/** 選択待機中か */
	UFUNCTION(BlueprintPure, Category = "アップグレード選択")
	bool IsWaitingForSelection() const { return bIsWaitingForSelection; }

	/** リロール可能か */
	UFUNCTION(BlueprintPure, Category = "アップグレード選択")
	bool CanReroll() const;

	/** 残りリロール回数を取得 */
	UFUNCTION(BlueprintPure, Category = "アップグレード選択")
	int32 GetRemainingRerolls() const { return RemainingRerolls; }

	// ========================================================================
	// イベント
	// ========================================================================

	/** アップグレード選択完了時のデリゲート */
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnUpgradeSelectionComplete OnSelectionComplete;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ========================================================================
	// UI要素（Blueprintでバインド）
	// ========================================================================

	/** カードコンテナ */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> CardContainer;

	/** タイトルテキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TitleText;

	/** Wave情報テキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WaveInfoText;

	/** リロールボタン */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> RerollButton;

	/** リロール回数テキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RerollCountText;

	/** スキップボタン */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> SkipButton;

	/** 背景オーバーレイ */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UOverlay> BackgroundOverlay;

	/** 説明テキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> InstructionText;

	// ========================================================================
	// 設定
	// ========================================================================

	/** カードウィジェットクラス */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	TSubclassOf<UUpgradeCardWidget> CardWidgetClass;

	/** 最大リロール回数 */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	int32 MaxRerolls = 2;

	/** カード表示の遅延間隔（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "設定|アニメーション")
	float CardRevealDelay = 0.15f;

	/** 選択後に閉じるまでの遅延（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "設定|アニメーション")
	float CloseDelay = 1.0f;

private:
	// ========================================================================
	// 内部データ
	// ========================================================================

	/** UpgradeSubsystemへの参照 */
	UPROPERTY()
	TObjectPtr<UUpgradeSubsystem> UpgradeSubsystem;

	/** 生成されたカードウィジェット */
	UPROPERTY()
	TArray<TObjectPtr<UUpgradeCardWidget>> CardWidgets;

	/** 現在の選択肢 */
	UPROPERTY()
	TArray<TObjectPtr<UUpgradeDataAsset>> CurrentChoices;

	/** 現在のウェーブ番号 */
	int32 CurrentWaveNumber = 0;

	/** 残りリロール回数 */
	int32 RemainingRerolls = 0;

	/** 選択待機中フラグ */
	bool bIsWaitingForSelection = false;

	/** 選択済みフラグ */
	bool bHasSelected = false;

	// ========================================================================
	// 内部関数
	// ========================================================================

	/** カードウィジェットを生成 */
	void CreateCardWidgets(int32 Count);

	/** カードを更新 */
	void UpdateCards(const TArray<UUpgradeDataAsset*>& Choices);

	/** カードの登場アニメーションを再生 */
	void PlayCardsRevealAnimation();

	/** 選択後の演出を再生 */
	void PlaySelectionAnimation(UUpgradeCardWidget* SelectedCard);

	/** リロールボタンの状態を更新 */
	void UpdateRerollButton();

	/** UpgradeSubsystemを取得 */
	void CacheUpgradeSubsystem();

	// ========================================================================
	// イベントハンドラ
	// ========================================================================

	UFUNCTION()
	void OnCardSelected(UUpgradeDataAsset* SelectedUpgrade);

	UFUNCTION()
	void OnRerollButtonClicked();

	UFUNCTION()
	void OnSkipButtonClicked();

	/** 閉じるタイマー */
	FTimerHandle CloseTimerHandle;

	/** 閉じる処理を実行 */
	void ExecuteClose();
};
