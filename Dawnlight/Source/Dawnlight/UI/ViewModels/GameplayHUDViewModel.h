// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ViewModelBase.h"
#include "Core/DawnlightGameMode.h"
#include "Subsystems/SoulCollectionSubsystem.h"
#include "GameplayHUDViewModel.generated.h"

class ADawnlightGameMode;
class USoulCollectionSubsystem;
class UAnimalSpawnerSubsystem;
class UWaveSpawnerSubsystem;
class AEnemyCharacter;

/**
 * Gameplay HUD用のViewModel
 *
 * HUDに表示されるデータを管理し、プロパティ変更通知を提供
 * - フェーズ情報
 * - リーパーゲージ
 * - 魂カウント
 * - 時間表示
 * - Wave情報
 * - プレイヤーHP
 */
UCLASS(BlueprintType)
class DAWNLIGHT_API UGameplayHUDViewModel : public UViewModelBase
{
	GENERATED_BODY()

public:
	UGameplayHUDViewModel();

	// ========================================================================
	// ライフサイクル
	// ========================================================================

	virtual void Initialize(UWorld* InWorld) override;
	virtual void Deinitialize() override;

	// ========================================================================
	// 公開プロパティ（読み取り専用）
	// ========================================================================

	/** 現在のゲームフェーズ */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|フェーズ")
	EGamePhase CurrentPhase;

	/** フェーズ名（表示用テキスト） */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|フェーズ")
	FText PhaseName;

	/** Night Phase残り時間（秒） */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|時間")
	float NightTimeRemaining;

	/** フォーマットされた残り時間（"MM:SS"形式） */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|時間")
	FText FormattedTimeRemaining;

	/** 総魂数 */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|魂")
	int32 TotalSoulCount;

	/** リーパーゲージ（0.0〜1.0） */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|リーパー")
	float ReaperGaugePercent;

	/** リーパーモードが発動可能か */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|リーパー")
	bool bIsReaperModeReady;

	/** リーパーモードが発動中か */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|リーパー")
	bool bIsReaperModeActive;

	/** 現在のWave番号 */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Wave")
	int32 CurrentWaveNumber;

	/** 総Wave数 */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Wave")
	int32 TotalWaveCount;

	/** 残り敵数 */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Wave")
	int32 RemainingEnemies;

	/** 生存中の動物数（Night Phase） */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|動物")
	int32 AliveAnimalCount;

	/** 総動物数（Night Phase） */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|動物")
	int32 TotalAnimalCount;

	/** プレイヤー現在HP */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|HP")
	float PlayerCurrentHP;

	/** プレイヤー最大HP */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|HP")
	float PlayerMaxHP;

	/** プレイヤーHPパーセント（0.0〜1.0） */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|HP")
	float PlayerHPPercent;

	/** ダメージバフパーセント */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|バフ")
	float DamageBuffPercent;

	/** 夜明け警告を表示すべきか */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|警告")
	bool bShouldShowDawnWarning;

	// ========================================================================
	// プロパティ名定数（バインディング用）
	// ========================================================================

	static const FName PROP_CurrentPhase;
	static const FName PROP_PhaseName;
	static const FName PROP_NightTimeRemaining;
	static const FName PROP_FormattedTimeRemaining;
	static const FName PROP_TotalSoulCount;
	static const FName PROP_ReaperGaugePercent;
	static const FName PROP_IsReaperModeReady;
	static const FName PROP_IsReaperModeActive;
	static const FName PROP_CurrentWaveNumber;
	static const FName PROP_TotalWaveCount;
	static const FName PROP_RemainingEnemies;
	static const FName PROP_AliveAnimalCount;
	static const FName PROP_TotalAnimalCount;
	static const FName PROP_PlayerCurrentHP;
	static const FName PROP_PlayerMaxHP;
	static const FName PROP_PlayerHPPercent;
	static const FName PROP_DamageBuffPercent;
	static const FName PROP_ShouldShowDawnWarning;

	// ========================================================================
	// 追加デリゲート（特定イベント用）
	// ========================================================================

	/** Wave開始時（アナウンスメント表示用） */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStartedEvent, int32, WaveNumber);
	UPROPERTY(BlueprintAssignable, Category = "HUD|イベント")
	FOnWaveStartedEvent OnWaveStartedEvent;

	/** リーパーモード状態変更時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReaperModeChanged, bool, bIsActive);
	UPROPERTY(BlueprintAssignable, Category = "HUD|イベント")
	FOnReaperModeChanged OnReaperModeChanged;

	/** プレイヤーダメージ受けた時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerDamaged, float, DamageAmount, float, RemainingHP);
	UPROPERTY(BlueprintAssignable, Category = "HUD|イベント")
	FOnPlayerDamaged OnPlayerDamaged;

	// ========================================================================
	// 公開関数
	// ========================================================================

	/**
	 * プレイヤーHPを更新
	 * キャラクターから直接呼び出し可能
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdatePlayerHealth(float CurrentHP, float MaxHP);

	/**
	 * リーパーゲージを更新
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateReaperGauge(float NormalizedValue);

	/**
	 * ダメージバフを更新
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateDamageBuff(float BuffPercent);

	/**
	 * リーパーモード状態を更新
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetReaperModeActive(bool bActive);

protected:
	// ========================================================================
	// イベントハンドラ
	// ========================================================================

	/** フェーズ変更時のハンドラ */
	UFUNCTION()
	void HandlePhaseChanged(EGamePhase OldPhase, EGamePhase NewPhase);

	/** 魂収集時のハンドラ */
	UFUNCTION()
	void HandleSoulCollected(const FSoulCollectedEventData& EventData);

	/** Wave開始時のハンドラ */
	UFUNCTION()
	void HandleWaveStarted(int32 WaveNumber);

	/** Wave完了時のハンドラ */
	UFUNCTION()
	void HandleWaveCompleted(int32 WaveNumber);

	/** 敵撃破時のハンドラ */
	UFUNCTION()
	void HandleEnemyKilled(AEnemyCharacter* Enemy);

private:
	// ========================================================================
	// サブシステム参照
	// ========================================================================

	UPROPERTY()
	TWeakObjectPtr<ADawnlightGameMode> GameMode;

	UPROPERTY()
	TWeakObjectPtr<USoulCollectionSubsystem> SoulSubsystem;

	UPROPERTY()
	TWeakObjectPtr<UAnimalSpawnerSubsystem> AnimalSubsystem;

	UPROPERTY()
	TWeakObjectPtr<UWaveSpawnerSubsystem> WaveSubsystem;

	// ========================================================================
	// 内部関数
	// ========================================================================

	/** サブシステムへのイベントバインド */
	void BindToSubsystems();

	/** サブシステムからのイベントアンバインド */
	void UnbindFromSubsystems();

	/** 時間をMM:SS形式にフォーマット */
	FText FormatTime(float Seconds) const;

	/** フェーズ名を取得 */
	FText GetPhaseDisplayName(EGamePhase Phase) const;

	/** ゲームモードからデータを同期（Tick代替） */
	void SyncFromGameMode();

	/** 動物情報を同期 */
	void SyncAnimalInfo();

	/** Wave情報を同期 */
	void SyncWaveInfo();

	// ========================================================================
	// 定数
	// ========================================================================

	/** 夜明け警告を表示する残り秒数 */
	static constexpr float DAWN_WARNING_THRESHOLD = 30.0f;
};
