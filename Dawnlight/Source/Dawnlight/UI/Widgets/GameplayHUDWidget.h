// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DawnlightWidgetBase.h"
#include "Core/DawnlightGameMode.h"
#include "GameplayHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class UHorizontalBox;
class UGameplayHUDViewModel;

/**
 * ゲームプレイHUDウィジェット
 *
 * Soul Reaperのインゲームで表示されるHUD要素を管理
 * ViewModelからデータを受け取り、表示を更新する（MVVMパターン）
 *
 * - リーパーゲージ
 * - 残り時間表示
 * - 魂カウント
 * - フェーズ表示
 * - 警告表示
 */
UCLASS()
class DAWNLIGHT_API UGameplayHUDWidget : public UDawnlightWidgetBase
{
	GENERATED_BODY()

public:
	// ========================================================================
	// ViewModel
	// ========================================================================

	/**
	 * ViewModelを設定
	 * Widgetを使用する前に必ず呼び出すこと
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetViewModel(UGameplayHUDViewModel* InViewModel);

	/**
	 * ViewModelを取得
	 */
	UFUNCTION(BlueprintPure, Category = "HUD")
	UGameplayHUDViewModel* GetViewModel() const { return ViewModel; }

	// ========================================================================
	// 手動更新関数（ViewModel経由でない直接更新用）
	// ========================================================================

	/** リーパーゲージを更新 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateReaperGauge(float NormalizedValue);

	/** 残り時間を更新（Night Phase中） */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateRemainingTime(float RemainingSeconds);

	/** 魂カウントを更新 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateSoulCount(int32 TotalSouls);

	/** 残り動物数を更新（Night Phase中） */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateAnimalCount(int32 AliveAnimals, int32 TotalAnimals);

	/** プレイヤーHPを更新 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdatePlayerHealth(float CurrentHP, float MaxHP);

	/** フェーズ表示を更新 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdatePhaseDisplay(EGamePhase Phase);

	/** Wave情報を更新（Dawn Phase中） */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateWaveInfo(int32 CurrentWave, int32 TotalWaves, int32 RemainingEnemies);

	// ========================================================================
	// 警告表示
	// ========================================================================

	/** リーパーゲージMAX警告を表示 */
	UFUNCTION(BlueprintCallable, Category = "HUD|警告")
	void ShowReaperReadyWarning();

	/** リーパーゲージMAX警告を非表示 */
	UFUNCTION(BlueprintCallable, Category = "HUD|警告")
	void HideReaperReadyWarning();

	/** 夜明け警告を表示 */
	UFUNCTION(BlueprintCallable, Category = "HUD|警告")
	void ShowDawnWarning();

	/** Wave開始警告を表示 */
	UFUNCTION(BlueprintCallable, Category = "HUD|警告")
	void ShowWaveStartWarning(int32 WaveNumber);

	// ========================================================================
	// 状態表示
	// ========================================================================

	/** リーパーモード表示 */
	UFUNCTION(BlueprintCallable, Category = "HUD|状態")
	void ShowReaperModeIndicator(bool bShow);

	/** ダメージバフ表示 */
	UFUNCTION(BlueprintCallable, Category = "HUD|状態")
	void ShowDamageBuffIndicator(float BuffPercent);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ========================================================================
	// UI要素（Blueprintでバインド）
	// ========================================================================

	/** リーパーゲージ */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ReaperGauge;

	/** 残り時間テキスト */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RemainingTimeText;

	/** 魂カウントテキスト */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SoulCountText;

	/** フェーズテキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PhaseText;

	/** Wave情報テキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WaveInfoText;

	/** 残り敵数テキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EnemyCountText;

	/** 残り動物数テキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> AnimalCountText;

	/** プレイヤーHPバー */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PlayerHealthBar;

	/** プレイヤーHPテキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PlayerHealthText;

	/** Night Phase用パネル */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> NightPhasePanel;

	/** Dawn Phase用パネル */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> DawnPhasePanel;

	/** リーパーレディ警告パネル */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> ReaperReadyWarningPanel;

	/** 夜明け警告パネル */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> DawnWarningPanel;

	/** Wave開始アナウンスメントパネル */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> WaveAnnouncementPanel;

	/** Wave開始アナウンスメントテキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WaveAnnouncementText;

	/** リーパーモードインジケーター */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> ReaperModeIndicator;

	/** バフ表示インジケーター */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> BuffIndicator;

	/** バフパーセントテキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> BuffPercentText;

	/** 魂アイコン */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> SoulIcon;

	// ========================================================================
	// 設定
	// ========================================================================

	/** リーパーゲージの通常色（魂の色：紫系） */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|スタイル")
	FLinearColor ReaperGaugeNormalColor;

	/** リーパーゲージの高い色（チャージ中：オレンジ） */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|スタイル")
	FLinearColor ReaperGaugeHighColor;

	/** リーパーゲージのMAX色（発動可能：金色） */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|スタイル")
	FLinearColor ReaperGaugeMaxColor;

	/** チャージ中閾値 */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|設定")
	float ChargeThreshold = 0.5f;

	/** MAX閾値 */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|設定")
	float MaxThreshold = 1.0f;

private:
	// ========================================================================
	// ViewModel
	// ========================================================================

	UPROPERTY()
	TObjectPtr<UGameplayHUDViewModel> ViewModel;

	// ========================================================================
	// ViewModelイベントハンドラ
	// ========================================================================

	/** ViewModelのプロパティ変更ハンドラ */
	UFUNCTION()
	void HandlePropertyChanged(FName PropertyName);

	/** ViewModelの全プロパティ変更ハンドラ */
	UFUNCTION()
	void HandleAllPropertiesChanged();

	/** Wave開始イベントハンドラ */
	UFUNCTION()
	void HandleWaveStarted(int32 WaveNumber);

	/** リーパーモード変更ハンドラ */
	UFUNCTION()
	void HandleReaperModeChanged(bool bIsActive);

	// ========================================================================
	// 内部関数
	// ========================================================================

	/** ViewModelにバインド */
	void BindToViewModel();

	/** ViewModelからアンバインド */
	void UnbindFromViewModel();

	/** ViewModelからUIを全更新 */
	void RefreshFromViewModel();

	/** Waveアナウンスメントを非表示タイマー */
	FTimerHandle WaveAnnouncementTimerHandle;

	/** アナウンスメント表示時間（秒） */
	float WaveAnnouncementDuration = 2.0f;

	/** Waveアナウンスメントを非表示にする */
	void HideWaveAnnouncement();

	/** リーパーゲージの色を更新 */
	void UpdateReaperGaugeColor(float NormalizedValue);

	/** 時間をフォーマット */
	FText FormatTime(float Seconds) const;

	/** フェーズ名を取得 */
	FText GetPhaseName(EGamePhase Phase) const;

	/** フェーズに応じたパネル表示を切り替え */
	void UpdatePhasePanels(EGamePhase Phase);
};
