// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#include "BGMSubsystem.h"
#include "Dawnlight.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "UI/UISubsystem.h"

void UBGMSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentTrack = EBGMTrack::None;
	NextTrack = EBGMTrack::None;
	MasterVolume = 1.0f;
	MusicVolume = 1.0f;
	bIsFading = false;
	FadeElapsedTime = 0.0f;
	FadeDuration = 0.0f;
	bIsFadingIn = false;

	DefaultFadeInDuration = 1.0f;
	DefaultFadeOutDuration = 1.0f;
	CrossFadeDuration = 2.0f;

	// BGMアセットをロード
	LoadBGMAssets();

	// UIサブシステムから音量設定を取得
	if (UUISubsystem* UISubsystem = GetGameInstance()->GetSubsystem<UUISubsystem>())
	{
		FDawnlightAudioSettings AudioSettings = UISubsystem->GetAudioSettings();
		MasterVolume = AudioSettings.MasterVolume;
		MusicVolume = AudioSettings.MusicVolume;
	}

	// Tick用タイマーを開始（60FPS）
	GetWorld()->GetTimerManager().SetTimer(
		TickTimerHandle,
		FTimerDelegate::CreateUObject(this, &UBGMSubsystem::Tick, 1.0f / 60.0f),
		1.0f / 60.0f,
		true
	);

	UE_LOG(LogDawnlight, Log, TEXT("[BGMSubsystem] 初期化完了"));
}

void UBGMSubsystem::Deinitialize()
{
	// タイマーをクリア
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TickTimerHandle);
	}

	// BGMを停止
	if (AudioComponent)
	{
		AudioComponent->Stop();
		AudioComponent = nullptr;
	}

	if (NextAudioComponent)
	{
		NextAudioComponent->Stop();
		NextAudioComponent = nullptr;
	}

	LoadedSounds.Empty();

	UE_LOG(LogDawnlight, Log, TEXT("[BGMSubsystem] 終了処理完了"));

	Super::Deinitialize();
}

void UBGMSubsystem::LoadBGMAssets()
{
	// ソフトリファレンスをハードリファレンスに変換してロード
	if (!MainMenuBGM.IsNull())
	{
		USoundBase* Sound = MainMenuBGM.LoadSynchronous();
		if (Sound)
		{
			LoadedSounds.Add(EBGMTrack::MainMenu, Sound);
			UE_LOG(LogDawnlight, Log, TEXT("[BGMSubsystem] メインメニューBGMをロード: %s"), *Sound->GetName());
		}
	}

	if (!NightBGM.IsNull())
	{
		USoundBase* Sound = NightBGM.LoadSynchronous();
		if (Sound)
		{
			LoadedSounds.Add(EBGMTrack::Night, Sound);
			UE_LOG(LogDawnlight, Log, TEXT("[BGMSubsystem] 夜フェーズBGMをロード: %s"), *Sound->GetName());
		}
	}

	if (!DawnBGM.IsNull())
	{
		USoundBase* Sound = DawnBGM.LoadSynchronous();
		if (Sound)
		{
			LoadedSounds.Add(EBGMTrack::Dawn, Sound);
			UE_LOG(LogDawnlight, Log, TEXT("[BGMSubsystem] 夜明けフェーズBGMをロード: %s"), *Sound->GetName());
		}
	}
}

USoundBase* UBGMSubsystem::GetSoundForTrack(EBGMTrack Track) const
{
	const TObjectPtr<USoundBase>* FoundSound = LoadedSounds.Find(Track);
	return FoundSound ? *FoundSound : nullptr;
}

UAudioComponent* UBGMSubsystem::CreateAudioComponent()
{
	if (!GetWorld())
	{
		return nullptr;
	}

	UAudioComponent* NewAudioComponent = NewObject<UAudioComponent>(this, UAudioComponent::StaticClass());
	if (NewAudioComponent)
	{
		NewAudioComponent->bAutoDestroy = false;
		NewAudioComponent->bIsUISound = true;
		NewAudioComponent->RegisterComponent();
	}

	return NewAudioComponent;
}

