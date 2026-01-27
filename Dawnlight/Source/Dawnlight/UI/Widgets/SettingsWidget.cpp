// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#include "SettingsWidget.h"
#include "WidgetUtilities.h"
#include "UI/UISubsystem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Dawnlight.h"

USettingsWidget::USettingsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CurrentCategory(ESettingsCategory::Audio)
{
}

void USettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// BindWidgetで見つからなかった場合、手動で検索
	// タブボタン
	if (!AudioTabButton) AudioTabButton = FindWidgetByBaseName<UButton>(this, TEXT("AudioTabButton"));
	if (!GraphicsTabButton) GraphicsTabButton = FindWidgetByBaseName<UButton>(this, TEXT("GraphicsTabButton"));
	if (!ControlsTabButton) ControlsTabButton = FindWidgetByBaseName<UButton>(this, TEXT("ControlsTabButton"));
	if (!GameplayTabButton) GameplayTabButton = FindWidgetByBaseName<UButton>(this, TEXT("GameplayTabButton"));
	if (!CategorySwitcher) CategorySwitcher = FindWidgetByBaseName<UWidgetSwitcher>(this, TEXT("CategorySwitcher"));

	// オーディオ設定
	if (!MasterVolumeSlider) MasterVolumeSlider = FindWidgetByBaseName<USlider>(this, TEXT("MasterVolumeSlider"));
	if (!MasterVolumeText) MasterVolumeText = FindWidgetByBaseName<UTextBlock>(this, TEXT("MasterVolumeText"));
	if (!MusicVolumeSlider) MusicVolumeSlider = FindWidgetByBaseName<USlider>(this, TEXT("MusicVolumeSlider"));
	if (!MusicVolumeText) MusicVolumeText = FindWidgetByBaseName<UTextBlock>(this, TEXT("MusicVolumeText"));
	if (!SFXVolumeSlider) SFXVolumeSlider = FindWidgetByBaseName<USlider>(this, TEXT("SFXVolumeSlider"));
	if (!SFXVolumeText) SFXVolumeText = FindWidgetByBaseName<UTextBlock>(this, TEXT("SFXVolumeText"));
	if (!AmbientVolumeSlider) AmbientVolumeSlider = FindWidgetByBaseName<USlider>(this, TEXT("AmbientVolumeSlider"));
	if (!AmbientVolumeText) AmbientVolumeText = FindWidgetByBaseName<UTextBlock>(this, TEXT("AmbientVolumeText"));

	// グラフィック設定
	if (!ResolutionComboBox) ResolutionComboBox = FindWidgetByBaseName<UComboBoxString>(this, TEXT("ResolutionComboBox"));
	if (!WindowModeComboBox) WindowModeComboBox = FindWidgetByBaseName<UComboBoxString>(this, TEXT("WindowModeComboBox"));
	if (!VSyncCheckBox) VSyncCheckBox = FindWidgetByBaseName<UCheckBox>(this, TEXT("VSyncCheckBox"));
	if (!QualityPresetComboBox) QualityPresetComboBox = FindWidgetByBaseName<UComboBoxString>(this, TEXT("QualityPresetComboBox"));
	if (!BrightnessSlider) BrightnessSlider = FindWidgetByBaseName<USlider>(this, TEXT("BrightnessSlider"));
	if (!BrightnessText) BrightnessText = FindWidgetByBaseName<UTextBlock>(this, TEXT("BrightnessText"));
	if (!GammaSlider) GammaSlider = FindWidgetByBaseName<USlider>(this, TEXT("GammaSlider"));
	if (!GammaText) GammaText = FindWidgetByBaseName<UTextBlock>(this, TEXT("GammaText"));

	// 操作設定
	if (!MouseSensitivitySlider) MouseSensitivitySlider = FindWidgetByBaseName<USlider>(this, TEXT("MouseSensitivitySlider"));
	if (!MouseSensitivityText) MouseSensitivityText = FindWidgetByBaseName<UTextBlock>(this, TEXT("MouseSensitivityText"));
	if (!InvertYCheckBox) InvertYCheckBox = FindWidgetByBaseName<UCheckBox>(this, TEXT("InvertYCheckBox"));
	if (!VibrationCheckBox) VibrationCheckBox = FindWidgetByBaseName<UCheckBox>(this, TEXT("VibrationCheckBox"));

	// ゲームプレイ設定
	if (!SubtitlesCheckBox) SubtitlesCheckBox = FindWidgetByBaseName<UCheckBox>(this, TEXT("SubtitlesCheckBox"));
	if (!SubtitleScaleSlider) SubtitleScaleSlider = FindWidgetByBaseName<USlider>(this, TEXT("SubtitleScaleSlider"));
	if (!SubtitleScaleText) SubtitleScaleText = FindWidgetByBaseName<UTextBlock>(this, TEXT("SubtitleScaleText"));
	if (!CameraShakeSlider) CameraShakeSlider = FindWidgetByBaseName<USlider>(this, TEXT("CameraShakeSlider"));
	if (!CameraShakeText) CameraShakeText = FindWidgetByBaseName<UTextBlock>(this, TEXT("CameraShakeText"));
	if (!HintsCheckBox) HintsCheckBox = FindWidgetByBaseName<UCheckBox>(this, TEXT("HintsCheckBox"));

	// 共通ボタン
	if (!ApplyButton) ApplyButton = FindWidgetByBaseName<UButton>(this, TEXT("ApplyButton"));
	if (!BackButton) BackButton = FindWidgetByBaseName<UButton>(this, TEXT("BackButton"));
	if (!ResetButton) ResetButton = FindWidgetByBaseName<UButton>(this, TEXT("ResetButton"));

	// UIサブシステムから現在の設定を取得
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UUISubsystem* UISubsystem = GameInstance->GetSubsystem<UUISubsystem>())
		{
			WorkingSettings = UISubsystem->GetCurrentSettings();
			AvailableResolutions = UISubsystem->GetAvailableResolutions();
		}
	}

	// コンボボックスの選択肢を設定
	PopulateResolutionOptions();
	PopulateWindowModeOptions();
	PopulateQualityPresetOptions();

	// イベントをバインド
	BindEvents();

	// UIを設定値で更新
	RefreshFromCurrentSettings();

	UE_LOG(LogDawnlight, Log, TEXT("[SettingsWidget] 初期化完了"));
}

