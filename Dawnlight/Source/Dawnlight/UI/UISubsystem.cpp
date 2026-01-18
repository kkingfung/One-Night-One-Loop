// Copyright Epic Games, Inc. All Rights Reserved.

#include "UISubsystem.h"
#include "DawnlightSaveGame.h"
#include "Dawnlight.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"
#include "AudioDevice.h"
#include "AudioMixerDevice.h"

void UUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentScreenType = EDawnlightScreenType::None;

	// 設定を読み込み（存在しない場合はデフォルト）
	if (!LoadSettings())
	{
		UE_LOG(LogDawnlight, Log, TEXT("[UISubsystem] 保存された設定が見つかりません。デフォルト設定を使用します。"));
		CurrentSettings = UDawnlightSaveGame::GetDefaultSettings();
	}

	// 読み込んだ設定を適用
	ApplyGraphicsToEngine(CurrentSettings.Graphics);
	ApplyAudioToEngine(CurrentSettings.Audio);

	UE_LOG(LogDawnlight, Log, TEXT("[UISubsystem] 初期化完了"));
}

void UUISubsystem::Deinitialize()
{
	// 終了時に設定を保存
	SaveSettings();

	UE_LOG(LogDawnlight, Log, TEXT("[UISubsystem] 終了処理完了"));

	Super::Deinitialize();
}

void UUISubsystem::SetCurrentScreenType(EDawnlightScreenType NewScreenType)
{
	if (CurrentScreenType != NewScreenType)
	{
		CurrentScreenType = NewScreenType;
		OnScreenChanged.Broadcast(NewScreenType);

		UE_LOG(LogDawnlight, Verbose, TEXT("[UISubsystem] 画面切り替え: %d"), static_cast<int32>(NewScreenType));
	}
}

void UUISubsystem::ApplySettings(const FDawnlightAllSettings& NewSettings)
{
	CurrentSettings = NewSettings;

	ApplyGraphicsToEngine(CurrentSettings.Graphics);
	ApplyAudioToEngine(CurrentSettings.Audio);

	OnSettingsChanged.Broadcast(CurrentSettings);
	SaveSettings();

	UE_LOG(LogDawnlight, Log, TEXT("[UISubsystem] 全設定を適用しました"));
}

void UUISubsystem::ApplyAudioSettings(const FDawnlightAudioSettings& AudioSettings)
{
	CurrentSettings.Audio = AudioSettings;
	ApplyAudioToEngine(AudioSettings);

	OnSettingsChanged.Broadcast(CurrentSettings);
	SaveSettings();

	UE_LOG(LogDawnlight, Log, TEXT("[UISubsystem] オーディオ設定を適用: Master=%.2f, Music=%.2f, SFX=%.2f"),
		AudioSettings.MasterVolume, AudioSettings.MusicVolume, AudioSettings.SFXVolume);
}

void UUISubsystem::ApplyGraphicsSettings(const FDawnlightGraphicsSettings& GraphicsSettings)
{
	CurrentSettings.Graphics = GraphicsSettings;
	ApplyGraphicsToEngine(GraphicsSettings);

	OnSettingsChanged.Broadcast(CurrentSettings);
	SaveSettings();

	UE_LOG(LogDawnlight, Log, TEXT("[UISubsystem] グラフィック設定を適用: %dx%d, VSync=%d"),
		GraphicsSettings.Resolution.X, GraphicsSettings.Resolution.Y, GraphicsSettings.bVSync);
}

void UUISubsystem::ApplyControlSettings(const FDawnlightControlSettings& ControlSettings)
{
	CurrentSettings.Controls = ControlSettings;

	OnSettingsChanged.Broadcast(CurrentSettings);
	SaveSettings();

	UE_LOG(LogDawnlight, Log, TEXT("[UISubsystem] 操作設定を適用: Sensitivity=%.2f, InvertY=%d"),
		ControlSettings.MouseSensitivity, ControlSettings.bInvertY);
}

void UUISubsystem::ApplyGameplaySettings(const FDawnlightGameplaySettings& GameplaySettings)
{
	CurrentSettings.Gameplay = GameplaySettings;

	OnSettingsChanged.Broadcast(CurrentSettings);
	SaveSettings();

	UE_LOG(LogDawnlight, Log, TEXT("[UISubsystem] ゲームプレイ設定を適用: Subtitles=%d, Hints=%d"),
		GameplaySettings.bShowSubtitles, GameplaySettings.bShowHints);
}

