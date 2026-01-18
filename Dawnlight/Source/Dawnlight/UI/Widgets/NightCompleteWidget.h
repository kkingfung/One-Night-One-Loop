// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DawnlightWidgetBase.h"
#include "NightCompleteWidget.generated.h"

class UTextBlock;
class UImage;

/**
 * Night Complete ウィジェット
 *
 * 夜明け時に表示される完了画面
 * - "NIGHT 1 COMPLETE" テキスト
 * - フェードイン/アウトアニメーション
 * - 撮影した証拠のサマリー（オプション）
 */
UCLASS()
class DAWNLIGHT_API UNightCompleteWidget : public UDawnlightWidgetBase
{
	GENERATED_BODY()

public:
	// ========================================================================
	// 表示制御
	// ========================================================================

	/** 夜番号を設定してウィジェットを表示 */
	UFUNCTION(BlueprintCallable, Category = "表示")
	void ShowWithNightNumber(int32 NightNumber);

	/** 証拠サマリーを設定 */
	UFUNCTION(BlueprintCallable, Category = "表示")
	void SetEvidenceSummary(int32 PhotosTaken, float TotalEvidenceValue);

protected:
	virtual void NativeConstruct() override;

	// ========================================================================
	// UI要素（Blueprintでバインド）
	// ========================================================================

	/** "NIGHT X" テキスト */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NightNumberText;

	/** "COMPLETE" テキスト */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CompleteText;

	/** 撮影枚数テキスト（オプション） */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PhotoCountText;

	/** 証拠価値テキスト（オプション） */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EvidenceValueText;

	/** 背景オーバーレイ */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> BackgroundOverlay;

private:
	/** 夜番号 */
	int32 CurrentNightNumber;

	/** テキストを更新 */
	void UpdateNightText();
};