void USettingsWidget::NativeDestruct()
{
	UnbindEvents();

	Super::NativeDestruct();
}

FReply USettingsWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		OnBackClicked();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void USettingsWidget::RefreshFromCurrentSettings()
{
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UUISubsystem* UISubsystem = GameInstance->GetSubsystem<UUISubsystem>())
		{
			WorkingSettings = UISubsystem->GetCurrentSettings();
		}
	}

	UpdateAudioUI();
	UpdateGraphicsUI();
	UpdateControlsUI();
	UpdateGameplayUI();
}

void USettingsWidget::SwitchToCategory(ESettingsCategory Category)
{
	CurrentCategory = Category;

	if (CategorySwitcher)
	{
		CategorySwitcher->SetActiveWidgetIndex(static_cast<int32>(Category));
	}

	UE_LOG(LogDawnlight, Verbose, TEXT("[SettingsWidget] カテゴリ切り替え: %d"), static_cast<int32>(Category));
}

void USettingsWidget::BindEvents()
{
	// タブボタン
	if (AudioTabButton)
	{
		AudioTabButton->OnClicked.AddDynamic(this, &USettingsWidget::OnAudioTabClicked);
		ApplyButtonHoverAnimation(AudioTabButton);
	}
	if (GraphicsTabButton)
	{
		GraphicsTabButton->OnClicked.AddDynamic(this, &USettingsWidget::OnGraphicsTabClicked);
		ApplyButtonHoverAnimation(GraphicsTabButton);
	}
	if (ControlsTabButton)
	{
		ControlsTabButton->OnClicked.AddDynamic(this, &USettingsWidget::OnControlsTabClicked);
		ApplyButtonHoverAnimation(ControlsTabButton);
	}
	if (GameplayTabButton)
	{
		GameplayTabButton->OnClicked.AddDynamic(this, &USettingsWidget::OnGameplayTabClicked);
		ApplyButtonHoverAnimation(GameplayTabButton);
	}

	// オーディオスライダー
	if (MasterVolumeSlider) MasterVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnMasterVolumeChanged);
	if (MusicVolumeSlider) MusicVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnMusicVolumeChanged);
	if (SFXVolumeSlider) SFXVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnSFXVolumeChanged);
	if (AmbientVolumeSlider) AmbientVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnAmbientVolumeChanged);

	// グラフィック
	if (ResolutionComboBox) ResolutionComboBox->OnSelectionChanged.AddDynamic(this, &USettingsWidget::OnResolutionChanged);
	if (WindowModeComboBox) WindowModeComboBox->OnSelectionChanged.AddDynamic(this, &USettingsWidget::OnWindowModeChanged);
	if (VSyncCheckBox) VSyncCheckBox->OnCheckStateChanged.AddDynamic(this, &USettingsWidget::OnVSyncChanged);
	if (QualityPresetComboBox) QualityPresetComboBox->OnSelectionChanged.AddDynamic(this, &USettingsWidget::OnQualityPresetChanged);
	if (BrightnessSlider) BrightnessSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnBrightnessChanged);
	if (GammaSlider) GammaSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnGammaChanged);

	// 操作
	if (MouseSensitivitySlider) MouseSensitivitySlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnMouseSensitivityChanged);
	if (InvertYCheckBox) InvertYCheckBox->OnCheckStateChanged.AddDynamic(this, &USettingsWidget::OnInvertYChanged);
	if (VibrationCheckBox) VibrationCheckBox->OnCheckStateChanged.AddDynamic(this, &USettingsWidget::OnVibrationChanged);

	// ゲームプレイ
	if (SubtitlesCheckBox) SubtitlesCheckBox->OnCheckStateChanged.AddDynamic(this, &USettingsWidget::OnSubtitlesChanged);
	if (SubtitleScaleSlider) SubtitleScaleSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnSubtitleScaleChanged);
	if (CameraShakeSlider) CameraShakeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnCameraShakeChanged);
	if (HintsCheckBox) HintsCheckBox->OnCheckStateChanged.AddDynamic(this, &USettingsWidget::OnHintsChanged);

	// 共通ボタン
	if (ApplyButton)
	{
		ApplyButton->OnClicked.AddDynamic(this, &USettingsWidget::OnApplyClicked);
		ApplyButtonHoverAnimation(ApplyButton);
	}
	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &USettingsWidget::OnBackClicked);
		ApplyButtonHoverAnimation(BackButton);
	}
	if (ResetButton)
	{
		ResetButton->OnClicked.AddDynamic(this, &USettingsWidget::OnResetClicked);
		ApplyButtonHoverAnimation(ResetButton);
	}
}

