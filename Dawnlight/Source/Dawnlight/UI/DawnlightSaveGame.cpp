// Copyright Epic Games, Inc. All Rights Reserved.

#include "DawnlightSaveGame.h"

const FString UDawnlightSaveGame::SaveSlotName = TEXT("DawnlightSettings");
const int32 UDawnlightSaveGame::UserIndex = 0;

UDawnlightSaveGame::UDawnlightSaveGame()
	: SettingsVersion(GetCurrentVersion())
	, LastSaveTime(FDateTime::Now())
{
	Settings = GetDefaultSettings();
}

FDawnlightAllSettings UDawnlightSaveGame::GetDefaultSettings()
{
	FDawnlightAllSettings DefaultSettings;

	// オーディオのデフォルト値
	DefaultSettings.Audio.MasterVolume = 1.0f;
	DefaultSettings.Audio.MusicVolume = 0.8f;
	DefaultSettings.Audio.SFXVolume = 1.0f;
	DefaultSettings.Audio.AmbientVolume = 0.7f;
	DefaultSettings.Audio.UIVolume = 0.8f;

	// グラフィックのデフォルト値
	DefaultSettings.Graphics.WindowMode = EWindowModeType::Fullscreen;
	DefaultSettings.Graphics.Resolution = FIntPoint(1920, 1080);
	DefaultSettings.Graphics.bVSync = true;
	DefaultSettings.Graphics.FrameRateLimit = 60;
	DefaultSettings.Graphics.OverallQuality = EGraphicsQuality::High;
	DefaultSettings.Graphics.ShadowQuality = 3;
	DefaultSettings.Graphics.PostProcessQuality = 3;
	DefaultSettings.Graphics.TextureQuality = 3;
	DefaultSettings.Graphics.AntiAliasingQuality = 3;
	DefaultSettings.Graphics.EffectsQuality = 3;
	DefaultSettings.Graphics.Brightness = 0.0f;
	DefaultSettings.Graphics.Gamma = 2.2f;

	// 操作のデフォルト値
	DefaultSettings.Controls.MouseSensitivity = 1.0f;
	DefaultSettings.Controls.bInvertY = false;
	DefaultSettings.Controls.bEnableVibration = true;

	// ゲームプレイのデフォルト値
	DefaultSettings.Gameplay.bShowSubtitles = true;
	DefaultSettings.Gameplay.SubtitleScale = 1.0f;
	DefaultSettings.Gameplay.CameraShakeIntensity = 1.0f;
	DefaultSettings.Gameplay.bShowHints = true;

	return DefaultSettings;
}