void UUISubsystem::ResetToDefaultSettings()
{
	CurrentSettings = UDawnlightSaveGame::GetDefaultSettings();

	ApplyGraphicsToEngine(CurrentSettings.Graphics);
	ApplyAudioToEngine(CurrentSettings.Audio);

	OnSettingsChanged.Broadcast(CurrentSettings);
	SaveSettings();

	UE_LOG(LogDawnlight, Log, TEXT("[UISubsystem] 全設定をデフォルトにリセットしました"));
}

void UUISubsystem::ResetCategoryToDefault(ESettingsCategory Category)
{
	FDawnlightAllSettings Defaults = UDawnlightSaveGame::GetDefaultSettings();

	switch (Category)
	{
	case ESettingsCategory::Audio:
		CurrentSettings.Audio = Defaults.Audio;
		ApplyAudioToEngine(CurrentSettings.Audio);
		break;

	case ESettingsCategory::Graphics:
		CurrentSettings.Graphics = Defaults.Graphics;
		ApplyGraphicsToEngine(CurrentSettings.Graphics);
		break;

	case ESettingsCategory::Controls:
		CurrentSettings.Controls = Defaults.Controls;
		break;

	case ESettingsCategory::Gameplay:
		CurrentSettings.Gameplay = Defaults.Gameplay;
		break;
	}

	OnSettingsChanged.Broadcast(CurrentSettings);
	SaveSettings();

	UE_LOG(LogDawnlight, Log, TEXT("[UISubsystem] カテゴリ %d の設定をリセットしました"), static_cast<int32>(Category));
}

bool UUISubsystem::SaveSettings()
{
	UDawnlightSaveGame* SaveGameInstance = Cast<UDawnlightSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UDawnlightSaveGame::StaticClass()));

	if (!SaveGameInstance)
	{
		UE_LOG(LogDawnlight, Error, TEXT("[UISubsystem] SaveGameオブジェクトの作成に失敗しました"));
		return false;
	}

	SaveGameInstance->Settings = CurrentSettings;
	SaveGameInstance->SettingsVersion = UDawnlightSaveGame::GetCurrentVersion();
	SaveGameInstance->LastSaveTime = FDateTime::Now();

	bool bSuccess = UGameplayStatics::SaveGameToSlot(
		SaveGameInstance,
		UDawnlightSaveGame::SaveSlotName,
		UDawnlightSaveGame::UserIndex);

	if (bSuccess)
	{
		UE_LOG(LogDawnlight, Log, TEXT("[UISubsystem] 設定を保存しました: %s"), *UDawnlightSaveGame::SaveSlotName);
	}
	else
	{
		UE_LOG(LogDawnlight, Error, TEXT("[UISubsystem] 設定の保存に失敗しました"));
	}

	return bSuccess;
}

bool UUISubsystem::LoadSettings()
{
	if (!DoesSaveExist())
	{
		return false;
	}

	UDawnlightSaveGame* LoadedGame = Cast<UDawnlightSaveGame>(
		UGameplayStatics::LoadGameFromSlot(
			UDawnlightSaveGame::SaveSlotName,
			UDawnlightSaveGame::UserIndex));

	if (!LoadedGame)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[UISubsystem] 設定の読み込みに失敗しました"));
		return false;
	}

	// バージョンチェック（将来のマイグレーション用）
	if (LoadedGame->SettingsVersion < UDawnlightSaveGame::GetCurrentVersion())
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[UISubsystem] 古いバージョンの設定を検出: v%d -> v%d"),
			LoadedGame->SettingsVersion, UDawnlightSaveGame::GetCurrentVersion());
		// TODO: マイグレーション処理
	}

	CurrentSettings = LoadedGame->Settings;

	UE_LOG(LogDawnlight, Log, TEXT("[UISubsystem] 設定を読み込みました（保存日時: %s）"),
		*LoadedGame->LastSaveTime.ToString());

	return true;
}

bool UUISubsystem::DoesSaveExist() const
{
	return UGameplayStatics::DoesSaveGameExist(
		UDawnlightSaveGame::SaveSlotName,
		UDawnlightSaveGame::UserIndex);
}

