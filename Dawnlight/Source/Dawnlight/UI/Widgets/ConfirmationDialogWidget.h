// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DawnlightWidgetBase.h"
#include "ConfirmationDialogWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;

// デリゲート宣言
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogConfirmed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogCancelled);

/**
 * 確認ダイアログの種類
 */
UENUM(BlueprintType)
enum class EConfirmationDialogType : uint8
{
	YesNo			UMETA(DisplayName = "はい/いいえ"),
	OkCancel		UMETA(DisplayName = "OK/キャンセル"),
	Ok				UMETA(DisplayName = "OKのみ"),
	Custom			UMETA(DisplayName = "カスタム")
};

/**
 * 確認ダイアログウィジェット
 *
 * 汎用的な確認ダイアログUI
 * - ゲーム終了確認
 * - メインメニューに戻る確認
 * - 設定リセット確認
 * など
 */
UCLASS()
class DAWNLIGHT_API UConfirmationDialogWidget : public UDawnlightWidgetBase
{
	GENERATED_BODY()

public:
	UConfirmationDialogWidget(const FObjectInitializer& ObjectInitializer);

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** 確認された（はい/OK） */
	UPROPERTY(BlueprintAssignable, Category = "ダイアログ|イベント")
	FOnDialogConfirmed OnDialogConfirmed;

	/** キャンセルされた（いいえ/キャンセル） */
	UPROPERTY(BlueprintAssignable, Category = "ダイアログ|イベント")
	FOnDialogCancelled OnDialogCancelled;

	// ========================================================================
	// 公開関数
	// ========================================================================

	/** ダイアログを設定して表示 */
	UFUNCTION(BlueprintCallable, Category = "ダイアログ")
	void ShowDialog(
		const FText& Title,
		const FText& Message,
		EConfirmationDialogType DialogType = EConfirmationDialogType::YesNo
	);

	/** カスタムボタンテキストでダイアログを表示 */
	UFUNCTION(BlueprintCallable, Category = "ダイアログ")
	void ShowDialogCustom(
		const FText& Title,
		const FText& Message,
		const FText& ConfirmText,
		const FText& CancelText,
		bool bShowCancelButton = true
	);

	/** ダイアログを閉じる */
	UFUNCTION(BlueprintCallable, Category = "ダイアログ")
	void CloseDialog();

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

	/** タイトルテキスト */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ダイアログ|要素")
	TObjectPtr<UTextBlock> TitleText;

	/** メッセージテキスト */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ダイアログ|要素")
	TObjectPtr<UTextBlock> MessageText;

	/** 確認ボタン */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ダイアログ|要素")
	TObjectPtr<UButton> ConfirmButton;

	/** 確認ボタンテキスト */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ダイアログ|要素")
	TObjectPtr<UTextBlock> ConfirmButtonText;

	/** キャンセルボタン */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ダイアログ|要素")
	TObjectPtr<UButton> CancelButton;

	/** キャンセルボタンテキスト */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ダイアログ|要素")
	TObjectPtr<UTextBlock> CancelButtonText;

	/** 背景オーバーレイ */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ダイアログ|要素")
	TObjectPtr<UImage> BackgroundOverlay;

	// ========================================================================
	// ボタンハンドラ
	// ========================================================================

	UFUNCTION()
	void OnConfirmClicked();

	UFUNCTION()
	void OnCancelClicked();

	void OnDialogButtonHovered();

private:
	/** ボタンにイベントをバインド */
	void BindButtonEvents();

	/** ボタンのイベントをアンバインド */
	void UnbindButtonEvents();

	/** ダイアログタイプに応じたボタンテキストを設定 */
	void SetButtonTextForType(EConfirmationDialogType DialogType);
};
