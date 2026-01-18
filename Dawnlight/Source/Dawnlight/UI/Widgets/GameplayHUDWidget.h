// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DawnlightWidgetBase.h"
#include "GameplayHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class UNightProgressSubsystem;
class USurveillanceSubsystem;
class UPhotographyComponent;
class USurveillanceDetectorComponent;

/**
 * ゲームプレイHUDウィジェット
 *
 * インゲームで表示されるHUD要素を管理
 * - 検知ゲージ
 * - 残り時間表示
 * - 撮影カウント
 * - 警告表示
 */
UCLASS()
class DAWNLIGHT_API UGameplayHUDWidget : public UDawnlightWidgetBase
{
	GENERATED_BODY()

public:
	// ========================================================================
	// 更新関数
	// ========================================================================

	/** 検知ゲージを更新 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateDetectionGauge(float NormalizedValue);

	/** 残り時間を更新 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateRemainingTime(float RemainingSeconds);

	/** 撮影カウントを更新 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdatePhotoCount(int32 RemainingPhotos, int32 MaxPhotos);

	/** フェーズ表示を更新 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdatePhaseDisplay(int32 PhaseIndex);

	// ========================================================================
	// 警告表示
	// ========================================================================

	/** 検知警告を表示 */
	UFUNCTION(BlueprintCallable, Category = "HUD|警告")
	void ShowDetectionWarning();

	/** 検知警告を非表示 */
	UFUNCTION(BlueprintCallable, Category = "HUD|警告")
	void HideDetectionWarning();

	/** 夜明け警告を表示 */
	UFUNCTION(BlueprintCallable, Category = "HUD|警告")
	void ShowDawnWarning();

	/** フィルム切れ警告を表示 */
	UFUNCTION(BlueprintCallable, Category = "HUD|警告")
	void ShowNoFilmWarning();

	// ========================================================================
	// 状態表示
	// ========================================================================

	/** 撮影中表示 */
	UFUNCTION(BlueprintCallable, Category = "HUD|状態")
	void ShowPhotographingIndicator(bool bShow);

	/** 隠れ中表示 */
	UFUNCTION(BlueprintCallable, Category = "HUD|状態")
	void ShowHiddenIndicator(bool bShow);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ========================================================================
	// UI要素（Blueprintでバインド）
	// ========================================================================

	/** 検知ゲージ */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> DetectionGauge;

	/** 残り時間テキスト */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RemainingTimeText;

	/** 撮影カウントテキスト */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PhotoCountText;

	/** フェーズテキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PhaseText;

	/** 検知警告パネル */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> DetectionWarningPanel;

	/** 夜明け警告パネル */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> DawnWarningPanel;

	/** 撮影中インジケーター */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> PhotographingIndicator;

	/** 隠れ中インジケーター */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> HiddenIndicator;

	/** カメラアイコン */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> CameraIcon;

	// ========================================================================
	// 設定
	// ========================================================================

	/** 検知ゲージの通常色 */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|スタイル")
	FLinearColor DetectionGaugeNormalColor;

	/** 検知ゲージの警告色 */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|スタイル")
	FLinearColor DetectionGaugeWarningColor;

	/** 検知ゲージの危険色 */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|スタイル")
	FLinearColor DetectionGaugeDangerColor;

	/** 警告表示の閾値 */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|設定")
	float WarningThreshold = 0.5f;

	/** 危険表示の閾値 */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|設定")
	float DangerThreshold = 0.8f;

private:
	// ========================================================================
	// サブシステム参照
	// ========================================================================

	UPROPERTY()
	TWeakObjectPtr<UNightProgressSubsystem> NightProgressSubsystem;

	UPROPERTY()
	TWeakObjectPtr<USurveillanceSubsystem> SurveillanceSubsystem;

	UPROPERTY()
	TWeakObjectPtr<UPhotographyComponent> PlayerPhotographyComponent;

	UPROPERTY()
	TWeakObjectPtr<USurveillanceDetectorComponent> PlayerDetectorComponent;

	/** サブシステムを取得 */
	void CacheSubsystems();

	/** 検知ゲージの色を更新 */
	void UpdateDetectionGaugeColor(float NormalizedValue);

	/** 時間をフォーマット */
	FText FormatTime(float Seconds) const;

	/** フェーズ名を取得 */
	FText GetPhaseName(int32 PhaseIndex) const;
};