void USettingsWidget::UnbindEvents()
{
	// タブボタン
	if (AudioTabButton) AudioTabButton->OnClicked.RemoveAll(this);
	if (GraphicsTabButton) GraphicsTabButton->OnClicked.RemoveAll(this);
	if (ControlsTabButton) ControlsTabButton->OnClicked.RemoveAll(this);
	if (GameplayTabButton) GameplayTabButton->OnClicked.RemoveAll(this);

	// オーディオスライダー
	if (MasterVolumeSlider) MasterVolumeSlider->OnValueChanged.RemoveAll(this);
	if (MusicVolumeSlider) MusicVolumeSlider->OnValueChanged.RemoveAll(this);
	if (SFXVolumeSlider) SFXVolumeSlider->OnValueChanged.RemoveAll(this);
	if (AmbientVolumeSlider) AmbientVolumeSlider->OnValueChanged.RemoveAll(this);

	// グラフィック
	if (ResolutionComboBox) ResolutionComboBox->OnSelectionChanged.RemoveAll(this);
	if (WindowModeComboBox) WindowModeComboBox->OnSelectionChanged.RemoveAll(this);
	if (VSyncCheckBox) VSyncCheckBox->OnCheckStateChanged.RemoveAll(this);
	if (QualityPresetComboBox) QualityPresetComboBox->OnSelectionChanged.RemoveAll(this);
	if (BrightnessSlider) BrightnessSlider->OnValueChanged.RemoveAll(this);
	if (GammaSlider) GammaSlider->OnValueChanged.RemoveAll(this);

	// 操作
	if (MouseSensitivitySlider) MouseSensitivitySlider->OnValueChanged.RemoveAll(this);
	if (InvertYCheckBox) InvertYCheckBox->OnCheckStateChanged.RemoveAll(this);
	if (VibrationCheckBox) VibrationCheckBox->OnCheckStateChanged.RemoveAll(this);

	// ゲームプレイ
	if (SubtitlesCheckBox) SubtitlesCheckBox->OnCheckStateChanged.RemoveAll(this);
	if (SubtitleScaleSlider) SubtitleScaleSlider->OnValueChanged.RemoveAll(this);
	if (CameraShakeSlider) CameraShakeSlider->OnValueChanged.RemoveAll(this);
	if (HintsCheckBox) HintsCheckBox->OnCheckStateChanged.RemoveAll(this);

	// 共通ボタン
	if (ApplyButton) ApplyButton->OnClicked.RemoveAll(this);
	if (BackButton) BackButton->OnClicked.RemoveAll(this);
	if (ResetButton) ResetButton->OnClicked.RemoveAll(this);
}

