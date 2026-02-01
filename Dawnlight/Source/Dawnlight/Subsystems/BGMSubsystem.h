// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BGMSubsystem.generated.h"

class UAudioComponent;
class USoundBase;

/**
 * BGMトラックタイプ
 */
UENUM(BlueprintType)
enum class EBGMTrack : uint8
{
	None           UMETA(DisplayName = "なし"),
	MainMenu       UMETA(DisplayName = "メインメニュー"),
	Night          UMETA(DisplayName = "夜フェーズ"),
	Dawn           UMETA(DisplayName = "夜明けフェーズ")
};

/**
 * BGM管理サブシステム
 *
 * ゲーム全体のBGM再生を管理
 * - ゲームステート別のBGM再生（MainMenu/Night/Dawn）
 * - トラック間のスムーズな切り替え（クロスフェード）
 * - ボリューム設定の反映
 */
UCLASS()
class DAWNLIGHT_API UBGMSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// ========================================================================
	// サブシステムライフサイクル
	// ========================================================================

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ========================================================================
	// BGM再生制御
	// ========================================================================

	/** 指定したBGMトラックを再生 */
	UFUNCTION(BlueprintCallable, Category = "BGM")
	void PlayBGM(EBGMTrack Track, float FadeInDuration = 1.0f);

	/** 現在のBGMを停止 */
	UFUNCTION(BlueprintCallable, Category = "BGM")
	void StopBGM(float FadeOutDuration = 1.0f);

	/** BGMを一時停止 */
	UFUNCTION(BlueprintCallable, Category = "BGM")
	void PauseBGM();

	/** BGMを再開 */
	UFUNCTION(BlueprintCallable, Category = "BGM")
	void ResumeBGM();

	/** 現在再生中のトラックを取得 */
	UFUNCTION(BlueprintPure, Category = "BGM")
	EBGMTrack GetCurrentTrack() const { return CurrentTrack; }

	/** BGMが再生中かどうか */
	UFUNCTION(BlueprintPure, Category = "BGM")
	bool IsPlaying() const;

	// ========================================================================
	// ボリューム制御
	// ========================================================================

	/** マスターボリュームを設定 (0.0 - 1.0) */
	UFUNCTION(BlueprintCallable, Category = "BGM|ボリューム")
	void SetMasterVolume(float Volume);

	/** 音楽ボリュームを設定 (0.0 - 1.0) */
	UFUNCTION(BlueprintCallable, Category = "BGM|ボリューム")
	void SetMusicVolume(float Volume);

	/** 現在のマスターボリュームを取得 */
	UFUNCTION(BlueprintPure, Category = "BGM|ボリューム")
	float GetMasterVolume() const { return MasterVolume; }

	/** 現在の音楽ボリュームを取得 */
	UFUNCTION(BlueprintPure, Category = "BGM|ボリューム")
	float GetMusicVolume() const { return MusicVolume; }

protected:
	// ========================================================================
	// BGMアセット
	// ========================================================================

	/** メインメニューBGM */
	UPROPERTY(EditDefaultsOnly, Category = "BGM|アセット")
	TSoftObjectPtr<USoundBase> MainMenuBGM;

	/** 夜フェーズBGM */
	UPROPERTY(EditDefaultsOnly, Category = "BGM|アセット")
	TSoftObjectPtr<USoundBase> NightBGM;

	/** 夜明けフェーズBGM */
	UPROPERTY(EditDefaultsOnly, Category = "BGM|アセット")
	TSoftObjectPtr<USoundBase> DawnBGM;

	// ========================================================================
	// フェード設定
	// ========================================================================

	/** デフォルトフェードイン時間（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "BGM|フェード")
	float DefaultFadeInDuration;

	/** デフォルトフェードアウト時間（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "BGM|フェード")
	float DefaultFadeOutDuration;

	/** クロスフェード時間（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "BGM|フェード")
	float CrossFadeDuration;

private:
	// ========================================================================
	// 内部処理
	// ========================================================================

	/** BGMアセットをロード */
	void LoadBGMAssets();

	/** 指定したトラックのサウンドアセットを取得 */
	USoundBase* GetSoundForTrack(EBGMTrack Track) const;

	/** オーディオコンポーネントを作成 */
	UAudioComponent* CreateAudioComponent();

	/** 最終的なボリュームを計算（Master * Music） */
	float CalculateFinalVolume() const;

	/** ボリュームを更新 */
	void UpdateVolume();

	/** フェード処理をTick */
	void TickFade(float DeltaTime);

	/** 内部Tick処理 */
	void Tick(float DeltaTime);

	// ========================================================================
	// 状態
	// ========================================================================

	/** 現在再生中のトラック */
	UPROPERTY(Transient)
	EBGMTrack CurrentTrack;

	/** 次に再生するトラック（クロスフェード中） */
	UPROPERTY(Transient)
	EBGMTrack NextTrack;

	/** メインオーディオコンポーネント */
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> AudioComponent;

	/** クロスフェード用の次のオーディオコンポーネント */
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> NextAudioComponent;

	/** マスターボリューム (0.0 - 1.0) */
	float MasterVolume;

	/** 音楽ボリューム (0.0 - 1.0) */
	float MusicVolume;

	/** フェード中かどうか */
	bool bIsFading;

	/** フェード経過時間 */
	float FadeElapsedTime;

	/** フェード持続時間 */
	float FadeDuration;

	/** フェードイン中かどうか */
	bool bIsFadingIn;

	/** Tick用タイマーハンドル */
	FTimerHandle TickTimerHandle;

	/** ロード済みのサウンドアセット */
	UPROPERTY(Transient)
	TMap<EBGMTrack, TObjectPtr<USoundBase>> LoadedSounds;
};
