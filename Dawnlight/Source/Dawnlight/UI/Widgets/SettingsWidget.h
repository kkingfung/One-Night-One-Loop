// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DawnlightWidgetBase.h"
#include "UI/DawnlightUITypes.h"
#include "SettingsWidget.generated.h"

class UButton;
class UTextBlock;
class USlider;
class UCheckBox;
class UComboBoxString;
class UWidgetSwitcher;

// デリゲート宣言
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingsApplied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingsCancelled);

/**
 * 設定画面ウィジェット
 *
 * ゲーム設定UIを管理
 * - オーディオ設定
 * - グラフィック設定
 * - 操作設定
 * - ゲームプレイ設定
 */
UCLASS()
class DAWNLIGHT_API USettingsWidget : public UDawnlightWidgetBase
{
	GENERATED_BODY()

public:
	USettingsWidget(const FObjectInitializer& ObjectInitializer);

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** 設定が適用された */
	UPROPERTY(BlueprintAssignable, Category = "設定|イベント")
	FOnSettingsApplied OnSettingsApplied;

	/** 設定がキャンセルされた */
	UPROPERTY(BlueprintAssignable, Category = "設定|イベント")
	FOnSettingsCancelled OnSettingsCancelled;

	// ========================================================================
	// 公開関数
	// ========================================================================

	/** 現在の設定でUIを更新 */
	UFUNCTION(BlueprintCallable, Category = "設定")
	void RefreshFromCurrentSettings();

	/** 特定のカテゴリタブに切り替え */
	UFUNCTION(BlueprintCallable, Category = "設定")
	void SwitchToCategory(ESettingsCategory Category);

protected:
	// ========================================================================
	// UUserWidget オーバーライド
	// ========================================================================

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// ========================================================================
	// タブボタン（カテゴリ切り替え用）
	// ========================================================================

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|タブ")
	TObjectPtr<UButton> AudioTabButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|タブ")
	TObjectPtr<UButton> GraphicsTabButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|タブ")
	TObjectPtr<UButton> ControlsTabButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|タブ")
	TObjectPtr<UButton> GameplayTabButton;

	/** カテゴリコンテンツを切り替えるウィジェットスイッチャー */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|コンテナ")
	TObjectPtr<UWidgetSwitcher> CategorySwitcher;

	// ========================================================================
	// オーディオ設定要素
	// ========================================================================

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|オーディオ")
	TObjectPtr<USlider> MasterVolumeSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|オーディオ")
	TObjectPtr<UTextBlock> MasterVolumeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|オーディオ")
	TObjectPtr<USlider> MusicVolumeSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|オーディオ")
	TObjectPtr<UTextBlock> MusicVolumeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|オーディオ")
	TObjectPtr<USlider> SFXVolumeSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|オーディオ")
	TObjectPtr<UTextBlock> SFXVolumeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|オーディオ")
	TObjectPtr<USlider> AmbientVolumeSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|オーディオ")
	TObjectPtr<UTextBlock> AmbientVolumeText;

	// ========================================================================
	// グラフィック設定要素
	// ========================================================================

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|グラフィック")
	TObjectPtr<UComboBoxString> ResolutionComboBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|グラフィック")
	TObjectPtr<UComboBoxString> WindowModeComboBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|グラフィック")
	TObjectPtr<UCheckBox> VSyncCheckBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|グラフィック")
	TObjectPtr<UComboBoxString> QualityPresetComboBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|グラフィック")
	TObjectPtr<USlider> BrightnessSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|グラフィック")
	TObjectPtr<UTextBlock> BrightnessText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|グラフィック")
	TObjectPtr<USlider> GammaSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|グラフィック")
	TObjectPtr<UTextBlock> GammaText;

	// ========================================================================
	// 操作設定要素
	// ========================================================================

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|操作")
	TObjectPtr<USlider> MouseSensitivitySlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|操作")
	TObjectPtr<UTextBlock> MouseSensitivityText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|操作")
	TObjectPtr<UCheckBox> InvertYCheckBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|操作")
	TObjectPtr<UCheckBox> VibrationCheckBox;

	// ========================================================================
	// ゲームプレイ設定要素
	// ========================================================================

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|ゲームプレイ")
	TObjectPtr<UCheckBox> SubtitlesCheckBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|ゲームプレイ")
	TObjectPtr<USlider> SubtitleScaleSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|ゲームプレイ")
	TObjectPtr<UTextBlock> SubtitleScaleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|ゲームプレイ")
	TObjectPtr<USlider> CameraShakeSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|ゲームプレイ")
	TObjectPtr<UTextBlock> CameraShakeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|ゲームプレイ")
	TObjectPtr<UCheckBox> HintsCheckBox;

	// ========================================================================
	// 共通ボタン
	// ========================================================================

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|ボタン")
	TObjectPtr<UButton> ApplyButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|ボタン")
	TObjectPtr<UButton> BackButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "設定|ボタン")
	TObjectPtr<UButton> ResetButton;

	// ========================================================================
	// イベントハンドラ
	// ========================================================================

	// タブ切り替え
	UFUNCTION()
	void OnAudioTabClicked();

	UFUNCTION()
	void OnGraphicsTabClicked();

	UFUNCTION()
	void OnControlsTabClicked();

	UFUNCTION()
	void OnGameplayTabClicked();

	// オーディオ設定
	UFUNCTION()
	void OnMasterVolumeChanged(float Value);

	UFUNCTION()
	void OnMusicVolumeChanged(float Value);

	UFUNCTION()
	void OnSFXVolumeChanged(float Value);

	UFUNCTION()
	void OnAmbientVolumeChanged(float Value);

	// グラフィック設定
	UFUNCTION()
	void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnVSyncChanged(bool bIsChecked);

	UFUNCTION()
	void OnQualityPresetChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnBrightnessChanged(float Value);

	UFUNCTION()
	void OnGammaChanged(float Value);

	// 操作設定
	UFUNCTION()
	void OnMouseSensitivityChanged(float Value);

	UFUNCTION()
	void OnInvertYChanged(bool bIsChecked);

	UFUNCTION()
	void OnVibrationChanged(bool bIsChecked);

	// ゲームプレイ設定
	UFUNCTION()
	void OnSubtitlesChanged(bool bIsChecked);

	UFUNCTION()
	void OnSubtitleScaleChanged(float Value);

	UFUNCTION()
	void OnCameraShakeChanged(float Value);

	UFUNCTION()
	void OnHintsChanged(bool bIsChecked);

	// 共通ボタン
	UFUNCTION()
	void OnApplyClicked();

	UFUNCTION()
	void OnBackClicked();

	UFUNCTION()
	void OnResetClicked();

	void OnSettingsButtonHovered();

private:
	/** 作業用設定データ（適用前） */
	FDawnlightAllSettings WorkingSettings;

	/** 現在選択中のカテゴリ */
	ESettingsCategory CurrentCategory;

	/** 利用可能な解像度リスト */
	TArray<FIntPoint> AvailableResolutions;

	/** イベントバインド */
	void BindEvents();
	void UnbindEvents();

	/** コンボボックス初期化 */
	void PopulateResolutionOptions();
	void PopulateWindowModeOptions();
	void PopulateQualityPresetOptions();

	/** UI更新ヘルパー */
	void UpdateAudioUI();
	void UpdateGraphicsUI();
	void UpdateControlsUI();
	void UpdateGameplayUI();
	void UpdateVolumeText(UTextBlock* TextBlock, float Value);

	/** 解像度を文字列に変換 */
	FString ResolutionToString(const FIntPoint& Resolution) const;
	FIntPoint StringToResolution(const FString& String) const;
};