void USettingsWidget::PopulateResolutionOptions()
{
	if (!ResolutionComboBox)
	{
		return;
	}

	ResolutionComboBox->ClearOptions();

	for (const FIntPoint& Resolution : AvailableResolutions)
	{
		ResolutionComboBox->AddOption(ResolutionToString(Resolution));
	}
}

void USettingsWidget::PopulateWindowModeOptions()
{
	if (!WindowModeComboBox)
	{
		return;
	}

	WindowModeComboBox->ClearOptions();
	WindowModeComboBox->AddOption(TEXT("フルスクリーン"));
	WindowModeComboBox->AddOption(TEXT("ボーダーレス"));
	WindowModeComboBox->AddOption(TEXT("ウィンドウ"));
}

void USettingsWidget::PopulateQualityPresetOptions()
{
	if (!QualityPresetComboBox)
	{
		return;
	}

	QualityPresetComboBox->ClearOptions();
	QualityPresetComboBox->AddOption(TEXT("低"));
	QualityPresetComboBox->AddOption(TEXT("中"));
	QualityPresetComboBox->AddOption(TEXT("高"));
	QualityPresetComboBox->AddOption(TEXT("ウルトラ"));
	QualityPresetComboBox->AddOption(TEXT("カスタム"));
}

void USettingsWidget::UpdateAudioUI()
{
	if (MasterVolumeSlider) MasterVolumeSlider->SetValue(WorkingSettings.Audio.MasterVolume);
	if (MusicVolumeSlider) MusicVolumeSlider->SetValue(WorkingSettings.Audio.MusicVolume);
	if (SFXVolumeSlider) SFXVolumeSlider->SetValue(WorkingSettings.Audio.SFXVolume);
	if (AmbientVolumeSlider) AmbientVolumeSlider->SetValue(WorkingSettings.Audio.AmbientVolume);

	UpdateVolumeText(MasterVolumeText, WorkingSettings.Audio.MasterVolume);
	UpdateVolumeText(MusicVolumeText, WorkingSettings.Audio.MusicVolume);
	UpdateVolumeText(SFXVolumeText, WorkingSettings.Audio.SFXVolume);
	UpdateVolumeText(AmbientVolumeText, WorkingSettings.Audio.AmbientVolume);
}