float UBGMSubsystem::CalculateFinalVolume() const
{
	return MasterVolume * MusicVolume;
}

void UBGMSubsystem::UpdateVolume()
{
	float FinalVolume = CalculateFinalVolume();

	if (AudioComponent && AudioComponent->IsPlaying())
	{
		AudioComponent->SetVolumeMultiplier(FinalVolume);
	}

	if (NextAudioComponent && NextAudioComponent->IsPlaying())
	{
		NextAudioComponent->SetVolumeMultiplier(FinalVolume);
	}
}

void UBGMSubsystem::PlayBGM(EBGMTrack Track, float FadeInDuration)
{
	if (Track == EBGMTrack::None)
	{
		StopBGM(DefaultFadeOutDuration);
		return;
	}

	// 同じトラックが既に再生中の場合は何もしない
	if (Track == CurrentTrack && AudioComponent && AudioComponent->IsPlaying())
	{
		UE_LOG(LogDawnlight, Verbose, TEXT("[BGMSubsystem] 既に同じトラックが再生中: %d"), static_cast<int32>(Track));
		return;
	}

	USoundBase* Sound = GetSoundForTrack(Track);
	if (!Sound)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[BGMSubsystem] BGMが見つかりません: %d"), static_cast<int32>(Track));
		return;
	}

	// 現在のトラックがある場合はクロスフェード
	if (CurrentTrack != EBGMTrack::None && AudioComponent && AudioComponent->IsPlaying())
	{
		// 次のオーディオコンポーネントを作成
		if (NextAudioComponent)
		{
			NextAudioComponent->Stop();
			NextAudioComponent = nullptr;
		}

		NextAudioComponent = CreateAudioComponent();
		if (NextAudioComponent)
		{
			NextAudioComponent->SetSound(Sound);
			NextAudioComponent->SetVolumeMultiplier(0.0f);
			NextAudioComponent->Play();

			NextTrack = Track;
			bIsFading = true;
			bIsFadingIn = true;
			FadeElapsedTime = 0.0f;
			FadeDuration = FadeInDuration > 0.0f ? FadeInDuration : CrossFadeDuration;

			UE_LOG(LogDawnlight, Log, TEXT("[BGMSubsystem] クロスフェード開始: %d -> %d (%.1f秒)"),
				static_cast<int32>(CurrentTrack), static_cast<int32>(Track), FadeDuration);
		}
	}
	else
	{
		// 新規に再生
		if (!AudioComponent)
		{
			AudioComponent = CreateAudioComponent();
		}

		if (AudioComponent)
		{
			AudioComponent->Stop();
			AudioComponent->SetSound(Sound);
			AudioComponent->SetVolumeMultiplier(0.0f);
			AudioComponent->Play();

			CurrentTrack = Track;
			bIsFading = true;
			bIsFadingIn = true;
			FadeElapsedTime = 0.0f;
			FadeDuration = FadeInDuration > 0.0f ? FadeInDuration : DefaultFadeInDuration;

			UE_LOG(LogDawnlight, Log, TEXT("[BGMSubsystem] BGM再生開始: %d (%.1f秒フェードイン)"),
				static_cast<int32>(Track), FadeDuration);
		}
	}
}

void UBGMSubsystem::StopBGM(float FadeOutDuration)
{
	if (!AudioComponent || !AudioComponent->IsPlaying())
	{
		return;
	}

	if (FadeOutDuration > 0.0f)
	{
		bIsFading = true;
		bIsFadingIn = false;
		FadeElapsedTime = 0.0f;
		FadeDuration = FadeOutDuration;

		UE_LOG(LogDawnlight, Log, TEXT("[BGMSubsystem] BGM停止開始: %d (%.1f秒フェードアウト)"),
			static_cast<int32>(CurrentTrack), FadeDuration);
	}
	else
	{
		AudioComponent->Stop();
		CurrentTrack = EBGMTrack::None;

		UE_LOG(LogDawnlight, Log, TEXT("[BGMSubsystem] BGM即座に停止"));
	}
}