TArray<FIntPoint> UUISubsystem::GetAvailableResolutions() const
{
	TArray<FIntPoint> Resolutions;

	FScreenResolutionArray ResolutionArray;
	if (RHIGetAvailableResolutions(ResolutionArray, true))
	{
		for (const FScreenResolutionRHI& Resolution : ResolutionArray)
		{
			// 最小解像度のフィルタリング
			if (Resolution.Width >= 1280 && Resolution.Height >= 720)
			{
				Resolutions.AddUnique(FIntPoint(Resolution.Width, Resolution.Height));
			}
		}
	}

	// デフォルト解像度を追加（見つからない場合のフォールバック）
	if (Resolutions.Num() == 0)
	{
		Resolutions.Add(FIntPoint(1280, 720));
		Resolutions.Add(FIntPoint(1920, 1080));
		Resolutions.Add(FIntPoint(2560, 1440));
		Resolutions.Add(FIntPoint(3840, 2160));
	}

	// 解像度をソート（小さい順）
	Resolutions.Sort([](const FIntPoint& A, const FIntPoint& B)
	{
		return (A.X * A.Y) < (B.X * B.Y);
	});

	return Resolutions;
}

void UUISubsystem::ApplyQualityPreset(EGraphicsQuality Quality)
{
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (!UserSettings)
	{
		return;
	}

	int32 QualityLevel = 0;
	switch (Quality)
	{
	case EGraphicsQuality::Low:
		QualityLevel = 0;
		break;
	case EGraphicsQuality::Medium:
		QualityLevel = 1;
		break;
	case EGraphicsQuality::High:
		QualityLevel = 2;
		break;
	case EGraphicsQuality::Ultra:
		QualityLevel = 3;
		break;
	case EGraphicsQuality::Custom:
		// カスタムの場合は何もしない
		return;
	}

	// スケーラビリティ設定を適用
	UserSettings->SetOverallScalabilityLevel(QualityLevel);

	// 個別設定も更新
	CurrentSettings.Graphics.OverallQuality = Quality;
	CurrentSettings.Graphics.ShadowQuality = QualityLevel;
	CurrentSettings.Graphics.PostProcessQuality = QualityLevel;
	CurrentSettings.Graphics.TextureQuality = QualityLevel;
	CurrentSettings.Graphics.AntiAliasingQuality = QualityLevel;
	CurrentSettings.Graphics.EffectsQuality = QualityLevel;

	UserSettings->ApplySettings(false);

	OnSettingsChanged.Broadcast(CurrentSettings);
	SaveSettings();

	UE_LOG(LogDawnlight, Log, TEXT("[UISubsystem] 品質プリセットを適用: %d"), QualityLevel);
}

void UUISubsystem::ApplyGraphicsToEngine(const FDawnlightGraphicsSettings& Settings)
{
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (!UserSettings)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[UISubsystem] GameUserSettingsが見つかりません"));
		return;
	}

	// ウィンドウモード
	UserSettings->SetFullscreenMode(ConvertWindowMode(Settings.WindowMode));

	// 解像度
	UserSettings->SetScreenResolution(Settings.Resolution);

	// VSync
	UserSettings->SetVSyncEnabled(Settings.bVSync);

	// フレームレート制限
	UserSettings->SetFrameRateLimit(Settings.FrameRateLimit > 0 ? Settings.FrameRateLimit : 0.0f);

	// スケーラビリティ設定
	UserSettings->SetShadowQuality(Settings.ShadowQuality);
	UserSettings->SetPostProcessingQuality(Settings.PostProcessQuality);
	UserSettings->SetTextureQuality(Settings.TextureQuality);
	UserSettings->SetAntiAliasingQuality(Settings.AntiAliasingQuality);
	UserSettings->SetVisualEffectQuality(Settings.EffectsQuality);

	// 設定を適用
	UserSettings->ApplySettings(false);
	UserSettings->SaveSettings();

	// ガンマ設定
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->DisplayGamma = Settings.Gamma;
	}
}

void UUISubsystem::ApplyAudioToEngine(const FDawnlightAudioSettings& Settings)
{
	// サウンドクラスを使ったボリューム調整は、
	// プロジェクトのSound Mix/Sound Classの設定に依存するため、
	// Blueprintでの実装を推奨
	// ここではログのみ出力
	UE_LOG(LogDawnlight, Verbose, TEXT("[UISubsystem] オーディオ設定を更新: Master=%.2f"),
		Settings.MasterVolume);

	// TODO: プロジェクトにSound Mixが設定されたら、ここで適用
	// USoundMix/USoundClassを使用してボリュームを調整
}

EWindowMode::Type UUISubsystem::ConvertWindowMode(EWindowModeType Mode) const
{
	switch (Mode)
	{
	case EWindowModeType::Fullscreen:
		return EWindowMode::Fullscreen;
	case EWindowModeType::WindowedFullscreen:
		return EWindowMode::WindowedFullscreen;
	case EWindowModeType::Windowed:
		return EWindowMode::Windowed;
	default:
		return EWindowMode::Fullscreen;
	}
}