void USettingsWidget::UpdateGraphicsUI()
{
	if (ResolutionComboBox)
	{
		ResolutionComboBox->SetSelectedOption(ResolutionToString(WorkingSettings.Graphics.Resolution));
	}

	if (WindowModeComboBox)
	{
		int32 ModeIndex = static_cast<int32>(WorkingSettings.Graphics.WindowMode);
		if (ModeIndex >= 0 && ModeIndex < 3)
		{
			WindowModeComboBox->SetSelectedIndex(ModeIndex);
		}
	}

	if (VSyncCheckBox)
	{
		VSyncCheckBox->SetIsChecked(WorkingSettings.Graphics.bVSync);
	}

	if (QualityPresetComboBox)
	{
		int32 QualityIndex = static_cast<int32>(WorkingSettings.Graphics.OverallQuality);
		if (QualityIndex >= 0 && QualityIndex < 5)
		{
			QualityPresetComboBox->SetSelectedIndex(QualityIndex);
		}
	}

	if (BrightnessSlider)
	{
		// -1 to 1 -> 0 to 1
		BrightnessSlider->SetValue((WorkingSettings.Graphics.Brightness + 1.0f) / 2.0f);
	}
	if (BrightnessText)
	{
		BrightnessText->SetText(FText::AsNumber(FMath::RoundToInt(WorkingSettings.Graphics.Brightness * 100)));
	}

	if (GammaSlider)
	{
		// 1 to 3 -> 0 to 1
		GammaSlider->SetValue((WorkingSettings.Graphics.Gamma - 1.0f) / 2.0f);
	}
	if (GammaText)
	{
		GammaText->SetText(FText::AsNumber(WorkingSettings.Graphics.Gamma, &FNumberFormattingOptions().SetMaximumFractionalDigits(1)));
	}
}

void USettingsWidget::UpdateControlsUI()
{
	if (MouseSensitivitySlider)
	{
		// 0.1 to 3 -> 0 to 1
		MouseSensitivitySlider->SetValue((WorkingSettings.Controls.MouseSensitivity - 0.1f) / 2.9f);
	}
	if (MouseSensitivityText)
	{
		MouseSensitivityText->SetText(FText::AsNumber(WorkingSettings.Controls.MouseSensitivity, &FNumberFormattingOptions().SetMaximumFractionalDigits(1)));
	}

	if (InvertYCheckBox)
	{
		InvertYCheckBox->SetIsChecked(WorkingSettings.Controls.bInvertY);
	}

	if (VibrationCheckBox)
	{
		VibrationCheckBox->SetIsChecked(WorkingSettings.Controls.bEnableVibration);
	}
}

void USettingsWidget::UpdateGameplayUI()
{
	if (SubtitlesCheckBox)
	{
		SubtitlesCheckBox->SetIsChecked(WorkingSettings.Gameplay.bShowSubtitles);
	}

	if (SubtitleScaleSlider)
	{
		// 0.5 to 2 -> 0 to 1
		SubtitleScaleSlider->SetValue((WorkingSettings.Gameplay.SubtitleScale - 0.5f) / 1.5f);
	}
	if (SubtitleScaleText)
	{
		SubtitleScaleText->SetText(FText::AsNumber(WorkingSettings.Gameplay.SubtitleScale, &FNumberFormattingOptions().SetMaximumFractionalDigits(1)));
	}

	if (CameraShakeSlider)
	{
		CameraShakeSlider->SetValue(WorkingSettings.Gameplay.CameraShakeIntensity);
	}
	if (CameraShakeText)
	{
		CameraShakeText->SetText(FText::AsNumber(FMath::RoundToInt(WorkingSettings.Gameplay.CameraShakeIntensity * 100)));
	}

	if (HintsCheckBox)
	{
		HintsCheckBox->SetIsChecked(WorkingSettings.Gameplay.bShowHints);
	}
}

void USettingsWidget::UpdateVolumeText(UTextBlock* TextBlock, float Value)
{
	if (TextBlock)
	{
		TextBlock->SetText(FText::AsNumber(FMath::RoundToInt(Value * 100)));
	}
}

FString USettingsWidget::ResolutionToString(const FIntPoint& Resolution) const
{
	return FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y);
}

