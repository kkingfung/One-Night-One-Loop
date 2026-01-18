// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Components/UIAnimationComponent.h"
#include "DawnlightWidgetBase.generated.h"

class UUIStyleDataAsset;
class UButton;
class UWidget;

/**
 * Dawnlight UIウィジェットの基底クラス
 *
 * すべてのUIウィジェットが継承するべき共通機能を提供
 * - アニメーション対応の表示/非表示
 * - フォーカス管理
 * - サウンド再生
 */
UCLASS(Abstract)
class DAWNLIGHT_API UDawnlightWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UDawnlightWidgetBase(const FObjectInitializer& ObjectInitializer);

	// ========================================================================
	// 表示制御
	// ========================================================================

	/** ウィジェットを表示（アニメーション付き） */
	UFUNCTION(BlueprintCallable, Category = "UI|表示")
	virtual void Show();

	/** ウィジェットを非表示（アニメーション付き） */
	UFUNCTION(BlueprintCallable, Category = "UI|表示")
	virtual void Hide();

	/** アニメーションなしで即座に表示 */
	UFUNCTION(BlueprintCallable, Category = "UI|表示")
	void ShowImmediate();

	/** アニメーションなしで即座に非表示 */
	UFUNCTION(BlueprintCallable, Category = "UI|表示")
	void HideImmediate();

	/** 表示中かどうか */
	UFUNCTION(BlueprintPure, Category = "UI|表示")
	bool IsShowing() const { return bIsShowing; }

	// ========================================================================
	// Blueprint実装可能イベント
	// ========================================================================

	/** 表示アニメーション（Blueprintで実装） */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI|アニメーション", meta = (DisplayName = "Play Show Animation"))
	void BP_PlayShowAnimation();

	/** 非表示アニメーション（Blueprintで実装） */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI|アニメーション", meta = (DisplayName = "Play Hide Animation"))
	void BP_PlayHideAnimation();

	/** 表示完了時のコールバック（Blueprintで実装） */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI|イベント", meta = (DisplayName = "On Show Completed"))
	void BP_OnShowCompleted();

	/** 非表示完了時のコールバック（Blueprintで実装） */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI|イベント", meta = (DisplayName = "On Hide Completed"))
	void BP_OnHideCompleted();

	/** 初期フォーカス設定（Blueprintで実装） */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI|フォーカス", meta = (DisplayName = "Set Initial Focus"))
	void BP_SetInitialFocus();

protected:
	// ========================================================================
	// UUserWidget オーバーライド
	// ========================================================================

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// ========================================================================
	// 内部関数
	// ========================================================================

	/** 表示アニメーション完了時に呼び出し */
	UFUNCTION(BlueprintCallable, Category = "UI|アニメーション")
	void OnShowAnimationFinished();

	/** 非表示アニメーション完了時に呼び出し */
	UFUNCTION(BlueprintCallable, Category = "UI|アニメーション")
	void OnHideAnimationFinished();

	// ========================================================================
	// サウンド
	// ========================================================================

	/** ボタンホバーサウンド */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|サウンド")
	TObjectPtr<USoundBase> HoverSound;

	/** ボタンクリックサウンド */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|サウンド")
	TObjectPtr<USoundBase> ClickSound;

	/** 戻るサウンド */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|サウンド")
	TObjectPtr<USoundBase> BackSound;

	/** UIサウンドを再生 */
	UFUNCTION(BlueprintCallable, Category = "UI|サウンド")
	void PlayUISound(USoundBase* Sound);

	// ========================================================================
	// 状態
	// ========================================================================

	/** 表示中フラグ */
	UPROPERTY(BlueprintReadOnly, Category = "UI|状態")
	bool bIsShowing;

	/** アニメーション中フラグ */
	UPROPERTY(BlueprintReadOnly, Category = "UI|状態")
	bool bIsAnimating;

	/** アニメーションを使用するか */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|設定")
	bool bUseAnimation = true;

	/** デフォルトのアニメーション時間 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|設定", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float DefaultAnimationDuration = 0.3f;

	// ========================================================================
	// ジューシー機能
	// ========================================================================

	/** UIスタイルデータアセット */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|スタイル")
	TObjectPtr<UUIStyleDataAsset> StyleDataAsset;

	/** ボタンにホバーアニメーションを適用 */
	UFUNCTION(BlueprintCallable, Category = "UI|アニメーション")
	void ApplyButtonHoverAnimation(UButton* Button);

	/** 複数ウィジェットにスタガーアニメーションを適用 */
	UFUNCTION(BlueprintCallable, Category = "UI|アニメーション")
	void PlayMenuItemsStaggerAnimation(const TArray<UWidget*>& MenuItems, EUIAnimationType AnimationType = EUIAnimationType::SlideInLeft);

	/** ウィジェットにフェードインアニメーション */
	UFUNCTION(BlueprintCallable, Category = "UI|アニメーション")
	void PlayWidgetFadeIn(UWidget* Widget, float Duration = 0.3f, float Delay = 0.0f);

	/** ウィジェットにフェードアウトアニメーション */
	UFUNCTION(BlueprintCallable, Category = "UI|アニメーション")
	void PlayWidgetFadeOut(UWidget* Widget, float Duration = 0.2f, float Delay = 0.0f);

	/** ウィジェットにスライドインアニメーション */
	UFUNCTION(BlueprintCallable, Category = "UI|アニメーション")
	void PlayWidgetSlideIn(UWidget* Widget, EUIAnimationType Direction, float Distance = 100.0f, float Duration = 0.3f, float Delay = 0.0f);

	/** エラー時のシェイクアニメーション */
	UFUNCTION(BlueprintCallable, Category = "UI|アニメーション")
	void PlayErrorShake(UWidget* Widget);

	/** 注意を引くパルスアニメーション */
	UFUNCTION(BlueprintCallable, Category = "UI|アニメーション")
	void PlayAttentionPulse(UWidget* Widget, bool bLoop = true);

	/** アニメーションを停止 */
	UFUNCTION(BlueprintCallable, Category = "UI|アニメーション")
	void StopWidgetAnimation(UWidget* Widget);

private:
	/** ボタンホバー時のコールバック */
	UFUNCTION()
	void OnButtonHovered();

	/** ボタンアンホバー時のコールバック */
	UFUNCTION()
	void OnButtonUnhovered();

	/** ホバーアニメーション用のボタン参照を保持 */
	UPROPERTY()
	TArray<TObjectPtr<UButton>> AnimatedButtons;
};