void UBGMSubsystem::PauseBGM()
{
	if (AudioComponent && AudioComponent->IsPlaying())
	{
		AudioComponent->SetPaused(true);
		UE_LOG(LogDawnlight, Verbose, TEXT("[BGMSubsystem] BGM一時停止"));
	}
}

void UBGMSubsystem::ResumeBGM()
{
	if (AudioComponent)
	{
		AudioComponent->SetPaused(false);
		UE_LOG(LogDawnlight, Verbose, TEXT("[BGMSubsystem] BGM再開"));
	}
}

bool UBGMSubsystem::IsPlaying() const
{
	return AudioComponent && AudioComponent->IsPlaying();
}

void UBGMSubsystem::SetMasterVolume(float Volume)
{
	MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	UpdateVolume();

	UE_LOG(LogDawnlight, Verbose, TEXT("[BGMSubsystem] マスターボリューム設定: %.2f"), MasterVolume);
}

void UBGMSubsystem::SetMusicVolume(float Volume)
{
	MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	UpdateVolume();

	UE_LOG(LogDawnlight, Verbose, TEXT("[BGMSubsystem] 音楽ボリューム設定: %.2f"), MusicVolume);
}

void UBGMSubsystem::TickFade(float DeltaTime)
{
	if (!bIsFading)
	{
		return;
	}

	FadeElapsedTime += DeltaTime;
	float FadeProgress = FMath::Clamp(FadeElapsedTime / FadeDuration, 0.0f, 1.0f);

	if (bIsFadingIn)
	{
		// フェードイン
		if (NextAudioComponent && NextAudioComponent->IsPlaying())
		{
			// クロスフェード：現在のトラックをフェードアウト、次のトラックをフェードイン
			float CurrentVolume = (1.0f - FadeProgress) * CalculateFinalVolume();
			float NextVolume = FadeProgress * CalculateFinalVolume();

			if (AudioComponent)
			{
				AudioComponent->SetVolumeMultiplier(CurrentVolume);
			}
			NextAudioComponent->SetVolumeMultiplier(NextVolume);

			if (FadeProgress >= 1.0f)
			{
				// クロスフェード完了：古いコンポーネントを停止
				if (AudioComponent)
				{
					AudioComponent->Stop();
				}

				// 次のコンポーネントを現在のコンポーネントに昇格
				AudioComponent = NextAudioComponent;
				NextAudioComponent = nullptr;
				CurrentTrack = NextTrack;
				NextTrack = EBGMTrack::None;
				bIsFading = false;

				UE_LOG(LogDawnlight, Log, TEXT("[BGMSubsystem] クロスフェード完了: %d"), static_cast<int32>(CurrentTrack));
			}
		}
		else
		{
			// 通常のフェードイン
			float Volume = FadeProgress * CalculateFinalVolume();
			if (AudioComponent)
			{
				AudioComponent->SetVolumeMultiplier(Volume);
			}

			if (FadeProgress >= 1.0f)
			{
				bIsFading = false;
				UE_LOG(LogDawnlight, Log, TEXT("[BGMSubsystem] フェードイン完了: %d"), static_cast<int32>(CurrentTrack));
			}
		}
	}
	else
	{
		// フェードアウト
		float Volume = (1.0f - FadeProgress) * CalculateFinalVolume();
		if (AudioComponent)
		{
			AudioComponent->SetVolumeMultiplier(Volume);
		}

		if (FadeProgress >= 1.0f)
		{
			if (AudioComponent)
			{
				AudioComponent->Stop();
			}
			CurrentTrack = EBGMTrack::None;
			bIsFading = false;

			UE_LOG(LogDawnlight, Log, TEXT("[BGMSubsystem] フェードアウト完了"));
		}
	}
}

void UBGMSubsystem::Tick(float DeltaTime)
{
	// フェード処理
	TickFade(DeltaTime);
}