FIntPoint USettingsWidget::StringToResolution(const FString& String) const
{
	FIntPoint Result(1920, 1080);

	TArray<FString> Parts;
	String.ParseIntoArray(Parts, TEXT(" x "));

	if (Parts.Num() == 2)
	{
		Result.X = FCString::Atoi(*Parts[0]);
		Result.Y = FCString::Atoi(*Parts[1]);
	}

	return Result;
}

// タブ切り替えハンドラ
void USettingsWidget::OnAudioTabClicked()
{
	PlayUISound(ClickSound);
	SwitchToCategory(ESettingsCategory::Audio);
}

void USettingsWidget::OnGraphicsTabClicked()
{
	PlayUISound(ClickSound);
	SwitchToCategory(ESettingsCategory::Graphics);
}

void USettingsWidget::OnControlsTabClicked()
{
	PlayUISound(ClickSound);
	SwitchToCategory(ESettingsCategory::Controls);
}

void USettingsWidget::OnGameplayTabClicked()
{
	PlayUISound(ClickSound);
	SwitchToCategory(ESettingsCategory::Gameplay);
}

// オーディオ設定ハンドラ
void USettingsWidget::OnMasterVolumeChanged(float Value)
{
	WorkingSettings.Audio.MasterVolume = Value;
	UpdateVolumeText(MasterVolumeText, Value);
}

void USettingsWidget::OnMusicVolumeChanged(float Value)
{
	WorkingSettings.Audio.MusicVolume = Value;
	UpdateVolumeText(MusicVolumeText, Value);
}

void USettingsWidget::OnSFXVolumeChanged(float Value)
{
	WorkingSettings.Audio.SFXVolume = Value;
	UpdateVolumeText(SFXVolumeText, Value);
}

void USettingsWidget::OnAmbientVolumeChanged(float Value)
{
	WorkingSettings.Audio.AmbientVolume = Value;
	UpdateVolumeText(AmbientVolumeText, Value);
}

// グラフィック設定ハンドラ
void USettingsWidget::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType != ESelectInfo::Direct)
	{
		WorkingSettings.Graphics.Resolution = StringToResolution(SelectedItem);
	}
}

void USettingsWidget::OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType != ESelectInfo::Direct)
	{
		if (SelectedItem == TEXT("フルスクリーン"))
		{
			WorkingSettings.Graphics.WindowMode = EWindowModeType::Fullscreen;
		}
		else if (SelectedItem == TEXT("ボーダーレス"))
		{
			WorkingSettings.Graphics.WindowMode = EWindowModeType::WindowedFullscreen;
		}
		else
		{
			WorkingSettings.Graphics.WindowMode = EWindowModeType::Windowed;
		}
	}
}

void USettingsWidget::OnVSyncChanged(bool bIsChecked)
{
	WorkingSettings.Graphics.bVSync = bIsChecked;
}

void USettingsWidget::OnQualityPresetChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType != ESelectInfo::Direct)
	{
		if (SelectedItem == TEXT("低"))
		{
			WorkingSettings.Graphics.OverallQuality = EGraphicsQuality::Low;
		}
		else if (SelectedItem == TEXT("中"))
		{
			WorkingSettings.Graphics.OverallQuality = EGraphicsQuality::Medium;
		}
		else if (SelectedItem == TEXT("高"))
		{
			WorkingSettings.Graphics.OverallQuality = EGraphicsQuality::High;
		}
		else if (SelectedItem == TEXT("ウルトラ"))
		{
			WorkingSettings.Graphics.OverallQuality = EGraphicsQuality::Ultra;
		}
		else
		{
			WorkingSettings.Graphics.OverallQuality = EGraphicsQuality::Custom;
		}
	}
}

void USettingsWidget::OnBrightnessChanged(float Value)
{
	// 0 to 1 -> -1 to 1
	WorkingSettings.Graphics.Brightness = (Value * 2.0f) - 1.0f;
	if (BrightnessText)
	{
		BrightnessText->SetText(FText::AsNumber(FMath::RoundToInt(WorkingSettings.Graphics.Brightness * 100)));
	}
}

