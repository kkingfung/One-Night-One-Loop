// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PhotoCaptureWidget.generated.h"

class UImage;
class UBorder;
class UTextBlock;
class USoundBase;

// デリゲート宣言
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhotoTaken);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhotoCooldownComplete, bool, bSuccess);

/**
 * 撮影キャプチャウィジェット
 *
 * 撮影時のフィードバック演出
 * - フラッシュ効果
 * - ファインダーフレーム
 * - シャッターアニメーション
 * - 撮影完了演出
 */
UCLASS()
class DAWNLIGHT_API UPhotoCaptureWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPhotoCaptureWidget(const FObjectInitializer& ObjectInitializer);

	// ========================================================================
	// 撮影制御
	// ========================================================================

	/** ファインダーを表示（撮影モード開始） */
	UFUNCTION(BlueprintCallable, Category = "撮影")
	void ShowViewfinder();

	/** ファインダーを非表示（撮影モード終了） */
	UFUNCTION(BlueprintCallable, Category = "撮影")
	void HideViewfinder();

	/** 撮影を実行（フラッシュ + シャッター） */
	UFUNCTION(BlueprintCallable, Category = "撮影")
	void TakePhoto();

	/** 撮影成功演出 */
	UFUNCTION(BlueprintCallable, Category = "撮影")
	void ShowPhotoSuccess();

	/** 撮影失敗演出（ブレ、検知など） */
	UFUNCTION(BlueprintCallable, Category = "撮影")
	void ShowPhotoFailed(const FText& Reason);

	/** フォーカス状態を設定 */
	UFUNCTION(BlueprintCallable, Category = "撮影")
	void SetFocusState(bool bIsFocused);

	/** 危険表示を設定（シャッター音でばれる可能性） */
	UFUNCTION(BlueprintCallable, Category = "撮影")
	void SetDangerIndicator(bool bShow);

	// ========================================================================
	// 取得
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "撮影")
	bool IsViewfinderActive() const { return bIsViewfinderActive; }

	UFUNCTION(BlueprintPure, Category = "撮影")
	bool IsFocused() const { return bIsFocused; }

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** 撮影が行われた */
	UPROPERTY(BlueprintAssignable, Category = "撮影|イベント")
	FOnPhotoTaken OnPhotoTaken;

	/** 撮影クールダウン完了 */
	UPROPERTY(BlueprintAssignable, Category = "撮影|イベント")
	FOnPhotoCooldownComplete OnPhotoCooldownComplete;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ========================================================================
	// UI要素
	// ========================================================================

	/** フラッシュオーバーレイ */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> FlashOverlay;

	/** ファインダーフレーム */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> ViewfinderFrame;

	/** フォーカスインジケーター */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> FocusIndicator;

	/** クロスヘア（中央） */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> Crosshair;

	/** 危険インジケーター */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> DangerIndicator;

	/** 撮影結果テキスト */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ResultText;

	/** コーナーフレーム（4つ） */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> CornerTopLeft;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> CornerTopRight;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> CornerBottomLeft;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> CornerBottomRight;

	// ========================================================================
	// 色設定
	// ========================================================================

	/** フラッシュ色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|色")
	FLinearColor FlashColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.9f);

	/** ファインダーフレーム色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|色")
	FLinearColor ViewfinderColor = FLinearColor(0.8f, 0.9f, 1.0f, 0.6f);

	/** フォーカス中の色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|色")
	FLinearColor FocusedColor = FLinearColor(0.2f, 0.8f, 0.4f, 1.0f);

	/** 非フォーカスの色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|色")
	FLinearColor UnfocusedColor = FLinearColor(0.9f, 0.6f, 0.2f, 1.0f);

	/** 危険表示の色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|色")
	FLinearColor DangerColor = FLinearColor(0.9f, 0.2f, 0.1f, 1.0f);

	/** 成功テキスト色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|色")
	FLinearColor SuccessTextColor = FLinearColor(0.2f, 0.9f, 0.3f, 1.0f);

	/** 失敗テキスト色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|色")
	FLinearColor FailedTextColor = FLinearColor(0.9f, 0.3f, 0.2f, 1.0f);

	// ========================================================================
	// アニメーション設定
	// ========================================================================

	/** フラッシュ持続時間 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|アニメーション", meta = (ClampMin = "0.05", ClampMax = "0.5"))
	float FlashDuration = 0.15f;

	/** ファインダーフェードイン時間 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|アニメーション", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float ViewfinderFadeInDuration = 0.2f;

	/** シャッターアニメーション時間 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|アニメーション", meta = (ClampMin = "0.05", ClampMax = "0.3"))
	float ShutterAnimDuration = 0.1f;

	/** 結果テキスト表示時間 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|アニメーション", meta = (ClampMin = "0.5", ClampMax = "3.0"))
	float ResultTextDuration = 1.5f;

	/** フォーカスパルス速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|アニメーション")
	float FocusPulseSpeed = 3.0f;

	/** 危険インジケーターパルス速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|アニメーション")
	float DangerPulseSpeed = 6.0f;

	// ========================================================================
	// サウンド
	// ========================================================================

	/** シャッター音 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|サウンド")
	TObjectPtr<USoundBase> ShutterSound;

	/** フォーカス音 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|サウンド")
	TObjectPtr<USoundBase> FocusSound;

	/** 撮影成功音 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|サウンド")
	TObjectPtr<USoundBase> SuccessSound;

	/** 撮影失敗音 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "撮影|サウンド")
	TObjectPtr<USoundBase> FailedSound;

private:
	/** ファインダーアクティブ */
	bool bIsViewfinderActive;

	/** フォーカス中 */
	bool bIsFocused;

	/** 危険表示中 */
	bool bShowDanger;

	/** フラッシュタイマー */
	float FlashTimer;

	/** ファインダーフェードタイマー */
	float ViewfinderFadeTimer;

	/** シャッターアニメーションタイマー */
	float ShutterAnimTimer;

	/** 結果テキストタイマー */
	float ResultTextTimer;

	/** パルスタイマー */
	float PulseTimer;

	/** フラッシュを更新 */
	void UpdateFlash(float DeltaTime);

	/** ファインダーを更新 */
	void UpdateViewfinder(float DeltaTime);

	/** フォーカスインジケーターを更新 */
	void UpdateFocusIndicator(float DeltaTime);

	/** 危険インジケーターを更新 */
	void UpdateDangerIndicator(float DeltaTime);

	/** 結果テキストを更新 */
	void UpdateResultText(float DeltaTime);

	/** シャッターアニメーションを更新 */
	void UpdateShutterAnimation(float DeltaTime);

	/** コーナーをアニメーション */
	void AnimateCorners(float Alpha);
};