void USettingsWidget::OnGammaChanged(float Value)
{
	// 0 to 1 -> 1 to 3
	WorkingSettings.Graphics.Gamma = (Value * 2.0f) + 1.0f;
	if (GammaText)
	{
		GammaText->SetText(FText::AsNumber(WorkingSettings.Graphics.Gamma, &FNumberFormattingOptions().SetMaximumFractionalDigits(1)));
	}
}

// 操作設定ハンドラ
void USettingsWidget::OnMouseSensitivityChanged(float Value)
{
	// 0 to 1 -> 0.1 to 3
	WorkingSettings.Controls.MouseSensitivity = (Value * 2.9f) + 0.1f;
	if (MouseSensitivityText)
	{
		MouseSensitivityText->SetText(FText::AsNumber(WorkingSettings.Controls.MouseSensitivity, &FNumberFormattingOptions().SetMaximumFractionalDigits(1)));
	}
}

void USettingsWidget::OnInvertYChanged(bool bIsChecked)
{
	WorkingSettings.Controls.bInvertY = bIsChecked;
}

void USettingsWidget::OnVibrationChanged(bool bIsChecked)
{
	WorkingSettings.Controls.bEnableVibration = bIsChecked;
}

// ゲームプレイ設定ハンドラ
void USettingsWidget::OnSubtitlesChanged(bool bIsChecked)
{
	WorkingSettings.Gameplay.bShowSubtitles = bIsChecked;
}

void USettingsWidget::OnSubtitleScaleChanged(float Value)
{
	// 0 to 1 -> 0.5 to 2
	WorkingSettings.Gameplay.SubtitleScale = (Value * 1.5f) + 0.5f;
	if (SubtitleScaleText)
	{
		SubtitleScaleText->SetText(FText::AsNumber(WorkingSettings.Gameplay.SubtitleScale, &FNumberFormattingOptions().SetMaximumFractionalDigits(1)));
	}
}

void USettingsWidget::OnCameraShakeChanged(float Value)
{
	WorkingSettings.Gameplay.CameraShakeIntensity = Value;
	if (CameraShakeText)
	{
		CameraShakeText->SetText(FText::AsNumber(FMath::RoundToInt(Value * 100)));
	}
}

void USettingsWidget::OnHintsChanged(bool bIsChecked)
{
	WorkingSettings.Gameplay.bShowHints = bIsChecked;
}

// 共通ボタンハンドラ
void USettingsWidget::OnApplyClicked()
{
	PlayUISound(ClickSound);

	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UUISubsystem* UISubsystem = GameInstance->GetSubsystem<UUISubsystem>())
		{
			UISubsystem->ApplySettings(WorkingSettings);
		}
	}

	OnSettingsApplied.Broadcast();
	UE_LOG(LogDawnlight, Log, TEXT("[SettingsWidget] 設定を適用しました"));
}

void USettingsWidget::OnBackClicked()
{
	PlayUISound(BackSound);
	OnSettingsCancelled.Broadcast();
	UE_LOG(LogDawnlight, Log, TEXT("[SettingsWidget] 戻る"));
}

void USettingsWidget::OnResetClicked()
{
	PlayUISound(ClickSound);

	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UUISubsystem* UISubsystem = GameInstance->GetSubsystem<UUISubsystem>())
		{
			UISubsystem->ResetCategoryToDefault(CurrentCategory);
			WorkingSettings = UISubsystem->GetCurrentSettings();
		}
	}

	// 現在のカテゴリのUIを更新
	switch (CurrentCategory)
	{
	case ESettingsCategory::Audio:
		UpdateAudioUI();
		break;
	case ESettingsCategory::Graphics:
		UpdateGraphicsUI();
		break;
	case ESettingsCategory::Controls:
		UpdateControlsUI();
		break;
	case ESettingsCategory::Gameplay:
		UpdateGameplayUI();
		break;
	}

	UE_LOG(LogDawnlight, Log, TEXT("[SettingsWidget] カテゴリ %d をリセットしました"), static_cast<int32>(CurrentCategory));
}

void USettingsWidget::OnSettingsButtonHovered()
{
	// ジューシーアニメーションがホバーサウンドを処理
}
